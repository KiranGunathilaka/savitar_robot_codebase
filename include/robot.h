#pragma once

#include "Arduino.h"
#include "config.h"
#include "motion.h"
#include "reporting.h"
#include "sensors.h"
#include "systick.h"
#include "nvs.h"

class Robot;
extern Robot robot;

class Robot
{
    void follow_Color_line(){
        systick.enableSlowMode(true);
        motion.reset_drive_system();
        bool isAllEqual = true;
        for (int i =1; i <5 ; i++){
            if (sensors.sensorColors[0] != sensors.sensorColors[i]){
                isAllEqual = false;
                break;
            }
        }

    
    }
};