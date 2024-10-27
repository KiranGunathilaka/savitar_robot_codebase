#pragma once

#include <Arduino.h>
#include "sensors.h"
#include "motors.h"
#include "encoders.h"
#include "config.h"

class Calibaration;
extern Calibaration calibaration;

class Calibaration
{
private:
    // Struct to hold RGB and Lux readings
    struct ColorReading
    {
        float r_ratio;
        float g_ratio;
        float b_ratio;
        uint16_t lux;
    };

    const int SAMPLES_PER_PERCENTAGE = 50; // Number of samples to average for each percentage
    const int STABILIZE_DELAY = 400;       // Time in ms to wait for motor speed to stabilize
    const float PERCENTAGE_STEP = 1.0;     // Step size for percentage increments

public:
    struct MotorData
    {
        float percentage;
        float rps_left_back;
        float rps_right_back;
        float rps_left_front;
        float rps_right_front;
    };

    bool calibrateSensors()
    {
        Sensors::Colors currentColor = Sensors::WHITE;
        bool calibrationComplete = false;

        Serial.println("Starting color sensor calibration...");
        Serial.println("Place sensors on WHITE surface and press SW1");

        while (!calibrationComplete)
        {
            int switchState = switches.switchRead(); // calibaration starts only if switch 1 is on.
            // turn it off before it starts calibarating another color

            if (switchState == 1)
            {
                switch (currentColor)
                {
                case Sensors::WHITE:
                    if (calibrateWhite())
                    {
                        currentColor = Sensors::RED;
                        Serial.println("WHITE calibrated. Place sensors on RED surface and press SW1");
                    }
                    break;

                case Sensors::RED:
                    if (calibrateRed())
                    {
                        currentColor = Sensors::BLUE;
                        Serial.println("RED calibrated. Place sensors on BLUE surface and press SW1");
                    }
                    break;

                case Sensors::BLUE:
                    if (calibrateBlue())
                    {
                        currentColor = Sensors::BLACK;
                        Serial.println("BLUE calibrated. Place sensors on BLACK surface and press SW1");
                    }
                    break;

                case Sensors::BLACK:
                    if (calibrateBlack())
                    {
                        calibrationComplete = true;
                        Serial.println("Calibration complete!");
                    }
                    break;
                }
            }
            else if (switchState == 2)
            { // SW2 pressed - abort calibration
                Serial.println("Calibration aborted!");
                return false;
            }

            delay(100); // Debounce delay
        }

        return true;
    }
    // Helper function to get average readings
    ColorReading getAverageReading(int sensorIndex)
    {
        ColorReading total = {0, 0, 0, 0};
        int validSamples = 0;

        for (int i = 0; i < SAMPLES_PER_CALIBRATION; i++)
        {
            uint16_t r, g, b, c, lux;
            sensors.selectChannel(sensorIndex + 3); // Adjust channel offset

            sensors.colourSensorArr[sensorIndex]->getRawData(&r, &g, &b, &c);
            if (c == 0 || r == 65535 || g == 65535 || b == 65535 || c == 65535)
                continue; // Skip invalid readings

            lux = sensors.colourSensorArr[sensorIndex]->calculateLux(r, g, b);

            total.r_ratio += (float)r / c;
            total.g_ratio += (float)g / c;
            total.b_ratio += (float)b / c;
            total.lux += lux;
            validSamples++;

            // delay(CALIBRATION_DELAY_MS);
        }

        if (validSamples == 0)
            return {0, 0, 0, 0};

        return {
            total.r_ratio / validSamples,
            total.g_ratio / validSamples,
            total.b_ratio / validSamples,
            (uint16_t)(total.lux / validSamples)};
    }

    bool calibrateWhite()
    {
        for (int sensor = 0; sensor < 5; sensor++)
        {
            ColorReading reading = getAverageReading(sensor);
            if (reading.lux == 0)
                return false; // Invalid readings

            // Store white threshold with some margin
            sensors.whiteThreshold[sensor][sensors.getModeIndex()] = reading.lux;
        }
        return true;
    }

