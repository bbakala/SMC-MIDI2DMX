# Development Environment

This document describes the verified and recommended development environment for the SMC-MIDI2DMX project.

---

## Recommended environment

Current verified development environment:

| Component | Version |
|---|---:|
| Arduino IDE | 2.3.10 |
| ESP32 board package | 3.3.10 |
| NimBLE-Arduino | 2.5.0 |
| Adafruit NeoPixel | Current Library Manager version |
| Git for Windows | 2.55.0.windows.2 |

This environment has been verified with the ESP32-S3 platform.

The following functions were tested successfully:

- project compilation,
- firmware upload,
- BLE connection to the M-VAVE SMC-MIXER,
- BLE MIDI communication in DAW mode,
- RGB status LED,
- USB MIDI.

---

## Previous verified environment

The original reference environment remains compatible:

| Component | Version |
|---|---:|
| Arduino IDE | 2.3.8 |
| ESP32 board package | 3.3.6 |
| NimBLE-Arduino | 2.4.0 |

Both environments are considered compatible with the current project.

For new installations, use the recommended environment listed above.

---

## Important SMC-MIXER setting

The M-VAVE SMC-MIXER must be configured in:
DAW mode