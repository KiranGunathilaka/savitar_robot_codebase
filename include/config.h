#pragma once
#include "math.h"
#include "Arduino.h"

//*******************************************************Motors****************************************************************

const int LeftFrontEncoderPin1 = 18;
const int LeftFrontEncoderPin2 = 8;

const int LeftBackEncoderPin1 = 3;
const int LeftBackEncoderPin2 = 46;

const int RightFrontEncoderPin1 = 48;
const int RightFrontEncoderPin2 = 45;

const int RightBackEncoderPin1 = 35;
const int RightBackEncoderPin2 = 36;

// motor controller 1
const int LEFT_FRONT_MOTOR_PWM = 17;
const int LEFT_FRONT_MOTOR_IN1 = 15;
const int LEFT_FRONT_MOTOR_IN2 = 16;
const int LEFT_BACK_MOTOR_IN1 = 5;
const int LEFT_BACK_MOTOR_IN2 = 6;
const int LEFT_BACK_MOTOR_PWM = 4;

// motor controller 2
const int RIGHT_BACK_MOTOR_PWM = 37;
const int RIGHT_BACK_MOTOR_IN1 = 39;
const int RIGHT_BACK_MOTOR_IN2 = 40;
const int RIGHT_FRONT_MOTOR_IN1 = 42;
const int RIGHT_FRONT_MOTOR_IN2 = 2;
const int RIGHT_FRONT_MOTOR_PWM = 1;

const int PULSES_PER_ROTATION = 1495; // encoder pulse count for rotation, all have same values for 20 rotations (1494, 1495, 1496, 1497 averages)
const int WHEEL_DIAMETER_BACK = 65;
const int WHEEL_DIAMETER_FRONT = 63;
const float WHEEL_GAP = 189.5; // distance between the wheels in mm
const float MM_PER_ROTATION_BACK = WHEEL_DIAMETER_BACK * PI;
const float MM_PER_ROTATION_FRONT = WHEEL_DIAMETER_FRONT * PI; 
const float DEG_PER_MM_DIFFERENCE = 180.0 / (PI * WHEEL_GAP); //degrees of rotation per unit difference between the left and right wheel travel distances

const float FWD_KP_BACK = 1.0;
const float FWD_KD_BACK = 1.0;
const float ROT_KP_BACK = 4.0;
const float ROT_KD_BACK = 0.8;

const float FWD_KP_FRONT = 1.0;
const float FWD_KD_FRONT = 1.0;
const float ROT_KP_FRONT = 4.0;
const float ROT_KD_FRONT = 0.8;

const int ROBOT_RADIUS = 115; //measure it to the absolute 1mm accuracy
const float RADIANS_PER_DEGREE = PI / 180; 

const int PWM_RESOLUTION_BITS = 8;
const int PWM_RESOLUTION  = 256;


const int MIN_MOTOR_BIAS = 13;
const int MAX_MOTOR_REACH = 90; //this is the value the final motor percentages reaches upto
const float MAX_MOTOR_PERCENTAGE = 90; //smaller head room is given for PID
const float MIN_MOTOR_PERCENTAGE = 5; // when the given percentage is below this value, percentage is set to zero to damp oscillations
//find this value for the all 4 motors and set the maximum from them here

//change motor directions to make the motors spin forward when both motors are forward commanded (for use of incorrect wiring)
#define MOTOR_LEFT_BACK_POLARITY (1)
#define MOTOR_RIGHT_BACK_POLARITY (-1)
#define MOTOR_LEFT_FRONT_POLARITY (-1)
#define MOTOR_RIGHT_FRONT_POLARITY (1)

//**********************************************************Sensors**********************************************************

// I2C Pins for ESP32
#define I2C_SDA_0 19
#define I2C_SCL_0 20

#define I2C_SDA_1 21
#define I2C_SCL_1 47

#define MULTIPLEXER_ADDR 0x70
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

const float STEERING_KP = 90.0;             //working values speed - 70 Kp - 90 Kd - 25
const float STEERING_KD =25.0;
const float SENSOR_WEIGHTS[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};

//*************************************************************CALIBRATION******************************************

static const int SAMPLES_PER_CALIBRATION = 100;
static const int CALIBRATION_DELAY_MS = 0;
//*************************************************************SWITCHES******************************************

const int SWITCH_PIN = 7;

//*************************************************************SYSTICK*******************************************

#define FAST_TICKER 0.025
#define SLOW_TICKER 0.060

//**************************************************************ROBOT*********************************************

const int OMEGA = 540;
const int ALPHA = 3600;


//***********************************************************REPORTING*********************************************

const uint8_t broadcastAddress[] = { 0xCC, 0x50, 0xE3, 0X41, 0x1C, 0xD6 };

//**********************************************************SERVOS*************************************************

const int SERVO_GRIPPER_PIN = 7;
const int SERVO_LIFT_PIN = 0;