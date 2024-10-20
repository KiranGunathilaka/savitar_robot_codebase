#pragma once

#include <Arduino.h>
#include "config.h"
#include "encoders.h"

class Motors;
extern Motors motors;

class Motors
{
private:
  float m_left_motor_percentage;
  float m_right_motor_percentage;

  float m_previous_fwd_error;
  float m_previous_rot_error;
  float m_fwd_error;
  float m_rot_error;

  float m_velocity;
  float m_omega;

  bool m_feedforward_enabled = true;
  bool m_controller_output_enabled;
  unsigned long i = 0;

public:
  // remove after testing

  // float fwdKp = FWD_KP_SMALL;
  // float fwdKd = FWD_KD_SMALL;
  // float rotKp = ROT_KP_90;
  // float rotKd = ROT_KD_90;

  // // remove

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
    ledcSetup(0, 10000, PWM_RESOLUTION_BITS); // check for different pwm frequencies
    ledcAttachPin(LEFT_BACK_MOTOR_PWM, 0);
    ledcSetup(1, 10000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_BACK_MOTOR_PWM, 1);
    ledcSetup(2, 10000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_BACK_MOTOR_PWM, 2);
    ledcSetup(3, 10000, PWM_RESOLUTION_BITS);
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

  // void stop()
  // {
  //   set_left_motor_percentage(0);
  //   set_right_motor_percentage(0);
  // }

  // float position_controller()
  // {
  //   float increment = m_velocity * encoders.loopTime_s();
  //   m_fwd_error += increment - encoders.robot_fwd_change();
  //   float diff = m_fwd_error - m_previous_fwd_error;
  //   m_previous_fwd_error = m_fwd_error;

  //   // change them to config kp kd later
  //   float output = fwdKp * m_fwd_error + fwdKd * diff;
  //   return output;
  // }

  // float angle_controller(float steering_adjustment)
  // {
  //   float increment = m_omega * encoders.loopTime_s();
  //   m_rot_error += increment - encoders.robot_rot_change();
  //   float diff = m_rot_error - m_previous_rot_error;
  //   m_previous_rot_error = m_rot_error;
  //   m_rot_error -= steering_adjustment;

  //   // Serial.print("Steering  :");
  //   // Serial.print(steering_adjustment);

  //   // changethis kp kd to config kp kd later
  //   float output = rotKp * m_rot_error + rotKd * diff;
  //   return output;
  // }

  // void update(float velocity, float omega, float steering)
  // {
  //   m_velocity = velocity;
  //   m_omega = omega;

  //   float pos_output = position_controller();
  //   float rot_output = angle_controller(steering);
  //   float left_output = 0;
  //   float right_output = 0;

  //   left_output = pos_output - rot_output;
  //   right_output = pos_output + rot_output;

  //   float tangent_speed = m_omega * MOUSE_RADIUS * RADIANS_PER_DEGREE;
  //   float left_speed = m_velocity - tangent_speed;
  //   float right_speed = m_velocity + tangent_speed;
  //   float left_ff = left_feed_forward_percentage(left_speed);
  //   float right_ff = right_feed_forward_percentage(right_speed);
  //   if (m_feedforward_enabled)
  //   {
  //     left_output += left_ff;
  //     right_output += right_ff;
  //   }
  //   if (m_controller_output_enabled)
  //   {
  //     set_left_motor_percentage(left_output);
  //     set_right_motor_percentage(right_output);

  //     // Serial.print("  left  : ");
  //     // Serial.print(left_output);

  //     // Serial.print("   right  : ");
  //     // Serial.println(right_output);
  //   }
  // }

