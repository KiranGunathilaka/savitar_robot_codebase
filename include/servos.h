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
        servoGripper.attach(SERVO_GRIPPER_PIN);
        delay(100);
        servoGripper.write(OPEN_ARM_ANGLE);
        delay(1000);
        servoGripper.detach();
        delay(100);
    }

    void closeArms()
    {
        servoGripper.attach(SERVO_GRIPPER_PIN);
        delay(200);
        servoGripper.write(CLOSE_ARM_ANGLE);
        delay(1000);
        servoGripper.detach();
        delay(100);
    }

    void closeArmsFull(){
        servoGripper.attach(SERVO_GRIPPER_PIN);
        delay(100);
        servoGripper.write(180);
        delay(1000);
        servoGripper.detach();
        delay(100);
    }

    void liftUp()
    {
        servoLift.attach(SERVO_LIFT_PIN);
        delay(100);
        servoLift.write(130);
        delay(1200);
        servoLift.write(90);
        delay(500);
        servoLift.detach();
        delay(100);
    }

    void liftDown(){
        servoLift.attach(SERVO_LIFT_PIN);
        delay(100);
        servoLift.write(60);
        delay(1200);
        servoLift.write(90);
        delay(500);
        servoLift.detach();
        delay(100);
    }

    int detectHeight(){

        if (sensors.center_top_tof < HEIGHT_DETECTING_THRESHOLD){
            return 15;
        } else if (sensors.center_bottom_tof < HEIGHT_DETECTING_THRESHOLD){
            return 10;
        }else if (sensors.front_tof < HEIGHT_DETECTING_THRESHOLD){
            return 5;
        }
        return 0;
    }

    
};

extern Servos servos;