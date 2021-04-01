#ifndef IO_AUDIO_SYNTH_WAVE_H_
#define IO_AUDIO_SYNTH_WAVE_H_

#include <Arduino.h>
#include <Audio.h>

#include "AudioSynthWaveTableSD.h"
// #include "AudioSynthWaveTableSD2.h"
#include "arbitraryWaveform.h"
#include "audio_dumb.h"
#include "io_util.h"

// need to make a PR to get waveform count
#define WAVEFORM_COUNT 9

#define WAVETABLE_COUNT 255 - WAVEFORM_COUNT
#define WAVETABLE_NAME_LEN 20
#define WAVETABLE_FOLDER "raw/"

class IO_AudioSynthWaveListRaw {
   protected:
    static IO_AudioSynthWaveListRaw* instance;

    IO_AudioSynthWaveListRaw() { init(); }

    void init() {
        rawWaveCount = 0;

        File root = SD.open(WAVETABLE_FOLDER);
        if (root) {
            while (true) {
                File entry = root.openNextFile();
                if (!entry) {
                    break;
                }
                if (!entry.isDirectory()) {
                    snprintf(wavetableName[rawWaveCount], WAVETABLE_NAME_LEN,
                             entry.name());
                    rawWaveCount++;
                }
                entry.close();
            }
            root.close();
        }
    }

   public:
    static IO_AudioSynthWaveListRaw* getInstance() {
        if (!instance) instance = new IO_AudioSynthWaveListRaw;
        return instance;
    }

    byte rawWaveCount = 0;
    char wavetableName[WAVETABLE_COUNT][WAVETABLE_NAME_LEN];
};

IO_AudioSynthWaveListRaw* IO_AudioSynthWaveListRaw::instance = 0;

class IO_AudioSynthWave : public AudioDumb {
   protected:
   public:
    char wavetableFullPath[WAVETABLE_NAME_LEN + 10];

    AudioSynthWaveformModulated waveform;
    AudioSynthWaveTableSD<> waveTable;
    AudioSynthSimpleDrum simpleDrum;
    AudioDumb input;

    byte currentWaveform = WAVEFORM_SINE;

    float frequency = 440;
    float amplitude = 1.0;

    AudioConnection* patchCordInputToWaveForm;
    AudioConnection* patchCordWaveFormToDumb;

    AudioConnection* patchCordInputToWaveTable;
    AudioConnection* patchCordWaveTableToDumb;

    AudioConnection* patchCordInputToSimpleDrum;
    AudioConnection* patchCordSimpleDrumToDumb;

    IO_AudioSynthWave() {
        patchCordInputToWaveTable = new AudioConnection(input, waveTable);
        patchCordInputToWaveForm = new AudioConnection(input, waveform);
        patchCordInputToSimpleDrum = new AudioConnection(input, simpleDrum);

        patchCordWaveFormToDumb = new AudioConnection(waveform, *this);
        patchCordWaveTableToDumb = new AudioConnection(waveTable, *this);
        patchCordSimpleDrumToDumb = new AudioConnection(simpleDrum, *this);

        applyCord();

        waveform.frequency(frequency);
        waveform.amplitude(amplitude);
        waveform.arbitraryWaveform(arbitraryWaveform, 172.0);
        waveform.begin(currentWaveform);

        waveTable.amplitude(amplitude)->frequency(frequency);

        simpleDrum.frequency(frequency);
        // should attack decay
        // simpleDrum.length(100);
    }

    void applyCord() {
        patchCordInputToWaveTable->disconnect();
        patchCordWaveTableToDumb->disconnect();
        patchCordWaveFormToDumb->disconnect();
        patchCordInputToWaveForm->disconnect();
        patchCordInputToSimpleDrum->disconnect();
        patchCordSimpleDrumToDumb->disconnect();
        if (currentWaveform < WAVEFORM_COUNT) {
            patchCordWaveFormToDumb->connect();
            patchCordInputToWaveForm->connect();
        } else if (currentWaveform == WAVEFORM_COUNT) {
            patchCordInputToSimpleDrum->connect();
            patchCordSimpleDrumToDumb->connect();
        } else {
            patchCordInputToWaveTable->connect();
            patchCordWaveTableToDumb->connect();
        }
    }

    void setFrequency(int8_t direction) {
        frequency =
            constrain(frequency + direction, 0, AUDIO_SAMPLE_RATE_EXACT / 2);
        waveform.frequency(frequency);
        waveTable.frequency(frequency);
        simpleDrum.frequency(frequency);
    }

    void setAmplitude(int8_t direction) {
        amplitude = pctAdd(amplitude, direction);
        waveform.amplitude(amplitude);
        waveTable.amplitude(amplitude);
    }

    void init() {
        IO_AudioSynthWaveListRaw::getInstance();
        applyCord();
    }

    void setNextWaveform(int8_t direction) {
        IO_AudioSynthWaveListRaw* r = r->getInstance();
        currentWaveform =
            mod(currentWaveform + direction, WAVEFORM_COUNT + r->rawWaveCount);
        if (currentWaveform < WAVEFORM_COUNT) {
            waveform.begin(currentWaveform);
        } else {
            sprintf(wavetableFullPath, "%s%s", WAVETABLE_FOLDER,
                    r->wavetableName[currentWaveform - WAVEFORM_COUNT]);
            waveTable.load(wavetableFullPath);
        }
        applyCord();
    }
};

#endif
