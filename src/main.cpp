#include <Arduino.h>
#include "systick.h"
#include "config.h"
#include "switches.h"

Motors motors;
Encoders encoders;
Sensors sensors;
Systick systick;
Switches switches;
Time time;


void setup()
{
  Serial.begin(115200);

  encoders.begin();
  // motors.begin();
  sensors.begin();
  systick.begin();

  switches.enableSimulation(true);
}

void loop()
{

}