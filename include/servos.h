#pragma once

#include <Arduino.h>
#include "ESP32Servo.h"
#include <stdint.h>
#include "config.h"
#include "sensors.h"

class Servos;

class Servos {
private:
    Servo servoGripper, servoLift;

public:
    bool isServoGripOn = false;
    bool isServoLiftOn = false;

    void begin(){
        servoGripper.attach(SERVO_GRIPPER_PIN);
        servoGripper.attach(SERVO_LIFT_PIN);
    }


};

extern Servos servos;