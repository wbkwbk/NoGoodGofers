// Eric Lyons 2018
// Updated by Martin Ruff & Grok 3 & Deepseek & ChatGPT 2025
// Interfaced for pinduino shield v0.2 and v0.3 for "No Good Gophers" pinball
// Uses pinduino library

#include "pinduinoext.h"

// === Debug Configuration ===
#define DEBUG 1
#if DEBUG == 1
  #define DEBUG_PRINT(x) Serial.print(F(x))
  #define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
  #define DEBUG_PRINTLN(x) Serial.println(F(x))
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x)
#endif

// === Constants ===
#define DELAYTIME 100
#define IDLE_COLOR_FADE_TIME 200
const unsigned long attractTimeout = 20000;
const unsigned long effectDuration = 2000; // Shortened for pinball pace
const unsigned long attractColorChangeInterval = 3000;

// RGB Color Constants for Attract Mode
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
unsigned long lasteffectOccuredTime = 0;
unsigned long timeLastEvent = 0;
unsigned long effectStartTime = 0;
unsigned long lastAttractColorChange = 0;
uint8_t currentAttractR = DARK_GREEN_R;
uint8_t currentAttractG = DARK_GREEN_G;
uint8_t currentAttractB = DARK_GREEN_B;
String attractColorName = "darkgreen";
bool gameActive = false;

// Color cycling variables for attract mode
bool colorUsed[NUM_ATTRACT_COLORS] = {false};
int availableColors[NUM_ATTRACT_COLORS];
int numAvailableColors = 0;

// === Brightness Control ===
const int potPin = A5;
int brightness = 250;
int currentBrightness = 250;
const int brightnessChangeThreshold = 5;

// === Pinduino Instance ===
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// === Dynamic Effect and Color Management ===
const int NUM_EFFECTS_J126_10 = 4;
const int NUM_EFFECTS_J126_9 = 4;
const int NUM_EFFECTS_J126_7 = 4;
const int NUM_COLORS = 13;

static bool usedEffectsJ126_10[NUM_EFFECTS_J126_10] = {false};
static bool usedEffectsJ126_9[NUM_EFFECTS_J126_9] = {false};
static bool usedEffectsJ126_7[NUM_EFFECTS_J126_7] = {false};
static int effectsRemainingJ126_10 = NUM_EFFECTS_J126_10;
static int effectsRemainingJ126_9 = NUM_EFFECTS_J126_9;
static int effectsRemainingJ126_7 = NUM_EFFECTS_J126_7;

static bool usedColors[NUM_COLORS] = {false};
static int colorsRemaining = NUM_COLORS;
// Themed colors for "No Good Gophers": golf course greens, gopher browns, and accents
static const char* colorList[NUM_COLORS] = {"green", "lime", "yellow", "sky", "brown", "orange", "white", "red", "cyan", "purple", "gold", "tan", "blue"};

// === Function Prototypes ===
void setToIdleColor(bool fade = true);
void handleAttractMode(bool newGame);
void handleWhiteIdleMode(bool hadeffectOccured);
void handleEffectActiveMode();
bool checkPinStates();
bool isDelayOver();
void readPotentiometer();
void debugLEDState(const char* newState);
bool checkForNewGame();
void handleSimpleEffect(uint8_t r, uint8_t g, uint8_t b, const char* effectDescription);
void changeAttractColor();
void setAttractEffect();
void resetColorCycle();
int getNextRandomColor();
int getRandomEffect(int eventNum, int numEffects, bool* usedEffects, int& effectsRemaining);
const char* getRandomColor();
void getRGBForColor(const char* color, uint8_t& r, uint8_t& g, uint8_t& b);
void resetEffects(int eventNum);
void resetColors();

// === Setup ===
void setup() {
  Serial.begin(115200);
  delay(1000); // Add delay for serial stabilization
  DEBUG_PRINTLN("setup");
  gameActive = false;

  nggPinduno.adrLED1()->clear();
  DEBUG_PRINTLN("Effect: clear (LED1)");
  nggPinduno.adrLED2()->clear();
  DEBUG_PRINTLN("Effect: clear (LED2)");
  nggPinduno.adrLED3()->clear();
  DEBUG_PRINTLN("Effect: clear (LED3)");
  nggPinduno.pinState()->reset();
  nggPinduno.adrLED1()->setBrightness(250);
  pinMode(potPin, INPUT);
  
  // Start in attract mode with initial color
  ledState = ATTRACT;
  resetColorCycle();
  changeAttractColor();
  setAttractEffect();
  debugLEDState("ATTRACT");

  // Seed random number generator
  randomSeed(analogRead(A0));
}

