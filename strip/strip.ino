#include <FastLED.h>
const int NUM_LEDS = 51;
const int DATA_PIN = 5;
CRGB leds[NUM_LEDS];

void setup()
{ 
  Serial.begin(115200);
  FastLED.addLeds<WS2811, 5, GRB>(leds, NUM_LEDS);
}

void loop() {
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::Blue; 
  }
  FastLED.show();
  delay(300);
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::White; 
    delay(300);
    leds[i] = CRGB::Black; 
    FastLED.show();
  }
}
