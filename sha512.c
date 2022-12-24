#include <endian.h>
#include <memory.h>

#include "sha512.h"

#define SHA512_BLOCK_SIZE 128
#define SHA512_BLOCK_MASK (SHA512_BLOCK_SIZE - 1)

static const uint64_t K[] = {
  UINT64_C(0x428a2f98d728ae22),
  UINT64_C(0x7137449123ef65cd),
  UINT64_C(0xb5c0fbcfec4d3b2f),
  UINT64_C(0xe9b5dba58189dbbc),
  UINT64_C(0x3956c25bf348b538),
  UINT64_C(0x59f111f1b605d019),
  UINT64_C(0x923f82a4af194f9b),
  UINT64_C(0xab1c5ed5da6d8118),
  UINT64_C(0xd807aa98a3030242),
  UINT64_C(0x12835b0145706fbe),
  UINT64_C(0x243185be4ee4b28c),
  UINT64_C(0x550c7dc3d5ffb4e2),
  UINT64_C(0x72be5d74f27b896f),
  UINT64_C(0x80deb1fe3b1696b1),
  UINT64_C(0x9bdc06a725c71235),
  UINT64_C(0xc19bf174cf692694),
  UINT64_C(0xe49b69c19ef14ad2),
  UINT64_C(0xefbe4786384f25e3),
  UINT64_C(0x0fc19dc68b8cd5b5),
  UINT64_C(0x240ca1cc77ac9c65),
  UINT64_C(0x2de92c6f592b0275),
  UINT64_C(0x4a7484aa6ea6e483),
  UINT64_C(0x5cb0a9dcbd41fbd4),
  UINT64_C(0x76f988da831153b5),
  UINT64_C(0x983e5152ee66dfab),
  UINT64_C(0xa831c66d2db43210),
  UINT64_C(0xb00327c898fb213f),
  UINT64_C(0xbf597fc7beef0ee4),
  UINT64_C(0xc6e00bf33da88fc2),
  UINT64_C(0xd5a79147930aa725),
  UINT64_C(0x06ca6351e003826f),
  UINT64_C(0x142929670a0e6e70),
  UINT64_C(0x27b70a8546d22ffc),
  UINT64_C(0x2e1b21385c26c926),
  UINT64_C(0x4d2c6dfc5ac42aed),
  UINT64_C(0x53380d139d95b3df),
  UINT64_C(0x650a73548baf63de),
  UINT64_C(0x766a0abb3c77b2a8),
  UINT64_C(0x81c2c92e47edaee6),
  UINT64_C(0x92722c851482353b),
  UINT64_C(0xa2bfe8a14cf10364),
  UINT64_C(0xa81a664bbc423001),
  UINT64_C(0xc24b8b70d0f89791),
  UINT64_C(0xc76c51a30654be30),
  UINT64_C(0xd192e819d6ef5218),
  UINT64_C(0xd69906245565a910),
  UINT64_C(0xf40e35855771202a),
  UINT64_C(0x106aa07032bbd1b8),
  UINT64_C(0x19a4c116b8d2d0c8),
  UINT64_C(0x1e376c085141ab53),
  UINT64_C(0x2748774cdf8eeb99),
  UINT64_C(0x34b0bcb5e19b48a8),
  UINT64_C(0x391c0cb3c5c95a63),
  UINT64_C(0x4ed8aa4ae3418acb),
  UINT64_C(0x5b9cca4f7763e373),
  UINT64_C(0x682e6ff3d6b2b8a3),
  UINT64_C(0x748f82ee5defb2fc),
  UINT64_C(0x78a5636f43172f60),
  UINT64_C(0x84c87814a1f0ab72),
  UINT64_C(0x8cc702081a6439ec),
  UINT64_C(0x90befffa23631e28),
  UINT64_C(0xa4506cebde82bde9),
  UINT64_C(0xbef9a3f7b2c67915),
  UINT64_C(0xc67178f2e372532b),
  UINT64_C(0xca273eceea26619c),
  UINT64_C(0xd186b8c721c0c207),
  UINT64_C(0xeada7dd6cde0eb1e),
  UINT64_C(0xf57d4f7fee6ed178),
  UINT64_C(0x06f067aa72176fba),
  UINT64_C(0x0a637dc5a2c898a6),
  UINT64_C(0x113f9804bef90dae),
  UINT64_C(0x1b710b35131c471b),
  UINT64_C(0x28db77f523047d84),
  UINT64_C(0x32caab7b40c72493),
  UINT64_C(0x3c9ebe0a15c9bebc),
  UINT64_C(0x431d67c49c100d4c),
  UINT64_C(0x4cc5d4becb3e42b6),
  UINT64_C(0x597f299cfc657e2a),
  UINT64_C(0x5fcb6fab3ad6faec),
  UINT64_C(0x6c44198c4a475817),
};

static inline uint64_t ror(uint64_t x, uint32_t n) {
  return (x >> n) | (x << (64 - n));
}

static inline uint64_t ch(uint64_t x, uint64_t y, uint64_t z) {
  // return (x & y) | (~x & z);
  return ((y ^ z) & x) ^ z;
}

