#ifndef EXTENDEDADDRESSABLESTRIP_H
#define EXTENDEDADDRESSABLESTRIP_H

#include "AddressableStrip.h"

class ExtendedAddressableStrip : public AddressableStrip {
public:
    ExtendedAddressableStrip(int num, int pin, pinduinoPins* pinState);
    void setBrightness(uint8_t newBrightnessValue);
    void show(bool force);
    uint32_t getPixelColor(uint16_t n);
    void rainbow(int wait);
    void rainbowCycleWS2812FX(int wait);
    void rainbowWS2812FX(int wait);  // New method
private:
    uint32_t colorWheel(byte WheelPos);  // Helper for rainbow colors
};

#endif