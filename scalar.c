#include "types.h"
#include "zint.h"

/* scalar mod 0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed */

static const int32_t k_l21 = (1L << 21) - 1;

static inline void reduce_step(int64_t *s) {
  *(s - 12) += *s * 0x0a2c13;
  *(s - 11) += *s * 0x072d18;
  *(s - 10) += *s * 0x09fb67;
  *(s - 9) -= *s * 0x0f39ad;
  *(s - 8) += *s * 0x0215d1;
  *(s - 7) -= *s * 0x0a6f7d;
}

static void reduce(uint8_t s[32], int64_t r[24]) {
  int64_t c;

  reduce_step(r + 23);
  reduce_step(r + 22);
  reduce_step(r + 21);
  reduce_step(r + 20);
  reduce_step(r + 19);
  reduce_step(r + 18);

  r[13] += r[12] >> 21; r[12] &= k_l21;
  r[14] += r[13] >> 21; r[13] &= k_l21;
  r[15] += r[14] >> 21; r[14] &= k_l21;
  r[16] += r[15] >> 21; r[15] &= k_l21;
  r[17] += r[16] >> 21; r[16] &= k_l21;

  reduce_step(r + 17);
  reduce_step(r + 16);
  reduce_step(r + 15);
  reduce_step(r + 14);
  reduce_step(r + 13);

  r[11] += r[10] >> 21; r[10] &= k_l21;
  c = (r[11] + (1 << 21)) >> 21; r[11] -= c << 21; r[12] += c;
  
  reduce_step(r + 12);

  r[ 1] += r[ 0] >> 21; r[ 0] &= k_l21;
  r[ 2] += r[ 1] >> 21; r[ 1] &= k_l21;
  r[ 3] += r[ 2] >> 21; r[ 2] &= k_l21;
  r[ 4] += r[ 3] >> 21; r[ 3] &= k_l21;
  r[ 5] += r[ 4] >> 21; r[ 4] &= k_l21;
  r[ 6] += r[ 5] >> 21; r[ 5] &= k_l21;
  r[ 7] += r[ 6] >> 21; r[ 6] &= k_l21;
  r[ 8] += r[ 7] >> 21; r[ 7] &= k_l21;
  r[ 9] += r[ 8] >> 21; r[ 8] &= k_l21;
  r[10] += r[ 9] >> 21; r[ 9] &= k_l21;
  r[11] += r[10] >> 21; r[10] &= k_l21;
  r[12]  = r[11] >> 21; r[11] &= k_l21;

  reduce_step(r + 12);

  r[ 1] += r[ 0] >> 21; r[ 0] &= k_l21;
  r[ 2] += r[ 1] >> 21; r[ 1] &= k_l21;
  r[ 3] += r[ 2] >> 21; r[ 2] &= k_l21;
  r[ 4] += r[ 3] >> 21; r[ 3] &= k_l21;
  r[ 5] += r[ 4] >> 21; r[ 4] &= k_l21;
  r[ 6] += r[ 5] >> 21; r[ 5] &= k_l21;
  r[ 7] += r[ 6] >> 21; r[ 6] &= k_l21;
  r[ 8] += r[ 7] >> 21; r[ 7] &= k_l21;
  r[ 9] += r[ 8] >> 21; r[ 8] &= k_l21;
  r[10] += r[ 9] >> 21; r[ 9] &= k_l21;
  r[11] += r[10] >> 21; r[10] &= k_l21;

  dumple32(s, (r[0]) | (r[1] << 21));
  dumple32(s + 4, (r[1] >> 11) | (r[2] << 10) | (r[3] << 31));
  dumple32(s + 8, (r[3] >> 1) | (r[4] << 20));
  dumple32(s + 12, (r[4] >> 12) | (r[5] << 9) | (r[6] << 30));
  dumple32(s + 16, (r[6] >> 2) | (r[7] << 19));
  dumple32(s + 20, (r[7] >> 13) | (r[8] << 8) | (r[9] << 29));
  dumple32(s + 24, (r[9] >> 3) | (r[10] << 18));
  dumple32(s + 28, (r[10] >> 14) | (r[11] << 7));
}

