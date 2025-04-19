#include "pinduinoext.h"

// Debug Configuration
#define DEBUG 1  // Set to 1 for debugging, revert to 0 after testing
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
const int RED_BUTTON_PIN = 6;   // D6 - Red Button
const int BLUE_BUTTON_PIN = 7;  // D7 - Blue Button
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long LONG_PRESS_DURATION = 1000;  // 1s
const unsigned long DOUBLE_CLICK_WINDOW = 1000;  // 1s
const unsigned long BRIGHTNESS_UPDATE_INTERVAL = 50;  // 50ms

// Meta-State Machine
enum MetaGameState { NO_LED, ALL_LED, ALL_LED_RUN_ONLY, SET_BRIGHTNESS, SET_STATICCOLOR };
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

// Color Selection for SET_STATICCOLOR
const int NUM_STATIC_COLORS = 13;
const String staticColors[] = {"white", "red", "green", "blue", "yellow", "cyan", "purple", "orange", "lime", "sky", "mint", "magenta", "lavender"}; // Kept for debug
int staticColorIndex = 0;  // Start with "white"

// RGB values for staticColors
uint32_t staticColorRGB[NUM_STATIC_COLORS] = {
  nggPinduno.adrLED1()->strip()->Color(255, 255, 255), // white
  nggPinduno.adrLED1()->strip()->Color(255, 0, 0),     // red
  nggPinduno.adrLED1()->strip()->Color(0, 255, 0),     // green
  nggPinduno.adrLED1()->strip()->Color(0, 0, 255),     // blue
  nggPinduno.adrLED1()->strip()->Color(255, 255, 0),   // yellow
  nggPinduno.adrLED1()->strip()->Color(0, 255, 255),   // cyan
  nggPinduno.adrLED1()->strip()->Color(128, 0, 128),   // purple
  nggPinduno.adrLED1()->strip()->Color(255, 165, 0),   // orange
  nggPinduno.adrLED1()->strip()->Color(50, 205, 50),   // lime
  nggPinduno.adrLED1()->strip()->Color(135, 206, 235), // sky
  nggPinduno.adrLED1()->strip()->Color(62, 245, 180),  // mint
  nggPinduno.adrLED1()->strip()->Color(255, 0, 255),   // magenta
  nggPinduno.adrLED1()->strip()->Color(230, 230, 250)  // lavender
};

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

// Method to set all LEDs to the specified color from staticColorRGB
void setStripColor(int colorIndex) {
  for (int i = 0; i < aLEDNum1; i++) {
    nggPinduno.adrLED1()->strip()->setPixelColor(i, staticColorRGB[colorIndex]);
  }
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
}

void setup() {
  #if DEBUG == 1
    Serial.begin(115200);
  #endif
  debug_println("System Initializing...");
  debug_println("LED Control v3.1 - With Button Enhancements");

  // Power stabilization
  delay(500);

  // Initialize LEDs
  nggPinduno.adrLED1()->strip()->begin();
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  nggPinduno.pinState()->reset();

  selectNextAttractColor();
  debug_println("Initialization Complete");

  // Initialize Buttons
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  updateButtonStates();
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
    case SET_STATICCOLOR:
      handleSetStaticColorState();
      break;
  }
}

