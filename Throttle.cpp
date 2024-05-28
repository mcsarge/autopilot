// Throttle.cpp
#include <Arduino.h>
#include "Throttle.h"
#include <ESP32Servo.h>


//Setup for Servo mode
Throttle::Throttle(int servoPin) {
  Serial.print("servoPin = ");
  Serial.println(servoPin);
  
  throttleServo.setPeriodHertz(50);  

  THRUST_PIN = servoPin;
  throttleServo.attach(servoPin, 500, 2500); //180 degree base

  Serial.println("Throttle setup - servo mode");
  
  stop();
}

void Throttle::loop() {
  // Your Throttle loop logic here
}

void Throttle::set(int thrust_percent, ThrustDirection direction) {
  if (thrust_percent == 0) {
    stop();
    return;
  }

   //check for the maximum thrust percent
  if(thrust_percent > 100)
    thrust_percent = 100;

  //set the value of speed to a negative or positive
  int32_t thrust = direction==Forward?thrust_percent:-thrust_percent;

  if (thrust < -90){ //Limit reverse thrust to 1/2 full speed
    thrust = -90;
    Serial.print("Thrust (limited) = ");
    Serial.print(thrust);
  } else {
    Serial.print("Thrust  = ");
    Serial.print(thrust);
  }

  //Map the thrust to a servo position to the ESC. 0=full reverse, 180=full forward, 90 = stop.
  int servo_position = map(thrust, -100, 100, 0, 180);
  Serial.print("; Servo position = ");
  Serial.println(servo_position);
    
  //If a damper is needed, that code should be here.

  throttleServo.write(servo_position);

}

void Throttle::stop() {
    //zero the thrust.
    throttleServo.write(90);
}
