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

    void closeArmsFull(){
        servoGripper.write(180);
        delay(1000);
    }

    void liftUp()
    {
        servoLift.write(130);
        delay(1200);
        servoLift.write(90);
        delay(500);
    }

    void liftDown(){
        servoLift.write(60);
        delay(1200);
        servoLift.write(90);
        delay(500);
    }

    int detectHeight(){
        bool is5 = false;
        bool is10 = false;
        bool is15 = false;

        if (sensors.center_top_tof <100){
            return 15;
        } else if (sensors.center_bottom_tof <100){
            return 10;
        }else if (sensors.front_tof < 100){
            return 5;
        }
        return 0;
    }

    
};

extern Servos servos;