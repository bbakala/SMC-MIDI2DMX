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

Stav: 🟡

Čeká na ověření.

---

## MIDI DIN

Stav: 🟡

Čeká na ověření.

---

## Scene

Stav: 🟡

Čeká na ověření.

---

## MAP

Stav: 🟡

Čeká na ověření.

---

## DMX

Stav: 🟡

Čeká na ověření.

---

# ESP32-S3

Platforma není součástí release v04_02.

Požadavky:

- Sketch musí být kompilovatelný.
- Platform-specific vývoj bude pokračovat po vydání v04_02.

---

# Release checklist

- [ ] Core stabilní
- [ ] DEBUG_SERIAL ověřen
- [ ] BLE ověřeno
- [ ] MIDI DIN ověřeno
- [ ] Scene ověřeny
- [ ] MAP ověřeny
- [ ] DMX ověřeno
- [ ] Dokumentace aktualizována
- [ ] CHANGELOG aktualizován
- [ ] Tag v04_02 vytvořen