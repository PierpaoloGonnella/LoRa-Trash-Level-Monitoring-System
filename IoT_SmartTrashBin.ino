/*
 *     ,·'´¨;.  '                                   ,.,   '                   ,. -,                 ,. -,          ,.-·.         ,.-.                                 _,.,  °    
    ;   ';:\           .·´¨';\                 ;´   '· .,            ,.·'´,    ,'\          ,.·'´,    ,'\        /    ;'\'      /   ';\ '                       ,.·'´  ,. ,  `;\ '  
   ;     ';:'\      .'´     ;:'\              .´  .-,    ';\      ,·'´ .·´'´-·'´::::\'    ,·'´ .·´'´-·'´::::\'     ;    ;:::\    ';    ;:'\      ,·'´';          .´   ;´:::::\`'´ \'\  
   ;   ,  '·:;  .·´,.´';  ,'::;'             /   /:\:';   ;:'\'   ;    ';:::\::\::;:'    ;    ';:::\::\::;:'     ';    ;::::;'    ';   ;::;     ,'  ,''\        /   ,'::\::::::\:::\:' 
  ;   ;'`.    ¨,.·´::;'  ;:::;            ,'  ,'::::'\';  ;::';   \·.    `·;:'-·'´       \·.    `·;:'-·'´         ;   ;::::;     ';   ';::;   ,'  ,':::'\'     ;   ;:;:-·'~^ª*';\'´   
  ;  ';::; \*´\:::::;  ,':::;        ,.-·'  '·~^*'´¨,  ';::;    \:`·.   '`·,  '        \:`·.   '`·,  '        ';  ;'::::;       ';   ;:;  ,'  ,':::::;'     ;  ,.-·:*'´¨'`*´\::\ '  
 ';  ,'::;   \::\;:·';  ;:::; '        ':,  ,·:²*´¨¯'`;  ;::';      `·:'`·,   \'           `·:'`·,   \'         ;  ';:::';         ;   ;:;'´ ,'::::::;'  '   ;   ;\::::::::::::'\;'   
 ;  ';::;     '*´  ;',·':::;          ,'  / \::::::::';  ;::';       ,.'-:;'  ,·\           ,.'-:;'  ,·\        ';  ;::::;'        ';   '´ ,·':::::;'        ;  ;'_\_:;:: -·^*';\   
 \´¨\::;          \¨\::::;          ,' ,'::::\·²*'´¨¯':,'\:;   ,·'´     ,.·´:::'\     ,·'´     ,.·´:::'\        \*´\:::;         ,'   ,.'\::;·´          ';    ,  ,. -·:*'´:\:'\° 
  '\::\;            \:\;·'           \`¨\:::/          \::\'    \`*'´\::::::::;·'     \`*'´\::::::::;·'        '\::\:;'          \`*´\:::\;             \`*´ ¯\:::::::::::\;' '
    '´¨               ¨'              '\::\;'            '\;'  '   \::::\:;:·´           \::::\:;:·´               `*´            '\:::\;'                  \:::::\;::-·^*'´     
                                       `¨'                        '`*'´                 '`*'´                                     `*´                     `*´¯  
 *  
 *        .-.      .-----.                  .-.    .--.              
 *        : :      `-. .-'                  : :   : .--'             
 *        : : .--.   : :  .--.  .--.   .--. : `-. : :    .--.  ,-.,-.
 *        : :' .; :  : :  : ..'' .; ; `._-.': .. :: :__ ' .; ; : ,. :
 *        :_;`.__.'  :_;  :_;  `.__,_;`.__.':_;:_;`.__.'`.__,_;:_;:_;
 *                                                           
 *                                                       
 * 
 * COMPONENTS:
 * 
 * +ArduinoUNO
 * +Ultrasonic Sensor HC-SR04 
 * +Led G
 * +Led B
 * +Led R
 * +RFID module MFRC522
 *
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Crated by:
 *            Pierpaolo Gonnella, 
 *                   Ossama Saki, 
 *              Martin Fleischer. 
 *              
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------                     
 */


//#define BLYNK_PRINT Serial

#include <SPI.h> //optional
#include <ArduinoJson.h>
#include <MFRC522.h>

int distance = 0;
long duration= 0;

//3 Threshold distances in cm
int thresh[3] = {14, 50, 130};

//SENSOR
const int trigPin = 5; 
const int echoPin = 6;

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

//ACTUATOR (LED)
int blueLed = 2;
int greenLed = 3;
int redLed = 4;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

DynamicJsonBuffer jsonBuffer;

void setup()
{
  // Debug console
  Serial.begin(9600);    // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  
}


void readRFID() {
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
}

void measureThrashLevel(){
  
  digitalWrite(trigPin, LOW);   //turn OFF ultrasound emitter (Trigger)
 delayMicroseconds(2);         //wait to finish 2 us
  
 digitalWrite(trigPin, HIGH);  //turn ON emitter 
 delayMicroseconds(10);        //emitter produces 8 cycle sonic burst every 10 us (800000 cycle/s)
                               //they travel at speed of sound = 340 m/s
 digitalWrite(trigPin, LOW);   //turn OFF emitter
 
 duration = pulseIn(echoPin, HIGH);  //turn ON ultrasound receiver (Echo)
                                     // OUTPUT = pulse of duration [us] proportional Round Trip Time of sound 
 //root[F("duration")] = duration;    
  
 //v = 340 [m/s] = 0.034 [cm/us]
 //s = distance [cm]
 //t = duration/2 [us] (Round Trip Time/2)
  
 distance = duration*0.034/2;     // s = t * v  
  
// root[F("distance")] = distance;
 
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
    
  case 1: //Turn ON only B led when it's over threshold 1
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    break;
    
  case 2: //Turn ON only G led when it's over threshold 2
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    break;
    
  case 3: //Turn ON only R led when it's over threshold 3
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
    break;
    
  default: //Turn OFF all
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    break;
} 
}

void loop()
{ 
    // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()) {
     readRFID();
  }
  measureThrashLevel();

 JsonObject& root = jsonBuffer.createObject();
  
 
  delay(100);
}

