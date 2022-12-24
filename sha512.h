#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint8_t buf[128];
  uint64_t s[8];
  uint32_t len;
} sha512_ctx_t;

void sha512_init(sha512_ctx_t *ctx);
void sha512_update(sha512_ctx_t *ctx, const uint8_t *src, size_t n);
void sha512_finalize(sha512_ctx_t *ctx, uint8_t *dst);

#endif  /* SHA512_H */
