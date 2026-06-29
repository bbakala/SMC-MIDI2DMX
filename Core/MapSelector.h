#pragma once

#include "FixtureTypes.h"
#include "Map_PFT01.h"
#include "Map_PFT02.h"
#include "Map_BakiPAR.h"

// -----------------------------------------------------------------------------
// MapSelector.h - společný pro všechny HW varianty
//
// MAP0 = PFT01
// MAP1 = PFT02
// MAP2 = BakiPAR
// MAP3 = PFT01 alias / rezerva
// -----------------------------------------------------------------------------

inline const DmxMapItem* activeDmxMap(uint8_t mapIndex)
{
    switch(mapIndex & 0x03)
    {
        case 0: return dmxMapPFT01;
        case 1: return dmxMapPFT02;
        case 2: return dmxMapBakiPAR;
        case 3: return dmxMapPFT01; // alias / rezerva
    }

    return dmxMapPFT01;
}

inline uint8_t activeDmxMapCount(uint8_t mapIndex)
{
    switch(mapIndex & 0x03)
    {
        case 0: return dmxMapPFT01Count;
        case 1: return dmxMapPFT02Count;
        case 2: return dmxMapBakiPARCount;
        case 3: return dmxMapPFT01Count;
    }

    return dmxMapPFT01Count;
}

inline const char* activeDmxMapName(uint8_t mapIndex)
{
    switch(mapIndex & 0x03)
    {
        case 0: return "PFT01";
        case 1: return "PFT02";
        case 2: return "BakiPAR";
        case 3: return "PFT01 alias";
    }

    return "PFT01";
}
