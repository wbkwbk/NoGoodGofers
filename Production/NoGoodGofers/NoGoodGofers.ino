#include "pinduinoext.h"

// Debug Configuration
#define DEBUG_LEVEL 0  // 0: Off, 1: Full, 2: Only checkPinStates
#if DEBUG_LEVEL == 1
  #define debug_print(x) Serial.print(x)
  #define debug_print_dec(x) Serial.print(x, DEC)
  #define debug_println(x) Serial.println(x)
  #define debug_print_var(x) Serial.print(x)
  #define debug_println_var(x) Serial.println(x)
  #define debug_println_dec(x) Serial.println(x, DEC)
  #define pin_debug_print(x) Serial.print(x)
  #define pin_debug_print_dec(x) Serial.print(x, DEC)
  #define pin_debug_println(x) Serial.println(x)
  #define pin_debug_print_var(x) Serial.print(x)
  #define pin_debug_println_var(x) Serial.println(x)
  #define pin_debug_println_dec(x) Serial.println(x, DEC)
#elif DEBUG_LEVEL == 2
  #define debug_print(x)
  #define debug_print_dec(x)
  #define debug_println(x)
  #define debug_print_var(x)
  #define debug_println_var(x)
  #define debug_println_dec(x)
  #define pin_debug_print(x) Serial.print(x)
  #define pin_debug_print_dec(x) Serial.print(x, DEC)
  #define pin_debug_println(x) Serial.println(x)
  #define pin_debug_print_var(x) Serial.print(x)
  #define pin_debug_println_var(x) Serial.println(x)
  #define pin_debug_println_dec(x) Serial.println(x, DEC)
#else // DEBUG_LEVEL == 0
  #define debug_print(x)
  #define debug_print_dec(x)
  #define debug_println(x)
  #define debug_print_var(x)
  #define debug_println_var(x)
  #define debug_println_dec(x)
  #define pin_debug_print(x)
  #define pin_debug_print_dec(x)
  #define pin_debug_println(x)
  #define pin_debug_print_var(x)
  #define pin_debug_println_var(x)
  #define pin_debug_println_dec(x)
#endif

// LED Configuration
const int aLEDNum1 = 80;
const int aLEDNum2 = 0;
const int aLEDNum3 = 0;
const char* EFFECTFINISHED = "";
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// Button Configuration
const int RED_BUTTON_PIN = 6;   // D6 - Red Button
const int BLUE_BUTTON_PIN = 7;  // D7 - Blue Button
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long LONG_PRESS_DURATION = 1000;  // 1s
const unsigned long DOUBLE_CLICK_WINDOW = 1000;  // 1s
const unsigned long BRIGHTNESS_UPDATE_INTERVAL = 15;  // 15ms

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

// Forward declaration
void updateButtonState(ButtonState& button, int pin, const char* name, unsigned long currentTime);

ButtonState redButton = {HIGH, HIGH, 0, 0, false, false, 0, 0};
ButtonState blueButton = {HIGH, HIGH, 0, 0, false, false, 0, 0};

// Timing Constants
const unsigned long effectDuration = 2000;
const unsigned long attractColorDuration = 5000;
const unsigned long attractTimeout = 20000;

// Blink Green State
struct BlinkGreenState {
  bool active;
  int numBlinks;
  int currentBlink;
  bool holdAfter;
  bool isOn;
  unsigned long lastUpdate;
  unsigned long onDuration;
  unsigned long offDuration;
  unsigned long holdDuration;
};

BlinkGreenState blinkGreenState = {false, 0, 0, false, false, 0, 1000, 200, 3000};

// Game Variables
unsigned long timeLastEvent = 0;
unsigned long lastColorChangeTime = 0;
unsigned long effectStartTime = 0;
unsigned long lastBrightnessUpdate = 0;
const char* currentEffectColor = "";
const char* currentAttractColor = "";
int attractColorIndex = 0;
bool colorsShown[3] = {false, false, false};
uint8_t brightness = 255;  // Default brightness
int currentStaticColorIndex = -1;  // Track current static color

