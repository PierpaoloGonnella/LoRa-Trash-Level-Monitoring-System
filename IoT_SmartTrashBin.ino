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
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  
}

void loop()
{ 

 JsonObject& root = jsonBuffer.createObject();
  
 digitalWrite(trigPin, LOW);   //turn OFF ultrasound emitter (Trigger)
 delayMicroseconds(2);         //wait to finish 2 us
  
 digitalWrite(trigPin, HIGH);  //turn ON emitter 
 delayMicroseconds(10);        //emitter produces 8 cycle sonic burst every 10 us (800000 cycle/s)
                               //they travel at speed of sound = 340 m/s
 digitalWrite(trigPin, LOW);   //turn OFF emitter
 
 duration = pulseIn(echoPin, HIGH);  //turn ON ultrasound receiver (Echo)
                                     // OUTPUT = pulse of duration [us] proportional Round Trip Time of sound 
 root[F("duration")] = duration;    
  
 //v = 340 [m/s] = 0.034 [cm/us]
 //s = distance [cm]
 //t = duration/2 [us]
  
 distance = duration*0.034/2;     // s = t * v  
  
 root[F("distance")] = distance;
 
  Serial.println(distance);
 
  //Levels of trash 
  
  int level = 0;  //empty
 
  if(distance<thresh[2])
  level = 1;     //not empty
 
  if(distance<thresh[1])
  level = 2;     //half full
 
  if(distance<thresh[0])
  level = 3;    //full
 
switch (level) {
    
  case 1: //Turn on only B led when it's over threshold 1
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    break;
    
  case 2: //Turn on only G led when it's over threshold 2
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    break;
    
  case 3: //Turn on only R led when it's over threshold 3
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
    break;
    
  default: //Turn off all
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    break;
} 
  delay(100);
}

