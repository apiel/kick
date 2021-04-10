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
    uint32_t startPart = 0;

    AudioWaveTable(void) : AudioStream(2, inputQueueArray) {
        frequency(100.0);
        amplitude(1.0);
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

    AudioWaveTable *frequency(float freq) {
        if (freq < 0.0) {
            freq = 0.0;
        } else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2) {
            freq = AUDIO_SAMPLE_RATE_EXACT / 2;
        }
        // 4294967296 = 2^32 where 2147483647 is max 32 bit number
        // so 2 * 2147483647 = 4294967294
        // 4294967296/44100 = 97391
        // AUDIO_SAMPLE_RATE_EXACT 44100.0 max frequency is 44100 / 2
        phase_increment = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
        // 0x7FFE0000u = 2147352576 more or less the biggest 32 bit number
        if (phase_increment > 0x7FFE0000u) phase_increment = 0x7FFE0000;

        // to remove
        if (freq <= 0.0) {
            yo_phase_increment = 0.0;
        } else {
            yo_phase_increment = freq / yo_baseFreq;
        }
        return this;
    }

    AudioWaveTable *amplitude(float n) {  // 0 to 1.0
        if (n < 0) {
            n = 0;
        } else if (n > 1.0) {
            n = 1.0;
        }
        magnitude = n * 65536.0;
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

    AudioWaveTable *setStartPart(int _startPart) {
        startPart = _startPart < 0 ? 0 : _startPart;
    }

    AudioWaveTable *setStart(uint32_t val) {
        start = val;
        return this;
    }

    void update(void) {
        if (magnitude == 0) {
            yo_phase_accumulator += yo_phase_increment * AUDIO_BLOCK_SAMPLES;
            return;
        }
        audio_block_t *block = allocate();
        if (!block) {
            yo_phase_accumulator += yo_phase_increment * AUDIO_BLOCK_SAMPLES;
            return;
        }

        audio_block_t *moddata = receiveReadOnly(0);

        int16_t *bp = block->data;
        for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            yo_phase_accumulator += yo_phase_increment;
            if ((uint32_t)yo_phase_accumulator > WAVETABLE_SINE512_SIZE) {
                yo_phase_accumulator = 0;
            }

            computeFrequencyModulation(moddata);

            *bp++ = *(wavetable + (uint32_t)yo_phase_accumulator);
        }

        transmit(block, 0);
        release(block);
        if (moddata) release(moddata);
    }

   private:
    audio_block_t *inputQueueArray[2];
    uint32_t phase_increment = 0;
    uint32_t modulation_factor = 32768;
    int32_t magnitude = 0;
    const int16_t *wavetable = NULL;
    uint32_t wavesize = 0;

    float yo_baseFreq = NOTE_FREQ[_C4];
    float yo_phase_accumulator;
    float yo_phase_increment;

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
                yo_phase_accumulator += (phstep >> 16) >> 24;
            } else {
                yo_phase_accumulator += 0x7FFE0000 >> 24;
            }

            // chould we use modulo
            if ((uint32_t)yo_phase_accumulator > wavesize) {
                yo_phase_accumulator = start;
            }
        }
    }
};

#endif