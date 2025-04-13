// ExtendedAddressableStrip.cpp
#include "ExtendedAddressableStrip.h"

ExtendedAddressableStrip::ExtendedAddressableStrip(int num, int pin, pinduinoPins* pinState)
    : AddressableStrip(num, pin, pinState) {}

void ExtendedAddressableStrip::setBrightness(uint8_t newBrightnessValue) {
    strip()->setBrightness(newBrightnessValue);
    show(true);  // Force show when changing brightness
}

void ExtendedAddressableStrip::show(bool force) {
    if (force || strip()->canShow()) {
        noInterrupts();
        strip()->show();
        interrupts();
        delayMicroseconds(300);  // Brief pause for signal stability
    }
}

uint32_t ExtendedAddressableStrip::getPixelColor(uint16_t n) {
    return strip()->getPixelColor(n);
}