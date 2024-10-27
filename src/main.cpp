#include <Arduino.h>
#include <Ultrasonic.h>
#include "PinChangeInterrupt.h"
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h> 
#include <Engine.h> 
#include <SoftI2C.h>
#include "Adafruit_APDS9960.h"
#include <CustomChar.h>

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
const int PINO_PWM2 = 8;

const double RAIO_WHEEL = 0.057/2.0;

const double RAIO_ROBOT = 0.13;

const double DEAD_ZONE_ENGINE_LEFT = 150;
const double DEAD_ZONE_ENGINE_RIGHT = 150;

//Declaracao dos pinos conectados aos canais de saida do encoder
const int PINO_CH2 = 20;
const int PINO_CH1 = 7;
const int PINO_CH4 = 21;
const int PINO_CH3 = 9;

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
void interrupt2(){ engineRight.speedSensor->countPulse1(); }

//----------------------------- LCD -------------------------------//

byte byteRead;

rgb_lcd lcd;

const int colorR = 50;
const int colorG = 50;
const int colorB = 50;

void happyExprression();
void neutralExprression();
void centralEyesExprression();
void blinckExprression();
void leftEyeExprression();
void rightEyeExprression();

//------------------------- COLLISION ----------------------//

const int PINO_END_CURSE_1 = 2; 
const int PINO_END_CURSE_2 = 14;
const int PINO_END_CURSE_3 = 15;
const int PINO_END_CURSE_4 = 16;

void setup() {

  // ------- DEBUG ----------//  
  Serial.begin(115200);

  //------- ENGINES --------//
    
  //inicia o sensor com os pinos de interrupção
  engineLeft.startSpeedSendor(PINO_CH1, PINO_CH2);
  engineRight.startSpeedSendor(PINO_CH3, PINO_CH4);
  
  //inicia o codigo com os enginees parados
  engineLeft.stop();
  engineRight.stop();

  //Inicializa as interrupcoes com os pinos configurados para chamar as funcoes 
  attachPCINT(digitalPinToPCINT(engineLeft.speedSensor->m_pinIn[0]), interrupt1, CHANGE);
  attachPCINT(digitalPinToPCINT(engineRight.speedSensor->m_pinIn[0]), interrupt2, CHANGE);
 
 //------- LCD --------//

  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Iniciando ...");
  lcd.createCharFromProgmem(1, blockUp);
  lcd.createCharFromProgmem(2, blockDown);
  lcd.createCharFromProgmem(3, blockCenter);
  lcd.createCharFromProgmem(4, blockAll);
  neutralExprression();
  delay(1000);
  lcd.clear();

  // ------- Servo ------- //

  myServo.attach(A6); 
  myServo.write(90);

  // ------- IMU ------- //

  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  delay(20);
  calculate_IMU_error();

  //------- COLLISION --------//

  pinMode(PINO_END_CURSE_1, INPUT);
  pinMode(PINO_END_CURSE_2, INPUT);
  pinMode(PINO_END_CURSE_3, INPUT);
  pinMode(PINO_END_CURSE_4, INPUT);

}

