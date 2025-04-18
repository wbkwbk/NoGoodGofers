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

// Helper function for rainbow color calculation (ported from WS2812FX)
uint32_t ExtendedAddressableStrip::colorWheel(byte WheelPos) {
    if (WheelPos < 85) {
        return strip()->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return strip()->Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return strip()->Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

void ExtendedAddressableStrip::rainbow(int wait) {
    int i, j;
    // 100 iterations for ~2 seconds at wait=1
    for (j = 0; j < 200; j++) {
        updatePinState();
        for (i = 0; i < strip()->numPixels(); i++) {
            updatePinState();
            strip()->setPixelColor(strip()->numPixels() - i, Wheel((i + j * 256 / 100) & 255));
        }
        strip()->show();
        delay(wait * 10); // 20ms per iteration, scaled by wait
    }
}

void ExtendedAddressableStrip::rainbowCycleWS2812FX(int wait) {
    int i, j;
    // 100 iterations for ~2 seconds at wait=1
    for (j = 0; j < 100; j++) {
        updatePinState();
        for (i = 0; i < strip()->numPixels(); i++) {
            updatePinState();
            strip()->setPixelColor(strip()->numPixels() - i, 
                Wheel(((i * 256 / strip()->numPixels()) + j * 2 * 256 / 100) & 255));
        }
        strip()->show();
        delay(wait * 15); // 20ms per iteration, scaled by wait
    }
}

void ExtendedAddressableStrip::rainbowWS2812FX(int wait) {
    const int FRAME_COUNT = 100;  // 100 frames at 20ms = 2 seconds
    const int FRAME_DELAY = 20;   // 20ms per frame
    int i, j;

    // Animate a moving rainbow for 2 seconds
    for (j = 0; j < FRAME_COUNT; j++) {
        updatePinState();
        for (i = 0; i < strip()->numPixels(); i++) {
            // Calculate color: base position + phase shift
            byte wheelPos = ((i * 256 / strip()->numPixels()) + (j * 256 / FRAME_COUNT)) & 0xFF;
            strip()->setPixelColor(i, colorWheel(wheelPos));
        }
        show(true);  // Force show to update the strip
        delay(FRAME_DELAY);  // 20ms delay per frame
    }
}