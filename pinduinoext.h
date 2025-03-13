#ifndef PINDUINOEXT_H
#define PINDUINOEXT_H

#include "pinduino.h"
#include "ExtendedAddressableStrip.h"

class pinduinoext : public pinduino {
public:
    // Constructor for Arduino Mega and Nano
    pinduinoext(int num1, int num2, int num3, String arduinoType);

    // Override the adrLED methods to return ExtendedAddressableStrip objects
    ExtendedAddressableStrip* adrLED1();
    ExtendedAddressableStrip* adrLED2();
    ExtendedAddressableStrip* adrLED3();

private:
    // Extended addressable strips
    ExtendedAddressableStrip* extendedALED1;
    ExtendedAddressableStrip* extendedALED2;
    ExtendedAddressableStrip* extendedALED3;

    // Helper function to initialize the strips based on Arduino type
    void initExtendedStrips(int num1, int num2, int num3, String arduinoType);
};

#endif