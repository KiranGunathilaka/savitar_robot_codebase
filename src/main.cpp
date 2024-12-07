#include <Arduino.h>
#include "nvs.h"
#include "systick.h"
#include "config.h"
#include "switches.h"
#include "sensors.h"
#include "servos.h"
#include "printer.h"
#include "calibaration.h"
#include "nvs.h"
#include "profile.h"
#include "motion.h"
#include "robot.h"
#include "reporting.h"

Motors motors;
Encoders encoders;
Sensors sensors;
Servos servos;
Systick systick;
Switches switches;
Printer printer;
NVS nvs;
Calibaration calibaration;
Profile rotation;
Profile forward;
Motion motion;
Robot robot;
Reporting reporter;


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
  //printer.printCalibrationData();
  systick.enableSlowMode(true);
  motion.reset_drive_system();
  sensors.setFollowingColor(Sensors::RED);
}

void loop()
{
  // sensors.set_steering_mode(STEERING_OFF);
  
  // robot.turn_left();

  // delay(2000);

  // robot.turn_right();

  // delay(2000);

  // robot.turn_180();

  // delay(2000);
  // //sensors.set_steering_mode(STEER_NORMAL);
  
  // motion.move(1200, 200, 0, 1000);

  // delay(2000);

  // motion.move(-1200, 200, 0, 1000);

  // delay(2000);
}