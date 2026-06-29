# Protocol

## DMX hodnota

- interní hodnoty Fixture jsou `0..127`,
- DMX hodnota = MIDI hodnota × 2,
- výsledný rozsah DMX je `0..254`.

## MIDI RAW vrstva

Příchozí kompletní MIDI zpráva typu `status data1 data2` se v RAW režimu převádí takto:

- `data1` = DMX kanál `0..127`,
- `data2` = MIDI hodnota `0..127`,
- DMX hodnota = `data2 * 2`.

RAW vstup je použit pro:

- MIDI DIN IN u ESP32 WROOM,
- USB MIDI IN u ESP32-S3.
