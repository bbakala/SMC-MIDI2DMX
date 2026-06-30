# SMC-MIDI2DMX ChangeLog

## 04_01

Revize verze 04_00.

### Opravy a úpravy

- opraveny relativní `#include` cesty po přesunu hlavních sketchů do Arduino podsložek,
- doplněn `Core/Version.h`,
- přejmenována kořenová složka a hlavní sketche podle nové konvence `vv_rr`,
- aktualizovány README soubory a release notes,
- zachováno sjednocené pořadí map pro obě HW větve.

### Pořadí MAP

- MAP0 = `Map_PFT01.h`
- MAP1 = `Map_PFT02.h`
- MAP2 = `Map_BakiPAR.h`
- MAP3 = alias `Map_PFT01.h` / rezerva

---

## 04_00

První oficiální společná verze projektu pod názvem **SMC-MIDI2DMX**.

### Architektura

- nová struktura projektu:
  - `Core/`
  - `HW_ESP32_S3/`
  - `HW_ESP32_WROOM/`
  - `Docs/`
- sjednocené pořadí map pro obě HW větve,
- společný `MapSelector.h` přesunut do `Core/`,
- názvy sketchů obsahují verzi projektu.

### SMC-Mixer BLE

- BLE MIDI Client pro M-VAVE SMC-MIXER v DAW mode,
- výběr fixture 0..15 přes R-bank,
- `S` aktivuje/deaktivuje Strobe hodnotu,
- `M` aktivuje/deaktivuje Effect/Mode hodnotu,
- Save / Load / Clear / Scene 0..3,
- LED feedback do M-VAVE přes queue.

### DMX

- DMX OUT pro obě HW varianty,
- DMX kanál = MIDI CC / `data1`,
- DMX hodnota = MIDI hodnota × 2,
- `fixture[]` vrstva + MIDI RAW vrstva.

### MIDI RAW vrstva

- `midiRaw[]` drží poslední hodnoty ze sequenceru,
- RAW má prioritu jen na kanálech, kde je aktivní,
- ruční změna stejného kanálu ze SMC zruší RAW pro tento kanál,
- další MIDI zpráva na stejném kanálu RAW znovu aktivuje,
- `CLEAR` a `LOAD` nemažou MIDI RAW vrstvu.

### ESP32-S3

- BLE MIDI Client,
- USB MIDI OUT z `fixture[]`,
- USB MIDI IN jako RAW vstup do DMX vrstvy,
- RGB LED na GPIO48,
- DMX OUT na GPIO17.

### ESP32 WROOM

- BLE MIDI Client,
- MIDI DIN IN přes UART0 / RX GPIO3,
- DEBUG_SERIAL režim:
  - `0` = MIDI DIN IN 31250 Bd,
  - `1` = Serial debug 115200 Bd, MIDI DIN IN vypnutý,
- stavová LED BLE na GPIO4,
- DMX OUT na GPIO17.


04_01 struktura zdrojového stromu
---------------------------------
- projekt uvnitř archivu má stabilní kořen `SMC-MIDI2DMX/`,
- číslo verze je jen v `Core/Version.h` a dokumentaci,
- odstraněny mezisložky `HW_ESP32_*/SMC-MIDI2DMX_*_04_01/`,
- Arduino sketche jsou přímo ve složkách `SMC-MIDI2DMX_ESP32_S3/` a `SMC-MIDI2DMX_ESP32_WROOM/`,
- relativní cesty `#include` změněny na `../Core/...`.

## v04_02 – develop

### Scope
- Stabilizace společného Core.
- Stabilizace ESP32-WROOM jako referenční platformy.
- ESP32-S3 ponechána kompilovatelná, platform-specific testy odloženy.

### Core

- Version.h stabilizováno.
- Config.h stabilizováno.
- TargetPlatform.h stabilizováno.
- Debug.h stabilizováno.
- FixtureTypes.h stabilizováno.
- MAP modul stabilizován.
