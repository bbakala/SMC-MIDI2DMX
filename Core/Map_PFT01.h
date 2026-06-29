#pragma once
#include "FixtureTypes.h"

// -----------------------------------------------------------------------------
// Map_PFT01.h
// Samostatná záložka podle listu PFT01 z MAP.xlsx.
// DMX kanál = MIDI CC číslo. DMX hodnota = MIDI hodnota * 2.
// -----------------------------------------------------------------------------

const DmxMapItem dmxMapPFT01[] =
{
    {   1,  0, VAL_DIMMER   },   // PAR01 0 dimmer
    {   2,  0, VAL_RED      },   // PAR01 1 red
    {   3,  0, VAL_GREEN    },   // PAR01 2 green
    {   4,  0, VAL_BLUE     },   // PAR01 3 blue
    {   5,  0, VAL_WHITE    },   // PAR01 4 white
    {   6,  0, VAL_STROBE   },   // PAR01 5 strobe
    {   7,  0, VAL_EFFECT   },   // PAR01 6 effect
    {   8,  0, VAL_SPEED    },   // PAR01 7 speed
    {  11,  1, VAL_DIMMER   },   // PAR02 0 dimmer
    {  12,  1, VAL_RED      },   // PAR02 1 red
    {  13,  1, VAL_GREEN    },   // PAR02 2 green
    {  14,  1, VAL_BLUE     },   // PAR02 3 blue
    {  15,  1, VAL_WHITE    },   // PAR02 4 white
    {  16,  1, VAL_STROBE   },   // PAR02 5 strobe
    {  17,  1, VAL_EFFECT   },   // PAR02 6 effect
    {  18,  1, VAL_SPEED    },   // PAR02 7 speed
    {  21,  2, VAL_DIMMER   },   // Vana01 0 dimmer
    {  22,  2, VAL_RED      },   // Vana01 1 red
    {  23,  2, VAL_GREEN    },   // Vana01 2 green
    {  24,  2, VAL_BLUE     },   // Vana01 3 blue
    {  25,  2, VAL_WHITE    },   // Vana01 4 white
    {  26,  2, VAL_STROBE   },   // Vana01 5 strobe
    {  27,  2, VAL_EFFECT   },   // Vana01 6 effect
    {  28,  2, VAL_SPEED    },   // Vana01 7 speed
    {  31,  3, VAL_DIMMER   },   // Vana02 0 dimmer
    {  32,  3, VAL_RED      },   // Vana02 1 red
    {  33,  3, VAL_GREEN    },   // Vana02 2 green
    {  34,  3, VAL_BLUE     },   // Vana02 3 blue
    {  35,  3, VAL_WHITE    },   // Vana02 4 white
    {  36,  3, VAL_STROBE   },   // Vana02 5 strobe
    {  37,  3, VAL_EFFECT   },   // Vana02 6 effect
    {  38,  3, VAL_SPEED    },   // Vana02 7 speed
    {  41,  4, VAL_DIMMER   },   // PAR03 0 dimmer
    {  42,  4, VAL_RED      },   // PAR03 1 red
    {  43,  4, VAL_GREEN    },   // PAR03 2 green
    {  44,  4, VAL_BLUE     },   // PAR03 3 blue
    {  45,  4, VAL_WHITE    },   // PAR03 4 white
    {  46,  4, VAL_STROBE   },   // PAR03 5 strobe
    {  47,  4, VAL_EFFECT   },   // PAR03 6 effect
    {  48,  4, VAL_SPEED    },   // PAR03 7 speed
    {  51,  5, VAL_DIMMER   },   // PAR04 0 dimmer
    {  52,  5, VAL_RED      },   // PAR04 1 red
    {  53,  5, VAL_GREEN    },   // PAR04 2 green
    {  54,  5, VAL_BLUE     },   // PAR04 3 blue
    {  55,  5, VAL_WHITE    },   // PAR04 4 white
    {  56,  5, VAL_STROBE   },   // PAR04 5 strobe
    {  57,  5, VAL_EFFECT   },   // PAR04 6 effect
    {  58,  5, VAL_SPEED    },   // PAR04 7 speed
    {  61,  6, VAL_RED      },   // Spot01 Horizont
    {  62,  6, VAL_GREEN    },   // Spot01 Vertikal
    {  63,  6, VAL_SPEED    },   // Spot01 Speed
    {  64,  6, VAL_DIMMER   },   // Spot01 Dimmer
    {  65,  6, VAL_STROBE   },   // Spot01 Strobe
    {  66,  6, VAL_BLUE     },   // Spot01 Colour
    {  67,  6, VAL_EFFECT   },   // Spot01 Pattern
    {  68,  6, VAL_WHITE    },   // Spot01 Prism
    {  69,  6, VAL_CUSTOM   },   // Spot01 ColorEff
    {  70,  6, VAL_SELFWALK },   // Spot01 SelfWalk
    {  71,  6, VAL_UNUSED   },   // Spot01 Reset
    {  72,  6, VAL_EXTRA    },   // Spot01 Strip
    {  73,  7, VAL_RED      },   // Spot02 Horizont
    {  74,  7, VAL_GREEN    },   // Spot02 Vertikal
    {  75,  7, VAL_SPEED    },   // Spot02 Speed
    {  76,  7, VAL_DIMMER   },   // Spot02 Dimmer
    {  77,  7, VAL_STROBE   },   // Spot02 Strobe
    {  78,  7, VAL_BLUE     },   // Spot02 Colour
    {  79,  7, VAL_EFFECT   },   // Spot02 Pattern
    {  80,  7, VAL_WHITE    },   // Spot02 Prism
    {  81,  7, VAL_CUSTOM   },   // Spot02 ColorEff
    {  82,  7, VAL_SELFWALK },   // Spot02 SelfWalk
    {  83,  7, VAL_UNUSED   },   // Spot02 Reset
    {  84,  7, VAL_EXTRA    },   // Spot02 Strip
};

const uint8_t dmxMapPFT01Count = sizeof(dmxMapPFT01) / sizeof(dmxMapPFT01[0]);
const uint8_t dmxMapPFT01MaxCc = 84;
