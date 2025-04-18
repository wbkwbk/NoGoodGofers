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
const int aLEDNum1 = 80;  // 80 LEDs, matching your No Good Gophers setup
const int aLEDNum2 = 0;
const int aLEDNum3 = 0;
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

void setup() {
  #if DEBUG == 1
    Serial.begin(115200);
    // Wait for Serial to stabilize
    while (!Serial) {
      delay(10);
    }
  #endif
  debug_println("Rainbow Static Test Initializing...");

  // Power stabilization (as in your main program)
  delay(500);

  // Initialize LEDs
  nggPinduno.adrLED1()->strip()->begin();
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->setBrightness(255);  // Full brightness for testing
  nggPinduno.adrLED1()->show(true);
  nggPinduno.pinState()->reset();

  debug_println("Initialization Complete");
}

void loop() {
  debug_print("Starting rainbowWS2812FX at: ");
  debug_println_var(millis());

  // Call rainbowStatic (2-second moving rainbow effect)
  // Note: The '0' parameter is ignored, as rainbowStatic is hardcoded to 2 seconds
  //nggPinduno.adrLED1()->rainbowCycleWS2812FX(1);
  nggPinduno.adrLED1()->rainbowWS2812FX(1);
  debug_print("Finished rainbowWS2812FX at: ");
  debug_println_var(millis());

  // Turn off the LED strip
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED1()->show(true);
  debug_print("LED strip cleared at: ");
  debug_println_var(millis());

  // Pause 1 second with LEDs off
  delay(1000);

  debug_print("Pause ended at: ");
  debug_println_var(millis());
}