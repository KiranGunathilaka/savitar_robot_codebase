#pragma once
#include "math.h"
#include "Arduino.h"

//*******************************************************Motors****************************************************************

const int LeftFrontEncoderPin1 = 17;
const int LeftFrontEncoderPin2 = 18;

const int LeftBackEncoderPin1 = 17;
const int LeftBackEncoderPin2 = 18;

const int RightFrontEncoderPin1 = 48;
const int RightFrontEncoderPin2 = 45;

const int RightBackEncoderPin1 = 48;
const int RightBackEncoderPin2 = 45;

//motor controller 1
const int LEFT_FRONT_MOTOR_PWM = 4;
const int LEFT_FRONT_MOTOR_IN1 = 5;
const int LEFT_FRONT_MOTOR_IN2 = 6;
const int LEFT_BACK_MOTOR_IN1 = 5;
const int LEFT_BACK_MOTOR_IN2 = 6;
const int LEFT_BACK_MOTOR_PWM = 4;

//motor controller 2
const int RIGHT_FRONT_MOTOR_PWM = 4;
const int RIGHT_FRONT_MOTOR_IN1 = 5;
const int RIGHT_FRONT_MOTOR_IN2 = 6;
const int RIGHT_BACK_MOTOR_IN1 = 5;
const int RIGHT_BACK_MOTOR_IN2 = 6;
const int RIGHT_BACK_MOTOR_PWM = 4;

const int PWM_RESOLUTION_BITS = 8;

const int PULSES_PER_ROTATION = 1250; //encoder pulse count for rotation, check whether all 4 encoders before using same value for all.
const int WHEEL_DIAMETER = 65;
const float WHEEL_GAP = 108; // distance between the wheels in mm
const float MM_PER_ROTATION = WHEEL_DIAMETER * PI; //  pi*wheel diameter .......d=33mm
const float DEG_PER_MM_DIFFERENCE = 180.0/(PI * WHEEL_GAP);

//**********************************************************Sensors**********************************************************

// I2C Pins for ESP32
#define I2C_SDA 20
#define I2C_SCL 21

#define MULTIPLEXER_ADDR 0x70
#define TCS34725_ADDR 0x29

const int ToF_XSHUT_Right = 14;
const int ToF_XSHUT_Left = 13;
const int ToF_XSHUT_Center_Top = 12;
const int ToF_XSHUT_Center_Bottom = 15;

const int TOF_LEFT_ADD = 0x30;
const int TOF_RIGHT_ADD = 0x31;
const int TOF_CENTER_TOP_ADD = 0x32;
const int TOF_CENTER_BOTTOM_ADD = 0x34;


