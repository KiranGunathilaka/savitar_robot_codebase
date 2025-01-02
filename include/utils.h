#pragma once 

#include "Arduino.h"
#include "config.h"

class Utils {
private:

public:

    void begin() {
        pinMode(EM_PIN, OUTPUT);
        pinMode(LED_PIN, OUTPUT);
    }

    void turnOnEM(){
        digitalWrite(EM_PIN , HIGH);
    }

    void turnOffEM(){
        digitalWrite(EM_PIN , LOW);
    }

    void turnOnLED(){
        delay(500);
        digitalWrite(LED_PIN, HIGH);
    }

    void turnOffLED(){
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
};

extern Utils utils;