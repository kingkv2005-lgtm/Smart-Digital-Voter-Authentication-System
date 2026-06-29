#include "crypto.h"

const uint8_t aes_key[32] = {
  0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
  0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
  0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
  0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
};

String toHexLower(const uint8_t *buf, size_t len) {
  const char hex[] = "0123456789abcdef";
  String s; s.reserve(len * 2);
  for (size_t i = 0; i < len; ++i) {
    s += hex[(buf[i] >> 4) & 0xF];
    s += hex[buf[i] & 0xF];
  }
  return s;
}

String sha256Hex(const uint8_t *data, size_t len) {
  uint8_t out[32];
  mbedtls_sha256(data, len, out, 0);
  return toHexLower(out, 32);
}

String sha256Hex(const String &s) {
  return sha256Hex((const uint8_t*)s.c_str(), s.length());
}

void randomBytes(uint8_t *buf, size_t len) {
  for (size_t i = 0; i < len; i += 4) {
    uint32_t r = esp_random();
    size_t chunk = min((size_t)4, len - i);
    memcpy(buf + i, &r, chunk);
  }
}

bool aesGcmEncrypt(const uint8_t *key, const uint8_t *plaintext, size_t plen,
                   const uint8_t *adata, size_t adlen,
                   uint8_t *out_iv, size_t iv_len,
                   uint8_t *ciphertext, uint8_t *tag, size_t tag_len) {
  if (iv_len != 12 || tag_len < 12) return false;
  mbedtls_gcm_context ctx;
  mbedtls_gcm_init(&ctx);
  int ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, 256);
  if (ret != 0) { mbedtls_gcm_free(&ctx); return false; }
  ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT,
                                  plen, out_iv, iv_len,
                                  adata, adlen, plaintext, ciphertext,
                                  tag_len, tag);
  mbedtls_gcm_free(&ctx);
  return (ret == 0);
}
