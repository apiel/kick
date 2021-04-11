#ifndef IO_STATE_H_
#define IO_STATE_H_

#include <Arduino.h>

#include "io_util.h"

enum { VIEW_SYNTH, VIEW_COUNT };

IO_AudioKick kick;

byte currentView = VIEW_SYNTH;
bool mcMode = false;
bool sdAvailable = true;

void toggleGcMode() { mcMode = !mcMode; }

// ToDo: when button pressed, possibilty to press 1-8 button to directly
// select the view. Also while it is pressed the 1-8 button should light up
// to display the current selection
void setCurrentView(int8_t direction) {
    currentView = mod(currentView + direction, VIEW_COUNT);
}

#endif
