#ifndef IO_DISPLAY_SYNTH_H_
#define IO_DISPLAY_SYNTH_H_

#include <Adafruit_SSD1306.h>

#include "io_display.h"
#include "io_display_util.h"

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

    d->printf("%.1fHz %d%%\n", synth.frequency, (int)(synth.amplitude * 100));

    d->printf("ADSR %d|%d\n", (int)synth.attackMs, (int)synth.decayMs);

    addToCursor(d, 0, 4);
    d->printf("%s %.1fHz\n", getFilter(synth.currentFilter),
              synth.filterFrequency);
    d->printf("Reso %.1f Octave %.1f\n", synth.filterResonance,
              synth.filterOctaveControl);

    addToCursor(d, 0, 4);
    d->printf("mod %d|%d|%d\n", (int)synth.modAttackMs, (int)synth.modHoldMs,
              (int)synth.modDecayMs);
}

#endif
