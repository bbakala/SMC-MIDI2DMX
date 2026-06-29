#pragma once

#include "../Core/FixtureTypes.h"
#include "../Core/Map_PFT01.h"

// -----------------------------------------------------------------------------
// Map_MIDI_DIN_IN.h
// Pevná vstupní mapa pro MIDI DIN IN.
// Pro tuto HW variantu je vstupní CC mapa stejná jako Map_PFT01.h.
// Příchozí MIDI CC číslo se přeloží na fixture/valueIndex a zapíše do fixture[].
// Tato mapa je nezávislá na aktivní výstupní MAP, ale záměrně odpovídá PFT01.
// -----------------------------------------------------------------------------

const DmxMapItem* const midiDinInMap = dmxMapPFT01;
const uint8_t midiDinInMapCount = dmxMapPFT01Count;
