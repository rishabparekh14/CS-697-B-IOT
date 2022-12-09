#include <Arduino.h>
#include "Adafruit_APDS9960.h"
#include <SPI.h>
Adafruit_APDS9960 apds;

void setup()
{
  Serial.begin(115200);

  if (!apds.begin())
  {
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else
    Serial.println("Device initialized!");

  // enable color sensign mode
  apds.enableColor(true);
}

void loop()
{
  // create some variables to store the color data in
  uint16_t r, g, b, c;

  // wait for color data to be ready
  while (!apds.colorDataReady())
  {
    delay(5);
  }

  // get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  Serial.print("Color is Red: ");
  Serial.print(r);
  Serial.print("   ");

  Serial.print("Color is Green: ");
  Serial.print(g);
  Serial.print("   ");

  Serial.print("Color is Blue: ");
  Serial.print(b);
  Serial.print("   ");

  Serial.print("It's clear as sky: ");
  Serial.println(c);
  Serial.println();

  delay(1000);
}