static inline uint64_t maj(uint64_t x, uint64_t y, uint64_t z) {
  // return (x & y) | (y & z) | (z & x);
  return (x & (y | z)) | (y & z);
}

static inline uint64_t expsig0(uint64_t x) {
  return ror(x, 1) ^ ror(x, 8) ^ (x >> 7);
}

static inline uint64_t expsig1(uint64_t x) {
  return ror(x, 19) ^ ror(x, 61) ^ (x >> 6);
}

static inline uint64_t hashsig0(uint64_t x) {
  return ror(x, 28) ^ ror(x, 34) ^ ror(x, 39);
}

static inline uint64_t hashsig1(uint64_t x) {
  return ror(x, 14) ^ ror(x, 18) ^ ror(x, 41);
}

static inline void expand(uint64_t *w, int i) {
  w[i] = expsig1(w[i - 2]) + w[i - 7] + expsig0(w[i - 15]) + w[i - 16];
}

#define idx(x, i) (x - i) & 7
#define sha512_round(t, w, i) \
  t[idx(7, i)] += hashsig1(t[idx(4, i)]) + ch(t[idx(4, i)], t[idx(5, i)], t[idx(6, i)]) + K[i] + w[i]; \
  t[idx(3, i)] += t[idx(7, i)]; \
  t[idx(7, i)] += hashsig0(t[idx(0, i)]) + maj(t[idx(0, i)], t[idx(1, i)], t[idx(2, i)])

void sha512_init(sha512_ctx_t *ctx) {
  ctx->len = 0;
  ctx->s[0] = UINT64_C(0x6a09e667f3bcc908);
  ctx->s[1] = UINT64_C(0xbb67ae8584caa73b);
  ctx->s[2] = UINT64_C(0x3c6ef372fe94f82b);
  ctx->s[3] = UINT64_C(0xa54ff53a5f1d36f1);
  ctx->s[4] = UINT64_C(0x510e527fade682d1);
  ctx->s[5] = UINT64_C(0x9b05688c2b3e6c1f);
  ctx->s[6] = UINT64_C(0x1f83d9abfb41bd6b);
  ctx->s[7] = UINT64_C(0x5be0cd19137e2179);
}

