#pragma once

#include "FixtureTypes.h"

// -----------------------------------------------------------------------------
// Map_PFT02.h
// Samostatná záložka MAP 0 podle listu PFT02 z MAP.xlsx.
// DMX kanál = MIDI CC číslo. DMX hodnota = MIDI hodnota * 2.
// -----------------------------------------------------------------------------

const DmxMapItem dmxMapPFT02[] =
{
    {  1, 0, VAL_DIMMER }, {  2, 0, VAL_RED },    {  3, 0, VAL_GREEN },
    {  4, 0, VAL_BLUE },   {  5, 0, VAL_WHITE },  {  6, 0, VAL_STROBE },
    {  7, 0, VAL_EFFECT }, {  8, 0, VAL_SPEED },

    {  9, 1, VAL_DIMMER }, { 10, 1, VAL_RED },    { 11, 1, VAL_GREEN },
    { 12, 1, VAL_BLUE },   { 13, 1, VAL_WHITE },  { 14, 1, VAL_STROBE },
    { 15, 1, VAL_EFFECT }, { 16, 1, VAL_SPEED },

    { 17, 2, VAL_DIMMER }, { 18, 2, VAL_RED },    { 19, 2, VAL_GREEN },
    { 20, 2, VAL_BLUE },   { 21, 2, VAL_EFFECT }, { 22, 2, VAL_STROBE },

    { 23, 3, VAL_DIMMER }, { 24, 3, VAL_RED },    { 25, 3, VAL_GREEN },
    { 26, 3, VAL_BLUE },   { 27, 3, VAL_EFFECT }, { 28, 3, VAL_STROBE },

    { 29, 4, VAL_DIMMER }, { 30, 4, VAL_RED },    { 31, 4, VAL_GREEN },
    { 32, 4, VAL_BLUE },   { 33, 4, VAL_WHITE },  { 34, 4, VAL_STROBE },
    { 35, 4, VAL_EFFECT }, { 36, 4, VAL_SPEED },

    { 37, 5, VAL_DIMMER }, { 38, 5, VAL_RED },    { 39, 5, VAL_GREEN },
    { 40, 5, VAL_BLUE },   { 41, 5, VAL_WHITE },  { 42, 5, VAL_STROBE },
    { 43, 5, VAL_EFFECT }, { 44, 5, VAL_SPEED },

    { 45, 6, VAL_RED },      // Spot01 Horizont -> encoder RED
    { 46, 6, VAL_GREEN },    // Spot01 Vertikal -> encoder GREEN
    { 47, 6, VAL_SPEED },    // Spot01 Speed
    { 48, 6, VAL_DIMMER },   // Spot01 Dimmer
    { 49, 6, VAL_STROBE },   // Spot01 Strobe
    { 50, 6, VAL_BLUE },     // Spot01 Colour -> encoder BLUE
    { 51, 6, VAL_EFFECT },   // Spot01 Pattern -> encoder EFFECT
    { 52, 6, VAL_WHITE },    // Spot01 Prism -> encoder WHITE
    { 53, 6, VAL_CUSTOM },   // Spot01 ColorEff -> encoder CUSTOM
    { 54, 6, VAL_SELFWALK },   // Spot01 SelfWalk
    { 55, 6, VAL_UNUSED },   // Spot01 Reset
    { 56, 6, VAL_EXTRA },   // Spot01 Strip

    { 57, 7, VAL_RED },      // Spot02 Horizont -> encoder RED
    { 58, 7, VAL_GREEN },    // Spot02 Vertikal -> encoder GREEN
    { 59, 7, VAL_SPEED },    // Spot02 Speed
    { 60, 7, VAL_DIMMER },   // Spot02 Dimmer
    { 61, 7, VAL_STROBE },   // Spot02 Strobe
    { 62, 7, VAL_BLUE },     // Spot02 Colour -> encoder BLUE
    { 63, 7, VAL_EFFECT },   // Spot02 Pattern -> encoder EFFECT
    { 64, 7, VAL_WHITE },    // Spot02 Prism -> encoder WHITE
    { 65, 7, VAL_CUSTOM },   // Spot02 ColorEff -> encoder CUSTOM
    { 66, 7, VAL_SELFWALK },   // Spot02 SelfWalk
    { 67, 7, VAL_UNUSED },   // Spot02 Reset
    { 68, 7, VAL_EXTRA },   // Spot02 Strip

    { 69, 8, VAL_CUSTOM },   // lista01 Yosa
    { 70, 8, VAL_SPEED },    // lista01 Speed
    { 71, 8, VAL_DIMMER },   // lista01 Dimmer
    { 72, 8, VAL_STROBE },   // lista01 Strobe
    { 73, 8, VAL_RED },      // lista01 Red
    { 74, 8, VAL_GREEN },    // lista01 Green
    { 75, 8, VAL_BLUE },     // lista01 Blue
    { 76, 8, VAL_WHITE },    // lista01 White
    { 77, 8, VAL_EFFECT },   // lista01 Mode
    { 78, 8, VAL_EXTRA },   // lista01 Speed 2 / doplňkový parametr
    { 79, 8, VAL_COLOUR },   // lista01 BackColor
    { 80, 8, VAL_PATTERN },   // lista01 BackTone
    { 81, 8, VAL_SELFWALK }    // lista01 Reset
};

const uint8_t dmxMapPFT02Count = sizeof(dmxMapPFT02) / sizeof(dmxMapPFT02[0]);

