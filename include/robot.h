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

class Robot;
extern Robot robot;

class Robot
{
public:
    int code = 0;

    void turn_180()
    {
        sensors.set_steering_mode(STEERING_OFF);
        static int direction = 1;
        direction *= -1; // alternate direction each time it is called

        motion.reset_drive_system();

        motion.start_turn(direction * (195), OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
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

        // if (sensors.sensorColors[2] != sensors.getFollowingColor())
        // {
        //     motion.reset_drive_system();
        //     motion.start_turn(-30, OMEGA, 0, ALPHA);
        //     while (!sensors.sensorColors[2] != sensors.getFollowingColor())
        //     {
        //         delay(systick.getLoopTime() * 1000);
        //     }
        // }
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

        // if (sensors.sensorColors[2] != sensors.getFollowingColor())
        // {
        //     motion.reset_drive_system();
        //     motion.start_turn(30, OMEGA, 0, ALPHA);
        //     while (!sensors.sensorColors[2] != sensors.getFollowingColor())
        //     {
        //         delay(systick.getLoopTime() * 1000);
        //     }
        // }
    }

    void go(int distance, bool forward)
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();

        motion.start_move((forward ? 1 : -1) * distance, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    void go_30(bool forward)
    {
        if (forward)
        {
            sensors.set_steering_mode(STEER_NORMAL);
        }
        else
        {
            sensors.set_steering_mode(STEERING_OFF);
        }

        motion.reset_drive_system();
        motion.start_move((forward ? 1 : -1) * 300, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }

        if (sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
        {
            motion.reset_drive_system();

            motion.start_move((forward ? 1 : -1) * 30, RUN_SPEED, 0, ACCELERATION);

            while (!sensors.sensorColors[0] == Sensors::WHITE && sensors.sensorColors[1] == Sensors::WHITE || sensors.sensorColors[4] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                delay(systick.getLoopTime() * 1000);
            }
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

        go(MOVE_AFTER_DETECT, true);

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

    int8_t solve_maze(int position)
    {
        bool coveredOneIsFirst = false;
        if (position != 0)
        {
            go(LINE_WIDTH, true);
            go_30(true);
            go(MOVE_AFTER_DETECT, true);
            turn_right();
            go_30(true);

            if (sensors.left_tof < DIST_TO_BARRIER_FROM_LINE2)
            {
                coveredOneIsFirst = true;
            }
        }
        else
        {
            utils.turnOnLED();
            go(LINE_WIDTH, true);
            go_30(true);
            utils.turnOffLED();
            go_30(false); // go backward
            turn_right();
            go_30(true);

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
                go(LINE_WIDTH, true);
                turn_left();
                delay(400);
            }
            else if (!turnLeft && turnRight)
            {
                turnRight = false;
                turnLeft = false;
                go(LINE_WIDTH, true);
                turn_right();
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
                Serial.print("fuck");
                break;
            }
        }
        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);
    }





    void go_28_aligning_line()
    {
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(MIDDLE_BOX_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == Sensors::BLACK && sensors.sensorColors[4] == Sensors::BLACK)
            {
                break;
            }
        }

        go(MOVE_AFTER_DETECT, true);
    }

    void go_33_end()
    {
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(END_BOX_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    void grab(int height){
        if (height <5){
            servos.liftDown();
        }
        servos.closeArms();
        servos.liftUp();
    }

    void release(){
        servos.openArms();

        motion.start_move(-REVERSE_DISTANCE, RUN_SPEED , 0, ACCELERATION);

        while(!motion.move_finished()){
            delay(systick.getLoopTime() * 1000);
        }
    }

    int measureHeight(int distance)
    {
        go_28_aligning_line();

        turn_left();
        delay(400);

        servos.liftUp();
        

        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(BOX_BRANCH_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.front_tof <= 60 || sensors.center_bottom_tof <= 60)
            {
                break;
            }
        }
        motion.reset_drive_system();

        int height = servos.detectHeight();
        reporter.sendMsg(height);

        servos.liftDown();

        turn_180();
        delay(1100);

        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(BOX_BRANCH_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == Sensors::BLACK || sensors.sensorColors[4] == Sensors::BLACK)
            {
                break;
            }
        }
        motion.reset_drive_system();

        go(MOVE_AFTER_DETECT, true);

        turn_left();

        return height;
    }

    void arrangeBox(bool isAscending)
    {
        systick.enableSlowMode(false);
        delay(300);
        sensors.set_steering_mode(STEER_NORMAL);
        sensors.setFollowingColor(Sensors::BLACK);
        motion.reset_drive_system();
        sensors.enableUnknownToFollowing();

        servos.openArms();
        servos.liftDown();

        int height1 = measureHeight(INITIAL_BOX_DISTANCE);
        int height2 = measureHeight(MIDDLE_BOX_DISTANCE);
        int height3 = 30 - height1 - height2;

        reporter.sendMsg(height1);
        reporter.sendMsg(height2);
        reporter.sendMsg(height3);
    }








    void pickUpfromCheckpoint()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.enableUnknownToFollowing();

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

        go(MOVE_AFTER_DETECT, true);

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

        robot.turn_180();

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

        go(MOVE_AFTER_DETECT, true);

        motion.reset_drive_system();

        turn_left();
    }

    void insertChamber()
    {
        systick.enableSlowMode(false);
        delay(500);
        sensors.setFollowingColor(Sensors::BLACK);
        delay(100);
        sensors.enableUnknownToFollowing();

        go(LINE_WIDTH, true);

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

        go(60, false);

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

        go(MOVE_AFTER_DETECT, true);
        turn_right();
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

        sensors.disableUnknownToFollowing();
        motion.reset_drive_system();

        sensors.isInUneven = true;

        go(ROBOT_LENGTH, true);

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
                turn_right();
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
};
