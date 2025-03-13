// Eric Lyons 2018
// Note to self: Play more pinball!
// Interfaced for pinduino shield v0.2 and v0.3
// Uses pinduino library

#include "pinduinoext.h"

int aLEDNum1 = 34; // left ramp
int aLEDNum2 = 0;  // right ramp
int aLEDNum3 = 0;

pinduinoext pd(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

int bg_on = 1; // attract effect
unsigned long timeLastEvent = 0; // time last event was last triggered
int startChaseWaitTime = 20000; // Time before chase lights restart (1000 = 1 sec)
int bgWhiteTime = 50;
String color = "white";

const int potPin = A6; // Potentiometer pin
int brightness = 255;  // Initial brightness

void setup() {
    Serial.begin(115200);
    pd.adrLED1()->clear();
    pd.adrLED2()->clear();
    pd.adrLED3()->clear();
    pd.pinState()->reset();
    pinMode(potPin, INPUT);
}

void loop() {
    readPotentiometer(); // Read and adjust brightness

    if (bg_on) {
        background();
    }

    pd.pinState()->update();
    checkPinStates();

    if (millis() - timeLastEvent > startChaseWaitTime) {
        bg_on = 1;
    }
    
    if (millis() - timeLastEvent > bgWhiteTime && !bg_on) {
        pd.adrLED1()->colorRGB(128, 128, 128);
    }
}

// Function to read potentiometer value and set brightness
void readPotentiometer() {
    int potValue = analogRead(potPin); // Read potentiometer (0-1023)
    brightness = map(potValue, 0, 1023, 0, 255); // Scale to 0-255
    pd.adrLED1()->setBrightness(brightness); // Apply brightness

    Serial.print("Potentiometer: ");
    Serial.print(potValue);
    Serial.print(" | Brightness: ");
    Serial.println(brightness);
}

void checkPinStates() {
    int trigger = 0;

    if (pd.pinState()->J126(12)) {
        pd.adrLED1()->color("blue");
        delay(100);
        trigger = 1;
    }

    if (pd.pinState()->J126(11)) {
        pd.adrLED1()->color("red");
        delay(100);
        trigger = 1;
    }

    if (pd.pinState()->J126(10)) {
        pd.adrLED1()->fadeOut(50);
        pd.adrLED1()->bullet2Color("green", "red", 20, 2, 1);
        trigger = 1;
    }

    if (pd.pinState()->J126(9)) {
        pd.adrLED1()->fadeOut(50);
        pd.adrLED1()->bulletFromPoint2Color("white", "green", 17, 5, 17);
        trigger = 1;
    }

    if (pd.pinState()->J126(7)) {
        pd.adrLED1()->fadeOut(50);
        pd.adrLED1()->bulletFromPoint2Color("green", "white", 17, 5, 17);
        trigger = 1;
    }

    if (pd.pinState()->J126(6)) {
        pd.adrLED1()->color("green");
        delay(50);
        trigger = 1;
    }

    if (pd.pinState()->J126(5)) {
        pd.adrLED1()->color("red");
        delay(50);
        trigger = 1;
    }

    if (pd.pinState()->J126(4)) {
        pd.adrLED1()->color("blue");
        delay(50);
        trigger = 1;
    }

    if (trigger) {
        pd.pinState()->reset();
        trigger = 0;
        bg_on = 0;
        timeLastEvent = millis();
    }
}

void background() {
    pd.adrLED1()->sparkle(color, 20);

    if (random(1000) == 0) {
        if (color == "white") color = "green";
        else if (color == "green") color = "blue";
        else color = "white";
    }
}
