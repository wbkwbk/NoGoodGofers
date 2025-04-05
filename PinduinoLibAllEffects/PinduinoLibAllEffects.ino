#include <Adafruit_NeoPixel.h>
#include <AddressableStrip.h> // Direct include from Pinduino

#define LED_PIN 2
#define NUM_LEDS 80
const unsigned long EFFECT_DURATION = 5000;

AddressableStrip *strip = new AddressableStrip(NUM_LEDS, LED_PIN, nullptr);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  strip->strip()->begin();
  strip->strip()->setBrightness(50); // Cap brightness for power safety
  strip->clear();
  strip->strip()->show();
  Serial.println("Pinduino Effects Test Starting (All 61 Effects)...");
}

void loop() {
  // 1. colorRGB(int r, int g, int b)
  Serial.println("Effect: colorRGB - Red");
  strip->colorRGB(255, 0, 0);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 2. colorRGB(int r, int g, int b)
  Serial.println("Effect: colorRGB - Green");
  strip->colorRGB(0, 255, 0);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 3. color(String color)
  Serial.println("Effect: color - Blue");
  strip->color("blue");
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 4. clear()
  Serial.println("Effect: clear - White to Black");
  strip->colorRGB(255, 255, 255);
  strip->strip()->show();
  delay(1000);
  strip->clear();
  strip->strip()->show();
  delay(EFFECT_DURATION - 1000);

  // 5. fadeIn(String color, float time)
  Serial.println("Effect: fadeIn - Yellow");
  strip->fadeIn("yellow", 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 6. fadeInRGB(int r, int g, int b, float time)
  Serial.println("Effect: fadeInRGB - Cyan");
  strip->fadeInRGB(0, 255, 255, 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 7. fadeRGB2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float time)
  Serial.println("Effect: fadeRGB2RGB - Red to Blue");
  strip->fadeRGB2RGB(255, 0, 0, 0, 0, 255, 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 8. fadeColor2Color(String color1, String color2, float time)
  Serial.println("Effect: fadeColor2Color - Green to Purple");
  strip->fadeColor2Color("green", "purple", 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 9. fadeOut(float steps)
  Serial.println("Effect: fadeOut - White to Black");
  strip->colorRGB(255, 255, 255);
  strip->strip()->show();
  delay(1000);
  strip->fadeOut(1000);
  strip->strip()->show();
  delay(EFFECT_DURATION - 1000);

  // 10. RGBBand(int pos, int r, int g, int b, int span)
  Serial.println("Effect: RGBBand - Orange");
  strip->RGBBand(NUM_LEDS / 2, 255, 165, 0, 10);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 11. RGBBandCont(int pos, int r, int g, int b, int span, int startLED, int endLED)
  Serial.println("Effect: RGBBandCont - Purple");
  strip->RGBBandCont(NUM_LEDS / 2, 255, 0, 255, 10, 0, NUM_LEDS);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 12. chase2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir)
  Serial.println("Effect: chase2RGB - Red to Green");
  strip->chase2RGB(255, 0, 0, 0, 255, 0, 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 13. chase2Color(String color1, String color2, float span, int time, int dir)
  Serial.println("Effect: chase2Color - Blue to Yellow");
  strip->chase2Color("blue", "yellow", 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 14. chaseRGB(int r, int g, int b, int span, int time, int dir)
  Serial.println("Effect: chaseRGB - Purple");
  strip->chaseRGB(255, 0, 255, 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 15. chase(String color, float span, int time, int dir)
  Serial.println("Effect: chase - Cyan");
  strip->chase("cyan", 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 16. chase2RGBFromPoint(int pos, float r1, float g1, float b1, float r2, float g2, float b2, int span, int time)
  Serial.println("Effect: chase2RGBFromPoint - Red to Green");
  strip->chase2RGBFromPoint(NUM_LEDS / 2, 255, 0, 0, 0, 255, 0, 5, 20);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 17. chase2ColorFromPoint(int pos, String color1, String color2, int span, int time)
  Serial.println("Effect: chase2ColorFromPoint - Blue to Yellow");
  strip->chase2ColorFromPoint(NUM_LEDS / 2, "blue", "yellow", 5, 20);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 18. chase2RGBCont(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir, int startLED, int endLED)
  Serial.println("Effect: chase2RGBCont - Green to Purple");
  strip->chase2RGBCont(0, 255, 0, 255, 0, 255, 5, 20, 1, 0, NUM_LEDS);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 19. chase2ColorCont(String color1, String color2, float span, int time, int dir, int startLED, int endLED)
  Serial.println("Effect: chase2ColorCont - Cyan to Orange");
  strip->chase2ColorCont("cyan", "orange", 5, 20, 1, 0, NUM_LEDS);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 20. chaseColorCont(String color, float span, int time, int dir, int startLED, int endLED)
  Serial.println("Effect: chaseColorCont - Pink");
  strip->chaseColorCont("pink", 5, 20, 1, 0, NUM_LEDS);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 21. spreadInFromPoint2RGB(int pos, float r1, float g1, float b1, float r2, float g2, float b2, int time)
  Serial.println("Effect: spreadInFromPoint2RGB - Red to Blue");
  strip->spreadInFromPoint2RGB(NUM_LEDS / 2, 255, 0, 0, 0, 0, 255, 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 22. spreadInFromPoint2Color(int pos, String color1, String color2, int time)
  Serial.println("Effect: spreadInFromPoint2Color - Green to Yellow");
  strip->spreadInFromPoint2Color(NUM_LEDS / 2, "green", "yellow", 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 23. spreadInFromPointRGB(int pos, float r, float g, float b, int time)
  Serial.println("Effect: spreadInFromPointRGB - Purple");
  strip->spreadInFromPointRGB(NUM_LEDS / 2, 255, 0, 255, 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 24. spreadInFromPoint(int pos, String color, int time)
  Serial.println("Effect: spreadInFromPoint - Cyan");
  strip->spreadInFromPoint(NUM_LEDS / 2, "cyan", 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 25. spreadOutFromPoint(int pos, float time)
  Serial.println("Effect: spreadOutFromPoint - White fading out");
  strip->colorRGB(255, 255, 255);
  strip->strip()->show();
  strip->spreadOutFromPoint(NUM_LEDS / 2, 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION - 1000);

  // 26. spreadOutToPoint(int pos, float time)
  Serial.println("Effect: spreadOutToPoint - White fading to center");
  strip->colorRGB(255, 255, 255);
  strip->strip()->show();
  strip->spreadOutToPoint(NUM_LEDS / 2, 1000);
  strip->strip()->show();
  delay(EFFECT_DURATION - 1000);

  // 27. explosionRGB(int pos, int r, int g, int b, int span)
  Serial.println("Effect: explosionRGB - Orange");
  strip->explosionRGB(NUM_LEDS / 2, 255, 165, 0, 10);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 28. explosion(int pos, String color, int span)
  Serial.println("Effect: explosion - Purple");
  strip->explosion(NUM_LEDS / 2, "purple", 10);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 29. rainbow(int wait)
  Serial.println("Effect: rainbow");
  strip->rainbow(20);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 30. rainbowCycle(int wait)
  Serial.println("Effect: rainbowCycle");
  strip->rainbowCycle(20);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 31. RGBBullet(int pos, int r, int g, int b, int span, int dir)
  Serial.println("Effect: RGBBullet - Green");
  strip->RGBBullet(NUM_LEDS / 2, 0, 255, 0, 10, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 32. RGBBulletCont(int pos, int r, int g, int b, int span, int dir)
  Serial.println("Effect: RGBBulletCont - Blue");
  strip->RGBBulletCont(NUM_LEDS / 2, 0, 0, 255, 10, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 33. bulletP2P_2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir, int start, int stop)
  Serial.println("Effect: bulletP2P_2RGB - Red to Green");
  strip->bulletP2P_2RGB(255, 0, 0, 0, 255, 0, 5, 20, 1, 0, NUM_LEDS);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 34. bulletP2P_2Color(String color1, String color2, float span, int time, int dir, int start, int stop)
  Serial.println("Effect: bulletP2P_2Color - Blue to Yellow");
  strip->bulletP2P_2Color("blue", "yellow", 5, 20, 1, 0, NUM_LEDS);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 35. bulletFromPoint2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int start_pos)
  Serial.println("Effect: bulletFromPoint2RGB - Green to Purple");
  strip->bulletFromPoint2RGB(0, 255, 0, 255, 0, 255, 5, 20, NUM_LEDS / 2);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 36. bulletFromPointRGB(int r, int g, int b, int span, int time, int start_pos)
  Serial.println("Effect: bulletFromPointRGB - Cyan");
  strip->bulletFromPointRGB(0, 255, 255, 5, 20, NUM_LEDS / 2);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 37. bulletFromPoint(String color, float span, int time, int start_pos)
  Serial.println("Effect: bulletFromPoint - Orange");
  strip->bulletFromPoint("orange", 5, 20, NUM_LEDS / 2);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 38. bulletFromPoint2Color(String color1, String color2, float span, int time, int start_pos)
  Serial.println("Effect: bulletFromPoint2Color - Red to Blue");
  strip->bulletFromPoint2Color("red", "blue", 5, 20, NUM_LEDS / 2);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 39. bullet2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir)
  Serial.println("Effect: bullet2RGB - Yellow to Purple");
  strip->bullet2RGB(255, 255, 0, 255, 0, 255, 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 40. bulletRGB(int r, int g, int b, int span, int time, int dir)
  Serial.println("Effect: bulletRGB - Green");
  strip->bulletRGB(0, 255, 0, 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 41. bullet2Color(String color1, String color2, float span, int time, int dir)
  Serial.println("Effect: bullet2Color - Cyan to Pink");
  strip->bullet2Color("cyan", "pink", 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 42. bullet(String color, float span, int time, int dir)
  Serial.println("Effect: bullet - Yellow");
  strip->bullet("yellow", 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 43. multiBullet2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir, int num)
  Serial.println("Effect: multiBullet2RGB - Red to Blue");
  strip->multiBullet2RGB(255, 0, 0, 0, 0, 255, 5, 20, 1, 3);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 44. multiBulletRGB(int r, int g, int b, int span, int time, int dir, int num)
  Serial.println("Effect: multiBulletRGB - Green");
  strip->multiBulletRGB(0, 255, 0, 5, 20, 1, 3);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 45. multiBullet(String color, float span, int time, int dir, int num)
  Serial.println("Effect: multiBullet - Purple");
  strip->multiBullet("purple", 5, 20, 1, 3);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 46. multiBullet2Color(String color1, String color2, float span, int time, int dir, int num)
  Serial.println("Effect: multiBullet2Color - Blue to Orange");
  strip->multiBullet2Color("blue", "orange", 5, 20, 1, 3);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 47. Matrix2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir)
  Serial.println("Effect: Matrix2RGB - Green to Blue");
  strip->Matrix2RGB(0, 255, 0, 0, 0, 255, 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 48. Matrix2Color(String color1, String color2, float span, int time, int dir)
  Serial.println("Effect: Matrix2Color - Red to Yellow");
  strip->Matrix2Color("red", "yellow", 5, 20, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 49. RGBMatrix(int rows, int r, int g, int b, int span, int dir)
  Serial.println("Effect: RGBMatrix - Cyan");
  strip->RGBMatrix(5, 0, 255, 255, 5, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 50. sparkleRGB(int r, int g, int b, int density, int speed)
  Serial.println("Effect: sparkleRGB - Pink");
  strip->sparkleRGB(255, 192, 203, 20, 50);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 51. sparkle(String color, int density, int speed)
  Serial.println("Effect: sparkle - Orange");
  strip->sparkle("orange", 20, 50);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 52. dataStreamRGB(int r, int g, int b, int density, int speed, int dir)
  Serial.println("Effect: dataStreamRGB - Blue");
  for (int i = 0; i < 100; i++) {
    strip->dataStreamRGB(0, 0, 255, 20, 50, 1);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 53. dataStream(String color, int density, int speed, int dir)
  Serial.println("Effect: dataStream - Green");
  for (int i = 0; i < 100; i++) {
    strip->dataStream("green", 20, 50, 1);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 54. dataStreamNoTailRGB(int r, int g, int b, int density, int speed, int dir)
  Serial.println("Effect: dataStreamNoTailRGB - Red");
  for (int i = 0; i < 100; i++) {
    strip->dataStreamNoTailRGB(255, 0, 0, 20, 50, 1);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 55. dataStreamNoTail(String color, int density, int speed, int dir)
  Serial.println("Effect: dataStreamNoTail - Yellow");
  for (int i = 0; i < 100; i++) {
    strip->dataStreamNoTail("yellow", 20, 50, 1);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 56. dataStreamNoTail2RGB(float r1, float g1, float b1, float r2, float g2, float b2, int density, int speed, int dir)
  Serial.println("Effect: dataStreamNoTail2RGB - Cyan to Purple");
  for (int i = 0; i < 100; i++) {
    strip->dataStreamNoTail2RGB(0, 255, 255, 255, 0, 255, 20, 50, 1);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 57. dataStreamNoTail2Color(String color1, String color2, int density, int speed, int dir)
  Serial.println("Effect: dataStreamNoTail2Color - Red to Blue");
  for (int i = 0; i < 100; i++) {
    strip->dataStreamNoTail2Color("red", "blue", 20, 50, 1);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 58. equalizerRGB(float r1, float g1, float b1, float r2, float g2, float b2, int span, int spd, int tipFallDelaySpan, int dir, int nofade)
  Serial.println("Effect: equalizerRGB - Green to Red");
  strip->equalizerRGB(0, 255, 0, 255, 0, 0, 20, 50, 5, 1, 0);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 59. equalizer(String color1, String color2, int span, int spd, int tipFallDelaySpan, int dir)
  Serial.println("Effect: equalizer - Blue to Yellow");
  strip->equalizer("blue", "yellow", 20, 50, 5, 1);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  // 60. fire(int density, int speed)
  Serial.println("Effect: fire - Red/Orange/Yellow");
  for (int i = 0; i < 100; i++) {
    strip->fire(20, 50);
    strip->strip()->show();
  }
  strip->clear();
  strip->strip()->show();

  // 61. meteorRain(int r, int g, int b, int meteorSize, int meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
  Serial.println("Effect: meteorRain - White");
  strip->meteorRain(255, 255, 255, 5, 64, true, 30);
  strip->strip()->show();
  delay(EFFECT_DURATION);
  strip->clear();
  strip->strip()->show();

  Serial.println("Test Complete. Restarting...");
  delay(2000);
}