void sc_reduce(uint8_t s[64]) {
  int64_t r[24] = {
    (loadle32(s) >> 0) & k_l21,
    (loadle32(s + 2) >> 5) & k_l21,
    (loadle32(s + 5) >> 2) & k_l21,
    (loadle32(s + 7) >> 7) & k_l21,
    (loadle32(s + 10) >> 4) & k_l21,
    (loadle32(s + 13) >> 1) & k_l21,
    (loadle32(s + 15) >> 6) & k_l21,
    (loadle32(s + 18) >> 3) & k_l21,
    (loadle32(s + 21) >> 0) & k_l21,
    (loadle32(s + 23) >> 5) & k_l21,
    (loadle32(s + 26) >> 2) & k_l21,
    (loadle32(s + 28) >> 7) & k_l21,
    (loadle32(s + 31) >> 4) & k_l21,
    (loadle32(s + 34) >> 1) & k_l21,
    (loadle32(s + 36) >> 6) & k_l21,
    (loadle32(s + 39) >> 3) & k_l21,
    (loadle32(s + 42) >> 0) & k_l21,
    (loadle32(s + 44) >> 5) & k_l21,
    (loadle32(s + 47) >> 2) & k_l21,
    (loadle32(s + 49) >> 7) & k_l21,
    (loadle32(s + 52) >> 4) & k_l21,
    (loadle32(s + 55) >> 1) & k_l21,
    (loadle32(s + 57) >> 6) & k_l21,
    (loadle32(s + 60) >> 3),
  };
  
  reduce(s, r);
}

