#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "Config.h"
#include "FixtureTypes.h"

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

using SceneCompatLoader = void (*)(uint8_t idx, const void* oldFxPtr);
using SceneAfterLoad = void (*)();

inline void loadSceneCommon(
    Preferences& prefs,
    uint8_t scene,
    Fixture* fixture,
    size_t fixtureSize,
    uint8_t& activeScene,
    SceneCompatLoader compatLoader,
    SceneAfterLoad afterLoad
)
{
    if(!isValidScene(scene))
        return;

    memset(fixture, 0, fixtureSize);

    size_t len = getSceneDataLength(prefs, scene);

    if(len == fixtureSize)
    {
        loadSceneData(prefs, scene, fixture, fixtureSize);
    }
    else if(len == sizeof(Fixture05dCompat) * 16)
    {
        Fixture05dCompat oldFx[16];
        loadSceneData(prefs, scene, oldFx, sizeof(oldFx));

        for(uint8_t i = 0; i < 16; i++)
            compatLoader(i, &oldFx[i]);
    }
    else if(len == sizeof(Fixture05dCompat) * 8)
    {
        Fixture05dCompat oldFx[8];
        loadSceneData(prefs, scene, oldFx, sizeof(oldFx));

        for(uint8_t i = 0; i < 8; i++)
            compatLoader(i, &oldFx[i]);
    }
    else
    {
        memset(fixture, 0, fixtureSize);
    }

    activeScene = scene;
    saveActiveScene(prefs, scene);

    if(afterLoad != nullptr)
        afterLoad();
}