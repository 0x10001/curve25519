#ifndef CURVE25519_GROUP_H
#define CURVE25519_GROUP_H

#include "types.h"

void ge_scmul(ge_p3_t *s, const uint8_t a[32]);
void ge_scmulsub(ge_p2_t *s, const uint8_t b[32], const uint8_t a[32], const ge_pz_t p[8]);
void ge_to_cache(ge_pz_t *s, const ge_p3_t *e);
void ge_tobytes(uint8_t s[32], const ge_p2_t *e);
int32_t ge_frombytes(ge_p3_t *e, const uint8_t s[32]);

#endif  /* CURVE25519_GROUP_H */
