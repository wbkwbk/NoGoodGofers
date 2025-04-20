#include "pinduinoext.h"

// Debug Configuration
#define DEBUG_LEVEL 1  // 0: Off, 1: Full, 2: Only checkPinStates
#if DEBUG_LEVEL == 2
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
const String EFFECTFINISHED = "";
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// Button Configuration
const int RED_BUTTON_PIN = 6;   // D6 - Red Button
const int BLUE_BUTTON_PIN = 7;  // D7 - Blue Button
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long LONG_PRESS_DURATION = 1000;  // 1s
const unsigned long DOUBLE_CLICK_WINDOW = 1000;  // 1s
const unsigned long BRIGHTNESS_UPDATE_INTERVAL = 15;  // 50ms

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
const String staticColors[] = {"white", "red", "green", "blue", "yellow", "cyan", "purple", "orange", "lime", "sky", "mint", "magenta", "lavender"};
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
  nggPinduno.adrLED1()->strip()->Color(255, 165, 0),   //004 orange
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

// Effect Names for Debug Output
const String j126_10_effectNames[] = {"bullet2Color", "bulletFromPoint2Color", "spreadOutToPoint", "spreadInFromPoint2Color"};
const String j126_9_effectNames[] = {"bullet2Color", "dataStreamNoTail2Color", "spreadInFromPoint2Color"};
const String j126_7_effectNames[] = {"rainbowWS2812FX", "rainbowCycleWS2812FX", "spreadInFromPoint2Color"};

// Method to set all LEDs to the specified color from staticColorRGB
void setStripColor(int colorIndex) {
  for (int i = 0; i < aLEDNum1; i++) {
    nggPinduno.adrLED1()->strip()->setPixelColor(i, staticColorRGB[colorIndex]);
  }
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
}

void setup() {
  #if DEBUG_LEVEL == 1 || DEBUG_LEVEL == 2
    Serial.begin(115200);
  #endif
  debug_println(F("[INIT] System Initializing..."));
  debug_println(F("[INIT] LED Control v3.1 - Enhanced Debug Output"));

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
  debug_print(F("[INIT] Initialization Complete, Starting in NO_LED, Attract Color: "));
  debug_println_var(currentAttractColor);
}

