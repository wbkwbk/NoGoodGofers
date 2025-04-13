// ExtendedAddressableStrip.cpp
#include "ExtendedAddressableStrip.h"

ExtendedAddressableStrip::ExtendedAddressableStrip(int num, int pin, pinduinoPins* pinState)
    : AddressableStrip(num, pin, pinState) {}

void ExtendedAddressableStrip::setBrightness(uint8_t newBrightnessValue) {
    strip()->setBrightness(newBrightnessValue);
    strip()->show(); // Apply brightness change
}

void ExtendedAddressableStrip::show() {
    strip()->show(); // Directly call the underlying show() method
}