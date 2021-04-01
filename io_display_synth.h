#ifndef IO_DISPLAY_SYNTH_H_
#define IO_DISPLAY_SYNTH_H_

#include <Adafruit_SSD1306.h>

#include "io_display.h"
#include "io_display_util.h"

const char* getWave(byte w) {
    if (w < WAVEFORM_COUNT) {
        switch (w) {
            case 0:
                return "Sine";
            case 1:
                return "Sawtooth";
            case 2:
                return "Square";
            case 3:
                return "Triangle";
            case 4:
                return "Arbitrary";
            case 5:
                return "Pulse";
            case 6:
                return "Saw reverse";
            case 7:
                return "Sample hold";
            case 8:
                return "Tri var";
        }
    }
    IO_AudioSynthWaveListRaw* r = r->getInstance();
    w = w - WAVEFORM_COUNT;
    if (w < r->rawWaveCount) {
        return r->wavetableName[w];
    }
    return "unknown";
}

const char* getFilter(byte w) {
    switch (w) {
        case 0:
            return "LowPass";
        case 1:
            return "BandPass";
        case 2:
            return "HighPass";
    }
    return "unknown";
}

void displaySynth(Adafruit_SSD1306* d) {
    d->clearDisplay();
    d->setCursor(0, 0);

    d->printf("%s\n", getWave(synth.wave.currentWaveform));

    d->printf("%.1fHz %d%%\n", synth.wave.frequency,
              (int)(synth.wave.amplitude * 100));

    if (synth.useAdsr) {
        d->printf("ADSR %d|%d|%d%%|%d\n", (int)synth.attackMs,
                  (int)synth.decayMs, (int)(synth.sustainLevel * 100),
                  (int)synth.releaseMs);
    } else {
        d->println("ADSR off");
    }

    addToCursor(d, 0, 4);
    d->printf("%s %.1fHz\n", getFilter(synth.currentFilter),
              synth.filterFrequency);
    d->printf("Reso %.1f Octave %.1f\n", synth.filterResonance,
              synth.filterOctaveControl);

    addToCursor(d, 0, 4);
    if (synth.modulation.currentModulation == MOD_ENV) {
        d->printf("mod %d|%d|%d%%|%d\n", (int)synth.modulation.modAttackMs,
                  (int)synth.modulation.modDecayMs,
                  (int)(synth.modulation.modSustainLevel * 100),
                  (int)synth.modulation.modReleaseMs);
    } else if (synth.modulation.currentModulation == MOD_LFO) {
        d->printf("Lfo %.1fHz %d%% %s\n", synth.modulation.lfoFrequency,
                  (int)(synth.modulation.lfoAmplitude * 100),
                  getWave(synth.modulation.lfoWave));
    }
}

#endif