void loop() {
  // Update button states first to ensure responsiveness
  updateButtonStates();

  // Handle meta-state
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
          debug_print(F("[BUTTON] Red Press Detected at "));
          debug_println_var(millis());
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
          debug_print(F("[BUTTON] Blue Press Detected at "));
          debug_println_var(millis());
        }
      } else {
        blueButton.isPressed = false;
        if (blueButton.isLongPressed && metaState == SET_BRIGHTNESS) {
          // Exit SET_BRIGHTNESS
          debug_print(F("[STATE] Exiting SET_BRIGHTNESS to "));
          debug_print_var(previousMetaState);
          debug_print(F(", Brightness: "));
          debug_print_dec(brightness);
          metaState = previousMetaState;
          // Reset blinkGreen state to prevent lingering effects
          blinkGreenState.active = false;
          if (metaState == NO_LED) {
            nggPinduno.adrLED1()->clear();
            nggPinduno.adrLED1()->show(true);
            debug_println(F(" - LEDs Cleared"));
          } else {
            nggPinduno.adrLED1()->setBrightness(brightness);
            debug_println(F(" - Brightness Preserved"));
          }
          stateChanged = true;
        }
      }
    }
  }
  blueButton.lastState = blueReading;

  // Detect Red Button Double-Click
  if (redButton.clickCount > 0 && millis() - redButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (redButton.clickCount >= 2) {
      if (metaState == SET_STATICCOLOR) {
        // In SET_STATICCOLOR: Reset to white
        staticColorIndex = 0;
        setStripColor(staticColorIndex);
        debug_print(F("[BUTTON] Red Double-Click in SET_STATICCOLOR: Reset to "));
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(F(" (index "));
        debug_print_dec(staticColorIndex);
        debug_println(F(")"));
      } else {
        // Outside SET_STATICCOLOR: Set ALL_LED, brightness 255, use selected color
        debug_print(F("[STATE] Red Double-Click: Transition to ALL_LED, Brightness: 255, Color: "));
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(F(" (index "));
        debug_print_dec(staticColorIndex);
        debug_println(F(")"));
        metaState = ALL_LED;
        brightness = 255;
        nggPinduno.adrLED1()->setBrightness(brightness);
        setStripColor(staticColorIndex);
        stateChanged = true;
      }
    } else if (redButton.clickCount == 1 && !redButton.isPressed) {
      // Single press
      if (metaState == NO_LED) {
        debug_println(F("[STATE] Red Single Press: NO_LED -> ALL_LED"));
        metaState = ALL_LED;
      } else if (metaState == ALL_LED) {
        debug_println(F("[STATE] Red Single Press: ALL_LED -> NO_LED, LEDs Cleared"));
        metaState = NO_LED;
        nggPinduno.adrLED1()->clear();
        nggPinduno.adrLED1()->show(true);
        currentEffectColor = EFFECTFINISHED;
      } else if (metaState == SET_STATICCOLOR) {
        staticColorIndex = (staticColorIndex + 1) % NUM_STATIC_COLORS;
        setStripColor(staticColorIndex);
        debug_print(F("[BUTTON] Red Single Press: Cycling to "));
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(F(" (index "));
        debug_print_dec(staticColorIndex);
        debug_println(F(")"));
      }
    }
    redButton.clickCount = 0;
  }

  // Detect Blue Button Actions
  if (blueButton.clickCount > 0 && millis() - blueButton.lastClickTime > DOUBLE_CLICK_WINDOW) {
    if (blueButton.clickCount >= 1 && !blueButton.isPressed && !blueButton.isLongPressed) {
      // Single click (also handles double-click for SET_STATICCOLOR exit)
      if (metaState == NO_LED) {
        previousMetaState = metaState;
        metaState = SET_STATICCOLOR;
        staticColorIndex = 0;
        setStripColor(staticColorIndex);
        stateChanged = true;
        debug_println(F("[STATE] Blue Press: NO_LED -> SET_STATICCOLOR, Starting with white"));
      } else if (metaState == ALL_LED) {
        metaState = ALL_LED_RUN_ONLY;
        currentState = GAME_RUN;
        stateChanged = true;
        debug_println(F("[STATE] Blue Press: ALL_LED -> ALL_LED_RUN_ONLY, Forcing GAME_RUN"));
      } else if (metaState == ALL_LED_RUN_ONLY) {
        metaState = ALL_LED;
        currentState = ATTRACT;
        stateChanged = true;
        debug_println(F("[STATE] Blue Press: ALL_LED_RUN_ONLY -> ALL_LED, Entering ATTRACT"));
      } else if (metaState == SET_STATICCOLOR) {
        debug_print(F("[STATE] Blue Press: Exiting SET_STATICCOLOR to "));
        debug_print_var(previousMetaState);
        debug_print(F(", Color: "));
        debug_print_var(staticColors[staticColorIndex]);
        debug_print(F(" (index "));
        debug_print_dec(staticColorIndex);
        debug_println(F(")"));
        metaState = previousMetaState;
        stateChanged = true;
        if (metaState == NO_LED) {
          nggPinduno.adrLED1()->clear();
          nggPinduno.adrLED1()->show(true);
          debug_println(F(" - LEDs Cleared"));
        } else {
          nggPinduno.adrLED1()->setBrightness(brightness);
          debug_println(F(" - Color Preserved"));
        }
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
      debug_print(F("[STATE] Blue Long Press: Entering SET_BRIGHTNESS from "));
      debug_println_var(previousMetaState);
    }
    blueButton.clickCount = 0;
  }
}

void handleNoLedState() {
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->show(true);
  if (stateChanged) {
    debug_print(F("[STATE] NO_LED: LEDs Off, Brightness: "));
    debug_println_dec(brightness);
    stateChanged = false;
  }
}

void handleAllLedState() {
  if (stateChanged) {
    currentEffectColor = EFFECTFINISHED;
    debug_print(F("[STATE] ALL_LED: Reset Effects, Current State: "));
    debug_println_var(currentState);
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
    debug_println(F("[STATE] ALL_LED_RUN_ONLY: Forcing GAME_RUN, Effects Reset"));
  }
  handleGameRunState();
}

void blinkGreen(int numBlinks, bool holdAfter) {
  // Initialize non-blocking blink sequence
  blinkGreenState.active = true;
  blinkGreenState.numBlinks = numBlinks;
  blinkGreenState.currentBlink = 0;
  blinkGreenState.holdAfter = holdAfter;
  blinkGreenState.isOn = true;
  blinkGreenState.lastUpdate = millis();
  // Set initial LED state
  nggPinduno.adrLED1()->color("green");
  nggPinduno.adrLED1()->setBrightness(255);
  nggPinduno.adrLED1()->show(true);
  debug_print(F("[EFFECT] Blinked Green Started, Blinks: "));
  debug_print_dec(numBlinks);
  debug_print(F(", Hold: "));
  debug_println_var(holdAfter);
}

