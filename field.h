#ifndef CURVE25519_FIELD_H
#define CURVE25519_FIELD_H

#include "types.h"

void fe_frombytes(fe_t *e, const uint8_t s[32]);
void fe_tobytes(uint8_t s[32], const fe_t *e);
void fe_neg(fe_t *s, const fe_t *e);
void fe_add(fe_t *s, const fe_t *a, const fe_t *b);
void fe_sub(fe_t *s, const fe_t *a, const fe_t *b);
void fe_mul(fe_t *s, const fe_t *a, const fe_t *b);
void fe_sqr(fe_t *s, const fe_t *a);
void fe_mul121666(fe_t *s, const fe_t *a);
void fe_powt(fe_t *s, const fe_t *a);
void fe_inv(fe_t *s, const fe_t *a);
int8_t fe_isneg(const fe_t *e);
int8_t fe_iszero(const fe_t *e);

void fe_cmov(fe_t *s, const fe_t *e, int32_t m);
void fe_cswap(fe_t *a, fe_t *b, int32_t m);
void fe_cneg(fe_t *s, int32_t m);

#endif  /* CURVE25519_FIELD_H */
