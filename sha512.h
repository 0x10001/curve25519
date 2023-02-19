#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>
#include <stddef.h>

#include "md.h"

void sha512_init(void *ctx);
void sha512_update(void *ctx, const uint8_t *msg, size_t len);
void sha512_finalize(void *ctx, uint8_t *dst);

static const md_t md_sha512 = {
  64,
  sha512_init,
  sha512_update,
  sha512_finalize,
};

#endif  /* SHA512_H */
