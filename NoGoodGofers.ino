// Eric Lyons 2018
// Note to self: Play more pinball!
// Interfaced for pinduino shield v0.2 and v0.3
// Uses pinduino library

#include "pinduinoext.h"

#ifdef DEBUG == 1
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define DELAYTIME 100
#define FADETOWHITETIME 50

int aLEDNum1 = 80; // left ramp
int aLEDNum2 = 0;  // right ramp
int aLEDNum3 = 0;
unsigned long lastTriggerTime = 0;
bool gameActive = false;
unsigned long lastEffectEndTime = 0;
const unsigned long effectCooldown = 1000; // Time after effect before returning to white
enum LEDState { ATTRACT, EFFECT_ACTIVE, FADING_TO_WHITE, WHITE_IDLE };
LEDState ledState = ATTRACT;
unsigned long stateChangeTime = 0;



pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

unsigned long timeLastEvent = 0; // time last event was last triggered
int waitUntilStateChangeToAttractMode = 20000; // Time before chase lights restart (1000 = 1 sec)
int bgWhiteTime = 50;
String color = "white";

const int potPin = A6; // Potentiometer pin
int brightness = 250;  // Initial brightness
int currentbrightness = 250;
int brightnesschangethreshold = 5;

void setup() {
    Serial.begin(115200);
    gameActive = false;
    nggPinduno.adrLED1()->clear();
    nggPinduno.adrLED2()->clear();
    nggPinduno.adrLED3()->clear();
    nggPinduno.pinState()->reset();
    pinMode(potPin, INPUT);
    nggPinduno.adrLED1()->setBrightness(currentbrightness);
}


void loop() {
    readPotentiometer();
    nggPinduno.pinState()->update();
    bool hadTrigger = checkPinStates();

    switch (ledState) {
        case ATTRACT:
            background();
            if (hadTrigger) {
                ledState = FADING_TO_WHITE;
                stateChangeTime = millis();
                nggPinduno.adrLED1()->fadeInRGB(128, 128, 128, FADETOWHITETIME);
                gameActive = true;
            }
            break;

        case FADING_TO_WHITE:
            if (millis() - stateChangeTime >= FADETOWHITETIME) { // Wait for fade to complete
                ledState = WHITE_IDLE;
            }
            break;

        case WHITE_IDLE:
            if (hadTrigger) {
                ledState = EFFECT_ACTIVE;
            } else if (millis() - timeLastEvent > waitUntilStateChangeToAttractMode) {
                ledState = ATTRACT;
                nggPinduno.adrLED1()->fadeOut(500);
                gameActive = false;
            }
            break;

        case EFFECT_ACTIVE:
            if (!hadTrigger && (millis() - lastEffectEndTime > effectCooldown)) {
                ledState = FADING_TO_WHITE;
                stateChangeTime = millis();
                nggPinduno.adrLED1()->fadeInRGB(128, 128, 128, 500);
            }
            break;
    }
}



bool checkPinStates() {
    static int trigger = 0;

    if (isDelayOver()) { // Ensure DELAYTIMEms delay between checks
        if (nggPinduno.pinState()->J126(12)) {
            nggPinduno.adrLED1()->color("blue");
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(11)) {
            nggPinduno.adrLED1()->color("red");
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(10)) {
            nggPinduno.adrLED1()->fadeOut(50);
            nggPinduno.adrLED1()->bullet2Color("green", "red", 20, 2, 1);
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(9)) {
            nggPinduno.adrLED1()->fadeOut(50);
            nggPinduno.adrLED1()->bulletFromPoint2Color("white", "green", 17, 5, 17);
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(7)) {
            nggPinduno.adrLED1()->fadeOut(50);
            nggPinduno.adrLED1()->bulletFromPoint2Color("green", "white", 17, 5, 17);
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(6)) {
            nggPinduno.adrLED1()->color("green");
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(5)) {
            nggPinduno.adrLED1()->color("red");
            trigger = 1;
        }
        else if (nggPinduno.pinState()->J126(4)) {
            nggPinduno.adrLED1()->color("blue");
            trigger = 1;
        }

        if (trigger) {
            nggPinduno.pinState()->reset();
            timeLastEvent = millis();
            trigger = 0;
            // Directly update state machine if needed
            if (ledState == WHITE_IDLE) {
                ledState = EFFECT_ACTIVE;
            }
        }
    }
}

boolean isDelayOver(){
  boolean delayOver = false;
  if(millis() - lastTriggerTime >= DELAYTIME){
    delayOver = true;  
    lastTriggerTime = millis();
  }
  return delayOver;
}
// Function to read potentiometer value and set brightness
void readPotentiometer() {
    int potValue = analogRead(potPin); // Read potentiometer (0-1023)
    brightness = map(potValue, 0, 1023, 0, 255); // Scale to 0-255
    if(abs(currentbrightness - brightness) > brightnesschangethreshold){
      if(brightness == 0){ //0 could lead to odd behaviour
        brightness = 1;  
      }
      //nggPinduno.adrLED1()->setBrightness(brightness); // Apply brightness
      currentbrightness = brightness;
    }
    DEBUG_PRINT("Potentiometer: ");
    DEBUG_PRINT(potValue);
    DEBUG_PRINT(" | Brightness: ");
    DEBUG_PRINTLN(currentbrightness);
}

void background() {
    nggPinduno.adrLED1()->sparkle(color, 20);

    if (random(1000) == 0) {
        if (color == "white") color = "green";
        else if (color == "green") color = "blue";
        else color = "white";
    }
}