// Static Colors for SET_STATICCOLOR and Simple Effects
const int NUM_STATIC_COLORS = 13;
const char* staticColors[] = {
  "white", "red", "green", "blue", "yellow", "cyan", "purple", 
  "orange", "lime", "sky", "mint", "magenta", "lavender"
};

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

int staticColorIndex = 0;  // Start with "white"

// Colors for Complex Effects
const int NUM_COLORS = 13;
const char* availableColors[] = {
  "red", "green", "blue", "yellow", "cyan", "purple", "white", 
  "orange", "lime", "sky", "mint", "magenta", "lavender"
};

// Effect Tracking for Complex Effects
const int NUM_J126_10_EFFECTS = 4;
bool j126_10_effectsUsed[NUM_J126_10_EFFECTS] = {false};
const int NUM_J126_9_EFFECTS = 3;
bool j126_9_effectsUsed[NUM_J126_9_EFFECTS] = {false};
const int NUM_J126_7_EFFECTS = 3;
bool j126_7_effectsUsed[NUM_J126_7_EFFECTS] = {false};

// Effect Names for Debug Output
const char* j126_10_effectNames[] = {
  "bullet2Color", "bulletFromPoint2Color", "spreadOutToPoint", "spreadInFromPoint2Color"
};
const char* j126_9_effectNames[] = {
  "bullet2Color", "dataStreamNoTail2Color", "spreadInFromPoint2Color"
};
const char* j126_7_effectNames[] = {
  "rainbowWS2812FX", "rainbowCycleWS2812FX", "spreadInFromPoint2Color"
};

// Method to set all LEDs to the specified color
void setStripColor(int colorIndex) {
  nggPinduno.adrLED1()->strip()->fill(staticColorRGB[colorIndex], 0, aLEDNum1);
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  currentStaticColorIndex = colorIndex;
}

// Debug routine for complex effects
void debugCheckPinStateStart(const char* effectName, const char* pinId, int effectIndex, const char* color1, const char* color2, bool color2Applicable) {
  unsigned long startTime = millis();
  pin_debug_print("[EFFECT]: ");
  pin_debug_print(effectName ? effectName : "InvalidEffect");
  pin_debug_print(" (");
  pin_debug_print(pinId);
  pin_debug_print(", index ");
  pin_debug_print_dec(effectIndex);
  pin_debug_println(")");
  pin_debug_print("Start Time: ");
  pin_debug_println_dec(startTime);
  pin_debug_print("[COLOR] 1: Selected: ");
  pin_debug_println(color1 ? color1 : "None");
  pin_debug_print("[COLOR] 2: Selected: ");
  if (color2Applicable) {
    pin_debug_println(color2 ? color2 : "None");
  } else {
    pin_debug_println("N/A");
  }
  Serial.flush();
}

// Debug routine for complex effects: end time
void debugCheckPinStateEnd() {
  unsigned long endTime = millis();
  pin_debug_print("End Time: ");
  pin_debug_println_dec(endTime);
  Serial.flush();
}

void setup() {
  #if DEBUG_LEVEL == 1 || DEBUG_LEVEL == 2
    Serial.begin(115200);
  #endif
  randomSeed(analogRead(0));
  debug_println("[INIT] System Initializing...");
  debug_println("[INIT] LED Control v3.1 - Enhanced Debug Output");

  // Initialize button pins
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);

  // Power stabilization
  delay(500);

  // Initialize LEDs
  nggPinduno.adrLED1()->strip()->begin();
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  nggPinduno.pinState()->reset();

  selectNextAttractColor();
  debug_print("[INIT] Initialization Complete, Starting in NO_LED, Attract Color: ");
  debug_println_var(currentAttractColor);
}

void loop() {
  unsigned long currentTime = millis();
  updateButtonStates(currentTime);
  switch (metaState) {
    case NO_LED:
      handleNoLedState();
      break;
    case ALL_LED:
      handleAllLedState(currentTime);
      break;
    case ALL_LED_RUN_ONLY:
      handleAllLedRunOnlyState();
      break;
    case SET_BRIGHTNESS:
      handleSetBrightnessState(currentTime);
      break;
    case SET_STATICCOLOR:
      handleSetStaticColorState();
      break;
  }
}

