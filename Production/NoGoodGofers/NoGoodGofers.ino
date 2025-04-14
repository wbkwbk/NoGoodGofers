// No Good Gophers - Final Stable Implementation with Voltage Monitoring
// Modified for dynamic effect and color selection for J126(10), J126(9), J126(7)
// Fixed spreadOutToPoint for J126(10) to use correct parameters and dynamic color
// Removed redundant #if DEBUG == 1 directives for debug output (handled by macros)

#include "pinduinoext.h"

// Debug Configuration
#define DEBUG 1
#if DEBUG == 1
  #define debug_print(x) Serial.print(F(x))
  #define debug_print_dec(x) Serial.print(x, DEC)
  #define debug_println(x) Serial.println(F(x))
  #define debug_print_var(x) Serial.print(x)
  #define debug_println_var(x) Serial.println(x)
#else
  #define debug_print(x)
  #define debug_print_dec(x)
  #define debug_println(x)
  #define debug_print_var(x)
  #define debug_println_var(x)
#endif

// LED Configuration
const int aLEDNum1 = 80;
const int aLEDNum2 = 0;
const int aLEDNum3 = 0;
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// Timing Constants
const unsigned long effectDuration = 2000;
const unsigned long attractColorDuration = 5000;
const unsigned long attractTimeout = 20000;
const unsigned long minRefreshInterval = 50;
const unsigned long voltageCheckInterval = 5000;

// State Machine
enum GameState { ATTRACT, GAME_RUN, EFFECT_ACTIVE };
GameState currentState = ATTRACT;
bool stateChanged = true;

// Game Variables
unsigned long timeLastEvent = 0;
unsigned long lastColorChangeTime = 0;
unsigned long effectStartTime = 0;
unsigned long lastRefreshTime = 0;
unsigned long lastVoltageCheck = 0;
String currentEffectColor = "";
String currentAttractColor = "";
int attractColorIndex = 0;
const String attractColors[] = {"green", "blue", "red"};
bool colorsShown[3] = {false, false, false};

// Voltage Monitoring
const int VOLTAGE_PIN = A0;
const float VOLTAGE_DIVIDER_RATIO = 2.0; // Adjust based on your voltage divider
const float REFERENCE_VOLTAGE = 5.0;

// Effect and Color Tracking
const int NUM_COLORS = 13;
const String availableColors[] = {"red", "green", "blue", "yellow", "cyan", "purple", "white", "orange", "lime", "sky", "mint", "magenta", "lavender"};
bool colorsUsed[NUM_COLORS] = {false}; // Tracks used colors
const int NUM_J126_10_EFFECTS = 4;
bool j126_10_effectsUsed[NUM_J126_10_EFFECTS] = {false}; // Tracks J126(10) effects
const int NUM_J126_9_EFFECTS = 3;
bool j126_9_effectsUsed[NUM_J126_9_EFFECTS] = {false}; // Tracks J126(9) effects
const int NUM_J126_7_EFFECTS = 3;
bool j126_7_effectsUsed[NUM_J126_7_EFFECTS] = {false}; // Tracks J126(7) effects

// Select random unused color
String getRandomColor() {
  // Check if all colors used
  bool allUsed = true;
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!colorsUsed[i]) {
      allUsed = false;
      break;
    }
  }
  // Reset if all used
  if (allUsed) {
    for (int i = 0; i < NUM_COLORS; i++) {
      colorsUsed[i] = false;
    }
    debug_println("All colors used, resetting color tracking");
  }
  // Count available colors
  int availableCount = 0;
  int availableIndices[NUM_COLORS];
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!colorsUsed[i]) {
      availableIndices[availableCount++] = i;
    }
  }
  // Select random available color
  if (availableCount > 0) {
    int selectedIndex = availableIndices[random(availableCount)];
    colorsUsed[selectedIndex] = true;
    debug_print("Selected color: ");
    debug_println_var(availableColors[selectedIndex]);
    return availableColors[selectedIndex];
  }
  return "white"; // Fallback
}

// Select random unused effect for J126(x)
int getRandomEffect(bool* effectsUsed, int numEffects) {
  // Check if all effects used
  bool allUsed = true;
  for (int i = 0; i < numEffects; i++) {
    if (!effectsUsed[i]) {
      allUsed = false;
      break;
    }
  }
  // Reset if all used
  if (allUsed) {
    for (int i = 0; i < numEffects; i++) {
      effectsUsed[i] = false;
    }
    debug_println("All effects used, resetting effect tracking");
  }
  // Count available effects
  int availableCount = 0;
  int availableIndices[numEffects];
  for (int i = 0; i < numEffects; i++) {
    if (!effectsUsed[i]) {
      availableIndices[availableCount++] = i;
    }
  }
  // Select random available effect
  if (availableCount > 0) {
    int selectedIndex = availableIndices[random(availableCount)];
    effectsUsed[selectedIndex] = true;
    return selectedIndex;
  }
  return 0; // Fallback
}

