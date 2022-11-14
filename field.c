#include <endian.h>

#include "field.h"

#include "field64.c"
#include "field32.c"

void fe_powt(fe_t *s, const fe_t *a) {
  fe_t e, x, y, z;
  fe_sqr(&y, a);
  fe_sqr(&x, &y);
  fe_sqr(&x, &x);
  fe_mul(&x, &x, a);
  fe_mul(&e, &y, &x);  /* e = a ** 11 */
  fe_sqr(&y, &e);
  fe_mul(&x, &y, &x);  /* x = a ** 31 = a ** (2 ** 5 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &x, &z);  /* y = a ** (2 ** 10 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 20 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&x, &z, &x);  /* x = a ** (2 ** 25 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 50 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 100 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 125 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 250 - 1) */

  fe_sqr(&y, &y);
  fe_sqr(&y, &y);  /* y = a ** (2 ** 252 - 4) */
  fe_mul(s, &y, a);
}

void fe_inv(fe_t *s, const fe_t *a) {
  fe_t e, x, y, z;
  fe_sqr(&y, a);
  fe_sqr(&x, &y);
  fe_sqr(&x, &x);
  fe_mul(&x, &x, a);
  fe_mul(&e, &y, &x);  /* a ** 11 */
  fe_sqr(&y, &e);
  fe_mul(&x, &y, &x);  /* x = a ** 31 = a ** (2 ** 5 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &x, &z);  /* y = a ** (2 ** 10 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 20 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&x, &z, &x);  /* x = a ** (2 ** 25 - 1) */

  fe_sqr(&z, &x);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 50 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 100 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &x);  /* y = a ** (2 ** 125 - 1) */

  fe_sqr(&z, &y);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_sqr(&z, &z);
  fe_mul(&y, &z, &y);  /* y = a ** (2 ** 250 - 1) */

  fe_sqr(&y, &y);
  fe_sqr(&y, &y);
  fe_sqr(&y, &y);
  fe_sqr(&y, &y);
  fe_sqr(&y, &y);  /* y = a ** (2 ** 255 - 32) */
  fe_mul(s, &y, &e);
}