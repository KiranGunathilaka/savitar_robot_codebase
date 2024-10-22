#pragma once

#include <Arduino.h>
#include "sensors.h"
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

public:
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

            //delay(CALIBRATION_DELAY_MS);
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
};