#include "painlessMesh.h"
#include <Arduino.h>

#define LED 13
#define PIR 18

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

#define SEALEVELPRESSURE_HPA (1013.25)

// Adafruit_BME680 bme; // I2C
// Adafruit_BME680 bme(BME_CS); // hardware SPI
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

String temp;

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void sendMessage()
{
  // String msg = "ON";
  // msg += mesh.getNodeId();
  // mesh.sendBroadcast( msg );
  mesh.sendBroadcast(temp);
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

// Needed for painless library
// void receivedCallback( uint32_t from, String &msg ) {
//   Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
// }

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(F("BME680 async test"));

  if (!bme.begin())
  {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1)
      ;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE); // all types on
                                                                                                                 // mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  // mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop()
{
  // it will run the user scheduler as well
  mesh.update();
  unsigned long endTime = bme.beginReading();
  if (endTime == 0)
  {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));
  delay(50); // This represents parallel work.
  // There's no need to delay() until millis() >= endTime: bme.endReading()
  // takes care of that. It's okay for parallel work to take longer than
  // BME680's measurement time.

  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading())
  {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed at "));
  Serial.println(millis());

  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  temp = "Humidity is -->";
  temp += bme.humidity;
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print(F("Approx. Altitude = "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));

  Serial.println();
  delay(2000);
}
