#include <Arduino.h>
#include "nvs.h"
#include "systick.h"
#include "config.h"
#include "switches.h"
#include "printer.h"

Motors motors;
Encoders encoders;
Sensors sensors;
Systick systick;
Switches switches;
Printer printer;
NVS nvs;


void setup()
{
  Serial.begin(115200);

  encoders.begin();
  // motors.begin();
  sensors.begin();
  systick.begin();

  switches.enableSimulation(true); //will accept serial inputs as switch data (until switches are connected)

  // systick.enableSlowMode(false);
  // sensors.calibrateSensors();
  // nvs.saveCalibrationData();

  // systick.enableSlowMode(true);
  // sensors.calibrateSensors();
  // nvs.saveCalibrationData();

  nvs.loadCalibrationData();
  printer.printCalibrationData();
}

void loop()
{

}