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
  delay(3000);
  Serial.begin(115200);

  encoders.begin();
  motors.begin();
  sensors.begin();

  sensors.enableColourSensorReadings();
  sensors.enableToFReadings();
  sensors.set_steering_mode(STEER_NORMAL);

  systick.begin();
  reporter.begin();
  servos.begin();
  utils.begin();

  //utils.turnOnEM();
  utils.turnOffLED();
  
  // switches.enableSimulation(true); // will accept serial inputs as switch data (until switches are connected)

  // systick.enableSlowMode(false);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  // systick.enableSlowMode(true);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  //nvs.loadCalibrationData();

  //printer.printCalibrationData();

  // systick.enableSlowMode(false);
  // motion.reset_drive_system();
  // sensors.setFollowingColor(Sensors::BLACK);
  // sensors.enableUnknownToFollowing();
}

void loop()
{
  // uint16_t code = robot.detectBarCode() >> 3;

  // reporter.sendMsg(code);

  // uint16_t mazePosition = robot.maze_entrance(code);

  //robot.colorLineFollowing();
  // robot.arrangeBox(true);

  robot.pickUpfromCheckpoint();

  robot.insertChamber();

  // while ((true))
  // {

  // }
  
  

}