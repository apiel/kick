#ifndef IO_MIDI_DEFAULT_H_
#define IO_MIDI_DEFAULT_H_

#include <Arduino.h>

#include "io_state.h"

bool defaultNoteOnHandler(byte channel, byte note, byte velocity) {
    if (note == 16 || note == 40) {
        toggleGcMode();
        return true;
    } else if (note == 18 || note == 42) {
        setCurrentView(-1);
        return true;
    } else if (note == 19 || note == 43) {
        setCurrentView(1);
        return true;
    }
    return false;
}

#endif
