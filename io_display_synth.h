#ifndef IO_DISPLAY_SYNTH_H_
#define IO_DISPLAY_SYNTH_H_

#include <Adafruit_SSD1306.h>

#include "io_display.h"
#include "io_display_util.h"

const char* getFilter(byte w) {
    switch (w) {
        case 0:
            return "LowP";
        case 1:
            return "BandP";
        case 2:
            return "HighP";
    }
    return "??";
}

void displaySynth(Adafruit_SSD1306* d) {
    d->clearDisplay();
    d->setCursor(0, 0);

    d->printf("%.1fHz %d%%\n", synth.frequency, (int)(synth.amplitude * 100));

    d->printf("EG %d|%d\n", (int)synth.attackMs, (int)synth.decayMs);

    addToCursor(d, 0, 4);
    d->printf("%s %.1fHz %.1f\n", getFilter(synth.currentFilter),
              synth.filterFrequency, synth.filterResonance);

    addToCursor(d, 0, 4);
    d->printf("mod %d|%d|%d\n", (int)synth.modAttackMs, (int)synth.modHoldMs,
              (int)synth.modDecayMs);
}

#endif
