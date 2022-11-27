#ifndef CURVE25519_SCALAR_H
#define CURVE25519_SCALAR_H

#include <stdint.h>

void sc_reduce(uint8_t s[64]);
void sc_muladd(uint8_t s[32], const uint8_t a[32], const uint8_t b[32], const uint8_t c[32]);
int32_t sc_verify(const uint8_t s[32]);

#endif  /* CURVE25519_SCALAR_H */
