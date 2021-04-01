#ifndef IO_AUDIO_H_
#define IO_AUDIO_H_

#include <Arduino.h>
#include <Audio.h>

#include "io_audio_synth.h"
#include "io_audio_synth_modulation.h"
#include "io_audio_synth_storage.h"
#include "io_audio_wav.h"

AudioOutputMQS audioOut;
AudioConnection patchCordSynth(synth, audioOut);

void audioInit() {
    AudioMemory(25);
    synth.init();
}

#endif
