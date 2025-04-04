// Eric Lyons 2018
// Updated by ChatGPT 2025
// Interfaced for pinduino shield v0.2 and v0.3
// Uses pinduino library

#include "pinduinoext.h"

// === Debug Configuration ===
#define DEBUG 1
#if DEBUG == 1
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x)
#endif

// === Constants ===
#define DELAYTIME 100
#define IDLE_COLOR_FADE_TIME 200  // Smooth transition to idle color
const unsigned long attractTimeout = 20000; // Time before returning to attract
const unsigned long effectDuration = 2000; // Duration for simple effects (2 seconds)

// === LED Configuration ===
const int aLEDNum1 = 80; // left ramp
const int aLEDNum2 = 0;  // right ramp
const int aLEDNum3 = 0;
const uint8_t idleColorR = 128;
const uint8_t idleColorG = 128;
const uint8_t idleColorB = 128;

// === State Enums ===
enum LEDState { ATTRACT, WHITE_IDLE, EFFECT_ACTIVE };
LEDState ledState = ATTRACT;

// === Variables ===
unsigned long lastTriggerTime = 0;
unsigned long timeLastEvent = 0;
unsigned long effectStartTime = 0;
String attractColor = "white";
bool gameActive = false;

// === Brightness Control ===
const int potPin = A6;
int brightness = 250;
int currentBrightness = 250;
const int brightnessChangeThreshold = 5;

// === Pinduino Instance ===
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// === Function Prototypes ===
void setToIdleColor(bool fade = true);
void handleAttractMode(bool newGame);
void handleWhiteIdleMode(bool hadTrigger);
void handleEffectActiveMode();
bool checkPinStates();
bool isDelayOver();
void readPotentiometer();
void debugLEDState(const char* newState);
bool checkForNewGame();
void handleSimpleEffect(const String& color);

// === Setup ===
void setup() {
  Serial.begin(115200);
  gameActive = false;

  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED2()->clear();
  nggPinduno.adrLED3()->clear();
  nggPinduno.pinState()->reset();
  nggPinduno.adrLED1()->setBrightness(250);
  pinMode(potPin, INPUT);
}

// === Main Loop ===
void loop() {
  readPotentiometer();
  nggPinduno.pinState()->update();
  bool newGame = (!gameActive && checkForNewGame());
  bool hadTrigger = checkPinStates();

  switch (ledState) {
    case ATTRACT:
      handleAttractMode(newGame);
      break;
      
    case WHITE_IDLE:
      handleWhiteIdleMode(hadTrigger);
      break;
      
    case EFFECT_ACTIVE:
      handleEffectActiveMode();
      break;
  }
}

// === Check for Initial Game Start ===
bool checkForNewGame() {
  return (nggPinduno.pinState()->J126(12) ||
          nggPinduno.pinState()->J126(11) ||
          nggPinduno.pinState()->J126(10) ||
          nggPinduno.pinState()->J126(9) ||
          nggPinduno.pinState()->J126(7) ||
          nggPinduno.pinState()->J126(6) ||
          nggPinduno.pinState()->J126(5) ||
          nggPinduno.pinState()->J126(4));
}

// === State Handlers ===
void handleAttractMode(bool newGame) {
  nggPinduno.adrLED1()->sparkle(attractColor, 20);
  
  if (random(1000) == 0) {
    if (attractColor == "white") attractColor = "green";
    else if (attractColor == "green") attractColor = "blue";
    else attractColor = "white";
  }

  if (newGame) {
    gameActive = true;
    checkPinStates(); // Trigger initial effect immediately
    ledState = WHITE_IDLE; // Go to idle after effect (complex) or set up simple effect
    setToIdleColor();
    timeLastEvent = millis();
    debugLEDState("WHITE_IDLE");
  }
}

void handleWhiteIdleMode(bool hadTrigger) {
  setToIdleColor(false);
  
  if (hadTrigger) {
    // Effect is already handled in checkPinStates()
    timeLastEvent = millis();
  } 
  else if (millis() - timeLastEvent > attractTimeout) {
    ledState = ATTRACT;
    nggPinduno.adrLED1()->fadeOut(500);
    gameActive = false;
    debugLEDState("ATTRACT");
  }
}

void handleEffectActiveMode() {
  if (millis() - effectStartTime > effectDuration) {
    ledState = WHITE_IDLE;
    setToIdleColor();
    timeLastEvent = millis();
    debugLEDState("WHITE_IDLE");
  }
}

// === Helper Functions ===
void setToIdleColor(bool fade) {
  if (fade) {
    nggPinduno.adrLED1()->fadeInRGB(idleColorR, idleColorG, idleColorB, IDLE_COLOR_FADE_TIME);
  } else {
    nggPinduno.adrLED1()->colorRGB(idleColorR, idleColorG, idleColorB);
  }
}

// === Pin Trigger Handling ===
bool checkPinStates() {
  static int trigger = 0;

  if (isDelayOver()) {
    if (nggPinduno.pinState()->J126(12)) {
      handleSimpleEffect("blue");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) {
      handleSimpleEffect("red");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(10)) {
      nggPinduno.adrLED1()->fadeOut(50);
      nggPinduno.adrLED1()->bullet2Color("green", "red", 20, 2, 1); // Blocking
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(9)) {
      nggPinduno.adrLED1()->fadeOut(50);
      nggPinduno.adrLED1()->bulletFromPoint2Color("white", "green", 17, 5, 17); // Blocking
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(7)) {
      nggPinduno.adrLED1()->fadeOut(50);
      nggPinduno.adrLED1()->bulletFromPoint2Color("green", "white", 17, 5, 17); // Blocking
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(6)) {
      handleSimpleEffect("green");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(5)) {
      handleSimpleEffect("red");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(4)) {
      handleSimpleEffect("blue");
      trigger = 1;
    }

    if (trigger) {
      nggPinduno.pinState()->reset();
      timeLastEvent = millis();
      trigger = 0;
      if (ledState != EFFECT_ACTIVE) { // If not already in EFFECT_ACTIVE (simple effect)
        ledState = WHITE_IDLE; // Return to idle after complex effect
        setToIdleColor();
        debugLEDState("WHITE_IDLE");
      }
      return true;
    }
  }
  return false;
}

// === Simple Effect Handler ===
void handleSimpleEffect(const String& color) {
  nggPinduno.adrLED1()->color(color);
  effectStartTime = millis();
  ledState = EFFECT_ACTIVE;
  debugLEDState("EFFECT_ACTIVE");
}

// === Delay Timer Check ===
bool isDelayOver() {
  if (millis() - lastTriggerTime >= DELAYTIME) {
    lastTriggerTime = millis();
    return true;
  }
  return false;
}

// === Potentiometer Brightness Control ===
void readPotentiometer() {
  int potValue = analogRead(potPin);
  brightness = map(potValue, 0, 1023, 0, 255);
  
  if (abs(currentBrightness - brightness) > brightnessChangeThreshold) {
    brightness = constrain(brightness, 1, 255);
    nggPinduno.adrLED1()->setBrightness(brightness);
    currentBrightness = brightness;
    DEBUG_PRINT("Brightness: ");
    DEBUG_PRINTLN(currentBrightness);
  }
}

// === Debugging Helper ===
void debugLEDState(const char* newState) {
  static unsigned long lastStateChange = 0;
  DEBUG_PRINT("[STATE] ");
  DEBUG_PRINT(millis() - lastStateChange);
  DEBUG_PRINT("ms -> ");
  DEBUG_PRINTLN(newState);
  lastStateChange = millis();
}