#include <Arduino.h>
#include <Ultrasonic.h>
//#include "PinChangeInterrupt.h"
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h> 
#include <Engine.h> 
#include <SoftI2C.h>
#include "Adafruit_APDS9960.h"
#include <SoftwareSerial.h>

// --------- Gesture Sensor ------//
Adafruit_APDS9960 apdsRight;
Adafruit_APDS9960 apdsLeft;
uint8_t diretionRight = 0;
uint8_t diretionLeft = 0;

// --------- IMU ------//
//SoftI2C SoftWire =SoftI2C(14, 15); //sda, scl
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

void calculate_IMU_error();
 
// --------- Servo Engine ------//
Servo myServo;
int servoTarget = 0.0;

//--------- Ultrassonic ---------//

// defines pins numbers
const int trigPin = 5;
const int echoPin = 4;
// defines variables
long duration;
int distance;

Ultrasonic ultrasonic(trigPin, echoPin);  //(initialize the library with connections)

//----------- ROBOT CONTROL ----------//

//declaracao dos pinos utilizados para controlar o sentido do engine
const int PINO_IN1 = 10; 
const int PINO_IN2 = 11;
const int PINO_IN3 = 12;
const int PINO_IN4 = 13;
const int PINO_PWM1 = 6;
const int PINO_PWM2 = 9;

const double RAIO_WHEEL = 0.057/2.0;

const double RAIO_ROBOT = 0.13;

const double DEAD_ZONE_ENGINE_LEFT = 130;
const double DEAD_ZONE_ENGINE_RIGHT = 110;

//Declaracao dos pinos conectados aos canais de saida do encoder
const int PINO_CH2 = 20;
const int PINO_CH1 = 7;
const int PINO_CH4 = 21;
const int PINO_CH3 = 8;

Engine engineLeft = Engine(PINO_IN1, PINO_IN2, PINO_PWM1);
Engine engineRight = Engine(PINO_IN3, PINO_IN4, PINO_PWM2);

//Variavel de target do robô
double vR = 0;
double wR = 0;
double vActual = 0;
double wActual = 0;
double Kp = 1.0;
double Ki = 0.2;
double Kd = 0.7;
double error = 0;
double sum = 0;
long int count = 0;
double P = 0;
double I = 0;
double D = 0;
double output = 0;
bool debug = false;

void interrupt1(){ engineLeft.speedSensor->countPulse1();  }
void interrupt2(){ engineLeft.speedSensor->countPulse2();  }
void interrupt3(){ engineRight.speedSensor->countPulse1(); }
void interrupt4(){ engineRight.speedSensor->countPulse2(); }

//----------------------------- LCD -------------------------------//

byte byteRead;

rgb_lcd lcd;

const int colorR = 50;
const int colorG = 50;
const int colorB = 50;

//----------------------------- HC12 -------------------------------//

String msgRecieve = "";
String msgSend = "";
SoftwareSerial HC12(3, 2); // HC-12 TX Pin, HC-12 RX Pin

void setup() {

  // ------- DEBUG ----------//  
  Serial.begin(1200);

  // ------- COMUNICATION ----------//  
  HC12.begin(1200);

  //------- ENGINES --------//
    
  //inicia o sensor com os pinos de interrupção
  engineLeft.startSpeedSendor(PINO_CH1, PINO_CH2);
  engineRight.startSpeedSendor(PINO_CH3, PINO_CH4);
  
  //inicia o codigo com os enginees parados
  engineLeft.stop();
  engineRight.stop();

 //Inicializa as interrupcoes com os pinos configurados para chamar as funcoes 
  //attachPCINT(digitalPinToPCINT(engineLeft.speedSensor->m_pinIn[0]), interrupt1, CHANGE);
  //attachPCINT(digitalPinToPCINT(engineLeft.speedSensor->m_pinIn[1]), interrupt2, CHANGE);
  //attachPCINT(digitalPinToPCINT(engineRight.speedSensor->m_pinIn[0]), interrupt3, CHANGE);
  //attachPCINT(digitalPinToPCINT(engineRight.speedSensor->m_pinIn[1]), interrupt4, CHANGE);
 
 //------- LCD --------//

  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Iniciando ...");
  delay(1000);
  lcd.clear();

  // ------- Servo ------- //

  myServo.attach(A0); 
  myServo.write(90);

  // ------- IMU ------- //

  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  delay(20);

  // ------- DIRECTION SENSOR ------- //

  if(!apdsRight.begin(10, APDS9960_AGAIN_4X, APDS9960_ADDRESS, new SoftI2C(16,15))){
    Serial.println("failed to initialize device apdsRight! Please check your wiring.");
    //gesture mode will be entered once proximity mode senses something close
    apdsRight.enableProximity(true);
    apdsRight.enableGesture(true);
    apdsRight.setProximityInterruptThreshold(0, 175);
  }
  else Serial.println("Device initialized apdsRight!");


  if(!apdsLeft.begin(10, APDS9960_AGAIN_4X, APDS9960_ADDRESS, new SoftI2C(17,15))){
    Serial.println("failed to initialize device apdsLeft! Please check your wiring.");
    //gesture mode will be entered once proximity mode senses something close
    apdsLeft.enableProximity(true);
    apdsLeft.enableGesture(true);
    apdsLeft.setProximityInterruptThreshold(0, 175);
  }
  else Serial.println("Device initialized apdsLeft!");
  delay(1000);

}

