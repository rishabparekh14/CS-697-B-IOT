#include "painlessMesh.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <string>

#define MESH_PREFIX "group5Mesh"
#define MESH_PASSWORD "group5Mesh"
#define MESH_PORT 5555
const char *ssid = "group5";
const char *password = "testpassword";

WiFiServer server(80);

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;
String pirSensorReading;
String LedSensorReading;
String SoilSensorReading;

// User stub
// void sendMessage(); // Prototype so PlatformIO doesn't complain

// Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

// void sendMessage()
// {
//   String msg = "Hello from node ";
//   msg += mesh.getNodeId();
//   mesh.sendBroadcast(msg);
//   taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
// }

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{

  // Motion sensor
  // pirSensorReading = msg.c_str();
  if (msg.startsWith("LED"))
  {
    LedSensorReading = msg;
  }
  else if (msg.startsWith("Soil"))
  {
    SoilSensorReading = msg;
  }
  else
  {
    pirSensorReading = msg;
  }
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

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

  mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);
  // all types on
  // mesh.setDebugMsgTypes(CONNECTION | SYNC); // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // userScheduler.addTask(taskSendMessage);
  // taskSendMessage.enable();

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop()
{
  // it will run the user scheduler as well
  mesh.update();
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); // the connection will be closed after completion of the response
          client.println("Refresh: 5");        // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println(pirSensorReading); // Report a sensor value instead of saying hello world...
          client.println(SoilSensorReading);
          client.println(LedSensorReading);
          client.println("</html>");
          break;
        }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
  }
}