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

// motor controller 1
const int LEFT_FRONT_MOTOR_PWM = 4;
const int LEFT_FRONT_MOTOR_IN1 = 5;
const int LEFT_FRONT_MOTOR_IN2 = 6;
const int LEFT_BACK_MOTOR_IN1 = 5;
const int LEFT_BACK_MOTOR_IN2 = 6;
const int LEFT_BACK_MOTOR_PWM = 4;

// motor controller 2
const int RIGHT_FRONT_MOTOR_PWM = 4;
const int RIGHT_FRONT_MOTOR_IN1 = 5;
const int RIGHT_FRONT_MOTOR_IN2 = 6;
const int RIGHT_BACK_MOTOR_IN1 = 5;
const int RIGHT_BACK_MOTOR_IN2 = 6;
const int RIGHT_BACK_MOTOR_PWM = 4;

const int PWM_RESOLUTION_BITS = 8;

const int PULSES_PER_ROTATION = 1250; // encoder pulse count for rotation, check whether all 4 encoders before using same value for all.
const int WHEEL_DIAMETER = 65;
const float WHEEL_GAP = 108;                       // distance between the wheels in mm
const float MM_PER_ROTATION = WHEEL_DIAMETER * PI; //  pi*wheel diameter .......d=33mm
const float DEG_PER_MM_DIFFERENCE = 180.0 / (PI * WHEEL_GAP);

//**********************************************************Sensors**********************************************************

// I2C Pins for ESP32
#define I2C_SDA_0 19
#define I2C_SCL_0 20

#define I2C_SDA_1 21
#define I2C_SCL_1 47

#define MULTIPLEXER_ADDR 0x73
#define TCS34725_ADDR 0x29

#define TCS34725_FAST_INTEGRATION_TIME 0xFF // 1 cycles,  4.8 seconds for one sensor , delay 25ms
#define TCS34725_SLOW_INTEGRATION_TIME 0xFC // 4 cycles,  9.6 seconds for one sensor , delay 60ms

const int ToF_XSHUT_Right = 9;
const int ToF_XSHUT_Left = 10;
const int ToF_XSHUT_Front = 11;
const int ToF_XSHUT_Center_Top = 12;
const int ToF_XSHUT_Center_Bottom = 13;

const int TOF_LEFT_ADD = 0x30;
const int TOF_RIGHT_ADD = 0x31;
const int TOF_FRONT_ADD = 0x32;
const int TOF_CENTER_TOP_ADD = 0x33;
const int TOF_CENTER_BOTTOM_ADD = 0x34;

const float steering_kp = 1.0;
const float steering_kd = 1.0;
const float steering_ki = 1.0;
const float SENSOR_WEIGHTS[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};

//*************************************************************CALIBRATION******************************************

static const int SAMPLES_PER_CALIBRATION = 100;
static const int CALIBRATION_DELAY_MS = 0;
//*************************************************************SWITCHES******************************************

const int SWITCH_PIN = 7;

//*************************************************************SYSTICK*******************************************

#define FAST_TICKER 0.025
#define SLOW_TICKER 0.060