    bool calibrateRed()
    {
        for (int sensor = 0; sensor < 5; sensor++)
        {
            ColorReading reading = getAverageReading(sensor);
            if (reading.lux == 0)
                return false;

            // Store red-green and red-blue offsets
            sensors.redToGreenOffset[sensor][sensors.getModeIndex()] = (reading.r_ratio - reading.g_ratio);
            sensors.redToBlueOffset[sensor][sensors.getModeIndex()] = (reading.r_ratio - reading.b_ratio);
        }
        return true;
    }

    bool calibrateBlue()
    {
        for (int sensor = 0; sensor < 5; sensor++)
        {
            ColorReading reading = getAverageReading(sensor);
            if (reading.lux == 0)
                return false;

            // Store blue-red and blue-green offsets
            sensors.blueToRedOffset[sensor][sensors.getModeIndex()] = (reading.b_ratio - reading.r_ratio);
            sensors.blueToGreenOffset[sensor][sensors.getModeIndex()] = (reading.b_ratio - reading.g_ratio);
        }
        return true;
    }

    bool calibrateBlack()
    {
        for (int sensor = 0; sensor < 5; sensor++)
        {
            ColorReading reading = getAverageReading(sensor);
            if (reading.lux == 0)
                return false;

            // Store black threshold with some margin
            sensors.blackThreshold[sensor][sensors.getModeIndex()] = reading.lux;
        }
        return true;
    }


    // Motor feed forward calibration functions
    //  Function to collect data for a single percentage value
    MotorData measureAtPercentage(float percentage)
    {
        MotorData data;
        data.percentage = percentage;

        // Set all motors to the same percentage
        motors.set_left_back_motor_percentage(percentage);
        motors.set_right_back_motor_percentage(percentage);
        motors.set_left_front_motor_percentage(percentage);
        motors.set_right_front_motor_percentage(percentage);

        // Wait for speed to stabilize
        delay(STABILIZE_DELAY);

        // Take multiple samples and average them
        float lb_sum = 0, rb_sum = 0, lf_sum = 0, rf_sum = 0;

        for (int i = 0; i < SAMPLES_PER_PERCENTAGE; i++)
        {
            lb_sum += abs((float) encoders.leftRPS_back());
            rb_sum += abs((float) encoders.rightRPS_back());
            lf_sum += abs((float) encoders.leftRPS_front());
            rf_sum += abs((float) encoders.rightRPS_front());
            delay(10); // Small delay between samples
        }

        // Calculate averages
        data.rps_left_back = lb_sum / SAMPLES_PER_PERCENTAGE;
        data.rps_right_back = rb_sum / SAMPLES_PER_PERCENTAGE;
        data.rps_left_front = lf_sum / SAMPLES_PER_PERCENTAGE;
        data.rps_right_front = rf_sum / SAMPLES_PER_PERCENTAGE;

        return data;
    }

    // Function to run a complete calibration sequence
    void runMotorCalibration()
    {
        Serial.println("Starting motor calibration...");
        Serial.println("Percentage, LB_RPS, RB_RPS, LF_RPS, RF_RPS");

        // Test from MIN_MOTOR_PERCENTAGE to MAX_MOTOR_PERCENTAGE
        for (float percentage = MIN_MOTOR_PERCENTAGE; percentage <= MAX_MOTOR_PERCENTAGE; percentage += PERCENTAGE_STEP)
        {

            MotorData data = measureAtPercentage(percentage);
            printMotorData(data);
            delay(20); // Brief pause between measurements
        }

        delay(30);
        motors.coast();
        delay(500);
        // Test negative percentages
        for (float percentage = -MIN_MOTOR_PERCENTAGE; percentage >= -MAX_MOTOR_PERCENTAGE; percentage -= PERCENTAGE_STEP)
        {

            MotorData data = measureAtPercentage(percentage);
            printMotorData(data);
            delay(20);
        }
        delay(30);
        motors.coast();
        delay(500);

        // Stop motors after calibration
        motors.stop();
        Serial.println("Calibration complete!");
    }

    // Don't need to move this function to the printer class, as this will be only used one time for the entire project 
    //unless current limited power sources and motors didn't change
    void printMotorData(const MotorData &data)
    {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),
                 "%6.2f, %6.2f, %6.2f, %6.2f, %6.2f",
                 data.percentage,
                 data.rps_left_back,
                 data.rps_right_back,
                 data.rps_left_front,
                 data.rps_right_front);
        Serial.println(buffer);
    }
};