void loop() {

  // ------- IMU ----------//
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s
  if( ( GyroX < (GyroErrorX - 1.0) ) || ( GyroX > (GyroErrorX + 1.0)) )
    roll = roll + GyroX * elapsedTime; // deg/s * s = deg

/*
  // ------- SERVO ----------//

  myServo.write(servoTarget);
*/
  // ------- ULTRASSONIC ---------//

  distance = ultrasonic.read();      //print data in cm

  // ------- LCD ---------//
  
  // Just visual feedback read a distance
  if(distance < 12){
    lcd.setRGB(255, 0, 0);
  }
  else if(distance < 30){
    lcd.setRGB(255, 255, 0);
  }
  else {
    lcd.setRGB(0, 255, 0);
    centralEyesExprression();
  }

  //---------- ROBOT CONTROL -------//

  static long int t = millis();

  Serial.print("error: ");
  Serial.print(error);
  Serial.print("|  P: ");
  Serial.print(P);
  Serial.print("|  I: ");
  Serial.print(I);
  Serial.print("|  output: ");
  Serial.print(output);

  int rollTarget = 10;
  float tolRoll = 1.0;

  //vActual = (engineLeft.speedSensor->m_speed + engineRight.speedSensor->m_speed) / 2.0;
  //wActual = (RAIO_ROBOT * (engineLeft.speedSensor->m_speed - engineRight.speedSensor->m_speed)) / 2.0;

  error = rollTarget - roll;

  if(abs(error) < tolRoll){
    error = 0;
  }

  count++;

  sum += error;

  P = Kp * error;

  I = Ki * (sum/count);

  D = Kd * error / (millis() - t);

  output =  P;// + I + D;

  int outputMotorLeft = 0;
  int outputMotorRight = 0;

  if (output < 0){
    outputMotorLeft = - (DEAD_ZONE_ENGINE_LEFT - output);
    outputMotorRight = (DEAD_ZONE_ENGINE_RIGHT - output);
  }
  else{
    outputMotorLeft = (DEAD_ZONE_ENGINE_LEFT + output);
    outputMotorRight = -(DEAD_ZONE_ENGINE_RIGHT + output);
  }

  Serial.print("|  outputMotorLeft: ");
  Serial.print(outputMotorLeft);
  Serial.print("|  outputMotorRight: ");
  Serial.println(outputMotorRight);

  //---------- COLLISION -------//

  if(digitalRead(PINO_END_CURSE_1) || 
      digitalRead(PINO_END_CURSE_2)){

    engineRight.run(-150);
    engineLeft.run(255);
    leftEyeExprression();
    delay(1000);

  } else if(digitalRead(PINO_END_CURSE_3) || 
            digitalRead(PINO_END_CURSE_4)){

    engineRight.run(255);
    engineLeft.run(-150);
    rightEyeExprression();
    delay(1000);

  }

  //---------- OUTPUT -------//

  engineRight.run( outputMotorRight);
  engineLeft.run(outputMotorLeft);

  t = millis();

}

void calculate_IMU_error(){
  float samples = 100;
  for (int i = 0; i < samples; i++){
    // === Read gyroscope data === //
    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = millis();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
    Wire.beginTransmission(MPU);
    Wire.write(0x43); // Gyro data first register address 0x43
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
    GyroErrorX += (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s
    delay(100);
  }
  GyroErrorX = GyroErrorX / samples;
}

void happyExprression(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.write(5);
  lcd.write(4);
  lcd.write(2);
  lcd.write(1);

  lcd.setCursor(3, 1);
  lcd.write(3);
  lcd.write(6);

  lcd.setCursor(1, 0);
  lcd.write(7);
  lcd.write(5);
  lcd.write(5);
  lcd.write(5);
  lcd.write(7);

  lcd.setCursor(0, 1);
  lcd.write(6);
  lcd.print("     ");
  lcd.write(6);

  lcd.setCursor(10, 0);
  lcd.write(7);
  lcd.write(5);
  lcd.write(5);
  lcd.write(5);
  lcd.write(7);

  lcd.setCursor(9, 1);
  lcd.write(6);
  lcd.print("     ");
  lcd.write(6);
}

void neutralExprression(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);

  lcd.setCursor(0, 1);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);

  lcd.setCursor(9, 0);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);
  lcd.write(7);

  lcd.setCursor(9, 1);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
}

void centralEyesExprression(){
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(3, 1);
  lcd.write(4);
  lcd.write(4);

  lcd.rightToLeft();

  lcd.setCursor(13, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(13, 1);
  lcd.write(4);
  lcd.write(4);
}

void blinckExprression(){ 
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(3, 1);
  lcd.write(4);
  lcd.write(4);

  lcd.rightToLeft();

  lcd.setCursor(13, 0);
  lcd.write(7);
  lcd.write(7);

  lcd.setCursor(13, 1);
  lcd.write(6);
  lcd.write(6);
}

void leftEyeExprression(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.write(4);

  lcd.rightToLeft();

  lcd.setCursor(11, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(11, 1);
  lcd.write(4);
  lcd.write(4);    
}

void rightEyeExprression(){
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(4, 1);
  lcd.write(4);
  lcd.write(4);

  lcd.rightToLeft();

  lcd.setCursor(14, 0);
  lcd.write(4);
  lcd.write(4);

  lcd.setCursor(14, 1);
  lcd.write(4);
  lcd.write(4);
}