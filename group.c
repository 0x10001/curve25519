#include <memory.h>

#include "constants.h"
#include "field.h"

static inline void ge_to_p2(ge_p2_t *s, const ge_p1_t *g) {
  fe_mul(&s->x, &g->x, &g->t);
  fe_mul(&s->y, &g->z, &g->y);
  fe_mul(&s->z, &g->z, &g->t);
}

static inline void ge_to_p3(ge_p3_t *s, const ge_p1_t *g) {
  fe_mul(&s->x, &g->x, &g->t);
  fe_mul(&s->y, &g->z, &g->y);
  fe_mul(&s->z, &g->z, &g->t);
  fe_mul(&s->t, &g->x, &g->y);
}

static void ge_to_pz(ge_pz_t *s, const ge_p3_t *e) {
  fe_add(&s->yplusx, &e->y, &e->x);
  fe_sub(&s->yminusx, &e->y, &e->x);
  fe_mul(&s->td2, &e->t, &k_d2);
  memcpy(&s->z, &e->z, sizeof(s->z));
}

static inline void ge_cmov(ge_pc_t *s, const ge_pc_t *g, int32_t m) {
  fe_cmov(&s->yplusx, &g->yplusx, m);
  fe_cmov(&s->yminusx, &g->yminusx, m);
  fe_cmov(&s->xyd2, &g->xyd2, m);
}

static inline void ge_cneg(ge_pc_t *s, int32_t m) {
  fe_cswap(&s->yplusx, &s->yminusx, m);
  fe_cneg(&s->xyd2, m);
}

static inline int32_t equal(int8_t a, int8_t b) {
  return ((int32_t)(a ^ b) - 1) >> 31;
}

static void lookup_table(ge_pc_t *s, int i, int8_t j) {
  int8_t neg = j >> 7;
  int8_t abs = j - ((j << 1) & neg);
  const ge_pc_t *t = k_base_table[i];

  memcpy(s, &k_cache_identity, sizeof(*s));

  ge_cmov(s, t + 0, equal(abs, 1));
  ge_cmov(s, t + 1, equal(abs, 2));
  ge_cmov(s, t + 2, equal(abs, 3));
  ge_cmov(s, t + 3, equal(abs, 4));
  ge_cmov(s, t + 4, equal(abs, 5));
  ge_cmov(s, t + 5, equal(abs, 6));
  ge_cmov(s, t + 6, equal(abs, 7));
  ge_cmov(s, t + 7, equal(abs, 8));
  ge_cneg(s, neg);
}

static void ge_addc(ge_p1_t *s, const ge_p3_t *a, const ge_pc_t *b) {
  fe_t w;
  fe_add(&s->x, &a->y, &a->x);
  fe_sub(&s->y, &a->y, &a->x);
  fe_mul(&s->z, &s->x, &b->yplusx);
  fe_mul(&s->y, &s->y, &b->yminusx);
  fe_mul(&s->t, &a->t, &b->xyd2);
  fe_add(&w, &a->z, &a->z);
  fe_sub(&s->x, &s->z, &s->y);
  fe_add(&s->y, &s->z, &s->y);
  fe_add(&s->z, &w, &s->t);
  fe_sub(&s->t, &w, &s->t);
}

static void ge_addz(ge_p1_t *s, const ge_p3_t *a, const ge_pz_t *b) {
  fe_t w;
  fe_add(&s->x, &a->y, &a->x);
  fe_sub(&s->y, &a->y, &a->x);
  fe_mul(&s->z, &s->x, &b->yplusx);
  fe_mul(&s->y, &s->y, &b->yminusx);
  fe_mul(&s->t, &a->t, &b->td2);
  fe_add(&w, &a->z, &a->z);
  fe_mul(&w, &w, &b->z);
  fe_sub(&s->x, &s->z, &s->y);
  fe_add(&s->y, &s->z, &s->y);
  fe_add(&s->z, &w, &s->t);
  fe_sub(&s->t, &w, &s->t);
}

