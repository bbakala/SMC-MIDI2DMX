/*****************************************************************
 SMC-MIDI2DMX_ESP32_WROOM_04_01 / Core_04_01

 ESP32 DevKit + M-VAVE SMC-MIXER DAW mode
 BLE MIDI client + MIDI DIN IN + DMX OUT

 Základní pravidlo verze 05:
 - notifyCB() pouze přijímá a parsuje.
 - Z notifyCB()/parseMidi() se neposílá sendMidi().
 - Veškeré LED feedback zprávy jdou přes LED queue.
*****************************************************************/

#include <NimBLEDevice.h>
#include <Preferences.h>
#include "../Core/Version.h"
#include "../Core/FixtureTypes.h"
#include "../Core/Map_PFT01.h"
#include "../Core/MapSelector.h"
#include "Map_MIDI_DIN_IN.h"

#include "../Core/Config.h"
#include "../Core/TargetPlatform.h"
#include "../Core/Debug.h"
#include "../Core/DmxConfig.h"

// Stavová LED HW varianty. GPIO4: svítí = BLE připojeno, nesvítí = BLE odpojeno.
#define BLE_STATUS_LED_PIN 4

// DMX UART. Uprav podle zapojení RS485 převodníku.
#define DMX_TX_PIN 17
#define DMX_RX_PIN -1
#define DMX_FRAME_INTERVAL_MS 25

// MIDI DIN IN přes UART0 podle odladěného starého HW.
// RX0 = GPIO3, TX0 = GPIO1. TX se pro MIDI nepoužívá, ale necháváme ho aktivní
// pro případný dočasný debug přes sériový převodník při baudrate 31250.
#define MIDI_DIN_USE_UART0 1
#define MIDI_DIN_RX_PIN 3
#define MIDI_DIN_TX_PIN 1
#define MIDI_DIN_BAUD 31250
#define MIDI_DIN_CHANNEL 0  // 0 = OMNI, 1..16 = konkrétní MIDI kanál

// UART0 nemůže současně sloužit jako MIDI IN 31250 i konzole 115200.
// 0 = provoz: UART0 přijímá MIDI DIN na 31250.
// 1 = ladění: UART0 je Serial konzole 115200, MIDI DIN IN je vypnutý.
#define DEBUG_SERIAL 0
#define DEBUG_SERIAL_BAUD 115200

#if DEBUG_SERIAL
#define DBG_PRINT(x) Serial.print(x)
#define DBG_PRINTLN(x) Serial.println(x)
#define DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DBG_PRINT(x)
#define DBG_PRINTLN(x)
#define DBG_PRINTF(...)
#endif

// Když je 1, příchozí MIDI DIN může obnovovat SMC feedback.
// Pro přehrávání ze sequenceru je bezpečnější 0, aby BLE feedback nezahltí fader pickup.
#define MIDI_DIN_REFRESH_CONTROLLER 0

// 1 = MIDI DIN IN se chová jako přímý převodník MIDI -> DMX:
//     z libovolné tříbytové channel voice zprávy (Note/CC/atd.) vezme
//     data1 jako DMX kanál 0..127 a data2 jako MIDI hodnotu 0..127.
//     DMX hodnota = data2 * 2. Status byte určuje jen MIDI kanál.
// 0 = původní režim: přijímá pouze CC a zapisuje přes Map_MIDI_DIN_IN do fixture[].
#define MIDI_DIN_RAW_TO_DMX 1

// Když je RAW režim zapnutý, může zároveň zkusit aktualizovat fixture[] přes
// pevnou vstupní mapu. Samotný DMX výstup ale vždy používá přímo data1/data2.
#define MIDI_DIN_RAW_UPDATE_FIXTURE 1

#define MIDI_SERVICE_UUID "03B80E5A-EDE8-4B33-A751-6CE34EC4C700"
#define MIDI_CHARACTERISTIC_UUID "7772E5DB-3868-4112-A1A9-F2669D106BF3"

Preferences prefs;

HardwareSerial dmxSerial(1);
// MIDI DIN IN používá UART0 = Serial v režimu DEBUG_SERIAL=0.
// V režimu DEBUG_SERIAL=1 je UART0 konzole 115200 a MIDI DIN IN je vypnutý.

// DMX slot na vodiči odpovídá číslu CC z tabulky MAP:
// CC 1 -> DMX kanál 1. dmxBuffer[0] je DMX start code.
#define DMX_LOGICAL_CHANNELS 128
uint8_t dmxBuffer[DMX_BUFFER_SIZE];
uint32_t lastDMXFrame = 0;

#if MIDI_DIN_RAW_TO_DMX
bool midiRawValid[DMX_LOGICAL_CHANNELS];
uint8_t midiRawDmxValue[DMX_LOGICAL_CHANNELS];
#endif

enum WorkMode
{
    MODE_NORMAL,
    MODE_LOAD,
    MODE_SAVE
};

Fixture fixture[NUM_FIXTURES];

// R tlačítko 0..7 nyní vybírá banku pro fyzickou pozici:
// false = fixture 0..7, true = fixture 8..15.
bool fixtureBankHigh[NUM_STRIPS];

uint8_t controlledFixture(uint8_t strip)
{
    strip &= 0x07;
    return strip + (fixtureBankHigh[strip] ? 8 : 0);
}

struct GlobalState
{
    WorkMode mode;

    uint8_t mapDMX;      // 0..3

    uint8_t activeScene; // 0..3

    // Indikace LED scény.
    // Po resetu ESP se nastaví true.
    // Po CLEAR se nastaví false a po reconnectu se znovu nerozsvěcí.
    bool showSceneLed;

    // Indikace LED mapy.
    // Po resetu ESP se nastaví true.
    // Po CLEAR zůstává true a mapa zůstává indikovaná.
    bool showMapLed;
};

GlobalState global;

volatile bool requestRefreshController = false;
volatile bool requestGlobalLeds = false;
volatile bool requestBlinkClear = false;
volatile bool requestPrint = false;
volatile bool requestDMX = false;
volatile bool requestFullOutput = false;

