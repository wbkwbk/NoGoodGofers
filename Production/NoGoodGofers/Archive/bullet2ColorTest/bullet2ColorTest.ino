#include "pinduinoext.h"

// Debug Configuration
// Debug Configuration
#define DEBUG_LEVEL 1  // 1: Enable debug output
#if DEBUG_LEVEL == 1
  #define debug_print(x) Serial.print(x)
  #define debug_print_dec(x) Serial.print(x, DEC)
  #define debug_println(x) Serial.println(x)
  #define debug_print_var(x) Serial.print(x)
  #define debug_println_var(x) Serial.println(x)
  #define debug_println_dec(x) Serial.println(x, DEC) // Add this line
#else
  #define debug_print(x)
  #define debug_print_dec(x)
  #define debug_println(x)
  #define debug_print_var(x)
  #define debug_println_var(x)
  #define debug_println_dec(x) // Add this line
#endif

// LED Configuration
const int aLEDNum1 = 80;  // 80 LEDs on strip 1
const int aLEDNum2 = 0;
const int aLEDNum3 = 0;
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// Color List
const int NUM_COLORS = 13;
const char* availableColors[] = {"white", "red", "green", "blue", "yellow", "cyan", "purple", "orange", "lime", "sky", "mint", "magenta", "lavender"};
bool colorsUsed[NUM_COLORS] = {false};  // Track used colors to avoid repeats

// Effect Timing
const unsigned long EFFECT_INTERVAL = 5000;  // Run effect every 5 seconds
const unsigned long PAUSE_DURATION = 1000;   // 1-second pause with strip off
unsigned long lastEffectTime = 0;
bool effectRunning = false;  // Track effect state to manage pause

void setup() {
  #if DEBUG_LEVEL == 1
    Serial.begin(115200);
  #endif
  debug_println(F("[INIT] Test Program for bullet2Color Starting..."));

  // Initialize random seed
  randomSeed(analogRead(0));

  // Initialize LEDs
  nggPinduno.adrLED1()->strip()->begin();
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->setBrightness(255);  // Full brightness
  nggPinduno.adrLED1()->show(true);

  debug_println(F("[INIT] LED Strip Initialized, Ready to Test bullet2Color"));
}

void loop() {
  unsigned long currentTime = millis();

  if (!effectRunning && currentTime - lastEffectTime >= EFFECT_INTERVAL) {
    // Select random colors
    const char* color1 = getRandomColor();
    const char* color2 = getRandomColor();

    // Debug output
    debug_print(F("[EFFECT] Running bullet2Color with colors: "));
    debug_print_var(color1);
    debug_print(F(", "));
    debug_print_var(color2);
    debug_print(F(" at time: "));
    debug_println_dec(currentTime);

    // Execute bullet2Color
    nggPinduno.adrLED1()->bullet2Color(color1, color2, 20, 2, 1);

    // Clear the strip (turn off LEDs)
    nggPinduno.adrLED1()->clear();
    nggPinduno.adrLED1()->show(true);

    // Debug completion
    debug_print(F("[EFFECT] bullet2Color Completed, Strip Cleared at time: "));
    debug_println_dec(millis());

    // Mark effect as running to start pause
    effectRunning = true;
    lastEffectTime = currentTime;
  }

  // Handle 1-second pause with strip off
  if (effectRunning && currentTime - lastEffectTime >= PAUSE_DURATION) {
    effectRunning = false;  // End pause, allow next effect after full EFFECT_INTERVAL
    debug_print(F("[EFFECT] 1-Second Pause Completed at time: "));
    debug_println_dec(millis());
  }
}

// Select a random color from availableColors, avoiding repeats until all used
const char* getRandomColor() {
  // Check if all colors are used
  bool allUsed = true;
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!colorsUsed[i]) {
      allUsed = false;
      break;
    }
  }

  // Reset tracking if all colors are used
  if (allUsed) {
    for (int i = 0; i < NUM_COLORS; i++) {
      colorsUsed[i] = false;
    }
    debug_println(F("[COLOR] All Colors Used, Resetting Color Tracking"));
  }

  // Collect available colors
  int availableCount = 0;
  int availableIndices[NUM_COLORS];
  for (int i = 0; i < NUM_COLORS; i++) {
    if (!colorsUsed[i]) {
      availableIndices[availableCount++] = i;
    }
  }

  // Select a random available color
  if (availableCount > 0) {
    int selectedIndex = availableIndices[random(availableCount)];
    colorsUsed[selectedIndex] = true;
    debug_print(F("[COLOR] Selected: "));
    debug_print_var(availableColors[selectedIndex]);
    debug_print(F(" (index "));
    debug_print_dec(selectedIndex);
    debug_println(F(")"));
    return availableColors[selectedIndex];
  }

  // Fallback if no colors available (should not happen)
  debug_println(F("[COLOR] No Colors Available, Defaulting to white"));
  return "white";
}