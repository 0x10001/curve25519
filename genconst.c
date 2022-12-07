#include <stdio.h>
#include <endian.h>
// #include <stdlib.h>

#include "field.h"

// k_d
// k_d2
// k_sqrtm1
// k_fe_identity
// k_p3_identity
// k_cache_identity
// k_base_cache
// k_base_table

typedef struct {
  fe_t x;
  fe_t y;
} ge_t;

static const char *fe_type_str = "fe_t";
static const char *gep3_type_str = "ge_p3_t";
static const char *gepc_type_str = "ge_pc_t";

static fe_t k_p;
static fe_t k_d;
static fe_t k_d2;
static fe_t k_sqrtm1;
static fe_t k_fe_identity = {{1}};

static ge_t k_ge_base;

static void fe_load(fe_t *e, const char *s) {
  uint32_t buf[8];

  sscanf(
    s,
    "0x%08x%08x%08x%08x%08x%08x%08x%08x",
    &buf[7], &buf[6], &buf[5], &buf[4], &buf[3], &buf[2], &buf[1], &buf[0]
  );
  buf[0] = htole32(buf[0]);
  buf[1] = htole32(buf[1]);
  buf[2] = htole32(buf[2]);
  buf[3] = htole32(buf[3]);
  buf[4] = htole32(buf[4]);
  buf[5] = htole32(buf[5]);
  buf[6] = htole32(buf[6]);
  buf[7] = htole32(buf[7]);
  fe_frombytes(e, (uint8_t *)buf);
}

static void fe_dump(FILE *fp, const fe_t *e, size_t ind, uint8_t end) {
  uint32_t buf[8];
  fe_t x;
  uint8_t space[8] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, };
  space[ind] = '\0';

  fe_tobytes((uint8_t *)buf, e);
  fe_frombytes(&x, (uint8_t *)buf);

#ifndef NDEBUG
  fe_tobytes((uint8_t *)buf, e);
  fprintf(
    fp,
    "%s0x%08x%08x%08x%08x%08x%08x%08x%08x%c\n",
    space, buf[7], buf[6], buf[5], buf[4], buf[3], buf[2], buf[1], buf[0], end
  );
#else  /* NDEBUG */

#  ifdef CURVE25519_USE128
  fprintf(
    fp,
    "%s{{ 0x%014lxL, 0x%014lxL, 0x%014lxL, 0x%014lxL, 0x%014lxL, }}%c\n",
    space, x.d[0], x.d[1], x.d[2], x.d[3], x.d[4], end
  );
#  else  /* !CURVE25519_USE128 */
  fprintf(
    fp,
    "%s{{ 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, }}%c\n",
    space, x.d[0], x.d[1], x.d[2], x.d[3], x.d[4], x.d[5], x.d[6], x.d[7], x.d[8], x.d[9], end
  );
#  endif  /* CURVE25519_USE128 */

#endif  /* NDEBUG */
}

static inline void fe_dump_colon(FILE *fp, const fe_t *e, size_t ind) {
  fe_dump(fp, e, ind, ';');
}

static inline void fe_dump_comma(FILE *fp, const fe_t *e, size_t ind) {
  fe_dump(fp, e, ind, ',');
}

static void ge_dump(FILE *fp, const ge_t *e, size_t ind) {
  fe_t t;
  size_t subind = ind + 2;
  uint8_t space[8] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, };
  space[ind] = '\0';

  fprintf(fp, "%s{\n", space);
  fe_add(&t, &e->y, &e->x);
  fe_dump_comma(fp, &t, subind);
  fe_sub(&t, &e->y, &e->x);
  fe_dump_comma(fp, &t, subind);
  fe_mul(&t, &e->y, &e->x);
  fe_mul(&t, &t, &k_d2);
  fe_dump_comma(fp, &t, subind);
  fprintf(fp, "%s},\n", space);
}

