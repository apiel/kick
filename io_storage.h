#ifndef IO_STORAGE_H_
#define IO_STORAGE_H_

#include <Arduino.h>
#include <SD.h>

#define STORAGE_BUFFER_SIZE 32
#define STORAGE_VALUES_SIZE 20
#define FILE_PATH_LEN 255

char storageBuffer[STORAGE_BUFFER_SIZE];
int storageValues[STORAGE_VALUES_SIZE];
char filePath[FILE_PATH_LEN];

void setFilePath(const char* path, byte pos) {
    snprintf(filePath, FILE_PATH_LEN, path, pos);
    Serial.println(filePath);
}

bool assignStorageValues(File* file) {
    char c;
    byte pos = -1;
    byte i = 0;
    while (file->available() && i < STORAGE_VALUES_SIZE) {
        pos++;
        c = file->read();
        storageBuffer[pos] = c;
        if (c == ' ' || c == '\n') {
            storageBuffer[pos] = '\0';
            storageValues[i] = atoi(storageBuffer);
            i++;
            pos = -1;
            if (c == '\n') {
                return true;
            }
        }
    }
    return false;
}

int f2i(float val) { return (int)(val * 100); }
float i2f(int val) { return (float)(val * 0.01); }
int b2i(bool val) { return val ? 1 : 0; }
int i2b(int val) { return val == 1; }

#endif