// === Main Loop ===
void loop() {
  //readPotentiometer();
  nggPinduno.pinState()->update();
  bool newGame = (!gameActive && checkForNewGame());
  bool hadeffectOccured = checkPinStates();

  switch (ledState) {
    case ATTRACT:
      handleAttractMode(newGame);
      break;
      
    case WHITE_IDLE:
      handleWhiteIdleMode(hadeffectOccured);
      break;
      
    case EFFECT_ACTIVE:
      handleEffectActiveMode();
      break;
  }
}

// === Attract Mode Color Cycling ===
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
  Serial.print(attractColorName);
  DEBUG_PRINTLN("");
  DEBUG_PRINT("Colors remaining: ");
  DEBUG_PRINTDEC(numAvailableColors);
  DEBUG_PRINTLN("");
}

void setAttractEffect() {
  DEBUG_PRINT("Effect: colorRGB with color: ");
  Serial.print(attractColorName);
  DEBUG_PRINTLN("");
  nggPinduno.adrLED1()->colorRGB(currentAttractR, currentAttractG, currentAttractB);
  DEBUG_PRINT("Effect: sparkle with color: ");
  Serial.print(attractColorName);
  DEBUG_PRINTLN("");
  nggPinduno.adrLED1()->sparkle(
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
    checkPinStates(); // effectOccured initial effect immediately
    ledState = WHITE_IDLE;
    setToIdleColor();
    timeLastEvent = millis();
    debugLEDState("WHITE_IDLE");
  }
}

void handleWhiteIdleMode(bool hadeffectOccured) {
  setToIdleColor(false);
  
  if (hadeffectOccured) {
    timeLastEvent = millis();
  } 
  else if (millis() - timeLastEvent > attractTimeout) {
    ledState = ATTRACT;
    DEBUG_PRINTLN("Effect: fadeOut");
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
    DEBUG_PRINTLN("Effect: fadeInRGB with color: idle (gray)");
    nggPinduno.adrLED1()->fadeInRGB(idleColorR, idleColorG, idleColorB, IDLE_COLOR_FADE_TIME);
  } else {
    DEBUG_PRINTLN("Effect: colorRGB with color: idle (gray)");
    nggPinduno.adrLED1()->colorRGB(idleColorR, idleColorG, idleColorB);
  }
}

// === Dynamic Effect and Color Selection ===
int getRandomEffect(int eventNum, int numEffects, bool* usedEffects, int& effectsRemaining) {
  if (effectsRemaining == 0) {
    resetEffects(eventNum);
  }

  int availableEffects[numEffects];
  int availableCount = 0;
  for (int i = 0; i < numEffects; i++) {
    if (!usedEffects[i]) {
      availableEffects[availableCount++] = i;
    }
  }

  int selectedIndex = random(availableCount);
  int selectedEffect = availableEffects[selectedIndex];
  usedEffects[selectedEffect] = true;
  effectsRemaining--;
  
  DEBUG_PRINT("Selected effect for J126(");
  DEBUG_PRINTDEC(eventNum);
  DEBUG_PRINT("): ");
  DEBUG_PRINTDEC(selectedEffect);
  DEBUG_PRINTLN("");
  return selectedEffect;
}

void resetEffects(int eventNum) {
  if (eventNum == 10) {
    for (int i = 0; i < NUM_EFFECTS_J126_10; i++) {
      usedEffectsJ126_10[i] = false;
    }
    effectsRemainingJ126_10 = NUM_EFFECTS_J126_10;
  } else if (eventNum == 9) {
    for (int i = 0; i < NUM_EFFECTS_J126_9; i++) {
      usedEffectsJ126_9[i] = false;
    }
    effectsRemainingJ126_9 = NUM_EFFECTS_J126_9;
  } else if (eventNum == 7) {
    for (int i = 0; i < NUM_EFFECTS_J126_7; i++) {
      usedEffectsJ126_7[i] = false;
    }
    effectsRemainingJ126_7 = NUM_EFFECTS_J126_7;
  }
  DEBUG_PRINT("Reset effects for J126(");
  DEBUG_PRINTDEC(eventNum);
  DEBUG_PRINTLN(")");
}

const char* getRandomColor() {
  if (colorsRemaining == 0) {
    resetColors();
  }

  int availableColorsList[NUM_COLORS];
  int availableCount = 0;
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!usedColors[i]) {
      availableColorsList[availableCount++] = i;
    }
  }

  int selectedIndex = random(availableCount);
  int selectedColorIdx = availableColorsList[selectedIndex];
  usedColors[selectedColorIdx] = true;
  colorsRemaining--;

  DEBUG_PRINT("Selected color: ");
  Serial.println(colorList[selectedColorIdx]);
  return colorList[selectedColorIdx];
}

