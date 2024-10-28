#pragma once

#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <VL53L0X.h>
#include "config.h"
#include "switches.h"
#include "encoders.h"

class Sensors;
extern Sensors sensors;

enum
{
    STEER_NORMAL,
    STEERING_OFF,
};

enum
{
    FAST_MODE,
    SLOW_MODE
};

class Sensors
{
private:
    int integration_time = TCS34725_FAST_INTEGRATION_TIME;
    bool isFast = true;

    // 0 for fast mode(default - black n white) , 1 for slow mode (Colour follwing)-this index will be used to get values from the offset and threshold arrs
    int modeIndex = FAST_MODE;

    bool tofEnabled = true;
    bool colourEnabled = true;

    bool isWire1Init = false;
    bool isWire0Init = false;

    float last_steering_error = 0;
    float accumelated_steering_error = 0;
    volatile float cross_track_error;
    volatile float steering_adjustment;

public:
    VL53L0X tofRight, tofLeft, tofFront, tofCenterTop, tofCenterBottom;
    int prevLeft, prevRight, prevFront, prevCenterTop, prevCenterBottom;
    volatile int left_tof, right_tof, front_tof, center_top_tof, center_bottom_tof;

    const int tofOffset[5] = {-10, -16, -10, -15, -10}; // adjust these values

    float steering_kp = STEERING_KP;
    float steering_kd = STEERING_KD;

    enum Colors
    {
        WHITE,
        RED,
        BLUE,
        BLACK,
        UNKNOWN
    };

    Adafruit_TCS34725 *colourSensorArr[5];
    bool sensorsOnLine[5] = {false, false, false, false, false}; // stores whether each sensor detected the currently following color
    Colors sensorColors[5] = {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN};

    int whiteThreshold[5][2] = {{538, 534}, {534, 534}, {534, 534}, {580, 550}, {534, 534}};
    float redToGreenOffset[5][2] = {{0.422, 0.416}, {0.377, 0.376}, {0.372, 0.380}, {0.414, 0.409}, {0.357, 0.364}};
    float redToBlueOffset[5][2] = {{0.47, 0.464}, {0.429, 0.428}, {0.423, 0.435}, {0.461, 0.456}, {0.422, 0.429}};
    float blueToRedOffset[5][2] = {{0.1036, 0.0991}, {0.1185, 0.1247}, {0.1161, 0.1106}, {0.1161, 0.1106}, {0.1161, 0.1106}};
    float blueToGreenOffset[5][2] = {{0.0704, 0.0674}, {0.0704, 0.0674}, {0.0704, 0.0674}, {0.0704, 0.0674}, {0.0704, 0.0674}};
    int blackThreshold[5][2] = {{53, 53}, {72, 70}, {89, 88}, {62, 62}, {59, 68}};

    Colors followingColor = WHITE;

    uint8_t steering_mode = STEER_NORMAL;

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

    // to swtich between colour sensor 2 cycle mode and 4 cycle mode
    void enableFastMode(bool enabled)
    {
        isFast = enabled;

        if (!isFast)
        {
            modeIndex = SLOW_MODE;
            integration_time = TCS34725_SLOW_INTEGRATION_TIME;
        }
        else
        {
            modeIndex = FAST_MODE;
            integration_time = TCS34725_FAST_INTEGRATION_TIME;
        }
    }