bool ccDirty[128];

uint32_t blinkTimer = 0;
bool blinkState = false;

uint32_t mapBlinkTimer = 0;
bool mapBlinkState = true;

static const NimBLEAdvertisedDevice* advDevice;
static bool doConnect = false;

bool midiConnected = false;

NimBLEClient* pClient = nullptr;
NimBLERemoteCharacteristic* midiChr = nullptr;

static uint32_t scanTimeMs = 5000;

void parseMidi(uint8_t*, size_t);
void parseGlobal(uint8_t, uint8_t, uint8_t);
void parseSelRSM(uint8_t, uint8_t, uint8_t);
uint8_t controlledFixture(uint8_t);
void parseEncoder(uint8_t, uint8_t, uint8_t);
void parseSlider(uint8_t, uint8_t, uint8_t);
void setupMidiDinIn();
void processMidiDinIn();
void handleMidiDinCC(uint8_t, uint8_t, uint8_t);
void handleMidiDinRaw(uint8_t, uint8_t, uint8_t);
bool midiDinStatusHasTwoData(uint8_t);
void applyMidiDinMappedCC(uint8_t, uint8_t);
void clearMidiRawAll();
void clearMidiRawForCC(uint8_t);
void clearMidiRawForFixtureValue(uint8_t, uint8_t);

void refreshController();
void sendGlobalLeds();
void queueMapIndicatorLeds();
void updateBlinking()
{
    if(!midiConnected)
        return;

    // MAP3 indikujeme blikáním obou dostupných MAP LED 5B/5C.
    // MAP0..2 jsou statické kombinace a obnovují se v sendGlobalLeds().
    if((global.mapDMX & 0x03) == 3)
    {
        if(millis() - mapBlinkTimer >= 500)
        {
            mapBlinkTimer = millis();
            mapBlinkState = !mapBlinkState;
            queueMapIndicatorLeds();
        }
    }

    if(global.mode == MODE_NORMAL)
        return;

    uint32_t interval = global.mode == MODE_LOAD ? 500 : 200;

    if(millis() - blinkTimer < interval)
        return;

    blinkTimer = millis();
    blinkState = !blinkState;

    if(global.mode == MODE_LOAD)
    {
        queueLed(0x90, 0x5E, blinkState ? 0x7F : 0x00);
        queueLed(0x90, 0x5F, 0x00);
    }

    else if(global.mode == MODE_SAVE)
    {
        queueLed(0x90, 0x5F, blinkState ? 0x7F : 0x00);
        queueLed(0x90, 0x5E, 0x00);
    }
}

uint8_t getFixtureValue(uint8_t, uint8_t);

uint8_t getMapValue(uint8_t fixtureIdx, uint8_t valueIndex);

void saveScene(uint8_t);
void loadScene(uint8_t);
void clearCurrent();
void clearSelected();

void printAllFixtures();

void sendMidi(uint8_t, uint8_t, uint8_t);

void reconnectNimBLE();
void setupNimBLE();
bool connectToServer();

void processLedQueue();
void queueLed(uint8_t, uint8_t, uint8_t);

void setStatusLed(bool on)
{
    digitalWrite(BLE_STATUS_LED_PIN, on ? HIGH : LOW);
}

void setRgbDisconnected()
{
    setStatusLed(false);
}

void setRgbScan()
{
    setStatusLed(false);
}

void setRgbConnected()
{
    setStatusLed(true);
}

void setRgbLoad()
{
    // Stavová LED indikuje pouze BLE připojení, režim LOAD neřeší.
    setStatusLed(midiConnected);
}

void setRgbSave()
{
    // Stavová LED indikuje pouze BLE připojení, režim SAVE neřeší.
    setStatusLed(midiConnected);
}

void setRgbError()
{
    setStatusLed(false);
}

class ClientCallbacks : public NimBLEClientCallbacks
{
    void onDisconnect(NimBLEClient* pClient, int reason) override
    {
        DBG_PRINTF("Disconnected reason=%d\n", reason);

        midiConnected = false;
        midiChr = nullptr;

        // Po odpojení BLE se vracíme do režimu skenování.
        setRgbScan();

        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }
};

ClientCallbacks clientCallbacks;

class ScanCallbacks : public NimBLEScanCallbacks
{
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override
    {
        if(advertisedDevice->getName() == "SMC-Mixer")
        {
            DBG_PRINTLN("SMC-Mixer found");

            NimBLEDevice::getScan()->stop();

            advDevice = advertisedDevice;
            doConnect = true;
        }
    }

    void onScanEnd(const NimBLEScanResults& results, int reason) override
    {
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }
};

ScanCallbacks scanCallbacks;

void notifyCB(
    NimBLERemoteCharacteristic* pChr,
    uint8_t* pData,
    size_t length,
    bool isNotify
)
{
    parseMidi(pData, length);
}

bool connectToServer()
{
    pClient = NimBLEDevice::createClient();

    pClient->setClientCallbacks(&clientCallbacks, false);
    pClient->setConnectionParams(6, 6, 0, 100);
    pClient->setConnectTimeout(5000);

    if(!pClient->connect(advDevice))
    {
        return false;
    }

    NimBLERemoteService* pSvc = pClient->getService(MIDI_SERVICE_UUID);

    if(!pSvc)
    {
        return false;
    }

    midiChr = pSvc->getCharacteristic(MIDI_CHARACTERISTIC_UUID);

    if(!midiChr)
    {
        return false;
    }

    if(!midiChr->subscribe(true, notifyCB))
    {
        return false;
    }

    midiConnected = true;
    setRgbConnected();

    delay(300);

    requestGlobalLeds = true;
    requestRefreshController = true;

    return true;
}

void reconnectNimBLE()
{
    if(!doConnect)
        return;

    doConnect = false;

    if(connectToServer())
    {
        DBG_PRINTLN("BLE connected");
    }
    else
    {
        setRgbError();
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }
}

