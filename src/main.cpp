#include <Arduino.h>
#include "nvs.h"
#include "systick.h"
#include "config.h"
#include "switches.h"
#include "sensors.h"
#include "printer.h"
#include "calibaration.h"
#include "nvs.h"
#include "profile.h"
#include "motion.h"

Motors motors;
Encoders encoders;
Sensors sensors;
Systick systick;
Switches switches;
Printer printer;
NVS nvs;
Calibaration calibaration;
Profile rotation;
Profile forward;
Motion motion;


void setup()
{
  Serial.begin(115200);

  encoders.begin();
  motors.begin();
  sensors.begin();
  systick.begin();

  switches.enableSimulation(true); // will accept serial inputs as switch data (until switches are connected)

  // systick.enableSlowMode(false);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  // systick.enableSlowMode(true);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  //nvs.loadCalibrationData();
  //systick.enableSlowMode(false);
  printer.printCalibrationData();
  systick.enableSlowMode(true);
  motion.reset_drive_system();
  sensors.setFollowingColor(Sensors::BLUE);
}

void loop()
{
  

}