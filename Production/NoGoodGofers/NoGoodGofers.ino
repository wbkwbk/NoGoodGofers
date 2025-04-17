// No Good Gophers - Enhanced with Button Control
// Integrates Red (A6) and Blue (A7) buttons for meta-state management
// Uses original Pinduino pin D10 for LED strip (adrLED1)
// New meta-states (NO_LED, ALL_LED, ALL_LED_RUN_ONLY, SET_BRIGHTNESS) layer above existing states
// Preserves original ATTRACT, GAME_RUN, EFFECT_ACTIVE state machine
// Implements single press, double-click, and long press detection
// Updated: SET_BRIGHTNESS sets LED strip to red
// Fixed: Blue Button single press toggles ALL_LED <-> ALL_LED_RUN_ONLY correctly

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
const String EFFECTFINISHED = "";
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// Button Configuration
const int RED_BUTTON_PIN = 6;   // D6 - Yellow Wire, Red Button
const int BLUE_BUTTON_PIN = 7;  // D7 - White Wire, Blue Button
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long LONG_PRESS_DURATION = 1000;  // 1s
const unsigned long DOUBLE_CLICK_WINDOW = 1000;  // 1s
const unsigned long BRIGHTNESS_UPDATE_INTERVAL = 50;  // 50ms

// Meta-State Machine (New Layer)
enum MetaGameState { NO_LED, ALL_LED, ALL_LED_RUN_ONLY, SET_BRIGHTNESS };
MetaGameState metaState = NO_LED;
MetaGameState previousMetaState = NO_LED;

// Original State Machine
enum GameState { ATTRACT, GAME_RUN, EFFECT_ACTIVE };
GameState currentState = ATTRACT;
bool stateChanged = true;

// Button States
struct ButtonState {
  bool currentState;
  bool lastState;
  unsigned long lastDebounceTime;
  unsigned long pressStartTime;
  bool isPressed;
  bool isLongPressed;
  unsigned long lastClickTime;
  int clickCount;
};

ButtonState redButton = {HIGH, HIGH, 0, 0, false, false, 0, 0};
ButtonState blueButton = {HIGH, HIGH, 0, 0, false, false, 0, 0};

// Timing Constants
const unsigned long effectDuration = 2000;
const unsigned long attractColorDuration = 5000;
const unsigned long attractTimeout = 20000;
const unsigned long minRefreshInterval = 50;
const unsigned long voltageCheckInterval = 5000;

// Game Variables
unsigned long timeLastEvent = 0;
unsigned long lastColorChangeTime = 0;
unsigned long effectStartTime = 0;
unsigned long lastRefreshTime = 0;
unsigned long lastVoltageCheck = 0;
unsigned long lastGameRunColorUpdate = 0;
unsigned long lastBrightnessUpdate = 0;
String currentEffectColor = "";
String currentAttractColor = "";
int attractColorIndex = 0;
const String attractColors[] = {"green", "blue", "red"};
bool colorsShown[3] = {false, false, false};
uint8_t brightness = 255;  // Default brightness

// Effect and Color Tracking
const int NUM_COLORS = 13;
const String availableColors[] = {"red", "green", "blue", "yellow", "cyan", "purple", "white", "orange", "lime", "sky", "mint", "magenta", "lavender"};
bool colorsUsed[NUM_COLORS] = {false};
const int NUM_J126_10_EFFECTS = 4;
bool j126_10_effectsUsed[NUM_J126_10_EFFECTS] = {false};
const int NUM_J126_9_EFFECTS = 3;
bool j126_9_effectsUsed[NUM_J126_9_EFFECTS] = {false};
const int NUM_J126_7_EFFECTS = 3;
bool j126_7_effectsUsed[NUM_J126_7_EFFECTS] = {false};

void setup() {
  #if DEBUG == 1
    Serial.begin(115200);
  #endif
  debug_println("System Initializing...");
  debug_println("LED Control v3.1 - With Button Enhancements");



  // Power stabilization
  delay(500);

  // Initialize LEDs
  nggPinduno.adrLED1()->strip()->begin(); // NeoPixel-Initialisierung
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  nggPinduno.pinState()->reset();

  selectNextAttractColor();  // Prepare for ALL_LED
  debug_println("Initialization Complete");

  // Initialize Buttons
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);

}

void loop() {
  // 1. Update button states
  updateButtonStates();

  // 2. Handle meta-state
  switch (metaState) {
    case NO_LED:
      handleNoLedState();
      break;
    case ALL_LED:
      handleAllLedState();
      break;
    case ALL_LED_RUN_ONLY:
      handleAllLedRunOnlyState();
      break;
    case SET_BRIGHTNESS:
      handleSetBrightnessState();
      break;
  }
}

