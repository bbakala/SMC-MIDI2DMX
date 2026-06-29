#pragma once

#include <Arduino.h>
#include "Config.h"

// -----------------------------------------------------------------------------
// FixtureTypes.h
// Shared fixture types for SMC-MIDI2DMX
// -----------------------------------------------------------------------------

struct Fixture
{
    bool selected;
    bool R;
    bool S;
    bool M;

    // Všechny hodnoty jsou v rozsahu MIDI 0..127.
    // Význam jednotlivých položek určuje aktivní MAP.
    uint8_t value[FIXTURE_VALUE_COUNT];
};

// Každý interní parametr má svůj symbolický název.
// Poznámka:
// VAL_CUSTOM a VAL_HORIZONT zatím sdílejí stejný index.
// Do budoucna je možné fyzické ovládací prvky zcela oddělit
// od interních parametrů zařízení.
enum ValueIndex : uint8_t
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

    // Aliasové názvy pro jiná zařízení
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

inline constexpr const char* valueName[FIXTURE_VALUE_COUNT] =
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
    uint8_t cc;          // MIDI CC 0..127 = logický DMX kanál
    uint8_t fixture;     // 0..FIXTURE_COUNT-1
    uint8_t valueIndex;  // ValueIndex nebo VAL_UNUSED
};