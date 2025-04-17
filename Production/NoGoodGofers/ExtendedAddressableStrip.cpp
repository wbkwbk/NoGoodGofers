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
        delayMicroseconds(1);  // Brief pause for signal stability
    }
}

uint32_t ExtendedAddressableStrip::getPixelColor(uint16_t n) {
    return strip()->getPixelColor(n);
}

void ExtendedAddressableStrip::rainbow(int wait) {
    int i, j;
    // 100 Iterationen für ~2 Sekunden bei wait=1
    for (j = 0; j < 100; j++) {
        updatePinState(); // Verwende geschützte Methode aus AddressableStrip
        for (i = 0; i < strip()->numPixels(); i++) {
            updatePinState();
            strip()->setPixelColor(strip()->numPixels() - i, Wheel((i + j * 256 / 100) & 255));
        }
        strip()->show();
        delay(wait * 0); // 20 ms pro Iteration bei wait=1 -> 100 * 20 ms = 2 Sekunden
    }
}

void ExtendedAddressableStrip::rainbowCycle(int wait) {
    int i, j;
    // 100 Iterationen für ~2 Sekunden bei wait=1
    for (j = 0; j < 100; j++) {
        updatePinState(); // Verwende geschützte Methode aus AddressableStrip
        for (i = 0; i < strip()->numPixels(); i++) {
            updatePinState();
            strip()->setPixelColor(strip()->numPixels() - i, 
                Wheel(((i * 256 / strip()->numPixels()) + j * 2 * 256 / 100) & 255));
        }
        strip()->show();
        delay(wait * 0); // 20 ms pro Iteration bei wait=1 -> 100 * 20 ms = 2 Sekunden
    }
}