void updateButtonStates() {
  // Read buttons with debouncing
  bool redReading = digitalRead(RED_BUTTON_PIN);
  bool blueReading = digitalRead(BLUE_BUTTON_PIN);

  // Red Button Debounce
  if (redReading != redButton.lastState) {
    redButton.lastDebounceTime = millis();
  }
  if (millis() - redButton.lastDebounceTime > DEBOUNCE_DELAY) {
    if (redReading != redButton.currentState) {
      redButton.currentState = redReading;
      if (redReading == LOW) {  // Pressed
        redButton.isPressed = true;
        redButton.pressStartTime = millis();
        redButton.clickCount++;
        if (redButton.clickCount == 1) {
          redButton.lastClickTime = millis();
        }
      } else {  // Released
        redButton.isPressed = false;
      }
    }
  }
  redButton.lastState = redReading;

  // Blue Button Debounce
  if (blueReading != blueButton.lastState) {
    blueButton.lastDebounceTime = millis();
  }
  if (millis() - blueButton.lastDebounceTime > DEBOUNCE_DELAY) {
    if (blueReading != blueButton.currentState) {
      blueButton.currentState = blueReading;
      if (blueReading == LOW) {  // Pressed
        blueButton.isPressed = true;
        blueButton.pressStartTime = millis();
        blueButton.isLongPressed = false;
      } else {  // Released
        blueButton.isPressed = false;
        if (blueButton.isLongPressed && metaState == SET_BRIGHTNESS) {
          // Save brightness and return to previous state
          metaState = previousMetaState;
          debug_print("Brightness set to: ");
          debug_print_dec(brightness); debug_println("");
        }
      }
    }
  }
  blueButton.lastState = blueReading;

  // Detect Red Button Double-Click
  if (redButton.clickCount > 0 && millis() - redButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (redButton.clickCount >= 2) {
      // Double-click: Set ALL_LED, brightness 255
      metaState = ALL_LED;
      brightness = 255;
      nggPinduno.adrLED1()->setBrightness(brightness);
      debug_println("Red Button Double-Click: ALL_LED, Brightness 255");
    } else if (redButton.clickCount == 1 && !redButton.isPressed) {
      // Single press: Toggle NO_LED/ALL_LED
      if (metaState == NO_LED) {
        metaState = ALL_LED;
        debug_println("Red Button Press: NO_LED -> ALL_LED");
      } else if (metaState == ALL_LED) {
        metaState = NO_LED;
        debug_println("Red Button Press: ALL_LED -> NO_LED");
      }
    }
    redButton.clickCount = 0;
  }

  // Detect Blue Button Long Press and Single Press
  if (blueButton.isPressed && !blueButton.isLongPressed && millis() - blueButton.pressStartTime >= LONG_PRESS_DURATION) {
    blueButton.isLongPressed = true;
    if (metaState != NO_LED) {
      previousMetaState = metaState;
      metaState = SET_BRIGHTNESS;
      debug_println("Blue Button Long Press: Entering SET_BRIGHTNESS");
    }
  } else if (!blueButton.isPressed && blueButton.currentState == HIGH && millis() - blueButton.pressStartTime < LONG_PRESS_DURATION && blueButton.pressStartTime > 0) {
    // Single press detection (on release, not a long press)
    if (metaState == ALL_LED) {
      metaState = ALL_LED_RUN_ONLY;
      currentState = GAME_RUN;
      stateChanged = true;
      debug_println("Blue Button Press: ALL_LED -> ALL_LED_RUN_ONLY");
    } else if (metaState == ALL_LED_RUN_ONLY) {
      metaState = ALL_LED;
      currentState = ATTRACT;  // Return to default
      stateChanged = true;
      debug_println("Blue Button Press: ALL_LED_RUN_ONLY -> ALL_LED");
    }
    blueButton.pressStartTime = 0;  // Reset to prevent multiple triggers
  }
}

void handleNoLedState() {
  nggPinduno.adrLED1()->clear();
   nggPinduno.adrLED1()->show(true);
  if (stateChanged) {
    debug_println("NO_LED: LEDs off");
    stateChanged = false;
  }
}

void handleAllLedState() {
  nggPinduno.pinState()->update();
  //debug_print("ALL_LED: Current state = ");
  //debug_println_var(currentState); // Debug Zustandsfluss
  switch (currentState) {
    case ATTRACT:
      handleAttractState();
      break;
    case GAME_RUN:
      handleGameRunState();
      break;
    case EFFECT_ACTIVE:
      handleEffectActiveState();
      break;
  }
  checkPinStates();
}