    int getModeIndex()
    {
        return modeIndex;
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

    float get_steering_adjustment()
    {
        return steering_adjustment;
    }

    void setFollowingColor(Colors color)
    {
        followingColor = color;
    }

    Colors getFollowingColor()
    {
        return followingColor;
    }

    void set_steering_mode(uint8_t mode)
    {
        last_steering_error = cross_track_error;
        steering_adjustment = 0;
        steering_mode = mode;
    }

    void calculate_steering_adjustment()
    {
        float pTerm = steering_kp * cross_track_error;
        float dTerm = steering_kd * (cross_track_error - last_steering_error);

        float adjustment = (pTerm + dTerm) * encoders.getLoopTime();

        last_steering_error = cross_track_error;
        steering_adjustment = adjustment;
    }

    void update()
    {
        if (tofEnabled)
        {
            prevRight = right_tof;
            right_tof = (float)(prevRight + tofRight.readRangeContinuousMillimeters() + tofOffset[1]) / 2.0;

            prevLeft = left_tof;
            left_tof = (prevLeft + tofLeft.readRangeContinuousMillimeters() + tofOffset[0]) / 2.0;

            prevFront = front_tof;
            front_tof = (prevFront + tofFront.readRangeContinuousMillimeters() + tofOffset[2]) / 2.0;

            // prevCenterBottom = center_bottom_tof;
            // center_bottom_tof = (prevCenterBottom + tofCenterBottom.readRangeContinuousMillimeters() + tofOffset[3]) / 2.0;

            // prevCenterTop = center_top_tof;
            // center_top_tof = (prevCenterTop + tofCenterTop.readRangeContinuousMillimeters() + tofOffset[4]) / 2.0;

            right_tof = abs(prevRight - right_tof) > 3 ? right_tof : prevRight;
            left_tof = abs(prevLeft - left_tof) > 3 ? left_tof : prevLeft;
            front_tof = abs(prevFront - front_tof) > 3 ? front_tof : prevFront;
            // center_bottom_tof = abs(prevCenterBottom - center_bottom_tof) > 3 ? center_bottom_tof : prevCenterBottom;
            // center_top_tof = abs(prevCenterTop - center_top_tof) > 3 ? center_top_tof : prevCenterTop;
        }

        if (colourEnabled)
        {
            int error = 0;

            for (uint8_t t = 3; t < 8; t++)
            {
                selectChannel(t);

                uint16_t r, g, b, c, lux;
                colourSensorArr[t - 3]->getRawData(&r, &g, &b, &c);
                lux = colourSensorArr[t - 3]->calculateLux(r, g, b);

                float r_ratio = (float)r / c;
                float g_ratio = (float)g / c;
                float b_ratio = (float)b / c;

                Colors color = sensors.classifyColor(t - 3, r_ratio, g_ratio, b_ratio, lux);
                sensorColors[t - 3] = color;

                // Serial.printf(" r: %f g: %f b: %f c: %d lux:%d ", r_ratio, g_ratio, b_ratio, c, lux);

                if (followingColor == color && steering_mode == STEER_NORMAL)
                {
                    error += SENSOR_WEIGHTS[t - 3];
                    sensorsOnLine[t - 3] = true;
                }

                cross_track_error = error;
                calculate_steering_adjustment();
            }
        }
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

    Colors classifyColor(int sensor, float r, float g, float b, uint16_t lux)
    {
        if (sensors.getModeIndex() == FAST_MODE)
        {
            if (lux > whiteThreshold[sensor][modeIndex] * 0.6)
            {
                return WHITE;
            }
            else if (lux < blackThreshold[sensor][modeIndex] * 1.4)
            {
                return BLACK;
            }
            return UNKNOWN;
        }

        else
        {

            if (lux > whiteThreshold[sensor][modeIndex] * 0.6)
            {
                return WHITE;
            }
            else if (r > g + redToGreenOffset[sensor][modeIndex] * 0.8 && r > b + redToBlueOffset[sensor][modeIndex] * 0.8)
            {
                return RED;
            }
            else if (b > r + blueToRedOffset[sensor][modeIndex] * 0.6 && b > g + blueToGreenOffset[sensor][modeIndex] * 0.6)
            {
                return BLUE;
            }
            else if (lux < blackThreshold[sensor][modeIndex] * 1.4)
            {
                return BLACK;
            }
            return UNKNOWN; // consider unknown to be the colour of the background as in the interfaces of 2 colours it gives none of the above values
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

        digitalWrite(ToF_XSHUT_Right, HIGH);
        tofRight.init(true);
        tofRight.setAddress(TOF_RIGHT_ADD);

        digitalWrite(ToF_XSHUT_Left, HIGH);
        tofLeft.init(true);
        tofLeft.setAddress(TOF_LEFT_ADD);

        digitalWrite(ToF_XSHUT_Front, HIGH);
        tofFront.init(true);
        tofFront.setAddress(TOF_FRONT_ADD);

        // digitalWrite(ToF_XSHUT_Center_Top, HIGH);
        // tofCenterTop.init(true);
        // tofCenterTop.setAddress(TOF_CENTER_TOP_ADD);

        // digitalWrite(ToF_XSHUT_Center_Bottom, HIGH);
        // tofCenterBottom.init(true);
        // tofCenterBottom.setAddress(TOF_CENTER_BOTTOM_ADD);

        Serial.println("ToF addresses set");

        delay(2);

        tofRight.startContinuous();
        tofLeft.startContinuous();
        tofFront.startContinuous();
        // tofCenterTop.startContinuous();
        // tofCenterBottom.startContinuous();

        tofRight.setMeasurementTimingBudget(20000);
        tofLeft.setMeasurementTimingBudget(20000);
        tofFront.setMeasurementTimingBudget(20000);
        // tofCenterTop.setMeasurementTimingBudget(20000);
        // tofCenterBottom.setMeasurementTimingBudget(20000);

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
                Serial.printf("Sensor %d initialized successfully\n", t - 2);
            }
            else
            {
                Serial.printf("Failed to initialize sensor %d\n", t - 2);
            }
        }
    }
};