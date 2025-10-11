from cereal import car
from opendbc.can.packer import CANPacker
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.car import apply_meas_steer_torque_limits
from openpilot.selfdrive.car.chrysler import chryslercan
from openpilot.selfdrive.car.chrysler.values import RAM_CARS, RAM_DT, CarControllerParams, ChryslerFlags, CAR
from openpilot.selfdrive.car.chrysler.values import JEEPS as CHRYSLER_JEEPS
from openpilot.selfdrive.car.interfaces import CarControllerBase
from openpilot.common.swaglog import cloudlog


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

    # Brake hold (Jeep SNG workaround) - matching jvePilot implementation
    self.brake_hold_decel = -2.0  # Default brake decel value like jvePilot
    self.last_das_3_counter = -1
    self.brake_hold_enabled = False  # Will be set in update based on frogpilot_toggles
    self.bh_recent_acc_enabled = False  # Track if ACC was recently enabled at standstill
    self.bh_hold_active = False  # Track if brake hold is currently active

  def update(self, CC, CS, now_nanos, frogpilot_toggles):
    can_sends = []

    lkas_active = CC.latActive and self.lkas_control_bit_prev

    # cruise buttons
    if (self.frame - self.last_button_frame)*DT_CTRL > 0.05:
      das_bus = 2 if self.CP.carFingerprint in RAM_CARS else 0

      # ACC cancellation (disabled for brake hold compatibility - matches jvePilot)
      # if CC.cruiseControl.cancel:
      #   self.last_button_frame = self.frame
      #   can_sends.append(chryslercan.create_cruise_buttons(self.packer, CS.button_counter + 1, das_bus, CS.button_message, cancel=True))

      # ACC resume from standstill (disabled for brake hold compatibility - matches jvePilot)
      # elif CC.cruiseControl.resume:
      #   self.last_button_frame = self.frame
      #   can_sends.append(chryslercan.create_cruise_buttons(self.packer, CS.button_counter + 1, das_bus, CS.button_message, resume=True))

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
    # This mirrors the jvePilot implementation for proper brake hold functionality
    try:
      is_jeep = (self.CP.carFingerprint in CHRYSLER_JEEPS)
    except Exception:
      is_jeep = False

    if is_jeep and getattr(frogpilot_toggles, 'jeep_brake_hold', False):
      # Set brake_hold_enabled flag like jvePilot
      self.brake_hold_enabled = True
      self.brake_hold(CC, CS, can_sends)
    else:
      self.brake_hold_enabled = False

    self.frame += 1

    new_actuators = CC.actuators.as_builder()
    new_actuators.steer = self.apply_steer_last / self.params.STEER_MAX
    new_actuators.steerOutputCan = self.apply_steer_last

    return new_actuators, can_sends

  def brake_hold(self, CC, CS, can_sends):
    # Track when ACC was enabled at standstill (matching jvePilot)
    if CS.out.cruiseState.enabled and CS.out.standstill:
      if not self.bh_recent_acc_enabled:
        cloudlog.info(f"Brake hold: ACC enabled at standstill - arming")
      self.bh_recent_acc_enabled = True

    # Arm hold when ACC falls to disabled while still at standstill (SNG timeout) (matching jvePilot)
    if (self.bh_recent_acc_enabled and
        not CS.cruise_active_actual and
        CS.out.standstill and
        CS.forward_gear and
        not CS.out.brakePressed):
      if not self.bh_hold_active:
        cloudlog.info(f"Brake hold: ACTIVATING - ACC disabled at standstill")
      self.bh_hold_active = True

    # Debug logging for activation conditions (log every 50 frames to avoid spam)
    if self.bh_recent_acc_enabled and not self.bh_hold_active and self.frame % 50 == 0:
      cloudlog.info(f"Brake hold: Not activating - cruise_actual={CS.cruise_active_actual}, standstill={CS.out.standstill}, fwd_gear={CS.forward_gear}, brake={CS.out.brakePressed}")

    # Disarm when ACC re-enables, vehicle moves, driver presses brake/gas, or gear not drive (matching jvePilot)
    if (CS.out.cruiseState.enabled or
        not CS.out.standstill or
        CS.out.brakePressed or
        CS.out.gasPressed or
        not CS.forward_gear):
      if self.bh_hold_active:
        cloudlog.info(f"Brake hold: DEACTIVATING - cruise={CS.out.cruiseState.enabled}, standstill={CS.out.standstill}, brake={CS.out.brakePressed}, gas={CS.out.gasPressed}, gear={CS.forward_gear}")
      self.bh_hold_active = False
      if CS.out.cruiseState.enabled:
        self.bh_recent_acc_enabled = False

    # While active, request brake decel with counter offsets like jvePilot; send ~50 Hz (matching jvePilot)
    if self.bh_hold_active and (self.frame % 2 == 0) and CS.das_3:
      das_bus = 0  # Jeep/Pacifica on bus 0

      # Track incoming counter to compute offset (matching jvePilot)
      counter_changed = (CS.das_3.get('COUNTER') != self.last_das_3_counter)
      self.last_das_3_counter = CS.das_3.get('COUNTER')
      counter_offset = 2 if counter_changed else 3

      # Track decel like jvePilot
      if CS.out.cruiseState.enabled:
        self.brake_hold_decel = min(self.brake_hold_decel, CS.das_3.get('ACC_DECEL', -2.0)) if CS.out.standstill else -2.0
      else:
        self.brake_hold_decel = self.brake_hold_decel if CS.out.standstill else -2.0

      # Send brake hold message with proper parameters (matching jvePilot exactly)
      msg = chryslercan.das_3_command(self.packer, counter_offset,
                                      False,  # go
                                      False,  # torque_req
                                      None,   # torque
                                      2,      # max_gear
                                      False,  # stop (standstill)
                                      self.brake_hold_decel,  # brake
                                      False,  # brake_prep
                                      CS.das_3)
      can_sends.append(msg)

      # Log every 50 frames (~1 second) to avoid spam
      if self.frame % 50 == 0:
        cloudlog.info(f"Brake hold: Sending DAS_3 - decel={self.brake_hold_decel}, counter_offset={counter_offset}")
