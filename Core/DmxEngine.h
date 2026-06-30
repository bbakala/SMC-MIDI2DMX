#pragma once

#include <Arduino.h>
#include "DmxConfig.h"
#include "FixtureTypes.h"

inline void applyMapItemToDmxBuffer(
    uint8_t* dmxBuffer,
    const DmxMapItem& item,
    uint8_t value
)
{
    uint8_t addr = item.cc;

    if(!isValidDmxLogicalChannel(addr))
        return;

    if(addr == 0)
        return;

    dmxBuffer[addr] = midiValueToDmx(value);
}