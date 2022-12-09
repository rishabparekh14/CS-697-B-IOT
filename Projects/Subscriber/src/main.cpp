#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>

#include "mbedtls/aes.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 34

const char *private_pem_key =
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

unsigned char cipherRSA[MBEDTLS_MPI_MAX_SIZE];

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

int count = 0, len = 0;
void loop()
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("\nReceived packet: ");
    while (LoRa.available())
    {
      cipherRSA[len++] = (char)LoRa.read();
      // Serial.printf("%02x", (int) cipherRSA[ind++]);
    }

    count++;
    if (count == 2)
    {
      for (int i = 0; i < len; i++)
      {
        Serial.printf("%02x", (int)cipherRSA[i]);
      }
      mbedtls_pk_context pk;             // stores a public or private key
      mbedtls_ctr_drbg_context ctr_drbg; // random number generator context
      mbedtls_entropy_context entropy;
      mbedtls_entropy_init(&entropy);
      mbedtls_ctr_drbg_init(&ctr_drbg);
      const char *pers = "decrypt";
      // Decrypt using RSA
      size_t olen = 256;
      unsigned char result[1024];
      mbedtls_pk_init(&pk);
      mbedtls_entropy_init(&entropy);
      int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                      &entropy, (const unsigned char *)pers,
                                      strlen(pers));
      ret = mbedtls_pk_parse_key(&pk, (unsigned char *)private_pem_key, strlen(private_pem_key) + 1, NULL, 0); // No password for key

      int lenCipher = olen;
      ret = mbedtls_pk_decrypt(&pk,
                               cipherRSA,
                               lenCipher,
                               result,
                               &olen,
                               sizeof(result),
                               mbedtls_ctr_drbg_random,
                               &ctr_drbg);

      if (ret == 0)
      {
        Serial.print("\nRSA Decrypted Data: ");
        result[16] = '\0';
        Serial.printf("%s", result);
      }
      else
      {
        Serial.printf(" failed\n  ! mbedtls_pk_decrypt returned -0x%04x\n", -ret);
      }

      count = 0;
      len = 0;
    }
  }
}