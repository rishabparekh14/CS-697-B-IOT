#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#include "mbedtls/aes.h"
#include "mbedtls/pk.h"
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 34

#define AES_BLOCK_SIZE 16

char *key = "acdefghijklmnopb";
unsigned char cipherText[AES_BLOCK_SIZE];

void setup()
{
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setSPI(SPI);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

void loop()
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  Serial.print(packetSize);
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet '");

    LoRa.available();
    for (int i = 0; i < AES_BLOCK_SIZE; i++)
    {
      cipherText[i] = (char)LoRa.read();
      Serial.printf("%02x", (int)cipherText[i]);
    }

    mbedtls_aes_context aes;

    // Decrypting AES Data using ecb mode
    char decryptedText[16];
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key, 128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char *)cipherText, (unsigned char *)decryptedText);
    mbedtls_aes_free(&aes);
    Serial.print("Decoded Data: ");
    Serial.print(decryptedText);
    Serial.println();
  }
}