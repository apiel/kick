#ifndef IO_MIDI_CORE_H_
#define IO_MIDI_CORE_H_

#include <Arduino.h>
#include <USBHost_t36.h>

#define MIDI_COUNT 4

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHub hub3(myusb);
MIDIDevice midi[MIDI_COUNT] = MIDIDevice(myusb);

#endif
