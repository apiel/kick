#ifndef AudioWaveTable_h_
#define AudioWaveTable_h_

#include <Arduino.h>
#include <Audio.h>

#include "wavetable/guitar01.h"
#include "wavetable/kick06.h"
#include "wavetable/sine256.h"
#include "wavetable/sine512.h"

class AudioWaveTable : public AudioStream {
   public:
    uint32_t start = 0;

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
        // partModulo = size / AUDIO_BLOCK_SAMPLES;
        partModulo = size / (AUDIO_BLOCK_SAMPLES * 2);
        start = 0;
        part = 0;
        wavetable = wavetablePtr;
        return this;
    }

    AudioWaveTable *frequency(float freq) {
        if (freq < 0.0) {
            freq = 0.0;
        } else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2) {
            freq = AUDIO_SAMPLE_RATE_EXACT / 2;
        }
        phase_increment = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
        if (phase_increment > 0x7FFE0000u) phase_increment = 0x7FFE0000;
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

    AudioWaveTable *offset(float n) {
        if (n < -1.0) {
            n = -1.0;
        } else if (n > 1.0) {
            n = 1.0;
        }
        tone_offset = n * 32767.0;
        return this;
    }

    AudioWaveTable *frequencyModulation(float octaves) {
        if (octaves > 12.0) {
            octaves = 12.0;
        } else if (octaves < 0.1) {
            octaves = 0.1;
        }
        modulation_factor = octaves * 4096.0;
        modulation_type = 0;
        return this;
    }

    AudioWaveTable *phaseModulation(float degrees) {
        if (degrees > 9000.0) {
            degrees = 9000.0;
        } else if (degrees < 30.0) {
            degrees = 30.0;
        }
        modulation_factor = degrees * (65536.0 / 180.0);
        modulation_type = 1;
        return this;
    }

    AudioWaveTable *setStart(int _start) {
        start = _start < 0 ? 0 : _start;
    }

    void update(void) {
        if (magnitude == 0) {
            phase_accumulator += phase_increment * AUDIO_BLOCK_SAMPLES;
            return;
        }
        audio_block_t *block = allocate();
        if (!block) {
            phase_accumulator += phase_increment * AUDIO_BLOCK_SAMPLES;
            return;
        }

        audio_block_t *moddata = receiveReadOnly(0);
        assignModulation(moddata);

        int16_t *bp = block->data;

        uint32_t addToIndex = start - 1;
        if (start == 0) {
            addToIndex = part * AUDIO_BLOCK_SAMPLES * 2;
            part = (part + 1) % partModulo;
        }
        for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            uint32_t ph = (this->*ptrComputeModulation)(moddata);
            uint32_t index = (ph >> 24) + addToIndex;
            uint32_t scale = (ph >> 8) & 0xFFFF;
            int32_t val1 = *(wavetable + index) * scale;
            int32_t val2 = *(wavetable + index + 1) * (0x10000 - scale);
            *bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
        }

        applyToneOffset(block->data);
        transmit(block, 0);
        release(block);
        if (moddata) release(moddata);
    }

   private:
    audio_block_t *inputQueueArray[2];
    uint32_t phase_accumulator = 0;
    uint32_t phase_increment = 0;
    uint32_t modulation_factor = 32768;
    int32_t magnitude = 0;
    const int16_t *wavetable = NULL;
    int16_t tone_offset = 0;
    uint8_t modulation_type = 0;
    uint8_t part = 0;
    uint8_t partModulo = 0;
    uint32_t (AudioWaveTable::*ptrComputeModulation)(audio_block_t *moddata);

    void assignModulation(audio_block_t *moddata) {
        if (moddata) {
            if (modulation_type == 0) {
                ptrComputeModulation =
                    &AudioWaveTable::computeFrequencyModulation;
            } else {
                ptrComputeModulation = &AudioWaveTable::computePhaseModulation;
            }
        } else {
            ptrComputeModulation = &AudioWaveTable::computeNoModulation;
        }
    }

    uint32_t computeNoModulation(audio_block_t *notUsed) {
        uint32_t ph = phase_accumulator;
        phase_accumulator += phase_increment;
        return ph;
    }

    uint32_t computePhaseModulation(audio_block_t *moddata) {
        int16_t *bp = moddata->data;
        // more than +/- 180 deg shift by 32 bit overflow of "n"
        uint32_t n = ((uint32_t)(*bp++)) * modulation_factor;
        uint32_t ph = phase_accumulator + n;
        phase_accumulator += phase_increment;
        return ph;
    }

    uint32_t computeFrequencyModulation(audio_block_t *moddata) {
        // Frequency Modulation
        int16_t *bp = moddata->data;
        int32_t n = (*bp++) * modulation_factor;  // n is # of octaves to mod
        int32_t ipart = n >> 27;                  // 4 integer bits
        n &= 0x7FFFFFF;                           // 27 fractional bits

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
            phase_accumulator += phstep >> 16;
        } else {
            phase_accumulator += 0x7FFE0000;
        }
        return phase_accumulator;
    }

    void applyToneOffset(int16_t *bp) {
        if (tone_offset) {
            int16_t *end = bp + AUDIO_BLOCK_SAMPLES;
            do {
                int32_t val1 = *bp;
                *bp++ = signed_saturate_rshift(val1 + tone_offset, 16, 0);
            } while (bp < end);
        }
    }
};

#endif