void updateButtonStates() {
  bool redReading = digitalRead(RED_BUTTON_PIN);
  bool blueReading = digitalRead(BLUE_BUTTON_PIN);

  // Red Button Debounce
  if (redReading != redButton.lastState) {
    redButton.lastDebounceTime = millis();
  }
  if (millis() - redButton.lastDebounceTime > DEBOUNCE_DELAY) {
    if (redReading != redButton.currentState) {
      redButton.currentState = redReading;
      if (redReading == LOW) {
        redButton.isPressed = true;
        redButton.pressStartTime = millis();
        redButton.clickCount++;
        if (redButton.clickCount == 1) {
          redButton.lastClickTime = millis();
        }
      } else {
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
      if (blueReading == LOW) {
        blueButton.isPressed = true;
        blueButton.pressStartTime = millis();
        blueButton.isLongPressed = false;
        blueButton.clickCount++;
        if (blueButton.clickCount == 1) {
          blueButton.lastClickTime = millis();
        }
      } else {
        blueButton.isPressed = false;
        if (blueButton.isLongPressed && metaState == SET_BRIGHTNESS) {
          metaState = previousMetaState;
          if (metaState == NO_LED) {
            nggPinduno.adrLED1()->clear();
            nggPinduno.adrLED1()->setBrightness(255);
            nggPinduno.adrLED1()->show(true);
            stateChanged = true;
            debug_println("SET_BRIGHTNESS -> NO_LED: LEDs cleared");
          } else {
            nggPinduno.adrLED1()->setBrightness(brightness);
            stateChanged = true;
          }
          debug_print("Brightness set to: ");
          debug_print_dec(brightness);
          debug_println("");
        }
      }
    }
  }
  blueButton.lastState = blueReading;

  // Detect Red Button Double-Click
  if (redButton.clickCount > 0 && millis() - redButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (redButton.clickCount >= 2) {
      // Double-click: Set ALL_LED, brightness 255, color white
      metaState = ALL_LED;
      brightness = 255;
      staticColorIndex = 0;  // "white" in staticColors/staticColorRGB
      nggPinduno.adrLED1()->setBrightness(brightness);
      setStripColor(staticColorIndex);
      stateChanged = true;
      debug_println("Red Button Double-Click: ALL_LED, Brightness 255, Color white");
    } else if (redButton.clickCount == 1 && !redButton.isPressed) {
      // Single press
      if (metaState == NO_LED) {
        metaState = ALL_LED;
        debug_println("Red Button Press: NO_LED -> ALL_LED");
      } else if (metaState == ALL_LED) {
        metaState = NO_LED;
        nggPinduno.adrLED1()->clear();
        nggPinduno.adrLED1()->setBrightness(255);
        nggPinduno.adrLED1()->show(true);
        currentEffectColor = EFFECTFINISHED;
        debug_println("Red Button Press: ALL_LED -> NO_LED, LEDs cleared");
      } else if (metaState == SET_STATICCOLOR) {
        staticColorIndex = (staticColorIndex + 1) % NUM_STATIC_COLORS;
        setStripColor(staticColorIndex);
        debug_print("Red Button Press: Color set to ");
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(" (index ");
        debug_print_dec(staticColorIndex);
        debug_println(")");
      }
    }
    redButton.clickCount = 0;
  }

  // Detect Blue Button Actions
  if (blueButton.clickCount > 0 && millis() - blueButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (blueButton.clickCount >= 2 && !blueButton.isPressed) {
      if (metaState == SET_STATICCOLOR) {
        metaState = previousMetaState;
        stateChanged = true;
        if (metaState == NO_LED) {
          nggPinduno.adrLED1()->clear();
          nggPinduno.adrLED1()->setBrightness(255);
          nggPinduno.adrLED1()->show(true);
          debug_println("Blue Button Double-Click: SET_STATICCOLOR -> NO_LED, LEDs cleared");
        } else {
          nggPinduno.adrLED1()->setBrightness(brightness);
          debug_print("Blue Button Double-Click: SET_STATICCOLOR -> ");
          debug_println_var(metaState);
        }
      }
      blueButton.clickCount = 0;
    } else if (blueButton.clickCount == 1 && !blueButton.isPressed && !blueButton.isLongPressed) {
      if (metaState == NO_LED) {
        previousMetaState = metaState;
        metaState = SET_STATICCOLOR;
        staticColorIndex = 0;
        setStripColor(staticColorIndex);
        stateChanged = true;
        debug_println("Blue Button Press: NO_LED -> SET_STATICCOLOR, starting with white");
      } else if (metaState == ALL_LED) {
        metaState = ALL_LED_RUN_ONLY;
        currentState = GAME_RUN;
        stateChanged = true;
        debug_println("Blue Button Press: ALL_LED -> ALL_LED_RUN_ONLY");
      } else if (metaState == ALL_LED_RUN_ONLY) {
        metaState = ALL_LED;
        currentState = ATTRACT;
        stateChanged = true;
        debug_println("Blue Button Press: ALL_LED_RUN_ONLY -> ALL_LED");
      } else if (metaState == SET_STATICCOLOR) {
        staticColorIndex = 0;
        setStripColor(staticColorIndex);
        debug_print("Blue Button Press: Color reset to ");
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(" (index ");
        debug_print_dec(staticColorIndex);
        debug_println(")");
      }
      blueButton.clickCount = 0;
    }
  }

  // Detect Blue Button Long Press
  if (blueButton.isPressed && !blueButton.isLongPressed && millis() - blueButton.pressStartTime >= LONG_PRESS_DURATION) {
    blueButton.isLongPressed = true;
    if (metaState != NO_LED && metaState != SET_STATICCOLOR) {
      previousMetaState = metaState;
      metaState = SET_BRIGHTNESS;
      stateChanged = true;
      debug_println("Blue Button Long Press: Entering SET_BRIGHTNESS");
    }
    blueButton.clickCount = 0;
  }
}

void handleNoLedState() {
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  if (stateChanged) {
    debug_println("NO_LED: LEDs off");
    stateChanged = false;
  }
}

void handleAllLedState() {
  if (stateChanged) {
    currentEffectColor = EFFECTFINISHED;
    debug_println("ALL_LED: Reset effects on state change");
  }
  nggPinduno.pinState()->update();
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
    currentEffectColor = EFFECTFINISHED;
    stateChanged = true;
    debug_println("ALL_LED_RUN_ONLY: Forcing GAME_RUN, effects reset");
  }
  handleGameRunState();
}

