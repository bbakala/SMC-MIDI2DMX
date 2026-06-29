#pragma once

#include <Arduino.h>

//====================================================
// SMC-MIDI2DMX
// Debug output abstraction
//====================================================

// 0 = debug output disabled
// 1 = debug output enabled
#ifndef DEBUG_ENABLE
#define DEBUG_ENABLE 0
#endif

#if DEBUG_ENABLE

#define DBG_PRINT(x)       Serial.print(x)
#define DBG_PRINTLN(x)     Serial.println(x)
#define DBG_PRINTF(...)    Serial.printf(__VA_ARGS__)

#else

#define DBG_PRINT(x)
#define DBG_PRINTLN(x)
#define DBG_PRINTF(...)

#endif