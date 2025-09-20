from cereal import car
from opendbc.can.packer import CANPacker
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.car import apply_meas_steer_torque_limits
from openpilot.selfdrive.car.chrysler import chryslercan
from openpilot.selfdrive.car.chrysler.values import RAM_CARS, RAM_DT, CarControllerParams, ChryslerFlags, CAR
from openpilot.selfdrive.car.chrysler.values import JEEPS as CHRYSLER_JEEPS
from openpilot.selfdrive.car.interfaces import CarControllerBase


class CarController(CarControllerBase):
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.apply_steer_last = 0
    self.frame = 0

    self.hud_count = 0
    self.last_lkas_falling_edge = 0
    self.lkas_control_bit_prev = False
    self.last_button_frame = 0

    self.packer = CANPacker(dbc_name)
    self.params = CarControllerParams(CP)

    # Brake hold (Jeep SNG workaround)
    self.bh_recent_acc_enabled = False
    self.bh_hold_active = False

  def update(self, CC, CS, now_nanos, frogpilot_toggles):
    can_sends = []

    lkas_active = CC.latActive and self.lkas_control_bit_prev

    # cruise buttons
    if (self.frame - self.last_button_frame)*DT_CTRL > 0.05:
      das_bus = 2 if self.CP.carFingerprint in RAM_CARS else 0

      # ACC cancellation
      if CC.cruiseControl.cancel:
        self.last_button_frame = self.frame
        can_sends.append(chryslercan.create_cruise_buttons(self.packer, CS.button_counter + 1, das_bus, CS.button_message, cancel=True))

      # ACC resume from standstill
      elif CC.cruiseControl.resume:
        self.last_button_frame = self.frame
        can_sends.append(chryslercan.create_cruise_buttons(self.packer, CS.button_counter + 1, das_bus, CS.button_message, resume=True))

    # HUD alerts
    if self.frame % 25 == 0:
      if CS.lkas_car_model != -1:
        can_sends.append(chryslercan.create_lkas_hud(self.packer, self.CP, lkas_active, CC.hudControl.visualAlert,
                                                     self.hud_count, CS.lkas_car_model, CS.auto_high_beam, CC.latActive,
                                                     CC.hudControl.leftLaneVisible, CC.hudControl.rightLaneVisible, CC.hudControl.leftLaneClose,
                                                     CC.hudControl.rightLaneClose, CC.hudControl.leftLaneDepart, CC.hudControl.rightLaneDepart))
        self.hud_count += 1

    # steering
    if self.frame % self.params.STEER_STEP == 0:

      # TODO: can we make this more sane? why is it different for all the cars?
      lkas_control_bit = self.lkas_control_bit_prev
      if self.CP.carFingerprint in RAM_DT:
        if self.CP.minEnableSpeed <= CS.out.vEgo <= self.CP.minEnableSpeed + 0.5:
          lkas_control_bit = True
        if (self.CP.minEnableSpeed >= 14.5) and (CS.out.gearShifter != 2):
          lkas_control_bit = False
      elif CS.out.vEgo > self.CP.minSteerSpeed:
        lkas_control_bit = True
      elif self.CP.flags & ChryslerFlags.HIGHER_MIN_STEERING_SPEED:
        if CS.out.vEgo < (self.CP.minSteerSpeed - 3.0):
          lkas_control_bit = False
      elif self.CP.carFingerprint in RAM_CARS:
        if CS.out.vEgo < (self.CP.minSteerSpeed - 0.5):
          lkas_control_bit = False

      # EPS faults if LKAS re-enables too quickly
      lkas_control_bit = lkas_control_bit and (self.frame - self.last_lkas_falling_edge > 200)

      if not lkas_control_bit and self.lkas_control_bit_prev:
        self.last_lkas_falling_edge = self.frame
      self.lkas_control_bit_prev = lkas_control_bit

      # steer torque
      new_steer = int(round(CC.actuators.steer * self.params.STEER_MAX))
      apply_steer = apply_meas_steer_torque_limits(new_steer, self.apply_steer_last, CS.out.steeringTorqueEps, self.params)
      if not lkas_active or not lkas_control_bit:
        apply_steer = 0
      self.apply_steer_last = apply_steer

      can_sends.append(chryslercan.create_lkas_command(self.packer, self.CP, int(apply_steer), lkas_control_bit))

    # Jeep Brake Hold: keep standstill asserted (and optional small brake) after SNG auto-cancel
    # This mirrors the Phase 2 plan without introducing general OP-long; only acts at standstill, gear in drive.
    try:
      is_jeep = (self.CP.carFingerprint in CHRYSLER_JEEPS)
    except Exception:
      is_jeep = False

    if is_jeep and getattr(frogpilot_toggles, 'jeep_brake_hold', False):
      # Track when ACC was enabled at standstill
      if CS.cruiseState.enabled and CS.cruiseState.standstill:
        self.bh_recent_acc_enabled = True

      # Arm hold when ACC falls to disabled while still at standstill (SNG timeout)
      if self.bh_recent_acc_enabled and (not CS.cruiseState.enabled) and CS.out.standstill and CS.out.gearShifter == car.CarState.GearShifter.drive and not CS.out.brakePressed:
        self.bh_hold_active = True

      # Disarm when ACC re-enables, vehicle moves, driver presses brake, or gear not drive
      if CS.cruiseState.enabled or not CS.out.standstill or CS.out.brakePressed or CS.out.gearShifter != car.CarState.GearShifter.drive:
        self.bh_hold_active = False
        if CS.cruiseState.enabled:
          self.bh_recent_acc_enabled = False

      # While active, assert ACC_STANDSTILL; send at ~25 Hz (aligned with HUD updates)
      if self.bh_hold_active and (self.frame % 4 == 0) and getattr(CS, 'das_3', None):
        msg = chryslercan.create_das_3_brake_hold(self.packer, CS.das_3, set_standstill=True)
        if msg is not None:
          can_sends.append(msg)

    self.frame += 1

    new_actuators = CC.actuators.as_builder()
    new_actuators.steer = self.apply_steer_last / self.params.STEER_MAX
    new_actuators.steerOutputCan = self.apply_steer_last

    return new_actuators, can_sends