void parseMidi(uint8_t* pData, size_t length)
{
    if(length < 5)
        return;

    uint8_t status = pData[2];
    uint8_t cc     = pData[3];
    uint8_t value  = pData[4];

    parseGlobal(status, cc, value);
    parseSelRSM(status, cc, value);
    parseEncoder(status, cc, value);
    parseSlider(status, cc, value);
}

void parseGlobal(uint8_t status, uint8_t cc, uint8_t value)
{
    if(status != 0x90)
        return;

    // Některá globální tlačítka M-VAVE po uvolnění lokálně zhasnou.
    // Proto po release znovu pošleme skutečný stav LED podle global.mapDMX.
    // Výběr mapy ale měníme pouze při value == 0x7F.
    if(value == 0x00)
    {
        if(cc == 0x5B || cc == 0x5C || cc == 0x2E || cc == 0x2F)
        {
            requestGlobalLeds = true;
        }
        return;
    }

    if(value != 0x7F)
        return;

    // LOAD 90 5E
    if(cc == 0x5E)
    {
        // Opětovný stisk LOAD zruší čekající LOAD režim.
        if(global.mode == MODE_LOAD)
        {
            global.mode = MODE_NORMAL;
            setRgbConnected();
        }
        else
        {
            global.mode = MODE_LOAD;
            setRgbLoad();
        }

        requestGlobalLeds = true;
        return;
    }

    // SAVE 90 5F
    if(cc == 0x5F)
    {
        // Opětovný stisk SAVE zruší čekající SAVE režim.
        if(global.mode == MODE_SAVE)
        {
            global.mode = MODE_NORMAL;
            setRgbConnected();
        }
        else
        {
            global.mode = MODE_SAVE;
            setRgbSave();
        }

        requestGlobalLeds = true;
        return;
    }

    // CLEAR 90 5D
    if(cc == 0x5D)
    {
        clearCurrent();
        global.mode = MODE_NORMAL;
        setRgbConnected();

        requestRefreshController = true;
        requestGlobalLeds = true;
        requestBlinkClear = true;
        requestFullOutput = true;
        requestPrint = true;
        return;
    }

    // MAP 0..3
    if(cc == 0x5B)
    {
        global.mapDMX = 0;
        prefs.putUChar("mapDMX", global.mapDMX);
        // Změna MAP nemění MIDI RAW vrstvu; RAW DMX kanály jsou nezávislé na MAP.
        mapBlinkState = true;
        mapBlinkTimer = millis();
        requestGlobalLeds = true;
        requestFullOutput = true;
        return;
    }

    if(cc == 0x5C)
    {
        global.mapDMX = 1;
        prefs.putUChar("mapDMX", global.mapDMX);
        // Změna MAP nemění MIDI RAW vrstvu; RAW DMX kanály jsou nezávislé na MAP.
        mapBlinkState = true;
        mapBlinkTimer = millis();
        requestGlobalLeds = true;
        requestFullOutput = true;
        return;
    }

    if(cc == 0x2E)
    {
        global.mapDMX = 2;
        prefs.putUChar("mapDMX", global.mapDMX);
        // Změna MAP nemění MIDI RAW vrstvu; RAW DMX kanály jsou nezávislé na MAP.
        mapBlinkState = true;
        mapBlinkTimer = millis();
        requestGlobalLeds = true;
        requestFullOutput = true;
        return;
    }

    if(cc == 0x2F)
    {
        global.mapDMX = 3;
        prefs.putUChar("mapDMX", global.mapDMX);
        // Změna MAP nemění MIDI RAW vrstvu; RAW DMX kanály jsou nezávislé na MAP.
        mapBlinkState = true;
        mapBlinkTimer = millis();
        requestGlobalLeds = true;
        requestFullOutput = true;
        return;
    }

    // SCENE 0..3
    if(cc >= 0x60 && cc <= 0x63)
    {
        uint8_t scene = cc - 0x60;

        global.activeScene = scene;
        prefs.putUChar("activeScene", scene);

        if(global.mode == MODE_LOAD)
        {
            global.mode = MODE_NORMAL;
            loadScene(scene);
            setRgbConnected();

            requestRefreshController = true;
            requestGlobalLeds = true;
            requestFullOutput = true;
            requestPrint = true;
            return;
        }

        if(global.mode == MODE_SAVE)
        {
            global.mode = MODE_NORMAL;
            saveScene(scene);
            setRgbConnected();

            requestGlobalLeds = true;
            requestDMX = true;
            requestPrint = true;
            return;
        }

        requestGlobalLeds = true;
        return;
    }
}

void parseSelRSM(uint8_t status, uint8_t cc, uint8_t value)
{
    if(status != 0x90)
        return;

    if(value != 0x7F)
        return;

    if(global.mode != MODE_NORMAL)
        return;

    // R 90 00..07
    // V _06 R nepřepíná parametr fixture.R, ale banku fyzické pozice.
    // R OFF = strip ovládá fixture 0..7, R ON = strip ovládá fixture 8..15.
    if(cc <= 0x07)
    {
        uint8_t strip = cc;

        fixtureBankHigh[strip] = !fixtureBankHigh[strip];

        // Překreslit LED fyzické pozice podle právě ovládaného fixture.
        uint8_t ch = controlledFixture(strip);

        queueLed(0x90, cc, fixtureBankHigh[strip] ? 0x7F : 0x00);
        queueLed(0x90, 0x08 + strip, fixture[ch].S ? 0x7F : 0x00);
        queueLed(0x90, 0x10 + strip, fixture[ch].M ? 0x7F : 0x00);
        queueLed(0x90, 0x18 + strip, fixture[ch].selected ? 0x7F : 0x00);
        queueLed(0xE0 + strip, 0x00, fixture[ch].value[VAL_DIMMER]);

        requestPrint = true;
        return;
    }

    // S 90 08..0F
    if(cc >= 0x08 && cc <= 0x0F)
    {
        uint8_t strip = cc - 0x08;
        uint8_t ch = controlledFixture(strip);
        fixture[ch].S = !fixture[ch].S;

        // S je nyní STROBE gate:
        // S ON  -> na výstup jde uložená hodnota fixture[ch].value[VAL_STROBE].
        // S OFF -> na výstup jde 0, uložená hodnota Strobe se nemaže.
        queueLed(0x90, cc, fixture[ch].S ? 0x7F : 0x00);
        markMappedCCDirty(ch, VAL_STROBE);
        requestDMX = true;
        requestPrint = true;
        return;
    }

    // M 90 10..17
    if(cc >= 0x10 && cc <= 0x17)
    {
        uint8_t strip = cc - 0x10;
        uint8_t ch = controlledFixture(strip);
        fixture[ch].M = !fixture[ch].M;

        // M je nyní EFFECT/MODE gate:
        // M ON  -> na výstup jde uložená hodnota fixture[ch].value[VAL_EFFECT].
        // M OFF -> na výstup jde 0, uložená hodnota Effect/Mode se nemaže.
        queueLed(0x90, cc, fixture[ch].M ? 0x7F : 0x00);
        markMappedCCDirty(ch, VAL_EFFECT);
        requestDMX = true;
        requestPrint = true;
        return;
    }

    // SELECT 90 18..1F
    if(cc >= 0x18 && cc <= 0x1F)
    {
        uint8_t strip = cc - 0x18;
        uint8_t ch = controlledFixture(strip);
        fixture[ch].selected = !fixture[ch].selected;
        queueLed(0x90, cc, fixture[ch].selected ? 0x7F : 0x00);
        requestPrint = true;
        return;
    }
}