void loop() {

  // ------- COMUNICATION ----------//  
  static bool conect = false;
  static uint16_t correctPacks = 0;
  msgRecieve  = "";
  /*if(Serial.available() > 0) {   // Enquanto Serial3 receber dados
    msgRecieve = Serial.readStringUntil('\n');
    Serial.flush();
    //Serial3.println(sendData());
  }*/
  while (HC12.available()) {        // If HC-12 has data
    conect = true;
    msgRecieve += char(HC12.read());      // Send the data to Serial monitor
  }
  if(msgRecieve  != ""){
    Serial.println(msgRecieve);
    correctPacks++;
    }

  // ------- IMU ----------//
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX + GyroErrorX; // GyroErrorX ~(-0.56)
  GyroY = GyroY + GyroErrorY; // GyroErrorY ~(2)
  GyroZ = GyroZ + GyroErrorZ; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  roll = roll + GyroX * elapsedTime; // deg/s * s = deg
  pitch = pitch + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  //Serial.println(roll);
/*
  // ------- SERVO ----------//

  myServo.write(servoTarget);
*/
  // ------- ULTRASSONIC ---------//

  distance = ultrasonic.read();      //print data in cm

  // ------- LCD ---------//

  static String conecting = ".";
  if(conect){
    lcd.setCursor(0, 0);
    lcd.print("Conected");
    lcd.setCursor(0, 1);
    lcd.print(correctPacks);
  }
  else{
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Conecting" + conecting);
    conecting = conecting == "....." ? "." : conecting + ".";
    delay(100);

  }
  
  // Just visual feedback read a distance
  if(distance < 12){
    lcd.setRGB(255, 0, 0);
  }
  else if(distance < 30){
    lcd.setRGB(255, 255, 0);
  }
  else {
    lcd.setRGB(0, 255, 0);
  }

  // ------- DIRECTION SENSOR ---------//

  diretionRight = apdsRight.readGesture();
  diretionLeft = apdsLeft.readGesture();

  //---------- ROBOT CONTROL -------//

  static long int t = millis();

  vActual = (engineLeft.speedSensor->m_speed + engineRight.speedSensor->m_speed) / 2.0;
  wActual = (RAIO_ROBOT * (engineLeft.speedSensor->m_speed - engineRight.speedSensor->m_speed)) / 2.0;

  error = vR - vActual;

  count++;

  sum += error;

  P = Kp * error;

  I = Ki * (sum/count);

  D = Kd * error / (millis() - t);

  output +=  P + I + D;

  //engineLeft.run(DEAD_ZONE_ENGINE_LEFT + (output/2.0));
  //engineRight.run(DEAD_ZONE_ENGINE_RIGHT + (output/2.0));
  if(msgRecieve == "->ML255MR255SV110\n"){
    engineLeft.run(255);
    engineRight.run(255);
    Serial.println("w");
  }
  //delay(1000);
  if(msgRecieve == "->ML510MR510SV110\n"){
    engineLeft.run(0);
    engineRight.run(0);
    Serial.println("s");
  }

    if(msgRecieve == "->ML255MR510SV110\n"){
    engineLeft.run(255);
    engineRight.run(-255);
    Serial.println("a");
  }

    if(msgRecieve == "->ML510MR255SV110\n"){
    engineLeft.run(-255);
    engineRight.run(255);
    Serial.println("d");
  }


  //delay(1000);

  t = millis();

}