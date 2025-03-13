#include "pinduinoext.h"

pinduinoext::pinduinoext(int num1, int num2, int num3, String arduinoType)
    : pinduino(num1, num2, num3, arduinoType) {
    extendedALED1 = new ExtendedAddressableStrip(num1, 10, pinState());
    extendedALED2 = new ExtendedAddressableStrip(num2, 11, pinState());
    extendedALED3 = new ExtendedAddressableStrip(num3, 0, pinState());
}

ExtendedAddressableStrip* pinduinoext::adrLED1() {
    return extendedALED1;
}

ExtendedAddressableStrip* pinduinoext::adrLED2() {
    return extendedALED2;
}

ExtendedAddressableStrip* pinduinoext::adrLED3() {
    return extendedALED3;
}
