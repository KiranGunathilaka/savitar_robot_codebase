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

class RobotHelpers;
extern RobotHelpers helpers;

class RobotHelpers
{
public:
    //************************************HELPER FUNCTIONS FOR TASK COMPLETION FUNCTIONS IN ROBOT CLASS*********************************** */

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

    void go_28_align_line_after()
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

    void go_33_grab()
    {
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
    }

    void go_33_release()
    {
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(END_BOX_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    void grab(int height)
    {
        if (height < 5)
        {
            servos.liftDown();
        }
        servos.closeArms();
        servos.liftUp();
    }

    void release()
    {
        servos.openArms();

        motion.start_move(-REVERSE_DISTANCE, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    int measureHeight(int distance)
    {
        go_28_align_line_after();

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

    void go_30_maze(bool forward)
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
};