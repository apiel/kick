#ifndef IO_MIDI_SYNTH_H_
#define IO_MIDI_SYNTH_H_

#include <Arduino.h>

#include "io_audio.h"
#include "io_audio_synth_storage.h"
#include "io_midi_util.h"
#include "io_state.h"

void synthNoteOnHandler(byte channel, byte note, byte velocity) {
    if (channel == 11) {
        byte key = getItemKey(note);
        if (key != 255) {
            // currentSynth = key;
        } else if (note == 22 || note == 46) {
            synth.noteOn();
        } else if (note == 23 || note == 47) {
            // saveSynth(currentSynth);
        } else if (note == 20) {
        } else if (note == 17 || note == 41) {
            if (mcMode) {
                synth.toggleRectifier();
            } else {
                synth.toggleModulation();
            }
        }
    }
}

void synthNoteOffHandler(byte channel, byte note, byte velocity) {
    if (channel == 11) {
        if (note == 22 || note == 46) {
            synth.noteOff();
        }
    }
}

void synthControlChangeHandler(byte channel, byte knob, int8_t direction) {
    if (channel == 11) {
        if (knob == 1) {
            if (mcMode) {
                synth.setBitcrusher(direction);
            } else {
                synth.waveform.setNextWaveform(direction);
            }
        } else if (knob == 2) {
            if (mcMode) {
                synth.setDistortion(direction);
            } else {
                synth.waveform.setFrequency(direction);
            }
        } else if (knob == 3) {
            if (mcMode) {
                synth.setDistortionRange(direction);
            } else {
                synth.waveform.setAmplitude(direction);
            }
        } else if (knob == 4) {
            if (mcMode) {
                synth.waveform.setStart(direction);
            } else {
                synth.setCurrentFilter(direction);
            }
        } else if (knob == 5) {
            synth.setFilterFrequency(direction);
        } else if (knob == 6) {
            synth.setFilterResonance(direction);
        } else if (knob == 7) {
            synth.setAttack(direction);
        } else if (knob == 8) {
            synth.setDecay(direction);
        } else if (knob == 11) {
            if (mcMode) {
                synth.setModLevel(0, direction);
            } else {
                synth.setModMs(0, direction);
            }
        } else if (knob == 12) {
            if (mcMode) {
                synth.setModLevel(1, direction);
            } else {
                synth.setModMs(1, direction);
            }
        } else if (knob == 13) {
            if (mcMode) {
                synth.setModLevel(2, direction);
            } else {
                synth.setModMs(2, direction);
            }
        } else if (knob == 14) {
            if (mcMode) {
                synth.setModLevel(3, direction);
            } else {
                synth.setModMs(3, direction);
            }
        } else if (knob == 15) {
            if (mcMode) {
                synth.setModLevel(4, direction);
            } else {
                synth.setModMs(4, direction);
            }
        } else if (knob == 16) {
            if (mcMode) {
                synth.setModLevel(5, direction);
            } else {
                synth.setModMs(5, direction);
            }
        } else if (knob == 17) {
            if (mcMode) {
                synth.setModLevel(6, direction);
            } else {
                synth.setModMs(6, direction);
            }
        } else if (knob == 0) {  // 0 for 18
            if (mcMode) {
                synth.setModLevel(7, direction);
            } else {
                synth.setModMs(7, direction);
            }
        }
    }
}

#endif
