#ifndef AudioWaveTable_h_
#define AudioWaveTable_h_

#include <Arduino.h>
#include <Audio.h>

#include "sine256.h"
#include "kick06.h"

class AudioWaveTable : public AudioStream {
   public:
    AudioWaveTable(void)
        : AudioStream(2, inputQueueArray) {
        frequency(100.0);
        amplitude(1.0);
        setTable(sine256, WAVETABLE_SINE256_SIZE);
    }

    AudioWaveTable *setTable(const int16_t *wavetablePtr, u_int16_t size) {
        partModulo = size / AUDIO_BLOCK_SAMPLES;
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

    void update(void) {
        audio_block_t *shapedata = receiveReadOnly(1);
        // uint32_t priorphase = phasedata[AUDIO_BLOCK_SAMPLES - 1]; // not used
        uint32_t ph = computePhase();

        // If the amplitude is zero, no output, but phase still increments
        if (magnitude == 0) {
            if (shapedata) release(shapedata);
            return;
        }

        audio_block_t *block = allocate();
        if (!block) {
            if (shapedata) release(shapedata);
            return;
        }
        int16_t *bp = block->data;

        uint32_t addToIndex = part * AUDIO_BLOCK_SAMPLES;
        for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            ph = phasedata[i];
            uint32_t scale = (ph >> 8) & 0xFFFF;
            uint32_t index = (ph >> 24) + addToIndex;
            // Serial.printf("ph %d index %d yo %d\n", ph, index, ph >> 21);
            // int32_t val1 = *(wavetable + index) * scale; // YoTable[index] * scale;
            // int32_t val2 = *(wavetable + index + 1) * (0x10000 - scale); // YoTable[index + 1] * (0x10000 - scale);
            int32_t val1 = sine256[index] * scale;
            int32_t val2 = sine256[index + 1] * (0x10000 - scale);
            *bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
        }
        part = (part + 1) % partModulo;

        applyToneOffset(block->data);
        if (shapedata) release(shapedata);
        transmit(block, 0);
        release(block);
    }

   private:
    audio_block_t *inputQueueArray[2];
    uint32_t phase_accumulator = 0;
    uint32_t phase_increment = 0;
    uint32_t modulation_factor = 32768;
    int32_t magnitude = 0;
    const int16_t *wavetable = NULL;
    uint32_t phasedata[AUDIO_BLOCK_SAMPLES];
    int16_t tone_offset = 0;
    uint8_t modulation_type = 0;
    uint8_t part = 0;
    uint8_t partModulo = 0;

    void applyToneOffset(int16_t *bp) {
        if (tone_offset) {
            int16_t *end = bp + AUDIO_BLOCK_SAMPLES;
            do {
                int32_t val1 = *bp;
                *bp++ = signed_saturate_rshift(val1 + tone_offset, 16, 0);
            } while (bp < end);
        }
    }

    uint32_t computePhase() {
        uint32_t ph = phase_accumulator;
        const uint32_t inc = phase_increment;
        audio_block_t *moddata = receiveReadOnly(0);
        if (moddata) {
            if (modulation_type == 0) {
                ph = computeFrequencyModulation(moddata, inc, ph);
            } else {
                ph = computePhaseModulation(moddata, inc, ph);
            }
            release(moddata);
        } else {
            // No Modulation Input
            for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
                phasedata[i] = ph;
                ph += inc;
            }
        }
        phase_accumulator = ph;
        return ph;
    }

    uint32_t computePhaseModulation(audio_block_t *moddata, const uint32_t inc,
                                    uint32_t ph) {
        // Phase Modulation
        int16_t *bp = moddata->data;
        for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            // more than +/- 180 deg shift by 32 bit overflow of "n"
            uint32_t n = ((uint32_t)(*bp++)) * modulation_factor;
            phasedata[i] = ph + n;
            ph += inc;
        }
        return ph;
    }

    uint32_t computeFrequencyModulation(audio_block_t *moddata,
                                        const uint32_t inc, uint32_t ph) {
        // Frequency Modulation
        int16_t *bp = moddata->data;
        for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            int32_t n =
                (*bp++) * modulation_factor;  // n is # of octaves to mod
            int32_t ipart = n >> 27;          // 4 integer bits
            n &= 0x7FFFFFF;                   // 27 fractional bits

            // exp2 algorithm by Laurent de Soras
            // https://www.musicdsp.org/en/latest/Other/106-fast-exp2-approximation.html
            n = (n + 134217728) << 3;

            n = multiply_32x32_rshift32_rounded(n, n);
            n = multiply_32x32_rshift32_rounded(n, 715827883) << 3;
            n = n + 715827882;

            uint32_t scale = n >> (14 - ipart);
            uint64_t phstep = (uint64_t)inc * scale;
            uint32_t phstep_msw = phstep >> 32;
            if (phstep_msw < 0x7FFE) {
                ph += phstep >> 16;
            } else {
                ph += 0x7FFE0000;
            }
            phasedata[i] = ph;
        }
        return ph;
    }
};

#endif