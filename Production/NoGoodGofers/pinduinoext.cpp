#include "pinduinoext.h"

// Constructor for Arduino Mega and Nano
pinduinoext::pinduinoext(int num1, int num2, int num3, String arduinoType)
    : pinduino(num1, num2, num3, arduinoType) {
    // Initialize the extended addressable strip and DataPort
    initExtendedStrip(num1, arduinoType);
}

// Initialize the extended addressable strip and DataPort
void pinduinoext::initExtendedStrip(int num1, String arduinoType) {
    _pinState = pinState(); // Verwende geerbte pinState()-Methode
    if (arduinoType == "Mega") {
        // Für Arduino Mega, verwende Pin A15
        extendedALED1 = new ExtendedAddressableStrip(num1, 69, _pinState); // A15
        dataPort1 = new DataPort(69); // A15
    } else if (arduinoType == "Nano") {
        // Für Arduino Nano, verwende Pin D12
        extendedALED1 = new ExtendedAddressableStrip(num1, 12, _pinState); // D12
        dataPort1 = new DataPort(12); // D12
    } else {
        Serial.print("Unsupported Arduino board: ");
        Serial.println(arduinoType);
    }
}

// Return the extended addressable strip
ExtendedAddressableStrip* pinduinoext::adrLED1() {
    return extendedALED1;
}

// Return pin state
pinduinoPins* pinduinoext::pinState() {
    return _pinState;
}

// Return DataPort
DataPort* pinduinoext::port1() {
    return dataPort1;
}