bool changeValue(
    uint8_t &v,
    int delta
)
{
    uint8_t old = v;
    int n = (int)v + delta;

    if(n < 0)
        n = 0;

    // Fixture hodnoty držíme v MIDI rozsahu 0..127.
    if(n > 127)
        n = 127;

    v = (uint8_t)n;
    return v != old;
}

void parseEncoder(uint8_t status, uint8_t cc, uint8_t value)
{
    if(status != 0xB0)
        return;

    int delta = 0;

    if(value == 0x01)
        delta = 1;
    else if(value == 0x41)
        delta = -1;
    else
        return;

    uint8_t valueIndex = VAL_UNUSED;

    switch(cc)
    {
        case 0x10: valueIndex = VAL_RED; break;
        case 0x11: valueIndex = VAL_GREEN; break;
        case 0x12: valueIndex = VAL_BLUE; break;
        case 0x13: valueIndex = VAL_WHITE; break;
        case 0x14: valueIndex = VAL_STROBE; break;
        case 0x15: valueIndex = VAL_EFFECT; break;
        case 0x16: valueIndex = VAL_SPEED; break;
        case 0x17: valueIndex = VAL_CUSTOM; break;
        default: return;
    }

    bool changed = false;

    for(uint8_t ch = 0; ch < NUM_FIXTURES; ch++)
    {
        if(!fixture[ch].selected)
            continue;

        if(changeValue(fixture[ch].value[valueIndex], delta))
        {
            markFixtureValueChanged(ch, valueIndex);
            changed = true;
        }
    }

    if(changed)
    {
        requestDMX = true;
        requestPrint = true;
    }
}

void parseSlider(uint8_t status, uint8_t cc, uint8_t value)
{
    if((status & 0xF0) != 0xE0)
        return;

    uint8_t strip = status & 0x0F;

    if(strip >= NUM_STRIPS)
        return;

    uint8_t ch = controlledFixture(strip);

    if(fixture[ch].value[VAL_DIMMER] != value)
    {
        fixture[ch].value[VAL_DIMMER] = value;
        markFixtureValueChanged(ch, VAL_DIMMER);
        requestDMX = true;
        requestPrint = true;
    }
}

void clearSelected()
{
    for(uint8_t i = 0; i < NUM_FIXTURES; i++)
    {
        fixture[i].selected = false;
    }
}

struct Fixture05dCompat
{
    bool selected;
    uint8_t dimmer;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    uint8_t strobe;
    uint8_t effect;
    uint8_t speed;
    uint8_t custom;
    bool R;
    bool S;
    bool M;
};

void copyOldFixtureToNew(uint8_t idx, const void* oldFxPtr)
{
    if(idx >= NUM_FIXTURES || oldFxPtr == nullptr)
        return;

    const Fixture05dCompat* oldFx = (const Fixture05dCompat*)oldFxPtr;

    fixture[idx].selected = oldFx->selected;
    fixture[idx].R = oldFx->R;
    fixture[idx].S = oldFx->S;
    fixture[idx].M = oldFx->M;

    fixture[idx].value[VAL_DIMMER] = oldFx->dimmer;
    fixture[idx].value[VAL_RED] = oldFx->red;
    fixture[idx].value[VAL_GREEN] = oldFx->green;
    fixture[idx].value[VAL_BLUE] = oldFx->blue;
    fixture[idx].value[VAL_WHITE] = oldFx->white;
    fixture[idx].value[VAL_STROBE] = oldFx->strobe;
    fixture[idx].value[VAL_EFFECT] = oldFx->effect;
    fixture[idx].value[VAL_SPEED] = oldFx->speed;
    fixture[idx].value[VAL_CUSTOM] = oldFx->custom;
}


void clearMidiRawAll()
{
#if MIDI_DIN_RAW_TO_DMX
    // Ruší celou MIDI RAW vrstvu. V Core_03 se nevolá při běžném CLEAR/LOAD,
    // ale zůstává k dispozici pro budoucí funkci CLEAR_RAW / panic.
    memset(midiRawValid, 0, sizeof(midiRawValid));
    memset(midiRawDmxValue, 0, sizeof(midiRawDmxValue));
#endif
}

void clearMidiRawForCC(uint8_t cc)
{
#if MIDI_DIN_RAW_TO_DMX
    if(cc < DMX_LOGICAL_CHANNELS)
    {
        midiRawValid[cc] = false;
        midiRawDmxValue[cc] = 0;
    }
#else
    (void)cc;
#endif
}

