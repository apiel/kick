#ifndef IO_DISPLAY_SYNTH_H_
#define IO_DISPLAY_SYNTH_H_

#include <Adafruit_SSD1306.h>

#include "io_display.h"
#include "io_display_util.h"
#include "io_state.h"

const char* getWave(byte w) {
    if (w < WAVEFORM_COUNT) {
        switch (w) {
            case 0:
                return "Sin";
            case 1:
                return "Saw";
            case 2:
                return "Squ";
            case 3:
                return "Tri";
            case 4:
                return "Arb";
            case 5:
                return "Pul";
            case 6:
                return "Rev";
            case 7:
                return "S&H";
            case 8:
                return "Var";
        }
    }
    return "unk";
}

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

    d->printf("%s %.1fHz %d%%\n", getWave(synth.currentWaveform),
              synth.frequency, (int)(synth.amplitude * 100));

    d->printf("EG %d|%d Start %d\n", (int)synth.attackMs, (int)synth.decayMs,
              synth.waveform.startPart - 1);

    addToCursor(d, 0, 2);
    d->printf("%s %.1fHz %.1f\n", getFilter(synth.currentFilter),
              synth.filterFrequency, synth.filterResonance);

    if (mcMode) {
        d->fillRect(126, 28, 2, 36, WHITE);
    } else {
        d->fillRect(126, 0, 2, 44, WHITE);
    }

    addToCursor(d, 0, 2);
    if (!synth.modulationOn) {
        d->println("No modulation\n");
    } else if (mcMode) {
        d->printf("ML%.2f|%.2f|%.2f|%.2f\n", synth.modLevel[0],
                  synth.modLevel[1], synth.modLevel[2], synth.modLevel[3]);
        d->printf("ML%.2f|%.2f|%.2f|%.2f\n", synth.modLevel[4],
                  synth.modLevel[5], synth.modLevel[6], synth.modLevel[7]);
    } else {
        d->printf("modMs %d|%d|%d|%d\n", (int)synth.modMs[0],
                  (int)synth.modMs[1], (int)synth.modMs[2],
                  (int)synth.modMs[3]);
        d->printf("modMs %d|%d|%d|%d\n", (int)synth.modMs[4],
                  (int)synth.modMs[5], (int)synth.modMs[6],
                  (int)synth.modMs[7]);
    }

    addToCursor(d, 0, 2);
    d->printf("Dist %d range %d\n", (int)synth.distortion.amount,
              (int)synth.distortion.range);
    d->printf("Bitcrusher %d %s\n", (int)synth.xcrushBits,
              synth.rectifierOn ? "RectON" : "RectOFF");
}

#endif
