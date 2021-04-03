#ifndef IO_MIDI_SYNTH_H_
#define IO_MIDI_SYNTH_H_

#include <Arduino.h>

#include "io_audio.h"
#include "io_audio_synth_storage.h"
#include "io_midi_util.h"

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
        } else if (knob == 2) {
            synth.setFrequency(direction);
        } else if (knob == 3) {
            synth.setAmplitude(direction);
        } else if (knob == 4) {
        } else if (knob == 5) {
        } else if (knob == 6) {
        } else if (knob == 7) {
            synth.setAttack(direction);
        } else if (knob == 8) {
            synth.setDecay(direction);
        } else if (knob == 11) {
            synth.setFilterFrequency(direction);
        } else if (knob == 12) {
            synth.setFilterResonance(direction);
        } else if (knob == 13) {
            synth.setFilterOctaveControl(direction);
        } else if (knob == 14) {
            synth.setCurrentFilter(direction);
        } else if (knob == 15) {
        } else if (knob == 16) {
            synth.setModAttack(direction);
        } else if (knob == 17) {
            synth.setModHold(direction);
        } else if (knob == 0) {  // 0 for 18
            synth.setModDecay(direction);
        }
    }
}

#endif