void clearMidiRawForFixtureValue(uint8_t fixtureIdx, uint8_t valueIndex)
{
#if MIDI_DIN_RAW_TO_DMX
    if(fixtureIdx >= NUM_FIXTURES || valueIndex >= NUM_VALUES)
        return;

    for(uint8_t i = 0; i < activeDmxMapCount(global.mapDMX); i++)
    {
        const DmxMapItem& item = activeDmxMap(global.mapDMX)[i];

        if(item.fixture == fixtureIdx && item.valueIndex == valueIndex)
            clearMidiRawForCC(item.cc);
    }
#else
    (void)fixtureIdx;
    (void)valueIndex;
#endif
}

void saveScene(uint8_t scene)
{
    if(scene > 3)
        return;

    char key[10];
    sprintf(key, "scene%d", scene);

    prefs.putBytes(key, fixture, sizeof(fixture));
}

void loadScene(uint8_t scene)
{
    if(scene > 3)
        return;

    // LOAD mění pouze fixture[] vrstvu.
    // MIDI RAW vrstva zůstává zachovaná: sequencer kanály běží dál.

    char key[10];
    sprintf(key, "scene%d", scene);

    memset(fixture, 0, sizeof(fixture));

    size_t len = prefs.getBytesLength(key);

    // Kompatibilita:
    // - _06a ukládá Fixture[16] s value[16].
    // - _05d/_06 ukládaly starou strukturu Fixture05dCompat pro 8 nebo 16 fixture.
    if(len == sizeof(fixture))
    {
        prefs.getBytes(key, fixture, sizeof(fixture));
    }
    else if(len == sizeof(Fixture05dCompat) * 16)
    {
        Fixture05dCompat oldFx[16];
        prefs.getBytes(key, oldFx, sizeof(oldFx));

        for(uint8_t i = 0; i < 16; i++)
            copyOldFixtureToNew(i, &oldFx[i]);
    }
    else if(len == sizeof(Fixture05dCompat) * 8)
    {
        Fixture05dCompat oldFx[8];
        prefs.getBytes(key, oldFx, sizeof(oldFx));

        for(uint8_t i = 0; i < 8; i++)
            copyOldFixtureToNew(i, &oldFx[i]);
    }
    else
    {
        memset(fixture, 0, sizeof(fixture));
    }

    global.activeScene = scene;
    prefs.putUChar("activeScene", scene);

    clearSelected();
}

void clearCurrent()
{
    // CLEAR nuluje pouze fixture[] vrstvu.
    // MIDI RAW vrstva zůstává zachovaná, aby běžící sequencer nemusel znovu posílat neměnné CC.

    memset(fixture, 0, sizeof(fixture));
    memset(fixtureBankHigh, 0, sizeof(fixtureBankHigh));
    clearSelected();
}

void printFixture(uint8_t ch)
{
    DBG_PRINTF("CH%02d ", ch);

    for(uint8_t i = 0; i < NUM_VALUES; i++)
    {
        DBG_PRINTF("V%02d:%3d ", i, fixture[ch].value[i]);
    }

    DBG_PRINTF(
        "R:%d S:%d M:%d Sel:%d\n",
        fixture[ch].R,
        fixture[ch].S,
        fixture[ch].M,
        fixture[ch].selected
    );
}

void printAllFixtures()
{
    DBG_PRINTLN("");

    for(uint8_t ch = 0; ch < NUM_FIXTURES; ch++)
    {
        printFixture(ch);
    }

    DBG_PRINTLN("");
}

struct PendingLed
{
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
};

#define LED_QUEUE_SIZE 128

PendingLed ledQueue[LED_QUEUE_SIZE];

volatile uint8_t ledHead = 0;
volatile uint8_t ledTail = 0;

void queueLed(uint8_t status, uint8_t data1, uint8_t data2)
{
    uint8_t next = (ledHead + 1) % LED_QUEUE_SIZE;

    if(next == ledTail)
    {
        DBG_PRINTLN("LED queue full");
        return;
    }

    ledQueue[ledHead].status = status;
    ledQueue[ledHead].data1 = data1;
    ledQueue[ledHead].data2 = data2;

    ledHead = next;
}

void sendMidi(uint8_t status, uint8_t data1, uint8_t data2)
{
    if(!midiConnected || !midiChr)
        return;

    uint8_t packet[5] =
    {
        0x80,
        0x80,
        status,
        data1,
        data2
    };

    midiChr->writeValue(packet, sizeof(packet), false);
}

void processLedQueue()
{
    static uint32_t lastSend = 0;

    if(!midiConnected)
        return;

    if(ledTail == ledHead)
        return;

    if(millis() - lastSend < 12)
        return;

    lastSend = millis();

    PendingLed msg = ledQueue[ledTail];

    ledTail = (ledTail + 1) % LED_QUEUE_SIZE;

    sendMidi(msg.status, msg.data1, msg.data2);
}

void refreshController()
{
    if(!midiConnected)
        return;

    // Fyzických je 8 pozic, ale každá může přes R tlačítko ovládat fixture 0..7 nebo 8..15.
    for(uint8_t i = 0; i < NUM_STRIPS; i++)
    {
        uint8_t ch = controlledFixture(i);

        // R LED nyní indikuje banku: OFF = fixture 0..7, ON = fixture 8..15.
        queueLed(0x90, i, fixtureBankHigh[i] ? 0x7F : 0x00);

        queueLed(0x90, 0x08 + i, fixture[ch].S ? 0x7F : 0x00);
        queueLed(0x90, 0x10 + i, fixture[ch].M ? 0x7F : 0x00);
        queueLed(0x90, 0x18 + i, fixture[ch].selected ? 0x7F : 0x00);
        queueLed(0xE0 + i, 0x00, fixture[ch].value[VAL_DIMMER]);
    }
}

