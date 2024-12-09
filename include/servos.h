#pragma once

#include <Arduino.h>
#include "ESP32Servo.h"
#include <stdint.h>
#include "config.h"
#include "sensors.h"

class Servos;

class Servos
{
private:
    Servo servoGripper, servoLift;

public:
    bool isServoGripOn = false;
    bool isServoLiftOn = false;

    void begin()
    {
        servoGripper.attach(SERVO_GRIPPER_PIN);
        servoGripper.attach(SERVO_LIFT_PIN);
    }

    void liftGripper()
    {
        servoLift.write(120);
        delay(200);
        servoLift.write(90);
        delay(200);
        servoLift.write(60);
        delay(200);
    }

    void openArms()
    {
        servoGripper.write(180);
        delay(1000);
        servoGripper.write(90);
        delay(1000);
        servoGripper.write(50);
        delay(1000);
    }
};

extern Servos servos;