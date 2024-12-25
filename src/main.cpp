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
  delay(2000);
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

  //switches.enableSimulation(true); // will accept serial inputs as switch data (until switches are connected)

  // systick.enableSlowMode(false);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  // systick.enableSlowMode(true);
  // calibaration.calibrateSensors();
  // nvs.saveCalibrationData();

  nvs.loadCalibrationData();

  //printer.printCalibrationData();

  systick.enableSlowMode(false);
  //motion.reset_drive_system();
  sensors.setFollowingColor(Sensors::BLACK);
  sensors.enableUnknownToFollowing();
}

void loop() {

    utils.turnOffLED();

    robot.arrangeBox(false);
    // int switchNum = switches.switchRead();
    // Serial.println(switchNum);

    // switch (switchNum) {
    //     case 1: {
    //         helpers.go_28_align_line_after();
    //         break;
    //     }
        
    //     case 2: {
    //         helpers.turn_left();
    //         break;
    //     }
        
    //     case 3: {
    //         helpers.turn_right();
    //         break;
    //     }
        
    //     case 4: {
    //         helpers.turn_180();
    //         break;
    //     }
        
    //     case 5: {
    //         helpers.go_for_grab();
    //         break;
    //     }
        
    //     case 6: {
    //         helpers.go_33_release();
    //         break;
    //     }
        
    //     case 7: {
    //         helpers.grab(5);
    //         break;
    //     }
        
    //     case 8: {
    //         helpers.release();
    //         break;
    //     }
        
    //     case 0: {
    //         break;
    //     }
    // }

    utils.turnOnLED();
    
    delay(2500);

    // int switchNum = switches.switchRead();
    
    // uint16_t code = 0;
    // uint16_t mazePosition = 0;
    // uint16_t code2 = 0;
    // uint16_t mazePosition2 = 0;
    // bool order = false;
    // bool order2 = false;

    // switch (switchNum) {
    //     case 1: {
    //         code = robot.detectBarCode() >> 3;
    //         reporter.sendMsg(code);
    //         mazePosition = robot.maze_entrance(code);
    //         robot.indicatePosition(mazePosition);
    //         order = robot.colorLineFollowing();
    //         robot.dashLineFollowing();
    //         robot.arrangeBox(order);
    //         robot.pickUpfromCheckpoint();
    //         robot.insertChamber();
    //         robot.unevenTerrain();
    //         break;
    //     }
        
    //     case 2: {
    //         code2 = robot.detectBarCode() >> 3;
    //         reporter.sendMsg(code2);
    //         mazePosition2 = robot.maze_entrance(code2);
    //         robot.indicatePosition(mazePosition2);
    //         break;
    //     }
        
    //     case 3: {
    //         order2 = robot.colorLineFollowing();
    //         robot.dashLineFollowing();
    //         break;
    //     }
        
    //     case 4: {
    //         robot.arrangeBox(true);
    //         break;
    //     }
        
    //     case 5: {
    //         robot.pickUpfromCheckpoint();
    //         break;
    //     }
        
    //     case 6: {
    //         robot.pickObjects();
    //         break;
    //     }
        
    //     case 7: {
    //         robot.pickUpfromCheckpoint();
    //         robot.pickObjects();
    //         robot.insertChamber();
    //         break;
    //     }
        
    //     case 8: {
    //         robot.unevenTerrain();
    //         break;
    //     }
        
    //     case 0: {
    //         robot.insertChamber();
    //         break;
    //     }
        
    // }
  
}