void queueMapIndicatorLeds()
{
    uint8_t led5B = 0x00;
    uint8_t led5C = 0x00;

    switch(global.mapDMX & 0x03)
    {
        case 0:
            led5B = 0x7F;
            led5C = 0x00;
            break;

        case 1:
            led5B = 0x00;
            led5C = 0x7F;
            break;

        case 2:
            led5B = 0x7F;
            led5C = 0x7F;
            break;

        case 3:
            led5B = mapBlinkState ? 0x7F : 0x00;
            led5C = mapBlinkState ? 0x7F : 0x00;
            break;
    }

    queueLed(0x90, 0x5B, led5B);
    queueLed(0x90, 0x5C, led5C);

    // 2E/2F necháváme jako vstupní tlačítka, ne jako stavové LED.
    queueLed(0x90, 0x2E, 0x00);
    queueLed(0x90, 0x2F, 0x00);
}

void sendGlobalLeds()
{
    // SAVE 90 5F
    queueLed(0x90, 0x5F, global.mode == MODE_SAVE ? 0x7F : 0x00);

    // LOAD 90 5E
    queueLed(0x90, 0x5E, global.mode == MODE_LOAD ? 0x7F : 0x00);

    // CLEAR 90 5D always OFF here
    queueLed(0x90, 0x5D, 0x00);

    // MAP 0..3
    // M-VAVE u tlačítek 2E/2F neposlouchá trvalý LED feedback.
    // Proto MAP stav indikujeme pouze pomocí řiditelných LED 5B/5C:
    // MAP0 = 5B, MAP1 = 5C, MAP2 = 5B+5C, MAP3 = 5B+5C bliká.
    queueMapIndicatorLeds();

    // SCENE 0..3
    for(uint8_t i = 0; i < 4; i++)
    {
        queueLed(0x90, 0x60 + i, global.activeScene == i ? 0x7F : 0x00);
    }
}

void blinkClearLed()
{
    queueLed(0x90, 0x5D, 0x7F);
    queueLed(0x90, 0x5D, 0x00);
    queueLed(0x90, 0x5D, 0x7F);
    queueLed(0x90, 0x5D, 0x00);
}


uint8_t getFixtureValue(uint8_t ch, uint8_t valueIndex)
{
    if(ch >= NUM_FIXTURES)
        return 0;

    if(valueIndex >= NUM_VALUES)
        return 0;

    return fixture[ch].value[valueIndex];
}

uint8_t getMapValue(uint8_t fixtureIdx, uint8_t valueIndex)
{
    if(fixtureIdx >= NUM_FIXTURES)
        return 0;

    if(valueIndex == VAL_UNUSED || valueIndex >= NUM_VALUES)
        return 0;

    // STROBE gate ovládaný tlačítkem S:
    // hodnota Strobe je uložena ve fixture[].value[VAL_STROBE],
    // ale do USB MIDI CC / DMX se propustí jen tehdy, když je fixture[].S zapnuto.
    if(valueIndex == VAL_STROBE && !fixture[fixtureIdx].S)
        return 0;

    // EFFECT/MODE gate ovládaný tlačítkem M:
    // hodnota Effect/Mode je uložena ve fixture[].value[VAL_EFFECT],
    // ale do USB MIDI CC / DMX se propustí jen tehdy, když je fixture[].M zapnuto.
    if(valueIndex == VAL_EFFECT && !fixture[fixtureIdx].M)
        return 0;

    return getFixtureValue(fixtureIdx, valueIndex);
}

void sendUsbCC(uint8_t cc, uint8_t value)
{
    if(cc > 127)
        return;

    if(value > 127)
        value = 127;

    // ESP32 DevKit varianta nemá USB MIDI OUT.
    // Funkce zůstává jako no-op kvůli zachování dirty-output vrstvy z _08_clean.
    (void)cc;
    (void)value;
}

void sendFixtureUsbMidiCC()
{
    // USB MIDI CC OUT je v této ESP32 DIN verzi vypnutý.
        for(uint8_t i = 0; i < activeDmxMapCount(global.mapDMX); i++)
    {
        uint8_t cc = activeDmxMap(global.mapDMX)[i].cc;
        uint8_t value = getMapValue(activeDmxMap(global.mapDMX)[i].fixture, activeDmxMap(global.mapDMX)[i].valueIndex);

        sendUsbCC(cc, value);
    }
}


void markMappedCCDirty(uint8_t fixtureIdx, uint8_t valueIndex)
{
    if(fixtureIdx >= NUM_FIXTURES || valueIndex >= NUM_VALUES)
        return;

    for(uint8_t i = 0; i < activeDmxMapCount(global.mapDMX); i++)
    {
        if(activeDmxMap(global.mapDMX)[i].fixture == fixtureIdx && activeDmxMap(global.mapDMX)[i].valueIndex == valueIndex)
        {
            uint8_t cc = activeDmxMap(global.mapDMX)[i].cc;

            if(cc < 128)
            {
                // Pravidlo Core_03: poslední zdroj na konkrétním DMX/CC kanálu vyhrává.
                // Ruční změna ze SMC-Mixeru zruší RAW platnost jen pro tento kanál.
                // Další MIDI zpráva na stejném kanálu RAW vrstvu znovu aktivuje.
                clearMidiRawForCC(cc);
                ccDirty[cc] = true;
            }
        }
    }
}

void markFixtureValueChanged(uint8_t fixtureIdx, uint8_t valueIndex)
{
    if(fixtureIdx >= NUM_FIXTURES || valueIndex >= NUM_VALUES)
        return;

    // Změna uložené hodnoty Strobe se na výstup projeví jen pokud je S zapnuté.
    // Pokud je S vypnuté, výstupní hodnota zůstává 0, takže neposíláme zbytečný CC 0.
    if(valueIndex == VAL_STROBE && !fixture[fixtureIdx].S)
        return;

    // Změna uložené hodnoty Effect/Mode se na výstup projeví jen pokud je M zapnuté.
    // Pokud je M vypnuté, výstupní hodnota zůstává 0, takže neposíláme zbytečný CC 0.
    if(valueIndex == VAL_EFFECT && !fixture[fixtureIdx].M)
        return;

    markMappedCCDirty(fixtureIdx, valueIndex);
}

