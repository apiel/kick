#ifndef IO_AUDIO_SYNTH_STORAGE_H_
#define IO_AUDIO_SYNTH_STORAGE_H_

#include <SD.h>

#include "io_audio.h"
#include "io_audio_synth.h"
#include "io_state.h"
#include "io_storage.h"

IO_AudioSynth synth;

void setSynthPath(byte pos) { setFilePath("synth/%03d.io", pos); }

bool loadSynth(byte synthPos) {
    Serial.println("loadSynth.");
    // setSynthPath(synthPos);
    // if (sdAvailable && SD.exists(filePath)) {
    //     File file = SD.open(filePath);
    //     if (file) {
    //         IO_AudioSynth* s = &synth[synthPos];
    //         while (file.available() && assignStorageValues(&file)) {
    //             switch (storageValues[0]) {
    //                 // Osc
    //                 case 0:
    //                     s->wave.currentWaveform = (byte)storageValues[1];
    //                     s->wave.setNextWaveform(0);
    //                     s->wave.frequency = i2f(storageValues[2]);
    //                     s->wave.setFrequency(0);
    //                     s->wave.amplitude = i2f(storageValues[3]);
    //                     s->wave.setAmplitude(0);
    //                     break;

    //                 // Env
    //                 case 10:
    //                     s->useAdsr = i2b(storageValues[1]);
    //                     s->applyFilterCord();
    //                     s->attackMs = i2f(storageValues[2]);
    //                     s->setAttack(0);
    //                     s->decayMs = i2f(storageValues[3]);
    //                     s->setDecay(0);
    //                     s->sustainLevel = i2f(storageValues[4]);
    //                     s->setSustain(0);
    //                     s->releaseMs = i2f(storageValues[5]);
    //                     s->setRelease(0);
    //                     break;

    //                 // Filter
    //                 case 11:
    //                     s->currentFilter = (byte)storageValues[1];
    //                     s->setCurrentFilter(0);
    //                     s->filterFrequency = i2f(storageValues[2]);
    //                     s->setFilterFrequency(0);
    //                     s->filterResonance = i2f(storageValues[3]);
    //                     s->setFilterResonance(0);
    //                     s->filterOctaveControl = i2f(storageValues[4]);
    //                     s->setFilterOctaveControl(0);
    //                     break;

    //                 // Modulation
    //                 case 20:
    //                     s->modulation.currentModulation =
    //                         (byte)storageValues[1];
    //                     s->modulation.applyCord();
    //                     break;

    //                 case 21:
    //                     s->modulation.modAttackMs = i2f(storageValues[1]);
    //                     s->modulation.setModAttack(0);
    //                     s->modulation.modDecayMs = i2f(storageValues[2]);
    //                     s->modulation.setModDecay(0);
    //                     s->modulation.modSustainLevel = i2f(storageValues[3]);
    //                     s->modulation.setModSustain(0);
    //                     s->modulation.modReleaseMs = i2f(storageValues[4]);
    //                     s->modulation.setModRelease(0);
    //                     break;

    //                 case 22:
    //                     s->modulation.lfoFrequency = i2f(storageValues[1]);
    //                     s->modulation.setFrequency(0);
    //                     s->modulation.lfoAmplitude = i2f(storageValues[2]);
    //                     s->modulation.setAmplitude(0);
    //                     s->modulation.lfoWave = (byte)storageValues[3];
    //                     s->modulation.setNextWaveform(0);
    //                     break;

    //                 default:
    //                     break;
    //             }
    //         }
    //         file.close();
    //         return;
    //     }
    // }
    Serial.println("No file found.");
}

bool saveSynth(byte synthPos) {
    Serial.println("saveSynth");
    // if (sdAvailable) {
    //     setSynthPath(synthPos);
    //     File file = SD.open(filePath, FILE_WRITE);

    //     if (file) {
    //         file.seek(0);
    //         IO_AudioSynth* s = &synth[synthPos];

    //         // Osc
    //         sprintf(storageBuffer, "0 %d %d %d\n", (int)s->wave.currentWaveform,
    //                 f2i(s->wave.frequency), f2i(s->wave.amplitude));
    //         file.print(storageBuffer);

    //         // Filters
    //         sprintf(storageBuffer, "10 %d %d %d %d %d\n", b2i(s->useAdsr),
    //                 f2i(s->attackMs), f2i(s->decayMs), f2i(s->sustainLevel),
    //                 f2i(s->releaseMs));
    //         file.print(storageBuffer);

    //         sprintf(storageBuffer, "11 %d %d %d %d\n", (int)s->currentFilter,
    //                 f2i(s->filterFrequency), f2i(s->filterResonance),
    //                 f2i(s->filterOctaveControl));
    //         file.print(storageBuffer);

    //         // modulation
    //         sprintf(storageBuffer, "20 %d\n",
    //                 (int)s->modulation.currentModulation);
    //         file.print(storageBuffer);

    //         sprintf(storageBuffer, "21 %d %d %d %d\n",
    //                 f2i(s->modulation.modAttackMs),
    //                 f2i(s->modulation.modDecayMs),
    //                 f2i(s->modulation.modSustainLevel),
    //                 f2i(s->modulation.modReleaseMs));
    //         file.print(storageBuffer);

    //         sprintf(
    //             storageBuffer, "22 %d %d %d\n", f2i(s->modulation.lfoFrequency),
    //             f2i(s->modulation.lfoAmplitude), (int)s->modulation.lfoWave);
    //         file.print(storageBuffer);

    //         file.close();
    //         return true;
    //     }
    // }
    // Serial.println("- failed to open file for writing");
    return false;
}

#endif