void updateButtonState(ButtonState& button, int pin, const char* name, unsigned long currentTime) {
  bool reading = digitalRead(pin);
  if (reading != button.lastState) {
    button.lastDebounceTime = currentTime;
  }
  if (currentTime - button.lastDebounceTime > DEBOUNCE_DELAY) {
    if (reading != button.currentState) {
      button.currentState = reading;
      if (reading == LOW) {
        button.isPressed = true;
        button.pressStartTime = currentTime;
        button.clickCount++;
        if (button.clickCount == 1) {
          button.lastClickTime = currentTime;
          debug_print("[BUTTON] ");
          debug_print(name);
          debug_print(" Press Detected at ");
          debug_println_var(currentTime);
        }
      } else {
        button.isPressed = false;
        if (button.isLongPressed && metaState == SET_BRIGHTNESS && pin == BLUE_BUTTON_PIN) {
          debug_print("[STATE] Exiting SET_BRIGHTNESS to ");
          debug_print_var(previousMetaState);
          debug_print(", Brightness: ");
          debug_print_dec(brightness);
          metaState = previousMetaState;
          blinkGreenState.active = false;
          if (metaState == NO_LED) {
            nggPinduno.adrLED1()->clear();
            nggPinduno.adrLED1()->show(true);
            currentStaticColorIndex = -1;
            debug_println(" - LEDs Cleared");
          } else {
            nggPinduno.adrLED1()->setBrightness(brightness);
            setStripColor(staticColorIndex);
            debug_println(" - Brightness Preserved");
          }
          stateChanged = true;
        }
      }
    }
  }
  button.lastState = reading;

  if (pin == BLUE_BUTTON_PIN && button.isPressed && !button.isLongPressed && 
      currentTime - button.pressStartTime >= LONG_PRESS_DURATION) {
    button.isLongPressed = true;
    if (metaState != NO_LED && metaState != SET_STATICCOLOR) {
      previousMetaState = metaState;
      metaState = SET_BRIGHTNESS;
      stateChanged = true;
      debug_print("[STATE] Blue Long Press: Entering SET_BRIGHTNESS from ");
      debug_println_var(previousMetaState);
    }
    button.clickCount = 0;
  }
}

void updateButtonStates(unsigned long currentTime) {
  updateButtonState(redButton, RED_BUTTON_PIN, "Red", currentTime);
  updateButtonState(blueButton, BLUE_BUTTON_PIN, "Blue", currentTime);

  if (redButton.clickCount > 0 && currentTime - redButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (redButton.clickCount >= 2) {
      if (metaState == SET_STATICCOLOR) {
        staticColorIndex = 0;
        setStripColor(staticColorIndex);
        debug_print("[BUTTON] Red Double-Click in SET_STATICCOLOR: Reset to ");
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(" (index ");
        debug_print_dec(staticColorIndex);
        debug_println(")");
      } else {
        debug_print("[STATE] Red Double-Click: Transition to ALL_LED, Brightness: 255, Color: ");
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(" (index ");
        debug_print_dec(staticColorIndex);
        debug_println(")");
        metaState = ALL_LED;
        brightness = 255;
        nggPinduno.adrLED1()->setBrightness(brightness);
        setStripColor(staticColorIndex);
        stateChanged = true;
      }
    } else if (redButton.clickCount == 1 && !redButton.isPressed) {
      if (metaState == NO_LED) {
        debug_println("[STATE] Red Single Press: NO_LED -> ALL_LED");
        metaState = ALL_LED;
        stateChanged = true;
      } else if (metaState == ALL_LED) {
        debug_println("[STATE] Red Single Press: ALL_LED -> NO_LED, LEDs Cleared");
        metaState = NO_LED;
        nggPinduno.adrLED1()->clear();
        nggPinduno.adrLED1()->show(true);
        currentStaticColorIndex = -1;
        currentEffectColor = EFFECTFINISHED;
        stateChanged = true;
      } else if (metaState == SET_STATICCOLOR) {
        staticColorIndex = (staticColorIndex + 1) % NUM_STATIC_COLORS;
        setStripColor(staticColorIndex);
        debug_print("[BUTTON] Red Single Press: Cycling to ");
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(" (index ");
        debug_print_dec(staticColorIndex);
        debug_println(")");
      }
    }
    redButton.clickCount = 0;
  }

  if (blueButton.clickCount > 0 && currentTime - blueButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (blueButton.clickCount >= 1 && !blueButton.isPressed && !blueButton.isLongPressed) {
      if (metaState == NO_LED) {
        previousMetaState = metaState;
        metaState = SET_STATICCOLOR;
        staticColorIndex = 0;
        setStripColor(staticColorIndex);
        stateChanged = true;
        debug_println("[STATE] Blue Press: NO_LED -> SET_STATICCOLOR, Starting with white");
      } else if (metaState == ALL_LED) {
        metaState = ALL_LED_RUN_ONLY;
        currentState = GAME_RUN;
        stateChanged = true;
        debug_println("[STATE] Blue Press: ALL_LED -> ALL_LED_RUN_ONLY, Forcing GAME_RUN");
      } else if (metaState == ALL_LED_RUN_ONLY) {
        metaState = ALL_LED;
        currentState = ATTRACT;
        stateChanged = true;
        debug_println("[STATE] Blue Press: ALL_LED_RUN_ONLY -> ALL_LED, Entering ATTRACT");
      } else if (metaState == SET_STATICCOLOR) {
        debug_print("[STATE] Blue Press: Exiting SET_STATICCOLOR to ");
        debug_print_var(previousMetaState);
        debug_print(", Color: ");
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(" (index ");
        debug_print_dec(staticColorIndex);
        debug_println(")");
        metaState = previousMetaState;
        stateChanged = true;
        if (metaState == NO_LED) {
          nggPinduno.adrLED1()->clear();
          nggPinduno.adrLED1()->show(true);
          currentStaticColorIndex = -1;
          debug_println(" - LEDs Cleared");
        } else {
          nggPinduno.adrLED1()->setBrightness(brightness);
          setStripColor(staticColorIndex);
          debug_println(" - Color Preserved");
        }
      }
      blueButton.clickCount = 0;
    }
  }
}

