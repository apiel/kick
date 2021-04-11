#ifndef IO_AUDIO_H_
#define IO_AUDIO_H_

#include <Arduino.h>
#include <Audio.h>

#include "instrument/io_audio_kick.h"
#include "io_audio_synth_storage.h"

AudioOutputMQS audioOut;
AudioConnection patchCordSynth(synth, audioOut);

void audioInit() {
    AudioMemory(25);
    synth.init();
}

#endif
