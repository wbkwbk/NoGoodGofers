#ifndef PINDUINOEXT_H
#define PINDUINOEXT_H

#include "pinduino.h"
#include "ExtendedAddressableStrip.h"

class pinduinoext : public pinduino {
public:
    pinduinoext(int num1, int num2, int num3, String arduinoType);
    ExtendedAddressableStrip* adrLED1();
    ExtendedAddressableStrip* adrLED2();
    ExtendedAddressableStrip* adrLED3();

private:
    ExtendedAddressableStrip* extendedALED1;
    ExtendedAddressableStrip* extendedALED2;
    ExtendedAddressableStrip* extendedALED3;
};

#endif
