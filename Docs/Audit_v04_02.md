# Audit v04_02

**Verze:** v04_02 (develop)

Účel:

Stabilizace společného Core a platformy ESP32-WROOM před vydáním
release v04_02.

---

# Etapa 1 – Core

| Modul | Stav |
|--------|------|
| Version | 🟢 |
| Config | 🟢 |
| TargetPlatform | 🟢 |
| Debug | 🟢 |
| FixtureTypes | 🟢 |
| MAP | 🟢 |
| ChangeLog | 🟢 |

Poznámka:

Moduly označené 🟢 jsou pro verzi v04_02 považovány za stabilní.
Další změny pouze při opravě nalezené chyby.

---

# Etapa 2 – ESP32-WROOM

## DEBUG_SERIAL

Stav: 🟢

Režim:

```cpp
#define DEBUG_SERIAL 0
```

- UART0 = MIDI DIN IN (31250 Bd)
- Debug vypnut

Režim:

```cpp
#define DEBUG_SERIAL 1
```

- UART0 = Serial konzole (115200 Bd)
- MIDI DIN IN vypnut

Poznámky:

- UART0 nemůže současně sloužit jako MIDI DIN IN i Serial konzole.
- Toto je platformně specifické chování ESP32-WROOM.
- Lokální DBG_* makra jsou pro v04_02 ponechána.

---

## BLE

Stav: 🟢

Ověřeno.

Výsledky:

- ✅ Automatické připojení k SMC-MIXER.
- ✅ BLE LED signalizace.
- ✅ Přenos MIDI přes BLE.
- ✅ LED feedback do SMC-MIXER.
- ✅ Automatický reconnect.
- ✅ Dlouhodobý provoz stabilní.

Poznámka:

BLE vrstva je pro referenční platformu ESP32-WROOM považována za stabilní pro vydání v04_02.

---

## MIDI DIN

Stav: 🟢

Ověřeno.

Výsledky:

- ✅ Příjem MIDI DIN přes UART0 (31250 Bd).
- ✅ MIDI RAW parser.
- ✅ Control Change.
- ✅ Note On / Note Off.
- ✅ Pitch Bend.
- ✅ Integrace s BLE řízením.
- ✅ Integrace s DMX mapováním.

Poznámky:

- DEBUG_SERIAL = 0:
  - UART0 slouží jako MIDI DIN IN.
- DEBUG_SERIAL = 1:
  - UART0 slouží jako Serial konzole.
  - MIDI DIN IN je vypnutý.

MIDI DIN vrstva je pro referenční platformu ESP32-WROOM považována za stabilní pro vydání v04_02.

---

## Scene

Stav: 🟢

Ověřeno.

Výsledky:

- ✅ SAVE scény ukládá aktuální stav `fixture[]`.
- ✅ LOAD scény obnoví uložený stav `fixture[]`.
- ✅ CLEAR vynuluje aktuální stav `fixture[]`.
- ✅ Po LOAD / CLEAR zůstává chování MIDI RAW vrstvy podle pravidel v04_02.
- ✅ Aktivní scéna je ukládána v Preferences.
- ✅ Po restartu se obnoví poslední aktivní scéna.
- ✅ BLE LED feedback po SAVE / LOAD / CLEAR odpovídá stavu programu.

Poznámka:

Scene logika je pro referenční platformu ESP32-WROOM považována za stabilní pro vydání v04_02.

---

## MAP

Stav: 🟢

Ověřeno.

Výsledky:

- ✅ MAP0 = PFT01.
- ✅ MAP1 = PFT02.
- ✅ MAP2 = BakiPAR.
- ✅ MAP3 = PFT01 alias / rezerva.
- ✅ Přepínání MAP0..3 funguje.
- ✅ Aktivní MAP je ukládána v Preferences.
- ✅ Po restartu se obnoví poslední aktivní MAP.
- ✅ MAP používá společné tabulky v Core.
- ✅ DMX i MIDI RAW pracují se stejným logickým kanálem 0..127.

Poznámka:

MAP modul je pro referenční platformu ESP32-WROOM považován za stabilní pro vydání v04_02.
---

## DMX

Stav: 🟢

Ověřeno.

### DMX Engine

- ✅ Obnovovací perioda 25 ms.
- ✅ Výstup 0..254.
- ✅ Break + Mark After Break generovány správně.
- ✅ Odesílá se kompletní DMX rámec.

### Mapování

- ✅ DMX kanál = logický kanál MAP tabulky.
- ✅ DMX hodnota = 2 × MIDI hodnota (0..127 → 0..254).
- ✅ Všechny výpočty vycházejí z `fixture[]`.

### Dirty update

- ✅ Změna hodnoty označí příslušný logický kanál jako změněný.
- ✅ USB MIDI CC OUT odesílá pouze změněné hodnoty.
- ✅ DMX rámec vždy obsahuje kompletní aktuální stav.

### Priorita zdrojů

- ✅ BLE SMC-Mixer mění `fixture[]`.
- ✅ MIDI DIN RAW může přepsat hodnoty podle MAP.
- ✅ DMX vždy vychází z aktuálního logického stavu.

### Stabilita

- ✅ Nevznikají nekonzistentní hodnoty.
- ✅ DMX výstup odpovídá aktivní MAP.

Poznámka:

DMX Engine je pro referenční platformu ESP32-WROOM považován za stabilní pro vydání v04_02.

---

# ESP32-S3

Platforma není součástí release v04_02.

Požadavky:

- Sketch musí být kompilovatelný.
- Platform-specific vývoj bude pokračovat po vydání v04_02.

---

# Release checklist

- [x] Core stabilní
- [x] DEBUG_SERIAL ověřen
- [x] BLE ověřeno
- [x] MIDI DIN ověřeno
- [x] Scene ověřeny
- [x] MAP ověřeny
- [x] DMX ověřeno
- [ ] Dokumentace aktualizována
- [ ] CHANGELOG aktualizován
- [ ] Tag v04_02 vytvořen