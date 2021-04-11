#ifndef AUDIO_DUMB_H_
#define AUDIO_DUMB_H_

#include <Arduino.h>
#include <AudioStream.h>

class AudioDumb : public AudioStream {
   public:
    AudioDumb(void) : AudioStream(1, inputQueueArray) {}
    virtual void update(void) {
        audio_block_t *block = receiveReadOnly();
        if (!block) return;
        transmit(block);
        release(block);
    }

   protected:
    audio_block_t *inputQueueArray[1];
};

#endif
