#pragma once

#include <Arduino.h>

#include "DmxConfig.h"
#include "FixtureTypes.h"
#include "MapSelector.h"

struct MidiRawState
{
    bool valid[DMX_LOGICAL_COUNT];
    uint8_t dmxValue[DMX_LOGICAL_COUNT];
};

inline void clearMidiRawAllCommon(MidiRawState& raw)
{
    memset(raw.valid, 0, sizeof(raw.valid));
    memset(raw.dmxValue, 0, sizeof(raw.dmxValue));
}

inline void clearMidiRawForCCCommon(MidiRawState& raw, uint8_t cc)
{
    if(isValidDmxLogicalChannel(cc))
    {
        raw.valid[cc] = false;
        raw.dmxValue[cc] = 0;
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

