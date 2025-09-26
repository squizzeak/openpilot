# Claude Code Configuration - Jeep Brake Hold Implementation

## Project Context

This repository contains a FrogPilot fork with Jeep Brake Hold functionality that was not working properly. The issue was identified by comparing with the working jvePilot/green branch implementation.

## Recent Work Completed

### Problem Analysis
- **Issue**: Jeep Brake Hold feature was implemented but not functioning properly
- **Root Cause**: Missing CarState variables and incorrect activation logic compared to jvePilot
- **Investigation**: Compared current implementation with jvePilot/green branch to identify differences

### Key Fixes Implemented (Commit: acae4d48e7)

#### 1. Added Missing CarState Variables
**File**: `selfdrive/car/chrysler/carstate.py`
- Added `self.brake_hold = False` - Direct brake hold state tracking
- Added `self.cruise_active_actual = False` - Actual cruise state (vs commanded)
- Added `self.acc_decelerating = False` - ACC deceleration detection
- Added `self.forward_gear = False` - Drive gear state
- Added `self.prev_acc_decel = 0.0` - Previous ACC decel for comparison

#### 2. Enhanced State Tracking Logic
**File**: `selfdrive/car/chrysler/carstate.py` (lines 96-105)
```python
# Additional brake hold state tracking (matching jvePilot)
self.cruise_active_actual = ret.cruiseState.enabled
self.forward_gear = ret.gearShifter == car.CarState.GearShifter.drive

# Track ACC deceleration for brake hold activation
current_acc_decel = cp_cruise.vl["DAS_3"].get("ACC_DECEL", 0.0)
self.acc_decelerating = (self.cruise_active_actual and
                        current_acc_decel < self.prev_acc_decel - 0.1 and
                        ret.cruiseState.standstill)
self.prev_acc_decel = current_acc_decel
```

#### 3. Fixed Brake Hold Activation Logic
**File**: `selfdrive/car/chrysler/carcontroller.py` (lines 106-117)
- **Activation**: Now triggers when `cruise_active_actual AND acc_decelerating AND standstill`
- **Deactivation**: Comprehensive exit conditions matching jvePilot behavior
- Uses new CarState variables instead of just standard openpilot states

### Technical Details

#### CarState Variable Usage Patterns
- **`CS.out.*`** - Data serialized via Cap'n Proto for inter-process messaging
- **`CS.*`** - Internal state variables used within single process iteration
- Our implementation correctly uses internal variables for brake hold state management

#### Panda Safety Configuration
- Verified DAS_3 (0x1F4) message is in TX allowlist for Jeep/Pacifica platforms
- Uses `das_3_command()` function matching jvePilot implementation
- Bus routing: Jeep/Pacifica uses bus 0 (vs RAM trucks on bus 2)

#### jvePilot Compatibility
- Matches jvePilot's brake hold logic patterns
- Uses same counter offset logic (2 if changed, 3 if unchanged)
- Maintains compatible deceleration tracking (-2.0 default)

## Current Branch Status

- **Branch**: `feature/brake-hold`
- **Latest Commit**: `acae4d48e7 - Fix Jeep Brake Hold: implement jvePilot-compatible logic`
- **Status**: Implementation complete, ready for testing

## Files Modified

1. `selfdrive/car/chrysler/carstate.py` - Added brake hold state variables and tracking
2. `selfdrive/car/chrysler/carcontroller.py` - Updated brake hold activation/deactivation logic

## Testing Notes

The brake hold should now:
- ✅ Activate when ACC is decelerating and comes to complete stop
- ✅ Maintain brake pressure after SNG timeout (3-second rule)
- ✅ Deactivate when driver intervenes (brake, gas, gear change)
- ✅ Work with existing FrogPilot toggle (`jeep_brake_hold`)

## Next Steps

1. **Test Implementation**: Verify brake hold functions in vehicle
2. **Monitor Logs**: Check for any DAS_3 message errors or timing issues
3. **Validate Edge Cases**: Test deactivation scenarios (brake press, gas press, gear changes)
4. **Consider PR**: If working properly, consider merging to main FrogPilot branch

## Key Learnings

- jvePilot uses additional CarState variables not present in standard openpilot
- Brake hold activation requires ACC deceleration detection, not just standstill
- Proper state management is critical for reliable brake hold operation
- FrogPilot integration maintains existing toggle compatibility

## Related Files for Reference

- `panda/board/safety/safety_chrysler.h` - Safety configuration and TX allowlists
- `selfdrive/car/chrysler/chryslercan.py` - DAS_3 message creation functions
- `selfdrive/car/chrysler/values.py` - Jeep vehicle definitions

---

*Last updated: Session ending after brake hold implementation fixes*