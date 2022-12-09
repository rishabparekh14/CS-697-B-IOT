#include <Arduino.h>
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 34

#define GPSSerial Serial2
Adafruit_GPS GPS(&GPSSerial);
#define SOIL 36

const int AirValue = 3600;
const int WaterValue = 2700;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;
String mositurePercentage = "";

int counter = 0;

void setup()
{
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setSPI(SPI);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  Serial.println("Init done...");
}

void computeMoisture()
{
  soilMoistureValue = analogRead(SOIL);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if (soilmoisturepercent >= 100)
  {
    mositurePercentage = "";
    mositurePercentage += "100";
  }
  else if (soilmoisturepercent <= 0)
  {
    mositurePercentage = "";
    mositurePercentage += "0";
  }
  else if (soilmoisturepercent > 0 && soilmoisturepercent < 100)
  {
    mositurePercentage = "";
    mositurePercentage += soilmoisturepercent;
    mositurePercentage += '%';
  }
}

void loop()
{
  // computeMoisture();
  if (GPSSerial.available())
  {
    GPS.read();
    if (GPS.newNMEAreceived())
    {
      GPS.parse(GPS.lastNMEA());
      Serial.println(GPS.lastNMEA());
      Serial.print("\nTime: ");
      Serial.print(GPS.hour, DEC);
      Serial.print(':');
      Serial.print(GPS.minute, DEC);
      Serial.print(':');
      Serial.print(GPS.seconds, DEC);
      Serial.print('.');
      Serial.println(GPS.milliseconds);
      Serial.print("Date: ");
      Serial.print(GPS.day, DEC);
      Serial.print('/');
      Serial.print(GPS.month, DEC);
      Serial.print("/20");
      Serial.println(GPS.year, DEC);
      Serial.print("Fix: ");
      Serial.print((int)GPS.fix);

      Serial.print(" quality: ");
      Serial.println((int)GPS.fixquality);
      if (GPS.fix)
      {
        LoRa.beginPacket();
        LoRa.print("lat: ");
        LoRa.print(GPS.latitude, 4);
        LoRa.print(GPS.lat);
        LoRa.print(", ");
        LoRa.print("long: ");
        LoRa.print(GPS.longitude, 4);
        LoRa.println(GPS.lon);
        LoRa.print("moistPerc: ");
        LoRa.print(mositurePercentage);
        LoRa.endPacket();
        delay(5000);
        Serial.print("packet sent");
      }
    }
  }
  else
  {
    Serial.print("here");
  }
}