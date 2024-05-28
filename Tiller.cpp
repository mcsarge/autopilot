// Motor.cpp
#include <Arduino.h>
#include "Tiller.h"
#include <ESP32Servo.h>

//Setup for LinearActuator mode
Tiller::Tiller(int leftPin, int rightPin) {

  mode = Tiller::LinearActuator_Mode;  

  Serial.print("leftPin = ");
  Serial.println(leftPin);
  Serial.print("rightPin = ");
  Serial.println(rightPin);

  RPWM_PIN = rightPin;
  LPWM_PIN = leftPin;

  pinMode(RPWM_PIN, OUTPUT);
  pinMode(LPWM_PIN, OUTPUT);
  analogWrite(RPWM_PIN, 0);
  analogWrite(LPWM_PIN, 0);
  Serial.println("Tiller setup - LA Mode");
}

//Setup for Servo mode
Tiller::Tiller(int servoPin) {

  mode = Tiller::Servo_Mode;  

  Serial.print("servoPin = ");
  Serial.println(servoPin);
  
  //Timers are allocated in the calling program
	//ESP32PWM::allocateTimer(0);
	//ESP32PWM::allocateTimer(1);
	//ESP32PWM::allocateTimer(2);
	//ESP32PWM::allocateTimer(3);
  
  tillerServo.setPeriodHertz(50);  

  RPWM_PIN = servoPin;
  tillerServo.attach(servoPin, 500, 2500); //180 degree base

  Serial.println("Tiller setup - servo mode");
  
  tillerServo.write(90); //Position the servo in the center.
}

void Tiller::loop() {
  // Your motor loop logic here
  // For example, you can call motor_set and other motor-related functions here
}

void Tiller::set(int speed, TillerDirection direction) {
  //Serial.print("Tiller.set speed = ");
  //Serial.println(speed);
  //Serial.print("Tiller.set direction = ");
  //Serial.println(direction);
  
  if (speed == 0) {
    stop();
    return;
  }

   //check for the maximum speed
  if(speed > 255)
    speed = 255;

  if (mode==Tiller::LinearActuator_Mode){
    //analog write has a limit of 255. 
    //Also, DC motors only have a good speed at 50% PWM = 122. May be lower for some motors.
    
    //First check if we are over the current limmit
    //current = current*0.99+analogRead(CS)*0.01;

    if(direction == Tiller_Left){
      if(end_left || current > max_current){
        Serial.print(current);
        Serial.println(" current too high end_left reached");
        end_left = true;
        Tiller::stop();
        return;
      }

      //if we were at the right end, and now turning left  again, then end_right is false;
      if(end_right && speed>0)
        end_right = false; 

    } else { // direction = Tiller_Right

      if(end_right || current > max_current){
        Serial.println("current too high end_right reached");
        end_right = true;      
        Tiller::stop();
        return;
      }
      //if we were at the left end, and now turning right again, then end_left is false;
      if(end_left && speed>0)
        end_left = false; 

    }

    //Set the pins based on the motor direction
    analogWrite(direction == Tiller::Tiller_Right?LPWM_PIN:RPWM_PIN, 0);
    analogWrite(direction == Tiller::Tiller_Right?RPWM_PIN:LPWM_PIN, speed); 

    if(!Tiller::moving){
      Tiller::moving = true;
      Tiller::state_millis = millis();
    }

  } else { //  mode==Tiller::Tiller_Servo
    //in servo mode, the speed is the amount we should move the servo
    //so we need to map -255 to 255 against 0 to 180

    //set the value of speed to a negative or positive
    int32_t deflection = direction==Tiller::Tiller_Right?speed:-speed;
    Serial.print("Deflection  = ");
    Serial.print(deflection);

    int position = map(deflection,-255, 255, 20, 160);
    Serial.print("; Servo position = ");
    Serial.println(position);
    
    //If a damper is needed, that code should be here.


    tillerServo.write(position);

  }


}

void Tiller::stop() {
  if (mode==Tiller::LinearActuator_Mode){

    analogWrite(LPWM_PIN, 0);
    analogWrite(RPWM_PIN, 0);

    if (Tiller::moving) {
      Tiller::moving = false;
      Tiller::state_millis = millis();
    }
  }
}

// Define other functions like motor_loop, motor_set, and other helper functions as needed
