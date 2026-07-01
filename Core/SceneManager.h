#pragma once

#include <Arduino.h>
#include "Config.h"
#include <Preferences.h>

inline bool isValidScene(uint8_t scene)
{
    return scene < SCENE_COUNT;
}

inline void makeSceneKey(uint8_t scene, char* key, size_t keySize)
{
    snprintf(key, keySize, "scene%d", scene);
}

inline void saveActiveScene(Preferences& prefs, uint8_t scene)
{
    prefs.putUChar("activeScene", scene);
}

inline uint8_t loadActiveScene(Preferences& prefs)
{
    uint8_t scene = prefs.getUChar("activeScene", 0);

    if(!isValidScene(scene))
        scene = 0;

    return scene;
}

inline void saveSceneData(
    Preferences& prefs,
    uint8_t scene,
    const Fixture* fixture,
    size_t fixtureSize
)
{
    if(!isValidScene(scene))
        return;

    char key[16];
    makeSceneKey(scene, key, sizeof(key));

    prefs.putBytes(key, fixture, fixtureSize);
}

inline size_t getSceneDataLength(Preferences& prefs, uint8_t scene)
{
    if(!isValidScene(scene))
        return 0;

    char key[16];
    makeSceneKey(scene, key, sizeof(key));

    return prefs.getBytesLength(key);
}

inline void loadSceneData(
    Preferences& prefs,
    uint8_t scene,
    void* buffer,
    size_t bufferSize
)
{
    if(!isValidScene(scene))
        return;

    char key[16];
    makeSceneKey(scene, key, sizeof(key));

    prefs.getBytes(key, buffer, bufferSize);
}

