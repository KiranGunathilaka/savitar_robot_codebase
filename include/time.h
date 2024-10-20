#pragma once

#include <Arduino.h>

class Time;
extern Time time;

class Time
{
public:
    int x = 0;
    int diff = 0;

    int getTimeDiff(){
        diff = millis() - x;
        x = millis();

        return diff;
    }
};