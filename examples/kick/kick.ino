#include <io_kick.h>

void setup() {
    Serial.begin(115200);
    ioSynthInit();
}

void loop() {
    ioSynthLoop();
}
