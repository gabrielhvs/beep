#pragma once

#include <Arduino.h>
#include <SpeedSensor.h>

class Engine {

  public:
    void run(int dutty);
    void stop();

    void startSpeedSendor(uint16_t pinIn1, uint16_t pinIn2);

    Engine(uint16_t pinIn1, uint16_t pinIn2, uint16_t pinPWM);

    private:
      uint16_t pinIn1;
      uint16_t pinIn2;
      uint16_t pinPWM;
    public: 
      SpeedSensor *speedSensor;
};

Engine::Engine(uint16_t pinIn1, uint16_t pinIn2, uint16_t pinPWM){
  this->pinIn1 = pinIn1;
  this->pinIn2 = pinIn2;
  this->pinPWM = pinPWM;
  pinMode(pinIn1, OUTPUT);
  pinMode(pinIn1, OUTPUT);
  pinMode(pinPWM, OUTPUT);
}

void Engine::startSpeedSendor(uint16_t pinIn1, uint16_t pinIn2){
  
  speedSensor = new SpeedSensor(pinIn1, pinIn2);

}

void Engine::run(int dutty){
  analogWrite(pinPWM, abs(dutty)); 
  if(dutty < 0){
    digitalWrite(pinIn1, LOW);
    digitalWrite(pinIn2, HIGH);
    return;
  }
  digitalWrite(pinIn1, HIGH);
  digitalWrite(pinIn2, LOW);
}

void Engine::stop(){
  analogWrite(pinPWM, 0); 
  digitalWrite(pinIn1, LOW); 
  digitalWrite(pinIn2, LOW);
}