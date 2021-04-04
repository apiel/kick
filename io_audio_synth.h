#ifndef IO_AUDIO_SYNTH_H_
#define IO_AUDIO_SYNTH_H_

#include <Arduino.h>
#include <Audio.h>

#include "audio_dumb.h"
#include "envelope.h"
#include "io_util.h"
#include "note.h"

#define FILTER_TYPE_COUNT 3
#define AUDIO_SYNTH_MOD 3
#define MOD_ENV_SIZE 8

class IO_AudioSynth : public AudioDumb {
   protected:
   public:
    AudioSynthWaveformModulated waveform;
    Envelope<2> env;
    AudioFilterStateVariable filter;
    AudioSynthWaveformDc dc;
    Envelope<8> envMod;

    float frequency = 15.0;
    float amplitude = 1.0;

    byte lastNote = 0;

    float attackMs = 10.0;
    float decayMs = 80.0;

    float modMs[MOD_ENV_SIZE] = {5.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 5.0};
    float modLevel[MOD_ENV_SIZE] = {1.0, 0.8, 0.6, 0.4, 0.3, 0.2, 0.1, 0.0};

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

        env.set(1, 1.0, attackMs);
        env.set(2, 0.0, decayMs);

        setCurrentFilter(0);
        filter.frequency(filterFrequency);
        filter.resonance(filterResonance);
        filter.octaveControl(filterOctaveControl);

        waveform.frequency(frequency);
        waveform.amplitude(amplitude);
        waveform.begin(WAVEFORM_SINE);

        dc.amplitude(0.5);

        for (byte n = 0; n < MOD_ENV_SIZE; n++) {
            envMod.set(n + 1, modLevel[n], modMs[n]);
        }
    }

    void init() {}

    void setModMs(byte state, int8_t direction) {
        modMs[state] = constrain(modMs[state] + direction, 0.0, 11880.0);
        envMod.set(state + 1, modLevel[state], modMs[state]);
    }

    void setModLevel(byte state, int8_t direction) {
        modLevel[state] =
            constrain(modLevel[state] + direction * 0.01, 0.0, 1.0);
        envMod.set(state + 1, modLevel[state], modMs[state]);
    }

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

    void setAttack(int8_t direction) {
        attackMs = constrain(attackMs + direction, 0, 11880);
        // env.attack(attackMs);
        env.set(1, 1.0, attackMs);
    }

    void setDecay(int8_t direction) {
        decayMs = constrain(decayMs + direction, 0, 11880);
        // env.decay(decayMs);
        env.set(2, 0.0, decayMs);
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
