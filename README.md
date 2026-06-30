# SMC-MIDI2DMX

Open-source BLE MIDI → DMX controller based on ESP32.

## Features

- BLE MIDI Client (M-VAVE SMC-MIXER)
- MIDI DIN IN (ESP32-WROOM)
- USB MIDI (ESP32-S3 – work in progress)
- DMX512 OUT
- Scene memory
- Multiple fixture maps

## Supported hardware

| Platform | Status |
|----------|--------|
| ESP32-WROOM | Reference platform |
| ESP32-S3 | Experimental |

## Repository structure

Core/
Shared code.

SMC-MIDI2DMX_ESP32_WROOM/
Reference hardware.

SMC-MIDI2DMX_ESP32_S3/
USB capable hardware.

Docs/
Project documentation.

## Current release

v04_02 (development)

See:

- Docs/ChangeLog.md
- Docs/Audit_v04_02.md