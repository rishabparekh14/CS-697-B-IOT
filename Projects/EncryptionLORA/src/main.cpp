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

// For 128 bit key, the key must be exactly 16 bytes
char *key = "acdefghijklmnopb";
unsigned char cipherText[AES_BLOCK_SIZE];

String aesCipherText = "";

void setup()
{
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setSPI(SPI);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // The maximum length the library supports a single 16 byte block of data
  char *input = "Hello Lora World";

  mbedtls_aes_context aes;

  // Encrypting Data using AES
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key, strlen(key) * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char *)input, cipherText);
  mbedtls_aes_free(&aes);

  Serial.println();
  Serial.print("Encrypted Data: ");
  for (int i = 0; i < AES_BLOCK_SIZE; i++)
  {
    // Print each byte as a hexadecimal
    // string with two characters padded
    // with a leading zero if needed
    Serial.printf("%02x", (int)cipherText[i]);
    aesCipherText += (char)cipherText[i];
  }
  Serial.println();

  // Decrypting AES Data using ecb mode
  char decryptedText[17];
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key, 128);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char *)cipherText, (unsigned char *)decryptedText);
  mbedtls_aes_free(&aes);
  Serial.print("Decoded Data: ");
  Serial.print(decryptedText);
  Serial.println();
}

void loop()
{
  LoRa.beginPacket();
  LoRa.println(aesCipherText);
  LoRa.endPacket();
  delay(5000);
}