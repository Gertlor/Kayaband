#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;

// LoRa
#define ss 5
#define rst 14
#define dio0 2

// SENSORS
#define blueLed 21
#define whiteLed 22
#define lightSensor 4
#define buzzerPin 27

bool lightSensorOff = true;
bool blueLedOff = true;
int freq = 2000;
int channel = 0;
int resolution = 8;
int lightValue;


double latitude;
double longitude;
int timeH;
int timeM;
int timeS;
int dateD;
int dateM;
int dateY;
double speedKmh;
int avSat;


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  
  // SENSORS
  pinMode(blueLed, OUTPUT);
  pinMode(whiteLed, OUTPUT);
  
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(buzzerPin, channel);
  
  while (!Serial);
  Serial.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
  
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xD5);
  Serial.println("LoRa Initializing OK!");

}

void loop() {
  
  if(Serial2.available() > 0){
    gps.encode(Serial2.read());
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    timeH = gps.time.hour();
    timeM = gps.time.minute();
    timeS = gps.time.second();
    dateD = gps.date.day();
    dateM = gps.date.month();
    dateY = gps.date.year();
    speedKmh = gps.speed.kmph();
    avSat = gps.satellites.value();
  }
  
  // try to parse packet
  int packetSize = LoRa.parsePacket();  
  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");
    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      if(LoRaData == "gpsOn"){
        Serial.println("GPS");
        delay(500);
        Serial.println("Sending GPS Data");
        LoRa.beginPacket();
        LoRa.print(String(dateD));LoRa.print(":");LoRa.print(String(dateM));LoRa.print(":");LoRa.print(String(dateY));LoRa.print(":");
        LoRa.print(String(timeH));LoRa.print(":");LoRa.print(String(timeM));LoRa.print(":");LoRa.print(String(timeS));LoRa.print(":");
        LoRa.print(String(latitude, 6));LoRa.print(":");LoRa.print(String(longitude, 6));LoRa.print(":");
        LoRa.print(String(speedKmh));LoRa.print(":");
        LoRa.print(String(avSat));
        LoRa.endPacket();
        
      } else if(LoRaData == "gpsOff"){
        Serial.println("GPS OFF");
      } else if(LoRaData == "blueLedOn" && blueLedOff == true){
        Serial.println("BLUE LED ON");
        blueLedOff = false;
        
      } else if(LoRaData == "blueLedOff"){
        blueLedOff = true;
        Serial.println("BLUE LED OFF");
        
      } else if(LoRaData == "whiteLedOff"){
        Serial.println("LIGHTSENSOR OFF");
        lightSensorOff = true;
        
      } else if(LoRaData == "whiteLedOn" && lightSensorOff == true){
        lightSensorOff = false;
        Serial.println("LIGHTSENSOR ON");
      }
      else if(LoRaData == "buzzerOn"){
        Serial.println("BUZZER ON");
        ledcWriteTone(channel, 2000);
        
      } else if(LoRaData == "buzzerOff"){
        Serial.println("BUZZER OFF");
        ledcWriteTone(channel, 0);
        
      } else{
        Serial.println();
        Serial.println("Something unknown received: ");
        Serial.println(LoRaData);
      }  
  }
 }

 if(!lightSensorOff){
  lightValue = analogRead(lightSensor);

    if(lightValue <= 2900){
      digitalWrite(whiteLed, HIGH);
    } else if(lightValue > 3000) {
      digitalWrite(whiteLed, LOW);
    }
  }else {
    digitalWrite(whiteLed, LOW);
  }

  if(!blueLedOff){
    digitalWrite(blueLed, HIGH);
  } else {
    digitalWrite(blueLed, LOW);
  }
 
}
