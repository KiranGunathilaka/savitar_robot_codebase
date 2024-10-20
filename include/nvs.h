#pragma once

#include <Preferences.h>
#include "sensors.h"

class NVS;

class NVS
{
public:
    Preferences preferences;

    void saveCalibrationData()
    {
        preferences.begin("sensor_cal", false);

        // Save white threshold values
        size_t written = preferences.putBytes("white_thresh", (uint8_t *)sensors.whiteThreshold, sizeof(sensors.whiteThreshold));
        Serial.printf("Saved %d bytes of white threshold\n", written);

        // Save red offset values
        written = preferences.putBytes("red_green", (uint8_t *)sensors.redToGreenOffset, sizeof(sensors.redToGreenOffset));
        Serial.printf("Saved %d bytes of red-green offset\n", written);

        written = preferences.putBytes("red_blue", (uint8_t *)sensors.redToBlueOffset, sizeof(sensors.redToBlueOffset));
        Serial.printf("Saved %d bytes of red-blue offset\n", written);

        // Save blue offset values
        written = preferences.putBytes("blue_red", (uint8_t *)sensors.blueToRedOffset, sizeof(sensors.blueToRedOffset));
        Serial.printf("Saved %d bytes of blue-red offset\n", written);

        written = preferences.putBytes("blue_green", (uint8_t *)sensors.blueToGreenOffset, sizeof(sensors.blueToGreenOffset));
        Serial.printf("Saved %d bytes of blue-green offset\n", written);

        // Save black threshold values
        written = preferences.putBytes("black_thresh", (uint8_t *)sensors.blackThreshold, sizeof(sensors.blackThreshold));
        Serial.printf("Saved %d bytes of black threshold\n", written);

        preferences.end();
    }

    void loadCalibrationData()
    {
        preferences.begin("sensor_cal", true);

        // Load white threshold values
        size_t read = preferences.getBytes("white_thresh", (uint8_t *)sensors.whiteThreshold, sizeof(sensors.whiteThreshold));
        Serial.printf("Loaded %d bytes of white threshold\n", read);

        // Load red offset values
        read = preferences.getBytes("red_green", (uint8_t *)sensors.redToGreenOffset, sizeof(sensors.redToGreenOffset));
        Serial.printf("Loaded %d bytes of red-green offset\n", read);

        read = preferences.getBytes("red_blue", (uint8_t *)sensors.redToBlueOffset, sizeof(sensors.redToBlueOffset));
        Serial.printf("Loaded %d bytes of red-blue offset\n", read);

        // Load blue offset values
        read = preferences.getBytes("blue_red", (uint8_t *)sensors.blueToRedOffset, sizeof(sensors.blueToRedOffset));
        Serial.printf("Loaded %d bytes of blue-red offset\n", read);

        read = preferences.getBytes("blue_green", (uint8_t *)sensors.blueToGreenOffset, sizeof(sensors.blueToGreenOffset));
        Serial.printf("Loaded %d bytes of blue-green offset\n", read);

        // Load black threshold values
        read = preferences.getBytes("black_thresh", (uint8_t *)sensors.blackThreshold, sizeof(sensors.blackThreshold));
        Serial.printf("Loaded %d bytes of black threshold\n", read);

        preferences.end();
    }

    // Method to clear all stored calibration data
    void clearCalibrationData()
    {
        preferences.begin("sensor_cal", false);
        preferences.clear();
        preferences.end();
        Serial.println("Cleared all sensor calibration data");
    }
};

extern NVS nvs;