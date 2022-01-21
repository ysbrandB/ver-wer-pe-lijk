//Project module 6 Create VER-WER-PE-LIJK
//This code is created and adapted by the VER-WER-PE-LIJK team to be able to play games using connected esp8266's and let the modules react to the game states
//the espnow- section of this code is largely inspired by https://github.com/bnbe-club/esp-now-examples-diy-62 which is under the creative commons license 'CC0 1.0 Universal'
//Fade in / Fade out LED animations inspired by https://gist.github.com/marmilicious/f86b39d8991e1efcfd9fbd90dcdf751b made by Scott Kletzien

//ESP Now Connectifity
#include<ESP8266WiFi.h>
#include<espnow.h>

#define WIFI_CHANNEL    1

#define buttonPin 4
#define MY_NAME         "Light ring"
uint8_t receiverAddress[] = {0x7C, 0x87, 0xCE, 0x81, 0xB6, 0x74};   // CONTROLLER

#include <FastLED.h>
const int NUM_LEDS = 51;
const int DATA_PIN = 2;
CRGB leds[NUM_LEDS];

struct __attribute__((packed)) dataPacket {
  boolean pressed;
  int gameState;
};

float startTime;
float interval = 3000;
int gameState=0;
boolean iAmPressed=false;

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
  gameState=packet.gameState;
} 

void setup() {
  Serial.begin(115200);     // initialize serial port
  // initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  // Initialize FastLED on pin
  FastLED.addLeds<WS2811, 2, GRB>(leds, NUM_LEDS);

  //ESP NOW
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print(MY_NAME);
  pinMode(buttonPin, INPUT);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
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

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);   
  esp_now_register_send_cb(transmissionComplete);         // this function will get called once all data is sent
  esp_now_register_recv_cb(dataReceived);               // this function will get called whenever we receive data
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);

  Serial.println("ESP-NOW Initialized.");
}

void loop() {
  if (millis() - startTime >= interval) {
  startTime = millis();
  dataPacket packet;
  
  packet.pressed = iAmPressed;
  packet.gameState = gameState;
  esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));

  iAmPressed=false;
  }
  if(digitalRead(buttonPin)){
    iAmPressed=true;
   }
   Serial.println(iAmPressed);
   updateLedColors();
}

void updateLedColors(){
  switch(gameState){
    case 0:{for(int i; i<NUM_LEDS; i++){
      leds[i]=CRGB::Red;
      }
    }break;
    case 1:{for(int i; i<NUM_LEDS; i++){
      leds[i]=CRGB::Green;
      }
    }break;
      case 2:{for(int i; i<NUM_LEDS; i++){
      leds[i]=CRGB::Blue;
      }
    }break;
  }
  FastLED.show();
}
