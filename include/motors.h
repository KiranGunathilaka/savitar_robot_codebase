#pragma once

#include <Arduino.h>
#include "config.h"
#include "encoders.h"

class Motors;
extern Motors motors;

class Motors
{
private:
  float m_previous_fwd_error;
  float m_previous_rot_error;
  float m_fwd_error;
  float m_rot_error;

  float m_velocity;
  float m_omega;

  bool m_feedforward_enabled = true;
  bool m_controller_output_enabled;

public:
  // remove this after testing
  float fwdKp = FWD_KP;
  float fwdKd = FWD_KD;
  float rotKp = ROT_KP;
  float rotKd = ROT_KD;

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
    ledcSetup(0, 5000, PWM_RESOLUTION_BITS); // check for different pwm frequencies
    ledcAttachPin(LEFT_BACK_MOTOR_PWM, 0);
    ledcSetup(1, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_BACK_MOTOR_PWM, 1);
    ledcSetup(2, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(LEFT_FRONT_MOTOR_PWM, 2);
    ledcSetup(3, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_FRONT_MOTOR_PWM, 3);
  }

  void reset_controllers()
  {
    m_fwd_error = 0;
    m_rot_error = 0;
    m_previous_fwd_error = 0;
    m_previous_rot_error = 0;
  }

  void enable_controllers()
  {
    m_controller_output_enabled = true;
  }

  void disable_controllers()
  {
    m_controller_output_enabled = false;
  }

  void stop()
  {
    set_left_back_motor_percentage(0);
    set_right_back_motor_percentage(0);
    set_left_front_motor_percentage(0);
    set_right_front_motor_percentage(0);
  }

  float position_controller()
  {
    float increment = m_velocity * encoders.loopTime_s();
    m_fwd_error += increment - encoders.robot_fwd_change();
    float diff = m_fwd_error - m_previous_fwd_error;
    m_previous_fwd_error = m_fwd_error;

    // change them to config kp kd later
    float output = fwdKp * m_fwd_error + fwdKd * diff;
    return output;
  }

  float angle_controller(float steering_adjustment)
  {
    float increment = m_omega * encoders.loopTime_s();
    m_rot_error += increment - encoders.robot_rot_change();
    float diff = m_rot_error - m_previous_rot_error;
    m_previous_rot_error = m_rot_error;

    m_rot_error -= steering_adjustment;

    // changethis kp kd to config kp kd later
    float output = rotKp * m_rot_error + rotKd * diff;
    return output;
  }

  //feed forward functions gives the percentage required to acheive a given velocity
  float left_feed_forward_percentage(float left_feed_velocity)
  {
    int l_rps = (left_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION;

    float l_feed_percentage = 289.6 - sqrt(-6.329 * l_rps + 85635.1);
    return l_feed_percentage;
  }

  float right_feed_forward_percentage(float left_feed_velocity)
  {
    int r_rps = (left_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION;

    float r_feed_percentage = 289.6 - sqrt(-6.329 * r_rps + 85635.1);
    return r_feed_percentage;
  }



  void update(float velocity, float omega, float steering)
  {
    m_velocity = velocity;
    m_omega = omega;

    float pos_output = position_controller();
    float rot_output = angle_controller(steering);

    float left_back_output = 0;
    float right_back_output = 0;
    float left_front_output = 0;
    float right_front_output = 0;

    left_back_output = pos_output - rot_output;
    right_back_output = pos_output + rot_output;

    float tangent_speed = m_omega * ROBOT_RADIUS * RADIANS_PER_DEGREE;
    float left_speed = m_velocity - tangent_speed;
    float right_speed = m_velocity + tangent_speed;
    float left_ff = left_feed_forward_percentage(left_speed);
    float right_ff = right_feed_forward_percentage(right_speed);
    if (m_feedforward_enabled)
    {
      left_back_output += left_ff;
      right_back_output += right_ff;
    }
    if (m_controller_output_enabled)
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

  //limits the given percentage to minimum bias and maximum value (needs to overcome motor inequalities at lower pwm, 
  //friction and give headroom for PID values to adjust in the higher edge)
  float constrainPercentage(float percentage)
  {
    percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
    if (percentage > MIN_MOTOR_PERCENTAGE)
    {
      percentage = map(percentage, MIN_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE, MIN_MOTOR_BIAS, MAX_MOTOR_PERCENTAGE);
    }
    else if (percentage < -MIN_MOTOR_PERCENTAGE)
    {
      percentage = map(percentage, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_PERCENTAGE, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_BIAS);
    }
    else if (-MIN_MOTOR_PERCENTAGE <= percentage <= MIN_MOTOR_PERCENTAGE)
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
      ledcWrite(0, -pwm);
    }
    else
    {
      digitalWrite(LEFT_BACK_MOTOR_IN1, LOW);
      digitalWrite(LEFT_BACK_MOTOR_IN2, HIGH);
      ledcWrite(0, pwm);
    }
  }

  void set_right_back_motor_pwm(int pwm)
  {
    pwm = MOTOR_RIGHT_BACK_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(RIGHT_BACK_MOTOR_IN1, HIGH);
      digitalWrite(RIGHT_BACK_MOTOR_IN2, LOW);
      ledcWrite(1, -pwm);
    }
    else
    {
      digitalWrite(RIGHT_BACK_MOTOR_IN1, LOW);
      digitalWrite(RIGHT_BACK_MOTOR_IN2, HIGH);
      ledcWrite(1, pwm);
    }
  }

  void set_left_front_motor_pwm(int pwm)
  {
    pwm = MOTOR_LEFT_FRONT_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(LEFT_FRONT_MOTOR_IN1, HIGH);
      digitalWrite(LEFT_FRONT_MOTOR_IN2, LOW);
      ledcWrite(2, -pwm);
    }
    else
    {
      digitalWrite(LEFT_FRONT_MOTOR_IN1, LOW);
      digitalWrite(LEFT_FRONT_MOTOR_IN2, HIGH);
      ledcWrite(2, pwm);
    }
  }

  void set_right_front_motor_pwm(int pwm)
  {
    pwm = MOTOR_RIGHT_FRONT_POLARITY * pwm;
    if (pwm < 0)
    {
      digitalWrite(RIGHT_FRONT_MOTOR_IN1, HIGH);
      digitalWrite(RIGHT_FRONT_MOTOR_IN2, LOW);
      ledcWrite(3, -pwm);
    }
    else
    {
      digitalWrite(RIGHT_FRONT_MOTOR_IN1, LOW);
      digitalWrite(RIGHT_FRONT_MOTOR_IN2, HIGH);
      ledcWrite(3, pwm);
    }
  }

  int calculate_pwm(float desired_percentage)
  {
    int pwm = MAX_MOTOR_PERCENTAGE * PWM_RESOLUTION * desired_percentage / 10000;
    return pwm;
  }
};