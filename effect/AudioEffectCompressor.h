// See https://github.com/jcugnoni/TeensyDist/blob/master/distPedalv08b/compressor.h

#ifndef AudioEffectCompressor_h_
#define AudioEffectCompressor_h_

#include <Arduino.h>
#include <Audio.h>
#include <arm_math.h>

class AudioEffectCompressor : public AudioStream {
   private:
    audio_block_t *inputQueueArray[1];
    float prevEnv = 0;
    float prevNoise = 0;
    float noisethr = 0;
    float gain = 1.0;
    float ratio = 4.0;
    float prevGain = 1.;
    float ratioInv = 0.25;
    float thr = 32767 / 8;
    float thrIn = 0;
    float grel, gatt = 0.0;
    float greln, gattn = 0.0;
    float gnoiseRel = 0.999;
    float gnoiseAtt = 0.005;
    bool enabled = true;

   public:
    AudioEffectCompressor(void) : AudioStream(1, inputQueueArray) {
        setGainThreshold(2.0, 0.75 * 32767);
        setAttack(10);
        setRelease(20);
        setNoiseThr(10);
        enabled = true;
        greln = exp(-1. / (44.1 * 5));
        gattn = exp(-1. / (44.1 * 10));
    };
    void update() {
        audio_block_t *block;

        block = AudioStream::receiveReadOnly();
        if (!block) return;

        if (!enabled) {
            AudioStream::transmit(block);
            AudioStream::release(block);
            return;
        }

        // processing
        float envelope = prevEnv;
        float noise = prevNoise;
        float gainSmooth = prevGain;
        float g = 0;
        float xabs = 0;
        for (uint16_t i = 0; i < AUDIO_BLOCK_SAMPLES; i += 2) {
            // compute envelope
            float x = block->data[i];
            xabs = fabs(x);
            // compute envelope
            if (envelope > xabs) {
                envelope -= grel * (envelope - xabs);
            } else {
                envelope += gatt * (xabs - envelope);
            }
            // compute noise lvl
            if (noise > xabs) {
                noise -= greln * (noise - xabs);
            } else {
                noise += gattn * (xabs - noise);
            }
            // compute regularized gain (secant gain)
            g = gainCurve(envelope, noise);
            gainSmooth += 0.01 * (g - gainSmooth);
            // compute output through gain
            block->data[i] = x * g;
        }
        // save for later
        prevEnv = envelope;
        prevNoise = noise;
        prevGain = gainSmooth;
        AudioStream::transmit(block);
        AudioStream::release(block);
    }

    void setNoiseThr(int16_t nthr) { noisethr = nthr / 32767.0; }
    void setThreshold(int16_t thres) {
        thr = thres / 32767.0;
        thrIn = thr / gain;
    }
    void setRatio(float r) {
        ratio = r;
        ratioInv = 1 / ratio;
    }
    void setGainRatio(float g, float r) {
        gain = g;
        ratio = r;
        ratioInv = 1 / ratio;
        thrIn = (ratio - 1) / (ratio * gain - 1);
        thr = gain * thrIn;
    }
    void setGainThreshold(float g, int16_t t) {
        gain = g;
        thr = t;
        thrIn = t / gain;
        ratio = (1 - thrIn) / (1 - thr);
        ratioInv = 1 / ratio;
    }
    void setRelease(float trel) {
        this->grel = expf(-1.0 / (44100.0 * (trel / 1000.0)));
    }
    void setAttack(float tatt) {
        this->gatt = expf(-1.0 / (44100.0 * (tatt / 1000.0)));
    }
    void setGain(float g) {
        this->gain = g;
        this->thrIn = thr / gain;
    }
    void setNoiseRate(float t1) {
        this->gnoiseRel = expf(-1.0 / (44100.0 * (t1 / 1000.0)));
        this->gnoiseAtt = (1.0 - this->gnoiseRel) * 5;
    }
    void enable() { this->enabled = true; }
    void bypass() { this->enabled = false; }

   private:
    inline float gainCurve(float level, float noise) {
        static float localGain = 0.;
        static float noiseGain = 1.0;
        if (noise < noisethr) {  // below noise thr
            if (noiseGain > 0.001) {
                noiseGain *= this->gnoiseRel;
                return noiseGain * this->gain;
            } else {
                return 0.;
            }
        } else if (level < thrIn) {  // higher than noise thr
            if (noiseGain < 0.999) {
                noiseGain += this->gnoiseAtt;
                return noiseGain * this->gain;
            } else {
                noiseGain = 1.0;
                localGain = this->gain;
                return localGain;
            }
        } else {  // larger than noise & threshold
            localGain = (thr + (level - thrIn) * ratioInv) / level;
            return localGain;
        }
    }
};

#endif
