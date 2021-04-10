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
        // setTable(sine512, 256);
        // setTable(kick06, WAVETABLE_KICK06_SIZE);
        // setTable(kick06, 256);
        // setTable(guitar01, WAVETABLE_GUITAR06_SIZE);
        // setTable(guitar01, 256);
    }

    AudioWaveTable *setTable(const int16_t *wavetablePtr, u_int16_t size) {
        wavetable = wavetablePtr;
        wavesize = size;
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

    AudioWaveTable *setStart(uint32_t val) {
        start = val;
        return this;
    }

    void update(void) {
        if (amplitude == 0.0) {
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
            if ((uint32_t)phase_accumulator > wavesize) {
                phase_accumulator = start;
            }
            computeFrequencyModulation(moddata);       

            *bp++ = (uint32_t)(*(wavetable + (uint32_t)phase_accumulator) *
                               amplitude);
        }

        transmit(block, 0);
        release(block);
        if (moddata) release(moddata);
    }

   private:
    audio_block_t *inputQueueArray[2];
    uint32_t modulation_factor = 32768;
    float amplitude = 0;
    const int16_t *wavetable = NULL;
    uint32_t wavesize = 0;

    float baseFreq = NOTE_FREQ[_C4];
    float phase_accumulator = 0.0;
    float phase_increment = 0.0;

    void computeFrequencyModulation(audio_block_t *moddata) {
        if (moddata) {
            int16_t *md = moddata->data;
            int32_t n =
                (*md++) * modulation_factor;  // n is # of octaves to mod
            int32_t ipart = n >> 27;          // 4 integer bits
            n &= 0x7FFFFFF;                   // 27 fractional bits

            // exp2 algorithm by Laurent de Soras
            // https://www.musicdsp.org/en/latest/Other/106-fast-exp2-approximation.html
            n = (n + 134217728) << 3;
            n = multiply_32x32_rshift32_rounded(n, n);
            n = multiply_32x32_rshift32_rounded(n, 715827883) << 3;
            n = n + 715827882;

            uint32_t scale = n >> (14 - ipart);
            uint64_t phstep = (uint64_t)phase_increment * scale;
            uint32_t phstep_msw = phstep >> 32;
            if (phstep_msw < 0x7FFE) {
                phase_accumulator += (phstep >> 16) >> 24;
            } else {
                phase_accumulator += 0x7FFE0000 >> 24;
            }

            // chould we use modulo
            if ((uint32_t)phase_accumulator > wavesize) {
                phase_accumulator = start;
            }
        }
    }
};

#endif