void getRGBForColor(const char* color, uint8_t& r, uint8_t& g, uint8_t& b) {
  if (strcmp(color, "green") == 0) { r = 0; g = 255; b = 0; }
  else if (strcmp(color, "lime") == 0) { r = 0; g = 255; b = 0; }
  else if (strcmp(color, "yellow") == 0) { r = 255; g = 255; b = 0; }
  else if (strcmp(color, "sky") == 0) { r = 135; g = 206; b = 235; }
  else if (strcmp(color, "brown") == 0) { r = 139; g = 69; b = 19; }
  else if (strcmp(color, "orange") == 0) { r = 255; g = 165; b = 0; }
  else if (strcmp(color, "white") == 0) { r = 255; g = 255; b = 255; }
  else if (strcmp(color, "red") == 0) { r = 255; g = 0; b = 0; }
  else if (strcmp(color, "cyan") == 0) { r = 0; g = 255; b = 255; }
  else if (strcmp(color, "purple") == 0) { r = 128; g = 0; b = 128; }
  else if (strcmp(color, "gold") == 0) { r = 255; g = 215; b = 0; }
  else if (strcmp(color, "tan") == 0) { r = 210; g = 180; b = 140; }
  else if (strcmp(color, "blue") == 0) { r = 0; g = 0; b = 255; }
  else { r = 255; g = 255; b = 255; }
}

void resetColors() {
  for (int i = 0; i < NUM_COLORS; i++) {
    usedColors[i] = false;
  }
  colorsRemaining = NUM_COLORS;
  DEBUG_PRINTLN("Reset colors");
}

