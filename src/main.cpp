#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "Ticker.h"
#include "config.h"

Motors motors;
Encoders encoders;

Ticker sendTicker;

void setup()
{
  // sets the pins as outputs:
  Serial.begin(115200);

  encoders.begin();
  motors.begin();

  sendTicker.attach(0.02, []()
                    { 
  encoders.update();
  Serial.print("Left RPS : ");
  Serial.print(encoders.leftRPS());
  Serial.print("   Right RPS : ");
  Serial.println(encoders.rightRPS()); });
}

void loop()
{
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  ledcWrite(0, 256);

  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  ledcWrite(1, 256);

  delay(3000);

  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, HIGH);
  ledcWrite(0, 256);

  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  ledcWrite(1, 256);

  delay(3000);
}