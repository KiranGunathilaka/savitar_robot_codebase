#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include <math.h>

class Encoders;
extern Encoders encoders;

class Encoders
{
public:
    long tempLeftBack;  // these values will be accessed by printer class for serial print
    long tempLeftFront; // directly taking values from variables that are getting written by interrupts may crash the program
    long tempRightBack;
    long tempRightFront;

    void begin()
    {
        pinMode(LeftBackEncoderPin1, INPUT_PULLUP); // can speed up gpio readtime by using gpio config(driver/gpio.h)
        pinMode(LeftBackEncoderPin2, INPUT_PULLUP);

        pinMode(RightBackEncoderPin1, INPUT_PULLUP);
        pinMode(RightBackEncoderPin2, INPUT_PULLUP);

        pinMode(LeftFrontEncoderPin1, INPUT_PULLUP);
        pinMode(LeftFrontEncoderPin2, INPUT_PULLUP);

        pinMode(RightFrontEncoderPin1, INPUT_PULLUP);
        pinMode(RightFrontEncoderPin2, INPUT_PULLUP);

        attachInterrupt(digitalPinToInterrupt(LeftBackEncoderPin1), updateLeftBackEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(LeftBackEncoderPin2), updateLeftBackEncoderISR, CHANGE);

        attachInterrupt(digitalPinToInterrupt(RightBackEncoderPin1), updateRightBackEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(RightBackEncoderPin2), updateRightBackEncoderISR, CHANGE);

        attachInterrupt(digitalPinToInterrupt(LeftFrontEncoderPin1), updateLeftFrontEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(LeftFrontEncoderPin2), updateLeftFrontEncoderISR, CHANGE);

        attachInterrupt(digitalPinToInterrupt(RightFrontEncoderPin1), updateRightFrontEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(RightFrontEncoderPin2), updateRightFrontEncoderISR, CHANGE);

        reset();
    }

    void setLoopTime(float time){
        loopTime = time;
    };

    float getLoopTime(){
        return loopTime;
    };


    void reset()
    {
        noInterrupts();

        encoderCounterLeftBack = 0;
        encoderCounterRightBack = 0;
        encoderCounterLeftFront = 0;
        encoderCounterRightFront = 0;

        robot_distance_back = 0;
        robot_angle_back = 0;
        robot_distance_front = 0;
        robot_angle_front = 0;

        interrupts();
    }

    static void updateLeftBackEncoderISR()
    {
        encoders.updateLeftBackEncoder();
    }

    static void updateRightBackEncoderISR()
    {
        encoders.updateRightBackEncoder();
    }

    static void updateLeftFrontEncoderISR()
    {
        encoders.updateLeftFrontEncoder();
    }

    static void updateRightFrontEncoderISR()
    {
        encoders.updateRightFrontEncoder();
    }

    void updateLeftBackEncoder()
    {
        int MSB = digitalRead(LeftBackEncoderPin1); // Most Significant Bit (A)
        int LSB = digitalRead(LeftBackEncoderPin2); // Least Significant Bit (B)

        int encoded = (MSB << 1) | LSB; // Create a 2-bit value from A and B

        int sum = (lastEncodedLeftBack << 2) | encoded; // Combine current and previous states

        // Update position based on the transition
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {
            encoderCounterLeftBack--;
        }
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {
            encoderCounterLeftBack++;
        }

        lastEncodedLeftBack = encoded; // Save the current state
    }

    void updateRightBackEncoder()
    {
        int MSB = digitalRead(RightBackEncoderPin1); // Most Significant Bit (A)
        int LSB = digitalRead(RightBackEncoderPin2); // Least Significant Bit (B)

        int encoded = (MSB << 1) | LSB; // Create a 2-bit value from A and B

        int sum = (lastEncodedRightBack << 2) | encoded; // Combine current and previous states

        // Update position based on the transition
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {
            encoderCounterRightBack--;
        }
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {
            encoderCounterRightBack++;
        }

        lastEncodedRightBack = encoded; // Save the current state
    }

    void updateLeftFrontEncoder()
    {
        int MSB = digitalRead(LeftFrontEncoderPin1); // Most Significant Bit (A)
        int LSB = digitalRead(LeftFrontEncoderPin2); // Least Significant Bit (B)

        int encoded = (MSB << 1) | LSB; // Create a 2-bit value from A and B

        int sum = (lastEncodedLeftFront << 2) | encoded; // Combine current and previous states

        // Update position based on the transition
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {
            encoderCounterLeftFront--;
        }
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {
            encoderCounterLeftFront++;
        }

        lastEncodedLeftFront = encoded; // Save the current state
    }

    void updateRightFrontEncoder()
    {
        int MSB = digitalRead(RightFrontEncoderPin1); // Most Significant Bit (A)
        int LSB = digitalRead(RightFrontEncoderPin2); // Least Significant Bit (B)

        int encoded = (MSB << 1) | LSB; // Create a 2-bit value from A and B

        int sum = (lastEncodedRightFront << 2) | encoded; // Combine current and previous states

        // Update position based on the transition
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {
            encoderCounterRightFront--;
        }
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {
            encoderCounterRightFront++;
        }

        lastEncodedRightFront = encoded; // Save the current state
    }

