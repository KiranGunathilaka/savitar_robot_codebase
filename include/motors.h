#pragma once

#include <Arduino.h>
#include "config.h"
#include "encoders.h"

class Motors;
extern Motors motors;

class Motors
{
private:
  float previous_fwd_error_back;
  float previous_rot_error_back;
  float fwd_error_back;
  float rot_error_back;

  float previous_fwd_error_front;
  float previous_rot_error_front;
  float fwd_error_front;
  float rot_error_front;

  float velocity;
  float omega;

  bool feedforward_enabled = true;
  bool controller_output_enabled;

public:
  // remove this after testing
  float fwdKpBack = FWD_KP_BACK;
  float fwdKdBack = FWD_KD_BACK;
  float rotKpBack = ROT_KP_BACK;
  float rotKdBack = ROT_KD_BACK;

  float fwdKpFront = FWD_KP_FRONT;
  float fwdKdFront = FWD_KD_FRONT;
  float rotKpFront = ROT_KP_FRONT;
  float rotKdFront = ROT_KD_FRONT;

  float left_back_motor_percentage;
  float right_back_motor_percentage;
  float right_front_motor_percentage;
  float left_front_motor_percentage;

  void begin()
  {
    pinMode(LEFT_BACK_MOTOR_PWM, OUTPUT);
    pinMode(LEFT_BACK_MOTOR_IN1, OUTPUT);
    pinMode(LEFT_BACK_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_BACK_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_BACK_MOTOR_IN1, OUTPUT);
    pinMode(RIGHT_BACK_MOTOR_PWM, OUTPUT);

    digitalWrite(LEFT_BACK_MOTOR_PWM, 0);
    digitalWrite(LEFT_BACK_MOTOR_IN1, 0);
    digitalWrite(LEFT_BACK_MOTOR_IN2, 0);
    digitalWrite(RIGHT_BACK_MOTOR_IN2, 0);
    digitalWrite(RIGHT_BACK_MOTOR_IN1, 0);
    digitalWrite(RIGHT_BACK_MOTOR_PWM, 0);

    pinMode(LEFT_FRONT_MOTOR_PWM, OUTPUT);
    pinMode(LEFT_FRONT_MOTOR_IN1, OUTPUT);
    pinMode(LEFT_FRONT_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_FRONT_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_FRONT_MOTOR_IN1, OUTPUT);
    pinMode(RIGHT_FRONT_MOTOR_PWM, OUTPUT);

    digitalWrite(LEFT_FRONT_MOTOR_PWM, 0);
    digitalWrite(LEFT_FRONT_MOTOR_IN1, 0);
    digitalWrite(LEFT_FRONT_MOTOR_IN2, 0);
    digitalWrite(RIGHT_FRONT_MOTOR_IN2, 0);
    digitalWrite(RIGHT_FRONT_MOTOR_IN1, 0);
    digitalWrite(RIGHT_FRONT_MOTOR_PWM, 0);

    setupPWM();
  }

