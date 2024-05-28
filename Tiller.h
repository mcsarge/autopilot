// Tiller.h
#ifndef TILLER_H
#define TILLER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class Tiller {
public:
  typedef enum {
        Servo_Mode =  0,
        LinearActuator_Mode = 1
    } TillerMode;

  typedef enum {
        Tiller_Left =  0,
        Tiller_Right = 1
    } TillerDirection;

  Tiller(int leftPin, int rightPin); //Linear Actuator Mode
  Tiller(int servoPin); //servo mode

  void loop();
  void set(int speed, TillerDirection direction);
  void stop();
  float current;
  int max_current;

private:
  const int CS_PIN = 35;
  int counter;
  long prevMillis;
  int startSpeed;
  int lastPosition;


  bool moving;
  unsigned long state_millis;

  int RPWM_PIN = 33;
  int LPWM_PIN = 32;

  //Servo tillerServo; 

  TillerMode mode = LinearActuator_Mode;

  bool end_left = false;
  bool end_right = false;

  Servo tillerServo;
};

#endif