void handleNoLedState() {
  if (stateChanged) {
    nggPinduno.adrLED1()->clear();
    nggPinduno.adrLED1()->show(true);
    currentStaticColorIndex = -1;
    debug_print("[STATE] NO_LED: LEDs Off, Brightness: ");
    debug_println_dec(brightness);
    stateChanged = false;
  }
}

void handleAllLedState(unsigned long currentTime) {
  if (stateChanged) {
    currentEffectColor = EFFECTFINISHED;
    debug_print("[STATE] ALL_LED: Reset Effects, Current State: ");
    debug_println_var(currentState);
  }
  nggPinduno.pinState()->update();
  switch (currentState) {
    case ATTRACT:
      handleAttractState(currentTime);
      break;
    case GAME_RUN:
      handleGameRunState();
      break;
    case EFFECT_ACTIVE:
      handleEffectActiveState(currentTime);
      break;
  }
  checkPinStates(currentTime);
}

void handleAllLedRunOnlyState() {
  if (stateChanged) {
    currentState = GAME_RUN;
    currentEffectColor = EFFECTFINISHED;
    debug_println("[STATE] ALL_LED_RUN_ONLY: Forcing GAME_RUN, Effects Reset");
    stateChanged = true;
  }
  handleGameRunState();
}

void blinkGreen(int numBlinks, bool holdAfter) {
  blinkGreenState.active = true;
  blinkGreenState.numBlinks = numBlinks;
  blinkGreenState.currentBlink = 0;
  blinkGreenState.holdAfter = holdAfter;
  blinkGreenState.isOn = true;
  blinkGreenState.lastUpdate = millis();
  nggPinduno.adrLED1()->color("green");
  nggPinduno.adrLED1()->setBrightness(255);
  nggPinduno.adrLED1()->show(true);
  currentStaticColorIndex = -1;
  debug_print("[EFFECT] Blinked Green Started, Blinks: ");
  debug_print_dec(numBlinks);
  debug_print(", Hold: ");
  debug_println_var(holdAfter);
}

