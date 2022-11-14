#include "field.h"

#ifdef CURVE25519_USE128

static const int64_t k_l51 = (1ul << 51) - 1;

static inline int64_t div(const fe_t *e) {
  int64_t q;
  q = (e->d[4] * 19 + (1ul << 50)) >> 51;
  q = (e->d[0] + q) >> 51;
  q = (e->d[1] + q) >> 51;
  q = (e->d[2] + q) >> 51;
  q = (e->d[3] + q) >> 51;
  q = (e->d[4] + q) >> 51;
  return q;
}

static inline void reduce(fe_t *s, int128_t r[5]) {
  int128_t c;

  r[4] += r[3] >> 51; r[3] &= k_l51;
  c = (r[4] + (1ul << 50)) >> 51;

  r[0] += c * 19; r[4] -= c << 51;
  r[1] += r[0] >> 51;
  r[2] += r[1] >> 51;
  r[3] += r[2] >> 51;

  s->d[0] = r[0] & k_l51;
  s->d[1] = r[1] & k_l51;
  s->d[2] = r[2] & k_l51;
  s->d[3] = r[3] & k_l51;
  s->d[4] = r[4] + (r[3] >> 51);
}

void fe_frombytes(fe_t *e, const uint8_t s[32]) {
  e->d[0] = htole64(*(int64_t *)(s) >> 0) & k_l51;
  e->d[1] = htole64(*(int64_t *)(s + 6) >> 3) & k_l51;
  e->d[2] = htole64(*(int64_t *)(s + 12) >> 6) & k_l51;
  e->d[3] = htole64(*(int64_t *)(s + 19) >> 1) & k_l51;
  e->d[4] = htole64(*(int64_t *)(s + 24) >> 12) & k_l51;
}

void fe_tobytes(uint8_t s[32], const fe_t *e) {
  int64_t e0 = e->d[0];
  int64_t e1 = e->d[1];
  int64_t e2 = e->d[2];
  int64_t e3 = e->d[3];
  int64_t e4 = e->d[4];

  e0 += div(e) * 19;
  e1 += e0 >> 51; e0 &= k_l51;
  e2 += e1 >> 51; e1 &= k_l51;
  e3 += e2 >> 51; e2 &= k_l51;
  e4 += e3 >> 51; e3 &= k_l51;
  e4 &= k_l51;

  int64_t *p = (int64_t *)s;
  *p = htole64((e0) | (e1 << 51)); p++;
  *p = htole64((e1 >> 13) | (e2 << 38)); p++;
  *p = htole64((e2 >> 26) | (e3 << 25)); p++;
  *p = htole64((e3 >> 39) | (e4 << 12)); p++;
}

// void fe_reduce(fe_t *e) {
//   e->d[0] += div(e) * 19;
//   e->d[1] += e->d[0] >> 51; e->d[0] &= k_l51;
//   e->d[2] += e->d[1] >> 51; e->d[1] &= k_l51;
//   e->d[3] += e->d[2] >> 51; e->d[2] &= k_l51;
//   e->d[4] += e->d[3] >> 51; e->d[3] &= k_l51;
//   e->d[4] &= k_l51;
// }

void fe_neg(fe_t *s, const fe_t *e) {
  s->d[0] = -e->d[0];
  s->d[1] = -e->d[1];
  s->d[2] = -e->d[2];
  s->d[3] = -e->d[3];
  s->d[4] = -e->d[4];
}

void fe_add(fe_t *s, const fe_t *a, const fe_t *b) {
  s->d[0] = a->d[0] + b->d[0];
  s->d[1] = a->d[1] + b->d[1];
  s->d[2] = a->d[2] + b->d[2];
  s->d[3] = a->d[3] + b->d[3];
  s->d[4] = a->d[4] + b->d[4];
}

