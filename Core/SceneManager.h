#pragma once

#include <Arduino.h>
#include "Config.h"

inline bool isValidScene(uint8_t scene)
{
    return scene < SCENE_COUNT;
}

inline void makeSceneKey(uint8_t scene, char* key, size_t keySize)
{
    snprintf(key, keySize, "scene%d", scene);
}

