#ifndef IO_MIDI_SYNTH_H_
#define IO_MIDI_SYNTH_H_

#include <Arduino.h>

#include "io_audio.h"
#include "io_midi_util.h"
#include "io_state.h"

void synthNoteOnHandler(byte channel, byte note, byte velocity) {
    if (channel == 11) {
        byte key = getItemKey(note);
        if (key != 255) {
            // currentSynth = key;
        } else if (note == 22 || note == 46) {
            kick.noteOn();
        } else if (note == 23 || note == 47) {
            // saveSynth(currentSynth);
        } else if (note == 20) {
        } else if (note == 17 || note == 41) {
            if (mcMode) {
                kick.toggleModulation();
            } else {
                kick.toggleRectifier();
            }
        }
    }
}

void synthNoteOffHandler(byte channel, byte note, byte velocity) {
    if (channel == 11) {
        if (note == 22 || note == 46) {
            kick.noteOff();
        }
    }
}

void synthControlChangeHandler(byte channel, byte knob, int8_t direction) {
    if (channel == 11) {
        if (knob == 1) {
            if (mcMode) {
                kick.setModLevel(0, direction);
            } else {
                kick.waveform.setNextWaveform(direction);
            }
        } else if (knob == 2) {
            if (mcMode) {
                kick.setModLevel(1, direction);
            } else {
                kick.waveform.setFrequency(direction);
            }
        } else if (knob == 3) {
            if (mcMode) {
                kick.setModLevel(2, direction);
            } else {
                kick.waveform.setAmplitude(direction);
            }
        } else if (knob == 4) {
            if (mcMode) {
                kick.setModLevel(3, direction);
            } else {
            }
        } else if (knob == 5) {
            if (mcMode) {
                kick.setModLevel(4, direction);
            } else {
            }
        } else if (knob == 6) {
            if (mcMode) {
                kick.setModLevel(5, direction);
            } else {
                kick.waveform.setStart(direction);
            }
        } else if (knob == 7) {
            if (mcMode) {
                kick.setModLevel(6, direction);
            } else {
                kick.setAttack(direction);
            }
        } else if (knob == 8) {
            if (mcMode) {
                kick.setModLevel(7, direction);
            } else {
                kick.setDecay(direction);
            }
        } else if (knob == 11) {
            if (mcMode) {
                kick.setModMs(0, direction);
            } else {
                kick.setCurrentFilter(direction);
            }
        } else if (knob == 12) {
            if (mcMode) {
                kick.setModMs(1, direction);
            } else {
                kick.setFilterFrequency(direction);
            }
        } else if (knob == 13) {
            if (mcMode) {
                kick.setModMs(2, direction);
            } else {
                kick.setFilterResonance(direction);
            }
        } else if (knob == 14) {
            if (mcMode) {
                kick.setModMs(3, direction);
            } else {
            }
        } else if (knob == 15) {
            if (mcMode) {
                kick.setModMs(4, direction);
            } else {
            }
        } else if (knob == 16) {
            if (mcMode) {
                kick.setModMs(5, direction);
            } else {
                kick.setDistortion(direction);
            }
        } else if (knob == 17) {
            if (mcMode) {
                kick.setModMs(6, direction);
            } else {
                kick.setDistortionRange(direction);
            }
        } else if (knob == 0) {  // 0 for 18
            if (mcMode) {
                kick.setModMs(7, direction);
            } else {
                kick.setBitcrusher(direction);
            }
        }
    }
}

#endif
