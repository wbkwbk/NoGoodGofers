#include <Adafruit_NeoPixel.h>
#include <AddressableStrip.h> // Include directly from Pinduino

#define LED_PIN 2
#define NUM_LEDS 80

AddressableStrip *strip = new AddressableStrip(NUM_LEDS, LED_PIN, NULL); // NULL for no custom pins

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  strip->strip()->begin();
  strip->strip()->setBrightness(50); // Ensure visibility
  strip->clear();
  strip->strip()->show();
  Serial.println("Direct AddressableStrip Test Starting...");
}

void loop() {
  strip->colorRGB(255, 0, 0); // Red
  strip->strip()->show();     // Explicitly update
  Serial.println("Red");
  delay(1000);
  strip->clear();
  strip->strip()->show();
  Serial.println("Off");
  delay(1000);
}