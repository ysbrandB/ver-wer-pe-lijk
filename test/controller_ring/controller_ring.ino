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
{ {0xBC, 0xFF, 0x4D, 0x81, 0x8A, 0xCA}
  ,  {0xBC, 0xDD, 0xC2, 0x9C, 0x3F, 0xF2}
  ,  {0xBC, 0xDD, 0xC2, 0x9C, 0x3F, 0xF2},
  {0xBC, 0xFF, 0x4D, 0x81, 0x7B, 0x8C}
};

struct __attribute__((packed)) dataPacket {
  boolean pressed;
  int gameState;
};

float startTime;
float interval = 3000;
int gameState=0;
boolean iAmPressed=false;

<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if (transmissionStatus == 0) {
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
<<<<<<< Updated upstream

    for (int i = 0; i < membersof(receiverAddresses); i++) {
      esp_now_send(receiverAddresses[i], (uint8_t *) &packet, sizeof(packet));
    }
  }
=======

    for (int i = 0; i < membersof(receiverAddresses); i++) {
      esp_now_send(receiverAddresses[i], (uint8_t *) &packet, sizeof(packet));
    }
  }
  updateSerial();
}

void updateSerial(){
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
     gameState= atoi(message);
     Serial.println(gameState);
     //Reset for the next message
     message_pos = 0;
   }
 }
>>>>>>> Stashed changes
}
