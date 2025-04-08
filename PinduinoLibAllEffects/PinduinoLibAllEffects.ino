#include <Adafruit_NeoPixel.h>
#include "AddressableStripCorrected.h"

#define LED_PIN 2
#define NUM_LEDS 160
const unsigned long EFFECT_DURATION = 5000;

AddressableStrip *strip = new AddressableStrip(NUM_LEDS, LED_PIN, nullptr);

// Structure to hold effect details
struct Effect {
  const char* name;
  void (*function)();
};

// Function declarations for all effects
void effectBullet() { strip->bullet("yellow", 5, 20, 1); }
void effectBullet2Color() { strip->bullet2Color("cyan", "pink", 5, 20, 1); }
void effectBullet2RGB() { strip->bullet2RGB(255, 255, 0, 255, 0, 255, 5, 20, 1); }
void effectBulletFromPoint() { strip->bulletFromPoint("orange", 5, 20, NUM_LEDS / 2); }
void effectBulletFromPoint2Color() { strip->bulletFromPoint2Color("red", "blue", 5, 20, NUM_LEDS / 2); }
void effectBulletFromPoint2RGB() { strip->bulletFromPoint2RGB(0, 255, 0, 255, 0, 255, 5, 20, NUM_LEDS / 2); }
void effectBulletFromPointRGB() { strip->bulletFromPointRGB(0, 255, 255, 5, 20, NUM_LEDS / 2); }
void effectBulletP2P_2Color() { strip->bulletP2P_2Color("blue", "yellow", 5, 20, 1, 0, NUM_LEDS); }
void effectBulletP2P_2RGB() { strip->bulletP2P_2RGB(255, 0, 0, 0, 255, 0, 5, 20, 1, 0, NUM_LEDS); }
void effectBulletRGB() { strip->bulletRGB(0, 255, 0, 5, 20, 1); }
void effectChase() { strip->chase("cyan", 5, 20, 1); }
void effectChase2Color() { strip->chase2Color("blue", "yellow", 5, 20, 1); }
void effectChase2ColorCont() { strip->chase2ColorCont("cyan", "orange", 5, 20, 1, 0, NUM_LEDS); }
void effectChase2ColorFromPoint() { strip->chase2ColorFromPoint(NUM_LEDS / 2, "blue", "yellow", 5, 20); }
void effectChase2RGB() { strip->chase2RGB(255, 0, 0, 0, 255, 0, 5, 20, 1); }
void effectChase2RGBCont() { strip->chase2RGBCont(0, 255, 0, 255, 0, 255, 5, 20, 1, 0, NUM_LEDS); }
void effectChase2RGBFromPoint() { strip->chase2RGBFromPoint(NUM_LEDS / 2, 255, 0, 0, 0, 255, 0, 5, 20); }
void effectChaseColorCont() { strip->chaseColorCont("pink", 5, 20, 1, 0, NUM_LEDS); }
void effectChaseRGB() { strip->chaseRGB(255, 0, 255, 5, 20, 1); }
void effectClear() { strip->colorRGB(255, 255, 255); strip->strip()->show(); delay(1000); strip->clear(); }
void effectColor() { strip->color("blue"); }
void effectColorRGB() { strip->colorRGB(255, 0, 0); }
void effectDataStream() { strip->dataStream("green", 20, 50, 1); }
void effectDataStreamNoTail() { strip->dataStreamNoTail("yellow", 20, 50, 1); }
void effectDataStreamNoTail2Color() { strip->dataStreamNoTail2Color("red", "blue", 20, 50, 1); }
void effectDataStreamNoTail2RGB() { strip->dataStreamNoTail2RGB(0, 255, 255, 255, 0, 255, 20, 50, 1); }
void effectDataStreamRGB() { strip->dataStreamRGB(0, 0, 255, 20, 50, 1); }
void effectEqualizer() { strip->equalizer("blue", "yellow", 20, 50, 5, 1); }
void effectEqualizerRGB() { strip->equalizerRGB(0, 255, 0, 255, 0, 0, 20, 50, 5, 1, 0); }
void effectExplosion() { strip->explosion(NUM_LEDS / 2, "purple", 10); }
void effectExplosionRGB() { strip->explosionRGB(NUM_LEDS / 2, 255, 165, 0, 10); }
void effectFadeColor2Color() { strip->fadeColor2Color("green", "purple", 1000); }
void effectFadeIn() { strip->fadeIn("yellow", 1000); }
void effectFadeInRGB() { strip->fadeInRGB(0, 255, 255, 1000); }
void effectFadeOut() { strip->colorRGB(255, 255, 255); strip->strip()->show(); delay(1000); strip->fadeOut(1000); }
void effectFadeRGB2RGB() { strip->fadeRGB2RGB(255, 0, 0, 0, 0, 255, 1000); }
//void effectFire() { strip->fire(20, 50); }
void effectMatrix2Color() { strip->Matrix2Color("red", "yellow", 5, 20, 1); }
void effectMatrix2RGB() { strip->Matrix2RGB(0, 255, 0, 0, 0, 255, 5, 20, 1); }
//void effectMeteorRain() { strip->meteorRain(255, 255, 255, 5, 64, true, 30); }
void effectMultiBullet() { strip->multiBullet("purple", 5, 20, 1, 3); }
void effectMultiBullet2Color() { strip->multiBullet2Color("blue", "orange", 5, 20, 1, 3); }
void effectMultiBullet2RGB() { strip->multiBullet2RGB(255, 0, 0, 0, 0, 255, 5, 20, 1, 3); }
void effectMultiBulletRGB() { strip->multiBulletRGB(0, 255, 0, 5, 20, 1, 3); }
void effectRainbow() { strip->rainbow(20); }
void effectRainbowCycle() { strip->rainbowCycle(20); }
void effectRGBBand() { strip->RGBBand(NUM_LEDS / 2, 255, 165, 0, 10); }
void effectRGBBandCont() { strip->RGBBandCont(NUM_LEDS / 2, 255, 0, 255, 10, 0, NUM_LEDS); }
void effectRGBBullet() { strip->RGBBullet(NUM_LEDS / 2, 0, 255, 0, 10, 1); }
void effectRGBBulletCont() { strip->RGBBulletCont(NUM_LEDS / 2, 0, 0, 255, 10, 1); }
void effectRGBMatrix() { strip->RGBMatrix(5, 0, 255, 255, 5, 1); }
void effectSparkle() { strip->sparkle("orange", 20, 50); }
void effectSparkleRGB() { strip->sparkleRGB(255, 192, 203, 20, 50); }
void effectSpreadInFromPoint() { strip->spreadInFromPoint(NUM_LEDS / 2, "cyan", 1000); }
void effectSpreadInFromPoint2Color() { strip->spreadInFromPoint2Color(NUM_LEDS / 2, "green", "yellow", 1000); }
void effectSpreadInFromPoint2RGB() { strip->spreadInFromPoint2RGB(NUM_LEDS / 2, 255, 0, 0, 0, 0, 255, 1000); }
void effectSpreadInFromPointRGB() { strip->spreadInFromPointRGB(NUM_LEDS / 2, 255, 0, 255, 1000); }
void effectSpreadOutFromPoint() { strip->colorRGB(255, 255, 255); strip->strip()->show(); strip->spreadOutFromPoint(NUM_LEDS / 2, 1000); }
void effectSpreadOutToPoint() { strip->colorRGB(255, 255, 255); strip->strip()->show(); strip->spreadOutToPoint(NUM_LEDS / 2, 1000); }
// New effects from pinduino.cpp
void effectChaseAllAdr2RGB() { strip->chase2RGB(255, 0, 0, 0, 0, 255, 5, 20, 1); } // Red to Blue chase
void effectChaseAllAdrRGB() { strip->chaseRGB(0, 255, 0, 5, 20, 1); } // Green chase
void effectFadeAllAdrColor2Color() { strip->fadeColor2Color("red", "yellow", 1000); } // Red to Yellow fade
void effectFadeAllAdrIn() { strip->fadeIn("purple", 1000); } // Fade in Purple
void effectFadeAllAdrOut() { strip->colorRGB(255, 255, 255); strip->strip()->show(); delay(1000); strip->fadeOut(1000); } // Fade out from White
void effectFadeAllAdrRGB2RGB() { strip->fadeRGB2RGB(0, 255, 0, 255, 0, 0, 1000); } // Green to Red fade
void effectSparkleAllAdr() { strip->sparkle("cyan", 20, 50); } // Cyan sparkle
void effectSparkleAllAdrRGB() { strip->sparkleRGB(255, 165, 0, 20, 50); } // Orange sparkle

