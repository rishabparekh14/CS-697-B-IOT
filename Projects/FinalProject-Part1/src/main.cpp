#include <Arduino.h>

const int relay = 25;
const int Airvalue = 620;
const int WaterValue = 310;
const int sensorPin = 36;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
}

void loop()
{
  // Normally Open configuration, send LOW signal to let current flow
  // (if you're usong Normally Closed configuration send HIGH signal)
  soilMoistureValue = analogRead(sensorPin);
  Serial.println("SoilMositureValue:");
  Serial.println(soilMoistureValue);
  soilMoisturePercent = map(soilMoistureValue, Airvalue, WaterValue, 0, 100);
  Serial.println("SoilMositurePercent:");
  Serial.println(soilMoisturePercent);

  if (soilMoisturePercent > 100)
  {
    digitalWrite(relay, HIGH);
    Serial.println("Current not Flowing");
    delay(5000);
  }
  else if (soilMoisturePercent >= 0 && soilMoisturePercent <= 100)
  {
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    delay(5000);
  }

  // Normally Open configuration, send HIGH signal stop current flow
  // (if you're usong Normally Closed configuration send LOW signal)
}