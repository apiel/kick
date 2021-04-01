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
            synth.toggleAdsr();
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
            synth.wave.setNextWaveform(direction);
        } else if (knob == 2) {
            synth.wave.setFrequency(direction);
        } else if (knob == 3) {
            synth.wave.setAmplitude(direction);
        } else if (knob == 4) {
            synth.modulation.setModulation(direction);
        } else if (knob == 5) {
            synth.setAttack(direction);
        } else if (knob == 6) {
            synth.setDecay(direction);
        } else if (knob == 7) {
            // synth.setSustain(direction);
        } else if (knob == 8) {
            // synth.setRelease(direction);
        } else if (knob == 11) {
            synth.setFilterFrequency(direction);
        } else if (knob == 12) {
            synth.setFilterResonance(direction);
        } else if (knob == 13) {
            synth.setFilterOctaveControl(direction);
        } else if (knob == 14) {
            synth.setCurrentFilter(direction);
        } else if (knob == 15) {
            if (synth.modulation.currentModulation == MOD_ENV) {
                synth.modulation.setModAttack(direction);
            }
        } else if (knob == 16) {
            if (synth.modulation.currentModulation == MOD_ENV) {
                synth.modulation.setModDecay(direction);
            } else if (synth.modulation.currentModulation == MOD_LFO) {
                synth.modulation.setFrequency(direction);
            }
        } else if (knob == 17) {
            if (synth.modulation.currentModulation == MOD_ENV) {
                // synth.modulation.setModSustain(direction);
            } else if (synth.modulation.currentModulation == MOD_LFO) {
                synth.modulation.setAmplitude(direction);
            }
        } else if (knob == 0) {  // 0 for 18
            if (synth.modulation.currentModulation == MOD_ENV) {
                // synth.modulation.setModRelease(direction);
            } else if (synth.modulation.currentModulation == MOD_LFO) {
                synth.modulation.setNextWaveform(direction);
            }
        }
    }
}

#endif
