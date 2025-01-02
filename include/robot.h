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
    // Task 1
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

    // Task 2
    void solve_maze(int barcode)
    {
        bool wallExistOn1st = false;
        uint modeBC = barcode % 5;

        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        sensors.setFollowingColor(Sensors::BLACK); // temporily
        systick.enableSlowMode(false);
        sensors.disableUnknownToFollowing();

        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            if (sensors.sensorColors[0] == sensors.getFollowingColor())
            {
                break;
            }
            delay(systick.getLoopTime() * 1000);
        }

        helpers.go(MOVE_AFTER_DETECT, true);

        motion.reset_drive_system();

        helpers.turn_right();

        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move(1000, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == sensors.getFollowingColor() && sensors.sensorColors[1] == sensors.getFollowingColor())
            {
                sensors.set_steering_mode(STEERING_OFF);
                break;
            }
        }
        motion.reset_drive_system();

        if (modeBC == 0)
        {
            utils.turnOnLED();
            helpers.go(MOVE_AFTER_DETECT, true);
            motion.reset_drive_system();

            helpers.go_maze_distance(false);
            utils.turnOffLED();

            helpers.go_maze_distance_reverse(true);

            helpers.turn_right();
            helpers.go_maze_distance(true);
            helpers.turn_left();
            helpers.go_maze_distance(true);

            wallExistOn1st = sensors.center_bottom_tof <= OBSTACLE_MEASURING_DISTANCE;
            reporter.sendMsg(1111);

            helpers.turn_left();
            helpers.go_maze_distance(false);

            utils.turnOnLED();

            helpers.go_maze_distance_reverse(false);

            if (wallExistOn1st)
            {
                helpers.go_maze_distance_reverse(false);
                helpers.go_maze_distance_reverse(false);
            }

            utils.turnOffLED();

            helpers.go_maze_distance_reverse(true);
            helpers.turn_left();
            helpers.go_maze_distance(true);
            helpers.turn_right();
            helpers.go_maze_distance(true);
            helpers.turn_right();
            helpers.go_maze_distance(true);
            utils.turnOnLED();

            helpers.go(GRID_TO_COLOR_BOX_DISTANCE, true);
            utils.turnOffLED();
        }
        else
        {
            helpers.go(MOVE_AFTER_DETECT, true);
            motion.reset_drive_system();

            helpers.go_maze_distance(true);
            helpers.turn_right();
            helpers.go_maze_distance(true);

            wallExistOn1st = sensors.left_tof <= OBSTACLE_MEASURING_DISTANCE;

            if (modeBC == 1)
            {
                if (wallExistOn1st)
                {
                    helpers.turn_180();
                    helpers.go_maze_distance(true);
                    helpers.turn_left();
                    helpers.go_maze_distance(true);
                    helpers.turn_left();
                    helpers.go_maze_distance(false);

                    utils.turnOnLED();
                    helpers.go(MOVE_AFTER_DETECT, true);
                    helpers.go_maze_distance(true);
                    helpers.go_maze_distance(false);
                    utils.turnOffLED();

                    helpers.mirroredCshapeInMaze();
                }
            }

            else if (modeBC == 2)
            {
                helpers.turn_right();
                helpers.go_maze_distance(true);
                helpers.turn_left();
                helpers.go_maze_distance(false);
                utils.turnOnLED();

                if (!wallExistOn1st)
                {
                    helpers.go_maze_distance_reverse(false);
                    utils.turnOffLED();
                }
                else
                {
                    helpers.go(MOVE_AFTER_DETECT, true);
                    helpers.go_maze_distance(false);
                    utils.turnOffLED();
                }

                helpers.mirroredCshapeInMaze();
            }

            else if (modeBC == 3)
            {
                if (wallExistOn1st)
                {
                    helpers.go_maze_distance(true);
                    helpers.go_maze_distance(true);
                }
                else
                {
                    helpers.turn_right();
                    helpers.go_maze_distance(true);
                    helpers.turn_left();
                    helpers.go_maze_distance(true);
                    helpers.go_maze_distance(false);
                    utils.turnOnLED();

                    helpers.go_maze_distance_reverse(false);
                    helpers.go_maze_distance_reverse(false);
                    utils.turnOffLED();

                    helpers.mirroredCshapeInMaze();
                }
            }

            else if (modeBC == 4)
            {
                helpers.turn_right();
                helpers.go_maze_distance(true);
                helpers.turn_left();
                helpers.go_maze_distance(true);
                helpers.go_maze_distance(true);
                helpers.go_maze_distance(false);
                utils.turnOnLED();

                helpers.go_maze_distance_reverse(false);

                if(wallExistOn1st){
                    utils.turnOffLED();
                }else{
                    helpers.go_maze_distance_reverse(false);
                    helpers.go_maze_distance_reverse(false);
                    utils.turnOffLED();
                }

                helpers.mirroredCshapeInMaze();
            }

            helpers.turn_right();
            helpers.go_maze_distance(false);
            utils.turnOnLED();
            helpers.go_maze_distance_reverse(false);
            helpers.go(GRID_TO_COLOR_BOX_DISTANCE, false);
            utils.turnOffLED();

            helpers.turn_180();
        }
    }

    // Task 3
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


    // Task 4
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

    // Task 5
    void gateDetection()
    {
    }

    // Task 6
    void arrangeBox(bool isAscending)
    {
        int height1, height2;

        systick.enableSlowMode(false);
        delay(300);
        sensors.set_steering_mode(STEER_NORMAL);
        sensors.setFollowingColor(Sensors::BLACK);
        motion.reset_drive_system();
        sensors.enableUnknownToFollowing();

        servos.openArms();
        servos.liftDown();

        helpers.go_align_line_after();
        helpers.turn_left();
        height1 = helpers.go_for_grab();
        helpers.turn_180();

        int fiveEqualTo;

        if (isAscending)
        {
            fiveEqualTo = 5;
        }
        else
        {
            fiveEqualTo = 15;
        }

        if (height1 == fiveEqualTo) // 1st position
        {
            height2 = helpers.goMirrored4shape();
            if (height2 == 10) // 2nd position
            {
                helpers.goMirrored4shape();

                helpers.go_align_line_after();
                helpers.go_for_release();
                helpers.turn_180();
                helpers.go_align_line_after();
                helpers.turn_right();

                helpers.go_align_line_after();
            }
            else
            {
                helpers.go4shape();
            }
        }
        else if (height1 == 10) // 1st position
        {
            height2 = helpers.go4shape();

            if (height2 == fiveEqualTo) // 2nd position
            {
                helpers.goHalfSwatikaShape();

                helpers.go_align_line_after();
                helpers.turn_left();
                helpers.go_for_grab();

                helpers.turn_180();
                helpers.go_align_line_after();
                helpers.go_for_release();
                helpers.turn_180();

                helpers.go_align_line_after();
                helpers.turn_right();
                helpers.go_align_line_after();
            }
            else
            {
                helpers.go4shape();

                helpers.go3To5();

                helpers.go_align_line_after();
                helpers.go_align_line_after();
            }
        }
        else
        {
            helpers.go_align_line_after();
            helpers.turn_left();
            helpers.go_align_line_after();
            helpers.go_align_line_after();
            helpers.turn_right();
            helpers.go_for_release();
            helpers.turn_180();

            // go for straight 3rd position instead of 2nd
            helpers.go_align_line_after();
            height2 = helpers.go_for_grab();
            helpers.turn_180();

            if (height2 == fiveEqualTo) // 3rd position
            {
                helpers.go3To5();

                helpers.turn_left();
                helpers.go_for_grab();
                helpers.turn_180();
                helpers.go_align_line_after();
                helpers.go_for_release();
                helpers.turn_180();
                helpers.go_align_line_after();
                helpers.turn_right();

                helpers.go_align_line_after();
                helpers.go_align_line_after();
            }
            else
            {
                helpers.go_align_line_after();
                helpers.turn_right();
                helpers.go_align_line_after();
                helpers.turn_left();
                helpers.go_for_release();
                helpers.turn_180();
                helpers.go_align_line_after();
                helpers.go_for_grab();
                helpers.turn_180();

                helpers.goHalfSwatikaShape();
                helpers.go_align_line_after();

                helpers.go_align_line_after();
            }
        }
    }

    // Task 7
    void pickUpfromCheckpoint()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.disableUnknownToFollowing();

        servos.openArms();
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

    // Task 8
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
    }
};
