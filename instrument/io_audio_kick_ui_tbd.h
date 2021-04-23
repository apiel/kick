#ifndef IO_AUDIO_BASS_UI_H_
#define IO_AUDIO_BASS_UI_H_

#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Audio.h>

#include "../io_display_util.h"
#include "../io_midi_util.h"
#include "../io_state.h"
#include "io_audio_bass.h"
#include "io_display_helper.h"

class IO_AudioBassUI : public IO_AudioBass {
   public:
    void display(Adafruit_SSD1306* d) {
        d->clearDisplay();
        d->setCursor(0, 0);

        d->printf("%s %s\n", waveform.isWave256() ? "#" : "~",
                  waveform.waveName);

        if (mcMode) {
            if (modulationOn) {
                addToCursor(d, 0, 4);
                d->println("Modulation level");
                d->printf("%.2f|%.2f|%.2f|%.2f\n", modLevel[0], modLevel[1],
                          modLevel[2], modLevel[3]);
                d->printf("%.2f|%.2f|%.2f|%.2f\n", modLevel[4], modLevel[5],
                          modLevel[6], modLevel[7]);

                addToCursor(d, 0, 4);
                d->println("Modulation ms");
                d->printf("%d|%d|%d|%d\n", (int)modMs[0], (int)modMs[1],
                          (int)modMs[2], (int)modMs[3]);
                d->printf("%d|%d|%d|%d\n", (int)modMs[4], (int)modMs[5],
                          (int)modMs[6], (int)modMs[7]);
            } else {
                d->println("\nNo modulation");
            }
        } else {
            d->printf("%.1fHz %d%%\n", waveform.frequency,
                      (int)(waveform.amplitude * 100));

            d->printf("Start %d EG %d|%d\n", waveform.getStart(), (int)attackMs,
                      (int)decayMs);

            addToCursor(d, 0, 4);
            d->printf("%s %.1fHz %.1f\n", getFilter(currentFilter),
                      filterFrequency, filterResonance);

            d->printf("Dist %d range %d\n", (int)distortion.amount,
                      (int)distortion.range);
            d->printf("Bitcrusher %d\n", (int)xcrushBits);

            d->printf("%s %s\n", modulationOn ? "ModON" : "ModOFF",
                      rectifierOn ? "RectON" : "RectOFF");
        }
    }

    void noteOnHandler(byte channel, byte note, byte velocity) {
        if (channel == 11) {
            byte key = getItemKey(note);
            if (key != 255) {
                // currentkick = key;
            } else if (note == 22 || note == 46) {
                noteOn();
            } else if (note == 23 || note == 47) {
                // savekick(currentkick);
            } else if (note == 20) {
            } else if (note == 17 || note == 41) {
                if (mcMode) {
                    toggleModulation();
                } else {
                    toggleRectifier();
                }
            }
        }
    }

    void noteOffHandler(byte channel, byte note, byte velocity) {
        if (channel == 11) {
            if (note == 22 || note == 46) {
                noteOff();
            }
        }
    }

    void controlChangeHandler(byte channel, byte knob, int8_t direction) {
        if (channel == 11) {
            if (knob == 1) {
                if (mcMode) {
                    setModLevel(0, direction);
                } else {
                    waveform.setNextWaveform(direction);
                }
            } else if (knob == 2) {
                if (mcMode) {
                    setModLevel(1, direction);
                } else {
                    waveform.setFrequency(direction);
                }
            } else if (knob == 3) {
                if (mcMode) {
                    setModLevel(2, direction);
                } else {
                    waveform.setAmplitude(direction);
                }
            } else if (knob == 4) {
                if (mcMode) {
                    setModLevel(3, direction);
                } else {
                }
            } else if (knob == 5) {
                if (mcMode) {
                    setModLevel(4, direction);
                } else {
                }
            } else if (knob == 6) {
                if (mcMode) {
                    setModLevel(5, direction);
                } else {
                    waveform.setStart(direction);
                }
            } else if (knob == 7) {
                if (mcMode) {
                    setModLevel(6, direction);
                } else {
                    setAttack(direction);
                }
            } else if (knob == 8) {
                if (mcMode) {
                    setModLevel(7, direction);
                } else {
                    setDecay(direction);
                }
            } else if (knob == 11) {
                if (mcMode) {
                    setModMs(0, direction);
                } else {
                    setCurrentFilter(direction);
                }
            } else if (knob == 12) {
                if (mcMode) {
                    setModMs(1, direction);
                } else {
                    setFilterFrequency(direction);
                }
            } else if (knob == 13) {
                if (mcMode) {
                    setModMs(2, direction);
                } else {
                    setFilterResonance(direction);
                }
            } else if (knob == 14) {
                if (mcMode) {
                    setModMs(3, direction);
                } else {
                }
            } else if (knob == 15) {
                if (mcMode) {
                    setModMs(4, direction);
                } else {
                }
            } else if (knob == 16) {
                if (mcMode) {
                    setModMs(5, direction);
                } else {
                    setDistortion(direction);
                }
            } else if (knob == 17) {
                if (mcMode) {
                    setModMs(6, direction);
                } else {
                    setDistortionRange(direction);
                }
            } else if (knob == 0) {  // 0 for 18
                if (mcMode) {
                    setModMs(7, direction);
                } else {
                    setBitcrusher(direction);
                }
            }
        }
    }
};

#endif
