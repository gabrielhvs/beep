#pragma once

#include <Arduino.h>
#include "PinChangeInterrupt.h"

//Variavel de numero de dentes do disco de leitura
const int NUMERO_DENTES = 20; //Altere se necessario ja contabilizando as reduções

class SpeedSensor {

  public:
    SpeedSensor(uint16_t pin1, uint16_t pin2);

    void countPulse1();
    void countPulse2();

    //private:
      uint16_t m_stateHalSensor[2];
      bool m_direction;
      float m_speed;
      uint16_t m_pinIn[2];
};

SpeedSensor::SpeedSensor(uint16_t pin1, uint16_t pin2) {
  m_pinIn[0] = pin1;
  m_pinIn[1] = pin2;

  pinMode(m_pinIn[0], INPUT);
  pinMode(m_pinIn[1], INPUT);
}

//Funcao de interrupcao
void SpeedSensor::countPulse2() {
  m_stateHalSensor[1] = digitalRead(m_pinIn[1]);
}

//Funcao de interrupcao
void SpeedSensor::countPulse1() {
  m_stateHalSensor[0] = digitalRead(m_pinIn[0]);
  static unsigned long timer = millis();
  double diff = (millis() - timer);
  m_speed = 1000.0/(diff*NUMERO_DENTES);
  timer = millis();
  //Verifica o sentido de rotacao do engine
  m_direction = m_stateHalSensor[0] == m_stateHalSensor[1];
}
