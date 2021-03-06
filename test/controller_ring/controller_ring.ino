//Project module 6 Create VER-WER-PE-LIJK
//This code is created and adapted by the VER-WER-PE-LIJK team to be able to play games using connected esp8266's and let the modules react to the game states
//the espnow- section of this code is largely inspired by https://github.com/bnbe-club/esp-now-examples-diy-62 which is under the creative commons license 'CC0 1.0 Universal'
#include<ESP8266WiFi.h>
#include<espnow.h>

#define WIFI_CHANNEL    1
#define MAX_MESSAGE_LENGTH 12
#define MY_NAME         "CONTROLLER NODE"
#define membersof(x) (sizeof(x) / sizeof(x[0]))

uint8_t receiverAddresses[][8] =
{ {0xBC, 0xFF, 0x4D, 0x81, 0x7C, 0x76}
  ,  {0xBC, 0xFF, 0x4D, 0x81, 0x75, 0x65},
  {0xBC, 0xFF, 0x4D, 0x81, 0x7B, 0x8C}
};

String receivedMacAdresses[membersof(receiverAddresses)];

struct __attribute__((packed)) dataPacket {
  boolean pressed;
  int gameState;
};

float startTime;
float interval = 3000;
int gameState = 1;
boolean iAmPressed = false;
int pressedThisGame = 0;
int totalConnected = 0;
int randomMole = random(totalConnected);
int whackTimer = millis();
int whackInterval=5000;

void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if (transmissionStatus == 0) {
    Serial.println("Data sent successfully");
    totalConnected += 1;
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

  if (packet.pressed) {
    for (int i = 0; i < membersof(receivedMacAdresses); i++) {
      if (receivedMacAdresses[i].equals(macStr)) {
        break;
      }
      else if (receivedMacAdresses[i] == NULL) {
        receivedMacAdresses[i] = macStr;
        break;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);     // initialize serial port

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print(MY_NAME);
  Serial.println("...initializing...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();        // we do not want to connect to a WiFi network

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(transmissionComplete);         // this function will get called once all data is sent
  esp_now_register_recv_cb(dataReceived);               // this function will get called whenever we receive data
  for (int i = 0; i < membersof(receiverAddresses); i++) {
    esp_now_add_peer(receiverAddresses[i], ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
  }
  startTime = millis();
  Serial.println("Initialized.");
}

void loop() {
  if (millis() - startTime >= interval) {
    startTime = millis();

    dataPacket packet;
    packet.pressed = iAmPressed;
    packet.gameState = gameState;
    Serial.println("Connected:" + String(totalConnected));
    Serial.println("");
    pressedThisGame = 0;
    for (int i = 0; i < membersof(receivedMacAdresses); i++) {
      if (receivedMacAdresses[i] != NULL) {
        Serial.println("THIS ONE IS PRESSED " + String(receivedMacAdresses[i]));

        pressedThisGame += 1;
        receivedMacAdresses[i] = "";
      }
    }
    updateGames();

    Serial.println("totalPressed:" + String(pressedThisGame));
    Serial.println("gameState: " + String(gameState));
    Serial.println("");
    totalConnected = 0;
    Serial.println(membersof(receiverAddresses));
    for (int i = 0; i < membersof(receiverAddresses); i++) {
      Serial.print(i);
      if (gameState == 7 || gameState == 8) {
        if (i == randomMole) {
          packet.gameState = 7;
        }
        else {
          packet.gameState = 8;
        }
      }
      esp_now_send(receiverAddresses[i], (uint8_t *) &packet, sizeof(packet));
    }
  }
  updateSerial();
}

void updateSerial() {
  //Check to see if anything is available in the serial receive buffer
  while (Serial.available() > 0)
  {
    //Create a place to hold the incoming message
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;

    //Read the next available byte in the serial receive buffer
    char inByte = Serial.read();

    //Message coming in (check not terminating character) and guard for over message size
    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
    {
      //Add the incoming byte to our message
      message[message_pos] = inByte;
      message_pos++;
    }
    //Full message received...
    else
    {
      //Add null character to string
      message[message_pos] = '\0';
      gameState = atoi(message);
      if (gameState == 8 || gameState == 7) {
        randomMole = random(totalConnected);
      }
      Serial.println(gameState);
      //Reset for the next message
      message_pos = 0;
    }
  }
}


void updateGames() {
  if (gameState == 2 && pressedThisGame == totalConnected) {
    gameState = 4; //rainbow effect
  }
  if (gameState == 7 || gameState == 8) {
    if (pressedThisGame >= 1) {
      Serial.println("GROTER!");
      gameState = 4;
    }
    else {
      if (millis() - whackTimer >= whackInterval) {
        whackTimer = millis();
        randomMole=random(totalConnected); 
      }
    }
  }
}
