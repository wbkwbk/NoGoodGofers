// Eric Lyons 2018
// Updated by Martin Ruff & Grok 3 & Deepseek & ChatGPT 2025
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
#define IDLE_COLOR_FADE_TIME 200
const unsigned long attractTimeout = 20000;
const unsigned long effectDuration = 2000;
const unsigned long attractColorChangeInterval = 3000;

// RGB Color Constants
const uint8_t DARK_GREEN_R = 0;
const uint8_t DARK_GREEN_G = 100;
const uint8_t DARK_GREEN_B = 0;

const uint8_t LIGHT_GREEN_R = 144;
const uint8_t LIGHT_GREEN_G = 238;
const uint8_t LIGHT_GREEN_B = 144;

const uint8_t DARK_BLUE_R = 0;
const uint8_t DARK_BLUE_G = 0;
const uint8_t DARK_BLUE_B = 139;

const uint8_t LIGHT_BLUE_R = 173;
const uint8_t LIGHT_BLUE_G = 216;
const uint8_t LIGHT_BLUE_B = 230;

const uint8_t PURPLE_R = 128;
const uint8_t PURPLE_G = 0;
const uint8_t PURPLE_B = 128;

const int NUM_ATTRACT_COLORS = 5;

// === LED Configuration ===
const int aLEDNum1 = 80;
const int aLEDNum2 = 0;
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
unsigned long lastAttractColorChange = 0;
uint8_t currentAttractR = DARK_GREEN_R;
uint8_t currentAttractG = DARK_GREEN_G;
uint8_t currentAttractB = DARK_GREEN_B;
String attractColorName = "darkgreen";
bool gameActive = false;

// Color cycling variables
bool colorUsed[NUM_ATTRACT_COLORS] = {false};
int availableColors[NUM_ATTRACT_COLORS];
int numAvailableColors = 0;

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
void handleSimpleEffect(uint8_t r, uint8_t g, uint8_t b);
void changeAttractColor();
void setAttractEffect();
void resetColorCycle();
int getNextRandomColor();

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
  
  // Start in attract mode with initial color
  ledState = ATTRACT;
  resetColorCycle();
  changeAttractColor();
  setAttractEffect();
  debugLEDState("ATTRACT");
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

void resetColorCycle() {
  for (int i = 0; i < NUM_ATTRACT_COLORS; i++) {
    colorUsed[i] = false;
  }
  numAvailableColors = NUM_ATTRACT_COLORS;
  DEBUG_PRINTLN("Color cycle reset - all colors available");
}

int getNextRandomColor() {
  if (numAvailableColors == 0) {
    resetColorCycle();
  }

  int availableIndex = 0;
  for (int i = 0; i < NUM_ATTRACT_COLORS; i++) {
    if (!colorUsed[i]) {
      availableColors[availableIndex++] = i;
    }
  }

  int selectedIndex = random(numAvailableColors);
  int selectedColor = availableColors[selectedIndex];
  
  colorUsed[selectedColor] = true;
  numAvailableColors--;
  
  return selectedColor;
}

void changeAttractColor() {
  int colorChoice = getNextRandomColor();
  
  switch(colorChoice) {
    case 0: // Dark Green
      currentAttractR = DARK_GREEN_R;
      currentAttractG = DARK_GREEN_G;
      currentAttractB = DARK_GREEN_B;
      attractColorName = "darkgreen";
      break;
    case 1: // Light Green
      currentAttractR = LIGHT_GREEN_R;
      currentAttractG = LIGHT_GREEN_G;
      currentAttractB = LIGHT_GREEN_B;
      attractColorName = "lightgreen";
      break;
    case 2: // Dark Blue
      currentAttractR = DARK_BLUE_R;
      currentAttractG = DARK_BLUE_G;
      currentAttractB = DARK_BLUE_B;
      attractColorName = "darkblue";
      break;
    case 3: // Light Blue
      currentAttractR = LIGHT_BLUE_R;
      currentAttractG = LIGHT_BLUE_G;
      currentAttractB = LIGHT_BLUE_B;
      attractColorName = "lightblue";
      break;
    case 4: // Purple
      currentAttractR = PURPLE_R;
      currentAttractG = PURPLE_G;
      currentAttractB = PURPLE_B;
      attractColorName = "purple";
      break;
  }
  DEBUG_PRINT("Attract color changed to: ");
  DEBUG_PRINTLN(attractColorName);
  DEBUG_PRINT("Colors remaining: ");
  DEBUG_PRINTLN(numAvailableColors);
}

void setAttractEffect() {
  // Set base color
  nggPinduno.adrLED1()->colorRGB(currentAttractR, currentAttractG, currentAttractB);
  // Add sparkle effect with a brighter version of the current color
  nggPinduno.adrLED1()->sparkleRGB(
    min(255, currentAttractR * 1.2),
    min(255, currentAttractG * 1.2),
    min(255, currentAttractB * 1.2),
    20 // 20% chance of sparkle
  );
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
  if (millis() - lastAttractColorChange > attractColorChangeInterval) {
    changeAttractColor();
    setAttractEffect();
    lastAttractColorChange = millis();
  }

  if (newGame) {
    gameActive = true;
    checkPinStates(); // Trigger initial effect immediately
    ledState = WHITE_IDLE;
    setToIdleColor();
    timeLastEvent = millis();
    debugLEDState("WHITE_IDLE");
  }
}

void handleWhiteIdleMode(bool hadTrigger) {
  setToIdleColor(false);
  
  if (hadTrigger) {
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
      handleSimpleEffect(0, 0, 255); // Blue
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) {
      handleSimpleEffect(255, 0, 0); // Red
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
      handleSimpleEffect(0, 255, 0); // Green
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(5)) {
      handleSimpleEffect(255, 0, 0); // Red
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(4)) {
      handleSimpleEffect(0, 0, 255); // Blue
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
void handleSimpleEffect(uint8_t r, uint8_t g, uint8_t b) {
  nggPinduno.adrLED1()->colorRGB(r, g, b);
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