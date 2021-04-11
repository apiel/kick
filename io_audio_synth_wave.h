#ifndef IO_AUDIO_SYNTH_WAVE_H_
#define IO_AUDIO_SYNTH_WAVE_H_

#include <Arduino.h>
#include <Audio.h>

#include "audio_dumb.h"
#include "io_util.h"
#include "note.h"
#include "wavetable/AudioWaveTable256.h"
#include "wavetable/AudioWaveTableBig.h"
#include "wavetable/AudioWaveTableList.h"

class IO_AudioSynthWave : public AudioDumb {
   public:
    AudioWaveTableBig waveBig;
    AudioWaveTable256 wave256;
    AudioWaveTableList waveList;
    AudioDumb input;

    uint16_t currentWave = 0;

    float frequency = NOTE_FREQ[_C4];
    float amplitude = 1.0;

    AudioConnection* patchCordInputToWaveBig;
    AudioConnection* patchCordWaveBigToDumb;

    AudioConnection* patchCordInputToWave256;
    AudioConnection* patchCordWave256ToDumb;

    IO_AudioSynthWave() {
        patchCordInputToWaveBig = new AudioConnection(input, waveBig);
        patchCordWaveBigToDumb = new AudioConnection(waveBig, *this);

        patchCordInputToWave256 = new AudioConnection(input, wave256);
        patchCordWave256ToDumb = new AudioConnection(wave256, *this);

        setFrequency(0);
        setAmplitude(0);
        setNextWaveform(0);
    }

    IO_AudioSynthWave(float freq) {
        frequency = freq;
        IO_AudioSynthWave();
    }

    bool isWave256() { return currentWave == 0; }

    IO_AudioSynthWave* setStart(int8_t direction) {
        if (isWave256()) {
            wave256.setStart(wave256.start + direction);
        } else {
            waveBig.setStart(waveBig.start + direction);
        }
        return this;
    }

    uint32_t getStart() { return isWave256() ? wave256.start : waveBig.start; }

    IO_AudioSynthWave* setFrequency(int8_t direction) {
        frequency =
            constrain(frequency + direction, 0, AUDIO_SAMPLE_RATE_EXACT / 2);
        wave256.setFrequency(frequency);
        waveBig.setFrequency(frequency);
        return this;
    }

    IO_AudioSynthWave* setAmplitude(int8_t direction) {
        amplitude = pctAdd(amplitude, direction);
        wave256.setAmplitude(amplitude);
        waveBig.setAmplitude(amplitude);
        return this;
    }

    IO_AudioSynthWave* setNextWaveform(int8_t direction) {
        currentWave = mod(currentWave + direction, 2);
        if (currentWave == 0) {
            wave256.setTable(waveList.getTable(0)->table,
                             waveList.getTable(0)->size);
        } else {
            waveBig.setTable(waveList.getTable(0)->table,
                             waveList.getTable(0)->size);
        }
        applyCord();
        return this;
    }

   private:
    void applyCord() {
        // will use something else
        if (isWave256()) {
            patchCordInputToWaveBig->disconnect();
            patchCordWaveBigToDumb->disconnect();
            patchCordInputToWave256->connect();
            patchCordWave256ToDumb->connect();
        } else {
            patchCordInputToWave256->disconnect();
            patchCordWave256ToDumb->disconnect();
            patchCordInputToWaveBig->connect();
            patchCordWaveBigToDumb->connect();
        }
    }
};

#endif
