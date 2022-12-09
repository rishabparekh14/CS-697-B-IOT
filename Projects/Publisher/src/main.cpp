#include <Arduino.h>
#include "mbedtls/aes.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 34

const char *public_pem_key =
    "-----BEGIN PUBLIC KEY-----\n" \
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqqMomm1ssDn/6Co0iXin\n" \
"nqmWs4c84KXm7oym8T6Iiee8CX4fLMckbrkwnPjdmDk8DtcBBkpMx0O+AsmUioHm\n" \
"UdTlsI7OsIDOr8pVG8Y/gOZZf/cMKHDvfqjbmYCYgnCbLpYnJsjjxzxbU+DVL+52\n" \
"OnlUoxYygD2hiSGqzQn/WXppRFy8ijun6pGr3C8YQpzk3EhkAo1FdVQ20h3gr2Zd\n" \
"VJEHJVCklzDjPDbudn9wRIQrNeF/chvLxXVVaZg0F9QLW2RsFQrAJn8zB8VsDTH4\n" \
"M66YB8JEp2krvvH4AUPFvHMU+IStr93BJJNG/g3Ec5NTzFiYZnzba9udcSqloB8q\n" \
"/QIDAQAB\n" \
"-----END PUBLIC KEY-----\n";

unsigned char cipherRSA[MBEDTLS_MPI_MAX_SIZE];
String rsacipher = "Hello Lora World";

void setup()
{
  Serial.begin(9600);
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setSPI(SPI);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
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
  unsigned char input[1024];
  char *msg = "Hello Lora test";
  memcpy(input, msg, strlen(msg));
  input[15] = '\0';

  Serial.printf("\n%s\n", input);

  // Encrypting using RSA public key
  mbedtls_pk_context pk;             // stores a public or private key
  mbedtls_ctr_drbg_context ctr_drbg; // random number generator context
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  const char *pers = "decrypt";
  int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                  &entropy, (const unsigned char *)pers,
                                  strlen(pers));
  // Serial.printf("mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret);  // Useful to debug ret messages
  size_t olen = 0;
  mbedtls_pk_init(&pk);
  ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char *)public_pem_key, strlen(public_pem_key) + 1);
  ret = mbedtls_pk_encrypt(&pk,
                           input,
                           strlen(msg),
                           cipherRSA,
                           &olen,
                           sizeof(cipherRSA),
                           mbedtls_ctr_drbg_random,
                           &ctr_drbg);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
  mbedtls_pk_free(&pk);

  if (ret == 0)
  {
    Serial.println();
    Serial.print("RSA Encrypted Data: \n");
    for (int i = 0; i < olen; i++)
    {
      Serial.printf("%02x", (int)cipherRSA[i]);
      rsacipher += (char)cipherRSA[i];
    }
  }
  else
  {
    Serial.printf(" failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret);
  }
}

void loop()
{
  LoRa.beginPacket();
  LoRa.print(rsacipher.substring(0, rsacipher.length() / 2));
  LoRa.endPacket();
  delay(2000);
  LoRa.beginPacket();
  LoRa.print(rsacipher.substring(rsacipher.length() / 2));
  LoRa.endPacket();

  delay(8000);
}