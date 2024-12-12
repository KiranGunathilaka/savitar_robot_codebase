#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC ADDRESS OF YOUR ESP01S RECEIVER
uint8_t receiverMac[] = { 0xD8, 0x3B, 0xDA, 0XA3, 0x88, 0x20 };

// Structure to hold the data to be sent
typedef struct sendPacket {
  float fwdKp1;
  float fwdKd1;
  float rotKp1;
  float rotKd1;

  float fwdKp2;
  float fwdKd2;
  float rotKp2;
  float rotKd2;

  float steeringKp;
  float steeringKd;
} sendPacket;

typedef struct receivePacket {
  int cs1;
  int cs2;
  int cs3;
  int cs4;
  int cs5;

  float tof1;
  float tof2;
  float tof3;
  float tof4;
  float tof5;

  float speedFront;
  float omegaFront;
  float speedBack;
  float omegaBack;

  float distanceBack;
  float distanceFront;
  float angleBack;
  float angleFront;

  bool servoGripper;
  bool servoLift;

  int barcode;
} receivePacket;

sendPacket sendData;
receivePacket receiveData;

esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);

  // Set the device in station mode
  WiFi.mode(WIFI_STA);

  // Initalize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register the receive callback
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  if (Serial.available() > 0) {
    // Read the data from the serial port
    sendData.fwdKp1 = Serial.parseFloat();
    sendData.fwdKp2 = Serial.parseFloat();

    sendData.fwdKd1 = Serial.parseFloat();
    sendData.fwdKd2 = Serial.parseFloat();

    sendData.rotKp1 = Serial.parseFloat();
    sendData.rotKp2 = Serial.parseFloat();

    sendData.rotKd1 = Serial.parseFloat();
    sendData.rotKd2 = Serial.parseFloat();

    sendData.steeringKp = Serial.parseFloat();
    sendData.steeringKd = Serial.parseFloat();


    // Send the data via ESP-NOW
    esp_now_send(receiverMac, (uint8_t *)&sendData, sizeof(sendData));
  }
}

// Callback function for sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("SEND_OK");
  } else {
    Serial.println("SEND_FAIL");
  }
}

String getColor(int color) {
  switch (color) {
    case 0:
      return "WHITE";
    case 1:
      return "RED";
    case 2:
      return "BLUE";
    case 3:
      return "BLACK";
    case 4:
      return "UNKNOWN";
    default:
      return "INVALID";
  }
}

// Callback function for received data
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  memcpy(&receiveData, data, sizeof(receiveData));

  //these are colour sensor serial prints, integer corresponds to a colour
  //0 white , 1 Red , 2 blue, 3 black , 4 unkonwn
  Serial.printf("1:%7s 2:%7s 3:%7s 4:%7s 5:%7s    ", getColor(receiveData.cs1).c_str(), getColor(receiveData.cs2).c_str(), getColor(receiveData.cs3).c_str(), getColor(receiveData.cs4).c_str(), getColor(receiveData.cs5).c_str());

  //these are tof distance data
  Serial.printf("L: %5.1f  R: %5.1f  C: %5.1f  GR: %5.1f  LH: %5.1f    ", receiveData.tof1, receiveData.tof2, receiveData.tof3, receiveData.tof4, receiveData.tof5);

  //odometry data, each value are in pairs as these are calculated and processed pairwise (front pair, back pair)
  Serial.printf("DistB: %5.1f DistF: %5.1f AngleB: %5.1f AngleF: %5.1f     ", receiveData.distanceBack, receiveData.distanceFront, receiveData.angleBack, receiveData.angleFront);

  //speeds and omegas of the two pairs
  Serial.printf("SpdB: %5.1f SpdF: %5.1f OmgB: %5.1f OmgF: %5.1f    ", receiveData.speedBack, receiveData.speedFront, receiveData.omegaBack, receiveData.omegaFront);


  //indicate wheteher servo is on or off
  Serial.printf("GrpSrvo: %d LftSrvo: %d   ", receiveData.servoGripper, receiveData.servoLift);

  Serial.printf("%d  \n", receiveData.barcode);
}