void handleAllLedRunOnlyState() {
  if (stateChanged) {
    currentState = GAME_RUN;
    stateChanged = true;
    debug_println("ALL_LED_RUN_ONLY: Forcing GAME_RUN");
  }
  handleGameRunState();
}

void handleSetBrightnessState() {
  if (stateChanged) {
    brightness = 1;  // Start at 1
    nggPinduno.adrLED1()->color("red");  // Set LED strip to red
    nggPinduno.adrLED1()->setBrightness(brightness);
    nggPinduno.adrLED1()->show(true);
    lastBrightnessUpdate = millis();
    stateChanged = false;
    debug_println("SET_BRIGHTNESS: Started with red color");
  }

  nggPinduno.pinState()->update(); // Pin-Updates hinzufügen

  if (millis() - lastBrightnessUpdate >= BRIGHTNESS_UPDATE_INTERVAL) {
    brightness++;
    if (brightness > 255) brightness = 1;
    nggPinduno.adrLED1()->setBrightness(brightness);
    nggPinduno.adrLED1()->show(true);
    lastBrightnessUpdate = millis();
    debug_print("Brightness: ");
    debug_print_dec(brightness); debug_println("");
  }
}

// Original State Handlers (Modified)
void handleAttractState() {
  if (stateChanged) {
    stateChanged = false;
    debug_println("Entering ATTRACT mode");
  }

  nggPinduno.adrLED1()->sparkle(currentAttractColor, 20);
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);

  if (millis() - lastColorChangeTime >= attractColorDuration) {
    selectNextAttractColor();
    lastColorChangeTime = millis();
  }
}

void handleGameRunState() {
  if (millis() - lastGameRunColorUpdate >= minRefreshInterval) {
    uint8_t r = constrain(128, 0, 255);
    uint8_t g = constrain(128, 0, 255);
    uint8_t b = constrain(128, 0, 255);

    nggPinduno.adrLED1()->colorRGB(r, g, b);
    nggPinduno.adrLED1()->setBrightness(brightness);
    nggPinduno.adrLED1()->show(true);
    lastGameRunColorUpdate = millis();
  }

  if (stateChanged) {
    stateChanged = false;
    debug_println("GAME_RUN initialized with forced color update");
  }
}

void handleEffectActiveState() {
  if (stateChanged) {
    stateChanged = false;
    debug_println("Entering EFFECT_ACTIVE mode");
  }

  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);

  if (isNOEffectrunning() || (millis() - effectStartTime >= effectDuration)) {
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = EFFECTFINISHED;
    debug_println("Effect completed - Returning to GAME_RUN");
  }
}

boolean isNOEffectrunning() {
  return currentEffectColor == EFFECTFINISHED;
}