void sha512_chunk(uint64_t *s, const uint8_t *src, size_t n) {
  uint64_t w[80];
  uint64_t *sp = (uint64_t *)src;
  uint64_t t[8];
  while (n) {
    memcpy(t, s, sizeof(t));

    w[0] = htobe64(*sp); sp++;
    w[1] = htobe64(*sp); sp++;
    w[2] = htobe64(*sp); sp++;
    w[3] = htobe64(*sp); sp++;
    w[4] = htobe64(*sp); sp++;
    w[5] = htobe64(*sp); sp++;
    w[6] = htobe64(*sp); sp++;
    w[7] = htobe64(*sp); sp++;
    w[8] = htobe64(*sp); sp++;
    w[9] = htobe64(*sp); sp++;
    w[10] = htobe64(*sp); sp++;
    w[11] = htobe64(*sp); sp++;
    w[12] = htobe64(*sp); sp++;
    w[13] = htobe64(*sp); sp++;
    w[14] = htobe64(*sp); sp++;
    w[15] = htobe64(*sp); sp++;

    expand(w, 16);
    expand(w, 17);
    expand(w, 18);
    expand(w, 19);
    expand(w, 20);
    expand(w, 21);
    expand(w, 22);
    expand(w, 23);
    expand(w, 24);
    expand(w, 25);
    expand(w, 26);
    expand(w, 27);
    expand(w, 28);
    expand(w, 29);
    expand(w, 30);
    expand(w, 31);
    expand(w, 32);
    expand(w, 33);
    expand(w, 34);
    expand(w, 35);
    expand(w, 36);
    expand(w, 37);
    expand(w, 38);
    expand(w, 39);
    expand(w, 40);
    expand(w, 41);
    expand(w, 42);
    expand(w, 43);
    expand(w, 44);
    expand(w, 45);
    expand(w, 46);
    expand(w, 47);
    expand(w, 48);
    expand(w, 49);
    expand(w, 50);
    expand(w, 51);
    expand(w, 52);
    expand(w, 53);
    expand(w, 54);
    expand(w, 55);
    expand(w, 56);
    expand(w, 57);
    expand(w, 58);
    expand(w, 59);
    expand(w, 60);
    expand(w, 61);
    expand(w, 62);
    expand(w, 63);
    expand(w, 64);
    expand(w, 65);
    expand(w, 66);
    expand(w, 67);
    expand(w, 68);
    expand(w, 69);
    expand(w, 70);
    expand(w, 71);
    expand(w, 72);
    expand(w, 73);
    expand(w, 74);
    expand(w, 75);
    expand(w, 76);
    expand(w, 77);
    expand(w, 78);
    expand(w, 79);

    sha512_round(t, w, 0);
    sha512_round(t, w, 1);
    sha512_round(t, w, 2);
    sha512_round(t, w, 3);
    sha512_round(t, w, 4);
    sha512_round(t, w, 5);
    sha512_round(t, w, 6);
    sha512_round(t, w, 7);
    sha512_round(t, w, 8);
    sha512_round(t, w, 9);
    sha512_round(t, w, 10);
    sha512_round(t, w, 11);
    sha512_round(t, w, 12);
    sha512_round(t, w, 13);
    sha512_round(t, w, 14);
    sha512_round(t, w, 15);
    sha512_round(t, w, 16);
    sha512_round(t, w, 17);
    sha512_round(t, w, 18);
    sha512_round(t, w, 19);
    sha512_round(t, w, 20);
    sha512_round(t, w, 21);
    sha512_round(t, w, 22);
    sha512_round(t, w, 23);
    sha512_round(t, w, 24);
    sha512_round(t, w, 25);
    sha512_round(t, w, 26);
    sha512_round(t, w, 27);
    sha512_round(t, w, 28);
    sha512_round(t, w, 29);
    sha512_round(t, w, 30);
    sha512_round(t, w, 31);
    sha512_round(t, w, 32);
    sha512_round(t, w, 33);
    sha512_round(t, w, 34);
    sha512_round(t, w, 35);
    sha512_round(t, w, 36);
    sha512_round(t, w, 37);
    sha512_round(t, w, 38);
    sha512_round(t, w, 39);
    sha512_round(t, w, 40);
    sha512_round(t, w, 41);
    sha512_round(t, w, 42);
    sha512_round(t, w, 43);
    sha512_round(t, w, 44);
    sha512_round(t, w, 45);
    sha512_round(t, w, 46);
    sha512_round(t, w, 47);
    sha512_round(t, w, 48);
    sha512_round(t, w, 49);
    sha512_round(t, w, 50);
    sha512_round(t, w, 51);
    sha512_round(t, w, 52);
    sha512_round(t, w, 53);
    sha512_round(t, w, 54);
    sha512_round(t, w, 55);
    sha512_round(t, w, 56);
    sha512_round(t, w, 57);
    sha512_round(t, w, 58);
    sha512_round(t, w, 59);
    sha512_round(t, w, 60);
    sha512_round(t, w, 61);
    sha512_round(t, w, 62);
    sha512_round(t, w, 63);
    sha512_round(t, w, 64);
    sha512_round(t, w, 65);
    sha512_round(t, w, 66);
    sha512_round(t, w, 67);
    sha512_round(t, w, 68);
    sha512_round(t, w, 69);
    sha512_round(t, w, 70);
    sha512_round(t, w, 71);
    sha512_round(t, w, 72);
    sha512_round(t, w, 73);
    sha512_round(t, w, 74);
    sha512_round(t, w, 75);
    sha512_round(t, w, 76);
    sha512_round(t, w, 77);
    sha512_round(t, w, 78);
    sha512_round(t, w, 79);

    s[0] += t[0];
    s[1] += t[1];
    s[2] += t[2];
    s[3] += t[3];
    s[4] += t[4];
    s[5] += t[5];
    s[6] += t[6];
    s[7] += t[7];

    n -= SHA512_BLOCK_SIZE;
  }
}

void sha512_update(sha512_ctx_t *ctx, const uint8_t *src, size_t n) {
  uint8_t offset = ctx->len & SHA512_BLOCK_MASK;
  ctx->len += n;
  if (offset) {
    uint8_t free = SHA512_BLOCK_SIZE - offset;
    if (free <= n) {
      memcpy(ctx->buf + offset, src, free);
      sha512_chunk(ctx->s, ctx->buf, SHA512_BLOCK_SIZE);
      n -= free;
      src += free;
      offset = 0;
    }
  }

  uint64_t len = n & ~SHA512_BLOCK_MASK;
  if (len) {
    sha512_chunk(ctx->s, src, len);
    src += len;
  }
  memcpy(ctx->buf + offset, src, n & SHA512_BLOCK_MASK);
}

void sha512_finalize(sha512_ctx_t *ctx, uint8_t *dst) {
  uint8_t *p = ctx->buf + (ctx->len & SHA512_BLOCK_MASK);
  uint8_t *end = ctx->buf + SHA512_BLOCK_SIZE;
  uint8_t *q = end - (sizeof(uint64_t) << 1);

  *p = 0x80; p++;
  if (p > q) {
    memset(p, 0x00, end - p);
    sha512_chunk(ctx->s, ctx->buf, SHA512_BLOCK_SIZE);
    p = ctx->buf;
  }

  q += sizeof(uint32_t) * 3;
  memset(p, 0x00, q - p);
  *(uint32_t *)q = htobe32(ctx->len << 3);
  sha512_chunk(ctx->s, ctx->buf, SHA512_BLOCK_SIZE);

  uint64_t *pd = (uint64_t *)dst;
  *pd = htobe64(ctx->s[0]); pd++;
  *pd = htobe64(ctx->s[1]); pd++;
  *pd = htobe64(ctx->s[2]); pd++;
  *pd = htobe64(ctx->s[3]); pd++;
  *pd = htobe64(ctx->s[4]); pd++;
  *pd = htobe64(ctx->s[5]); pd++;
  *pd = htobe64(ctx->s[6]); pd++;
  *pd = htobe64(ctx->s[7]);
}
