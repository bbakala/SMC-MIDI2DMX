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

using DmxValueResolver = uint8_t (*)(uint8_t fixtureIdx, uint8_t valueIndex);

inline void refreshDmxBuffer(
    uint8_t* dmxBuffer,
    size_t dmxBufferSize,
    const DmxMapItem* map,
    uint8_t mapCount,
    DmxValueResolver resolveValue,
    const bool* midiRawValid,
    const uint8_t* midiRawDmxValue,
    bool midiRawEnabled
)
{
    clearDmxBuffer(dmxBuffer, dmxBufferSize);

    for(uint8_t i = 0; i < mapCount; i++)
    {
        const DmxMapItem& item = map[i];
        uint8_t value = resolveValue(item.fixture, item.valueIndex);

        applyMapItemToDmxBuffer(dmxBuffer, item, value);
    }

    if(midiRawEnabled && midiRawValid != nullptr && midiRawDmxValue != nullptr)
    {
        applyMidiRawToDmxBuffer(dmxBuffer, midiRawValid, midiRawDmxValue);
    }
}

