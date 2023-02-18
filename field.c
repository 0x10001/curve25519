#include "field.h"
#include "zint.h"

static const int32_t k_l25 = (1 << 25) - 1;
static const int32_t k_l26 = (1 << 26) - 1;

static inline void reduce(fe_t *s, int64_t r[10]) {
  int64_t c;

  r[9] += r[8] >> 26; r[8] &= k_l26;

  c = (r[9] + (1 << 24)) >> 25;
  r[0] += c * 19; r[9] -= c << 25;

  r[1] += r[0] >> 26;
  r[2] += r[1] >> 25;
  r[3] += r[2] >> 26;
  r[4] += r[3] >> 25;
  r[5] += r[4] >> 26;
  r[6] += r[5] >> 25;
  r[7] += r[6] >> 26;
  r[8] += r[7] >> 25;

  s->d[0] = r[0] & k_l26;
  s->d[1] = r[1] & k_l25;
  s->d[2] = r[2] & k_l26;
  s->d[3] = r[3] & k_l25;
  s->d[4] = r[4] & k_l26;
  s->d[5] = r[5] & k_l25;
  s->d[6] = r[6] & k_l26;
  s->d[7] = r[7] & k_l25;
  s->d[8] = r[8] & k_l26;
  s->d[9] = r[9] + (r[8] >> 26);
}

void fe_frombytes(fe_t *e, const uint8_t s[32]) {
  e->d[0] = loadle32(s) & k_l26;
  e->d[1] = (loadle32(s + 3) >> 2) & k_l25;
  e->d[2] = (loadle32(s + 6) >> 3) & k_l26;
  e->d[3] = (loadle32(s + 9) >> 5) & k_l25;
  e->d[4] = (loadle32(s + 12) >> 6) & k_l26;
  e->d[5] = (loadle32(s + 16) >> 0) & k_l25;
  e->d[6] = (loadle32(s + 19) >> 1) & k_l26;
  e->d[7] = (loadle32(s + 22) >> 3) & k_l25;
  e->d[8] = (loadle32(s + 25) >> 4) & k_l26;
  e->d[9] = (loadle32(s + 28) >> 6) & k_l25;
}

static inline int32_t div(const fe_t *e) {
  int32_t q;
  q = (e->d[9] * 19 + (1 << 24)) >> 25;
  q = (e->d[0] + q) >> 26;
  q = (e->d[1] + q) >> 25;
  q = (e->d[2] + q) >> 26;
  q = (e->d[3] + q) >> 25;
  q = (e->d[4] + q) >> 26;
  q = (e->d[5] + q) >> 25;
  q = (e->d[6] + q) >> 26;
  q = (e->d[7] + q) >> 25;
  q = (e->d[8] + q) >> 26;
  q = (e->d[9] + q) >> 25;
  return q;
}

void fe_tobytes(uint8_t s[32], const fe_t *e) {
  int32_t e0 = e->d[0];
  int32_t e1 = e->d[1];
  int32_t e2 = e->d[2];
  int32_t e3 = e->d[3];
  int32_t e4 = e->d[4];
  int32_t e5 = e->d[5];
  int32_t e6 = e->d[6];
  int32_t e7 = e->d[7];
  int32_t e8 = e->d[8];
  int32_t e9 = e->d[9];

  e0 += div(e) * 19;
  e1 += e0 >> 26; e0 &= k_l26;
  e2 += e1 >> 25; e1 &= k_l25;
  e3 += e2 >> 26; e2 &= k_l26;
  e4 += e3 >> 25; e3 &= k_l25;
  e5 += e4 >> 26; e4 &= k_l26;
  e6 += e5 >> 25; e5 &= k_l25;
  e7 += e6 >> 26; e6 &= k_l26;
  e8 += e7 >> 25; e7 &= k_l25;
  e9 += e8 >> 26; e8 &= k_l26;
  e9 &= k_l25;

  dumple32(s, (e0) | (e1 << 26));
  dumple32(s + 4, (e1 >> 6) | (e2 << 19));
  dumple32(s + 8, (e2 >> 13) | (e3 << 13));
  dumple32(s + 12, (e3 >> 19) | (e4 << 6));
  dumple32(s + 16, (e5) | (e6 << 25));
  dumple32(s + 20, (e6 >> 7) | (e7 << 19));
  dumple32(s + 24, (e7 >> 13) | (e8 << 12));
  dumple32(s + 28, (e8 >> 20) | (e9 << 6));
}