// === Pin effectOccured Handling ===
bool checkPinStates() {
  static int effectOccured = 0;

  if (isDelayOver()) {
    if (nggPinduno.pinState()->J126(12)) {
      handleSimpleEffect(0, 0, 255, "colorRGB (Start button or tilt) with color: blue");
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) {
      handleSimpleEffect(255, 0, 0, "colorRGB (Game over or error) with color: red");
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(10)) {
      DEBUG_PRINTLN("Effect: fadeOut");
      nggPinduno.adrLED1()->fadeOut(50);
      int effect = getRandomEffect(10, NUM_EFFECTS_J126_10, usedEffectsJ126_10, effectsRemainingJ126_10);
      const char* color1 = getRandomColor();
      const char* color2 = (effect == 0 || effect == 1 || effect == 2) ? getRandomColor() : nullptr;
      switch (effect) {
        case 0:
          DEBUG_PRINT("Effect: bullet2Color (Fast shot) with colors: ");
          Serial.print(color1);
          DEBUG_PRINT(", ");
          Serial.print(color2);
          DEBUG_PRINTLN("");
          nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);
          break;
        case 1:
          DEBUG_PRINT("Effect: bulletFromPoint2Color (Ball launch) with colors: ");
          Serial.print(color1);
          DEBUG_PRINT(", ");
          Serial.print(color2);
          DEBUG_PRINTLN("");
          nggPinduno.adrLED1()->bulletFromPoint2Color(color1, color2, 20, 2, aLEDNum1 / 2);
          break;
        case 2:
          DEBUG_PRINT("Effect: chase2RGBFromPoint (Ramp chase) with colors: ");
          Serial.print(color1);
          DEBUG_PRINT(", ");
          Serial.print(color2);
          DEBUG_PRINTLN("");
          uint8_t r1, g1, b1, r2, g2, b2;
          getRGBForColor(color1, r1, g1, b1);
          getRGBForColor(color2, r2, g2, b2);
          nggPinduno.adrLED1()->chase2RGBFromPoint(aLEDNum1 / 2, r1, g1, b1, r2, g2, b2, 5, 20);
          break;
        case 3:
          DEBUG_PRINT("Effect: spreadInFromPoint (Target hit) with color: ");
          Serial.print(color1);
          DEBUG_PRINTLN("");
          nggPinduno.adrLED1()->spreadInFromPoint(aLEDNum1 / 2, color1, 1000);
          break;
      }
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(9)) {
      DEBUG_PRINTLN("Effect: fadeOut");
      nggPinduno.adrLED1()->fadeOut(50);
      int effect = getRandomEffect(9, NUM_EFFECTS_J126_9, usedEffectsJ126_9, effectsRemainingJ126_9);
      const char* color1 = getRandomColor();
      const char* color2 = (effect == 0 || effect == 3) ? getRandomColor() : nullptr;
      switch (effect) {
        case 0:
          DEBUG_PRINT("Effect: bullet2Color (Quick gopher pop) with colors: ");
          Serial.print(color1);
          DEBUG_PRINT(", ");
          Serial.print(color2);
          DEBUG_PRINTLN("");
          nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);
          break;
        case 1:
          DEBUG_PRINT("Effect: dataStream (Gopher digging) with color: ");
          Serial.print(color1);
          DEBUG_PRINTLN("");
          nggPinduno.adrLED1()->dataStream(color1, 20, 50, 1);
          break;
        case 2:
          DEBUG_PRINT("Effect: dataStreamRGB (Bumper hit) with color: ");
          Serial.print(color1);
          DEBUG_PRINTLN("");
          uint8_t r, g, b;
          getRGBForColor(color1, r, g, b);
          nggPinduno.adrLED1()->dataStreamRGB(r, g, b, 20, 50, 1);
          break;
        case 3:
          DEBUG_PRINT("Effect: spreadInFromPoint2Color (Gopher spread) with colors: ");
          Serial.print(color1);
          DEBUG_PRINT(", ");
          Serial.print(color2);
          DEBUG_PRINTLN("");
          nggPinduno.adrLED1()->spreadInFromPoint2Color(aLEDNum1 / 2, color1, color2, 1000);
          break;
      }
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(7)) {
      DEBUG_PRINTLN("Effect: fadeOut");
      nggPinduno.adrLED1()->fadeOut(50);
      int effect = getRandomEffect(7, NUM_EFFECTS_J126_7, usedEffectsJ126_7, effectsRemainingJ126_7);
      const char* color1 = getRandomColor();
      const char* color2 = (effect == 3) ? getRandomColor() : nullptr;
      switch (effect) {
        case 0:
          DEBUG_PRINTLN("Effect: rainbow (Multi-ball celebration)");
          nggPinduno.adrLED1()->rainbow(20);
          break;
        case 1:
          DEBUG_PRINTLN("Effect: rainbowCycle (Jackpot flash)");
          nggPinduno.adrLED1()->rainbowCycle(20);
          break;
        case 2:
          DEBUG_PRINT("Effect: RGBBullet (Ball explosion) with color: ");
          Serial.print(color1);
          DEBUG_PRINTLN("");
          uint8_t r, g, b;
          getRGBForColor(color1, r, g, b);
          nggPinduno.adrLED1()->RGBBullet(aLEDNum1 / 2, r, g, b, 10, 1);
          break;
        case 3:
          DEBUG_PRINT("Effect: spreadInFromPoint2RGB (Big score spread) with colors: ");
          Serial.print(color1);
          DEBUG_PRINT(", ");
          Serial.print(color2);
          DEBUG_PRINTLN("");
          uint8_t r1, g1, b1, r2, g2, b2;
          getRGBForColor(color1, r1, g1, b1);
          getRGBForColor(color2, r2, g2, b2);
          nggPinduno.adrLED1()->spreadInFromPoint2RGB(aLEDNum1 / 2, r1, g1, b1, r2, g2, b2, 1000);
          break;
      }
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(6)) {
      handleSimpleEffect(0, 255, 0, "colorRGB (Minor score) with color: green");
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(5)) {
      handleSimpleEffect(255, 0, 0, "colorRGB (Hazard or miss) with color: red");
      effectOccured = 1;
    }
    else if (nggPinduno.pinState()->J126(4)) {
      handleSimpleEffect(0, 0, 255, "colorRGB (Bonus) with color: blue");
      effectOccured = 1;
    }

    if (effectOccured) {
      nggPinduno.pinState()->reset();
      timeLastEvent = millis();
      effectOccured = 0;
      if (ledState != EFFECT_ACTIVE) {
        ledState = WHITE_IDLE;
        setToIdleColor();
        debugLEDState("WHITE_IDLE");
      }
      return true;
    }
  }
  return false;
}

// === Simple Effect Handler ===
void handleSimpleEffect(uint8_t r, uint8_t g, uint8_t b, const char* effectDescription) {
  Serial.println(effectDescription);
  nggPinduno.adrLED1()->colorRGB(r, g, b);
  effectStartTime = millis();
  ledState = EFFECT_ACTIVE;
  debugLEDState("EFFECT_ACTIVE");
}

// === Delay Timer Check ===
bool isDelayOver() {
  if (millis() - lasteffectOccuredTime >= DELAYTIME) {
    lasteffectOccuredTime = millis();
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
    DEBUG_PRINTDEC(currentBrightness);
    DEBUG_PRINTLN("");
  }
}

// === Debugging Helper ===
void debugLEDState(const char* newState) {
  static unsigned long lastStateChange = 0;
  DEBUG_PRINT("[STATE] ");
  DEBUG_PRINTDEC(millis() - lastStateChange);
  DEBUG_PRINT("ms -> ");
  Serial.println(newState);
  lastStateChange = millis();
}