void handleSetBrightnessState() {
  // Handle non-blocking blink sequence
  if (blinkGreenState.active) {
    unsigned long currentTime = millis();
    if (blinkGreenState.isOn) {
      if (currentTime - blinkGreenState.lastUpdate >= blinkGreenState.onDuration) {
        // Turn off LEDs
        nggPinduno.adrLED1()->clear();
        nggPinduno.adrLED1()->show(true);
        blinkGreenState.isOn = false;
        blinkGreenState.lastUpdate = currentTime;
      }
    } else {
      if (currentTime - blinkGreenState.lastUpdate >= blinkGreenState.offDuration) {
        blinkGreenState.currentBlink++;
        if (blinkGreenState.currentBlink < blinkGreenState.numBlinks) {
          // Next blink
          nggPinduno.adrLED1()->color("green");
          nggPinduno.adrLED1()->setBrightness(255);
          nggPinduno.adrLED1()->show(true);
          blinkGreenState.isOn = true;
          blinkGreenState.lastUpdate = currentTime;
        } else if (blinkGreenState.holdAfter) {
          // Start hold
          nggPinduno.adrLED1()->color("green");
          nggPinduno.adrLED1()->setBrightness(255);
          nggPinduno.adrLED1()->show(true);
          blinkGreenState.isOn = true; // Keep on for hold
          blinkGreenState.lastUpdate = currentTime;
          blinkGreenState.numBlinks = 0; // Prevent further blinks
          blinkGreenState.onDuration = blinkGreenState.holdDuration; // Set hold duration
        } else {
          // End blink sequence
          blinkGreenState.active = false;
          // Restore red color
          nggPinduno.adrLED1()->color("red");
          nggPinduno.adrLED1()->setBrightness(brightness);
          nggPinduno.adrLED1()->show(true);
          debug_print(F("[EFFECT] Blinked Green Completed, Blinks: "));
          debug_println_dec(blinkGreenState.numBlinks);
        }
      }
    }
    // Handle hold phase
    if (blinkGreenState.active && blinkGreenState.holdAfter && blinkGreenState.currentBlink >= blinkGreenState.numBlinks && !blinkGreenState.numBlinks) {
      if (currentTime - blinkGreenState.lastUpdate >= blinkGreenState.holdDuration) {
        // End hold
        blinkGreenState.active = false;
        // Restore red color
        nggPinduno.adrLED1()->color("red");
        nggPinduno.adrLED1()->setBrightness(brightness);
        nggPinduno.adrLED1()->show(true);
        debug_print(F("[EFFECT] Blinked Green Hold Completed, Duration: "));
        debug_println_dec(blinkGreenState.holdDuration);
      }
    }
  } else {
    // Normal brightness cycling
    nggPinduno.adrLED1()->color("red");
    nggPinduno.adrLED1()->setBrightness(brightness);
    nggPinduno.adrLED1()->show(true);
  }

  if (stateChanged) {
    brightness = 1;
    lastBrightnessUpdate = millis();
    stateChanged = false;
    // Ensure blink sequence is off at start
    blinkGreenState.active = false;
    debug_println(F("[STATE] SET_BRIGHTNESS: Started with Red, Brightness Cycling"));
  }

  nggPinduno.pinState()->update();
  if (!blinkGreenState.active && millis() - lastBrightnessUpdate >= BRIGHTNESS_UPDATE_INTERVAL) {
    brightness++;
    if (brightness > 255) brightness = 1;
    if (brightness == 200) {
      debug_print(F("[EFFECT] SET_BRIGHTNESS: Brightness 200 - Blink 1x Green, Current Brightness: "));
      debug_println_dec(brightness);
      blinkGreen(1, false);
    } else if (brightness == 230) {
      debug_print(F("[EFFECT] SET_BRIGHTNESS: Brightness 230 - Blink 2x Green, Current Brightness: "));
      debug_println_dec(brightness);
      blinkGreen(2, false);
    } else if (brightness == 255) {
      debug_print(F("[EFFECT] SET_BRIGHTNESS: Brightness 255 - Blink 3x Green + 3s Hold, Current Brightness: "));
      debug_println_dec(brightness);
      blinkGreen(3, true);
    }
    if (!blinkGreenState.active) {
      nggPinduno.adrLED1()->setBrightness(brightness);
      nggPinduno.adrLED1()->show(true);
      lastBrightnessUpdate = millis();
      debug_print(F("[BRIGHTNESS] Current: "));
      debug_println_dec(brightness);
    }
  }
}

