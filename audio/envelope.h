#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#include <Arduino.h>
#include <AudioStream.h>
#include <utility/dspinst.h>

#define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT / 1000.0)

template <byte STATE_COUNT = 3>
class Envelope : public AudioStream {
   public:
    Envelope() : AudioStream(1, inputQueueArray) {
        state = 1;
        state_mult[0] = 0.0;
    }

    void set(byte _state, float level) { set(_state, level, 0.0); }
    void set(byte _state, float level, float milliseconds) {
        state_mult[_state] = constrain(level, 0.0, 1.0) * 1073741824.0;
        state_count[_state] = milliseconds2count(milliseconds);
        if (state_count[_state] == 0) state_count[_state] = 1;
        if (state > 0) {
            if (state_mult[_state] == state_mult[_state - 1]) {
                inc_hires[_state] = 0;
            } else {
                inc_hires[_state] =
                    (state_mult[_state] - state_mult[_state - 1]) /
                    (int32_t)state_count[_state];
            }
        }
    }

    using AudioStream::release;

    void noteOn() {
        state = 0;
        count = 0;
        mult_hires = 0;
    }

    void noteOff() {}

    void update(void) {
        audio_block_t *block;
        uint32_t *p, *end;
        uint32_t sample12, sample34, sample56, sample78, tmp1, tmp2;

        block = receiveWritable();
        if (!block) return;
        if (state > STATE_COUNT) {
            release(block);
            return;
        }
        p = (uint32_t *)(block->data);
        end = p + AUDIO_BLOCK_SAMPLES / 2;

        while (p < end) {
            if (count == 0) {
                state++;
                count = state_count[state];
                if (state > STATE_COUNT) {
                    while (p < end) {
                        *p++ = 0;
                        *p++ = 0;
                        *p++ = 0;
                        *p++ = 0;
                    }
                    break;
                }
            }

            int32_t mult = mult_hires >> 14;
            int32_t inc = inc_hires[state] >> 17;
            // process 8 samples, using only mult and inc (16 bit resolution)
            sample12 = *p++;
            sample34 = *p++;
            sample56 = *p++;
            sample78 = *p++;
            p -= 4;
            mult += inc;
            tmp1 = signed_multiply_32x16b(mult, sample12);
            mult += inc;
            tmp2 = signed_multiply_32x16t(mult, sample12);
            sample12 = pack_16b_16b(tmp2, tmp1);
            mult += inc;
            tmp1 = signed_multiply_32x16b(mult, sample34);
            mult += inc;
            tmp2 = signed_multiply_32x16t(mult, sample34);
            sample34 = pack_16b_16b(tmp2, tmp1);
            mult += inc;
            tmp1 = signed_multiply_32x16b(mult, sample56);
            mult += inc;
            tmp2 = signed_multiply_32x16t(mult, sample56);
            sample56 = pack_16b_16b(tmp2, tmp1);
            mult += inc;
            tmp1 = signed_multiply_32x16b(mult, sample78);
            mult += inc;
            tmp2 = signed_multiply_32x16t(mult, sample78);
            sample78 = pack_16b_16b(tmp2, tmp1);
            *p++ = sample12;
            *p++ = sample34;
            *p++ = sample56;
            *p++ = sample78;
            // adjust the long-term gain using 30 bit resolution (fix #102)
            // https://github.com/PaulStoffregen/Audio/issues/102
            mult_hires += inc_hires[state];
            count--;
        }
        transmit(block);
        release(block);
    }

   private:
    uint16_t milliseconds2count(float milliseconds) {
        if (milliseconds < 0.0) milliseconds = 0.0;
        uint32_t c = ((uint32_t)(milliseconds * SAMPLES_PER_MSEC) + 7) >> 3;
        if (c > 65535) c = 65535;  // allow up to 11.88 seconds
        return c;
    }
    audio_block_t *inputQueueArray[1];
    // state
    uint8_t state;
    uint16_t count;  // how much time remains in this state, in 8 sample units
    int32_t mult_hires;  // attenuation, 0=off, 0x40000000=unity gain
    int32_t inc_hires[STATE_COUNT + 1];  // change mult_hires every 8 samples

    uint16_t state_count[STATE_COUNT + 1];
    int32_t state_mult[STATE_COUNT + 1];
};

#undef SAMPLES_PER_MSEC
#endif