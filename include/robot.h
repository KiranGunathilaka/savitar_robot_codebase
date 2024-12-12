#pragma once

#include "Arduino.h"
#include "config.h"
#include "motion.h"
#include "sensors.h"
#include "systick.h"
#include "reporting.h"
#include "nvs.h"

class Robot;
extern Robot robot;

class Robot
{
public:
    int code = 0;

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

    void is_left_equal()
    {
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

    void is_right_equal()
    {
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

    uint16_t detectBarCode()
    {
        uint16_t code = 0;

        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);
        systick.enableSlowMode(false);
        motion.start_move(2000, 100, 0, 1000);

        float distance = 0;
        bool startTrackingWhite = false;
        bool barcodeStarted = false;

        while (!motion.move_finished())
        {
            bool leftBlack = sensors.sensorColors[0] == Sensors::BLACK;
            bool rightBlack = sensors.sensorColors[4] == Sensors::BLACK;
            bool leftWhite = sensors.sensorColors[0] == Sensors::WHITE;
            bool rightWhite = sensors.sensorColors[4] == Sensors::WHITE;

            if (leftBlack && rightBlack)
            {
                if (!barcodeStarted)
                {
                    barcodeStarted = true;
                }
                else if (barcodeStarted && startTrackingWhite)
                {
                    distance = encoders.robotDistanceBack() - distance;
                    startTrackingWhite = false;
                    if (distance > BARCODE_THRESHOLD_DISTANCE)
                    {
                        code = (code << 1) | 1;
                    }
                    else
                    {
                        code = (code << 1);
                    }
                }
            }
            else if (leftWhite && rightWhite && barcodeStarted && !startTrackingWhite)
            {
                distance = encoders.robotDistanceBack();
                startTrackingWhite = true;
            }

            if (code >= 7 && ((code & 0b111) == 0b000 || (code & 0b111) == 0b111))
            {
                break;
            }
        }
        return code >> 3;
    }

    int8_t maze_entrance(int barcode)
    {
        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);
        sensors.setFollowingColor(Sensors::WHITE);
        systick.enableSlowMode(false);

        bool turnRight = false;
        bool turnLeft = false;

        motion.start_move(1000, 40, 0, 1000);
        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE)
            {
                turnRight = true;
                turnLeft = false;
                reporter.sendMsg(000);
                break;
            }
            else if (sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                turnRight = false;
                turnLeft = true;
                reporter.sendMsg(333);
                break;
            };
            delay(systick.getLoopTime() * 1000);
        }
        motion.reset_drive_system();

        if (turnLeft && !turnRight)
        {
            reporter.sendMsg(111);
            turn_left();
        }
        else if (!turnLeft && turnRight)
        {
            reporter.sendMsg(222);
            turn_right();
        }

        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_move(1000, 100, 0, 1000);
        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                sensors.set_steering_mode(STEERING_OFF);
                return barcode % 5;
            }
        }
        return barcode;
    }
};