void markAllMappedCCDirty()
{
    for(uint8_t i = 0; i < activeDmxMapCount(global.mapDMX); i++)
    {
        uint8_t cc = activeDmxMap(global.mapDMX)[i].cc;

        if(cc < 128)
            ccDirty[cc] = true;
    }
}

void sendDirtyFixtureUsbMidiCC()
{
    for(uint8_t i = 0; i < activeDmxMapCount(global.mapDMX); i++)
    {
        uint8_t cc = activeDmxMap(global.mapDMX)[i].cc;

        if(cc >= 128 || !ccDirty[cc])
            continue;

        uint8_t value = getMapValue(activeDmxMap(global.mapDMX)[i].fixture, activeDmxMap(global.mapDMX)[i].valueIndex);
        sendUsbCC(cc, value);
        ccDirty[cc] = false;
    }
}

void refreshDMX()
{
    clearDmxBuffer(dmxBuffer, sizeof(dmxBuffer));

    // DMX OUT podle aktivní MAP tabulky.
    for(uint8_t i = 0; i < activeDmxMapCount(global.mapDMX); i++)
    {
        uint8_t addr = activeDmxMap(global.mapDMX)[i].cc;

        if(addr >= DMX_LOGICAL_CHANNELS)
            continue;

        uint8_t value = getMapValue(activeDmxMap(global.mapDMX)[i].fixture, activeDmxMap(global.mapDMX)[i].valueIndex);

        // DMX hodnota je pouze 2 * MIDI hodnota, tedy 0..254.
        if(addr == 0)
            continue;

        dmxBuffer[addr] = midiValueToDmx(value);
    }

#if MIDI_DIN_RAW_TO_DMX
    // MIDI RAW vrstva: poslední hodnota ze sequenceru má prioritu jen na kanálech,
    // kde je midiRawValid[addr] == true.
    for(uint8_t addr = 1; addr < DMX_LOGICAL_CHANNELS; addr++)
    {
        if(midiRawValid[addr])
            dmxBuffer[addr] = midiRawDmxValue[addr];
    }
#endif
}

void refreshOutputs()
{
    // Běžná změna: po USB MIDI odešle pouze CC označená jako změněná.
    // DMX buffer se přepočítá celý z fixture[], protože DMX se stejně vysílá jako rámec.
    sendDirtyFixtureUsbMidiCC();
    refreshDMX();
}

void setupDMX()
{
    clearDmxBuffer(dmxBuffer, sizeof(dmxBuffer));
#if MIDI_DIN_RAW_TO_DMX
    memset(midiRawValid, 0, sizeof(midiRawValid));
    memset(midiRawDmxValue, 0, sizeof(midiRawDmxValue));
#endif
    dmxSerial.begin(DMX_BAUD, SERIAL_8N2, DMX_RX_PIN, DMX_TX_PIN);
}

void sendDMXFrame()
{
    if(millis() - lastDMXFrame < DMX_FRAME_INTERVAL_MS)
        return;

    lastDMXFrame = millis();

    dmxSerial.flush();

    // DMX break + mark-after-break.
    dmxSerial.end();
    pinMode(DMX_TX_PIN, OUTPUT);
    digitalWrite(DMX_TX_PIN, LOW);
    delayMicroseconds(120);
    digitalWrite(DMX_TX_PIN, HIGH);
    delayMicroseconds(12);
    dmxSerial.begin(DMX_BAUD, SERIAL_8N2, DMX_RX_PIN, DMX_TX_PIN);

    dmxSerial.write(dmxBuffer, sizeof(dmxBuffer));
}


// -----------------------------------------------------------------------------
// MIDI DIN IN
// -----------------------------------------------------------------------------

uint8_t midiDinRunningStatus = 0;
uint8_t midiDinData[2] = {0, 0};
uint8_t midiDinDataCount = 0;

bool midiDinStatusHasTwoData(uint8_t status)
{
    switch(status & 0xF0)
    {
        case 0x80: // Note Off
        case 0x90: // Note On
        case 0xA0: // Poly Pressure
        case 0xB0: // Control Change
        case 0xE0: // Pitch Bend
            return true;
    }

    return false;
}

bool midiDinChannelAccepted(uint8_t status)
{
    if((status & 0xF0) < 0x80 || (status & 0xF0) > 0xE0)
        return false;

    if(MIDI_DIN_CHANNEL == 0)
        return true;

    uint8_t ch = (status & 0x0F) + 1;
    return ch == MIDI_DIN_CHANNEL;
}

void setupMidiDinIn()
{
#if DEBUG_SERIAL
    DBG_PRINTLN("MIDI DIN IN vypnuty: UART0 je debug konzole 115200");
#else
    Serial.begin(MIDI_DIN_BAUD, SERIAL_8N1, MIDI_DIN_RX_PIN, MIDI_DIN_TX_PIN);
#endif
}

void processMidiDinByte(uint8_t b)
{
    // MIDI realtime zprávy mohou přijít kdykoliv, pro převod je ignorujeme.
    if(b >= 0xF8)
        return;

    if(b & 0x80)
    {
        midiDinRunningStatus = b;
        midiDinDataCount = 0;
        return;
    }

    if(midiDinRunningStatus == 0)
        return;

#if MIDI_DIN_RAW_TO_DMX
    // RAW režim: nezajímá nás, jestli to byla Note nebo CC.
    // Použije se poslední kompletní trojice: status, data1, data2.
    if(!midiDinStatusHasTwoData(midiDinRunningStatus))
        return;
#else
    // Mapovaný režim: přijímáme pouze Control Change B0..BF.
    if((midiDinRunningStatus & 0xF0) != 0xB0)
        return;
#endif

    if(midiDinDataCount == 0)
    {
        midiDinData[0] = b; // data1: v RAW režimu DMX kanál, v CC režimu CC číslo
        midiDinDataCount = 1;
    }
    else
    {
        midiDinData[1] = b; // data2: MIDI hodnota
        midiDinDataCount = 0;

        if(midiDinChannelAccepted(midiDinRunningStatus))
        {
#if MIDI_DIN_RAW_TO_DMX
            handleMidiDinRaw(midiDinRunningStatus, midiDinData[0], midiDinData[1]);
#else
            uint8_t channel = (midiDinRunningStatus & 0x0F) + 1;
            handleMidiDinCC(channel, midiDinData[0], midiDinData[1]);
#endif
        }
    }
}

