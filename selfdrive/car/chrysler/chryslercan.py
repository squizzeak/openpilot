from cereal import car
from openpilot.selfdrive.car.chrysler.values import ChryslerFlags, RAM_CARS

GearShifter = car.CarState.GearShifter
VisualAlert = car.CarControl.HUDControl.VisualAlert

def create_lkas_hud(packer, CP, lkas_active, hud_alert, hud_count, car_model, auto_high_beam, lat_active,
                    left_lane_visible, right_lane_visible, left_lane_close, right_lane_close, left_lane_depart, right_lane_depart):
  # LKAS_HUD - Controls what lane-keeping icon is displayed

  # == Color ==
  # 0 hidden?
  # 1 white
  # 2 green
  # 3 ldw

  # == Lines ==
  #  0x01 (transparent lines): 1
  #  0x02 (left white): 2
  #  0x03 (right white): 3
  #  0x04 (left yellow with car on top): 4
  #  0x05 (right yellow with car on top): 5
  #  0x06 (both white): 6
  #  0x07 (left yellow): 7
  #  0x08 (left yellow right white): 8
  #  0x09 (right yellow): 9
  #  0x0a (right yellow left white): 10
  #  0x0b (left yellow with car on top right white): 11
  #  0x0c (right yellow with car on top left white): 12

  # == Alerts ==
  # 0 Normal
  # 1 LaneSense On
  # 2 LaneSense Off
  # 6 place hands on wheel
  # 7 lane departure place hands on wheel
  # 9 lane sense unavailable + clean windshield
  # 10 lane sense unavailable + service required
  # 11 lane sense unavailable + auto high beam unavailable + clean windshield
  # 12 lane sense unavailable + service required + auto high beam unavailable

  color, lines, alerts = 0, 0, 0

  if hud_alert == VisualAlert.ldw:
    color = 3
    alerts = 7
  elif hud_alert == VisualAlert.steerRequired:
    color = 1
    alerts = 6
  elif lkas_active:
    color = 2
    lines = 6
    if left_lane_visible and right_lane_visible:
      lines = 6
    elif left_lane_depart and right_lane_visible:
      lines = 11
    elif left_lane_depart:
      lines = 4
    elif right_lane_depart and left_lane_visible:
      lines = 12
    elif right_lane_depart:
      lines = 5
    elif left_lane_close and right_lane_visible:
      lines = 8
    elif left_lane_close:
      lines = 7
    elif right_lane_close and left_lane_visible:
      lines = 10
    elif right_lane_close:
      lines = 9
    elif left_lane_visible:
      lines = 2
    elif right_lane_visible:
      lines = 3
  elif lat_active:
    color = 1
    lines = 1
    if hud_count < (1 * 10):
      alerts = 1

  values = {
    "LKAS_ICON_COLOR": color,
    "CAR_MODEL": car_model,
    "LKAS_LANE_LINES": lines,
    "LKAS_ALERTS": alerts,
  }

  if CP.carFingerprint in RAM_CARS:
    values['AUTO_HIGH_BEAM_ON'] = auto_high_beam
    values['LKAS_DISABLED'] = 0 if lat_active else 1

  return packer.make_can_msg("DAS_6", 0, values)


def create_lkas_command(packer, CP, apply_steer, lkas_control_bit):
  # LKAS_COMMAND Lane-keeping signal to turn the wheel
  enabled_val = 2 if CP.carFingerprint in RAM_CARS else 1
  values = {
    "STEERING_TORQUE": apply_steer,
    "LKAS_CONTROL_BIT": enabled_val if lkas_control_bit else 0,
  }
  return packer.make_can_msg("LKAS_COMMAND", 0, values)

def create_lkas_heartbit(packer, lkas_enabled, lkas_heartbit):
  # LKAS_HEARTBIT (697) LKAS heartbeat
  values = lkas_heartbit.copy()  # forward what we parsed
  values["LKAS_DISABLED"] = 0 if lkas_enabled else 1
  return packer.make_can_msg("LKAS_HEARTBIT", 0, values)

def create_cruise_buttons(packer, CP, frame, bus, cancel=False, resume=False):
  values = {
    "ACC_Cancel": cancel,
    "ACC_Resume": resume,
    "COUNTER": frame % 0x10,
  }
  button_message = "CRUISE_BUTTONS_ALT" if CP.flags & ChryslerFlags.RAM_HD_ALT_BUTTONS else "CRUISE_BUTTONS"
  return packer.make_can_msg(button_message, bus, values)