static void ge_subc(ge_p1_t *s, const ge_p3_t *a, const ge_pc_t *b) {
  fe_t w;
  fe_add(&s->x, &a->y, &a->x);
  fe_sub(&s->y, &a->y, &a->x);
  fe_mul(&s->z, &s->x, &b->yminusx);
  fe_mul(&s->y, &s->y, &b->yplusx);
  fe_mul(&s->t, &a->t, &b->xyd2);
  fe_add(&w, &a->z, &a->z);
  fe_sub(&s->x, &s->z, &s->y);
  fe_add(&s->y, &s->z, &s->y);
  fe_sub(&s->z, &w, &s->t);
  fe_add(&s->t, &w, &s->t);
}

static void ge_subz(ge_p1_t *s, const ge_p3_t *a, const ge_pz_t *b) {
  fe_t w;
  fe_add(&s->x, &a->y, &a->x);
  fe_sub(&s->y, &a->y, &a->x);
  fe_mul(&s->z, &s->x, &b->yminusx);
  fe_mul(&s->y, &s->y, &b->yplusx);
  fe_mul(&s->t, &a->t, &b->td2);
  fe_add(&w, &a->z, &a->z);
  fe_mul(&w, &w, &b->z);
  fe_sub(&s->x, &s->z, &s->y);
  fe_add(&s->y, &s->z, &s->y);
  fe_sub(&s->z, &w, &s->t);
  fe_add(&s->t, &w, &s->t);
}

static void ge_dbl(ge_p1_t *s, const ge_p2_t *a) {
  fe_t w;
  fe_sqr(&s->x, &a->x);
  fe_sqr(&s->y, &a->z);
  fe_add(&s->t, &s->y, &s->y);
  fe_sqr(&s->z, &a->y);
  fe_add(&s->y, &a->x, &a->y);
  fe_sqr(&w, &s->y);
  fe_add(&s->y, &s->z, &s->x);
  fe_sub(&s->z, &s->z, &s->x);
  fe_sub(&s->x, &w, &s->y);
  fe_sub(&s->t, &s->t, &s->z);
}