void handleSetBrightnessState(unsigned long currentTime) {
  if (blinkGreenState.active) {
    if (blinkGreenState.isOn) {
      if (currentTime - blinkGreenState.lastUpdate >= blinkGreenState.onDuration) {
        nggPinduno.adrLED1()->clear();
        nggPinduno.adrLED1()->show(true);
        blinkGreenState.isOn = false;
        blinkGreenState.lastUpdate = currentTime;
      }
    } else {
      if (currentTime - blinkGreenState.lastUpdate >= blinkGreenState.offDuration) {
        blinkGreenState.currentBlink++;
        if (blinkGreenState.currentBlink < blinkGreenState.numBlinks) {
          nggPinduno.adrLED1()->color("green");
          nggPinduno.adrLED1()->setBrightness(255);
          nggPinduno.adrLED1()->show(true);
          blinkGreenState.isOn = true;
          blinkGreenState.lastUpdate = currentTime;
        } else if (blinkGreenState.holdAfter) {
          nggPinduno.adrLED1()->color("green");
          nggPinduno.adrLED1()->setBrightness(255);
          nggPinduno.adrLED1()->show(true);
          blinkGreenState.isOn = true;
          blinkGreenState.lastUpdate = currentTime;
          blinkGreenState.numBlinks = 0;
          blinkGreenState.onDuration = blinkGreenState.holdDuration;
        } else {
          blinkGreenState.active = false;
          nggPinduno.adrLED1()->color("red");
          nggPinduno.adrLED1()->setBrightness(brightness);
          nggPinduno.adrLED1()->show(true);
          currentStaticColorIndex = -1;
          debug_print("[EFFECT] Blinked Green Completed, Blinks: ");
          debug_println_dec(blinkGreenState.numBlinks);
        }
      }
    }
    if (blinkGreenState.active && blinkGreenState.holdAfter && blinkGreenState.currentBlink >= blinkGreenState.numBlinks && !blinkGreenState.numBlinks) {
      if (currentTime - blinkGreenState.lastUpdate >= blinkGreenState.holdDuration) {
        blinkGreenState.active = false;
        nggPinduno.adrLED1()->color("red");
        nggPinduno.adrLED1()->setBrightness(brightness);
        nggPinduno.adrLED1()->show(true);
        currentStaticColorIndex = -1;
        debug_print("[EFFECT] Blinked Green Hold Completed, Duration: ");
        debug_println_dec(blinkGreenState.holdDuration);
      }
    }
  } else {
    nggPinduno.adrLED1()->color("red");
    nggPinduno.adrLED1()->setBrightness(brightness);
    nggPinduno.adrLED1()->show(true);
    currentStaticColorIndex = -1;
  }

  if (stateChanged) {
    brightness = 1;
    lastBrightnessUpdate = currentTime;
    stateChanged = false;
    blinkGreenState.active = false;
    debug_println("[STATE] SET_BRIGHTNESS: Started with Red, Brightness Cycling");
  }

  nggPinduno.pinState()->update();
  if (!blinkGreenState.active && currentTime - lastBrightnessUpdate >= BRIGHTNESS_UPDATE_INTERVAL) {
    brightness++;
    if (brightness > 255) brightness = 1;
    if (brightness == 200) {
      debug_print("[EFFECT] SET_BRIGHTNESS: Brightness 200 - Blink 1x Green, Current Brightness: ");
      debug_println_dec(brightness);
      blinkGreen(1, false);
    } else if (brightness == 230) {
      debug_print("[EFFECT] SET_BRIGHTNESS: Brightness 230 - Blink 2x Green, Current Brightness: ");
      debug_println_dec(brightness);
      blinkGreen(2, false);
    } else if (brightness == 255) {
      debug_print("[EFFECT] SET_BRIGHTNESS: Brightness 255 - Blink 3x Green + 3s Hold, Current Brightness: ");
      debug_println_dec(brightness);
      blinkGreen(3, true);
    }
    if (!blinkGreenState.active) {
      nggPinduno.adrLED1()->setBrightness(brightness);
      nggPinduno.adrLED1()->show(true);
      currentStaticColorIndex = -1;
      lastBrightnessUpdate = currentTime;
      debug_print("[BRIGHTNESS] Current: ");
      debug_println_dec(brightness);
    }
  }
}

