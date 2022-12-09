#include <Arduino.h>
//#include <U8x8lib.h>
#define LED 25
#define PIR 18  // Add the pin you used to connect the PIR
#define RELAY 13 // Add the pin you used to connect the RELAY

bool on = false;

void setup()
{
  pinMode(LED, OUTPUT);   // Sets the LED as an output
  pinMode(RELAY, OUTPUT); // Sets RELAY as an output
}

void loop()
{

  if (on)
  {
    on = false;
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED, HIGH);
  }
  else
  {
    on = true;
    digitalWrite(RELAY, LOW);
    digitalWrite(LED, LOW);
  }
  delay(1000);
}