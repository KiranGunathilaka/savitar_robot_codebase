#pragma once

#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "Ticker.h"
#include "printer.h"
#include "config.h"

class Systick;
extern Systick systick;

class Systick
{
private:
    double tickerTime = FAST_TICKER;
    bool slowMode = false;
    Ticker ticker;

public:
    bool isSlowModeEnabled()
    {
        return slowMode;
    }

    void begin()
    {
        encoders.setLoopTime(tickerTime);
        ticker.attach(tickerTime, []()
                      {
                          encoders.update();
                          //motors.update();
                          //sensors.update();

                          // printer.printTimeDiff();
                          // printer.printTof();
                          // printer.printAllColors();
                          // printer.printEncoderCounts();
                      });
    }

    float getLoopTime()
    {
        return tickerTime;
    }

    void enableSlowMode(bool enable)
    {
        if (enable != slowMode)
        {
            ticker.detach();
            if (enable)
            {
                tickerTime = SLOW_TICKER;
                sensors.enableFastMode(false);
                Serial.println("Slow Mode");
            }
            else
            {
                tickerTime = FAST_TICKER;
                sensors.enableFastMode(true);
                Serial.println("Fast Mode");
            }

            begin();
            slowMode = enable;
        }
    }
};
