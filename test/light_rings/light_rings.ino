//Project module 6 Create VER-WER-PE-LIJK
//This code is created and adapted by the VER-WER-PE-LIJK team to be able to play games using connected esp8266's and let the modules react to the game states
//the espnow- section of this code is largely inspired by https://github.com/bnbe-club/esp-now-examples-diy-62 which is under the creative commons license 'CC0 1.0 Universal'
//Fade in / Fade out LED animations inspired by https://gist.github.com/marmilicious/f86b39d8991e1efcfd9fbd90dcdf751b made by Scott Kletzien

//ESP Now connectivity
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
CHSV myColor;
struct __attribute__((packed)) dataPacket {
  boolean pressed;
  int gameState;
};

float startTime;
float effectTimer;
float effectLastMillis = 0;
boolean effectSwitch = false;
float interval = 3000;
int gameState = 0;
boolean iAmPressed = false;
int framesPressed = 0;
float gHue = 0;
int currentLed = 0;
int rainbowfunTimer = 0;
void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if (transmissionStatus == 0) {
    Serial.println("Data sent successfully");
  } else {
    Serial.print("Error code: ");
    gameState = 0;
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
  if (gameState != packet.gameState) {
    iAmPressed = false;
    gameState = packet.gameState;
  }
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
  startTime = millis();
  effectTimer = millis();
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  Serial.println("...initializing...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();        // we do not want to connect to a WiFi network

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    gameState = 0;
    return;
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
  }
  if (digitalRead(buttonPin)) {
    framesPressed++;
  } else {
    framesPressed = 0;
  }
  if (framesPressed > 10 && !iAmPressed && gameState!=8) {
    iAmPressed = true;
    if (gameState == 6) {
      rainbowfunTimer = 6000;
    }
  }

  Serial.println(iAmPressed);
  Serial.println(framesPressed);
  updateLedColors();
}

void updateLedColors() {
  if (millis() - effectLastMillis >= effectTimer) {
    effectLastMillis = millis();
    if (effectSwitch)effectSwitch = false;
    else effectSwitch = true;
    switch (gameState) {
      case 0: {//not connected
          for (int i = 0; i <= NUM_LEDS; i++) {
            if (effectSwitch) {
              leds[i] = CRGB::Red;
            } else {
              leds[i] = CRGB::Black;
            }
          }
          effectTimer = 2000;
        } break;
      case 1: {//connected idle
          for (int i = 0; i <= NUM_LEDS; i++) {
            leds[i] = CRGB::Green;
          }
        } break;
      case 2: {//game 1 all for the team
          if (iAmPressed) {
            for (int i = 0; i <= NUM_LEDS; i++) {
              leds[i] = CRGB::Blue;
            }
          } else {
            for (int i = 0; i <= NUM_LEDS; i++) {
              leds[i] = CRGB::Red;
            }
          }
          effectTimer = 100;
        } break;
      case 3: {//colorfun
          if (iAmPressed) {
            CRGB randomColor = CHSV(random(100, 255), 255, 255);
            for (int i = 0; i <= NUM_LEDS; i++) {
              leds[i] = randomColor;
            }
            iAmPressed = false;
          }
          effectTimer = 300;
        } break;
      case 4: {//rainbow effect
          fill_rainbow( leds, NUM_LEDS, gHue, 7);
          gHue++;
          effectTimer = 10;
        } break;
      case 5: {
          if (currentLed == 0) {
            leds[0] = CHSV(random(100, 255), 255, 255);
          }
          else {
            leds[currentLed] = leds[currentLed - 1];
          }
          currentLed++;
          if (currentLed >= NUM_LEDS - 1) {
            currentLed = 0;
          }
          effectTimer = random(10, 30);
        }break;
      case 6: { //rainbowfun
          if (iAmPressed) {
            for (int i = 0; i < NUM_LEDS; i++) {
              if (effectSwitch) {
                leds[i] = CRGB::White;
              } else {
                leds[i] = CRGB::Black;
              }
            }
            effectTimer = 500;
            } else {
              fill_rainbow( leds, NUM_LEDS, gHue, 7);
              gHue++;
              effectTimer = 10;
            }
          }
          break;
        case 7: {//mole 1
             for (int i = 0; i <= NUM_LEDS; i++) {
              leds[i] = CRGB::Red;
            }
            effectTimer = 10;
          }break;
          case 8: {//mole 2
           for (int i = 0; i <= NUM_LEDS; i++) {
              leds[i] = CRGB::Green;
            }
            effectTimer = 10;
          }break;
        }
    }
    FastLED.show();
  }
