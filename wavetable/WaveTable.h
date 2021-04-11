#ifndef WaveTable_h_
#define WaveTable_h_

#include <Arduino.h>

class WaveTable {
   public:
    const int16_t* table;
    uint32_t size;

    WaveTable(uint32_t _size, const int16_t* _table) {
        table = _table;
        size = _size;
    }
};

#endif