void fe_sub(fe_t *s, const fe_t *a, const fe_t *b) {
  s->d[0] = a->d[0] - b->d[0];
  s->d[1] = a->d[1] - b->d[1];
  s->d[2] = a->d[2] - b->d[2];
  s->d[3] = a->d[3] - b->d[3];
  s->d[4] = a->d[4] - b->d[4];
}

void fe_mul(fe_t *s, const fe_t *a, const fe_t *b) {
  int128_t a0 = a->d[0];
  int128_t a1 = a->d[1];
  int128_t a2 = a->d[2];
  int128_t a3 = a->d[3];
  int128_t a4 = a->d[4];
  int128_t b0 = b->d[0];
  int128_t b1 = b->d[1];
  int128_t b2 = b->d[2];
  int128_t b3 = b->d[3];
  int128_t b4 = b->d[4];

  int128_t r[5] = {
    a0 * b0 + (a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1) * 19,
    a0 * b1 + a1 * b0 + (a2 * b4 + a3 * b3 + a4 * b2) * 19,
    a0 * b2 + a1 * b1 + a2 * b0 + (a3 * b4 + a4 * b3) * 19,
    a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0 + (a4 * b4) * 19,
    a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0,
  };

  reduce(s, r);
}

void fe_sqr(fe_t *s, const fe_t *a) {
  int128_t a0 = a->d[0];
  int128_t a1 = a->d[1];
  int128_t a2 = a->d[2];
  int128_t a3 = a->d[3];
  int128_t a4 = a->d[4];

  int128_t r[5] = {
    a0 * a0 + (a1 * a4 + a2 * a3) * 38,
    a0 * a1 * 2 + (a2 * a4 * 2 + a3 * a3) * 19,
    a1 * a1 + (a0 * a2 + a3 * a4 * 19) * 2,
    (a0 * a3 + a1 * a2) * 2 + (a4 * a4) * 19,
    a2 * a2 + (a0 * a4 + a1 * a3) * 2,
  };

  reduce(s, r);
}

void fe_mul121666(fe_t *s, const fe_t *a) {
  int128_t r[5] = {
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

  reduce(s, r);
}

void fe_cmov(fe_t *s, const fe_t *e, int64_t m) {
  s->d[0] ^= (s->d[0] ^ e->d[0]) & m;
  s->d[1] ^= (s->d[1] ^ e->d[1]) & m;
  s->d[2] ^= (s->d[2] ^ e->d[2]) & m;
  s->d[3] ^= (s->d[3] ^ e->d[3]) & m;
  s->d[4] ^= (s->d[4] ^ e->d[4]) & m;
}

void fe_cswap(fe_t *a, fe_t *b, int64_t m) {
  int64_t t;
  t = m & (a->d[0] ^ b->d[0]); a->d[0] ^= t; b->d[0] ^= t;
  t = m & (a->d[1] ^ b->d[1]); a->d[1] ^= t; b->d[1] ^= t;
  t = m & (a->d[2] ^ b->d[2]); a->d[2] ^= t; b->d[2] ^= t;
  t = m & (a->d[3] ^ b->d[3]); a->d[3] ^= t; b->d[3] ^= t;
  t = m & (a->d[4] ^ b->d[4]); a->d[4] ^= t; b->d[4] ^= t;
}

void fe_cneg(fe_t *s, int64_t m) {
  s->d[0] -= (s->d[0] << 1) & m;
  s->d[1] -= (s->d[1] << 1) & m;
  s->d[2] -= (s->d[2] << 1) & m;
  s->d[3] -= (s->d[3] << 1) & m;
  s->d[4] -= (s->d[4] << 1) & m;
}

int8_t fe_isneg(const fe_t *e) {
  return (div(e) ^ e->d[0]) & 1;
}

int8_t fe_iszero(const fe_t *e) {
  uint64_t b[4];
  fe_tobytes((uint8_t *)b, e);
  return !(b[0] | b[1] | b[2] | b[3]);
}

#endif
