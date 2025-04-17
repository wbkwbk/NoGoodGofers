#ifndef PINDUINOEXT_H
#define PINDUINOEXT_H

#include <Arduino.h>
#include "pinduino.h"
#include "ExtendedAddressableStrip.h"
#include "DataPort.h"

class pinduinoext : public pinduino {
public:
    // Constructor for Arduino Mega and Nano
    pinduinoext(int num1, int num2, int num3, String arduinoType);

    // Methoden für LED-Strip und DataPort
    ExtendedAddressableStrip* adrLED1();
    pinduinoPins* pinState();
    DataPort* port1();

private:
    // Extended addressable strip
    ExtendedAddressableStrip* extendedALED1;
    // DataPort
    DataPort* dataPort1;
    // Pin state
    pinduinoPins* _pinState;
    // Helper function to initialize the strip and port
    void initExtendedStrip(int num1, String arduinoType);
};

#endif