#ifndef IO_DISPLAY_KICK_H_
#define IO_DISPLAY_KICK_H_

#include <Adafruit_SSD1306.h>

#include "io_display.h"
#include "io_display_util.h"
#include "io_state.h"

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

void displayKick(Adafruit_SSD1306* d) {
    d->clearDisplay();
    d->setCursor(0, 0);

    d->printf("%s %s\n", kick.waveform.isWave256() ? "#" : "~",
              kick.waveform.waveName);

    if (mcMode) {
        if (kick.modulationOn) {
            addToCursor(d, 0, 4);
            d->println("Modulation level");
            d->printf("%.2f|%.2f|%.2f|%.2f\n", kick.modLevel[0],
                      kick.modLevel[1], kick.modLevel[2], kick.modLevel[3]);
            d->printf("%.2f|%.2f|%.2f|%.2f\n", kick.modLevel[4],
                      kick.modLevel[5], kick.modLevel[6], kick.modLevel[7]);

            addToCursor(d, 0, 4);
            d->println("Modulation ms");
            d->printf("%d|%d|%d|%d\n", (int)kick.modMs[0], (int)kick.modMs[1],
                      (int)kick.modMs[2], (int)kick.modMs[3]);
            d->printf("%d|%d|%d|%d\n", (int)kick.modMs[4], (int)kick.modMs[5],
                      (int)kick.modMs[6], (int)kick.modMs[7]);
        } else {
            d->println("\nNo modulation");
        }
    } else {
        d->printf("%.1fHz %d%%\n", kick.waveform.frequency,
                  (int)(kick.waveform.amplitude * 100));

        d->printf("Start %d EG %d|%d\n", kick.waveform.getStart(),
                  (int)kick.attackMs, (int)kick.decayMs);

        addToCursor(d, 0, 4);
        d->printf("%s %.1fHz %.1f\n", getFilter(kick.currentFilter),
                  kick.filterFrequency, kick.filterResonance);

        d->printf("Dist %d range %d\n", (int)kick.distortion.amount,
                  (int)kick.distortion.range);
        d->printf("Bitcrusher %d\n", (int)kick.xcrushBits);

        d->printf("%s %s\n", kick.modulationOn ? "ModON" : "ModOFF",
                  kick.rectifierOn ? "RectON" : "RectOFF");
    }
}

#endif
