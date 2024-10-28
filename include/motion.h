#pragma once

#include <Arduino.h>
#include "motors.h"
#include "profile.h"

class Motion;
extern Motion motion;

class Motion
{
public:
  void reset_drive_system()
  {
    motors.stop();
    motors.disable_controllers();
    encoders.reset();
    forward.reset();
    rotation.reset();
    motors.reset_controllers();
    motors.enable_controllers();
  }

  void stop()
  {
    motors.stop();
  }

  void disable_drive()
  {
    motors.disable_controllers();
  }

  float position()
  {
    return forward.position();
  }

  float velocity()
  {
    return forward.speed();
  }

  float acceleration()
  {
    return forward.acceleration();
  }

  void set_target_velocity(float velocity)
  {
    forward.set_target_speed(velocity);
  }

  float angle()
  {
    return rotation.position();
  }

  float omega()
  {
    return rotation.speed();
  }

  float alpha()
  {
    return rotation.acceleration();
  }

  void start_move(float distance, float top_speed, float final_speed, float acceleration)
  {
    forward.start(distance, top_speed, final_speed, acceleration);
  }

  bool move_finished()
  {
    return forward.is_finished();
  }



  void start_turn(float distance, float top_speed, float final_speed, float acceleration)
  {
    rotation.start(distance, top_speed, final_speed, acceleration);
  }

  bool turn_finished()
  {
    return rotation.is_finished();
  }


  void update()
  {
    forward.update();
    rotation.update();
  }

  void set_position(float pos)
  {
    forward.set_position(pos);
  }

  void adjust_forward_position(float delta)
  {
    forward.adjust_position(delta);
  }



  //****************************************************************************//
  // void stop_at(float position)
  // {
  //   float remaining = position - forward.position();
  //   forward.move(remaining, forward.speed(), 0, forward.acceleration());
  // }

  // void stop_after(float distance)
  // {
  //   forward.move(distance, forward.speed(), 0, forward.acceleration());
  // }

  void wait_until_position(float position)
  {
    while (forward.position() < position)
    {
      delay(2);
    }
  }

  void wait_until_distance(float distance)
  {
    float target = forward.position() + distance;
    wait_until_position(target);
  }
};
