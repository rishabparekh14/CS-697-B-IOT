#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
// You will need an internet connection for this...
const char *ssid = "Franklin T9 5405";
const char *password = "8a56d96b";
// The endpoint is broker address
const char *awsEndpoint = "a7h3r07cffmef-ats.iot.us-west-2.amazonaws.com";
// This is the arn
const char *topic = "arn:aws:iot:us-west-2:889687679673:thing/FinalProject";
// Right now we are also subscribing to our own publication...
// You can create another topic on AWS to subscribe to if you wish...
const char *subscribeTopic = topic;
const int relay = 25;
const int Airvalue = 620;
const int WaterValue = 310;
const int sensorPin = 36;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;
// Update the two certificate strings below. Paste in the text of your AWS
// device certificate and private key. Add a quote character at the start
// of each line and a backslash, n, quote, space, backslash at the end
// of each line:
// xxxxxxxxxx-certificate.pem.crt
const char *certificate_pem_crt =
    "-----BEGIN CERTIFICATE-----\n" \
	"MIIDWjCCAkKgAwIBAgIVAOjv8QPWHku5m+/DNYIs9DZ4ouWUMA0GCSqGSIb3DQEB\n" \
	"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
	"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjEyMDcwMzA3\n" \
	"NDhaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
	"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDbrtjGjz8tehTEQAJI\n" \
	"FUF3+AD/Rz7v4rTKgA7XtN5csb2J2dWSv2w/KM/qtQ2LEmEUi17hoLwTUp4NjqR0\n" \
	"4aUuuear3ThC7UkmNdXC7WBfcTyY8hthyP+0gO3wyJytZX/4uykAXBldNS3qt8Or\n" \
	"xmVQiYovD9DRVi97nsZebSxNM2A9uiByT++15DmENVGuYDLnCIa6jSr0TWqU61tQ\n" \
	"C2/3kCw8hxyLw3aCT4mQBjCG90MOz7mGu4mQjlze6baKDHPQx7sXb5Kj+x7X5A0s\n" \
	"ly/aUF0/lnZMLFAtS4KIpA5P3N9nQZCZG6TQG54kdYFnHDqM2QSlLeQbHwo45c54\n" \
	"iJzfAgMBAAGjYDBeMB8GA1UdIwQYMBaAFBpivR7dF04lnBvQJLLzjCNs/2EPMB0G\n" \
	"A1UdDgQWBBSSLHl5E6G6fc1Gufs2nk7erhugGzAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
	"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAiyyLHIvLAmtWdQM48HMuifGI\n" \
	"rcg1+eovX4cWnRzXNaqpWHUCG9k+PSiBrWxZko0fuouTr1d59tSSexDCgCd95hhs\n" \
	"7Smhlitifdx5nofMdSiqrCGi+t5L7HIRXR+NaGt/gwkocUGdmz9sd9kk8grEWsdr\n" \
	"dvpdxCCxYkMt1MI1IzjYTNCw/VzWlrY5ZD9iCmrZ4/UabZhEA13YSbT1poIIRinr\n" \
	"z1fxNvMmGK8Ytt1VKgsAobU8Tu1rd8XrPgF980iLsOpKKxRsq8n/aHeKB5XWJ5uH\n" \
	"ufPat+YjI9iOBdBrY5fL8qBZqfPuB2Ko2Qy8OYYuwv75tcokiADDj6RvD7Utug==\n" \
    "-----END CERTIFICATE-----\n";