void setup() {
  #if DEBUG == 1
    Serial.begin(115200);
  #endif
  debug_println("System Initializing...");
  debug_println("LED Control v3.1 - With Voltage Monitoring");

  // Power stabilization
  delay(500);
  
  // Initialize voltage monitoring
  pinMode(VOLTAGE_PIN, INPUT);
  
  // Initialize LEDs
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED2()->clear();
  nggPinduno.adrLED3()->clear();
  nggPinduno.pinState()->reset();
  
  selectNextAttractColor();
  nggPinduno.adrLED1()->show(true);
  
  debug_println("Initialization Complete");
}

void loop() {
  // 1. Check system health
  checkVoltage();
  
  // 2. Update pin states
  nggPinduno.pinState()->update();
  
  // 3. State Machine Execution
  switch(currentState) {
    case ATTRACT:
      handleAttractState();
      break;
    case GAME_RUN:
      handleGameRunState();
      maintainLEDs();
      break;
    case EFFECT_ACTIVE:
      handleEffectActiveState();
      break;
  }
  
  // 4. Check for pin triggers
  checkPinStates();
}

float readVoltage() {
  int rawValue = analogRead(VOLTAGE_PIN);
  float voltage = (rawValue * REFERENCE_VOLTAGE) / 1023.0;
  return voltage * VOLTAGE_DIVIDER_RATIO;
}

void checkVoltage() {
  if (millis() - lastVoltageCheck > voltageCheckInterval) {
    lastVoltageCheck = millis();
    float voltage = readVoltage();
    
    debug_print("System Voltage: ");
    debug_print_var(voltage);
    debug_println("V");
    
    if (voltage < 4.5) {
      debug_println("WARNING: Low voltage detected!");
    }
  }
}

void handleAttractState() {
  if (stateChanged) {
    stateChanged = false;
    debug_println("Entering ATTRACT mode");
  }
  
  nggPinduno.adrLED1()->sparkle(currentAttractColor, 20);
  
  if (millis() - lastColorChangeTime >= attractColorDuration) {
    selectNextAttractColor();
    lastColorChangeTime = millis();
  }
}

void handleGameRunState() {
  if (stateChanged) {
    // Validate and set color
    uint8_t r = constrain(128, 0, 255);
    uint8_t g = constrain(128, 0, 255);
    uint8_t b = constrain(128, 0, 255);
    
    nggPinduno.adrLED1()->colorRGB(r, g, b);
    nggPinduno.adrLED1()->show(true);  // Force initial update
    stateChanged = false;
    
    debug_println("GAME_RUN initialized with forced color update");
  }
}

void maintainLEDs() {
  static uint32_t lastColor = 0;
  uint32_t currentColor = nggPinduno.adrLED1()->getPixelColor(0);
  
  // Only refresh if color changed or 50ms passed
  if (currentColor != lastColor || millis() - lastRefreshTime >= 50) {
    nggPinduno.adrLED1()->show(true);  // Force refresh
    lastColor = currentColor;
    lastRefreshTime = millis();
    
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug >= 1000) {
      lastDebug = millis();
      debug_println("LED refresh maintained");
    }
  }
}

void handleEffectActiveState() {
  if (stateChanged) {
    stateChanged = false;
    debug_println("Entering EFFECT_ACTIVE mode");
  }
  
  if (currentEffectColor != "" && (millis() - effectStartTime >= effectDuration)) {
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = "";
    debug_println("Effect completed - Returning to GAME_RUN");
  }
}

void selectNextAttractColor() {
  // Check if all colors have been shown
  bool allShown = true;
  for (int i = 0; i < 3; i++) {
    if (!colorsShown[i]) {
      allShown = false;
      break;
    }
  }
  
  // Reset tracking if needed
  if (allShown) {
    for (int i = 0; i < 3; i++) colorsShown[i] = false;
  }
  
  // Select random unseen color
  int availableColors = 0;
  int availableIndices[3];
  
  for (int i = 0; i < 3; i++) {
    if (!colorsShown[i]) {
      availableIndices[availableColors++] = i;
    }
  }
  
  if (availableColors > 0) {
    attractColorIndex = availableIndices[random(availableColors)];
    currentAttractColor = attractColors[attractColorIndex];
    colorsShown[attractColorIndex] = true;
    
    debug_print("New attract color: ");
    debug_println_var(currentAttractColor);
  }
}

