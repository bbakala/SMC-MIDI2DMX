# SMC-MIDI2DMX ESP32 WROOM 04_01

Hardware větev pro ESP32 WROOM bez nativního USB MIDI.

Funkce:

- BLE MIDI Client pro M-VAVE SMC-MIXER,
- MIDI DIN IN přes UART0 / RX GPIO3,
- DMX OUT,
- stavová BLE LED GPIO4.

`DEBUG_SERIAL`:

- `0` = provoz, UART0 přijímá MIDI DIN IN na 31250 Bd,
- `1` = ladění, UART0 je Serial konzole 115200 Bd a MIDI DIN IN je vypnutý.

Hlavní sketch:

`SMC-MIDI2DMX_ESP32_WROOM.ino`
