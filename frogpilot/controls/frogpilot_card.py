#!/usr/bin/env python3
from cereal import car
from openpilot.common.params import Params

ButtonType = car.CarState.ButtonEvent.Type

class FrogPilotCard:
  def __init__(self, CP, FPCP):
    self.CP = CP

    self.params = Params()
    self.params_memory = Params(memory=True)

  def update(self, carState, frogpilotCarState, sm):
    return frogpilotCarState