void fe_neg(fe_t *s, const fe_t *e) {
  s->d[0] = -e->d[0];
  s->d[1] = -e->d[1];
  s->d[2] = -e->d[2];
  s->d[3] = -e->d[3];
  s->d[4] = -e->d[4];
  s->d[5] = -e->d[5];
  s->d[6] = -e->d[6];
  s->d[7] = -e->d[7];
  s->d[8] = -e->d[8];
  s->d[9] = -e->d[9];
}

void fe_add(fe_t *s, const fe_t *a, const fe_t *b) {
  s->d[0] = a->d[0] + b->d[0];
  s->d[1] = a->d[1] + b->d[1];
  s->d[2] = a->d[2] + b->d[2];
  s->d[3] = a->d[3] + b->d[3];
  s->d[4] = a->d[4] + b->d[4];
  s->d[5] = a->d[5] + b->d[5];
  s->d[6] = a->d[6] + b->d[6];
  s->d[7] = a->d[7] + b->d[7];
  s->d[8] = a->d[8] + b->d[8];
  s->d[9] = a->d[9] + b->d[9];
}

void fe_sub(fe_t *s, const fe_t *a, const fe_t *b) {
  s->d[0] = a->d[0] - b->d[0];
  s->d[1] = a->d[1] - b->d[1];
  s->d[2] = a->d[2] - b->d[2];
  s->d[3] = a->d[3] - b->d[3];
  s->d[4] = a->d[4] - b->d[4];
  s->d[5] = a->d[5] - b->d[5];
  s->d[6] = a->d[6] - b->d[6];
  s->d[7] = a->d[7] - b->d[7];
  s->d[8] = a->d[8] - b->d[8];
  s->d[9] = a->d[9] - b->d[9];
}

void fe_mul(fe_t *s, const fe_t *a, const fe_t *b) {
  int64_t a0 = a->d[0];
  int64_t a1 = a->d[1];
  int64_t a2 = a->d[2];
  int64_t a3 = a->d[3];
  int64_t a4 = a->d[4];
  int64_t a5 = a->d[5];
  int64_t a6 = a->d[6];
  int64_t a7 = a->d[7];
  int64_t a8 = a->d[8];
  int64_t a9 = a->d[9];
  int64_t b0 = b->d[0];
  int64_t b1 = b->d[1];
  int64_t b2 = b->d[2];
  int64_t b3 = b->d[3];
  int64_t b4 = b->d[4];
  int64_t b5 = b->d[5];
  int64_t b6 = b->d[6];
  int64_t b7 = b->d[7];
  int64_t b8 = b->d[8];
  int64_t b9 = b->d[9];

  int64_t r[10] = {
    a0 * b0 + ((a1 * b9 + a3 * b7 + a5 * b5 + a7 * b3 + a9 * b1) * 2 + a2 * b8 + a4 * b6 + a6 * b4 + a8 * b2) * 19,
    a0 * b1 + a1 * b0 + (a2 * b9 + a3 * b8 + a4 * b7 + a5 * b6 + a6 * b5 + a7 * b4 + a8 * b3 + a9 * b2) * 19,
    a0 * b2 + a2 * b0 + a1 * b1 * 2 + ((a3 * b9 + a5 * b7 + a7 * b5 + a9 * b3) * 2 + a4 * b8 + a6 * b6 + a8 * b4) * 19,
    a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0 + (a4 * b9 + a5 * b8 + a6 * b7 + a7 * b6 + a8 * b5 + a9 * b4) * 19,
    a0 * b4 + a2 * b2 + a4 * b0 + (a1 * b3 + a3 * b1) * 2 + ((a5 * b9 + a7 * b7 + a9 * b5) * 2 + a6 * b8 + a8 * b6) * 19,
    a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0 + (a6 * b9 + a7 * b8 + a8 * b7 + a9 * b6) * 19,
    a0 * b6 + a2 * b4 + a4 * b2 + a6 * b0 + (a1 * b5 + a3 * b3 + a5 * b1) * 2 + ((a7 * b9 + a9 * b7) * 2 + a8 * b8) * 19,
    a0 * b7 + a1 * b6 + a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2 + a6 * b1 + a7 * b0 + (a8 * b9 + a9 * b8) * 19,
    a0 * b8 + a2 * b6 + a4 * b4 + a6 * b2 + a8 * b0 + (a1 * b7 + a3 * b5 + a5 * b3 + a7 * b1 + a9 * b9 * 19) * 2,
    a0 * b9 + a1 * b8 + a2 * b7 + a3 * b6 + a4 * b5 + a5 * b4 + a6 * b3 + a7 * b2 + a8 * b1 + a9 * b0,
  };
  
  reduce(s, r);
}