// xxxxxxxxxx-private.pem.key
const char *private_pem_key =
    "-----BEGIN RSA PRIVATE KEY-----\n" \ 
    "MIIEpgIBAAKCAQEA267Yxo8/LXoUxEACSBVBd/gA/0c+7+K0yoAO17TeXLG9idnV\n" \
	"kr9sPyjP6rUNixJhFIte4aC8E1KeDY6kdOGlLrnmq904Qu1JJjXVwu1gX3E8mPIb\n" \
	"Ycj/tIDt8MicrWV/+LspAFwZXTUt6rfDq8ZlUImKLw/Q0VYve57GXm0sTTNgPbog\n" \
	"ck/vteQ5hDVRrmAy5wiGuo0q9E1qlOtbUAtv95AsPIcci8N2gk+JkAYwhvdDDs+5\n" \
	"hruJkI5c3um2igxz0Me7F2+So/se1+QNLJcv2lBdP5Z2TCxQLUuCiKQOT9zfZ0GQ\n" \
	"mRuk0BueJHWBZxw6jNkEpS3kGx8KOOXOeIic3wIDAQABAoIBAQCGfn5S2gJI2lP6\n" \
	"bmG3NeQ2CYy6vSin/rjGvS3ZG++C+oJa8hVSWYVxCZB5Vk0rVp4+0LDPG/PpNIx1\n" \
	"Kl4+/KJ/f+rcT3yRZOqCzTTIuUFmTVJXJOpkrIT4QnLqFjft5JU7d4I7qScSLGun\n" \
	"JshR0w3WB86kz179unEpkuRJjTcxL18X9WOhp2EQssGBsgmCGRUjww9DuIclYlUl\n" \
	"FftC8DYltzT5AzuBfaqraTwDSMs1ky3EzOq9EYQolxDRPIXNEwm7Yfmg5i7ezKZ2\n" \
	"1ADwxg5WRXJoVXWyE8K0RNv8NavuWCQY1gsUwvm763+zhZXUL10Ig/pL+OoJbLY9\n" \
	"pds50qh5AoGBAPvF/LwXMPpRb2FgmrfA6L5cAOvIifG4rGDeXeFcZYGYBZr8gTxj\n" \
	"fWeFNw3Vn0A0wxI+SGS6WoIuZ13CM4RG6IyFNHz+fdaVfNBT5sCPjJ+xwa7SZxGq\n" \
	"9jOZjqzmL/ucQBdYhS7q28R7Ztofp8Gq73nBNngu6bfafF7ohUbU0ub1AoGBAN9e\n" \
	"8u6x8E6YlDI4hY63ZuJGG4mSr0V4ly1H8qxh/muLQeP1IpckWuIR/fNz4YjcYz5p\n" \
	"Sipcz2BGMSuu/hV2u6fb2aZl+CyCzErbSGd57HW0CkwBEGWiaHEDzjABf2xACaqO\n" \
	"CUwuFiH3bgqmQeUp8RAxlKJfXaML72H02ZKlz0gDAoGBAJsNCuAqyBbgH9+wRpfz\n" \
	"TWIt9sM99PXP7Ria2OX4qOpuXMVdy5kNDCW8fHMgo7o+xANwJ9UgjqytNysedet4\n" \
	"X65qOjDf6U9FMGB7yxe7T7bpGYY4PRE8X/LXFm6UM+iYCUBXW23RZRZTiLu70uwK\n" \
	"AQDSZGEPQw/elck9GRbN6jQhAoGBANoo2gvXLmUVEGicW5Uv+wQ0hOCwqw6m4MAQ\n" \
	"7Sr/6oUO1aPW35nOoBT8jZMhMr2Awk1KeG03iDL3Wz7XAsHj3RPs3X5vSRvwA6d4\n" \
	"1BZ8Nj4fQY9J2sHpBSicFt08GT+Yq9gwMVR+Do/OIWO1TdaMSC0Qv6J2kfWUypBZ\n" \
	"m8ZmuzQRAoGBAOEzkfShAgvjfvKphaPstXmZ/EHshs6+Po82Mcxf46O+Idmkcj5K\n" \
	"9u3oAYukQZHAq+8CivvqEBr8kh1WT7k8D1pXmw0sCyUE2B5I+gQ2bgVOl7ddiZJW\n" \
	"FqD/XJCqJ0YGpjYnzU/+hT8MgudlsSupIQPtd0Hmy6tzj5Y5EuqaKZSL\n" \
    "-----END RSA PRIVATE KEY-----\n";
// This key should be fine as is. It is just the root certificate.
const char *rootCA =
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
	"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
	"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
	"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
	"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
	"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
	"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
	"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
	"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
	"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
	"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
	"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
	"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
	"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
	"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
	"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
	"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
	"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
    "-----END CERTIFICATE-----\n";
WiFiClientSecure wiFiClient;
void msgReceived(char *topic, byte *payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient);
void pubSubCheckConnect();
void setup()
{
  Serial.begin(115200);
  delay(50);
  pinMode(relay, OUTPUT);
  Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}
unsigned long lastPublish;
int msgCount;
void loop()
{
  delay(5000);
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
  }
  else if (soilMoisturePercent >= 0 && soilMoisturePercent <= 100)
  {
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    
  }	
  // pubSubCheckConnect();
  pubSubCheckConnect();

  if (millis() - lastPublish > 10000)
  {
    String msg = String("Moisture Percent ") + soilMoisturePercent;
    boolean rc = pubSubClient.publish(topic, msg.c_str());
    Serial.print("Published, rc=");
    Serial.print((rc ? "OK: " : "FAILED: "));
    Serial.println(msg);
    lastPublish = millis();
  }
}
void msgReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on ");
  Serial.print(topic);
  Serial.print(": ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
void pubSubCheckConnect()
{
  if (!pubSubClient.connected())
  {
    Serial.print("PubSubClient connecting to: ");
    Serial.print(awsEndpoint);
    while (!pubSubClient.connected())
    {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe(subscribeTopic);
  }
  pubSubClient.loop();
}
