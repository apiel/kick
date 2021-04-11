#ifndef AudioWaveTableList_h_
#define AudioWaveTableList_h_

#include <Arduino.h>

#include "WaveTable.h"
#include "guitar01.h"
#include "kick06.h"
#include "sine256.h"

class AudioWaveTableList {
   private:
    WaveTable * tables[1];

   public:
    AudioWaveTableList(void) {
        uint16_t pos = 0;
        tables[pos++] = new Sine256();
    }

    WaveTable *getTable(uint16_t pos) {
        return tables[pos];
    }
};

#endif
