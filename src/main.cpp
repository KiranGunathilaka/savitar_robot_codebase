#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "Ticker.h"
#include "config.h"

Motors motors;
Encoders encoders;
Sensors sensors;

Ticker sendTicker;

void setup()
{
  // sets the pins as outputs:
  Serial.begin(115200);

  encoders.begin();
  //motors.begin();
  sensors.begin();

  // sendTicker.attach(0.03, []()
  //   { 
  //     encoders.update();
  //     //sensors.update();
  //     Serial.print("Left RPS : ");
  //     Serial.print(encoders.leftRPS());
  //     Serial.print("   Right RPS : ");
  //     Serial.println(encoders.rightRPS()); 
  //       });
}

int x =0;

void loop()
{
  Serial.print(millis()-x);
  Serial.print("  ");
  x = millis();

  sensors.enableToFReadings();
  sensors.update();

}