void handleSetStaticColorState() {
  setStripColor(staticColorIndex);
  if (stateChanged) {
    debug_print(F("[STATE] SET_STATICCOLOR: Showing "));
    debug_print_var(staticColors[staticColorIndex]);
    debug_print(F(" (index "));
    debug_print_dec(staticColorIndex);
    debug_println(F(")"));
    stateChanged = false;
  }
}

void handleAttractState() {
  if (stateChanged) {
    stateChanged = false;
    debug_print(F("[STATE] ATTRACT: Sparkle with "));
    debug_println_var(currentAttractColor);
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
    debug_print(F("[STATE] GAME_RUN: Using static color "));
    debug_print_var(staticColors[staticColorIndex]);
    debug_print(F(" (index "));
    debug_print_dec(staticColorIndex);
    debug_println(F(")"));
  }
}

void handleEffectActiveState() {
  if (stateChanged) {
    stateChanged = false;
    debug_print(F("[STATE] EFFECT_ACTIVE: Running Effect, Color: "));
    debug_println_var(currentEffectColor);
  }
  nggPinduno.adrLED1()->setBrightness(brightness);
  nggPinduno.adrLED1()->show(true);
  if (isNOEffectrunning() || (millis() - effectStartTime >= effectDuration)) {
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = EFFECTFINISHED;
    debug_println(F("[STATE] EFFECT_ACTIVE: Effect Completed, Returning to GAME_RUN"));
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
    pin_debug_println(F("[STATE] ATTRACT -> GAME_RUN: Pin Activity Detected"));
  }

  if (currentState == GAME_RUN || currentState == EFFECT_ACTIVE) {
    if (metaState == ALL_LED && nggPinduno.pinState()->J126(10)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      int effectIndex = getRandomEffect(j126_10_effectsUsed, NUM_J126_10_EFFECTS);
      String color1 = getRandomColor();
      String color2 = getRandomColor();
      pin_debug_print(F("[EFFECT] J126(10) Started "));
      if (effectIndex >= 0 && effectIndex < NUM_J126_10_EFFECTS) {
        pin_debug_print(j126_10_effectNames[effectIndex]);
      } else {
        pin_debug_print(F("InvalidEffect"));
      }
      pin_debug_print(F(" (index "));
      pin_debug_print_dec(effectIndex);
      pin_debug_print(F(") with colors "));
      pin_debug_print(color1.length() > 0 ? color1 : F("None"));
      pin_debug_print(F(", "));
      pin_debug_print(color2.length() > 0 ? color2 : F("None"));
      pin_debug_println(F(""));
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
      pin_debug_print(F("[EFFECT] J126(10) Completed "));
      if (effectIndex >= 0 && effectIndex < NUM_J126_10_EFFECTS) {
        pin_debug_print(j126_10_effectNames[effectIndex]);
      } else {
        pin_debug_print(F("InvalidEffect"));
      }
      pin_debug_print(F(" (index "));
      pin_debug_print_dec(effectIndex);
      pin_debug_println(F(""));
    }
    else if (metaState == ALL_LED && nggPinduno.pinState()->J126(9)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      int effectIndex = getRandomEffect(j126_9_effectsUsed, NUM_J126_9_EFFECTS);
      String color1 = getRandomColor();
      String color2 = getRandomColor();
      pin_debug_print(F("[EFFECT] J126(9) Started "));
      if (effectIndex >= 0 && effectIndex < NUM_J126_9_EFFECTS) {
        pin_debug_print(j126_9_effectNames[effectIndex]);
      } else {
        pin_debug_print(F("InvalidEffect"));
      }
      pin_debug_print(F(" (index "));
      pin_debug_print_dec(effectIndex);
      pin_debug_print(F(") with colors "));
      pin_debug_print(color1.length() > 0 ? color1 : F("None"));
      pin_debug_print(F(", "));
      pin_debug_print(color2.length() > 0 ? color2 : F("None"));
      pin_debug_println(F(""));
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
      pin_debug_print(F("[EFFECT] J126(9) Completed "));
      if (effectIndex >= 0 && effectIndex < NUM_J126_9_EFFECTS) {
        pin_debug_print(j126_9_effectNames[effectIndex]);
      } else {
        pin_debug_print(F("InvalidEffect"));
      }
      pin_debug_print(F(" (index "));
      pin_debug_print_dec(effectIndex);
      pin_debug_println(F(""));
    }
    else if (metaState == ALL_LED && nggPinduno.pinState()->J126(7)) {
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      int effectIndex = getRandomEffect(j126_7_effectsUsed, NUM_J126_7_EFFECTS);
      String color1 = getRandomColor();
      static int rainbow_count = 0;
      pin_debug_print(F("[EFFECT] J126(7) Started "));
      if (effectIndex >= 0 && effectIndex < NUM_J126_7_EFFECTS) {
        pin_debug_print(j126_7_effectNames[effectIndex]);
      } else {
        pin_debug_print(F("InvalidEffect"));
      }
      pin_debug_print(F(" (index "));
      pin_debug_print_dec(effectIndex);
      pin_debug_print(F(") with color "));
      pin_debug_print(color1.length() > 0 ? color1 : F("None"));
      pin_debug_println(F(""));
      switch (effectIndex) {
        case 0:
          rainbow_count++;
          pin_debug_print(F("[EFFECT] Rainbow Start at "));
          pin_debug_println_var(millis());
          nggPinduno.adrLED1()->rainbowWS2812FX(1);
          pin_debug_print(F("[EFFECT] Rainbow End at "));
          pin_debug_println_var(millis());
          pin_debug_print(F("[EFFECT] Rainbow Count: "));
          pin_debug_println_var(rainbow_count);
          break;
        case 1:
          nggPinduno.adrLED1()->rainbowCycleWS2812FX(8);
          break;
        case 2:
          nggPinduno.adrLED1()->spreadInFromPoint2Color(1, color1, color1, 1000);
          break;
      }
      currentState = GAME_RUN;
      stateChanged = true;
      currentEffectColor = EFFECTFINISHED;
      trigger = 1;
      pin_debug_print(F("[EFFECT] J126(7) Completed "));
      if (effectIndex >= 0 && effectIndex < NUM_J126_7_EFFECTS) {
        pin_debug_print(j126_7_effectNames[effectIndex]);
      } else {
        pin_debug_print(F("InvalidEffect"));
      }
      pin_debug_print(F(" (index "));
      pin_debug_print_dec(effectIndex);
      pin_debug_println(F(""));
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
      pin_debug_print(F("[PIN] Pin States Reset, Last Event at "));
      pin_debug_println_var(millis());
    }

    if (metaState == ALL_LED && millis() - timeLastEvent > attractTimeout) {
      currentState = ATTRACT;
      stateChanged = true;
      selectNextAttractColor();
      lastColorChangeTime = millis();
      pin_debug_print(F("[STATE] GAME_RUN -> ATTRACT: Timeout, New Color: "));
      pin_debug_println_var(currentAttractColor);
    }
  }
}

