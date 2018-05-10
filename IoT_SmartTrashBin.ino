/*
* Ultrasonic Sensor HC-SR04 and ArduinoUNO
* 3 Led (R,G,B)
* 
* 
* Crated by:
*            Pierpaolo Gonnella, 
*                   Ossama Saki, 
*              Martin Fleischer. 
*/


//#define BLYNK_PRINT Serial

#include <SPI.h> //optional
#include <ArduinoJson.h>

int distance = 0;
long duration= 0;

//3 Threshold distances in cm
int thresh[3] = {14, 50, 130};

//SENSOR
const int trigPin = 12;
const int echoPin = 13;

//ACTUATOR (LED)
int blueLed = 9;
int greenLed = 10;
int redLed = 11;

DynamicJsonBuffer jsonBuffer;

void setup()
{
  
  // Debug console
  Serial.begin(9600);
  
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
}

void loop()
{ 

 JsonObject& root = jsonBuffer.createObject();
  
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 
 duration = pulseIn(echoPin, HIGH);
 root[F("duration")] = duration;
 
 //Distance in cm:
 distance = duration*0.034/2;
 root[F("distance")] = distance;
 
 
  Serial.println(distance);

  int level = 0;  //empty
  
   //Turn on only B led when it's over threshold 1
  if(distance<thresh[2])
  level = 1;     //not empty
  
  //Turn on only G led when it's over threshold 2
  if(distance<thresh[1])
  level = 2;     //half full
  
  //Turn on only R led when it's over threshold 3
  if(distance<thresh[0])
  level = 3;    //full
 
switch (level) {
  case 1:
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    break;
  case 2:
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    break;
  case 3:
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
    break;
    
  default:
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    break;
} 
  delay(100);
}