void handleSetStaticColorState() {
  if (stateChanged || currentStaticColorIndex != staticColorIndex) {
    setStripColor(staticColorIndex);
    debug_print("[STATE] SET_STATICCOLOR: Showing ");
    debug_print_var(staticColors[staticColorIndex]);
    debug_print(" (index ");
    debug_print_dec(staticColorIndex);
    debug_println(")");
    stateChanged = false;
  }
}

void handleAttractState(unsigned long currentTime) {
  if (stateChanged) {
    stateChanged = false;
    debug_print("[STATE] ATTRACT: Sparkle with ");
    debug_println_var(currentAttractColor);
  }
  nggPinduno.adrLED1()->sparkle(currentAttractColor, 20);
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  currentStaticColorIndex = -1;
  if (currentTime - lastColorChangeTime >= attractColorDuration) {
    selectNextAttractColor();
    lastColorChangeTime = currentTime;
  }
}

void handleGameRunState() {
  if (stateChanged || currentStaticColorIndex != staticColorIndex) {
    setStripColor(staticColorIndex);
    debug_print("[STATE] GAME_RUN: Using static color ");
    debug_print_var(staticColors[staticColorIndex]);
    debug_print(" (index ");
    debug_print_dec(staticColorIndex);
    debug_println(")");
    stateChanged = false;
  }
}

void handleEffectActiveState(unsigned long currentTime) {
  if (stateChanged) {
    stateChanged = false;
    debug_print("[STATE] EFFECT_ACTIVE: Running Effect, Color: ");
    debug_println_var(currentEffectColor);
  }
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  currentStaticColorIndex = -1;
  if (isNOEffectrunning() || (currentTime - effectStartTime >= effectDuration)) {
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = EFFECTFINISHED;
    debug_println("[STATE] EFFECT_ACTIVE: Effect Completed, Returning to GAME_RUN");
  }
}

boolean isNOEffectrunning() {
  return currentEffectColor == EFFECTFINISHED;
}

const char* getRandomColorFromArray(const char* colors[], int numColors, const char* debugPrefix) {
  static bool simpleEffectColorsUsed[NUM_STATIC_COLORS] = {false};
  static bool complexEffectColorsUsed[NUM_COLORS] = {false};
  bool* used = (strcmp(debugPrefix, "[SIMPLE COLOR]") == 0) ? simpleEffectColorsUsed : complexEffectColorsUsed;

  bool allUsed = true;
  for (int i = 0; i < numColors; i++) {
    if (!used[i]) {
      allUsed = false;
      break;
    }
  }
  if (allUsed) {
    for (int i = 0; i < numColors; i++) {
      used[i] = false;
    }
    pin_debug_print(debugPrefix);
    pin_debug_println(" All Colors Used, Resetting");
  }
  int availableCount = 0;
  int availableIndices[numColors];
  for (int i = 0; i < numColors; i++) {
    if (!used[i]) {
      availableIndices[availableCount++] = i;
    }
  }
  if (availableCount > 0) {
    int selectedIndex = availableIndices[random(availableCount)];
    used[selectedIndex] = true;
    pin_debug_print(debugPrefix);
    pin_debug_print(" Selected: ");
    pin_debug_print_var(colors[selectedIndex]);
    pin_debug_print(" (index ");
    pin_debug_print_dec(selectedIndex);
    pin_debug_println(")");
    return colors[selectedIndex];
  }
  pin_debug_print(debugPrefix);
  pin_debug_println(" No Colors Available, Defaulting to white");
  return colors[0];  // white (first in both arrays)
}