static void ge_add(ge_t *s, const ge_t *a, const ge_t *b) {
  fe_t xx, xy, yx, yy, u;
  fe_mul(&xx, &a->x, &b->x);
  fe_mul(&xy, &a->x, &b->y);
  fe_mul(&yx, &a->y, &b->x);
  fe_mul(&yy, &a->y, &b->y);
  fe_mul(&u, &xy, &yx);
  fe_mul(&u, &u, &k_d);

  fe_add(&s->x, &k_fe_identity, &u);
  fe_inv(&s->x, &s->x);
  fe_add(&xy, &xy, &yx);
  fe_mul(&s->x, &s->x, &xy);

  fe_sub(&s->y, &k_fe_identity, &u);
  fe_inv(&s->y, &s->y);
  fe_add(&xx, &xx, &yy);
  fe_mul(&s->y, &s->y, &xx);
}

/* k_sqrtm1 = 2 ** (2t+1) */
static inline void get_sqrtm1() {
  fe_t u = {{4}};
  fe_powt(&u, &u);
  fe_add(&k_sqrtm1, &u, &u);
}

static void init() {
  fe_load(&k_p, "0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed");
  fe_load(&k_d, "0x52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3");
  fe_load(&k_ge_base.x, "0x216936d3cd6e53fec0a4e231fdd6dc5c692cc7609525a7b2c9562d608f25d51a");
  fe_load(&k_ge_base.y, "0x6666666666666666666666666666666666666666666666666666666666666658");

  fe_add(&k_d2, &k_d, &k_d);

  get_sqrtm1();
}

static void dump_base_cache(FILE *fp) {
  ge_t e2, g;
  ge_add(&e2, &k_ge_base, &k_ge_base);

  fprintf(fp, "static const %s k_base_cache[8] = {\n", gepc_type_str);

  ge_dump(fp, &k_ge_base, 0);
  ge_add(&g, &e2, &k_ge_base);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);
  ge_dump(fp, &g, 0);
  ge_add(&g, &e2, &g);

  fprintf(fp, "};\n");
}

static void dump_base_table_row(FILE *fp, ge_t *e, size_t ind) {
  ge_t g;
  size_t subind = ind + 2;
  uint8_t space[8] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, };
  space[ind] = '\0';

  fprintf(fp, "%s{\n", space);
  ge_dump(fp, e, subind);
  ge_add(&g, e, e);
  ge_dump(fp, &g, subind);
  ge_add(&g, &g, e);
  ge_dump(fp, &g, subind);
  ge_add(&g, &g, e);
  ge_dump(fp, &g, subind);
  ge_add(&g, &g, e);
  ge_dump(fp, &g, subind);
  ge_add(&g, &g, e);
  ge_dump(fp, &g, subind);
  ge_add(&g, &g, e);
  ge_dump(fp, &g, subind);
  ge_add(&g, &g, e);
  ge_dump(fp, &g, subind);
  fprintf(fp, "%s},\n", space);

  ge_add(&g, &g, &g);
  ge_add(&g, &g, &g);
  ge_add(&g, &g, &g);
  ge_add(&g, &g, &g);
  ge_add(e, &g, &g);
}

static void dump_base_table(FILE *fp) {
  ge_t e = k_ge_base;
  fprintf(fp, "static const %s k_base_table[32][8] = {\n", gepc_type_str);
  for (size_t i = 0; i < 32; i++) {
    dump_base_table_row(fp, &e, 2);
  }
  fprintf(fp, "};\n");
}

static void run(FILE *fp) {
  fprintf(fp, "/* generated with genconst */\n\n#include \"types.h\"\n#include \"field.h\"\n\n");

  fprintf(fp, "static const %s k_fe_identity = {{1}};\n", fe_type_str);
  fprintf(fp, "static const %s k_p3_identity = {{}, {{1}}, {{1}}, {}};\n", gep3_type_str);
  fprintf(fp, "static const %s k_cache_identity = {{{1}}, {{1}}, {}};\n", gepc_type_str);

  fputc('\n', fp);

  fprintf(fp, "static const %s k_d = ", fe_type_str);
  fe_dump_colon(fp, &k_d, 0);
  fprintf(fp, "static const %s k_d2 = ", fe_type_str);
  fe_dump_colon(fp, &k_d2, 0);
  fprintf(fp, "static const %s k_sqrtm1 = ", fe_type_str);
  fe_dump_colon(fp, &k_sqrtm1, 0);

  fputc('\n', fp);
  dump_base_cache(fp);

  fputc('\n', fp);
  dump_base_table(fp);
}

int32_t main() {
  FILE *fp = fopen("constants.h", "w");
  init();
  run(fp);
  fclose(fp);
  return 0;
}
