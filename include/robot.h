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

    void go_forward(int distance)
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();

        motion.start_move(distance, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
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
        sensors.disableUnknownToFollowing();

        motion.start_move(2000, READ_SPEED, 0, ACCELERATION);

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
        sensors.set_steering_mode(STEER_NORMAL);
        sensors.setFollowingColor(Sensors::WHITE);
        systick.enableSlowMode(false);
        sensors.disableUnknownToFollowing();

        bool turnRight = false;
        bool turnLeft = false;

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::WHITE)
            {
                turnRight = true;
                turnLeft = false;
                break;
            }
            else if (sensors.sensorColors[4] == Sensors::WHITE)
            {
                turnRight = false;
                turnLeft = true;
                break;
            };
            delay(systick.getLoopTime() * 1000);
        }

        go_forward(MOVE_AFTER_DETECT);

        motion.reset_drive_system();

        if (turnLeft && !turnRight)
        {
            turn_left();
        }
        else if (!turnLeft && turnRight)
        {
            turn_right();
        }

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                sensors.set_steering_mode(STEERING_OFF);

                motion.reset_drive_system();
                return barcode % 5;
            }
            delay(systick.getLoopTime() * 1000);
        }
        return barcode;
    }

    void go_forward_30()
    {
        go_forward(30);

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(400, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                sensors.set_steering_mode(STEERING_OFF);
                motion.reset_drive_system();
                break;
            }
            delay(systick.getLoopTime() * 1000);
        }
    }

    int8_t solve_maze(int position)
    {
        if (position != 0)
        {
            
        }
        return 0;
    }
};
