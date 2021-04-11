#ifndef WaveTable_h_
#define WaveTable_h_

#include <Arduino.h>

class WaveTable {
   public:
    const int16_t* table;
    uint32_t size;
    const char * name;

    WaveTable(const char * _name, uint32_t _size, const int16_t* _table) {
        table = _table;
        size = _size;
        name = _name;
    }
};

#endif
