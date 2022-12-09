#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// You will need an internet connection for this...
const char *ssid = "Apt247";
const char *password = "kukukamagic";
// The endpoint is broker address
const char *awsEndpoint = "a7h3r07cffmef-ats.iot.us-west-2.amazonaws.com";
// This is the arn
const char *topic = "arn:aws:iot:us-west-2:889687679673:thing/BME680";
// Right now we are also subscribing to our own publication...
// You can create another topic on AWS to subscribe to if you wish...
const char *subscribeTopic = topic;
// Update the two certificate strings below. Paste in the text of your AWS
// device certificate and private key. Add a quote character at the start
// of each line and a backslash, n, quote, space, backslash at the end
// of each line:
// xxxxxxxxxx-certificate.pem.crt
const char *certificate_pem_crt = \
    "-----BEGIN CERTIFICATE-----\n" \ 
    "MIIDWjCCAkKgAwIBAgIVAPL3j4YNPzDyTKvkrwS+LOYAWcANMA0GCSqGSIb3DQEB\n" \
    "CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
    "IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjEwMjcyMzQ1\n" \
    "MjFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
    "dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCqoyiabWywOf/oKjSJ\n" \
    "eKeeqZazhzzgpebujKbxPoiJ57wJfh8sxyRuuTCc+N2YOTwO1wEGSkzHQ74CyZSK\n" \
    "geZR1OWwjs6wgM6vylUbxj+A5ll/9wwocO9+qNuZgJiCcJsulicmyOPHPFtT4NUv\n" \
    "7nY6eVSjFjKAPaGJIarNCf9ZemlEXLyKO6fqkavcLxhCnOTcSGQCjUV1VDbSHeCv\n" \
    "Zl1UkQclUKSXMOM8Nu52f3BEhCs14X9yG8vFdVVpmDQX1AtbZGwVCsAmfzMHxWwN\n" \
    "MfgzrpgHwkSnaSu+8fgBQ8W8cxT4hK2v3cEkk0b+DcRzk1PMWJhmfNtr251xKqWg\n" \
    "Hyr9AgMBAAGjYDBeMB8GA1UdIwQYMBaAFL1nbYqjtHNda9726SPwOwbBgOX7MB0G\n" \
    "A1UdDgQWBBSYfjbM5hiYqY7TC/nfLIvW+yzKrTAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
    "DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEABBWdUtmMIFKYn1WAqjI/KxWm\n" \
    "iXYbT/Uxyj3ueI7BIVd4SHacowWmgJWjl6uolUJbSY3hWhKb2R0n8AKl2V4faO2b\n" \
    "UQtyz6jXKQbmMsAahlEvSfGLKR5xgijI4Mqv1P0XQg1tcZpNDjRlW9aC+3DJhsXV\n" \
    "Kz9/+cZG34UBN1g2QEzLG1FBM5W+laCo0bChZ+Mt2agIL3gNppQ9lG3OFMkCgICi\n" \
    "uYQipHa38/TzNhjW1E8Q+oK1CVOL2sO5jYG0jDVwhXafJSX9L+UuP2r0qpRNmfID\n" \
    "7VdmM6cfUJIAkVqGcX1q4U0+yeZP98GCkiEtV5xI3bFMcn4rQaASXWVMnyIybw==\n" \
    "-----END CERTIFICATE-----\n";