  void setupPWM()
  {
    ledcSetup(LEFT_BACK_PWM_CHANNEL, 5000, PWM_RESOLUTION_BITS); // check for different pwm frequencies
    ledcAttachPin(LEFT_BACK_MOTOR_PWM, LEFT_BACK_PWM_CHANNEL);
    ledcSetup(RIGHT_BACK_PWM_CHANNEL , 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_BACK_MOTOR_PWM, RIGHT_BACK_PWM_CHANNEL );
    ledcSetup(LEFT_FRONT_PWM_CHANNEL, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(LEFT_FRONT_MOTOR_PWM, LEFT_FRONT_PWM_CHANNEL);
    ledcSetup(RIGHT_FRONT_PWM_CHANNEL, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_FRONT_MOTOR_PWM, RIGHT_FRONT_PWM_CHANNEL);
  }

  void reset_controllers()
  {
    fwd_error_back = 0;
    rot_error_back = 0;
    previous_fwd_error_back = 0;
    previous_rot_error_back = 0;
  }

  void enable_controllers()
  {
    controller_output_enabled = true;
  }

  void disable_controllers()
  {
    controller_output_enabled = false;
  }

  void stop()
  {
    set_left_back_motor_percentage(0);
    set_right_back_motor_percentage(0);
    set_left_front_motor_percentage(0);
    set_right_front_motor_percentage(0);
  }

  float position_controller_front()
  {
    float increment = velocity * encoders.getLoopTime();
    fwd_error_front += increment - encoders.robot_fwd_change_front();
    float diff = fwd_error_front - previous_fwd_error_front;
    previous_fwd_error_front = fwd_error_front;

    // change them to config kp kd later
    float output = fwdKpFront * fwd_error_front + fwdKdFront * diff;
    return output;
  }

  float angle_controller_front(float steering_adjustment)
  {
    float increment = omega * encoders.getLoopTime();
    rot_error_front += increment - encoders.robot_rot_change_front();
    float diff = rot_error_front - previous_rot_error_front;
    previous_rot_error_front = rot_error_front;

    rot_error_front -= steering_adjustment;

    // changethis kp kd to config kp kd later
    float output = rotKpFront * rot_error_front + rotKdFront * diff;
    return output;
  }

  float position_controller_back()
  {
    float increment = velocity * encoders.getLoopTime();
    fwd_error_back += increment - encoders.robot_fwd_change_back();
    float diff = fwd_error_back - previous_fwd_error_back;
    previous_fwd_error_back = fwd_error_back;

    // change them to config kp kd later
    float output = fwdKpBack * fwd_error_back + fwdKdBack * diff;
    return output;
  }

  float angle_controller_back(float steering_adjustment)
  {
    float increment = omega * encoders.getLoopTime();
    rot_error_back += increment - encoders.robot_rot_change_back();
    float diff = rot_error_back - previous_rot_error_back;
    previous_rot_error_back = rot_error_back;

    rot_error_back -= steering_adjustment;

    // changethis kp kd to config kp kd later
    float output = rotKpBack * rot_error_back + rotKdBack * diff;
    return output;
  }

  // feed forward functions gives the percentage required to acheive a given velocity
  float left_front_feed_forward_percentage(float left_front_feed_velocity)
  {
    int l_rps = (left_front_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION_FRONT;
    float l_feed_percentage;
    if (l_rps >= 0)
    {
      l_feed_percentage = 0.0220 * l_rps - 3.4823;
    }
    else
    {
      l_feed_percentage = 0.0217 * l_rps + 5.2456;
    }

    return l_feed_percentage;
  }

  float right_front_feed_forward_percentage(float right_front_feed_velocity)
  {
    int r_rps = (right_front_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION_FRONT;

    float r_feed_percentage;
    if (r_rps >= 0)
    {
      r_feed_percentage = 0.0215 * r_rps - 2.6277;
    }
    else
    {
      r_feed_percentage = 0.0225 * r_rps + 5.3887;
    }

    return r_feed_percentage;
  }

  float left_back_feed_forward_percentage(float left_back_feed_velocity)
  {
    int l_rps = (left_back_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION_BACK;

    float l_feed_percentage;
    if (l_rps >= 0)
    {
      l_feed_percentage = 0.0221 * l_rps - 3.5795;
    }
    else
    {
      l_feed_percentage = 0.0222 * l_rps + 5.8467;
    }

    return l_feed_percentage;
  }

  float right_back_feed_forward_percentage(float right_back_feed_velocity)
  {
    int r_rps = (right_back_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION_BACK;

    float r_feed_percentage;
    if (r_rps >= 0)
    {
      r_feed_percentage = 0.0210 * r_rps - 5.1386;
    }
    else
    {
      r_feed_percentage = 0.0224 * r_rps + 7.2503;
    }

    return r_feed_percentage;
  }

  void update(float vel, float omg, float steering)
  {
    velocity = vel;
    omega = omg;

    // common parameters for both front and back pairs
    float tangent_speed = omega * ROBOT_RADIUS * RADIANS_PER_DEGREE;
    float left_speed = velocity - tangent_speed;
    float right_speed = velocity + tangent_speed;

    // pair-wise calculation for two pairs of motors
    float pos_output_back = position_controller_back();
    float rot_output_back = angle_controller_back(steering);
    float pos_output_front = position_controller_front();
    float rot_output_front = angle_controller_front(steering);

    float left_back_output = 0;
    float right_back_output = 0;
    float left_front_output = 0;
    float right_front_output = 0;

    left_back_output = pos_output_back - rot_output_back;
    right_back_output = pos_output_back + rot_output_back;
    left_front_output = pos_output_front - rot_output_front;
    right_front_output = pos_output_front + rot_output_front;

    float left_back_ff = left_back_feed_forward_percentage(left_speed);
    float right_back_ff = right_back_feed_forward_percentage(right_speed);
    float left_front_ff = left_front_feed_forward_percentage(left_speed);
    float right_front_ff = right_front_feed_forward_percentage(right_speed);

    if (feedforward_enabled)
    {
      left_back_output += left_back_ff;
      right_back_output += right_back_ff;
      left_front_output += left_front_ff;
      right_front_output += right_front_ff;
    }

    if (controller_output_enabled)
    {
      set_left_back_motor_percentage(left_back_output);
      set_right_back_motor_percentage(right_back_output);
      set_left_front_motor_percentage(left_front_output);
      set_right_front_motor_percentage(right_front_output);
    }
  }

  void set_left_back_motor_percentage(float percentage)
  {
    percentage = constrainPercentage(percentage);

    left_back_motor_percentage = percentage;
    int left_pwm = calculate_pwm(percentage);
    set_left_back_motor_pwm(left_pwm);
  }

  void set_right_back_motor_percentage(float percentage)
  {
    percentage = constrainPercentage(percentage);

    right_back_motor_percentage = percentage;
    int right_pwm = calculate_pwm(percentage);
    set_right_back_motor_pwm(right_pwm);
  }

  void set_left_front_motor_percentage(float percentage)
  {
    percentage = constrainPercentage(percentage);

    left_front_motor_percentage = percentage;
    int left_pwm = calculate_pwm(percentage);
    set_left_front_motor_pwm(left_pwm);
  }

  void set_right_front_motor_percentage(float percentage)
  {
    percentage = constrainPercentage(percentage);

    right_front_motor_percentage = percentage;
    int right_pwm = calculate_pwm(percentage);
    set_right_front_motor_pwm(right_pwm);
  }

  // limits the given percentage to minimum bias and maximum value (needs to overcome motor inequalities at lower pwm,
  // friction and give headroom for PID values to adjust in the higher edge)
  float constrainPercentage(float percentage)
  {
    percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
    if (percentage > MIN_MOTOR_PERCENTAGE)
    {
      percentage = map(percentage, MIN_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE, MIN_MOTOR_BIAS, MAX_MOTOR_REACH);
    }
    else if (percentage < -MIN_MOTOR_PERCENTAGE)
    {
      percentage = map(percentage, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_PERCENTAGE, -MAX_MOTOR_REACH, -MIN_MOTOR_BIAS);
    }
    else if (percentage >= -MIN_MOTOR_PERCENTAGE && percentage <= MIN_MOTOR_PERCENTAGE)
    {
      percentage = 0;
    }

    return percentage;
  }

  // sets pwm values and directions by these seperate functions for each motor
  void set_left_back_motor_pwm(int pwm)
  {
    pwm = MOTOR_LEFT_BACK_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(LEFT_BACK_MOTOR_IN1, HIGH);
      digitalWrite(LEFT_BACK_MOTOR_IN2, LOW);
      ledcWrite(LEFT_BACK_PWM_CHANNEL, -pwm);
    }
    else
    {
      digitalWrite(LEFT_BACK_MOTOR_IN1, LOW);
      digitalWrite(LEFT_BACK_MOTOR_IN2, HIGH);
      ledcWrite(LEFT_BACK_PWM_CHANNEL, pwm);
    }
  }

  void set_right_back_motor_pwm(int pwm)
  {
    pwm = MOTOR_RIGHT_BACK_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(RIGHT_BACK_MOTOR_IN1, HIGH);
      digitalWrite(RIGHT_BACK_MOTOR_IN2, LOW);
      ledcWrite(RIGHT_BACK_PWM_CHANNEL , -pwm);
    }
    else
    {
      digitalWrite(RIGHT_BACK_MOTOR_IN1, LOW);
      digitalWrite(RIGHT_BACK_MOTOR_IN2, HIGH);
      ledcWrite(RIGHT_BACK_PWM_CHANNEL , pwm);
    }
  }

  void set_left_front_motor_pwm(int pwm)
  {
    pwm = MOTOR_LEFT_FRONT_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(LEFT_FRONT_MOTOR_IN1, HIGH);
      digitalWrite(LEFT_FRONT_MOTOR_IN2, LOW);
      ledcWrite(LEFT_FRONT_PWM_CHANNEL, -pwm);
    }
    else
    {
      digitalWrite(LEFT_FRONT_MOTOR_IN1, LOW);
      digitalWrite(LEFT_FRONT_MOTOR_IN2, HIGH);
      ledcWrite(LEFT_FRONT_PWM_CHANNEL, pwm);
    }
  }

  void set_right_front_motor_pwm(int pwm)
  {
    pwm = MOTOR_RIGHT_FRONT_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(RIGHT_FRONT_MOTOR_IN1, HIGH);
      digitalWrite(RIGHT_FRONT_MOTOR_IN2, LOW);
      ledcWrite(RIGHT_FRONT_PWM_CHANNEL, -pwm);
    }
    else
    {
      digitalWrite(RIGHT_FRONT_MOTOR_IN1, LOW);
      digitalWrite(RIGHT_FRONT_MOTOR_IN2, HIGH);
      ledcWrite(RIGHT_FRONT_PWM_CHANNEL, pwm);
    }
  }

  int calculate_pwm(float desired_percentage)
  {
    int pwm = MAX_MOTOR_PERCENTAGE * PWM_RESOLUTION * desired_percentage / 10000;
    return pwm;
  }

  //mainly for using in the calibration class
  void coast()
  {
    // Set the control pins for each motor to LOW, allowing them to coast
    digitalWrite(LEFT_BACK_MOTOR_IN1, LOW);
    digitalWrite(LEFT_BACK_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_BACK_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_BACK_MOTOR_IN2, LOW);
    digitalWrite(LEFT_FRONT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_FRONT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_FRONT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_FRONT_MOTOR_IN2, LOW);

    // Set PWM to 0 for all motors to ensure no active power is applied
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
};