void triggerEffect(String color, String pin) {
  currentState = EFFECT_ACTIVE;
  stateChanged = true;
  currentEffectColor = color;
  effectStartTime = millis();
  pin_debug_print(F("[EFFECT] "));
  pin_debug_print_var(pin);
  pin_debug_print(F(": Triggering "));
  pin_debug_println_var(color);
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
    pin_debug_println(F("[COLOR] All Colors Used, Resetting Color Tracking"));
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
    pin_debug_print(F("[COLOR] Selected: "));
    pin_debug_print_var(availableColors[selectedIndex]);
    pin_debug_print(F(" (index "));
    pin_debug_print_dec(selectedIndex);
    pin_debug_println(F(")"));
    return availableColors[selectedIndex];
  }
  pin_debug_println(F("[COLOR] No Colors Available, Defaulting to white"));
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
    pin_debug_println(F("[EFFECT] All Effects Used, Resetting Effect Tracking"));
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
  pin_debug_println(F("[EFFECT] No Effects Available, Defaulting to 0"));
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
    debug_println(F("[COLOR] All Attract Colors Shown, Resetting"));
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
    debug_print(F("[COLOR] New Attract Color: "));
    debug_print_var(currentAttractColor);
    debug_print(F(" (index "));
    debug_print_dec(attractColorIndex);
    debug_println(F(")"));
  }
}

bool isAnyPinActive() {
  for (int i = 3; i <= 12; i++) {
    if (i != 8 && nggPinduno.pinState()->J126(i)) {
      pin_debug_print(F("[PIN] Active J126("));
      pin_debug_print_dec(i);
      pin_debug_println(F(")"));
      return true;
    }
  }
  return false;
}