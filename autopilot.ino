/*
MIT License

Copyright (c) 2024 Pieter Oskam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software for personal use only. Any distribution to other persons, commercialy or not is prohibited.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "Tiller.h"
#include "Throttle.h"

// UUIDs for the service and characteristic (randomly generated for example)
#define AP_SERVICE_UUID   "ab0828b1-198e-4351-b779-901fa0e0371e"
#define UUID_RECEIVE      "4ac8a682-9736-4e5d-932b-e9b31405049c"

int clutch_pin = 15;

Tiller __tiller(33);
Throttle __throttle(32);

void callTiller(int speed, Tiller::TillerDirection direction){
  __tiller.set(speed, direction);  
}
void callThrust(int thrust, Throttle::ThrustDirection direction){
  __throttle.set(thrust, direction);  
}


class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        String rxString = rxValue.c_str();
        //Serial.print("Received Value: ");
        //Serial.println(rxString);

        //do not change because there is old hardware.
        if (rxString.startsWith("motor")) {
          rxString.remove(0, 5);
          int motor_speed = rxString.toInt();
          int motor_direction = 1;
          if (motor_speed < 0) {
            motor_speed *= -1;
            motor_direction = 0;
          }
          callTiller(motor_speed, motor_direction==0?Tiller::Tiller_Left:Tiller::Tiller_Right);

        } else if (rxString.startsWith("speed")){
          String throttleString = rxValue.c_str();
          //Serial.print("Received Value: ");
          //Serial.println(throttleString);
          throttleString.remove(0, 5);
          int thrust_percent = rxString.toInt();
          Throttle::ThrustDirection direction = Throttle::Forward;
          if (thrust_percent < 0) {
            thrust_percent *= -1; //back to positive
            direction = Throttle::Reverse;
          }
          callThrust(thrust_percent, direction);

        } else {
          int splitpoint = rxString.indexOf(",");
          String key = rxString.substring(0, splitpoint);
          String value = rxString.substring(splitpoint+1);


          if (key == "apOn") {
            if(value == "0") digitalWrite(clutch_pin, LOW);
            else digitalWrite(clutch_pin, HIGH);; 
          }
        }
      }
    }
};


void setup() {

  Serial.begin(115200);
 
  // Allocation of all timers for servos
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);


  // Initialize BLE
  BLEDevice::init("AutoPilot");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(AP_SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                        UUID_RECEIVE,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World");
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(AP_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

  Serial.println("setup complete"); 

  delay(1000);
}

void loop(){
  
    
} ///END LOOP