void fe_sqr(fe_t *s, const fe_t *a) {
  int64_t a0 = a->d[0];
  int64_t a1 = a->d[1];
  int64_t a2 = a->d[2];
  int64_t a3 = a->d[3];
  int64_t a4 = a->d[4];
  int64_t a5 = a->d[5];
  int64_t a6 = a->d[6];
  int64_t a7 = a->d[7];
  int64_t a8 = a->d[8];
  int64_t a9 = a->d[9];

  int64_t r[10] = {
    a0 * a0 + ((a1 * a9 + a3 * a7) * 2 + a5 * a5 + a2 * a8 + a4 * a6) * 38,
    (a0 * a1 + (a2 * a9 + a3 * a8 + a4 * a7 + a5 * a6) * 19) * 2,
    (a0 * a2 + a1 * a1) * 2 + (((a3 * a9 + a5 * a7) * 2 + a4 * a8) * 2 + a6 * a6) * 19,
    (a0 * a3 + a1 * a2 + (a4 * a9 + a5 * a8 + a6 * a7) * 19) * 2,
    a2 * a2 + ((a0 * a4 + a1 * a3 * 2) + (a5 * a9 * 2 + a7 * a7 + a6 * a8) * 19) * 2,
    (a0 * a5 + a1 * a4 + a2 * a3 + (a6 * a9 + a7 * a8) * 19) * 2,
    (a0 * a6 + a2 * a4 + a1 * a5 * 2 + a3 * a3) * 2 + (a7 * a9 * 4 + a8 * a8) * 19,
    (a0 * a7 + a1 * a6 + a2 * a5 + a3 * a4 + a8 * a9 * 19) * 2,
    a4 * a4 + (a0 * a8 + a2 * a6 + (a1 * a7 + a3 * a5) * 2 + a9 * a9 * 19) * 2,
    (a0 * a9 + a1 * a8 + a2 * a7 + a3 * a6 + a4 * a5) * 2,
  };

  reduce(s, r);
}

void fe_mul121666(fe_t *s, const fe_t *a) {
  int64_t r[10] = {
    121666,
    121666,
    121666,
    121666,
    121666,
    121666,
    121666,
    121666,
    121666,
    121666,
  };

  r[0] *= a->d[0];
  r[1] *= a->d[1];
  r[2] *= a->d[2];
  r[3] *= a->d[3];
  r[4] *= a->d[4];
  r[5] *= a->d[5];
  r[6] *= a->d[6];
  r[7] *= a->d[7];
  r[8] *= a->d[8];
  r[9] *= a->d[9];

  reduce(s, r);
}

void fe_cmov(fe_t *s, const fe_t *e, int32_t m) {
  s->d[0] ^= (s->d[0] ^ e->d[0]) & m;
  s->d[1] ^= (s->d[1] ^ e->d[1]) & m;
  s->d[2] ^= (s->d[2] ^ e->d[2]) & m;
  s->d[3] ^= (s->d[3] ^ e->d[3]) & m;
  s->d[4] ^= (s->d[4] ^ e->d[4]) & m;
  s->d[5] ^= (s->d[5] ^ e->d[5]) & m;
  s->d[6] ^= (s->d[6] ^ e->d[6]) & m;
  s->d[7] ^= (s->d[7] ^ e->d[7]) & m;
  s->d[8] ^= (s->d[8] ^ e->d[8]) & m;
  s->d[9] ^= (s->d[9] ^ e->d[9]) & m;
}