void blinkGreen(int numBlinks, bool holdAfter) {
  for (int i = 0; i < numBlinks; i++) {
    nggPinduno.adrLED1()->color("green");
    nggPinduno.adrLED1()->setBrightness(255);
    nggPinduno.adrLED1()->show(true);
    delay(1000);
    nggPinduno.adrLED1()->clear();
    nggPinduno.adrLED1()->show(true);
    delay(200);
  }
  if (holdAfter) {
    nggPinduno.adrLED1()->color("green");
    nggPinduno.adrLED1()->setBrightness(255);
    nggPinduno.adrLED1()->show(true);
    delay(3000);
  }
}

void handleSetBrightnessState() {
  nggPinduno.adrLED1()->color("red");
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  if (stateChanged) {
    brightness = 1;
    lastBrightnessUpdate = millis();
    stateChanged = false;
    debug_println("SET_BRIGHTNESS: Started with red color");
  }
  nggPinduno.pinState()->update();
  if (millis() - lastBrightnessUpdate >= BRIGHTNESS_UPDATE_INTERVAL) {
    brightness++;
    if (brightness > 255) brightness = 1;
    if (brightness == 200) {
      debug_println("SET_BRIGHTNESS: Brightness 200 - Blink 1x green");
      blinkGreen(1, false);
      nggPinduno.adrLED1()->color("red");
      nggPinduno.adrLED1()->setBrightness(brightness);
      nggPinduno.adrLED1()->show(true);
    } else if (brightness == 230) {
      debug_println("SET_BRIGHTNESS: Brightness 230 - Blink 2x green");
      blinkGreen(2, false);
      nggPinduno.adrLED1()->color("red");
      nggPinduno.adrLED1()->setBrightness(brightness);
      nggPinduno.adrLED1()->show(true);
    } else if (brightness == 255) {
      debug_println("SET_BRIGHTNESS: Brightness 255 - Blink 3x green + 3s hold");
      blinkGreen(3, true);
      nggPinduno.adrLED1()->color("red");
      nggPinduno.adrLED1()->setBrightness(brightness);
      nggPinduno.adrLED1()->show(true);
    }
    nggPinduno.adrLED1()->setBrightness(brightness);
    nggPinduno.adrLED1()->show(true);
    lastBrightnessUpdate = millis();
    debug_print("Brightness: ");
    debug_print_dec(brightness);
    debug_println("");
  }
}

void handleSetStaticColorState() {
  setStripColor(staticColorIndex);
  if (stateChanged) {
    debug_print("SET_STATICCOLOR: Showing color ");
    debug_print_var(staticColors[staticColorIndex]);
    debug_print(" (index ");
    debug_print_dec(staticColorIndex);
    debug_println(")");
    stateChanged = false;
  }
}

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
    setStripColor(staticColorIndex);
    lastGameRunColorUpdate = millis();
  }
  if (stateChanged) {
    stateChanged = false;
    debug_print("GAME_RUN: Using static color ");
    debug_print_var(staticColors[staticColorIndex]);
    debug_print(" (index ");
    debug_print_dec(staticColorIndex);
    debug_println(")");
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
      currentEffectColor = EFFECTFINISHED; // Fixed typo
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
      currentEffectColor = EFFECTFINISHED; // Fixed typo
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
      currentEffectColor = EFFECTFINISHED; // Fixed typo
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