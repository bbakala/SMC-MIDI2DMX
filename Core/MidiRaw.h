#pragma once

#include <Arduino.h>

#include "DmxConfig.h"
#include "FixtureTypes.h"
#include "MapSelector.h"

inline void clearMidiRawAllCommon(
    bool* midiRawValid,
    uint8_t* midiRawDmxValue,
    size_t count
)
{
    memset(midiRawValid, 0, count * sizeof(bool));
    memset(midiRawDmxValue, 0, count * sizeof(uint8_t));
}

inline void clearMidiRawForCCCommon(
    bool* midiRawValid,
    uint8_t* midiRawDmxValue,
    uint8_t cc
)
{
    if(isValidDmxLogicalChannel(cc))
    {
        midiRawValid[cc] = false;
        midiRawDmxValue[cc] = 0;
    }
}

using MidiRawClearCC = void (*)(uint8_t cc);

inline void clearMidiRawForFixtureValueCommon(
    uint8_t fixtureIdx,
    uint8_t valueIndex,
    uint8_t mapId,
    MidiRawClearCC clearCC
)
{
    if(fixtureIdx >= FIXTURE_COUNT || valueIndex >= FIXTURE_VALUE_COUNT)
        return;

    const DmxMapItem* map = activeDmxMap(mapId);
    uint8_t count = activeDmxMapCount(mapId);

    for(uint8_t i = 0; i < count; i++)
    {
        const DmxMapItem& item = map[i];

        if(item.fixture == fixtureIdx && item.valueIndex == valueIndex)
            clearCC(item.cc);
    }
}

