#pragma once

#include <Arduino.h>

//====================================================
// SMC-MIDI2DMX
// Shared project configuration
//====================================================

constexpr uint8_t FIXTURE_COUNT      = 16;
constexpr uint8_t FIXTURES_PER_BANK  = 8;
constexpr uint8_t SCENE_COUNT        = 4;

constexpr uint8_t MIDI_MIN_VALUE     = 0;
constexpr uint8_t MIDI_MAX_VALUE     = 127;

constexpr uint8_t DMX_MIN_VALUE      = 0;
constexpr uint8_t DMX_MAX_VALUE      = 254;

constexpr uint8_t MIDI_CC_COUNT      = 128;
constexpr uint8_t DMX_LOGICAL_COUNT  = 128;

constexpr uint16_t DMX_FRAME_INTERVAL_MS = 25;