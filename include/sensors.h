#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <VL53L0X.h>
#include "config.h"

class Sensors;
extern Sensors sensors;

class Sensors
{
public:
    VL53L0X tofRight, tofLeft , tofCenterTop, tofCenterBottom;
    float prevLeft, prevRight, prevCenterTop, prevCenterBottom;
    const int tofOffset[4] = {-10, -16, -15, -10};
    volatile float left_tof, right_tof, center_top_tof, center_bottom_tof;

    Adafruit_TCS34725 *colourSensorArr[5];

    int redOffset = 0.4;
    int greenOffset = 0.05;
    int blueOffset = 0.05;
    int whiteOffset = 8;
    int blackOffset = 5;

    enum Colors
    {
        UNKNOWN,
        RED,
        GREEN,
        BLUE,
        WHITE,
        BLACK
    };



    // Select multiplexer channel
    bool selectChannel(uint8_t channel)
    {
        if (channel > 7)
            return false;

        Wire.beginTransmission(MULTIPLEXER_ADDR);
        Wire.write(1 << channel);
        return (Wire.endTransmission() == 0);
    }

    // Initialize all sensors
    void begin()
    {
        pinMode(I2C_SDA, INPUT_PULLUP);
        pinMode(I2C_SCL, INPUT_PULLUP);
        Wire.begin(I2C_SDA, I2C_SCL);

        for (int i = 0; i < 5; i++)
        {
            colourSensorArr[i] = nullptr;
        }

        for (uint8_t t = 3; t < 8; t++)
        {
            selectChannel(t);

            Serial.print("TCA Port #");
            Serial.println(t);

            colourSensorArr[t-3] = new Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X);

            if (colourSensorArr[t-3]->begin(TCS34725_ADDRESS, &Wire))
            {
                Serial.printf("Sensor %d initialized successfully\n", t);
            }
            else
            {
                Serial.printf("Failed to initialize sensor %d\n", t);
            }
        }

        pinMode(ToF_XSHUT_Right, OUTPUT);
        pinMode(ToF_XSHUT_Center_Top, OUTPUT);
        pinMode(ToF_XSHUT_Center_Bottom, OUTPUT);
        pinMode(ToF_XSHUT_Left, OUTPUT);

        // digitalWrite(ToF_XSHUT_Right, LOW);
        digitalWrite(ToF_XSHUT_Center_Top, LOW);
        // digitalWrite(ToF_XSHUT_Center_Bottom, LOW);
        // digitalWrite(ToF_XSHUT_Left, LOW);

        // digitalWrite(ToF_XSHUT_Right, HIGH);
        // delay(30);
        // tofRight.init(true);
        // delay(30);
        // tofRight.setAddress(TOF_RIGHT_ADD);

        // changing address of 2nd tof
        digitalWrite(ToF_XSHUT_Center_Top, HIGH);
        delay(30);
        tofCenterTop.init(true);
        delay(30);
        tofCenterTop.setAddress(TOF_CENTER_TOP_ADD);

        // digitalWrite(ToF_XSHUT_Center_Bottom, HIGH);
        // delay(30);
        // tofCenterBottom.init(true);
        // delay(30);
        // tofCenterBottom.setAddress(TOF_CENTER_BOTTOM_ADD);

        // // changing address of 3rd tof
        // digitalWrite(ToF_XSHUT_Left, HIGH);
        // delay(30);
        // tofLeft.init(true);
        // delay(30);
        // tofLeft.setAddress(TOF_LEFT_ADD);

        tofRight.startContinuous();
        tofCenterTop.startContinuous();
        // tofCenterBottom.startContinuous();
        // tofLeft.startContinuous();

        // tofLeft.setMeasurementTimingBudget(20000);
        tofCenterTop.setMeasurementTimingBudget(20000);
        // tofCenterBottom.setMeasurementTimingBudget(20000);
        // tofRight.setMeasurementTimingBudget(20000);
    }

    Colors classifyColor(float r, float g, float b, uint16_t lux)
    {
        // Find the dominant color
        if (r > g + redOffset && r > b + redOffset)
        {
            return RED;
        }
        else if (g > r + greenOffset && g > b + greenOffset)
        {
            return GREEN;
        }
        else if (b > r + blueOffset && b > g + blueOffset)
        {
            return BLUE;
        }
        else if (lux > whiteOffset)
        {
            return WHITE;
        }
        else if (lux < blackOffset)
        {
            return BLACK;
        }
        return UNKNOWN; // consider unknown to be the out of line colour as in the border, it gives a value in between white lux and black lux
    }




    void update()
    {
        for (uint8_t t = 3; t < 8; t++)
        {
            selectChannel(t);

            uint16_t r, g, b, c, lux;
            float r_ratio, g_ratio, b_ratio;
            colourSensorArr[t-3]->getRawData(&r, &g, &b, &c);
            lux = colourSensorArr[t-3]->calculateLux(r, g, b);

            r_ratio = (float)r/c; 
            g_ratio = (float)g/c; 
            b_ratio = (float)b/c; 

            int color = classifyColor(r_ratio, g_ratio, b_ratio, lux);

            Serial.printf("Sensor %d  : %s   ", t-2, color == 0 ? "UNKONWN" 
                                                : color == 1 ? "RED"
                                                : color == 2   ? "GREEN"
                                                : color == 3   ? "BLUE"
                                                : color == 4   ? "WHITE"
                                                : "BLACK");

            Serial.printf( " r: %f g: %f b: %f c: %d lux:%d ", r_ratio, g_ratio, b_ratio, c, lux );
            Serial.print("\n");
        }

        // prevLeft = left_tof;
        // left_tof = (float)(prevLeft + tofLeft.readRangeContinuousMillimeters() + tofOffset[0]) / 2.0;

        // prevRight = right_tof;
        // right_tof = (float)(prevRight + tofRight.readRangeContinuousMillimeters() + tofOffset[1]) / 2.0;

        // prevCenterBottom = center_bottom_tof;
        // center_bottom_tof = (float)(prevCenterBottom + tofCenterBottom.readRangeContinuousMillimeters() + tofOffset[2])/2.0;

        prevCenterTop = center_top_tof;
        center_top_tof = (float)(prevCenterTop + tofCenterTop.readRangeContinuousMillimeters() + tofOffset[3])/2.0;


        //left_tof = abs(prevLeft - left_tof) > 3 ? left_tof : prevLeft;
        //right_tof = abs(prevRight - right_tof) > 3 ? right_tof : prevRight;
        //center_bottom_tof = abs(prevCenterBottom - center_bottom_tof) > 3? center_bottom_tof : prevCenterBottom;
        center_top_tof = abs(prevCenterTop - center_top_tof) > 3? center_top_tof : prevCenterTop;

        Serial.print(center_top_tof);
    }
};