#ifndef AudioWaveTable_h_
#define AudioWaveTable_h_

#include <Arduino.h>
#include <Audio.h>

#include "../note.h"  // to remove
#include "wavetable/guitar01.h"
#include "wavetable/kick06.h"
#include "wavetable/sine256.h"
#include "wavetable/sine512.h"

class AudioWaveTable : public AudioStream {
   public:
    uint32_t start = 0;

    AudioWaveTable(void) : AudioStream(2, inputQueueArray) {
        setFrequency(100.0);
        setAmplitude(1.0);
        // setTable(sine256, WAVETABLE_SINE256_SIZE);
        setTable(sine512, WAVETABLE_SINE512_SIZE);
        // setTable(kick06, WAVETABLE_KICK06_SIZE);
        // setTable(guitar01, WAVETABLE_GUITAR06_SIZE);
    }

    AudioWaveTable *setTable(const int16_t *wavetablePtr, u_int16_t size) {
        wavetable = wavetablePtr;
        wavesize = size;
        start = 0;
        return this;
    }

    AudioWaveTable *setFrequency(float freq) {
        if (freq <= 0.0) {
            phase_increment = 0.0;
        } else {
            phase_increment = freq / baseFreq;
        }
        return this;
    }

    AudioWaveTable *setAmplitude(float n) {  // 0 to 1.0
        amplitude = constrain(n, 0.0, 1.0);
        return this;
    }

    AudioWaveTable *frequencyModulation(float octaves) {
        if (octaves > 12.0) {
            octaves = 12.0;
        } else if (octaves < 0.1) {
            octaves = 0.1;
        }
        modulation_factor = octaves * 4096.0;
        return this;
    }

    AudioWaveTable *setStart(int val) {
        start = val < 0 ? 0 : val;
        return this;
    }

    void update(void) {
        if (amplitude == 0) {
            phase_accumulator += phase_increment * AUDIO_BLOCK_SAMPLES;
            return;
        }
        audio_block_t *block = allocate();
        if (!block) {
            phase_accumulator += phase_increment * AUDIO_BLOCK_SAMPLES;
            return;
        }

        audio_block_t *moddata = receiveReadOnly(0);

        int16_t *bp = block->data;
        for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            phase_accumulator += phase_increment;
            // chould we use modulo
            if ((uint32_t)phase_accumulator > WAVETABLE_SINE512_SIZE) {
                phase_accumulator = start;
            }
            computeFrequencyModulation(moddata);
            *bp++ = (*(wavetable + (uint32_t)phase_accumulator)) * amplitude;
        }

        transmit(block, 0);
        release(block);
        if (moddata) release(moddata);
    }

   private:
    audio_block_t *inputQueueArray[2];
    uint32_t modulation_factor = 32768;
    int32_t amplitude = 0;
    const int16_t *wavetable = NULL;
    uint32_t wavesize = 0;

    float baseFreq = NOTE_FREQ[_C4];
    float phase_accumulator;
    float phase_increment;

    void computeFrequencyModulation(audio_block_t *moddata) {
        if (moddata) {
            int16_t *md = moddata->data;
            int32_t val = *md++;
            if (val) {
                float n = (float)val / 16384.0;
                // but this is not right maybe need to do 
                // something with octave or freq
                phase_accumulator += n;
                // chould we use modulo
                if ((uint32_t)phase_accumulator > wavesize) {
                    phase_accumulator = start;
                }
            }
        }
    }
};

#endif