void ge_scmul(ge_p3_t *s, const uint8_t a[32]) {
  int8_t n[64] = {
    a[ 0] & 0xf, a[ 0] >> 4,
    a[ 1] & 0xf, a[ 1] >> 4,
    a[ 2] & 0xf, a[ 2] >> 4,
    a[ 3] & 0xf, a[ 3] >> 4,
    a[ 4] & 0xf, a[ 4] >> 4,
    a[ 5] & 0xf, a[ 5] >> 4,
    a[ 6] & 0xf, a[ 6] >> 4,
    a[ 7] & 0xf, a[ 7] >> 4,
    a[ 8] & 0xf, a[ 8] >> 4,
    a[ 9] & 0xf, a[ 9] >> 4,
    a[10] & 0xf, a[10] >> 4,
    a[11] & 0xf, a[11] >> 4,
    a[12] & 0xf, a[12] >> 4,
    a[13] & 0xf, a[13] >> 4,
    a[14] & 0xf, a[14] >> 4,
    a[15] & 0xf, a[15] >> 4,
    a[16] & 0xf, a[16] >> 4,
    a[17] & 0xf, a[17] >> 4,
    a[18] & 0xf, a[18] >> 4,
    a[19] & 0xf, a[19] >> 4,
    a[20] & 0xf, a[20] >> 4,
    a[21] & 0xf, a[21] >> 4,
    a[22] & 0xf, a[22] >> 4,
    a[23] & 0xf, a[23] >> 4,
    a[24] & 0xf, a[24] >> 4,
    a[25] & 0xf, a[25] >> 4,
    a[26] & 0xf, a[26] >> 4,
    a[27] & 0xf, a[27] >> 4,
    a[28] & 0xf, a[28] >> 4,
    a[29] & 0xf, a[29] >> 4,
    a[30] & 0xf, a[30] >> 4,
    a[31] & 0xf, a[31] >> 4,
  };

  int8_t c;
  c = (n[ 0] + 8) >> 4; n[ 0] -= c << 4; n[ 1] += c;
  c = (n[ 1] + 8) >> 4; n[ 1] -= c << 4; n[ 2] += c;
  c = (n[ 2] + 8) >> 4; n[ 2] -= c << 4; n[ 3] += c;
  c = (n[ 3] + 8) >> 4; n[ 3] -= c << 4; n[ 4] += c;
  c = (n[ 4] + 8) >> 4; n[ 4] -= c << 4; n[ 5] += c;
  c = (n[ 5] + 8) >> 4; n[ 5] -= c << 4; n[ 6] += c;
  c = (n[ 6] + 8) >> 4; n[ 6] -= c << 4; n[ 7] += c;
  c = (n[ 7] + 8) >> 4; n[ 7] -= c << 4; n[ 8] += c;
  c = (n[ 8] + 8) >> 4; n[ 8] -= c << 4; n[ 9] += c;
  c = (n[ 9] + 8) >> 4; n[ 9] -= c << 4; n[10] += c;
  c = (n[10] + 8) >> 4; n[10] -= c << 4; n[11] += c;
  c = (n[11] + 8) >> 4; n[11] -= c << 4; n[12] += c;
  c = (n[12] + 8) >> 4; n[12] -= c << 4; n[13] += c;
  c = (n[13] + 8) >> 4; n[13] -= c << 4; n[14] += c;
  c = (n[14] + 8) >> 4; n[14] -= c << 4; n[15] += c;
  c = (n[15] + 8) >> 4; n[15] -= c << 4; n[16] += c;
  c = (n[16] + 8) >> 4; n[16] -= c << 4; n[17] += c;
  c = (n[17] + 8) >> 4; n[17] -= c << 4; n[18] += c;
  c = (n[18] + 8) >> 4; n[18] -= c << 4; n[19] += c;
  c = (n[19] + 8) >> 4; n[19] -= c << 4; n[20] += c;
  c = (n[20] + 8) >> 4; n[20] -= c << 4; n[21] += c;
  c = (n[21] + 8) >> 4; n[21] -= c << 4; n[22] += c;
  c = (n[22] + 8) >> 4; n[22] -= c << 4; n[23] += c;
  c = (n[23] + 8) >> 4; n[23] -= c << 4; n[24] += c;
  c = (n[24] + 8) >> 4; n[24] -= c << 4; n[25] += c;
  c = (n[25] + 8) >> 4; n[25] -= c << 4; n[26] += c;
  c = (n[26] + 8) >> 4; n[26] -= c << 4; n[27] += c;
  c = (n[27] + 8) >> 4; n[27] -= c << 4; n[28] += c;
  c = (n[28] + 8) >> 4; n[28] -= c << 4; n[29] += c;
  c = (n[29] + 8) >> 4; n[29] -= c << 4; n[30] += c;
  c = (n[30] + 8) >> 4; n[30] -= c << 4; n[31] += c;
  c = (n[31] + 8) >> 4; n[31] -= c << 4; n[32] += c;
  c = (n[32] + 8) >> 4; n[32] -= c << 4; n[33] += c;
  c = (n[33] + 8) >> 4; n[33] -= c << 4; n[34] += c;
  c = (n[34] + 8) >> 4; n[34] -= c << 4; n[35] += c;
  c = (n[35] + 8) >> 4; n[35] -= c << 4; n[36] += c;
  c = (n[36] + 8) >> 4; n[36] -= c << 4; n[37] += c;
  c = (n[37] + 8) >> 4; n[37] -= c << 4; n[38] += c;
  c = (n[38] + 8) >> 4; n[38] -= c << 4; n[39] += c;
  c = (n[39] + 8) >> 4; n[39] -= c << 4; n[40] += c;
  c = (n[40] + 8) >> 4; n[40] -= c << 4; n[41] += c;
  c = (n[41] + 8) >> 4; n[41] -= c << 4; n[42] += c;
  c = (n[42] + 8) >> 4; n[42] -= c << 4; n[43] += c;
  c = (n[43] + 8) >> 4; n[43] -= c << 4; n[44] += c;
  c = (n[44] + 8) >> 4; n[44] -= c << 4; n[45] += c;
  c = (n[45] + 8) >> 4; n[45] -= c << 4; n[46] += c;
  c = (n[46] + 8) >> 4; n[46] -= c << 4; n[47] += c;
  c = (n[47] + 8) >> 4; n[47] -= c << 4; n[48] += c;
  c = (n[48] + 8) >> 4; n[48] -= c << 4; n[49] += c;
  c = (n[49] + 8) >> 4; n[49] -= c << 4; n[50] += c;
  c = (n[50] + 8) >> 4; n[50] -= c << 4; n[51] += c;
  c = (n[51] + 8) >> 4; n[51] -= c << 4; n[52] += c;
  c = (n[52] + 8) >> 4; n[52] -= c << 4; n[53] += c;
  c = (n[53] + 8) >> 4; n[53] -= c << 4; n[54] += c;
  c = (n[54] + 8) >> 4; n[54] -= c << 4; n[55] += c;
  c = (n[55] + 8) >> 4; n[55] -= c << 4; n[56] += c;
  c = (n[56] + 8) >> 4; n[56] -= c << 4; n[57] += c;
  c = (n[57] + 8) >> 4; n[57] -= c << 4; n[58] += c;
  c = (n[58] + 8) >> 4; n[58] -= c << 4; n[59] += c;
  c = (n[59] + 8) >> 4; n[59] -= c << 4; n[60] += c;
  c = (n[60] + 8) >> 4; n[60] -= c << 4; n[61] += c;
  c = (n[61] + 8) >> 4; n[61] -= c << 4; n[62] += c;
  c = (n[62] + 8) >> 4; n[62] -= c << 4; n[63] += c;

  ge_p1_t r;
  ge_pc_t cache;

  memcpy(s, &k_p3_identity, sizeof(*s));

  lookup_table(&cache,  0, n[ 1]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  1, n[ 3]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  2, n[ 5]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  3, n[ 7]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  4, n[ 9]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  5, n[11]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  6, n[13]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  7, n[15]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  8, n[17]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  9, n[19]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 10, n[21]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 11, n[23]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 12, n[25]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 13, n[27]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 14, n[29]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 15, n[31]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 16, n[33]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 17, n[35]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 18, n[37]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 19, n[39]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 20, n[41]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 21, n[43]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 22, n[45]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 23, n[47]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 24, n[49]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 25, n[51]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 26, n[53]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 27, n[55]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 28, n[57]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 29, n[59]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 30, n[61]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 31, n[63]); ge_addc(&r, s, &cache); ge_to_p2((ge_p2_t *)s, &r);

  ge_dbl(&r, (ge_p2_t *)s);
  ge_to_p2((ge_p2_t *)s, &r);
  ge_dbl(&r, (ge_p2_t *)s);
  ge_to_p2((ge_p2_t *)s, &r);
  ge_dbl(&r, (ge_p2_t *)s);
  ge_to_p2((ge_p2_t *)s, &r);
  ge_dbl(&r, (ge_p2_t *)s);
  ge_to_p3(s, &r);

  lookup_table(&cache,  0, n[ 0]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  1, n[ 2]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  2, n[ 4]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  3, n[ 6]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  4, n[ 8]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  5, n[10]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  6, n[12]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  7, n[14]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  8, n[16]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache,  9, n[18]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 10, n[20]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 11, n[22]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 12, n[24]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 13, n[26]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 14, n[28]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 15, n[30]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 16, n[32]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 17, n[34]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 18, n[36]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 19, n[38]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 20, n[40]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 21, n[42]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 22, n[44]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 23, n[46]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 24, n[48]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 25, n[50]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 26, n[52]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 27, n[54]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 28, n[56]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 29, n[58]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 30, n[60]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);
  lookup_table(&cache, 31, n[62]); ge_addc(&r, s, &cache); ge_to_p3(s, &r);

  memset(n, 0, sizeof(n));
}

