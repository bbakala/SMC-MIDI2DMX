#pragma once

// -----------------------------------------------------------------------------
// FixtureTypes.h
// Společné typy pro obecný DMX/MIDI engine verze _07.
// -----------------------------------------------------------------------------

#define NUM_STRIPS 8
#define NUM_FIXTURES 16
#define NUM_VALUES 16

struct Fixture
{
    bool selected;
    bool R;
    bool S;
    bool M;

    // Všechny hodnoty jsou MIDI rozsah 0..127.
    // Význam value[] neurčuje struct, ale MAP tabulka.
    uint8_t value[NUM_VALUES];
};

// Každý interní parametr má svůj symbolický název.
// Poznámka: _07 zatím zachovává kompatibilitu _06a:
// VAL_CUSTOM a VAL_HORIZONT sdílejí value[8].
// V další etapě je možné oddělit fyzický encoder -> parametr zařízení úplně samostatně.
enum ValueIndex
{
    VAL_DIMMER   = 0,
    VAL_RED      = 1,
    VAL_GREEN    = 2,
    VAL_BLUE     = 3,
    VAL_WHITE    = 4,
    VAL_STROBE   = 5,
    VAL_EFFECT   = 6,
    VAL_SPEED    = 7,
    VAL_CUSTOM   = 8,

    // Aliasové názvy pro zařízení, která nejsou RGBW.
    VAL_HORIZONT = 8,
    VAL_VERTIKAL = 9,
    VAL_COLOUR   = 10,
    VAL_PATTERN  = 11,
    VAL_PRISM    = 12,
    VAL_COLOREFF = 13,
    VAL_SELFWALK = 14,
    VAL_EXTRA    = 15,

    VAL_UNUSED   = 255
};

static const char* valueName[NUM_VALUES] =
{
    "dimmer",
    "red/horiz/custom",
    "green/vertikal",
    "blue",
    "white",
    "strobe",
    "effect/mode",
    "speed",
    "custom/horizont",
    "vertikal",
    "colour",
    "pattern",
    "prism",
    "colorEff",
    "selfWalk",
    "extra"
};

struct DmxMapItem
{
    uint8_t cc;          // USB MIDI CC číslo 0..127 a současně DMX logický kanál 0..127
    uint8_t fixture;     // index fixture 0..15
    uint8_t valueIndex;  // value[] index 0..15 nebo VAL_UNUSED
};
