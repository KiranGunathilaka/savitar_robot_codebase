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
        servoLift.attach(SERVO_LIFT_PIN);
    }

    void liftBox()
    {
        while(sensors.center_top_tof >= 50){
            servoLift.write(120);
            delay(50);
        }
        servoLift.write(90);
    }

    void liftToMeasure15()
    {
        while(sensors.center_top_tof >= 20){
            servoLift.write(120);
            delay(50);
        }
        servoLift.write(90);
    }


    void openArms()
    {
        servoGripper.write(OPEN_ARM_ANGLE);
        delay(1000);
    }

    void closeArms()
    {
        servoGripper.write(CLOSE_ARM_ANGLE);
        delay(1000);
    }


};

extern Servos servos;