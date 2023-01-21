#ifndef CURVE25519_TYPES_H
#define CURVE25519_TYPES_H

#ifdef __SIZEOF_INT128__

#ifndef CURVE25519_USE128
#define CURVE25519_USE128

typedef __int128_t int128_t;
typedef __uint128_t uint128_t;

#endif  /* CURVE25519_USE128 */

#endif  /* __SIZEOF_INT128__ */

#include <stdint.h>

#ifdef CURVE25519_USE128
typedef struct {
  int64_t d[5];
} fe_t;
#else
typedef struct {
  int32_t d[10];
} fe_t;
#endif  /* CURVE25519_USE128 */

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
