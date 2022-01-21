// Ver-wer-pe-lijk
// Interactive Throwing Targets
// ESP2688 Slave file
//
// Used to detect hits, control the led strip and communicate with other targets
// Daisy Baars, Rachel den Otter, Ysbrand Burgstede, Tristan van Marle, Thomas van Klink

#include <FastLED.h>

// Set pins for switches
const int buttonPin = 4;     // the number of the pushbutton pin
const int indicatorPin =  5;       // the number of the LED pin
// Set pins for LED strip
const int NUM_LEDS = 51;
const int DATA_PIN = 2;
CRGB leds[NUM_LEDS];
int effect = 1;

// variable for storing the pushbutton status
int buttonState = 0;

void setup() {
  Serial.begin(115200);
  // initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output
  pinMode(indicatorPin, OUTPUT);
  // Initialize FastLED on pin
  FastLED.addLeds<WS2811, 2, GRB>(leds, NUM_LEDS);
}

void loop() {
  detect();
  light();
}

void detect(){
  // read the state of the pushbutton value
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
  if (buttonState == HIGH) {
    // turn LED on
    digitalWrite(indicatorPin, HIGH);
    Serial.println("On");
  } else {
    // turn LED off
    digitalWrite(indicatorPin, LOW);
    Serial.println("Off");
  }
}

void light(){
  switch(effect){
    // No connection - Blinking red
    case 1:
      FastLED.setBrightness(20);
      for (int i = 0; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Red; 
      }
      FastLED.show();
      delay(1000);
      for (int i = 0; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Black; 
      }
      FastLED.show();
      delay(1000);
    break;
    
    case 2:
    
    }
}
