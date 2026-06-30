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

inline void applyMidiRawToDmxBuffer(
    uint8_t* dmxBuffer,
    const bool* midiRawValid,
    const uint8_t* midiRawDmxValue
)
{
    for(uint8_t addr = 1; addr < DMX_LOGICAL_COUNT; addr++)
    {
        if(midiRawValid[addr])
            dmxBuffer[addr] = midiRawDmxValue[addr];
    }
}

