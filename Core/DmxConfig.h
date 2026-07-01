#pragma once

#include <Arduino.h>
#include "Config.h"

// DMX512
constexpr uint32_t DMX_BAUD = 250000;

// DMX buffer obsahuje start code + 128 logických kanálů.
constexpr uint16_t DMX_BUFFER_SIZE = DMX_LOGICAL_COUNT + 1;

// DMX start code
constexpr uint8_t DMX_START_CODE = 0x00;

inline uint8_t midiValueToDmx(uint8_t value)
{
    return value * 2;
}

inline void clearDmxBuffer(uint8_t* buffer, size_t size)
{
    memset(buffer, 0, size);
    buffer[0] = DMX_START_CODE;
}

inline bool isValidDmxLogicalChannel(uint8_t channel)
{
    return channel < DMX_LOGICAL_COUNT;
}

