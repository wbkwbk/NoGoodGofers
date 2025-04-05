#include "pinduinoext.h"

// Constructor for Arduino Mega and Nano
pinduinoext::pinduinoext(int num1, int num2, int num3, String arduinoType)
    : pinduino(num1, num2, num3, arduinoType) {
    // Initialize the extended addressable strips based on the Arduino type
    initExtendedStrips(num1, num2, num3, arduinoType);
}

// Initialize the extended addressable strips
void pinduinoext::initExtendedStrips(int num1, int num2, int num3, String arduinoType) {
    if (arduinoType == "Mega") {
        // For Arduino Mega, use analog pins A15, A14, A13
        extendedALED1 = new ExtendedAddressableStrip(num1, 69, pinState()); // A15
        extendedALED2 = new ExtendedAddressableStrip(num2, 68, pinState()); // A14
        extendedALED3 = new ExtendedAddressableStrip(num3, 67, pinState()); // A13
    } else if (arduinoType == "Nano") {
        // For Arduino Nano, use digital pins 10, 11, and 0
        extendedALED1 = new ExtendedAddressableStrip(num1, 10, pinState());
        extendedALED2 = new ExtendedAddressableStrip(num2, 11, pinState());
        extendedALED3 = new ExtendedAddressableStrip(num3, 0, pinState());
    } else {
        Serial.print("Unsupported Arduino board: ");
        Serial.println(arduinoType);
    }
}

// Return the extended addressable strips
ExtendedAddressableStrip* pinduinoext::adrLED1() {
    return extendedALED1;
}

ExtendedAddressableStrip* pinduinoext::adrLED2() {
    return extendedALED2;
}

ExtendedAddressableStrip* pinduinoext::adrLED3() {
    return extendedALED3;
}