#include "types.h"

#ifdef CURVE25519_USE128

#include <endian.h>

/* scalar mod 0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed */

static const int64_t k_l42 = (1L << 42) - 1;

static inline void reduce_step(int128_t *s) {
  *(s - 6) -= *s * 0x31a5cf5d3edL;
  *(s - 5) -= *s * 0x1e735960498L;
  *(s - 4) -= *s * 0x14def9dea2fL;
}

static void reduce(uint8_t s[32], int128_t r[12]) {
  int128_t c;
  int64_t *p = (int64_t *)s;

  reduce_step(r + 11);
  reduce_step(r + 10);
  reduce_step(r + 9);

  r[6] += r[5] >> 42; r[5] &= k_l42;
  r[7] += r[6] >> 42; r[6] &= k_l42;
  r[8] += r[7] >> 42; r[7] &= k_l42;

  reduce_step(r + 8);
  reduce_step(r + 7);

  r[5] += r[4] >> 42; r[4] &= k_l42;
  c = (r[5] + (1L << 41)) >> 42; r[5] -= c << 42; r[6] += c;
  
  reduce_step(r + 6);

  r[1] += r[0] >> 42; r[0] &= k_l42;
  r[2] += r[1] >> 42; r[1] &= k_l42;
  r[3] += r[2] >> 42; r[2] &= k_l42;
  r[4] += r[3] >> 42; r[3] &= k_l42;
  r[5] += r[4] >> 42; r[4] &= k_l42;
  r[6]  = r[5] >> 42; r[5] &= k_l42;

  reduce_step(r + 6);

  r[1] += r[0] >> 42; r[0] &= k_l42;
  r[2] += r[1] >> 42; r[1] &= k_l42;
  r[3] += r[2] >> 42; r[2] &= k_l42;
  r[4] += r[3] >> 42; r[3] &= k_l42;
  r[5] += r[4] >> 42; r[4] &= k_l42;

  *p = htole64((r[0]) | (r[1] << 42)); p++;
  *p = htole64((r[1] >> 22) | (r[2] << 20) | (r[3] << 62)); p++;
  *p = htole64((r[3] >> 2) | (r[4] << 40)); p++;
  *p = htole64((r[4] >> 24) | (r[5] << 18)); p++;
}

void sc_reduce(uint8_t s[64]) {
  int128_t r[12] = {
    (htole64(*(int64_t *)(s)) >> 0) & k_l42,
    (htole64(*(int64_t *)(s + 5)) >> 2) & k_l42,
    (htole64(*(int64_t *)(s + 10)) >> 4) & k_l42,
    (htole64(*(int64_t *)(s + 15)) >> 6) & k_l42,
    (htole64(*(int64_t *)(s + 21)) >> 0) & k_l42,
    (htole64(*(int64_t *)(s + 26)) >> 2) & k_l42,
    (htole64(*(int64_t *)(s + 31)) >> 4) & k_l42,
    (htole64(*(int64_t *)(s + 36)) >> 6) & k_l42,
    (htole64(*(int64_t *)(s + 42)) >> 0) & k_l42,
    (htole64(*(int64_t *)(s + 47)) >> 2) & k_l42,
    (htole64(*(int64_t *)(s + 52)) >> 4) & k_l42,
    (htole64(*(uint64_t *)(s + 56)) >> 14),
  };
  
  reduce(s, r);
}

void sc_muladd(uint8_t s[32], const uint8_t a[32], const uint8_t b[32], const uint8_t c[32]) {
  int128_t a0 = (htole64(*(int64_t *)(a)) >> 0) & k_l42;
  int128_t a1 = (htole64(*(int64_t *)(a + 5)) >> 2) & k_l42;
  int128_t a2 = (htole64(*(int64_t *)(a + 10)) >> 4) & k_l42;
  int128_t a3 = (htole64(*(int64_t *)(a + 15)) >> 6) & k_l42;
  int128_t a4 = (htole64(*(int64_t *)(a + 21)) >> 0) & k_l42;
  int128_t a5 = (htole64(*(uint64_t *)(a + 24)) >> 18);
  int128_t b0 = (htole64(*(int64_t *)(b)) >> 0) & k_l42;
  int128_t b1 = (htole64(*(int64_t *)(b + 5)) >> 2) & k_l42;
  int128_t b2 = (htole64(*(int64_t *)(b + 10)) >> 4) & k_l42;
  int128_t b3 = (htole64(*(int64_t *)(b + 15)) >> 6) & k_l42;
  int128_t b4 = (htole64(*(int64_t *)(b + 21)) >> 0) & k_l42;
  int128_t b5 = (htole64(*(uint64_t *)(b + 24)) >> 18);
  int128_t r[12];

  r[0] = (htole64(*(int64_t *)(c)) >> 0) & k_l42;
  r[1] = (htole64(*(int64_t *)(c + 5)) >> 2) & k_l42;
  r[2] = (htole64(*(int64_t *)(c + 10)) >> 4) & k_l42;
  r[3] = (htole64(*(int64_t *)(c + 15)) >> 6) & k_l42;
  r[4] = (htole64(*(int64_t *)(c + 21)) >> 0) & k_l42;
  r[5] = (htole64(*(uint64_t *)(c + 24)) >> 18);
  
  r[0] += a0 * b0;
  r[1] += a0 * b1 + a1 * b0;
  r[2] += a0 * b2 + a1 * b1 + a2 * b0;
  r[3] += a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
  r[4] += a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0;
  r[5] += a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0;
  r[6]  = a1 * b5 + a2 * b4 + a3 * b3 + a4 * b2 + a5 * b1;
  r[7]  = a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2;
  r[8]  = a3 * b5 + a4 * b4 + a5 * b3;
  r[9]  = a4 * b5 + a5 * b4;
  r[10] = a5 * b5;

  r[ 9] += r[ 8] >> 42; r[ 8] &= k_l42;
  r[10] += r[ 9] >> 42; r[ 9] &= k_l42;
  r[11]  = r[10] >> 42; r[10] &= k_l42;

  reduce(s, r);
}

int32_t sc_verify(const uint8_t s[32]) {
  if (s[31] > 0x10) return -1;
  if (s[31] < 0x10) return 0;

  int64_t c = 0x10000000 ^ *(int32_t *)(s + 28);
  c |= *(int32_t *)(s + 24);
  c |= *(int32_t *)(s + 20);
  c |= *(int32_t *)(s + 16);
  if (c) return -1;

  /* here c equals 0 */
  /* 0x14def9dea2f79cd65812631a5cf5d3ed - s[0:16] */
  c += 0x5cf5d3ed; c -= *(int32_t *)(s + 0); c >>= 32;
  c += 0x5812631a; c -= *(int32_t *)(s + 8); c >>= 32;
  c += 0xa2f79cd6; c -= *(int32_t *)(s + 16); c >>= 32;
  c += 0x14def9de; c -= *(int32_t *)(s + 24);

  return c < 0? -1: 0;
}

#endif  /* CURVE25519_USE128 */