void fe_cswap(fe_t *a, fe_t *b, int32_t m) {
  int32_t t;
  t = m & (a->d[0] ^ b->d[0]); a->d[0] ^= t; b->d[0] ^= t;
  t = m & (a->d[1] ^ b->d[1]); a->d[1] ^= t; b->d[1] ^= t;
  t = m & (a->d[2] ^ b->d[2]); a->d[2] ^= t; b->d[2] ^= t;
  t = m & (a->d[3] ^ b->d[3]); a->d[3] ^= t; b->d[3] ^= t;
  t = m & (a->d[4] ^ b->d[4]); a->d[4] ^= t; b->d[4] ^= t;
  t = m & (a->d[5] ^ b->d[5]); a->d[5] ^= t; b->d[5] ^= t;
  t = m & (a->d[6] ^ b->d[6]); a->d[6] ^= t; b->d[6] ^= t;
  t = m & (a->d[7] ^ b->d[7]); a->d[7] ^= t; b->d[7] ^= t;
  t = m & (a->d[8] ^ b->d[8]); a->d[8] ^= t; b->d[8] ^= t;
  t = m & (a->d[9] ^ b->d[9]); a->d[9] ^= t; b->d[9] ^= t;
}

void fe_cneg(fe_t *s, int32_t m) {
  s->d[0] -= (s->d[0] << 1) & m;
  s->d[1] -= (s->d[1] << 1) & m;
  s->d[2] -= (s->d[2] << 1) & m;
  s->d[3] -= (s->d[3] << 1) & m;
  s->d[4] -= (s->d[4] << 1) & m;
  s->d[5] -= (s->d[5] << 1) & m;
  s->d[6] -= (s->d[6] << 1) & m;
  s->d[7] -= (s->d[7] << 1) & m;
  s->d[8] -= (s->d[8] << 1) & m;
  s->d[9] -= (s->d[9] << 1) & m;
}

int8_t fe_isneg(const fe_t *e) {
  return (div(e) ^ e->d[0]) & 1;
}

int8_t fe_iszero(const fe_t *e) {
  uint32_t b[8];
  fe_tobytes((uint8_t *)b, e);
  return !(b[0] | b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7]);
}

void fe_powt(fe_t *s, const fe_t *a) {
  fe_t e, x, y, z;
  fe_sqr(&y, a);
  fe_sqr(&x, &y);
  fe_sqr(&x, &x);
  fe_mul(&x, &x, a);
  fe_mul(&e, &y, &x);  /* e = a ** 11 */
  fe_sqr(&y, &e);
  fe_mul(&x, &y, &x);  /* x = a ** 31 = a ** (2 ** 5 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &x, &z);  /* y = a ** (2 ** 10 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 20 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&x, &z, &x);  /* x = a ** (2 ** 25 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 50 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 100 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 125 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 250 - 1) */

  fe_sqr(&y, &y);
  fe_sqr(&y, &y);  /* y = a ** (2 ** 252 - 4) */
  fe_mul(s, &y, a);
}

void fe_inv(fe_t *s, const fe_t *a) {
  fe_t e, x, y, z;
  fe_sqr(&y, a);
  fe_sqr(&x, &y);
  fe_sqr(&x, &x);
  fe_mul(&x, &x, a);
  fe_mul(&e, &y, &x);  /* a ** 11 */
  fe_sqr(&y, &e);
  fe_mul(&x, &y, &x);  /* x = a ** 31 = a ** (2 ** 5 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &x, &z);  /* y = a ** (2 ** 10 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 20 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&x, &z, &x);  /* x = a ** (2 ** 25 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 50 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 100 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 125 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 250 - 1) */

  fe_sqr(&y, &y);
  fe_sqr(&y, &y);
  fe_sqr(&y, &y);
  fe_sqr(&y, &y);
  fe_sqr(&y, &y);  /* y = a ** (2 ** 255 - 32) */
  fe_mul(s, &y, &e);
}