// Array of all effects
Effect effects[] = {
  {"bullet", effectBullet},
  {"bullet2Color", effectBullet2Color},
  {"bullet2RGB", effectBullet2RGB},
  {"bulletFromPoint", effectBulletFromPoint},
  {"bulletFromPoint2Color", effectBulletFromPoint2Color},
  {"bulletFromPoint2RGB", effectBulletFromPoint2RGB},
  {"bulletFromPointRGB", effectBulletFromPointRGB},
  {"bulletP2P_2Color", effectBulletP2P_2Color},
  {"bulletP2P_2RGB", effectBulletP2P_2RGB},
  {"bulletRGB", effectBulletRGB},
  {"chase", effectChase},
  {"chase2Color", effectChase2Color},
  {"chase2ColorCont", effectChase2ColorCont},
  {"chase2ColorFromPoint", effectChase2ColorFromPoint},
  {"chase2RGB", effectChase2RGB},
  {"chase2RGBCont", effectChase2RGBCont},
  {"chase2RGBFromPoint", effectChase2RGBFromPoint},
  {"chaseColorCont", effectChaseColorCont},
  {"chaseRGB", effectChaseRGB},
  {"clear", effectClear},
  {"color", effectColor},
  {"colorRGB", effectColorRGB},
  {"dataStream", effectDataStream},
  {"dataStreamNoTail", effectDataStreamNoTail},
  {"dataStreamNoTail2Color", effectDataStreamNoTail2Color},
  {"dataStreamNoTail2RGB", effectDataStreamNoTail2RGB},
  {"dataStreamRGB", effectDataStreamRGB},
  {"equalizer", effectEqualizer},
  {"equalizerRGB", effectEqualizerRGB},
  {"explosion", effectExplosion},
  {"explosionRGB", effectExplosionRGB},
  {"fadeColor2Color", effectFadeColor2Color},
  {"fadeIn", effectFadeIn},
  {"fadeInRGB", effectFadeInRGB},
  {"fadeOut", effectFadeOut},
  {"fadeRGB2RGB", effectFadeRGB2RGB},
  //{"fire", effectFire},
  {"Matrix2Color", effectMatrix2Color},
  {"Matrix2RGB", effectMatrix2RGB},
  //{"meteorRain", effectMeteorRain},
  {"multiBullet", effectMultiBullet},
  {"multiBullet2Color", effectMultiBullet2Color},
  {"multiBullet2RGB", effectMultiBullet2RGB},
  {"multiBulletRGB", effectMultiBulletRGB},
  {"rainbow", effectRainbow},
  {"rainbowCycle", effectRainbowCycle},
  {"RGBBand", effectRGBBand},
  {"RGBBandCont", effectRGBBandCont},
  {"RGBBullet", effectRGBBullet},
  {"RGBBulletCont", effectRGBBulletCont},
  {"RGBMatrix", effectRGBMatrix},
  {"sparkle", effectSparkle},
  {"sparkleRGB", effectSparkleRGB},
  {"spreadInFromPoint", effectSpreadInFromPoint},
  {"spreadInFromPoint2Color", effectSpreadInFromPoint2Color},
  {"spreadInFromPoint2RGB", effectSpreadInFromPoint2RGB},
  {"spreadInFromPointRGB", effectSpreadInFromPointRGB},
  {"spreadOutFromPoint", effectSpreadOutFromPoint},
  {"spreadOutToPoint", effectSpreadOutToPoint},
  // New effects from pinduino.cpp
  {"chaseAllAdr2RGB", effectChaseAllAdr2RGB},
  {"chaseAllAdrRGB", effectChaseAllAdrRGB},
  {"fadeAllAdrColor2Color", effectFadeAllAdrColor2Color},
  {"fadeAllAdrIn", effectFadeAllAdrIn},
  {"fadeAllAdrOut", effectFadeAllAdrOut},
  {"fadeAllAdrRGB2RGB", effectFadeAllAdrRGB2RGB},
  {"sparkleAllAdr", effectSparkleAllAdr},
  {"sparkleAllAdrRGB", effectSparkleAllAdrRGB}
};

const int NUM_EFFECTS = sizeof(effects) / sizeof(effects[0]);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  strip->strip()->begin();
  strip->strip()->setBrightness(50); // Cap brightness for power safety
  strip->clear();
  strip->strip()->show();
  Serial.println("Pinduino Effects Test Starting (67 Effects)...");
}

void loop() {
  for (int i = 0; i < NUM_EFFECTS; i++) {
    // Print effect number and name
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(effects[i].name);
    Serial.println(" - Demo");

    // Run the effect
    effects[i].function();
    strip->strip()->show();
    delay(EFFECT_DURATION);

    // Clear the strip for the next effect
    strip->clear();
    strip->strip()->show();

    // Print double dash line
    Serial.println("--");
  }

  Serial.println("Test Complete. Restarting...");
  delay(2000);
}