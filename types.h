#ifndef CURVE25519_TYPES_H
#define CURVE25519_TYPES_H

#include <stdint.h>

typedef struct {
  int32_t d[10];
} fe_t;

typedef struct {
  fe_t x;
  fe_t y;
  fe_t z;
  fe_t t;
} ge_p1_t;

typedef struct {
  fe_t x;
  fe_t y;
  fe_t z;
} ge_p2_t;

typedef struct {
  fe_t x;
  fe_t y;
  fe_t z;
  fe_t t;
} ge_p3_t;

typedef struct {
  fe_t yplusx;
  fe_t yminusx;
  fe_t xyd2;
} ge_pc_t;

typedef struct {
  fe_t yplusx;
  fe_t yminusx;
  fe_t td2;
  fe_t z;
} ge_pz_t;

#endif  /* CURVE25519_TYPES_H */
