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
const int WHEEL_DIAMETER_BACK = 63;
const int WHEEL_DIAMETER_FRONT = 63;
const float WHEEL_GAP = 189.5; // distance between the wheels in mm
const float MM_PER_ROTATION_BACK = WHEEL_DIAMETER_BACK * PI;
const float MM_PER_ROTATION_FRONT = WHEEL_DIAMETER_FRONT * PI; 
const float DEG_PER_MM_DIFFERENCE = 180.0 / (PI * WHEEL_GAP); //degrees of rotation per unit difference between the left and right wheel travel distances

const float FWD_KP_BACK = 1.0;
const float FWD_KD_BACK = 0.5;
const float ROT_KP_BACK = 9.18;
const float ROT_KD_BACK = 0.229;

const float FWD_KP_FRONT = 1.0;
const float FWD_KD_FRONT = 0.5;
const float ROT_KP_FRONT = 9.18;
const float ROT_KD_FRONT = 0.229;

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

const int LEFT_BACK_PWM_CHANNEL = 4; 
const int RIGHT_BACK_PWM_CHANNEL = 5; 
const int LEFT_FRONT_PWM_CHANNEL = 2;
const int RIGHT_FRONT_PWM_CHANNEL = 3;

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

const int ToF_XSHUT_Right = 11;
const int ToF_XSHUT_Left = 13;
const int ToF_XSHUT_Front = 12;
const int ToF_XSHUT_Center_Top = 10;
const int ToF_XSHUT_Center_Bottom = 9;

const int TOF_LEFT_ADD = 0x30;
const int TOF_RIGHT_ADD = 0x31;
const int TOF_FRONT_ADD = 0x32;
const int TOF_CENTER_TOP_ADD = 0x33;
const int TOF_CENTER_BOTTOM_ADD = 0x34;

const float STEERING_KP = 14.0;             //working values speed - 70 Kp - 90 Kd - 25
const float STEERING_KD = 5.5;
const float SENSOR_WEIGHTS[5] = {2.0, 1.0, 0.0, -1.0, -2.0};

const float STEERING_KD_TOF = 10.0;
const float STEERING_KP_TOF = 1.0;

const float LEFT_WALL_ThRESHOLD = 100;
//*************************************************************CALIBRATION******************************************

static const int SAMPLES_PER_CALIBRATION = 100;
static const int CALIBRATION_DELAY_MS = 0;
//*************************************************************SWITCHES******************************************

const int SWITCH_PIN = 7;

//*************************************************************SYSTICK*******************************************

#define FAST_TICKER 0.025
#define SLOW_TICKER 0.060

//**************************************************************ROBOT*********************************************

const int OMEGA = 360;
const int ALPHA = 3600;

const int BARCODE_THRESHOLD_DISTANCE = 43;

const int ROBOT_LENGTH = 180;

const int INITIAL_BOX_DISTANCE = 310;
//***********************************************************REPORTING*********************************************

const uint8_t broadcastAddress[] = { 0xCC, 0xDB, 0xA7, 0X33, 0x1D, 0x00 };

//**********************************************************SERVOS*************************************************

const int SERVO_GRIPPER_PIN = 38;
const int SERVO_LIFT_PIN = 14;

const int OPEN_ARM_ANGLE = 0;
const int CLOSE_ARM_ANGLE = 140;

const float MIN_HEIGHT  = 50;
const float MAX_HEIGHT  = 120; //in mm

const int TOF_OFFSET = 15;

const int HEIGHT_DETECTING_THRESHOLD = 120;
//***********************************************************ROBOT*************************************************

const int MOVE_AFTER_DETECT = 70;
const int RUN_SPEED = 100;
const int ACCELERATION = 1000;

const int READ_SPEED = 100;

const int DIST_TO_BARRIER_FROM_LINE1 = 440;
const int DIST_TO_BARRIER_FROM_LINE2 = 140;

const int LINE_WIDTH = 30;

const int WALL_FOLLOWING_DIATANCE = 80;
const int BOX_BRANCH_DISTANCE = 400;

const int REVERSE_DISTANCE = 50;

const int HEIGHT_DETECTING_DISTANCE = 60;
const int PICKING_UP_DISTANCE = 30;

const int CORRECTION_OFFSET_90 = 8;
const int CORRECTION_OFFSET_180 = 17;
//***********************************************************UTILS**************************************************
const int LED_PIN =  0;      

const int EM_PIN = 41;