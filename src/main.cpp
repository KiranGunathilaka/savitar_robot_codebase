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
#include "utils.h"

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
Utils utils;

Reporting *Reporting::instance = nullptr;

void setup()
{
  delay(5000);
  Serial.begin(115200);

  encoders.begin();
  motors.begin();
  sensors.begin();

  sensors.enableColourSensorReadings();
  sensors.enableToFReadings();
  sensors.set_steering_mode(STEERING_OFF);

  systick.begin();
  reporter.begin();
  servos.begin();
  utils.begin();

  utils.turnOnEM();

  // switches.enableSimulation(true); // will accept serial inputs as switch data (until switches are connected)

  // systick.enableSlowMode(false);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  //systick.enableSlowMode(true);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  // nvs.loadCalibrationData();
}

void loop()
{
  uint16_t code = robot.detectBarCode();

  reporter.sendMsg(code);
  
  uint16_t mazePosition = robot.maze_entrance(code);
  // Serial.println(code);

  // while(true){
  //   motion.reset_drive_system();
  // }
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