
#include <stdlib.h>
#include<dht.h>
#define DHT11_PIN A0
#include <SoftwareSerial.h>
#include "SPI.h"
#include "MFRC522.h"
#define SS_PIN 10
#define RST_PIN 9
#define SP_PIN 8

SoftwareSerial bluetooth(2,3); // RX, TX
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
SoftwareSerial ser(0,1);  // rx,tx wifi
String apiKey = "K78D2VKV644PKZOU"; //thingspeak api key


int count=0;
char bt;

int relay1=7;
int relay2=8;
int pir=6;

dht DHT;

// if you require to change the pin number, Edit the pin with your arduino pin.

void setup() {
  // put your setup code here, to run once:
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(pir,INPUT);
  Serial.begin(9600);
  ser.begin(115200);
  bluetooth.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);
  digitalWrite(8,HIGH);
  
  digitalWrite(7,HIGH);
}

void loop() {
 
    rfid_read();
    temperature();
    bluetooth_read();

}

void temperature()
{
  count++;
  Serial.println(count);
  int chk = DHT.read11(DHT11_PIN);
  //Serial.println(" Humidity " );
  //Serial.println(DHT.humidity, 1);
  Serial.println(" Temparature ");
  Serial.println(DHT.temperature, 1);
  int temp=DHT.temperature;
  //Serial.println(temp);
  if(temp>24 && digitalRead(pir)==HIGH)
  {
    digitalWrite(relay1,LOW);
    Serial.println("High Temp and PIR");
    delay(40);
  }
  else
  {
    digitalWrite(relay1,HIGH);
    Serial.println("Low Temp or no PIR");
    delay(40);
  }
  delay(500);
  if(count>50)
  sendToThingSpeak(temp);
}

void sendToThingSpeak(int temp1)
{
  count=0;
  int strmq135SensedValue = temp1;

  Serial.println(strmq135SensedValue);

  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);

  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }

  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&temp=";
  getStr += String(strmq135SensedValue);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);

  if(ser.find('>')){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }

  // thingspeak needs 15 sec delay between updates
  delay(15000);  
}






void bluetooth_read()
{
    
    if (bluetooth.available()>0)
   {
     bt=bluetooth.read();
     Serial.println(bt);
     
     if(bt=='a' || DHT.temperature>30.0)
     {
      ll:
      Serial.println("Manual override");
      digitalWrite(relay2,LOW);
      delay(100);
      while(1)
      if(bluetooth.read()=='b')
      goto jk;
     
     }
     
     if(bt=='b')
     {
      jk:
      Serial.println("Manual override off");
      digitalWrite(relay2,HIGH);
      }
  }
}






void rfid_read()
{
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  return;
 
  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(("Your tag is not of type MIFARE Classic."));
    return;
  }

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
  
  if(strID=="EB:FE:EF:66") //RFID card number
  {
    Serial.println(" "Your name goes here" entered the house");
    digitalWrite(A4,HIGH);
    delay(1000);
    digitalWrite(A4,LOW);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  if(strID=="B0:D7:C1:80") //RFID card number
  {
    Serial.println(" "Your name goes here" enterted the house");
    digitalWrite(A1,HIGH);
    delay(1000);
    digitalWrite(A1,LOW);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  if(strID=="DB:B4:EC:66") //RFID card number
  {
    Serial.println(" "Your name goes here" entered the house");
    digitalWrite(A2,HIGH);
    delay(1000);
    digitalWrite(A2,LOW);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  if(strID=="DB:43:02:99") //RFID card number
  {
    Serial.println(" "Your name goes here" entered the house");
    digitalWrite(A3,HIGH);
    delay(1000);
    digitalWrite(A3,LOW);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
