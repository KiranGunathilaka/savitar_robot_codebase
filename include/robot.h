#pragma once

#include "Arduino.h"
#include "config.h"
#include "motion.h"
#include "sensors.h"
#include "systick.h"
#include "reporting.h"
#include "nvs.h"
#include "utils.h"
#include "servos.h"
#include "robotHelpers.h"

class Robot;
extern Robot robot;

RobotHelpers helpers;

class Robot
{
public:

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
        return code;
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

        helpers.go(MOVE_AFTER_DETECT, true);

        motion.reset_drive_system();

        if (turnLeft && !turnRight)
        {
            helpers.turn_left();
        }
        else if (!turnLeft && turnRight)
        {
            helpers.turn_right();
        }

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                sensors.set_steering_mode(STEERING_OFF);
                break;
            }
            delay(systick.getLoopTime() * 1000);
        }
        motion.reset_drive_system();
        return barcode % 5;
    }

    int8_t indicatePosition(int position)
    {
        if (position == 0)
        {
            utils.turnOnLED();
            delay(1000);
        }
        else
        {
            motion.reset_drive_system();
            helpers.go(MOVE_AFTER_DETECT, true);

            helpers.turn_right();
            delay(600);

            sensors.setFollowingColor(Sensors::WHITE);
            sensors.set_steering_mode(STEER_NORMAL);

            if (position == 1)
            {

                motion.start_move(400, RUN_SPEED, 0, ACCELERATION);
                while (!motion.move_finished())
                {
                    delay(systick.getLoopTime() * 1000);
                    if (sensors.sensorColors[3] == Sensors::WHITE && sensors.sensorColors[4] == Sensors::WHITE)
                    {
                        break;
                    }
                }
                motion.reset_drive_system();
                utils.turnOnLED();
            }
            else if (position == 2)
            {
                for (int i = 0; i < 2; i++)
                {
                    motion.start_move(400, RUN_SPEED, 0, ACCELERATION);
                    while (!motion.move_finished())
                    {
                        delay(systick.getLoopTime() * 1000);
                        if (sensors.sensorColors[3] == Sensors::WHITE && sensors.sensorColors[4] == Sensors::WHITE)
                        {
                            break;
                        }
                    }
                    motion.reset_drive_system();
                }
                utils.turnOnLED();
            }
            else if (position == 3)
            {
                for (int i = 0; i < 3; i++)
                {
                    motion.start_move(400, RUN_SPEED, 0, ACCELERATION);
                    while (!motion.move_finished())
                    {
                        delay(systick.getLoopTime() * 1000);
                        if (sensors.sensorColors[3] == Sensors::WHITE && sensors.sensorColors[4] == Sensors::WHITE)
                        {
                            break;
                        }
                    }
                    motion.reset_drive_system();
                }
                utils.turnOnLED();
            }
            else if (position == 2)
            {
                for (int i = 0; i < 4; i++)
                {
                    motion.start_move(400, RUN_SPEED, 0, ACCELERATION);
                    while (!motion.move_finished())
                    {
                        delay(systick.getLoopTime() * 1000);
                        if (sensors.sensorColors[3] == Sensors::WHITE && sensors.sensorColors[4] == Sensors::WHITE)
                        {
                            break;
                        }
                    }
                    motion.reset_drive_system();
                }
                utils.turnOnLED();
            }
        }
        return 0;
    }

    int8_t solve_maze(int position)
    {
        bool coveredOneIsFirst = false;
        if (position != 0)
        {
            helpers.go(LINE_WIDTH, true);
            helpers.go_30_maze(true);
            helpers.go(MOVE_AFTER_DETECT, true);
            helpers.turn_right();
            helpers.go_30_maze(true);

            if (sensors.left_tof < DIST_TO_BARRIER_FROM_LINE2)
            {
                coveredOneIsFirst = true;
            }
        }
        else
        {
            utils.turnOnLED();
            helpers.go(LINE_WIDTH, true);
            helpers.go_30_maze(true);
            utils.turnOffLED();
            helpers.go_30_maze(false); // go backward
            helpers.turn_right();
            helpers.go_30_maze(true);

            if (sensors.left_tof < DIST_TO_BARRIER_FROM_LINE1)
            {
                coveredOneIsFirst = true;
            }
        }
        return 0;
    }

    uint8_t colorLineFollowing()
    {
        systick.enableSlowMode(true);
        delay(300);
        sensors.set_steering_mode(STEERING_OFF);
        Sensors::Colors color = sensors.sensorColors[2];
        sensors.setFollowingColor(color);

        motion.reset_drive_system();
        motion.start_move(100, RUN_SPEED, RUN_SPEED, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }

        sensors.disableUnknownToFollowing();

        while (true)
        {
            Serial.println("fuck");

            sensors.set_steering_mode(STEER_NORMAL);
            motion.reset_drive_system();

            motion.start_move(3000, RUN_SPEED, 0, ACCELERATION);

            bool turnLeft = false;
            bool turnRight = false;

            while (!motion.move_finished())
            {
                delay(systick.getLoopTime() * 1000);

                if (sensors.sensorColors[0] == Sensors::WHITE || sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[2] == Sensors::WHITE || sensors.sensorColors[3] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE)
                {
                    break;
                }

                if (sensors.sensorColors[4] == sensors.getFollowingColor() && sensors.sensorColors[3] == sensors.getFollowingColor() && sensors.sensorColors[0] == sensors.getFollowingColor() && sensors.sensorColors[1] == sensors.getFollowingColor())
                {
                    continue;
                }
                else if (sensors.sensorColors[4] == sensors.getFollowingColor() && sensors.sensorColors[3] == sensors.getFollowingColor())
                {
                    turnLeft = true;
                    turnRight = false;
                    break;
                }
                else if (sensors.sensorColors[0] == sensors.getFollowingColor() && sensors.sensorColors[1] == sensors.getFollowingColor())
                {
                    turnRight = true;
                    turnLeft = false;
                    break;
                }
            }

            motion.reset_drive_system();

            if (turnLeft && !turnRight)
            {
                turnLeft = false;
                turnRight = false;
                helpers.go(LINE_WIDTH, true);
                helpers.turn_left();
                delay(400);
            }
            else if (!turnLeft && turnRight)
            {
                turnRight = false;
                turnLeft = false;
                helpers.go(LINE_WIDTH, true);
                helpers.turn_right();
                delay(400);
            }
        }
        motion.reset_drive_system();
        delay(100);
        return color == Sensors::BLUE ? 1 : 0; // ascending 1
    }

    void dashLineFollowing()
    {

        sensors.set_steering_mode(STEER_NORMAL);
        sensors.setFollowingColor(Sensors::WHITE);
        delay(200);
        systick.enableSlowMode(false);
        delay(200);

        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);
        delay(500);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[1] == Sensors::WHITE && sensors.sensorColors[2] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                break;
            }
        }
        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);
    }

        void one_to_one()
    {
        helpers.turn_180();
        delay(500);
        helpers.go_28_align_line_after();
        helpers.turn_right();
        helpers.go_33_grab();
        helpers.grab(5);
        helpers.turn_180();
        delay(1000);
        helpers.go_33_release();
        helpers.go_33_release();
        helpers.release();
        helpers.turn_180();
        helpers.go_33_release();
        helpers.turn_right();
        helpers.go_28_align_line_after();
    }

    void arrangeBox(bool isAscending)
    {
        systick.enableSlowMode(false);
        delay(300);
        sensors.set_steering_mode(STEER_NORMAL);
        sensors.setFollowingColor(Sensors::BLACK);
        motion.reset_drive_system();
        sensors.enableUnknownToFollowing();

        // servos.openArms();
        servos.liftDown();

        int height1 = helpers.measureHeight(INITIAL_BOX_DISTANCE);
        int height2 = helpers.measureHeight(MIDDLE_BOX_DISTANCE);
        int height3 = 30 - height1 - height2;

        reporter.sendMsg(height1);
        reporter.sendMsg(height2);
        reporter.sendMsg(height3);

        one_to_one();
    }

    void pickUpfromCheckpoint()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.disableUnknownToFollowing();

        // servos.openArms();
        servos.liftDown();

        sensors.set_steering_mode(STEER_NORMAL);

        motion.reset_drive_system();

        bool turnRight = false;
        bool turnLeft = false;

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::BLACK)
            {
                turnRight = true;
                turnLeft = false;
                break;
            }
            else if (sensors.sensorColors[4] == Sensors::BLACK)
            {
                turnRight = false;
                turnLeft = true;
                break;
            };
            delay(systick.getLoopTime() * 1000);
        }

        helpers.go(MOVE_AFTER_DETECT, true);

        motion.reset_drive_system();

        if (turnLeft && !turnRight)
        {
            helpers.turn_left();
        }
        else if (!turnLeft && turnRight)
        {
            helpers.turn_right();
        }

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK && sensors.sensorColors[3] == Sensors::BLACK)
            {
                sensors.set_steering_mode(STEERING_OFF);
                break;
            }
        }

        motion.reset_drive_system();

        motion.start_move(50, 50, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
        motion.reset_drive_system();

        servos.closeArms();
        delay(200);
        servos.liftUp();

        helpers.turn_180();

        motion.reset_drive_system();
        delay(50);
        sensors.set_steering_mode(STEER_NORMAL);

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK && sensors.sensorColors[3] == Sensors::BLACK)
            {
                break;
            }
            delay(systick.getLoopTime() * 1000);
        }

        helpers.go(MOVE_AFTER_DETECT, true);

        motion.reset_drive_system();

        helpers.turn_left();
    }

    void insertChamber()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.disableUnknownToFollowing();

        helpers.go(LINE_WIDTH, true);

        servos.liftDown();

        sensors.set_steering_mode(STEER_NORMAL);

        motion.reset_drive_system();

        motion.start_move(120, 40, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.center_bottom_tof <= 160)
            {
                break;
            }
        }
        motion.reset_drive_system();

        sensors.set_steering_mode(STEERING_OFF);
        servos.openArms();

        helpers.go(60, false);

        servos.closeArmsFull();

        motion.start_move(120, 30, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.center_bottom_tof <= 90)
            {
                break;
            }
        }
        motion.reset_drive_system();

        motion.start_move(-120, 50, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());
            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK)
            {
                break;
            }
        }

        helpers.go(MOVE_AFTER_DETECT, true);
        helpers.turn_right();
    }

    void unevenTerrain()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.enableUnknownToFollowing();
        delay(500);
        sensors.set_steering_mode(STEER_NORMAL);

        motion.start_move(300, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK)
            {
                break;
            }
        }
        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);

        if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK && sensors.sensorColors[3] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK)
        {

            motion.start_turn(30, OMEGA, 0, ALPHA);
            while (!(sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK && sensors.sensorColors[3] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK))
            {
                delay(systick.getLoopTime() * 1000);
            }
        }

        motion.reset_drive_system();

        motion.start_move(300, 50, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);

            int count = 0;
            bool whiteTracked = false;
            if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[4] == Sensors::WHITE && !whiteTracked)
            {
                whiteTracked = true;
                count++;
            }
            else if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK && whiteTracked)
            {
                whiteTracked = false;
            }

            if (count >= 3)
            {
                break;
            }
        }

        // sensors.disableUnknownToFollowing();
        motion.reset_drive_system();

        helpers.go(ROBOT_LENGTH, true);

        servos.liftUp();

        bool placeCoin = false;
        int i = 4;
        while (!(sensors.sensorColors[0] == Sensors::BLACK || sensors.sensorColors[1] == Sensors::BLACK || sensors.sensorColors[2] == Sensors::BLACK || sensors.sensorColors[3] == Sensors::BLACK || sensors.sensorColors[4] == Sensors::BLACK))
        {

            bool turnRight = false;
            motion.reset_drive_system();

            motion.start_move(2000, RUN_SPEED, 0, ACCELERATION);
            while (!motion.move_finished())
            {
                delay(systick.getLoopTime() * 1000);

                if (sensors.sensorColors[0] == Sensors::BLACK || sensors.sensorColors[1] == Sensors::BLACK || sensors.sensorColors[2] == Sensors::BLACK || sensors.sensorColors[3] == Sensors::BLACK || sensors.sensorColors[4] == Sensors::BLACK)
                {
                    placeCoin = true;
                    break;
                }
                if (sensors.front_tof < WALL_FOLLOWING_DIATANCE * ((int)i / 4))
                {
                    turnRight = true;
                    break;
                }
            }
            if (turnRight)
            {
                helpers.turn_right();
                turnRight = false;
            }
            i++;
            if (placeCoin)
            {
                break;
            }
        }
        motion.reset_drive_system();

        motion.start_move(50, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());
        }

        utils.turnOffEM();

        sensors.isInUneven = false;
    }

    void pickObjects()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.disableUnknownToFollowing();
        delay(500);
        sensors.set_steering_mode(STEER_NORMAL);

        motion.reset_drive_system();

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK)
            {
                break;
            }
        }

        motion.reset_drive_system();

        helpers.go(LINE_WIDTH, true);

        sensors.set_steering_mode(STEER_NORMAL);

        motion.reset_drive_system();

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK)
            {
                break;
            }
        }

        helpers.go(MOVE_AFTER_DETECT, true);

        helpers.turn_right();
        delay(700);

        servos.liftUp();
        delay(1000);

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.sensorColors[1] == Sensors::BLACK && sensors.sensorColors[2] == Sensors::BLACK && sensors.sensorColors[3] == Sensors::BLACK)
            {
                break;
            }
        }

        motion.reset_drive_system();

        servos.liftDown();

        helpers.turn_180();
        delay(1000);

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK)
            {
                break;
            }
        }
        motion.reset_drive_system();

        helpers.go(MOVE_AFTER_DETECT, true);

        helpers.turn_right();

        sensors.set_steering_mode(STEER_NORMAL);

        motion.reset_drive_system();

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delayMicroseconds(systick.getLoopTime());

            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[1] == Sensors::BLACK)
            {
                break;
            }
        }
    }
};
