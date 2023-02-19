#include <memory.h>
#include <endian.h>

#include "zint.h"
#include "field.h"
#include "group.h"
#include "scalar.h"
#include "constants.h"
#include "md.h"
#include "sha512.h"
#include "api.h"

static int32_t iszero(const uint8_t a[32]) {
  return loadle32(a) | loadle32(a + 4) | loadle32(a + 8) | loadle32(a + 12) |
    loadle32(a + 16) | loadle32(a + 20) | loadle32(a + 24) | loadle32(a + 28);
}

static int32_t diff(const uint8_t a[32], const uint8_t b[32]) {
  return (loadle32(a) ^ loadle32(b)) |
    (loadle32(a + 4) ^ loadle32(b + 4)) |
    (loadle32(a + 8) ^ loadle32(b + 8)) |
    (loadle32(a + 12) ^ loadle32(b + 12)) |
    (loadle32(a + 16) ^ loadle32(b + 16)) |
    (loadle32(a + 20) ^ loadle32(b + 20)) |
    (loadle32(a + 24) ^ loadle32(b + 24)) |
    (loadle32(a + 28) ^ loadle32(b + 28));
}

int32_t x25519_exchange(uint8_t s[32], const uint8_t k[32], const uint8_t p[32]) {
  fe_t x;
  fe_t xs, zs, xt, zt;
  fe_t *xd = &xs, *zd = &zs;
  fe_t *xa = &xt, *za = &zt;
  uint8_t e[32];
  intptr_t swap = 0;

  memcpy(e, k, sizeof(e));
  e[0] &= 0xf8;
  e[31] |= 0x40;
  fe_frombytes(&x, p);

  memcpy(xd, &k_fe_identity, sizeof(fe_t));
  memset(zd, 0, sizeof(fe_t));
  memcpy(xa, &x, sizeof(fe_t));
  memcpy(za, &k_fe_identity, sizeof(fe_t));

  for (int i = 254; i >= 0; i--) {
    fe_t u, v;
    intptr_t b = -((e[i >> 3] >> (i & 7)) & 1);
    intptr_t c;
    swap ^= b;
    c = ((intptr_t)xa ^ (intptr_t)xd) & swap;
    xa = (fe_t *)((intptr_t)xa ^ c);
    xd = (fe_t *)((intptr_t)xd ^ c);
    c = ((intptr_t)za ^ (intptr_t)zd) & swap;
    za = (fe_t *)((intptr_t)za ^ c);
    zd = (fe_t *)((intptr_t)zd ^ c);
    swap = b;

    fe_sub(&u, xa, za);
    fe_sub(&v, xd, zd);
    fe_add(xd, xd, zd);
    fe_add(zd, xa, za);
    fe_mul(za, &u, xd);
    fe_mul(zd, &v, zd);
    fe_sqr(&u, &v);
    fe_sqr(&v, xd);
    fe_add(xa, za, zd);
    fe_sub(zd, za, zd);
    fe_mul(xd, &v, &u);
    fe_sub(&v, &v, &u);
    fe_sqr(zd, zd);
    fe_mul121666(za, &v);
    fe_sqr(xa, xa);
    fe_add(&u, &u, za);
    fe_mul(za, &x, zd);
    fe_mul(zd, &u, &v);
  }

  fe_inv(&zs, &zs);
  fe_mul(&xs, &xs, &zs);
  fe_tobytes(s, &xs);
  memset(e, 0, sizeof(e));

  return iszero(s)? -1: 0;
}

void x25519_public_from_private(uint8_t pub[32], const uint8_t priv[32]) {
  uint8_t e[32];
  ge_p3_t p;
  fe_t u, v;

  memcpy(e, priv, 32);
  e[0] &= 0xf8;
  e[31] &= 0x7f;
  e[31] |= 0x40;

  ge_scmul(&p, e);

  fe_add(&u, &p.z, &p.y);
  fe_sub(&v, &p.z, &p.y);
  fe_inv(&v, &v);
  fe_mul(&u, &u, &v);
  fe_tobytes(pub, &u);

  memset(e, 0, sizeof(e));
}

void ed25519_privkey_from_bytes(ed_privkey_t *key, const uint8_t a[32]) {
  ge_p3_t g;

  md_hash(key->hash, NULL, &md_sha512, a, 32);
  memcpy(key->priv, a, 32);

  key->hash[0] &= 0xf8;
  key->hash[31] |= 0x40;
  key->hash[31] &= 0x7f;

  ge_scmul(&g, key->hash);
  ge_to_cache(key->cache, &g);
  ge_tobytes(key->pub, (const ge_p2_t *)&g);
}

void ed25519_sign(uint8_t sig[64], const ed_privkey_t *priv, const uint8_t *msg, size_t len) {
  md_ctx_t ctx;
  ge_p3_t g;
  uint8_t r[64];
  uint8_t h[64];

  md_init(&ctx, &md_sha512);
  md_update(&ctx, priv->hash + 32, 32);
  md_update(&ctx, msg, len);
  md_finalize(&ctx, r, NULL);

  sc_reduce(r);

  ge_scmul(&g, r);
  ge_tobytes(sig, (const ge_p2_t *)&g);

  sha512_init(&ctx);
  sha512_update(&ctx, sig, 32);
  sha512_update(&ctx, priv->pub, 32);
  sha512_update(&ctx, msg, len);
  sha512_finalize(&ctx, h);

  sc_reduce(h);
  sc_muladd(sig + 32, priv->hash, h, r);
}

int32_t ed25519_pubkey_from_bytes(ed_pubkey_t *key, const uint8_t a[32]) {
  ge_p3_t g;
  if (ge_frombytes(&g, a) < 0) {
    return -1;
  }

  memcpy(key->pub, a, 32);
  ge_to_cache(key->cache, &g);
  return 0;
}

int32_t ed25519_verify(const uint8_t sig[64], const ed_pubkey_t *pub, const uint8_t *msg, size_t len) {
  md_ctx_t ctx;
  ge_p2_t g;
  uint8_t r[32];
  uint8_t h[64];

  if (sc_verify(sig + 32) < 0) return -1;

  md_init(&ctx, &md_sha512);
  md_update(&ctx, sig, 32);
  md_update(&ctx, pub->pub, 32);
  md_update(&ctx, msg, len);
  md_finalize(&ctx, h, NULL);

  sc_reduce(h);
  ge_scmulsub(&g, sig + 32, h, pub->cache);
  ge_tobytes(r, &g);

  return diff(sig, r)? -1: 0;
}