  float left_feed_forward_percentage(float left_feed_velocity)
  {
    ///////give the percentage required to acheive a given velocity--- |v|<500
    int l_rps = (left_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION;

    float l_feed_percentage = 289.6 - sqrt(-6.329 * l_rps + 85635.1);
    return l_feed_percentage;
  }

  float right_feed_forward_percentage(float left_feed_velocity)
  {
    ///////give the percentage required to acheive a given velocity--- |v|<500
    int r_rps = (left_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION;

    float r_feed_percentage = 289.6 - sqrt(-6.329 * r_rps + 85635.1);
    return r_feed_percentage;
  }

  // void set_left_motor_percentage(float percentage)
  // {

  //   if (percentage >= 50)
  //   {
  //     percentage += (0.059 * percentage * percentage - 1.477 * percentage - 8.47) / (-0.434 * percentage + 93); ////////percentage +(error function/left motor function gradient)
  //     percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
  //     percentage = map(percentage, 0, MAX_MOTOR_PERCENTAGE, MIN_MOTOR_BIAS, MAX_MOTOR_PERCENTAGE);
  //   }
  //   else if (percentage > MIN_MOTOR_PERCENTAGE)
  //   {
  //     percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
  //     percentage = map(percentage, MIN_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE, MIN_MOTOR_BIAS, MAX_MOTOR_PERCENTAGE);
  //   }
  //   else if (percentage <= -50)
  //   {
  //     percentage -= (0.059 * percentage * percentage + 1.477 * percentage - 8.47) / (0.434 * percentage + 93); ////same as above. magnitude of percentage is taken and so a (-)percentage is taken in error correction
  //     percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
  //     percentage = map(percentage, -MAX_MOTOR_PERCENTAGE, 0, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_BIAS);
  //   }
  //   else if (percentage < -MIN_MOTOR_PERCENTAGE)
  //   {
  //     percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
  //     percentage = map(percentage, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_PERCENTAGE, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_BIAS);
  //   }
  //   else if (-MIN_MOTOR_PERCENTAGE <= percentage <= MIN_MOTOR_PERCENTAGE)
  //   {
  //     percentage = 0;
  //   }

  //   int left_pwm = calculate_pwm(percentage);
  //   // Serial.print("   Left pwm percentage: ");
  //   // Serial.print(percentage);

  //   set_left_motor_pwm(left_pwm);
  // }

  // void set_left_motor_pwm(int pwm)
  // {
  //   pwm = MOTOR_LEFT_POLARITY * pwm;
  //   if (pwm < 0)
  //   {
  //     digitalWrite(LEFT_MOTOR_IN1, HIGH);
  //     digitalWrite(LEFT_MOTOR_IN2, LOW);
  //     ledcWrite(0, -pwm + M_BALNCE_PWM);
  //   }
  //   else
  //   {
  //     digitalWrite(LEFT_MOTOR_IN1, LOW);
  //     digitalWrite(LEFT_MOTOR_IN2, HIGH);
  //     ledcWrite(0, pwm + M_BALNCE_PWM);
  //   }
  // }

  // int calculate_pwm(float desired_percentage)
  // {
  //   int pwm = MAX_MOTOR_PERCENTAGE * PWM_RESOLUTION * desired_percentage / 10000;
  //   return pwm;
  // }

  // void set_right_motor_percentage(float percentage)
  // {
  //   percentage = constrain(percentage, -MAX_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
  //   if (percentage > MIN_MOTOR_PERCENTAGE)
  //   {
  //     percentage = map(percentage, MIN_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE, MIN_MOTOR_BIAS, MAX_MOTOR_PERCENTAGE);
  //   }
  //   else if (percentage < -MIN_MOTOR_PERCENTAGE)
  //   {
  //     percentage = map(percentage, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_PERCENTAGE, -MAX_MOTOR_PERCENTAGE, -MIN_MOTOR_BIAS);
  //   }
  //   else if (-MIN_MOTOR_PERCENTAGE <= percentage <= MIN_MOTOR_PERCENTAGE)
  //   {
  //     percentage = 0;
  //   }

  //   m_right_motor_percentage = percentage;
  //   int right_pwm = calculate_pwm(percentage);

  //   // Serial.print("   right pwm percentage: ");
  //   // Serial.println(percentage);

  //   set_right_motor_pwm(right_pwm);
  // }

  // void set_right_motor_pwm(int pwm)
  // {
  //   pwm = MOTOR_RIGHT_POLARITY * pwm;
  //   if (pwm < 0)
  //   {
  //     digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  //     digitalWrite(RIGHT_MOTOR_IN2, LOW);
  //     ledcWrite(1, -pwm - M_BALNCE_PWM);
  //   }
  //   else
  //   {
  //     digitalWrite(RIGHT_MOTOR_IN1, LOW);
  //     digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  //     ledcWrite(1, pwm - M_BALNCE_PWM);
  //   }
  // }
};