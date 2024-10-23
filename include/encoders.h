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
    long tempLeftBack;
    long tempLeftFront;
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

    void reset()
    {
        noInterrupts();

        encoderCounterLeftBack = 0;
        encoderCounterRightBack = 0;
        robot_distance = 0;
        robot_angle = 0;

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
        unsigned long currentTime = micros(); // this overflows after 70 mins

        left_delta = 0.0;
        right_delta = 0.0;

        time_change_us_temp = prevTime;
        time_change_us = currentTime - prevTime;
        if (time_change_us == 0) // if the time is negligible between interupts (This doesn't occur now as ticker is used to update instead of the loop, but just in case)
        {
            time_change_us = 1;
        }

        prevTime = currentTime;

        // Make sure values don't change while being read.
        noInterrupts();
        tempLeftBack = encoderCounterLeftBack;
        tempLeftFront = encoderCounterLeftFront;
        tempRightBack = encoderCounterRightBack;
        tempRightFront = encoderCounterRightFront;

        encoderCounterLeftBack = 0;
        encoderCounterLeftFront = 0;
        encoderCounterRightBack = 0;
        encoderCounterRightFront = 0;
        interrupts();

        // doing floating point math outside critical section after making them interrupt-safe
        left_delta = (tempLeftBack + tempLeftFront) * 0.5;
        right_delta = (tempRightBack + tempRightFront) * 0.5;

        float left_change = (float)left_delta * MM_PER_ROTATION / PULSES_PER_ROTATION;
        float right_change = (float)right_delta * MM_PER_ROTATION / PULSES_PER_ROTATION;

        fwd_change = 0.5 * (right_change + left_change); // taking average, distance in millimeters
        robot_distance += fwd_change;
        rot_change = (right_change - left_change) * DEG_PER_MM_DIFFERENCE;
        robot_angle += rot_change;
    }

    inline int loopTime_us()
    {
        int looptime = time_change_us;
        return looptime;
    }

    inline float loopTime_s()
    {
        float time = (float)time_change_us / 1000000.0;
        return time;
    }

    inline float robotDistance()
    {
        float distance;

        noInterrupts();
        distance = robot_distance; // in mm
        interrupts();

        return distance;
    }

    inline float robotAngle()
    {
        float angle;

        noInterrupts();
        angle = robot_angle;
        interrupts();

        return angle;
    }

    inline float robot_speed()
    {
        float speed;

        noInterrupts();
        speed = (fwd_change / time_change_us) * 1000000;
        interrupts();

        return speed;
    }

    inline float robot_omega()
    { /////given in degrees per second!!!!!
        float omega;

        noInterrupts();
        omega = (rot_change / time_change_us) * 1000000;
        interrupts();

        return omega;
    }

    inline float robot_fwd_change()
    {
        float distance;

        noInterrupts();
        distance = fwd_change;
        interrupts();

        return distance;
    }

    inline float robot_rot_change()
    {
        float distance;

        noInterrupts();
        distance = rot_change;
        interrupts();

        return distance;
    }

    inline int leftRPS()
    {
        int rps;

        noInterrupts();
        rps = (left_delta / time_change_us) * 1000000;
        interrupts();

        return rps;
    }

    inline int rightRPS()
    {
        int rps;

        noInterrupts();
        rps = (right_delta / time_change_us) * 1000000;
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

    volatile float robot_distance; // the complete distance travel by robot, this get's incremented using the update function
    volatile float robot_angle;    // same like above

    unsigned long prevTime;
    
    // the change in distance or angle in the last tick.
    float fwd_change;
    float rot_change;

    float left_delta; // this variable holds the number of encoder counts during two update calls
    float right_delta;

    int time_change_us;
    int time_change_us_temp;
};