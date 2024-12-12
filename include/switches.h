#pragma once

#include <Arduino.h>
#include <config.h>

class Switches;

extern Switches switches;

class Switches
{
private:
    bool simulationMode = false;

public:

    void enableSimulation(bool enable) {
        simulationMode = enable;
        if (enable) {
            Serial.println("Switch simulation mode enabled. Enter values 0-6 to simulate switch states.");
        }
    }

    int switchRead() {
        if (simulationMode) {
            return simulatedSwitchRead();
        } else {
            return realSwitchRead();
        }
    }

private:
    int realSwitchRead() {
        int analogValue = analogRead(SWITCH_PIN);
        if (analogValue > 3900) return 1;
        else if (analogValue > 3300) return 2;
        else if (analogValue > 2800) return 3;
        else if (analogValue > 2300) return 4;
        else if (analogValue > 1800) return 5;
        else if (analogValue > 1300) return 6;
        else if (analogValue > 800) return 7;
        else if (analogValue > 300) return 8;
        return 0;
    }

    int simulatedSwitchRead() {
        while (true) {  // This creates a blocking read
            if (Serial.available() > 0) {
                int input = Serial.parseInt();
                Serial.read(); // Clear the buffer
                if (input >= 0 && input <= 6) {
                    Serial.print("Simulated switch value: ");
                    Serial.println(input);
                    return input;
                } else {
                    Serial.println("Invalid input. Please enter a value between 0 and 6.");
                }
            }
            delay(10); 
        }
    }
};