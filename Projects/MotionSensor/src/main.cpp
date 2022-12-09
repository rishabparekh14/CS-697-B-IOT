#include <Arduino.h>

#define LED 25
#define PIR 13 // Add the pin you used to connect the PIR

void setup()
{
  pinMode(LED, OUTPUT); // Sets the LED as an output
  pinMode(PIR, INPUT);  // Sets the PIR as an input
}

void loop()
{
  bool motion = digitalRead(PIR); // Read the PIR
  if (motion)
    digitalWrite(LED, HIGH); // Turns on the LED
  else
    digitalWrite(LED, LOW);
}