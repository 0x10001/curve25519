#ifndef CURVE25519_X_H
#define CURVE25519_X_H

#include <stddef.h>
#include "types.h"

typedef struct {
  ge_pz_t cache[8];
  uint8_t pub[32];
} ed_pubkey_t;

typedef struct {
  ge_pz_t cache[8];
  uint8_t pub[32];
  uint8_t priv[32];
  uint8_t hash[64];
} ed_privkey_t;

int32_t x25519_exchange(uint8_t s[32], const uint8_t k[32], const uint8_t p[32]);
void x25519_public_from_private(uint8_t pub[32], const uint8_t priv[32]);
void ed25519_privkey_from_bytes(ed_privkey_t *key, const uint8_t a[32]);
void ed25519_sign(uint8_t sig[64], const ed_privkey_t *priv, const uint8_t *msg, size_t len);
int32_t ed25519_pubkey_from_bytes(ed_pubkey_t *key, const uint8_t a[32]);
int32_t ed25519_verify(const uint8_t sig[64], const ed_pubkey_t *pub, const uint8_t *msg, size_t len);

#endif  /* CURVE25519_X_H */
