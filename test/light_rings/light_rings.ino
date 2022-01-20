//Project module 6 Create VER-WER-PE-LIJK
//This code is created and adapted by the VER-WER-PE-LIJK team to be able to play games using connected esp8266's and let the modules react to the game states
//the espnow- section of this code is largely inspired by https://github.com/bnbe-club/esp-now-examples-diy-62 which is under the creative commons license 'CC0 1.0 Universal'
//Fade in / Fade out LED animations inspired by https://gist.github.com/marmilicious/f86b39d8991e1efcfd9fbd90dcdf751b made by Scott Kletzien

//ESP Now Connectifity
#include<ESP8266WiFi.h>
#include<espnow.h>

//LED libary to control LED Strip
#include <FastLED.h>

//ESP NOW Functionality
#define MY_ROLE         ESP_NOW_ROLE_COMBO              // set the role of this device: CONTROLLER, SLAVE, COMBO
#define RECEIVER_ROLE   ESP_NOW_ROLE_COMBO              // set the role of the receiver
#define WIFI_CHANNEL    1

#define MY_NAME         "Light ring"
uint8_t receiverAddress[] = {0x7C, 0x87, 0xCE, 0x81, 0xB6, 0x74};   // CONTROLLER

struct __attribute__((packed)) dataPacket {
  boolean pressed;
  int gameState;
};

void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if(transmissionStatus == 0) {
    Serial.println("Data sent successfully");
  } else {
    Serial.print("Error code: ");
    Serial.println(transmissionStatus);
  }
}

void dataReceived(uint8_t *senderMac, uint8_t *data, uint8_t dataLength) {
  char macStr[18];
  dataPacket packet;  

  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);

  Serial.println();
  Serial.print("Received data from: ");
  Serial.println(macStr);
  
  memcpy(&packet, data, sizeof(packet));
  
  Serial.print("pressed: ");
  Serial.println(packet.pressed);
  Serial.print("gameState: ");
  Serial.println(packet.gameState);
}

// Set pins for switches
const int buttonPin = 4;
int buttonState = 0;
boolean pressed;
// Set pins and LED number
const int NUM_LEDS = 51;
const int DATA_PIN = 2;
CRGB leds[NUM_LEDS];
int effect;
int brightness;
int fadeBrightness = 0;
bool setColor; 

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);     // initialize serial port
  // initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  // Initialize FastLED on pin
  FastLED.addLeds<WS2811, 2, GRB>(leds, NUM_LEDS);
  effect = 4;
  brightness = 50;

  //ESP NOW
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print(MY_NAME);
  Serial.println("...initializing...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();        // we do not want to connect to a WiFi network

  if(esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
   // effect = 1 //LED effect that no connection had been made
    return;
  } else {
   // effect = 2 //LED effect that connection has been made
    }

  esp_now_set_self_role(MY_ROLE);   
  esp_now_register_send_cb(transmissionComplete);         // this function will get called once all data is sent
  esp_now_register_recv_cb(dataReceived);               // this function will get called whenever we receive data
  esp_now_add_peer(receiverAddress, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);

  Serial.println("ESP-NOW Initialized.");
}

void loop() {
  detect();
  control();
  
  unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis >= 3000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    send_packet();
  }
}

void send_packet(){
  dataPacket packet;
  packet.pressed=false;
  packet.gameState=0;
  esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));
}

void detect(){
  // read the state of the pushbutton value
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
  if (buttonState == HIGH) {
    pressed = true;
  } else {
    //pressed = false;
  }
}

void control(){
  FastLED.setBrightness(brightness);
  switch(effect){
    // Disconnected state - Red blinking
    case 1:{
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
    }break;
    // Connected state - Green fade in
    case 2:{
     int fadeAmount = 1;
     for(int i = 0; i < NUM_LEDS; i++ ){
       leds[i].setRGB(0,255,0);
       leds[i].fadeLightBy(fadeBrightness);
      }
      FastLED.show();
      fadeBrightness = fadeBrightness + fadeAmount;
      // reverse the direction of the fading at the ends of the fade: 
      if(fadeBrightness == 0 || fadeBrightness == 255){
        fadeAmount = -fadeAmount ; 
      }    
      delay(10);  // Speed of the fade
    }break;
    // Idle state - LED trailing back and forth
    case 3:{
      fadeToBlackBy( leds, NUM_LEDS, 1);
      int pos = beatsin16(5, 0, NUM_LEDS-1);
      leds[pos] += CHSV( 0, 255, 192);
      FastLED.show();
    }break;
    //Game 1 - If target is hit change to random color
    case 4:{
    if (!setColor){
      int randomColor = random8();
        for (int i = 0; i <= NUM_LEDS; i++) {
          leds[i] = CHSV(randomColor,255,255);
          setColor = true; 
        }
      }  
    FastLED.show();
     if(pressed){
      setColor = false;
      }
    }break;
  }
}
