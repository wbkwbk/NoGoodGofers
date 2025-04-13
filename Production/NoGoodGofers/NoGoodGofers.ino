// Eric Lyons 2018
// Updated by Martin Ruff & Grok 3 & Deepseek & ChatGPT 2025
// No Good Gophers - Pinduino Implementation
// Full program with 50ms GAME_RUN color refresh

#include "pinduinoext.h"

// === Debug Configuration ===
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

// === LED Configuration ===
const int aLEDNum1 = 80;
const int aLEDNum2 = 0;
const int aLEDNum3 = 0;
pinduinoext nggPinduno(aLEDNum1, aLEDNum2, aLEDNum3, "Nano");

// === Constants ===
const unsigned long effectDuration = 2000;    // 2s for simple effects
const unsigned long attractColorDuration = 5000;  // 5s per attract color
const unsigned long gameRunRefreshInterval = 50;  // 50ms refresh for GAME_RUN
const unsigned long attractTimeout = 20000;  // 20s timeout to ATTRACT

// === State Machine ===
enum GameState {
  ATTRACT,
  GAME_RUN,
  EFFECT_ACTIVE
};
GameState currentState = ATTRACT;
bool stateChanged = true; // Track state transitions

// === Variables ===
unsigned long timeLastEvent = 0;
unsigned long lastColorChangeTime = 0;
unsigned long effectStartTime = 0;
unsigned long lastGameRunRefresh = 0;
String currentEffectColor = "";
String currentAttractColor = "";
int attractColorIndex = 0;
const String attractColors[] = {"green", "blue", "red"};
bool colorsShown[3] = {false, false, false};

void setup() {
  #if DEBUG == 1
    Serial.begin(115200);
  #endif
  nggPinduno.adrLED1()->clear();
  nggPinduno.adrLED2()->clear();
  nggPinduno.adrLED3()->clear();
  nggPinduno.pinState()->reset();
  
  // Initialize first attract color
  selectNextAttractColor();
}

void loop() {
  nggPinduno.pinState()->update();
  
  // State machine logic
  switch(currentState) {
    case ATTRACT:
      if (stateChanged) {
        stateChanged = false;
        debug_println("Entering ATTRACT mode");
      }
      handleAttractState();
      break;
      
    case GAME_RUN:
      if (stateChanged) {
        stateChanged = false;
        debug_println("Entering GAME_RUN mode");
        // Initial color set
        nggPinduno.adrLED1()->colorRGB(128, 128, 128);
        lastGameRunRefresh = millis();
      }
      handleGameRunState();
      break;
      
    case EFFECT_ACTIVE:
      if (stateChanged) {
        stateChanged = false;
        debug_println("Entering EFFECT_ACTIVE mode");
      }
      handleEffectActiveState();
      break;
  }
  
  checkPinStates();
}

void handleAttractState() {
  // Show current attract color
  nggPinduno.adrLED1()->sparkle(currentAttractColor, 20);
  
  // Check if it's time to change attract color
  if (millis() - lastColorChangeTime >= attractColorDuration) {
    selectNextAttractColor();
    lastColorChangeTime = millis();
  }
}

void handleGameRunState() {
  // Refresh the color every 50ms
  if (millis() - lastGameRunRefresh >= gameRunRefreshInterval) {
    nggPinduno.adrLED1()->colorRGB(128, 128, 128);
    lastGameRunRefresh = millis();
    
    #if DEBUG == 1
      static unsigned long lastDebug = 0;
      if (millis() - lastDebug >= 1000) {
        lastDebug = millis();
        debug_println("Maintaining GAME_RUN color");
      }
    #endif
  }
}

void handleEffectActiveState() {
  // Only handle simple effects (complex effects are blocking)
  if (currentEffectColor != "" && (millis() - effectStartTime >= effectDuration)) {
    currentState = GAME_RUN;
    stateChanged = true;
    currentEffectColor = "";
    debug_println("Simple effect completed, returning to GAME_RUN");
  }
}

void selectNextAttractColor() {
  // Check if all colors have been shown once
  bool allShown = true;
  for (int i = 0; i < 3; i++) {
    if (!colorsShown[i]) {
      allShown = false;
      break;
    }
  }
  
  // If all have been shown, reset the tracking
  if (allShown) {
    for (int i = 0; i < 3; i++) {
      colorsShown[i] = false;
    }
  }
  
  // Select a random color that hasn't been shown yet
  int availableColors = 0;
  int availableIndices[3];
  
  for (int i = 0; i < 3; i++) {
    if (!colorsShown[i]) {
      availableIndices[availableColors] = i;
      availableColors++;
    }
  }
  
  if (availableColors > 0) {
    int selected = random(availableColors);
    attractColorIndex = availableIndices[selected];
    currentAttractColor = attractColors[attractColorIndex];
    colorsShown[attractColorIndex] = true;
    #if DEBUG == 1
      Serial.print("New attract color: ");
      Serial.println(currentAttractColor);
    #endif
  }
}

bool isAnyPinActive() {
  // Check all the pins we're monitoring
  for (int i = 3; i <= 12; i++) {
    if (i != 8) { // Skip pin 8 if not used
      if (nggPinduno.pinState()->J126(i)) {
        return true;
      }
    }
  }
  return false;
}

void checkPinStates() {
  int trigger = 0;
  
  if (currentState == ATTRACT) {
    // Any pin activity transitions to GAME_RUN
    if (isAnyPinActive()) {
      currentState = GAME_RUN;
      stateChanged = true;
      timeLastEvent = millis();
      nggPinduno.pinState()->reset();
      return;
    }
  }
  
  if (currentState == GAME_RUN || currentState == EFFECT_ACTIVE) {
    // Check for pin triggers
    if (nggPinduno.pinState()->J126(12)) { // upper right 1 (blue) - simple effect
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(11)) { // upper right 2 (red) - simple effect
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(10)) { // upper right 3 (white) - complex effect
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      nggPinduno.adrLED1()->bullet2Color("green", "red", 20, 2, 1); // Blocking call
      currentState = GAME_RUN;
      stateChanged = true;
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(9)) { // upper playfield right - complex effect
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      nggPinduno.adrLED1()->bulletFromPoint2Color("white", "green", 17, 5, 17);
      currentState = GAME_RUN;
      stateChanged = true;
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(7)) { // upper playfield left - complex effect
      currentState = EFFECT_ACTIVE;
      stateChanged = true;
      nggPinduno.adrLED1()->bulletFromPoint2Color("green", "white", 17, 5, 17);
      currentState = GAME_RUN;
      stateChanged = true;
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(6)) { // upper left 3 (white) - simple effect
      nggPinduno.adrLED1()->color("green");
      triggerEffect("green");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(5)) { // upper left 2 (red) - simple effect
      nggPinduno.adrLED1()->color("red");
      triggerEffect("red");
      trigger = 1;
    }
    else if (nggPinduno.pinState()->J126(4)) { // upper left 1 (blue) - simple effect
      nggPinduno.adrLED1()->color("blue");
      triggerEffect("blue");
      trigger = 1;
    }
    
    if (trigger) {
      nggPinduno.pinState()->reset();
      timeLastEvent = millis();
    }
    
    // Return to ATTRACT mode after timeout
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

























