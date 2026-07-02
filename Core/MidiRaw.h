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