static void slide(int8_t s[256], const uint8_t a[32]) {
  int t = a[0];
  int b = 0;
  for (int i = 1; i < 32; i++) {
    t += a[i] << 8;
    for (int j = 0; j < 8; b++, j++, t >>= 1) {
      if (t & 1) {
        s[b] = (t & 0xf) - (t & 0x10);
        t -= s[b];
      } else {
        s[b] = 0;
      }
    }
  }
  for (int j = 0; j < 8; b++, j++, t >>= 1) {
    if (t & 1) {
      s[b] = (t & 0xf) - (t & 0x10);
      t -= s[b];
    } else {
      s[b] = 0;
    }
  }
}

void ge_to_cache(ge_pz_t *s, const ge_p3_t *e) {
  ge_p1_t t;
  ge_p3_t e2, p;
  ge_to_pz(s + 0, e);
  ge_dbl(&t, (const ge_p2_t *)e); ge_to_p3(&e2, &t);
  ge_addz(&t, &e2, s); ge_to_p3(&p, &t); ge_to_pz(s + 1, &p);
  ge_addz(&t, &e2, s + 1); ge_to_p3(&p, &t); ge_to_pz(s + 2, &p);
  ge_addz(&t, &e2, s + 2); ge_to_p3(&p, &t); ge_to_pz(s + 3, &p);
  ge_addz(&t, &e2, s + 3); ge_to_p3(&p, &t); ge_to_pz(s + 4, &p);
  ge_addz(&t, &e2, s + 4); ge_to_p3(&p, &t); ge_to_pz(s + 5, &p);
  ge_addz(&t, &e2, s + 5); ge_to_p3(&p, &t); ge_to_pz(s + 6, &p);
  ge_addz(&t, &e2, s + 6); ge_to_p3(&p, &t); ge_to_pz(s + 7, &p);
}

