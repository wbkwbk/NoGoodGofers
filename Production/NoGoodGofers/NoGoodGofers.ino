// No Good Gophers - Final Stable Implementation with Voltage Monitoring
// Complete solution with reliable LED control and working voltage check

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

void setup() {
  #if DEBUG == 1
    Serial.begin(115200);
    debug_println("System Initializing...");
    debug_println("LED Control v3.1 - With Voltage Monitoring");
  #endif

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


float readVoltage() {
  int rawValue = analogRead(VOLTAGE_PIN);
  float voltage = (rawValue * REFERENCE_VOLTAGE) / 1023.0;
  return voltage * VOLTAGE_DIVIDER_RATIO;
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


void checkVoltage() {
  if (millis() - lastVoltageCheck > voltageCheckInterval) {
    lastVoltageCheck = millis();
    float voltage = readVoltage();
    
    #if DEBUG == 1
      debug_print("System Voltage: ");
      debug_print_var(voltage);
      debug_println("V");
      
      if (voltage < 4.5) {
        debug_println("WARNING: Low voltage detected!");
      }
    #endif
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
        
        #if DEBUG == 1
            debug_println("GAME_RUN initialized with forced color update");
        #endif
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
        
        #if DEBUG == 1
            static unsigned long lastDebug = 0;
            if (millis() - lastDebug >= 1000) {
                lastDebug = millis();
                debug_println("LED refresh maintained");
            }
        #endif
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
    
    #if DEBUG == 1
      debug_print("New attract color: ");
      debug_println_var(currentAttractColor);
    #endif
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
    // Simple Effects
    if (nggPinduno.pinState()->J126(12)) { // Blue
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) { // Red
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red");
      trigger = 1;
    }
    // Complex Effects
    else if (nggPinduno.pinState()->J126(10)) { // Green/Red animation
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      nggPinduno.adrLED1()->bullet2Color("green", "red", 20, 2, 1);
      currentState = GAME_RUN;
      stateChanged = true;
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(9)) { // White/Green animation
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      nggPinduno.adrLED1()->bulletFromPoint2Color("white", "green", 17, 5, 17);
      currentState = GAME_RUN;
      stateChanged = true;
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(7)) { // Green/White animation
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      nggPinduno.adrLED1()->bulletFromPoint2Color("green", "white", 17, 5, 17);
      currentState = GAME_RUN;
      stateChanged = true;
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
  
  #if DEBUG == 1
    debug_print("Triggering effect: ");
    debug_println_var(color);
  #endif
}


/*

//Eric Lyons 2018
//Note to self:  Play more pinball!
//Interfaced for pinduino shield v0.2 and v0.3
//Uses pinduino library

/// ---=====  NOTE ====---
/// 


#include <pinduino.h>
int aLEDNum1 = 80; //left ramp
int aLEDNum2 = 0; //right ramp
int aLEDNum3 = 0;

pinduino pd (aLEDNum1, aLEDNum2, aLEDNum3, "Nano");



int bg_on = 1; //attract effect
unsigned long timeLastEvent = 0; // time last event was last triggered
int startChaseWaitTime = 20000; //Amount of time to wait before chase lights start up again 1000 == 1 second
int bgWhiteTime = 50 ;
String color = "white";

void setup() {
  Serial.begin(115200);
  pd.adrLED1()->clear();
  pd.adrLED2()->clear();
  pd.adrLED3()->clear();
  pd.pinState()->reset();
}

void loop(){
  if (bg_on){background();}
//   Print the pin states out to serial 
//  pd.pinState()->print();
  pd.pinState()->update();
  checkPinStates();
  if (millis()-timeLastEvent > startChaseWaitTime) {bg_on=1;}
  if (millis()-timeLastEvent > bgWhiteTime && !bg_on) {
    pd.adrLED1()->colorRGB(128,128,128);
  }
}

void checkPinStates(){
  int trigger =0;
  
  if ( pd.pinState()->J126(12) ){ // upper right 1 (blue)
//   pd.adrLED1()->fadeOut(50); 
//   pd.adrLED1()->bullet2Color("blue", "white", 20, 2, 1);
    Serial.println("Event: J126(12):->pd.adrLED1()->color(blue)");
    pd.adrLED1()->color("blue");
    delay(100);
   trigger = 1; 
  }

  if ( pd.pinState()->J126(11) ){ //upper right 2 (red)
//   pd.adrLED1()->fadeOut(50); 
//   pd.adrLED1()->bullet2Color("red", "orange", 20, 2, 1);
    Serial.println("Event: J126(11):->pd.adrLED1()->color(red)");
    pd.adrLED1()->color("red");
    delay(100);
    trigger = 1; 
  }

  if ( pd.pinState()->J126(10) ){ //upper right 3 (white)
   pd.adrLED1()->fadeOut(50); 
       Serial.println("Event: J126(10):->pd.adrLED1()->bullet2Color(green, red, 20, 2, 1)");
     pd.adrLED1()->bullet2Color("green", "red", 20, 2, 1);
//    pd.adrLED1()->color("green");
//    delay(100);
    trigger = 1; 
  }
  if ( pd.pinState()->J126(9) ){ // upper playfield right
   pd.adrLED1()->fadeOut(50); 
    Serial.println("Event: J126(9):->pd.adrLED1()->bulletFromPoint2Color(white, green, 17, 5, 17)");
    pd.adrLED1()->bulletFromPoint2Color("white", "green", 17, 5, 17);
    trigger=1;
  }
  
  if ( pd.pinState()->J126(7) ){ // upper playfield left
   pd.adrLED1()->fadeOut(50); 
    Serial.println("Event: J126(7):->pd.adrLED1()->bulletFromPoint2Color(green, white, 17, 5, 17);");
    pd.adrLED1()->bulletFromPoint2Color("green", "white", 17, 5, 17);
    trigger =1;
  }
  
  if ( pd.pinState()->J126(6) ){ // upper left 3 (white)
    Serial.println("Event: J126(6):->pd.adrLED1()->color(green)");  
    pd.adrLED1()->color("green");
    delay(50);
    trigger = 1;
  }
  if ( pd.pinState()->J126(5) ){ // upper left 2 (red)
    Serial.println("Event: J126(5):->pd.adrLED1()->color(red)");  
    pd.adrLED1()->color("red");
    delay(50);
    trigger = 1;
  }
  if ( pd.pinState()->J126(4) ){ // upper left 1 (blue)
    Serial.println("Event: J126(4):->pd.adrLED1()->color(blue)");  
    pd.adrLED1()->color("blue");
    delay(50);
    trigger = 1;
  }
  if ( pd.pinState()->J126(3) ){ // 
  }

//trigger is to take care of any cleanup after a sequence has been triggered.
  if (trigger) {
   pd.pinState()->reset();
   trigger =0;
   bg_on = 0;
   timeLastEvent = millis();
  }

//end function checkPinStates
}



void background() {
  pd.adrLED1()->sparkle(color,20);
  if (random(1000) == 0) {
    if (color == "white") color = "green";
    else if (color == "green") color = "blue";
    else color = "white";
  }
}

*/

























