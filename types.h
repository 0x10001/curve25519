#ifndef CURVE25519_TYPES_H
#define CURVE25519_TYPES_H

#include <stdint.h>

typedef struct _fe fe_t;
typedef struct _ge_p1 ge_p1_t;
typedef struct _ge_p2 ge_p2_t;
typedef struct _ge_p3 ge_p3_t;
typedef struct _ge_pc ge_pc_t;
typedef struct _ge_pz ge_pz_t;

struct _fe {
  int32_t d[10];
};

struct _ge_p1 {
  fe_t x;
  fe_t y;
  fe_t z;
  fe_t t;
};

struct _ge_p2 {
  fe_t x;
  fe_t y;
  fe_t z;
};

struct _ge_p3 {
  fe_t x;
  fe_t y;
  fe_t z;
  fe_t t;
};

struct _ge_pc {
  fe_t yplusx;
  fe_t yminusx;
  fe_t xyd2;
};

struct _ge_pz {
  fe_t yplusx;
  fe_t yminusx;
  fe_t td2;
  fe_t z;
};

#endif  /* CURVE25519_TYPES_H */
