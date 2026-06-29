#ifndef CRYPTO_H
#define CRYPTO_H

#include "config.h"

String toHexLower(const uint8_t *buf, size_t len);
String sha256Hex(const uint8_t *data, size_t len);
String sha256Hex(const String &s);
void randomBytes(uint8_t *buf, size_t len);
bool aesGcmEncrypt(const uint8_t *key, const uint8_t *plaintext, size_t plen,
                   const uint8_t *adata, size_t adlen,
                   uint8_t *out_iv, size_t iv_len,
                   uint8_t *ciphertext, uint8_t *tag, size_t tag_len);

#endif
