#ifndef IO_AUDIO_SYNTH_H_
#define IO_AUDIO_SYNTH_H_

#include <Arduino.h>
#include <Audio.h>

#include "audio_dumb.h"
#include "io_util.h"
#include "note.h"

#define FILTER_TYPE_COUNT 3
#define AUDIO_SYNTH_MOD 3

class IO_AudioSynth : public AudioDumb {
   protected:
   public:
    AudioSynthWaveformModulated waveform;
    AudioEffectEnvelope env;
    AudioFilterStateVariable filter;
    AudioSynthWaveformDc dc;
    AudioEffectEnvelope envMod;

    float frequency = 15.0;
    float amplitude = 1.0;

    byte lastNote = 0;

    float attackMs = 10.0;
    float decayMs = 80.0;

    float modAttackMs = 5.0;
    float modHoldMs = 20.0;
    float modDecayMs = 100.0;

    float filterFrequency = 200.0;
    float filterOctaveControl = 1.0;
    float filterResonance = 0.7;
    byte currentFilter = 0;

    AudioConnection* patchCordFilter[FILTER_TYPE_COUNT];
    AudioConnection* patchCordEnvToFilter;
    AudioConnection* patchCordWaveToEnv;
    AudioConnection* patchCordDcToEnvMod;
    AudioConnection* patchCordEnvModToWave;

    IO_AudioSynth() {
        patchCordDcToEnvMod = new AudioConnection(dc, envMod);
        patchCordEnvModToWave = new AudioConnection(envMod, waveform);
        patchCordWaveToEnv = new AudioConnection(waveform, env);
        patchCordEnvToFilter = new AudioConnection(env, filter);
        patchCordFilter[0] = new AudioConnection(filter, 0, *this, 0);
        patchCordFilter[1] = new AudioConnection(filter, 1, *this, 0);
        patchCordFilter[2] = new AudioConnection(filter, 2, *this, 0);

        env.attack(attackMs);
        env.decay(decayMs);
        env.sustain(0.0);
        env.release(0.0);
        env.hold(0.0);
        env.delay(0.0);

        setCurrentFilter(0);
        filter.frequency(filterFrequency);
        filter.resonance(filterResonance);
        filter.octaveControl(filterOctaveControl);

        waveform.frequency(frequency);
        waveform.amplitude(amplitude);
        waveform.begin(WAVEFORM_SINE);

        dc.amplitude(0.5);
        envMod.delay(0.0);
        envMod.attack(modAttackMs);
        envMod.hold(modHoldMs);
        envMod.decay(modDecayMs);
        envMod.sustain(0.0);
        envMod.release(0.0);
    }

    void init() {}

    void setFrequency(int8_t direction) {
        frequency =
            constrain(frequency + direction, 0, AUDIO_SAMPLE_RATE_EXACT / 2);
        waveform.frequency(frequency);
    }

    void setAmplitude(int8_t direction) {
        amplitude = pctAdd(amplitude, direction);
        waveform.amplitude(amplitude);
    }

    void setCurrentFilter(int8_t direction) {
        currentFilter = mod(currentFilter + direction, FILTER_TYPE_COUNT);
        // as only the last connected is the one used
        // https://www.pjrc.com/teensy/td_libs_AudioConnection.html
        patchCordFilter[currentFilter]->connect();
    }

    void setFilterFrequency(int8_t direction) {
        filterFrequency = constrain(filterFrequency + direction, 0,
                                    AUDIO_SAMPLE_RATE_EXACT / 2);
        filter.frequency(filterFrequency);
    }

    void setFilterResonance(int8_t direction) {
        filterResonance =
            constrain(filterResonance + direction * 0.1, 0.7, 5.0);
        filter.resonance(filterResonance);
    }

    void setFilterOctaveControl(int8_t direction) {
        filterOctaveControl =
            constrain(filterOctaveControl + direction * 0.1, 0.0, 7.0);
        filter.octaveControl(filterOctaveControl);
    }

    void setModAttack(int8_t direction) {
        modAttackMs = constrain(modAttackMs + direction, 0, 11880);
        envMod.attack(modAttackMs);
    }

    void setModHold(int8_t direction) {
        modHoldMs = constrain(modHoldMs + direction, 0, 11880);
        envMod.hold(modHoldMs);
    }

    void setModDecay(int8_t direction) {
        modDecayMs = constrain(modDecayMs + direction, 0, 11880);
        envMod.decay(modDecayMs);
    }

    void setAttack(int8_t direction) {
        attackMs = constrain(attackMs + direction, 0, 11880);
        env.attack(attackMs);
    }

    void setDecay(int8_t direction) {
        decayMs = constrain(decayMs + direction, 0, 11880);
        env.decay(decayMs);
    }

    void noteOn() { noteOn(_C4, 127); }

    void noteOn(byte note, byte velocity) {
        lastNote = note;

        float _freq = frequency + NOTE_FREQ[note] - NOTE_FREQ[_C4];
        float _amp = amplitude * velocity / 127;

        waveform.amplitude(_amp);
        waveform.frequency(_freq);
        envMod.noteOn();
        env.noteOn();
    }

    void noteOff() {}
    void noteOff(byte note) {}
};

#endif