void checkPinStates(unsigned long currentTime) {
  int trigger = 0;
  if (metaState != ALL_LED && metaState != ALL_LED_RUN_ONLY) return;

  if (currentState == ATTRACT && isAnyPinActive()) {
    currentState = GAME_RUN;
    stateChanged = true;
    timeLastEvent = currentTime;
    pin_debug_println("[STATE] ATTRACT -> GAME_RUN: Pin Activity Detected");
  }

  if (currentState != GAME_RUN && currentState != EFFECT_ACTIVE) return;

  char pinId[10];
  if (metaState == ALL_LED && nggPinduno.pinState()->J126(10)) {
    currentState = EFFECT_ACTIVE;
    stateChanged = true;
    snprintf(pinId, sizeof(pinId), "J126(10)");
    int effectIndex = getRandomEffect(j126_10_effectsUsed, NUM_J126_10_EFFECTS);
    const char* color1 = getRandomColorFromArray(availableColors, NUM_COLORS, "[COLOR]");
    const char* color2 = getRandomColorFromArray(availableColors, NUM_COLORS, "[COLOR]");
    bool color2Applicable = (effectIndex != 2);
    debugCheckPinStateStart(j126_10_effectNames[effectIndex], pinId, effectIndex, color1, color2, color2Applicable);
    switch (effectIndex) {
      case 0: nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1); break;
      case 1: nggPinduno.adrLED1()->bulletFromPoint2Color(color1, color2, 17, 5, 17); break;
      case 2: nggPinduno.adrLED1()->color(color1); nggPinduno.adrLED1()->spreadOutToPoint(0, 1600); break;
      case 3: nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 1000); break;
    }
    debugCheckPinStateEnd();
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = EFFECTFINISHED;
    trigger = 1;
  } else if (metaState == ALL_LED && nggPinduno.pinState()->J126(9)) {
    currentState = EFFECT_ACTIVE;
    stateChanged = true;
    snprintf(pinId, sizeof(pinId), "J126(9)");
    int effectIndex = getRandomEffect(j126_9_effectsUsed, NUM_J126_9_EFFECTS);
    const char* color1 = getRandomColorFromArray(availableColors, NUM_COLORS, "[COLOR]");
    const char* color2 = getRandomColorFromArray(availableColors, NUM_COLORS, "[COLOR]");
    bool color2Applicable = true;
    debugCheckPinStateStart(j126_9_effectNames[effectIndex], pinId, effectIndex, color1, color2, color2Applicable);
    switch (effectIndex) {
      case 0: nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1); break;
      case 1: nggPinduno.adrLED1()->dataStreamNoTail2Color(color1, color2, 20, 20, 1); break;
      case 2: nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 1000); break;
    }
    debugCheckPinStateEnd();
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = EFFECTFINISHED;
    trigger = 1;
  } else if (metaState == ALL_LED && nggPinduno.pinState()->J126(7)) {
    currentState = EFFECT_ACTIVE;
    stateChanged = true;
    snprintf(pinId, sizeof(pinId), "J126(7)");
    int effectIndex = getRandomEffect(j126_7_effectsUsed, NUM_J126_7_EFFECTS);
    const char* color1 = getRandomColorFromArray(availableColors, NUM_COLORS, "[COLOR]");
    const char* color2 = getRandomColorFromArray(availableColors, NUM_COLORS, "[COLOR]");
    bool color2Applicable = (effectIndex == 2);
    debugCheckPinStateStart(j126_7_effectNames[effectIndex], pinId, effectIndex, color1, color2, color2Applicable);
    static int rainbow_count = 0;
    switch (effectIndex) {
      case 0:
        rainbow_count++;
        nggPinduno.adrLED1()->rainbowWS2812FX(1);
        pin_debug_print("[EFFECT] Rainbow Count: ");
        pin_debug_println_var(rainbow_count);
        break;
      case 1: nggPinduno.adrLED1()->rainbowCycleWS2812FX(1); break;
      case 2: nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color2, 1000); break;
    }
    debugCheckPinStateEnd();
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = EFFECTFINISHED;
    trigger = 1;
  } else if (nggPinduno.pinState()->J126(12)) {
    snprintf(pinId, sizeof(pinId), "J126(12)");
    const char* color = getRandomColorFromArray(staticColors, NUM_STATIC_COLORS, "[SIMPLE COLOR]");
    nggPinduno.adrLED1()->color(color);
    pin_debug_print("[SIMPLE EFFECT] ");
    pin_debug_print_var(pinId);
    pin_debug_print(": Color ");
    pin_debug_println_var(color);
    triggerEffect(color, pinId);
    trigger = 1;
  } else if (nggPinduno.pinState()->J126(11)) {
    snprintf(pinId, sizeof(pinId), "J126(11)");
    const char* color = getRandomColorFromArray(staticColors, NUM_STATIC_COLORS, "[SIMPLE COLOR]");
    nggPinduno.adrLED1()->color(color);
    pin_debug_print("[SIMPLE EFFECT] ");
    pin_debug_print_var(pinId);
    pin_debug_print(": Color ");
    pin_debug_println_var(color);
    triggerEffect(color, pinId);
    trigger = 1;
  } else if (nggPinduno.pinState()->J126(6)) {
    snprintf(pinId, sizeof(pinId), "J126(6)");
    const char* color = getRandomColorFromArray(staticColors, NUM_STATIC_COLORS, "[SIMPLE COLOR]");
    nggPinduno.adrLED1()->color(color);
    pin_debug_print("[SIMPLE EFFECT] ");
    pin_debug_print_var(pinId);
    pin_debug_print(": Color ");
    pin_debug_println_var(color);
    triggerEffect(color, pinId);
    trigger = 1;
  } else if (nggPinduno.pinState()->J126(5)) {
    snprintf(pinId, sizeof(pinId), "J126(5)");
    const char* color = getRandomColorFromArray(staticColors, NUM_STATIC_COLORS, "[SIMPLE COLOR]");
    nggPinduno.adrLED1()->color(color);
    pin_debug_print("[SIMPLE EFFECT] ");
    pin_debug_print_var(pinId);
    pin_debug_print(": Color ");
    pin_debug_println_var(color);
    triggerEffect(color, pinId);
    trigger = 1;
  } else if (nggPinduno.pinState()->J126(4)) {
    snprintf(pinId, sizeof(pinId), "J126(4)");
    const char* color = getRandomColorFromArray(staticColors, NUM_STATIC_COLORS, "[SIMPLE COLOR]");
    nggPinduno.adrLED1()->color(color);
    pin_debug_print("[SIMPLE EFFECT] ");
    pin_debug_print_var(pinId);
    pin_debug_print(": Color ");
    pin_debug_println_var(color);
    triggerEffect(color, pinId);
    trigger = 1;
  }

  if (trigger) {
    nggPinduno.pinState()->reset();
    timeLastEvent = currentTime;
    pin_debug_print("[PIN] Pin States Reset, Last Event at ");
    pin_debug_println_var(currentTime);
  }

  if (metaState == ALL_LED && currentTime - timeLastEvent > attractTimeout) {
    currentState = ATTRACT;
    stateChanged = true;
    selectNextAttractColor();
    lastColorChangeTime = currentTime;
    pin_debug_print("[STATE] GAME_RUN -> ATTRACT: Timeout, New Color: ");
    pin_debug_println_var(currentAttractColor);
  }
}