    void update()
    {
        left_delta_back = 0;
        right_delta_back = 0;
        left_delta_front = 0;
        right_delta_front = 0;

        // Make sure values don't change while being read.
        noInterrupts();
        left_delta_back = encoderCounterLeftBack;
        left_delta_front = encoderCounterLeftFront;
        right_delta_back = encoderCounterRightBack;
        right_delta_front = encoderCounterRightFront;

        encoderCounterLeftBack = 0;
        encoderCounterLeftFront = 0;
        encoderCounterRightBack = 0;
        encoderCounterRightFront = 0;
        interrupts();

        // for serial print
        tempLeftBack = left_delta_back;
        tempLeftFront = left_delta_front;
        tempRightBack = right_delta_back;
        tempRightFront = right_delta_front;

        float left_change_back = (float)left_delta_back * MM_PER_ROTATION_BACK / PULSES_PER_ROTATION;
        float right_change_back = (float)right_delta_back * MM_PER_ROTATION_BACK / PULSES_PER_ROTATION;
        float left_change_front = (float)left_delta_front * MM_PER_ROTATION_FRONT / PULSES_PER_ROTATION;
        float right_change_front = (float)right_delta_front * MM_PER_ROTATION_FRONT / PULSES_PER_ROTATION;

        fwd_change_back = 0.5 * (right_change_back + left_change_back); // taking average, distance in millimeters
        robot_distance_back += fwd_change_back;
        rot_change_back = (right_change_back - left_change_back) * DEG_PER_MM_DIFFERENCE;
        robot_angle_back += rot_change_back;

        fwd_change_front = 0.5 * (right_change_front + left_change_front); // taking average, distance in millimeters
        robot_distance_front += fwd_change_front;
        rot_change_front = (right_change_front - left_change_front) * DEG_PER_MM_DIFFERENCE;
        robot_angle_front += rot_change_front;
    }


    // back pair values
    inline float robotDistanceBack()
    {
        float distance;

        noInterrupts();
        distance = robot_distance_back; // in mm
        interrupts();

        return distance;
    }

    inline float robotAngleBack()
    {
        float angle;

        noInterrupts();
        angle = robot_angle_back;
        interrupts();

        return angle;
    }

    inline float robot_speed_back()
    {
        float speed;

        noInterrupts();
        speed = ((float) fwd_change_back / loopTime);
        interrupts();

        return speed;
    }

    inline float robot_omega_back()
    { /////given in degrees per second!!!!!
        float omega;

        noInterrupts();
        omega = ((float) rot_change_back / loopTime);
        interrupts();

        return omega;
    }

    inline float robot_fwd_change_back()
    {
        float distance;

        noInterrupts();
        distance = fwd_change_back;
        interrupts();

        return distance;
    }

    inline float robot_rot_change_back()
    {
        float distance;

        noInterrupts();
        distance = rot_change_back;
        interrupts();

        return distance;
    }

    inline int leftRPS_back()
    {
        int rps;

        noInterrupts();
        rps = ((float) left_delta_back / loopTime);
        interrupts();

        return rps;
    }

    inline int rightRPS_back()
    {
        int rps;

        noInterrupts();
        rps = ((float) right_delta_back / loopTime);
        interrupts();

        return rps;
    }

    // front pair values
    inline float robotDistanceFront()
    {
        float distance;

        noInterrupts();
        distance = robot_distance_front; // in mm
        interrupts();

        return distance;
    }

    inline float robotAngleFront()
    {
        float angle;

        noInterrupts();
        angle = robot_angle_front;
        interrupts();

        return angle;
    }

    inline float robot_speed_front()
    {
        float speed;

        noInterrupts();
        speed = ((float) fwd_change_front / loopTime);
        interrupts();

        return speed;
    }

    inline float robot_omega_front()
    { /////given in degrees per second!!!!!
        float omega;

        noInterrupts();
        omega = ((float) rot_change_front / loopTime);
        interrupts();

        return omega;
    }

    inline float robot_fwd_change_front()
    {
        float distance;

        noInterrupts();
        distance = fwd_change_front;
        interrupts();

        return distance;
    }

    inline float robot_rot_change_front()
    {
        float distance;

        noInterrupts();
        distance = rot_change_front;
        interrupts();

        return distance;
    }

    inline int leftRPS_front()
    {
        int rps;

        noInterrupts();
        rps = ((float) left_delta_front / loopTime);
        interrupts();

        return rps;
    }

    inline int rightRPS_front()
    {
        int rps;

        noInterrupts();
        rps = ((float) right_delta_front / loopTime);
        interrupts();

        return rps;
    }

private:
    volatile long encoderCounterLeftBack; // Encoder roatation count, this gets reset every time we call update
    volatile long lastEncodedLeftBack;    // Last encoded value

    volatile long encoderCounterRightBack;
    volatile long lastEncodedRightBack;

    volatile long encoderCounterRightFront;
    volatile long lastEncodedRightFront;

    volatile long encoderCounterLeftFront;
    volatile long lastEncodedLeftFront;

    volatile float robot_distance_back; // the complete distance travel by robot, this get's incremented using the update function
    volatile float robot_angle_back;    // same like above
    volatile float robot_distance_front;
    volatile float robot_angle_front;

    // the change in distance or angle in the last tick.
    float fwd_change_back;
    float rot_change_back;
    float fwd_change_front;
    float rot_change_front;

    int left_delta_front; // this variable holds the number of encoder counts during two update calls
    int right_delta_front;
    int left_delta_back; // this variable holds the number of encoder counts during two update calls
    int right_delta_back;

    float loopTime;
};