void checkPinStates() {
  int trigger = 0;

  if (metaState != ALL_LED && metaState != ALL_LED_RUN_ONLY) return;

  if (currentState == ATTRACT && isAnyPinActive()) {
    currentState = GAME_RUN;
    stateChanged = true;
    timeLastEvent = millis();
  }

  if (currentState == GAME_RUN || currentState == EFFECT_ACTIVE) {
    if (metaState == ALL_LED && nggPinduno.pinState()->J126(10)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      int effectIndex = getRandomEffect(j126_10_effectsUsed, NUM_J126_10_EFFECTS);
      String color1 = getRandomColor();
      String color2 = getRandomColor();
      switch (effectIndex) {
        case 0:
          nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);
          break;
        case 1:
          nggPinduno.adrLED1()->bulletFromPoint2Color(color1, color2, 17, 5, 17);
          break;
        case 2:
          nggPinduno.adrLED1()->color(color1);
          nggPinduno.adrLED1()->spreadOutToPoint(0, 1600);
          break;
        case 3:
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 1000);
          break;
      }
      currentState = GAME_RUN;
      stateChanged = true;
      currentEffectColor = EFFECTFINISHED;
      trigger = 1;
      debug_print("J126(10) Effect ");
      debug_print_dec(effectIndex);
      debug_print(" with colors ");
      debug_print_var(color1);
      if (effectIndex != 2) {
        debug_print(", ");
        debug_print_var(color2);
      }
      debug_println(" - Completed");
    }
    else if (metaState == ALL_LED && nggPinduno.pinState()->J126(9)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      int effectIndex = getRandomEffect(j126_9_effectsUsed, NUM_J126_9_EFFECTS);
      String color1 = getRandomColor();
      String color2 = getRandomColor();
      switch (effectIndex) {
        case 0:
          nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);
          break;
        case 1:
          nggPinduno.adrLED1()->dataStreamNoTail2Color(color1, color2, 20, 20, 1);
          break;
        case 2:
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 1000);
          break;
      }
      currentState = GAME_RUN;
      stateChanged = true;
      currentEffectColor = EFFECTFINISHED;
      trigger = 1;
      debug_print("J126(9) Effect ");
      debug_print_dec(effectIndex);
      debug_print(" with colors ");
      debug_print_var(color1);
      debug_print(", ");
      debug_println_var(color2);
      debug_println(" - Completed");
    }
    else if (metaState == ALL_LED && nggPinduno.pinState()->J126(7)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      int effectIndex = getRandomEffect(j126_7_effectsUsed, NUM_J126_7_EFFECTS);
      String color1 = getRandomColor();
      static int rainbow_count = 0;
      switch (effectIndex) {
        case 0:
          rainbow_count++;
          debug_print("Rainbow start: ");
          debug_println_var(millis());
          nggPinduno.adrLED1()->rainbow(8); // ~16s via ExtendedAddressableStrip
          debug_print("Rainbow end: ");
          debug_println_var(millis());
          debug_print("Rainbow count: ");
          debug_println_var(rainbow_count);
          break;
        case 1:
          nggPinduno.adrLED1()->rainbowCycle(8); // ~16s via ExtendedAddressableStrip
          break;
        case 2:
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color1, 1000);
          break;
      }
      currentState = GAME_RUN;
      stateChanged = true;
      currentEffectColor = EFFECTFINISHED;
      trigger = 1;
      debug_print("J126(7) Effect ");
      debug_print_dec(effectIndex);
      debug_print(" with color ");
      debug_println_var(color1);
      debug_println(" - Completed");
    }
    else if (nggPinduno.pinState()->J126(12)) {
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue", "J126(12)");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) {
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red", "J126(11)");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(6)) {
      nggPinduno.adrLED1()->color("green");
      triggerEffect("green", "J126(6)");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(5)) {
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red", "J126(5)");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(4)) {
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue", "J126(4)");
      trigger = 1;
    }

    if (trigger) {
      nggPinduno.pinState()->reset();
      timeLastEvent = millis();
    }

    if (metaState == ALL_LED && millis() - timeLastEvent > attractTimeout) {
      currentState = ATTRACT;
      stateChanged = true;
      selectNextAttractColor();
      lastColorChangeTime = millis();
    }
  }
}

void triggerEffect(String color, String pin) {
  currentState = EFFECT_ACTIVE;
  stateChanged = true;
  currentEffectColor = color;
  effectStartTime = millis();
  debug_print_var(pin);
  debug_print(": Triggering effect: ");
  debug_println_var(color);
}

String getRandomColor() {
  bool allUsed = true;
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!colorsUsed[i]) {
      allUsed = false;
      break;
    }
  }
  if (allUsed) {
    for (int i = 0; i < NUM_COLORS; i++) {
      colorsUsed[i] = false;
    }
    debug_println("All colors used, resetting color tracking");
  }
  int availableCount = 0;
  int availableIndices[NUM_COLORS];
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!colorsUsed[i]) {
      availableIndices[availableCount++] = i;
    }
  }
  if (availableCount > 0) {
    int selectedIndex = availableIndices[random(availableCount)];
    colorsUsed[selectedIndex] = true;
    debug_print("Selected color: ");
    debug_println_var(availableColors[selectedIndex]);
    return availableColors[selectedIndex];
  }
  return "white";
}

int getRandomEffect(bool* effectsUsed, int numEffects) {
  bool allUsed = true;
  for (int i = 0; i < numEffects; i++) {
    if (!effectsUsed[i]) {
      allUsed = false;
      break;
    }
  }
  if (allUsed) {
    for (int i = 0; i < numEffects; i++) {
      effectsUsed[i] = false;
    }
    debug_println("All effects used, resetting effect tracking");
  }
  int availableCount = 0;
  int availableIndices[numEffects];
  for (int i = 0; i < numEffects; i++) {
    if (!effectsUsed[i]) {
      availableIndices[availableCount++] = i;
    }
  }
  if (availableCount > 0) {
    int selectedIndex = availableIndices[random(availableCount)];
    effectsUsed[selectedIndex] = true;
    return selectedIndex;
  }
  return 0;
}

void selectNextAttractColor() {
  bool allShown = true;
  for (int i = 0; i < 3; i++) {
    if (!colorsShown[i]) {
      allShown = false;
      break;
    }
  }
  if (allShown) {
    for (int i = 0; i < 3; i++) colorsShown[i] = false;
  }
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