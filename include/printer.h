#pragma once

#include <Arduino.h>
#include "sensors.h"

class Printer;
extern Printer printer;

class Printer
{
public:
    int x = 0;
    int diff = 0;

    void printTimeDiff(bool newLine)
    {
        Serial.print(millis()-x);
        if (newLine){
            Serial.print("\n");
        }else{
            Serial.print(" ");
        }
        x = millis();
    }

    void printCalibrationData()
    {
        Serial.println("\n=== Sensor Calibration Data ===");

        // Print White Thresholds
        Serial.println("\nWhite Thresholds [sensor][mode]:");
        Serial.println("       Fast Mode  Slow Mode");
        Serial.println("       ---------  ---------");
        for (int i = 0; i < 5; i++)
        {
            Serial.printf("S%d:     %-9d %-9d\n",
                          i + 1,
                          sensors.whiteThreshold[i][0],
                          sensors.whiteThreshold[i][1]);
        }

        // Print Red Offsets
        Serial.println("\nRed to Green Offsets:");
        Serial.println("       Fast Mode  Slow Mode");
        Serial.println("       ---------  ---------");
        for (int i = 0; i < 5; i++)
        {
            Serial.printf("S%d:     %-9f %-9f\n",
                          i + 1,
                          sensors.redToGreenOffset[i][0],
                          sensors.redToGreenOffset[i][1]);
        }

        Serial.println("\nRed to Blue Offsets:");
        Serial.println("       Fast Mode  Slow Mode");
        Serial.println("       ---------  ---------");
        for (int i = 0; i < 5; i++)
        {
            Serial.printf("S%d:     %-9f %-9f\n",
                          i + 1,
                          sensors.redToBlueOffset[i][0],
                          sensors.redToBlueOffset[i][1]);
        }

        // Print Blue Offsets
        Serial.println("\nBlue to Red Offsets:");
        Serial.println("       Fast Mode  Slow Mode");
        Serial.println("       ---------  ---------");
        for (int i = 0; i < 5; i++)
        {
            Serial.printf("S%d:     %-9f %-9f\n",
                          i + 1,
                          sensors.blueToRedOffset[i][0],
                          sensors.blueToRedOffset[i][1]);
        }

        Serial.println("\nBlue to Green Offsets:");
        Serial.println("       Fast Mode  Slow Mode");
        Serial.println("       ---------  ---------");
        for (int i = 0; i < 5; i++)
        {
            Serial.printf("S%d:     %-9f %-9f\n",
                          i + 1,
                          sensors.blueToGreenOffset[i][0],
                          sensors.blueToGreenOffset[i][1]);
        }

        // Print Black Thresholds
        Serial.println("\nBlack Thresholds:");
        Serial.println("       Fast Mode  Slow Mode");
        Serial.println("       ---------  ---------");
        for (int i = 0; i < 5; i++)
        {
            Serial.printf("S%d:     %-9d %-9d\n",
                          i + 1,
                          sensors.blackThreshold[i][0],
                          sensors.blackThreshold[i][1]);
        }

        Serial.println("\n=== End of Calibration Data ===\n");
    }
};