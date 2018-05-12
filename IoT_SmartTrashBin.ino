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
//#include <MFRC522.h>

//-----------------------------------LoRa

#include "EBI.h"

EBI_LoRaWAN hEbi;

uint8_t AppSKey[] = {0x97, 0x09, 0xEF, 0x09, 0x42, 0x6F, 0xE7, 0x92, 0x8C, 0x3B, 0x51, 0xBC, 0x3A, 0x1D, 0xC6, 0xCD};
uint8_t NetSKey[] = {0x11, 0xA2, 0xC5, 0x83, 0x73, 0x80, 0x8A, 0x18, 0x0A, 0x5C, 0x39, 0x5F, 0x58, 0x6A, 0x01, 0x62};
uint8_t DevAddress[] = {0x70, 0x10, 0x3a, 0x31};

uint8_t rxBuffer[128];
long int lastTxTimestamp = 0;

//-----------------------------------LoRa

int distance = 0;
long duration= 0;
const int maxSample=100;

//3 Threshold distances in cm
int thresh[3] = {14, 50, 130};

//SENSOR
const int trigPin = 5; 
const int echoPin = 6;
int distanceArray[maxSample];

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

//ACTUATOR (LED)
int blueLed = 2;
int greenLed = 3;
int redLed = 4;

//MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


void setup()
{
  // Debug console
  Serial.begin(115200);    // Initialize serial communications with the PC

  Serial.print("Starting...");
  while(hEbi.begin()!=EBIES_Success) Serial.println("Failed");
  Serial.println("Done!");

  for(int i=0;i<16;i++) hEbi.AppSKey[i] = AppSKey[i];
  for(int i=0;i<16;i++) hEbi.NetSKey[i] = NetSKey[i];
  for(int i=0;i<4;i++) hEbi.DevAddress[i] = DevAddress[i];
  hEbi.lclass = ClassC;
  hEbi.joinMode = ABP;
  hEbi.transmitPower = 14;    //Set maximum power
  hEbi.disableAck();
  hEbi.disableDutyCycle();
  hEbi.enableADR(); 

  Serial.print("Connecting...");
  while(hEbi.connect()!=EBIES_Success){
    Serial.print("Failed...\n\tNew Attempt...");
  }  
  Serial.println("Done!");
  
 // while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
 // SPI.begin();      // Init SPI bus
 // mfrc522.PCD_Init();   // Init MFRC522
//  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
//  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  
}


//void readRFID() {
  // Select one of the cards
//  if ( ! mfrc522.PICC_ReadCardSerial()) {
//    return;
//  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
//  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
//}

int measureThrashLevel(){
  
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
 return distance;
}

int i=0;

void sendData(int avg){
  
   Serial.print("Average level: ");
   Serial.println(avg);

   Serial.println("Sending new data...");
   
    uint32_t payload = __builtin_bswap32(avg);

    Serial.print("Hexadecimal = ");
    Serial.println(payload,HEX);
    Serial.print("Binary = ");
    Serial.println(avg,BIN);

    //p1.bitSet(1,7);
    //int p2 = 544;
    //if(EBIES_Success == hEbi.sendInt(analogRead(A0),6)) {   //Send Analog Value using FPort=6
    if(EBIES_Success == hEbi.sendInt(payload, 6)) {   //Send Analog Value using FPort=6
      Serial.print("Success! (ACK RSSI = ");
      Serial.print(hEbi.getLastTxAckRSSI());
      Serial.println(" dBm)");
    } else {
      Serial.println("Failed");
    }

  //CHECK IF DL DATA IS AVAILABLE
  int dl_length = hEbi.available(1000); //Wait for 1 second for DL data
  if(dl_length>0){
     int datalength = hEbi.readBytes(rxBuffer,dl_length);
        Serial.print("Received Data: ");
        for(int i=0;i<datalength;i++){
          if(rxBuffer[i]<16) Serial.print("0");
          Serial.print(rxBuffer[i],HEX);
          Serial.print(" ");
        }
        Serial.print(" (RSSI = ");
        Serial.print(hEbi.getLastRxRSSI());
        Serial.println(" dBm)");
      }  
}


void loop()
{ 
    // Look for new cards
 // if (mfrc522.PICC_IsNewCardPresent()) {
  //   readRFID();
 // }
  distanceArray[i] = measureThrashLevel();
  i++;
  if(i==maxSample){
      long sum=0;
      for(int k=0; k<maxSample; k++){
        sum += distanceArray[k];
      }
      
      int average = sum/maxSample;
      sendData(average);
      i=0;
  }



  
 
  delay(100);
}