bool isAnyPinActive() {
  for (int i = 3; i <= 12; i++) {
    if (i != 8 && nggPinduno.pinState()->J126(i)) {
      return true;
    }
  }
  return false;
}

void checkPinStates() {
  int trigger = 0;
  
  if (currentState == ATTRACT && isAnyPinActive()) {
    currentState = GAME_RUN;
    stateChanged = true;
    timeLastEvent = millis();
    nggPinduno.pinState()->reset();
    return;
  }
  
  if (currentState == GAME_RUN || currentState == EFFECT_ACTIVE) {
    // Dynamic effect and color selection for J126(10), J126(9), J126(7)
    if (nggPinduno.pinState()->J126(10)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      // Select random unused effect
      int effectIndex = getRandomEffect(j126_10_effectsUsed, NUM_J126_10_EFFECTS);
      // Get random colors
      String color1 = getRandomColor();
      String color2 = getRandomColor();
      // Execute effect
      switch (effectIndex) {
        case 0:
          nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);
          break;
        case 1:
          nggPinduno.adrLED1()->bulletFromPoint2Color(color1, color2, 5, 2, 75);
          break;
        case 2:
          // Set color and use correct spreadOutToPoint parameters
          nggPinduno.adrLED1()->color(color1);
          nggPinduno.adrLED1()->spreadOutToPoint(0, 300);
          break;
        case 3:
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 500);
          break;
      }
      currentEffectColor = color1; // For state tracking
      effectStartTime = millis();
      trigger = 1;
      debug_print("J126(10) Effect ");
      debug_print_dec(effectIndex);
      debug_print(" with colors ");
      debug_print_var(color1);
      if (effectIndex != 2) { // spreadOutToPoint uses single color
        debug_print(", ");
        debug_print_var(color2);
      }
      debug_println("");
    }
    else if (nggPinduno.pinState()->J126(9)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      // Select random unused effect
      int effectIndex = getRandomEffect(j126_9_effectsUsed, NUM_J126_9_EFFECTS);
      // Get random colors
      String color1 = getRandomColor();
      String color2 = getRandomColor();
      // Execute effect
      switch (effectIndex) {
        case 0:
          nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);
          break;
        case 1:
          nggPinduno.adrLED1()->dataStreamNoTail2Color(color1, color2, 20, 20, 1);
          break;
        case 2:
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 500);
          break;
      }
      currentEffectColor = color1;
      effectStartTime = millis();
      trigger = 1;
      debug_print("J126(9) Effect ");
      debug_print_dec(effectIndex);
      debug_print(" with colors ");
      debug_print_var(color1);
      debug_print(", ");
      debug_println_var(color2);
    }
    else if (nggPinduno.pinState()->J126(7)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      // Select random unused effect
      int effectIndex = getRandomEffect(j126_7_effectsUsed, NUM_J126_7_EFFECTS);
      // Get random color (single color for simplicity)
      String color1 = getRandomColor();
      // Execute effect
      switch (effectIndex) {
        case 0:
          nggPinduno.adrLED1()->rainbow(80); // Uses color1 for consistency
          break;
        case 1:
          nggPinduno.adrLED1()->rainbowCycle(80);
          break;
        case 2:
          // Use spreadInFromPoint2Color with color1
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color1, 300);
          break;
      }
      currentEffectColor = color1;
      effectStartTime = millis();
      trigger = 1;
      debug_print("J126(7) Effect ");
      debug_print_dec(effectIndex);
      debug_print(" with color ");
      debug_println_var(color1);
    }
    // Existing triggers for other pins
    else if (nggPinduno.pinState()->J126(12)) { // Blue
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) { // Red
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(6)) { // Green
      nggPinduno.adrLED1()->color("green");
      triggerEffect("green");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(5)) { // Red
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(4)) { // Blue
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue");
      trigger = 1;
    }
    
    if (trigger) {
      nggPinduno.pinState()->reset();
      timeLastEvent = millis();
    }
    
    if (millis() - timeLastEvent > attractTimeout) {
      currentState = ATTRACT;
      stateChanged = true;
      selectNextAttractColor();
      lastColorChangeTime = millis();
    }
  }
}

void triggerEffect(String color) {
  currentState = EFFECT_ACTIVE;
  stateChanged = true;
  currentEffectColor = color;
  effectStartTime = millis();
  
  debug_print("Triggering effect: ");
  debug_println_var(color);
}