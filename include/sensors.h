#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <VL53L0X.h>
#include "config.h"

class Sensors;
extern Sensors sensors;

class Sensors
{
private:
    int integration_time = TCS34725_SLOW_INTEGRATION_TIME;
    bool tofEnabled = false;
    bool colourEnabled = true;

    bool isWire1Init = false;
    bool isWire0Init = false;

    int redOffset = 0.4;
    int greenOffset = 0.05;
    int blueOffset = 0.05;
    int whiteThreshold = 8;
    int blackThreshold = 5;

public:
    VL53L0X tofRight, tofLeft, tofFront, tofCenterTop, tofCenterBottom;
    int prevLeft, prevRight, prevFront, prevCenterTop, prevCenterBottom;
    const int tofOffset[5] = {-10, -16, -10, -15, -10};
    volatile int left_tof, right_tof, front_tof, center_top_tof, center_bottom_tof;

    Adafruit_TCS34725 *colourSensorArr[5];

    enum Colors
    {
        UNKNOWN,
        RED,
        BLUE,
        WHITE,
        BLACK
    };

    void setIntegrationTime(int time)
    {
        integration_time = time;
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

    Colors classifyColor(float r, float g, float b, uint16_t lux)
    {
        // Find the dominant color
        if (lux > whiteThreshold)
        {
            return WHITE;
        }
        else if (r > g + redOffset && r > b + redOffset)
        {
            return RED;
        }
        else if (b > r + blueOffset && b > g + blueOffset)
        {
            return BLUE;
        }
        else if (lux < blackThreshold)
        {
            return BLACK;
        }
        return UNKNOWN; // consider unknown to be the out of line colour as in the border, it gives a value in between white lux and black lux
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
            Serial.print(" ");
            Serial.print(left_tof);
            Serial.print(" ");
            Serial.print(front_tof);
            Serial.print(" ");
            Serial.print(center_bottom_tof);
            Serial.print(" ");
            Serial.print(center_top_tof);
            Serial.print("    ");
        }

        if (colourEnabled)
        {
            for (uint8_t t = 3; t < 8; t++)
            {
                selectChannel(t);

                uint16_t r, g, b, c, lux;
                float r_ratio, g_ratio, b_ratio;
                colourSensorArr[t - 3]->getRawData(&r, &g, &b, &c);
                lux = colourSensorArr[t - 3]->calculateLux(r, g, b);

                r_ratio = (float)r / c;
                g_ratio = (float)g / c;
                b_ratio = (float)b / c;

                int color = classifyColor(r_ratio, g_ratio, b_ratio, lux);

                Serial.printf("Sensor %d  : %s   ", t - 2, color == 0 ? "UNKONWN" : color == 1 ? "RED"
                                                                                : color == 2   ? "BLUE"
                                                                                : color == 3   ? "WHITE"
                                                                                               : "BLACK");

                //Serial.printf(" r: %f g: %f b: %f c: %d lux:%d ", r_ratio, g_ratio, b_ratio, c, lux);
            }
            Serial.print("\n");
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
};