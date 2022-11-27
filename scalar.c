#include "types.h"

#ifdef CURVE25519_USE128
#include "scalar64.c"
#else
#include "scalar32.c"
#endif  /* CURVE25519_USE128 */
