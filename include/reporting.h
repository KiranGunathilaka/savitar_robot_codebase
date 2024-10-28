// #ifndef REPORTING_H
// #define REPORTING_H

// #include <esp_now.h>
// #include <WiFi.h>
// #include "motors.h"
// #include "sensors.h"
// #include <config.h>

// class Reporting;
// extern Reporting reporter;

// class Mouse;

// class Reporting
// {
// public:
//     int speed, omega;

//     typedef struct sendData
//     {
//         float speedData;
//         float omegaData;
//         bool front;
//         bool left;
//         bool right;
//         float distance;
//         float angle;
//         int currentCellX;
//         int currentCellY;
//         int nextCellX;
//         int nextCellY;
//     } sendData;

//     typedef struct receiveData
//     {
//         float fwdKp;
//         float fwdKd;
//         float rotKp;
//         float rotKd;
//         float steeringKp;
//         float steeringKd;
//         int speed;
//         int omega;
//     } receiveData;

//     typedef struct wallSend
//     {
//         int x;
//         int y;
//         int north;
//         int east;
//         int south;
//         int west;
//     } wallSend;


//     receiveData command;
//     esp_now_peer_info_t peerInfo;
//     sendData transmitData;
//     static Reporting *instance;

//     static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
//     {
//         // No serial print needed
//     }

//     static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
//     {
//         if (instance)
//         {
//             memcpy(&instance->command, incomingData, sizeof(instance->command));

//             Serial.print("FWD KP  ");
//             motors.fwdKpBack = instance->command.fwdKp;
//             Serial.print(motors.fwdKpBack);

//             Serial.print("  FWD KD  ");
//             motors.fwdKdBack = instance->command.fwdKd;
//             Serial.print(motors.fwdKdBack);

//             Serial.print("  ROT KP  ");
//             motors.rotKpBack = instance->command.rotKp;
//             Serial.print(motors.rotKpBack);

//             Serial.print("  ROT KD  ");
//             motors.rotKdBack = instance->command.rotKd;
//             Serial.print(motors.rotKdBack);

//             Serial.print("Steer KP  ");
//             sensors.steering_kp = instance->command.steeringKp;
//             Serial.print(sensors.steering_kp);
            
//             Serial.print("  steer KD  ");
//             sensors.steering_kd = instance->command.steeringKd;
//             Serial.print(sensors.steering_kd);
            

//             // instance->speed = instance->command.speed;
//             // instance->omega = instance->command.omega;
//         }
//     }

//     void begin()
//     {
//         instance = this; // Set the instance pointer

//         Serial.begin(115200);
//         WiFi.mode(WIFI_STA);

//         // Init ESP-NOW
//         if (esp_now_init() != ESP_OK)
//         {
//             Serial.println("Error initializing ESP-NOW");
//             return;
//         }

//         // Register for Send CB to get the status of Transmitted packet
//         esp_now_register_send_cb(OnDataSent);

//         // Register peer
//         memcpy(peerInfo.peer_addr, broadcastAddress, 6);
//         peerInfo.channel = 0;
//         peerInfo.encrypt = false;

//         // Add peer
//         if (esp_now_add_peer(&peerInfo) != ESP_OK)
//         {
//             Serial.println("Failed to add peer");
//             return;
//         }

//         // Receiving code
//         esp_now_register_recv_cb(OnDataRecv);
//     }

//     void send()
//     {
//         transmitData.speedData = encoders.robot_speed();
//         transmitData.omegaData = encoders.robot_omega();
//         transmitData.front = sensors.frontWallExist;
//         transmitData.right = sensors.rightWallExist;
//         transmitData.left = sensors.leftWallExist;
//         transmitData.distance = encoders.robotAngle();
//         transmitData.angle = encoders.robotDistance();
//         transmitData.currentCellX = (int)mouse.getNowLocation().x;
//         transmitData.currentCellX = (int)mouse.getNowLocation().y;
//         transmitData.nextCellX = (int)mouse.getNextLocation().x;
//         transmitData.nextCellX = (int)mouse.getNextLocation().y;

//         // Send message via ESP-NOW
//         esp_now_send(broadcastAddress, (uint8_t *)&transmitData, sizeof(transmitData));
//     }

//     void sendWalls()
//     {
//         myData.x = mouse.getNowLocation().x;
//         myData.y = mouse.getNowLocation().y;
//         myData.north = maze.walls(mouse.getNowLocation()).north;
//         myData.east = maze.walls(mouse.getNowLocation()).east;
//         myData.south = maze.walls(mouse.getNowLocation()).south;
//         myData.west = maze.walls(mouse.getNowLocation()).west;

//         esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
//     }
// };

// #endif