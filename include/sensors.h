#pragma once

#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <VL53L0X.h>
#include "config.h"
#include "switches.h"

class Sensors;
extern Sensors sensors;

class Sensors
{
public:
    VL53L0X tofRight, tofLeft, tofFront, tofCenterTop, tofCenterBottom;
    int prevLeft, prevRight, prevFront, prevCenterTop, prevCenterBottom;
    volatile int left_tof, right_tof, front_tof, center_top_tof, center_bottom_tof;

    const int tofOffset[5] = {-10, -16, -10, -15, -10}; // adjust these values

    Adafruit_TCS34725 *colourSensorArr[5];

    enum Colors
    {
        UNKNOWN,
        WHITE,
        RED,
        BLUE,
        BLACK
    };

    enum CalibrationColor
    {
        CAL_WHITE = 0,
        CAL_RED = 1,
        CAL_BLUE = 2,
        CAL_BLACK = 3
    };

    int whiteThreshold[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int redToGreenOffset[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int redToBlueOffset[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int blueToRedOffset[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int blueToGreenOffset[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int blackThreshold[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    // to swtich between colour sensor 2 cycle mode and 4 cycle mode
    void enableFastMode(bool enabled)
    {
        isFast = enabled;

        if (!isFast)
        {
            modeIndex = 1;
            integration_time = TCS34725_SLOW_INTEGRATION_TIME;
        }
        else
        {
            modeIndex = 0;
            integration_time = TCS34725_FAST_INTEGRATION_TIME;
        }
    }

    // to not take readings and save time
    void enableToFReadings()
    {
        tofEnabled = true;
    }

    void disableColourSensorReadings()
    {
        tofEnabled = false;
    }

    void enableColourSensorReadings()
    {
        colourEnabled = true;
    }

    void disableToFReadings()
    {
        colourEnabled = false;
    }

    // Select multiplexer channel
    bool selectChannel(uint8_t channel)
    {
        if (channel > 7)
            return false;

        Wire1.beginTransmission(MULTIPLEXER_ADDR);
        Wire1.write(1 << channel);
        return (Wire1.endTransmission() == 0);
    }

    // Initialize all sensors
    void begin()
    {
        pinMode(ToF_XSHUT_Right, OUTPUT);
        pinMode(ToF_XSHUT_Left, OUTPUT);
        pinMode(ToF_XSHUT_Front, OUTPUT);
        pinMode(ToF_XSHUT_Center_Top, OUTPUT);
        pinMode(ToF_XSHUT_Center_Bottom, OUTPUT);

        colourSensorReset();

        tofReset();
    }

    Colors classifyColor(int i, float r, float g, float b, uint16_t lux)
    {
        // Find the dominant color
        if (lux > whiteThreshold[i][modeIndex])
        {
            return WHITE;
        }
        else if (r > g + redToGreenOffset[i][modeIndex] && r > b + redToBlueOffset[i][modeIndex])
        {
            return RED;
        }
        else if (b > r + blueToRedOffset[i][modeIndex] && b > g + blueToGreenOffset[i][modeIndex])
        {
            return BLUE;
        }
        else if (lux < blackThreshold[i][modeIndex])
        {
            return BLACK;
        }
        return UNKNOWN; // consider unknown to be the colour of the background as in the interfaces of 2 colours it gives none of the above values
    }

    void update()
    {

        if (tofEnabled)
        {
            // prevRight = right_tof;
            // right_tof = (float)(prevRight + tofRight.readRangeContinuousMillimeters() + tofOffset[1]) / 2.0;

            prevLeft = left_tof;
            left_tof = (prevLeft + tofLeft.readRangeContinuousMillimeters() + tofOffset[0]) / 2.0;

            prevFront = front_tof;
            front_tof = (prevFront + tofFront.readRangeContinuousMillimeters() + tofOffset[2]) / 2.0;

            prevCenterBottom = center_bottom_tof;
            center_bottom_tof = (prevCenterBottom + tofCenterBottom.readRangeContinuousMillimeters() + tofOffset[3]) / 2.0;

            prevCenterTop = center_top_tof;
            center_top_tof = (prevCenterTop + tofCenterTop.readRangeContinuousMillimeters() + tofOffset[4]) / 2.0;

            // right_tof = abs(prevRight - right_tof) > 3 ? right_tof : prevRight;
            left_tof = abs(prevLeft - left_tof) > 3 ? left_tof : prevLeft;
            front_tof = abs(prevFront - front_tof) > 3 ? front_tof : prevFront;
            center_bottom_tof = abs(prevCenterBottom - center_bottom_tof) > 3 ? center_bottom_tof : prevCenterBottom;
            center_top_tof = abs(prevCenterTop - center_top_tof) > 3 ? center_top_tof : prevCenterTop;

            // Serial.print(right_tof);
            // Serial.print(" ");
            // Serial.print(left_tof);
            // Serial.print(" ");
            // Serial.print(front_tof);
            // Serial.print(" ");
            // Serial.print(center_bottom_tof);
            // Serial.print(" ");
            // Serial.print(center_top_tof);
            // Serial.print("    ");
        }

        if (colourEnabled)
        {
            for (uint8_t t = 3; t < 8; t++)
            {
                selectChannel(t);

                uint16_t r, g, b, c, lux;
                colourSensorArr[t - 3]->getRawData(&r, &g, &b, &c);
                lux = colourSensorArr[t - 3]->calculateLux(r, g, b);

                float r_ratio = (float)r / c;
                float g_ratio = (float)g / c;
                float b_ratio = (float)b / c;

                int color = sensors.classifyColor(t - 3, r_ratio, g_ratio, b_ratio, lux);
                const char *colorStr;
                switch (color)
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
                //Serial.printf("sensor %d : %s  ",t-2, colorStr); 

                // Serial.printf(" r: %f g: %f b: %f c: %d lux:%d ", r_ratio, g_ratio, b_ratio, c, lux);
            }
            //Serial.print("\n");
        }
    }

    void tofReset()
    {
        digitalWrite(ToF_XSHUT_Right, LOW);
        digitalWrite(ToF_XSHUT_Left, LOW);
        digitalWrite(ToF_XSHUT_Front, LOW);
        digitalWrite(ToF_XSHUT_Center_Top, LOW);
        digitalWrite(ToF_XSHUT_Center_Bottom, LOW);

        if (isWire0Init)
        {
            Wire.end();
            delay(5);
        }

        Wire.begin(I2C_SDA_0, I2C_SCL_0, 400000);
        isWire0Init = true;

        // tofRight.setBus(&Wire);
        tofLeft.setBus(&Wire);
        tofFront.setBus(&Wire);
        tofCenterBottom.setBus(&Wire);
        tofCenterTop.setBus(&Wire);

        delay(2);

        // digitalWrite(ToF_XSHUT_Right, HIGH);
        // tofRight.init(true);
        // tofRight.setAddress(TOF_RIGHT_ADD);

        digitalWrite(ToF_XSHUT_Left, HIGH);
        tofLeft.init(true);
        tofLeft.setAddress(TOF_LEFT_ADD);

        digitalWrite(ToF_XSHUT_Front, HIGH);
        tofFront.init(true);
        tofFront.setAddress(TOF_FRONT_ADD);

        digitalWrite(ToF_XSHUT_Center_Top, HIGH);
        tofCenterTop.init(true);
        tofCenterTop.setAddress(TOF_CENTER_TOP_ADD);

        digitalWrite(ToF_XSHUT_Center_Bottom, HIGH);
        tofCenterBottom.init(true);
        tofCenterBottom.setAddress(TOF_CENTER_BOTTOM_ADD);

        Serial.println("ToF addresses set");

        delay(2);

        // tofRight.startContinuous();
        tofLeft.startContinuous();
        tofFront.startContinuous();
        tofCenterTop.startContinuous();
        tofCenterBottom.startContinuous();

        // tofRight.setMeasurementTimingBudget(20000);
        tofLeft.setMeasurementTimingBudget(20000);
        tofFront.setMeasurementTimingBudget(20000);
        tofCenterTop.setMeasurementTimingBudget(20000);
        tofCenterBottom.setMeasurementTimingBudget(20000);

        delay(5);
    }

    void colourSensorReset()
    {
        if (isWire1Init)
        {
            Wire1.end();
            delay(5);
        }

        Wire1.begin(I2C_SDA_1, I2C_SCL_1, 400000);
        isWire1Init = true;

        for (uint8_t t = 3; t < 8; t++)
        {
            selectChannel(t);

            Serial.print("TCA Port #");
            Serial.println(t);

            colourSensorArr[t - 3] = new Adafruit_TCS34725(integration_time, TCS34725_GAIN_60X);

            if (colourSensorArr[t - 3]->begin(TCS34725_ADDRESS, &Wire1))
            {
                Serial.printf("Sensor %d initialized successfully\n", t - 3);
            }
            else
            {
                Serial.printf("Failed to initialize sensor %d\n", t - 3);
            }
        }
    }

    bool calibrateSensors()
    {
        CalibrationColor currentColor = CAL_WHITE;
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
                case CAL_WHITE:
                    if (calibrateWhite())
                    {
                        currentColor = CAL_RED;
                        Serial.println("WHITE calibrated. Place sensors on RED surface and press SW1");
                    }
                    break;

                case CAL_RED:
                    if (calibrateRed())
                    {
                        currentColor = CAL_BLUE;
                        Serial.println("RED calibrated. Place sensors on BLUE surface and press SW1");
                    }
                    break;

                case CAL_BLUE:
                    if (calibrateBlue())
                    {
                        currentColor = CAL_BLACK;
                        Serial.println("BLUE calibrated. Place sensors on BLACK surface and press SW1");
                    }
                    break;

                case CAL_BLACK:
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

private:
    // Constants for calibration
    static const int SAMPLES_PER_CALIBRATION = 100;
    static const int CALIBRATION_DELAY_MS = 5;

    int integration_time = TCS34725_FAST_INTEGRATION_TIME;
    bool isFast = true;
    int modeIndex = 0; // 0 for fast mode(default) , 1 for fast mode
    // this index will be used to get values from the offset and threshold arrs

    bool tofEnabled = true;
    bool colourEnabled = true;

    bool isWire1Init = false;
    bool isWire0Init = false;

    // Struct to hold RGB and Lux readings
    struct ColorReading
    {
        float r_ratio;
        float g_ratio;
        float b_ratio;
        uint16_t lux;
    };

    // Helper function to get average readings
    ColorReading getAverageReading(int sensorIndex)
    {
        ColorReading total = {0, 0, 0, 0};
        int validSamples = 0;

        for (int i = 0; i < SAMPLES_PER_CALIBRATION; i++)
        {
            uint16_t r, g, b, c, lux;
            selectChannel(sensorIndex + 3); // Adjust channel offset

            colourSensorArr[sensorIndex]->getRawData(&r, &g, &b, &c);
            if (c == 0 || r == 65535 || g == 65535 || b == 65535 || c == 65535)
                continue; // Skip invalid readings

            lux = colourSensorArr[sensorIndex]->calculateLux(r, g, b);

            total.r_ratio += (float)r / c;
            total.g_ratio += (float)g / c;
            total.b_ratio += (float)b / c;
            total.lux += lux;
            validSamples++;

            delay(CALIBRATION_DELAY_MS);
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
            whiteThreshold[sensor][modeIndex] = reading.lux * 0.5; // 90% of white reading
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
            redToGreenOffset[sensor][modeIndex] = (reading.r_ratio - reading.g_ratio) * 0.5; // Red to green threshold
            redToBlueOffset[sensor][modeIndex] = (reading.r_ratio - reading.b_ratio) * 0.5;  // Red to blue threshold
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
            blueToRedOffset[sensor][modeIndex] = (reading.b_ratio - reading.r_ratio) * 0.5;   // Blue to red threshold
            blueToGreenOffset[sensor][modeIndex] = (reading.b_ratio - reading.g_ratio) * 0.5; // Blue to green threshold
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
            blackThreshold[sensor][modeIndex] = reading.lux * 1.5; // 150% of black reading
        }
        return true;
    }
};