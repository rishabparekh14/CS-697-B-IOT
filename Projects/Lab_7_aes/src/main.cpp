#include <Arduino.h>
#include "mbedtls/aes.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include <SPI.h>
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
void setup()
{
  Serial.begin(115200);
  // The maximum length the library supports a single 16 byte block of data
  unsigned char input[1024];
  char *msg = "Hello Lora World";
  memcpy(input, msg, strlen(msg));
  input[16] = '\0';
  Serial.printf("\n%s\n", input);
  // Encrypting using RSA public key
  mbedtls_pk_context pk;             // stores a public or private key
  mbedtls_ctr_drbg_context ctr_drbg; // random number generator context
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init(&entropy);
  const char *pers = "mbedtls_pk_decrypt";
  int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                  &entropy, (const unsigned char *)pers,
                                  strlen(pers));
  // Serial.printf("mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret);  // Useful to debug ret messages
  size_t olen = 0;
  unsigned char cipherRSA[MBEDTLS_MPI_MAX_SIZE];
  mbedtls_pk_init(&pk);
  ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char *)public_pem_key,
                                    strlen(public_pem_key) + 1);
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
    Serial.print("RSA Encrypted Data: ");
    for (int i = 0; i < olen; i++)
    {
      Serial.printf("%02x", (int)cipherRSA[i]);
    }
    Serial.println();
  }
  else
  {
    Serial.printf(" failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret);
  }
  // Decrypt using RSA
  unsigned char result[1024];
  mbedtls_pk_init(&pk);
  mbedtls_entropy_init(&entropy);
  ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                              &entropy, (const unsigned char *)pers,
                              strlen(pers));
  ret = mbedtls_pk_parse_key(&pk, (unsigned char *)private_pem_key,
                             strlen(private_pem_key) + 1, NULL, 0); // No password for key

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
    Serial.print("RSA Decrypted Data: ");
    result[16] = '\0';
    Serial.printf("%s", result);
  }
  else
  {
    Serial.printf(" failed\n  ! mbedtls_pk_decrypt returned -0x%04x\n", -ret);
  }
}
void loop() {}