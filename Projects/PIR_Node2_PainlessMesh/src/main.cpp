#include "painlessMesh.h"
#include <Arduino.h>

#define LED 13
#define PIR 18

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

String mayo;

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void sendMessage()
{
  // String msg = "ON";
  // msg += mesh.getNodeId();
  // mesh.sendBroadcast( msg );
  mesh.sendBroadcast(mayo);
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
  Serial.begin(115200);
  pinMode(LED, OUTPUT); // Sets the LED as an output
  pinMode(PIR, INPUT);  // Sets the PIR as an input

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
  bool motion = digitalRead(PIR); // Read the PIR
  if (motion)
  {
    mayo = "LED is ON";
    digitalWrite(LED, HIGH);
  }
  // Turns on the LED
  else
  {
    mayo = "LED is OFF";
    digitalWrite(LED, LOW);
  }
}