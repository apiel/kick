#ifndef IO_MIDI_UTIL_H_
#define IO_MIDI_UTIL_H_

#include <Arduino.h>

#include "io_util.h"

#define KNOB_COUNT 18
#define KNOB_MAX_VALUE 127
// use first time the know is turn to get the current value
#define KNOB_INIT_VALUE 255

byte knobValues[KNOB_COUNT] = {
    KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE,
    KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE,
    KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE,
    KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE, KNOB_INIT_VALUE,
    KNOB_INIT_VALUE, KNOB_INIT_VALUE};

int8_t getKnobDirection(byte knob, byte val) {
    int8_t direction = 0;
    if (knobValues[knob] == KNOB_INIT_VALUE) {
        knobValues[knob] = val;
    } else if (val == 0) {
        direction = -1;
        knobValues[knob] = 0;
    } else if (val == KNOB_MAX_VALUE) {
        direction = 1;
        knobValues[knob] = KNOB_MAX_VALUE;
    } else {
        direction = val - knobValues[knob];
        knobValues[knob] =
            constrain(knobValues[knob] + direction, 0, KNOB_MAX_VALUE);
    }
    return direction;
}

byte getItemKeyA(byte key) {
    if (key >= 8 && key <= 15) {
        return key - 8;
    }
    return 255;
}
byte getItemKeyB(byte key) {
    if (key >= 32 && key <= 39) {
        return key - 32;
    }
    return 255;
}
byte getItemKey(byte key) {
    if (getItemKeyA(key) < 255) {
        return getItemKeyA(key);
    }
    return getItemKeyB(key);
}

#endif