void triggerEffect(const char* color, const char* pin) {
  currentState = EFFECT_ACTIVE;
  stateChanged = true;
  currentEffectColor = color;
  effectStartTime = millis();
  pin_debug_print("[EFFECT] ");
  pin_debug_print_var(pin);
  pin_debug_print(": Triggering ");
  pin_debug_println_var(color);
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
    pin_debug_println("[EFFECT] All Effects Used, Resetting Effect Tracking");
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
  pin_debug_println("[EFFECT] No Effects Available, Defaulting to 0");
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
    debug_println("[COLOR] All Attract Colors Shown, Resetting");
  }
  int availableCount = 0;
  int availableIndices[3];
  for (int i = 0; i < 3; i++) {
    if (!colorsShown[i]) {
      availableIndices[availableCount++] = i;
    }
  }
  if (availableCount > 0) {
    attractColorIndex = availableIndices[random(availableCount)];
    // Map indices to staticColors: 0=green, 1=blue, 2=red
    const char* attractColorMap[] = {staticColors[2], staticColors[3], staticColors[1]};
    currentAttractColor = attractColorMap[attractColorIndex];
    colorsShown[attractColorIndex] = true;
    debug_print("[COLOR] New Attract Color: ");
    debug_print_var(currentAttractColor);
    debug_print(" (index ");
    debug_print_dec(attractColorIndex);
    debug_println(")");
  }
}

bool isAnyPinActive() {
  for (int i = 3; i <= 12; i++) {
    if (i != 8 && nggPinduno.pinState()->J126(i)) {
      pin_debug_print("[PIN] Active J126(");
      pin_debug_print_dec(i);
      pin_debug_println(")");
      return true;
    }
  }
  return false;
}