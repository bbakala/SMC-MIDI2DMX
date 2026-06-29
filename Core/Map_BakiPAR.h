#pragma once
#include "FixtureTypes.h"

// -----------------------------------------------------------------------------
// Map_BakiPAR.h
// Samostatná záložka podle listu BakiPAR z MAP.xlsx.
// DMX kanál = MIDI CC číslo. DMX hodnota = MIDI hodnota * 2.
// -----------------------------------------------------------------------------

const DmxMapItem dmxMapBakiPAR[] =
{
    {   1,  0, VAL_DIMMER   },   // PAR01 0 dimmer
    {   2,  0, VAL_RED      },   // PAR01 1 red
    {   3,  0, VAL_GREEN    },   // PAR01 2 green
    {   4,  0, VAL_BLUE     },   // PAR01 3 blue
    {   5,  0, VAL_STROBE   },   // PAR01 4 strobe
    {   6,  0, VAL_EFFECT   },   // PAR01 5 effect
    {   7,  0, VAL_SPEED    },   // PAR01 6 speed
    {  11,  1, VAL_DIMMER   },   // PAR02 0 dimmer
    {  12,  1, VAL_RED      },   // PAR02 1 red
    {  13,  1, VAL_GREEN    },   // PAR02 2 green
    {  14,  1, VAL_BLUE     },   // PAR02 3 blue
    {  15,  1, VAL_STROBE   },   // PAR02 4 strobe
    {  16,  1, VAL_EFFECT   },   // PAR02 5 effect
    {  17,  1, VAL_SPEED    },   // PAR02 6 speed
    {  21,  2, VAL_DIMMER   },   // PAR03 0 dimmer
    {  22,  2, VAL_RED      },   // PAR03 1 red
    {  23,  2, VAL_GREEN    },   // PAR03 2 green
    {  24,  2, VAL_BLUE     },   // PAR03 3 blue
    {  25,  2, VAL_STROBE   },   // PAR03 4 strobe
    {  26,  2, VAL_EFFECT   },   // PAR03 5 effect
    {  27,  2, VAL_SPEED    },   // PAR03 6 speed
    {  31,  3, VAL_DIMMER   },   // PAR04 0 dimmer
    {  32,  3, VAL_RED      },   // PAR04 1 red
    {  33,  3, VAL_GREEN    },   // PAR04 2 green
    {  34,  3, VAL_BLUE     },   // PAR04 3 blue
    {  35,  3, VAL_STROBE   },   // PAR04 4 strobe
    {  36,  3, VAL_EFFECT   },   // PAR04 5 effect
    {  37,  3, VAL_SPEED    },   // PAR04 6 speed
};

const uint8_t dmxMapBakiPARCount = sizeof(dmxMapBakiPAR) / sizeof(dmxMapBakiPAR[0]);
const uint8_t dmxMapBakiPARMaxCc = 37;
