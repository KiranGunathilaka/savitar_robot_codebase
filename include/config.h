#pragma once
#include "math.h"

#include "Arduino.h"

const int LeftEncoderPin1 = 20;
const int LeftEncoderPin2 = 21;

const int RightEncoderPin1 = 35;
const int RightEncoderPin2 = 36;

const int MM_PER_ROTATION = 32 * PI;
const int PULSES_PER_ROTATION = 1250;
const int DEG_PER_MM_DIFFERENCE = 12;

const int LEFT_MOTOR_PWM = 10;   //left is motor A
const int LEFT_MOTOR_IN1 = 11;
const int LEFT_MOTOR_IN2 = 12;
const int RIGHT_MOTOR_IN2 = 13; //right is motor B
const int RIGHT_MOTOR_IN1 = 14;
const int RIGHT_MOTOR_PWM = 15;

const int PWM_RESOLUTION_BITS = 8;