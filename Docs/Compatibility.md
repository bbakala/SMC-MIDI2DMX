# Compatibility

Verified development environments for SMC-MIDI2DMX.

| Arduino IDE | ESP32 Core | NimBLE-Arduino | Status | Notes |
|--------------|------------|----------------|--------|-------|
| 2.3.8 | 3.3.6 | 2.4.0 | ✅ PASS | Original reference environment |
| 2.3.10 | 3.3.6 | 2.4.0 | ✅ PASS | Verified |
| 2.3.10 | 3.3.10 | 2.4.0 | ✅ PASS | Verified |
| 2.3.10 | 3.3.10 | 2.5.0 | ✅ PASS | Current development environment |

## Notes

During validation, BLE communication initially appeared to fail.

The root cause was identified as the SMC-MIXER not being in DAW mode.

No compatibility issue was found between:

- Arduino IDE 2.3.10
- ESP32 Core 3.3.10
- NimBLE-Arduino 2.5.0

This is now the recommended development environment.