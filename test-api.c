#include <stdio.h>
#include <endian.h>
#include <memory.h>

#include "types.h"
#include "scalar.h"
#include "field.h"
#include "group.h"
#include "api.h"


static int32_t diff(int32_t a[8], int32_t b[8]) {
  return (a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) | (a[3] ^ b[3]) | (a[4] ^ b[4]) |
  (a[5] ^ b[5]) | (a[6] ^ b[6]) | (a[7] ^ b[7]);
}

static int test_sc_reduce(int k, FILE *fp) {
  int ret = 0;
  int32_t x[16], y[16];
  int32_t r[8];
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
      x + 15, x + 14, x + 13, x + 12, x + 11, x + 10, x + 9, x + 8,
      x + 7, x + 6, x + 5, x + 4, x + 3, x + 2, x + 1, x
    );
    memcpy(y, x, sizeof(x));
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    sc_reduce((uint8_t *)y);
    if (diff(y, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[sc_reduce]\n%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        x[15], x[14], x[13], x[12], x[11], x[10], x[9], x[8],
        x[7], x[6], x[5], x[4], x[3], x[2], x[1], x[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        y[7], y[6], y[5], y[4], y[3], y[2], y[1], y[0]
      );
    }
  }
  return ret;
}

static int test_sc_muladd(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], b[8], c[8], t[8];
  int32_t r[8];
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      b + 7, b + 6, b + 5, b + 4, b + 3, b + 2, b + 1, b
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      c + 7, c + 6, c + 5, c + 4, c + 3, c + 2, c + 1, c
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    sc_muladd((uint8_t *)t, (uint8_t *)a, (uint8_t *)b, (uint8_t *)c);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[sc_muladd]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0],
        c[7], c[6], c[5], c[4], c[3], c[2], c[1], c[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_fe_mul(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], b[8], t[8];
  int32_t r[8];
  fe_t g, h;
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      b + 7, b + 6, b + 5, b + 4, b + 3, b + 2, b + 1, b
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    fe_frombytes(&g, (uint8_t *)a);
    fe_frombytes(&h, (uint8_t *)b);
    fe_mul(&g, &g, &h);
    fe_tobytes((uint8_t *)t, &g);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[fe_mul]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_fe_sqr(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], t[8];
  int32_t r[8];
  fe_t g;
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    fe_frombytes(&g, (uint8_t *)a);
    fe_sqr(&g, &g);
    fe_tobytes((uint8_t *)t, &g);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[fe_mul]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_fe_inv(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], t[8];
  int32_t r[8];
  fe_t g;
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    fe_frombytes(&g, (uint8_t *)a);
    fe_inv(&g, &g);
    fe_tobytes((uint8_t *)t, &g);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[fe_mul]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_fe_powt(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], t[8];
  int32_t r[8];
  fe_t g;
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    fe_frombytes(&g, (uint8_t *)a);
    fe_powt(&g, &g);
    fe_tobytes((uint8_t *)t, &g);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[fe_mul]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_ge_scmul(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], t[8];
  int32_t r[8];
  ge_p3_t g;
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    ge_scmul(&g, (uint8_t *)a);
    ge_tobytes((uint8_t *)t, (const ge_p2_t *)&g);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[ge_scmul]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_ge_scmulsub(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], b[8], c[8], t[8];
  int32_t r[8];
  ge_p3_t g;
  ge_pz_t p[8];
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      b + 7, b + 6, b + 5, b + 4, b + 3, b + 2, b + 1, b
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      c + 7, c + 6, c + 5, c + 4, c + 3, c + 2, c + 1, c
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    ge_frombytes(&g, (uint8_t *)c);
    ge_to_cache(p, &g);
    ge_scmulsub((ge_p2_t *)&g, (uint8_t *)a, (uint8_t *)b, p);
    ge_tobytes((uint8_t *)t, (const ge_p2_t *)&g);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[ge_scmulsub]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0],
        c[7], c[6], c[5], c[4], c[3], c[2], c[1], c[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_x25519_from_private(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], t[8];
  int32_t r[8];
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    x25519_public_from_private((uint8_t *)t, (uint8_t *)a);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[x25519_from_private]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_x25519_xchg(int k, FILE *fp) {
  int ret = 0;
  int32_t a[8], b[8], t[8];
  int32_t r[8];
  while (k--) {
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      a + 7, a + 6, a + 5, a + 4, a + 3, a + 2, a + 1, a
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      b + 7, b + 6, b + 5, b + 4, b + 3, b + 2, b + 1, b
    );
    fscanf(fp,
      "%08x%08x%08x%08x%08x%08x%08x%08x",
      r + 7, r + 6, r + 5, r + 4, r + 3, r + 2, r + 1, r
    );
    x25519_exchange((uint8_t *)t, (uint8_t *)a, (uint8_t *)b);
    if (diff(t, r)) {
      ret = -1;
      fprintf(
        stderr,
        "[x25519_xchg]\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[expected]\n%08x%08x%08x%08x%08x%08x%08x%08x\n"
        "[actual]\n%08x%08x%08x%08x%08x%08x%08x%08x\n\n",
        a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0],
        b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0],
        r[7], r[6], r[5], r[4], r[3], r[2], r[1], r[0],
        t[7], t[6], t[5], t[4], t[3], t[2], t[1], t[0]
      );
    }
  }
  return ret;
}

static int test_ed25519_sign_verify(int k, FILE *fp) {
  int ret = 0;
  uint8_t a[32], p[32], t[64];
  uint8_t r[64];
  ed_privkey_t privkey;
  ed_pubkey_t pubkey;
  uint8_t msg[256];
  size_t len;
  while (k--) {
    for (size_t i = 0; i < 32; i++) {
      fscanf(fp, "%02hhx", a + i);
    }
    for (size_t i = 0; i < 32; i++) {
      fscanf(fp, "%02hhx", p + i);
    }
    fscanf(fp, "%lu", &len);
    for (size_t i = 0; i < len; i++) {
      fscanf(fp, "%02hhx", msg + i);
    }
    for (size_t i = 0; i < 64; i++) {
      fscanf(fp, "%02hhx", r + i);
    }

    ed25519_privkey_from_bytes(&privkey, a);
    ed25519_sign(t, &privkey, msg, len);
    if (memcmp(t, r, 64)) {
      ret = -1;
      fprintf(stderr, "[ed25519_sign]\n");
      for (size_t i = 0; i < 64; i++) {
        fprintf(stderr, "%02hhx", r[i]);
      }
      fputc('\n', stderr);
      for (size_t i = 0; i < 64; i++) {
        fprintf(stderr, "%02hhx", t[i]);
      }
      fputc('\n', stderr);
    }

    ed25519_pubkey_from_bytes(&pubkey, p);
    int z = ed25519_verify(r, &pubkey, msg, len);
    if (z) {
      ret = -1;
      fprintf(stderr, "[ed25519_verify]\n");
      for (size_t i = 0; i < 64; i++) {
        fprintf(stderr, "%02hhx", t[i]);
      }
      fputc('\n', stderr);
    }
  }
  return ret;
}

int main() {
  int n;
  int ret = 0;
  FILE *fp = fopen("test.in", "r");

  fscanf(fp, "%d", &n);
  ret |= test_sc_reduce(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_sc_muladd(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_fe_mul(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_fe_sqr(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_fe_inv(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_fe_powt(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_ge_scmul(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_ge_scmulsub(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_x25519_from_private(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_x25519_xchg(n, fp);
  fscanf(fp, "%d", &n);
  ret |= test_ed25519_sign_verify(n, fp);
  fclose(fp);
  return ret;
}