void processMidiDinIn()
{
    while(Serial.available() > 0)
    {
        uint8_t b = (uint8_t)Serial.read();
        processMidiDinByte(b);
    }
}

void handleMidiDinRaw(uint8_t status, uint8_t data1, uint8_t data2)
{
    (void)status;

    uint8_t dmxChannel = data1 & 0x7F;
    uint8_t midiValue = data2 & 0x7F;

#if MIDI_DIN_RAW_UPDATE_FIXTURE
    // Volitelně udržujeme fixture[] v souladu tam, kde existuje pevná vstupní mapa.
    applyMidiDinMappedCC(dmxChannel, midiValue);
#endif

    // DMX kanál 0 v praxi nepoužíváme, protože dmxBuffer[0] je start code.
    // Kanály 1..127 jsou přímý DMX/CC prostor.
    if(dmxChannel > 0 && dmxChannel < DMX_LOGICAL_CHANNELS)
    {
        midiRawValid[dmxChannel] = true;
        midiRawDmxValue[dmxChannel] = midiValue * 2;
        requestDMX = true;
    }
}

void handleMidiDinCC(uint8_t channel, uint8_t cc, uint8_t value)
{
    (void)channel;

    if(cc > 127)
        return;

    if(value > 127)
        value = 127;

    applyMidiDinMappedCC(cc, value);
}

void applyMidiDinMappedCC(uint8_t cc, uint8_t value)
{
    bool changed = false;

    for(uint8_t i = 0; i < midiDinInMapCount; i++)
    {
        const DmxMapItem& item = midiDinInMap[i];

        if(item.cc != cc)
            continue;

        if(item.fixture >= NUM_FIXTURES)
            continue;

        if(item.valueIndex == VAL_UNUSED || item.valueIndex >= NUM_VALUES)
            continue;

        Fixture& fx = fixture[item.fixture];

        // MIDI DIN CC reprezentuje skutečnou výstupní hodnotu.
        // U gate parametrů proto podle hodnoty nastavíme i S/M stav:
        //   Strobe CC > 0 -> S ON,  Strobe CC == 0 -> S OFF
        //   Effect CC > 0 -> M ON,  Effect CC == 0 -> M OFF
        if(item.valueIndex == VAL_STROBE)
        {
            fx.S = (value > 0);
            if(value > 0)
                fx.value[VAL_STROBE] = value;
        }
        else if(item.valueIndex == VAL_EFFECT)
        {
            fx.M = (value > 0);
            if(value > 0)
                fx.value[VAL_EFFECT] = value;
        }
        else
        {
            fx.value[item.valueIndex] = value;
        }

        changed = true;
    }

    if(changed)
    {
        requestDMX = true;
        requestPrint = true;

#if MIDI_DIN_REFRESH_CONTROLLER
        requestRefreshController = true;
        requestGlobalLeds = true;
#endif
    }
}

void setupNimBLE()
{
    NimBLEDevice::init("NimBLE-Client");

    NimBLEDevice::setPower(3);

    NimBLEScan* pScan = NimBLEDevice::getScan();

    pScan->setScanCallbacks(&scanCallbacks, false);
    pScan->setInterval(100);
    pScan->setWindow(100);
    pScan->setActiveScan(true);

    setRgbScan();

    pScan->start(scanTimeMs);
}

void initGlobal()
{
    global.mode = MODE_NORMAL;

    global.mapDMX = prefs.getUChar("mapDMX", 0);

    if(global.mapDMX > 3)
        global.mapDMX = 0;

    global.activeScene = prefs.getUChar("activeScene", 0);

    if(global.activeScene > 3)
        global.activeScene = 0;

    // Po resetu ESP zobrazíme poslední mapu a poslední scénu.
    // CLEAR tyto příznaky zhasne pouze v RAM.
    global.showMapLed = true;

    global.showSceneLed = true;

    loadScene(global.activeScene);

    requestRefreshController = true;
    requestGlobalLeds = true;
    requestFullOutput = true;
    requestPrint = true;
}

void setup()
{
#if DEBUG_SERIAL
    Serial.begin(DEBUG_SERIAL_BAUD);
#endif

    delay(300);

    // Status LED
    pinMode(BLE_STATUS_LED_PIN, OUTPUT);
    setStatusLed(false);

    // NVS
    prefs.begin("dmx", false);

    // DMX output
    setupDMX();

    // MIDI DIN input
    setupMidiDinIn();

    // Load persistent configuration
    initGlobal();

    // BLE MIDI client
    setupNimBLE();

    // Startup information
    DBG_PRINT(PROJECT_NAME);
    DBG_PRINT(" ESP32_WROOM ");
    DBG_PRINT(PROJECT_VERSION);
    DBG_PRINT("  ");
    DBG_PRINT(BUILD_DATE);
    DBG_PRINT(" ");
    DBG_PRINTLN(BUILD_TIME);
}

void loop()
{
    reconnectNimBLE();

#if !DEBUG_SERIAL
    processMidiDinIn();
#endif

    updateBlinking();

    processLedQueue();

    sendDMXFrame();

    if(requestRefreshController)
    {
        requestRefreshController = false;
        refreshController();
    }

    if(requestGlobalLeds)
    {
        requestGlobalLeds = false;
        sendGlobalLeds();
    }

    if(requestBlinkClear)
    {
        requestBlinkClear = false;
        blinkClearLed();
    }

    if(requestFullOutput)
    {
        requestFullOutput = false;
        requestDMX = false;
        markAllMappedCCDirty();
        sendDirtyFixtureUsbMidiCC();
        refreshDMX();
    }

    if(requestDMX)
    {
        requestDMX = false;
        refreshOutputs();
    }

    if(requestPrint)
    {
        requestPrint = false;
        printAllFixtures();
    }
}
