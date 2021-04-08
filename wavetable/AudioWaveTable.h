#ifndef AudioWaveTable_h_
#define AudioWaveTable_h_

#include <Arduino.h>
#include <Audio.h>

const int16_t YoTable[257] = {
    0,      804,    1608,   2410,   3212,   4011,   4808,   5602,   6393,
    7179,   7962,   8739,   9512,   10278,  11039,  11793,  12539,  13279,
    14010,  14732,  15446,  16151,  16846,  17530,  18204,  18868,  19519,
    20159,  20787,  21403,  22005,  22594,  23170,  23731,  24279,  24811,
    25329,  25832,  26319,  26790,  27245,  27683,  28105,  28510,  28898,
    29268,  29621,  29956,  30273,  30571,  30852,  31113,  31356,  31580,
    31785,  31971,  32137,  32285,  32412,  32521,  32609,  32678,  32728,
    32757,  32767,  32757,  32728,  32678,  32609,  32521,  32412,  32285,
    32137,  31971,  31785,  31580,  31356,  31113,  30852,  30571,  30273,
    29956,  29621,  29268,  28898,  28510,  28105,  27683,  27245,  26790,
    26319,  25832,  25329,  24811,  24279,  23731,  23170,  22594,  22005,
    21403,  20787,  20159,  19519,  18868,  18204,  17530,  16846,  16151,
    15446,  14732,  14010,  13279,  12539,  11793,  11039,  10278,  9512,
    8739,   7962,   7179,   6393,   5602,   4808,   4011,   3212,   2410,
    1608,   804,    0,      -804,   -1608,  -2410,  -3212,  -4011,  -4808,
    -5602,  -6393,  -7179,  -7962,  -8739,  -9512,  -10278, -11039, -11793,
    -12539, -13279, -14010, -14732, -15446, -16151, -16846, -17530, -18204,
    -18868, -19519, -20159, -20787, -21403, -22005, -22594, -23170, -23731,
    -24279, -24811, -25329, -25832, -26319, -26790, -27245, -27683, -28105,
    -28510, -28898, -29268, -29621, -29956, -30273, -30571, -30852, -31113,
    -31356, -31580, -31785, -31971, -32137, -32285, -32412, -32521, -32609,
    -32678, -32728, -32757, -32767, -32757, -32728, -32678, -32609, -32521,
    -32412, -32285, -32137, -31971, -31785, -31580, -31356, -31113, -30852,
    -30571, -30273, -29956, -29621, -29268, -28898, -28510, -28105, -27683,
    -27245, -26790, -26319, -25832, -25329, -24811, -24279, -23731, -23170,
    -22594, -22005, -21403, -20787, -20159, -19519, -18868, -18204, -17530,
    -16846, -16151, -15446, -14732, -14010, -13279, -12539, -11793, -11039,
    -10278, -9512,  -8739,  -7962,  -7179,  -6393,  -5602,  -4808,  -4011,
    -3212,  -2410,  -1608,  -804,   0};

class AudioWaveTable : public AudioStream {
   public:
    AudioWaveTable(void)
        : AudioStream(2, inputQueueArray),
          phase_accumulator(0),
          phase_increment(0),
          modulation_factor(32768),
          magnitude(0),
          arbdata(NULL),
          tone_offset(0),
          modulation_type(0) {
        frequency(100.0);
        amplitude(1.0);
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
    AudioWaveTable *arbitraryWaveform(const int16_t *data) {
        arbdata = data;
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
        uint32_t priorphase = phasedata[AUDIO_BLOCK_SAMPLES - 1];
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

        for (uint32_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            ph = phasedata[i];
            uint32_t index = ph >> 24;
            int32_t val1 = YoTable[index];
            int32_t val2 = YoTable[index + 1];
            uint32_t scale = (ph >> 8) & 0xFFFF;
            val2 *= scale;
            val1 *= 0x10000 - scale;
            *bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
        }

        if (tone_offset) {
            bp = block->data;
            int16_t *end = bp + AUDIO_BLOCK_SAMPLES;
            do {
                int32_t val1 = *bp;
                *bp++ = signed_saturate_rshift(val1 + tone_offset, 16, 0);
            } while (bp < end);
        }
        if (shapedata) release(shapedata);
        transmit(block, 0);
        release(block);
    }

   private:
    audio_block_t *inputQueueArray[2];
    uint32_t phase_accumulator;
    uint32_t phase_increment;
    uint32_t modulation_factor;
    int32_t magnitude;
    const int16_t *arbdata;
    uint32_t phasedata[AUDIO_BLOCK_SAMPLES];
    int16_t tone_offset;
    uint8_t modulation_type;

    uint32_t computePhase() {
        uint32_t ph = phase_accumulator;
        const uint32_t inc = phase_increment;
        audio_block_t *moddata = receiveReadOnly(0);
        if (moddata && modulation_type == 0) {
            ph = computeFrequencyModulation(moddata, inc, ph);
        } else if (moddata) {
            ph = computePhaseModulation(moddata, inc, ph);
        } else {
            // No Modulation Input
            for (uint32_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
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
        for (uint32_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            // more than +/- 180 deg shift by 32 bit overflow of "n"
            uint32_t n = ((uint32_t)(*bp++)) * modulation_factor;
            phasedata[i] = ph + n;
            ph += inc;
        }
        release(moddata);
        return ph;
    }

    uint32_t computeFrequencyModulation(audio_block_t *moddata,
                                        const uint32_t inc, uint32_t ph) {
        // Frequency Modulation
        int16_t *bp = moddata->data;
        for (uint32_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
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
        release(moddata);
        return ph;
    }
};

#endif