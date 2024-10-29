#pragma once

#include "Arduino.h"
#include "config.h"
#include "motion.h"
#include "reporting.h"
#include "sensors.h"
#include "systick.h"
#include "nvs.h"

class Robot;
extern Robot robot;

class Robot
{
    void follow_Color_line()
    {
        systick.enableSlowMode(true);
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
    }

    void is_all_colour_equal()
    {
        bool isAllEqual = true;
        for (int i = 0; i < 5; i++)
        {
            if (sensors.sensorColors[0] != sensors.sensorColors[i])
            {
                isAllEqual = false;
                break;
            }
        }
    }

    void is_left_equal(){
        bool isLeftEqual = true;
        for (int i = 0; i < 3; i++)
        {
            if (sensors.sensorColors[0] != sensors.sensorColors[i])
            {
                isLeftEqual = false;
                break;
            }
        }
    }

    void is_right_equal(){
        bool isRightEqual = true;
        for (int i = 2; i < 5; i++)
        {
            if (sensors.sensorColors[2] != sensors.sensorColors[i])
            {
                isRightEqual = false;
                break;
            }
        }
    }

    void turn_180()
    {
        sensors.set_steering_mode(STEERING_OFF);
        static int direction = 1;
        direction *= -1; // alternate direction each time it is called

        motion.reset_drive_system();

        motion.start_turn(direction * (180), OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delayMicroseconds(systick.getLoopTime());
        }
        delay(1);
    }

    void turn_right()
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_turn(-90, OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    void turn_left()
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_turn(90, OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }
};