#include <Arduino.h>
#include "nvs.h"
#include "systick.h"
#include "config.h"
#include "switches.h"
#include "sensors.h"
#include "printer.h"
#include "calibaration.h"
#include "nvs.h"

Motors motors;
Encoders encoders;
Sensors sensors;
Systick systick;
Switches switches;
Printer printer;
NVS nvs;
Calibaration calibaration;

void setup()
{
  Serial.begin(115200);

  encoders.begin();
  motors.begin();
  //sensors.begin();
  systick.begin();

  switches.enableSimulation(true); // will accept serial inputs as switch data (until switches are connected)

  //systick.enableSlowMode(false);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  // systick.enableSlowMode(true);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  // nvs.loadCalibrationData();
  // printer.printCalibrationData();
  delay(10000);
  calibaration.runMotorCalibration();
}

void loop()
{
  // motors.set_right_front_motor_percentage(-50);
  // motors.set_left_back_motor_percentage(50);
  // motors.set_right_back_motor_percentage(-50);
  // motors.set_left_front_motor_percentage(50);
  // delay(2000);

  // motors.set_right_front_motor_percentage(0);
  // motors.set_left_back_motor_percentage(-0);
  // motors.set_right_back_motor_percentage(0);
  // motors.set_left_front_motor_percentage(-0);
  // delay(1000);

  // motors.set_right_front_motor_percentage(50);
  // motors.set_left_back_motor_percentage(-50);
  // motors.set_right_back_motor_percentage(50);
  // motors.set_left_front_motor_percentage(-50);
  // delay(2000);

  // motors.set_right_front_motor_percentage(0);
  // motors.set_left_back_motor_percentage(-0);
  // motors.set_right_back_motor_percentage(0);
  // motors.set_left_front_motor_percentage(-0);
  // delay(1000);
}