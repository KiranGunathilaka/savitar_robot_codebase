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
    bool slowMode = false;
    Ticker ticker;
    
public:

    bool isSlowModeEnabled()
    {
        return slowMode;
    }

    void begin()
    {
        ticker.attach(tickerTime, []()
                      {
                        encoders.update();
                        //motors.update();
                        sensors.update();

                        // Serial.print(time.getTimeDiff());
                        // Serial.print("  ");
                         });
    }

    void enableSlowMode(bool enable)
    {
        if (enable != slowMode){
            ticker.detach();
            if (enable){
                tickerTime = SLOW_TICKER;
                sensors.enableFastMode(false);
                begin();
                Serial.print("Slow Mode");
            }
            else{
                tickerTime = FAST_TICKER;
                sensors.enableFastMode(true);
                begin();
                Serial.print("Fast Mode");
            }
            
            slowMode = enable;
        }
        
    }
};
