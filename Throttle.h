// Throttle.h
#ifndef THROTTLE_H
#define THROTTLE_H

#include <Arduino.h>
#include <ESP32Servo.h>

class Throttle {
public:
  typedef enum {
        Forward =  0,
        Reverse = 1
    } ThrustDirection;


  Throttle(int servoPin);

  void loop();
  void set(int thrust_percent, ThrustDirection direction=Forward);
  void stop();

private:

  int THRUST_PIN;
  Servo throttleServo;
};

#endif
