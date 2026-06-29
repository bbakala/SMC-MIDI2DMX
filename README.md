# SMC-MIDI2DMX

Jeden skutečný zdrojový strom projektu. Verze je uvedena pouze v `Core/Version.h` a v `Docs/ChangeLog.md`.

## Verze

`04_01`

## Struktura

```text
SMC-MIDI2DMX/
├── Core/
├── SMC-MIDI2DMX_ESP32_S3/
├── SMC-MIDI2DMX_ESP32_WROOM/
└── Docs/
```

## Arduino sketche

Otevírej vždy příslušnou složku sketchu:

- `SMC-MIDI2DMX_ESP32_S3/SMC-MIDI2DMX_ESP32_S3.ino`
- `SMC-MIDI2DMX_ESP32_WROOM/SMC-MIDI2DMX_ESP32_WROOM.ino`

Obě HW varianty sdílejí stejné `Core/`.

## MAP pořadí

- MAP0 = PFT01
- MAP1 = PFT02
- MAP2 = BakiPAR
- MAP3 = PFT01 alias / rezerva
