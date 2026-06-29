#pragma once

//====================================================
// SMC-MIDI2DMX
// Target platform detection
//====================================================

#if defined(CONFIG_IDF_TARGET_ESP32S3)

inline constexpr bool TARGET_ESP32_S3 = true;
inline constexpr bool TARGET_ESP32    = false;

#else

inline constexpr bool TARGET_ESP32_S3 = false;
inline constexpr bool TARGET_ESP32    = true;

#endif