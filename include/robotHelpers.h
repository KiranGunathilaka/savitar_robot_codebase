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
        motion.reset_drive_system();
        motion.start_turn(180 + CORRECTION_OFFSET_180, OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }

        delay(1000);

        motion.reset_drive_system();
    }

    void turn_right()
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_turn(-90 - CORRECTION_OFFSET_90, OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    void turn_left()
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_turn(90 + CORRECTION_OFFSET_90, OMEGA, 0, ALPHA);

        while (!motion.turn_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    void go_maze_distance(bool shouldMove)
    {
        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(MAZE_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == sensors.getFollowingColor() && sensors.sensorColors[1] == sensors.getFollowingColor() || sensors.sensorColors[3] == sensors.getFollowingColor() && sensors.sensorColors[4] == sensors.getFollowingColor())
            {
                sensors.set_steering_mode(STEERING_OFF);
                break;
            }
        }

        if (shouldMove)
        {
            go(MOVE_AFTER_DETECT, true);
        }

        motion.reset_drive_system();
    }

    void go_maze_distance_reverse(bool shouldMove)
    {
        go(INITIAL_REVERSING_DISTANCE, false);

        sensors.set_is_reverse(true);
        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);

        motion.start_move(-MAZE_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == sensors.getFollowingColor() && sensors.sensorColors[1] == sensors.getFollowingColor() || sensors.sensorColors[3] == sensors.getFollowingColor() && sensors.sensorColors[4] == sensors.getFollowingColor())
            {
                sensors.set_steering_mode(STEERING_OFF);
                break;
            }
        }

        sensors.set_is_reverse(false);
        if (shouldMove)
        {
            go(MOVE_AFTER_DETECT, true);
        }
        motion.reset_drive_system();
    }

    void mirroredCshapeInMaze()
    {
        helpers.go_maze_distance_reverse(true);
        helpers.turn_left();
        helpers.go_maze_distance(true);
        helpers.turn_right();
        helpers.go_maze_distance(true);
    }

    void go_align_line_after()
    {
        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(BOX_BRANCH_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[0] == sensors.getFollowingColor() && sensors.sensorColors[4] == sensors.getFollowingColor())
            {
                break;
            }
        }

        go(MOVE_AFTER_DETECT, true);

        motion.reset_drive_system();
    }

    void grab(int height)
    {
        if (height == 5)
        {
            servos.liftDown();
        }
        servos.closeArms();
        servos.liftUp();
    }

    void release()
    {
        servos.liftDown();
        servos.openArms();
        servos.liftDown();

        motion.reset_drive_system();
        sensors.set_steering_mode(STEERING_OFF);
        motion.start_move(-REVERSE_DISTANCE, RUN_SPEED, 0, ACCELERATION);

        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
        }
    }

    int go_for_grab()
    {
        servos.liftUp();

        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(BOX_BRANCH_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.front_tof <= HEIGHT_DETECTING_DISTANCE || sensors.center_bottom_tof <= HEIGHT_DETECTING_DISTANCE)
            {
                break;
            }
        }
        motion.reset_drive_system();

        int height = servos.detectHeight();
        reporter.sendMsg(height);

        motion.start_move(HEIGHT_DETECTING_DISTANCE, RUN_SPEED / 2, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.front_tof <= PICKING_UP_DISTANCE)
            {
                break;
            }
        }

        motion.reset_drive_system();

        grab(height);

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

        motion.reset_drive_system();
    }

    void go_for_release()
    {
        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(BOX_BRANCH_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.sensorColors[1] == Sensors::WHITE && sensors.sensorColors[2] == Sensors::WHITE && sensors.sensorColors[3] == Sensors::WHITE)
            {
                break;
            }
        }
        motion.reset_drive_system();

        release();
    }

    // from here |   |
    //            --
    //           |
    int goMirrored4shape()
    {
        helpers.go_align_line_after();
        helpers.go_for_release();
        helpers.turn_180();
        helpers.go_align_line_after();
        helpers.turn_right();
        helpers.go_align_line_after();
        helpers.turn_left();
        int height = helpers.go_for_grab();
        helpers.turn_180();

        return height;
    }

    // from here|  |
    //           --
    //             |
    int go4shape()
    {
        helpers.go_align_line_after();
        helpers.turn_left();
        helpers.go_align_line_after();
        helpers.turn_right();
        helpers.go_for_release();
        helpers.turn_180();
        helpers.go_align_line_after();
        int height = helpers.go_for_grab();
        helpers.turn_180();

        return height;
    }

    //        | from here
    //      ==
    //    ||
    void goHalfSwatikaShape()
    {
        helpers.go_align_line_after();
        helpers.turn_right();
        helpers.go_align_line_after();
        helpers.turn_left();
        helpers.go_for_release();
        helpers.turn_180();
        helpers.go_align_line_after();
        helpers.turn_right();
        helpers.go_align_line_after();
    }

    // from 3 to other sides 1
    void go3To5()
    {
        helpers.go_align_line_after();
        helpers.turn_right();
        helpers.go_align_line_after();
        helpers.go_align_line_after();
        helpers.turn_left();
        helpers.go_for_release();
        helpers.turn_180();

        helpers.go_align_line_after();
        helpers.turn_right();
        helpers.go_align_line_after();
    }

    // functions that didn't use after new revision
    int measureHeight(int distance)
    {
        go_align_line_after();

        turn_left();
        delay(400);

        servos.liftUp();

        motion.reset_drive_system();
        sensors.set_steering_mode(STEER_NORMAL);
        motion.start_move(BOX_BRANCH_DISTANCE, RUN_SPEED, 0, ACCELERATION);
        while (!motion.move_finished())
        {
            delay(systick.getLoopTime() * 1000);
            if (sensors.front_tof <= HEIGHT_DETECTING_DISTANCE || sensors.center_bottom_tof <= HEIGHT_DETECTING_DISTANCE)
            {
                break;
            }
        }
        motion.reset_drive_system();

        int height = servos.detectHeight();
        reporter.sendMsg(height);
        servos.liftDown();

        turn_180();
        delay(1000);

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
        delay(1000);
        return height;
    }
};