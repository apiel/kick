#ifndef IO_AUDIO_SYNTH_MODULATION_H_
#define IO_AUDIO_SYNTH_MODULATION_H_

#include <Arduino.h>
#include <Audio.h>

#include "audio_dumb.h"
#include "io_util.h"

#define LFO_WAVEFORM_COUNT 9

enum { MOD_NONE, MOD_ENV, MOD_LFO, MOD_COUNT };

class IO_AudioSynthModulation : public AudioDumb {
   protected:
   public:
    AudioSynthWaveformDc dc;
    AudioEffectEnvelope envMod;
    AudioSynthWaveform lfoMod;

    byte currentModulation = 1;
    float modAttackMs = 100.0;
    float modDecayMs = 50.0;
    float modSustainLevel = 0.7;
    float modReleaseMs = 50.0;

    float lfoFrequency = 1.0;
    float lfoAmplitude = 0.5;
    byte lfoWave = WAVEFORM_SINE;

    AudioConnection* patchCordLfoToWave;
    AudioConnection* patchCordDcToEnvMod;
    AudioConnection* patchCordEnvModToWave;

    IO_AudioSynthModulation() {
        patchCordDcToEnvMod = new AudioConnection(dc, envMod);
        patchCordEnvModToWave = new AudioConnection(envMod, *this);
        patchCordLfoToWave = new AudioConnection(lfoMod, *this);

        applyCord();

        lfoMod.frequency(lfoFrequency);
        lfoMod.amplitude(lfoAmplitude);
        lfoMod.begin(lfoWave);

        dc.amplitude(0.5);
        envMod.delay(0);
        envMod.attack(modAttackMs);
        envMod.hold(0);
        envMod.decay(modDecayMs);
        envMod.sustain(modSustainLevel);
        envMod.release(modReleaseMs);
    }

    void init() { applyCord(); }

    void setModulation(int8_t direction) {
        currentModulation = mod(currentModulation + direction, MOD_COUNT);
        applyCord();
    }

    void applyCord() {
        patchCordLfoToWave->disconnect();
        patchCordDcToEnvMod->disconnect();
        patchCordEnvModToWave->disconnect();
        if (currentModulation == MOD_ENV) {
            patchCordDcToEnvMod->connect();
            patchCordEnvModToWave->connect();
        } else if (currentModulation == MOD_LFO) {
            patchCordLfoToWave->connect();
        }
    }

    void setModAttack(int8_t direction) {
        modAttackMs = constrain(modAttackMs + direction, 0, 11880);
        envMod.attack(modAttackMs);
    }

    void setModDecay(int8_t direction) {
        modDecayMs = constrain(modDecayMs + direction, 0, 11880);
        envMod.decay(modDecayMs);
    }

    void setModSustain(int8_t direction) {
        modSustainLevel = pctAdd(modSustainLevel, direction);
        envMod.sustain(modSustainLevel);
    }

    void setModRelease(int8_t direction) {
        modReleaseMs = constrain(modReleaseMs + direction, 0, 11880);
        envMod.release(modReleaseMs);
    }

    void setNextWaveform(int8_t direction) {
        lfoWave = mod(lfoWave + direction, LFO_WAVEFORM_COUNT);
        lfoMod.begin(lfoWave);
    }

    void setFrequency(int8_t direction) {
        lfoFrequency =
            constrain(lfoFrequency + direction, 0, AUDIO_SAMPLE_RATE_EXACT / 2);
        lfoMod.frequency(lfoFrequency);
    }

    void setAmplitude(int8_t direction) {
        lfoAmplitude = pctAdd(lfoAmplitude, direction);
        lfoMod.amplitude(lfoAmplitude);
    }
};

#endif
