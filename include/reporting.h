#ifndef REPORTING_H
#define REPORTING_H

#include <esp_now.h>
#include <WiFi.h>
#include "motors.h"
#include "sensors.h"
#include "encoders.h"
#include "servos.h"
#include "config.h"

class Reporting;
extern Reporting reporter;

class Reporting
{
public:
    int speed, omega;

    typedef struct sendData
    {
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

    } sendData;

    typedef struct receiveData
    {
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

    } receiveData;

    receiveData command;
    esp_now_peer_info_t peerInfo;
    sendData transmitData;
    static Reporting *instance;

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        // No serial print needed
    }

    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
    {
        if (instance)
        {
            memcpy(&instance->command, incomingData, sizeof(instance->command));

            Serial.print("FWD KP: ");
            motors.fwdKpBack = instance->command.fwdKp1;
            Serial.print(motors.fwdKpBack);

            Serial.print(" FWD KD: ");
            motors.fwdKdBack = instance->command.fwdKd1;
            Serial.print(motors.fwdKdBack);

            Serial.print(" ROT KP: ");
            motors.rotKpBack = instance->command.rotKp1;
            Serial.print(motors.rotKpBack);

            Serial.print(" ROT KD: ");
            motors.rotKdBack = instance->command.rotKd1;
            Serial.print(motors.rotKdBack);

            Serial.print(" FWD KP: ");
            motors.fwdKpFront = instance->command.fwdKp2;
            Serial.print(motors.fwdKpBack);

            Serial.print(" FWD KD: ");
            motors.fwdKdFront = instance->command.fwdKd2;
            Serial.print(motors.fwdKdBack);

            Serial.print(" ROT KP: ");
            motors.rotKpFront = instance->command.rotKp2;
            Serial.print(motors.rotKpBack);

            Serial.print(" ROT KD: ");
            motors.rotKdFront = instance->command.rotKd2;
            Serial.print(motors.rotKdBack);

            Serial.print("Steer KP  ");
            sensors.steering_kp = instance->command.steeringKp;
            Serial.print(sensors.steering_kp);

            Serial.print("  steer KD  ");
            sensors.steering_kd = instance->command.steeringKd;
            Serial.print(sensors.steering_kd);
        }
    }

    void begin()
    {
        instance = this; // Set the instance pointer

        Serial.begin(115200);
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        // Register for Send CB to get the status of Transmitted packet
        esp_now_register_send_cb(OnDataSent);

        // Register peer
        memcpy(peerInfo.peer_addr, broadcastAddress, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        // Add peer
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            Serial.println("Failed to add peer");
            return;
        }

        // Receiving code
        esp_now_register_recv_cb(OnDataRecv);
    }

    void send()
    {
        transmitData.speedFront = encoders.robot_speed_front();
        transmitData.speedBack = encoders.robot_speed_back();

        transmitData.omegaBack = encoders.robot_omega_back();
        transmitData.omegaFront = encoders.robot_omega_front();

        transmitData.cs1 = sensors.sensorColors[0];
        transmitData.cs2 = sensors.sensorColors[1];
        transmitData.cs3 = sensors.sensorColors[2];
        transmitData.cs4 = sensors.sensorColors[3];
        transmitData.cs5 = sensors.sensorColors[4];

        transmitData.tof1 = sensors.left_tof;
        transmitData.tof2 = sensors.right_tof;
        transmitData.tof3 = sensors.front_tof;
        transmitData.tof4 = sensors.center_bottom_tof;
        transmitData.tof5 = sensors.center_top_tof;

        transmitData.angleBack = encoders.robotAngleBack();
        transmitData.angleFront = encoders.robotAngleFront();

        transmitData.distanceBack = encoders.robotDistanceBack();
        transmitData.distanceFront = encoders.robotDistanceFront();

        transmitData.servoGripper = servos.isServoGripOn;
        transmitData.servoLift = servos.isServoLiftOn;

        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&transmitData, sizeof(transmitData));

        if (result == ESP_OK)
        {
            Serial.println("Sent with success");
        }
        else
        {
            Serial.println("Error sending the data");
        }
    }
};

#endif