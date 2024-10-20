#include <Arduino.h>
#include "systick.h"
#include "config.h"

Motors motors;
Encoders encoders;
Sensors sensors;
Systick systick;
Time time;


void setup()
{
  Serial.begin(115200);

  encoders.begin();
  // motors.begin();
  sensors.begin();
  systick.begin();

  delay(10000);
}

void loop()
{
  sensors.enableToFReadings();


  systick.setTickerTime(SLOW_TICKER);
  systick.tickerReset();

  Serial.println("reattached");
  delay(100000);
}