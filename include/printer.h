#pragma once

#include <Arduino.h>
#include "sensors.h"
#include "encoders.h"
#include "motors.h"
// Do not call this function inside the sensors, encoders, motors classes as it will result in circular dependency error
// And call them always after the update functions as it will print the immediate correct values

class Printer;
extern Printer printer;

class Printer
{
public:
    int x = 0;
    int diff = 0;

    void printTimeDiff(bool newLine = false)
    {
        Serial.print(millis() - x);
        if (newLine)
        {
            Serial.print("\n");
        }
        else
        {
            Serial.print("  ");
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

        if (newline)
        {
            Serial.print("\n");
        }
        else
        {
            Serial.print("  ");
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
        else
        {
            Serial.print(" ");
        }
    }

    void printEncoderCounts(bool newline)
    {
        Serial.print("Encoder Countns || Left_Back: ");
        Serial.print(encoders.tempLeftBack);
        Serial.print("| Left_Front: ");
        Serial.print(encoders.tempLeftFront);
        Serial.print("| Right_Back: ");
        Serial.print(encoders.tempRightBack);
        Serial.print("| Right_Front: ");
        Serial.print(encoders.tempRightFront);

        if (newline)
        {
            Serial.print("\n");
        }
        else
        {
            Serial.print("  ");
        }
    }

    void printMotorFeedPercentages(bool newline)
    {
        Serial.print("Motor percentages || Left_Back: ");
        Serial.print(motors.left_back_motor_percentage);
        Serial.print("| Left_Front: ");
        Serial.print(motors.left_front_motor_percentage);
        Serial.print("| Right_Back: ");
        Serial.print(motors.right_front_motor_percentage);
        Serial.print("| Right_Front: ");
        Serial.print(motors.right_front_motor_percentage);

        if (newline)
        {
            Serial.print("\n");
        }
        else
        {
            Serial.print("  ");
        }
    }

    void printSteeringAdjustment(bool newline)
    {
        Serial.print("Seteering adjust : ");
        Serial.print(sensors.get_steering_adjustment());

        if (newline)
        {
            Serial.print("\n");
        }
        else
        {
            Serial.print("  ");
        }
    }
};