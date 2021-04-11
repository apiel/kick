#ifndef IO_AUDIO_H_
#define IO_AUDIO_H_

#include <Arduino.h>
#include <Audio.h>

#include "instrument/io_audio_kick.h"
#include "io_state.h"

AudioOutputMQS audioOut;
AudioConnection patchCordKick(kick, audioOut);

void audioInit() {
    AudioMemory(25);
    kick.init();
}

#endif
