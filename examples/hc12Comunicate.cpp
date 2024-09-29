#include <Arduino.h>

void setup() {
  Serial.begin(9600);             // Serial port to computer
  Serial2.begin(9600);               // Serial port to Serial2

}

void loop() {
  while (Serial2.available()) {        // If HC-12 has data
    Serial.write(Serial2.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    Serial2.write(Serial.read());      // Send that data to HC-12
  }
}