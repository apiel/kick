#ifndef AudioWaveTableList_h_
#define AudioWaveTableList_h_

#include <Arduino.h>

#include "WaveTable.h"
#include "guitar01.h"
#include "kick06.h"
#include "sine256.h"

#define AUDIO_WAVETABLE_SIZE 3

class AudioWaveTableList {
   private:
    WaveTable * tables[AUDIO_WAVETABLE_SIZE];

   public:
    AudioWaveTableList(void) {
        uint16_t pos = 0;
        tables[pos++] = new Sine256();
        tables[pos++] = new Kick06();
        tables[pos++] = new Guitar01();
    }

    WaveTable *getTable(uint16_t pos) {
        return tables[pos];
    }
};

#endif