void ge_scmulsub(ge_p2_t *s, const uint8_t a[32], const uint8_t b[32], const ge_pz_t p[8]) {
  int8_t ae[256];
  int8_t be[256];
  int i = 255;

  slide(ae, a);
  slide(be, b);

  memcpy(s, &k_p3_identity, sizeof(*s));

  while (i >= 0 && !(ae[i] || be[i])) {
    i--;
  }

  ge_p1_t t;
  ge_p3_t u;
  for ( ; i >= 0; i--) {
    ge_dbl(&t, s);

    if (ae[i] > 0) {
      ge_to_p3(&u, &t);
      ge_addc(&t, &u, &k_base_cache[ae[i] >> 1]);
    } else if (ae[i] < 0) {
      ge_to_p3(&u, &t);
      ge_subc(&t, &u, &k_base_cache[-ae[i] >> 1]);
    }

    if (be[i] > 0) {
      ge_to_p3(&u, &t);
      ge_subz(&t, &u, &p[be[i] >> 1]);
    } else if (be[i] < 0) {
      ge_to_p3(&u, &t);
      ge_addz(&t, &u, &p[-be[i] >> 1]);
    }

    ge_to_p2(s, &t);
  }
}

void ge_tobytes(uint8_t s[32], const ge_p2_t *e) {
  fe_t v, t;
  fe_inv(&v, &e->z);
  fe_mul(&t, &e->y, &v);
  fe_tobytes(s, &t);
  fe_mul(&t, &e->x, &v);
  s[31] |= fe_isneg(&t) << 7;
}

int32_t ge_frombytes(ge_p3_t *e, const uint8_t s[32]) {
  fe_t u, v;
  fe_t t, r;

  memcpy(&e->z, &k_fe_identity, sizeof(e->z));
  fe_frombytes(&e->y, s);
  fe_sqr(&u, &e->y);
  fe_mul(&v, &u, &k_d);
  fe_sub(&u, &u, &e->z);  /* u = yy - 1 */
  fe_add(&v, &v, &e->z);  /* v = dyy + 1 */

  fe_sqr(&t, &v);
  fe_mul(&t, &t, &v);
  fe_sqr(&e->x, &t);
  fe_mul(&e->x, &e->x, &v);
  fe_mul(&e->x, &e->x, &u);  /* x = u(v ** 7) */

  fe_powt(&e->x, &e->x);
  fe_mul(&e->x, &e->x, &u);
  fe_mul(&e->x, &e->x, &t);  /* x = u ** (t + 1) * v ** (7t + 3) */

  fe_sqr(&t, &e->x);
  fe_mul(&t, &t, &v);
  fe_sub(&r, &t, &u);
  if (!fe_iszero(&r)) {
    fe_add(&r, &t, &u);
    if (!fe_iszero(&r)) {
      return -1;
    }
    fe_mul(&e->x, &e->x, &k_sqrtm1);
  }

  if (fe_isneg(&e->x) != s[31] >> 7) {
    fe_neg(&e->x, &e->x);
  }

  fe_mul(&e->t, &e->x, &e->y);
  return 0;
}