void sc_muladd(uint8_t s[32], const uint8_t a[32], const uint8_t b[32], const uint8_t c[32]) {
  int64_t a0 = (loadle32(a) >> 0) & k_l21;
  int64_t a1 = (loadle32(a + 2) >> 5) & k_l21;
  int64_t a2 = (loadle32(a + 5) >> 2) & k_l21;
  int64_t a3 = (loadle32(a + 7) >> 7) & k_l21;
  int64_t a4 = (loadle32(a + 10) >> 4) & k_l21;
  int64_t a5 = (loadle32(a + 13) >> 1) & k_l21;
  int64_t a6 = (loadle32(a + 15) >> 6) & k_l21;
  int64_t a7 = (loadle32(a + 18) >> 3) & k_l21;
  int64_t a8 = (loadle32(a + 21) >> 0) & k_l21;
  int64_t a9 = (loadle32(a + 23) >> 5) & k_l21;
  int64_t aa = (loadle32(a + 26) >> 2) & k_l21;
  int64_t ab = (loadle32(a + 28) >> 7);
  int64_t b0 = (loadle32(b) >> 0) & k_l21;
  int64_t b1 = (loadle32(b + 2) >> 5) & k_l21;
  int64_t b2 = (loadle32(b + 5) >> 2) & k_l21;
  int64_t b3 = (loadle32(b + 7) >> 7) & k_l21;
  int64_t b4 = (loadle32(b + 10) >> 4) & k_l21;
  int64_t b5 = (loadle32(b + 13) >> 1) & k_l21;
  int64_t b6 = (loadle32(b + 15) >> 6) & k_l21;
  int64_t b7 = (loadle32(b + 18) >> 3) & k_l21;
  int64_t b8 = (loadle32(b + 21) >> 0) & k_l21;
  int64_t b9 = (loadle32(b + 23) >> 5) & k_l21;
  int64_t ba = (loadle32(b + 26) >> 2) & k_l21;
  int64_t bb = (loadle32(b + 28) >> 7);
  int64_t r[24];

  r[ 0] = (loadle32(c) >> 0) & k_l21;
  r[ 1] = (loadle32(c + 2) >> 5) & k_l21;
  r[ 2] = (loadle32(c + 5) >> 2) & k_l21;
  r[ 3] = (loadle32(c + 7) >> 7) & k_l21;
  r[ 4] = (loadle32(c + 10) >> 4) & k_l21;
  r[ 5] = (loadle32(c + 13) >> 1) & k_l21;
  r[ 6] = (loadle32(c + 15) >> 6) & k_l21;
  r[ 7] = (loadle32(c + 18) >> 3) & k_l21;
  r[ 8] = (loadle32(c + 21) >> 0) & k_l21;
  r[ 9] = (loadle32(c + 23) >> 5) & k_l21;
  r[10] = (loadle32(c + 26) >> 2) & k_l21;
  r[11] = (loadle32(c + 28) >> 7) & k_l21;
  
  r[ 0] += a0 * b0;
  r[ 1] += a0 * b1 + a1 * b0;
  r[ 2] += a0 * b2 + a1 * b1 + a2 * b0;
  r[ 3] += a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
  r[ 4] += a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0;
  r[ 5] += a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0;
  r[ 6] += a0 * b6 + a1 * b5 + a2 * b4 + a3 * b3 + a4 * b2 + a5 * b1 + a6 * b0;
  r[ 7] += a0 * b7 + a1 * b6 + a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2 + a6 * b1 + a7 * b0;
  r[ 8] += a0 * b8 + a1 * b7 + a2 * b6 + a3 * b5 + a4 * b4 + a5 * b3 + a6 * b2 + a7 * b1 + a8 * b0;
  r[ 9] += a0 * b9 + a1 * b8 + a2 * b7 + a3 * b6 + a4 * b5 + a5 * b4 + a6 * b3 + a7 * b2 + a8 * b1 + a9 * b0;
  r[10] += a0 * ba + a1 * b9 + a2 * b8 + a3 * b7 + a4 * b6 + a5 * b5 + a6 * b4 + a7 * b3 + a8 * b2 + a9 * b1 + aa * b0;
  r[11] += a0 * bb + a1 * ba + a2 * b9 + a3 * b8 + a4 * b7 + a5 * b6 + a6 * b5 + a7 * b4 + a8 * b3 + a9 * b2 + aa * b1 + ab * b0;

  r[12] = a1 * bb + a2 * ba + a3 * b9 + a4 * b8 + a5 * b7 + a6 * b6 + a7 * b5 + a8 * b4 + a9 * b3 + aa * b2 + ab * b1;
  r[13] = a2 * bb + a3 * ba + a4 * b9 + a5 * b8 + a6 * b7 + a7 * b6 + a8 * b5 + a9 * b4 + aa * b3 + ab * b2;
  r[14] = a3 * bb + a4 * ba + a5 * b9 + a6 * b8 + a7 * b7 + a8 * b6 + a9 * b5 + aa * b4 + ab * b3;
  r[15] = a4 * bb + a5 * ba + a6 * b9 + a7 * b8 + a8 * b7 + a9 * b6 + aa * b5 + ab * b4;
  r[16] = a5 * bb + a6 * ba + a7 * b9 + a8 * b8 + a9 * b7 + aa * b6 + ab * b5;
  r[17] = a6 * bb + a7 * ba + a8 * b9 + a9 * b8 + aa * b7 + ab * b6;
  r[18] = a7 * bb + a8 * ba + a9 * b9 + aa * b8 + ab * b7;
  r[19] = a8 * bb + a9 * ba + aa * b9 + ab * b8;
  r[20] = a9 * bb + aa * ba + ab * b9;
  r[21] = aa * bb + ab * ba;
  r[22] = ab * bb;

  r[18] += r[17] >> 21; r[17] &= k_l21;
  r[19] += r[18] >> 21; r[18] &= k_l21;
  r[20] += r[19] >> 21; r[19] &= k_l21;
  r[21] += r[20] >> 21; r[20] &= k_l21;
  r[22] += r[21] >> 21; r[21] &= k_l21;
  r[23]  = r[22] >> 21; r[22] &= k_l21;

  reduce(s, r);
}

int32_t sc_verify(const uint8_t s[32]) {
  if (s[31] > 0x10) return -1;
  if (s[31] < 0x10) return 0;

  int64_t c = 0x10000000 ^ loadle32(s + 28);
  c |= loadle32(s + 24);
  c |= loadle32(s + 20);
  c |= loadle32(s + 16);
  if (c) return -1;

  /* here c equals 0 */
  /* 0x14def9dea2f79cd65812631a5cf5d3ed - s[0:16] */
  c += 0x5cf5d3ed; c -= loadle32(s); c >>= 32;
  c += 0x5812631a; c -= loadle32(s + 4); c >>= 32;
  c += 0xa2f79cd6; c -= loadle32(s + 8); c >>= 32;
  c += 0x14def9de; c -= loadle32(s + 12);

  return c < 0? -1: 0;
}