// xxxxxxxxxx-private.pem.key
const char *private_pem_key = \
    "-----BEGIN RSA PRIVATE KEY-----\n" \ 
    "MIIEpAIBAAKCAQEAqqMomm1ssDn/6Co0iXinnqmWs4c84KXm7oym8T6Iiee8CX4f\n" \
    "LMckbrkwnPjdmDk8DtcBBkpMx0O+AsmUioHmUdTlsI7OsIDOr8pVG8Y/gOZZf/cM\n" \
    "KHDvfqjbmYCYgnCbLpYnJsjjxzxbU+DVL+52OnlUoxYygD2hiSGqzQn/WXppRFy8\n" \
    "ijun6pGr3C8YQpzk3EhkAo1FdVQ20h3gr2ZdVJEHJVCklzDjPDbudn9wRIQrNeF/\n" \
    "chvLxXVVaZg0F9QLW2RsFQrAJn8zB8VsDTH4M66YB8JEp2krvvH4AUPFvHMU+ISt\n" \
    "r93BJJNG/g3Ec5NTzFiYZnzba9udcSqloB8q/QIDAQABAoIBAD8K+tRbOz28n/yR\n" \
    "o0lrB1rKQ0Be5bIF+VUHKCKXPPDeag2DyFbN2HsD9p74Hz6/Xp4I93rwdn9kWs5C\n" \
    "1Zt6tLJlGttpV5nEbRhrg5g4XXvtHwH1L0u9te4wyrli1nXSzrKejSZ6MBWLCh7q\n" \
    "QTltO8cMzqVWbSAOJSE6DBWm7O2MfuiIHLoijtu8y5Nk4KAIc0/bAGfuBufCzSIa\n" \
    "/T0pbgdYjX4H9/65mF13akL+f0eGBNUA9cSdrIFZJp0GWAaMi0EBAm+K1l9FNHYn\n" \
    "bRSt+/qED0YY0exleAxifZq7YlpEMIOeiUd+7vDwcqqGrG2vLh2RtJOWufaTiViS\n" \
    "Hrt9Tu0CgYEA0o283dIOx4jdLWqevpTWmTD+1SCf48H5/chvOyBTFX1vc9opUv9b\n" \
    "FdR5/4Kuegr6faijQ8Rle+Ff3MsOz6dHq3C3kWE1tigdsGIeEVqMCXtSBoZ54RVl\n" \
    "OYLsee2eq+27P59ZvaJs4DR933e23zCvi5OkwDNL495Na45nrqBB7a8CgYEAz3fS\n" \
    "V4sMJKBMxWgN2YFbeX/EKG64mpL4kArq/ihaaaItH2NUKxx5xoWLj6e3Lq73lCqG\n" \
    "2YkSlymE8OImgQ0FLpP8W3v7zMlSALltFb2fQQunampkP4Z57zDnWXXuGvawMQm2\n" \
    "XNtwk/gNKWjejllMOQEqUni/Km/RwY8Sm1ZOqRMCgYEAoK3TrL/p0XHrwAX2oN/6\n" \
    "qKtQPZsJCYl1WquD3u47GVJBJHsfQBxpCU69zzSOK8qHADAab9PH0nLacidv5R32\n" \
    "PqnlHIe/soQIrBskcP/VkELRBPPIvPFT70CvcBjbFr90Pwg/Ns1mIu9MA6ZbIxYH\n" \
    "UQ/aEWbUClASavsguo3H4jcCgYA8gVxlzZaABAP/umX3ngM4fuuNQyUxZ1V4ZMa9\n" \
    "0rTzg6mXMte0LvzQyde20Jna0oo/uttTC+fRD/nGap4CGvL874m0gIch5UC2F13Z\n" \
    "7lBInwZ3QYn1ANoEyQVfkMYdNP6TfpqhoBnH/ZqRDhWB/Eb/hPh7zt0pmizZP0mU\n" \
    "5enQ4wKBgQCabcXXSMZyPU9I/iDbhYlSasr9frD3raHbTY0zoa6ZrsV7ncTeTY1b\n" \
    "+Il+/t/5nBNZLt2VUHKRSwJqo/KkC8z2zACbwh5uYhf4yF739J82bgqn0gda7fYT\n" \
    "jwDAEPBBk2NaUmKXYMdcnrdBUmrbqQrH2YSmW/TeWzQzpvQPm+4JUg==\n" \
    "-----END RSA PRIVATE KEY-----\n";
// This key should be fine as is. It is just the root certificate.
const char *rootCA = \
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
  // pubSubCheckConnect();
  pubSubCheckConnect();
  if (millis() - lastPublish > 10000)
  {
    String msg = String("Delay: ") + 100;
    boolean rc = pubSubClient.publish(topic, msg.c_str());
    Serial.print("Published, rc=");
    Serial.print((rc ? "OK: " : "FAILED: "));
    Serial.println(msg);
    lastPublish = millis();
  }
  delay(2000);
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
