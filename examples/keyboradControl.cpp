#include <Arduino.h>
#include <Wire.h>

#define m11 4

#define m12 5

#define m21 6

#define m22 7


void setup()

{

  pinMode(m11, OUTPUT);

  pinMode(m12, OUTPUT);

  pinMode(m21, OUTPUT);

  pinMode(m22, OUTPUT);

  Serial.begin(115200);
  Serial.print("here!");

}


void loop()

{

  Serial.print("here!");

  while(Serial.available())

  {

    char In=Serial.read();

    Serial.print(In);

    

    if(In=='f' || In=='F')            // Forward

    {

      digitalWrite(m11, HIGH);

      digitalWrite(m12, LOW);

      digitalWrite(m21, HIGH);

      digitalWrite(m22, LOW);

    }

    

     else if(In=='b' || In=='B')            //backward

    {

      digitalWrite(m11, LOW);

      digitalWrite(m12, HIGH);

      digitalWrite(m21, LOW);

      digitalWrite(m22, HIGH);

    }

    

     else if(In=='l' || In=='L')     // Left

    {

      digitalWrite(m11, HIGH);

      digitalWrite(m12, LOW);

      digitalWrite(m21, LOW);

      digitalWrite(m22, LOW);

    }

    

     else if(In=='r' || In=='R')     // Right

    {

      digitalWrite(m11, LOW);

      digitalWrite(m12, LOW);

      digitalWrite(m21, HIGH);

      digitalWrite(m22, LOW);

    }

    

     else if(In=='s' || In=='S')    // stop

    {

      digitalWrite(m11, LOW);

      digitalWrite(m12, LOW);

      digitalWrite(m21, LOW);

      digitalWrite(m22, LOW);

    }

    

    else

    {

      

    }

  }

}

