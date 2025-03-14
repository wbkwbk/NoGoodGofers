#ifndef EXTENDED_ADDRESSABLE_STRIP_H
#define EXTENDED_ADDRESSABLE_STRIP_H

#include "AddressableStrip.h"

class ExtendedAddressableStrip : public AddressableStrip {
public:
    ExtendedAddressableStrip(int num, int pin, pinduinoPins* pinState);
    void setBrightness(uint8_t newBrightnessValue);
};

#endif
