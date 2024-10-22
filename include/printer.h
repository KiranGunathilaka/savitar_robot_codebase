#pragma once

#include <Arduino.h>
#include "sensors.h"
//Do not call this function inside the sensors class as it will result in circular dependency error


class Printer;
extern Printer printer;

class Printer
{
public:
    int x = 0;
    int diff = 0;

    void printTimeDiff(bool newLine = true)
    {
        Serial.print(millis() - x);
        if (newLine)
        {
            Serial.print("\n");
        }
        else
        {
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

    void printTof(bool newline = false)
    {
        Serial.print(sensors.right_tof);
        Serial.print(" ");
        Serial.print(sensors.left_tof);
        Serial.print(" ");
        Serial.print(sensors.front_tof);
        Serial.print(" ");
        Serial.print(sensors.center_bottom_tof);
        Serial.print(" ");
        Serial.print(sensors.center_top_tof);
        Serial.print("    ");

        if (newline)
        {
            Serial.print("\n");
        }
    }

    void printAllColors(bool newline = false)
    {
        for (int i = 0; i < 5; i++)
        {
            const char *colorStr;
            switch (sensors.sensorColors[i])
            {
            case Sensors::WHITE:
                colorStr = "WHITE";
                break;
            case Sensors::RED:
                colorStr = "RED";
                break;
            case Sensors::BLUE:
                colorStr = "BLUE";
                break;
            case Sensors::BLACK:
                colorStr = "BLACK";
                break;
            default:
                colorStr = "UNKNOWN";
                break;
            }

            Serial.printf("sensor %d: %s", i, colorStr);

            if (i < 4)
            {
                Serial.print("  |  "); // separator between sensors
            }
        }

        if (newline)
        {
            Serial.print("\n");
        }
    }
};