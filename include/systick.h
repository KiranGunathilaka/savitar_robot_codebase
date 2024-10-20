#pragma once

#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "Ticker.h"
#include "time.h"
#include "config.h"

class Systick;
extern Systick systick;

class Systick
{
private:
    double tickerTime = FAST_TICKER;
    Ticker ticker;
    
public:
    void setTickerTime(double time)
    {
        tickerTime = time;
    }

    double getTickerTime()
    {
        return tickerTime;
    }

    void begin()
    {
        ticker.attach(tickerTime, []()
                      {
                        encoders.update();
                        //motors.update();
                        sensors.update();
                        Serial.print(time.getTimeDiff());
                        Serial.print("  ");
                         });
    }

    void tickerReset()
    {
        ticker.detach();
        begin();
    }
};
