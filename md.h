#ifndef MD_H
#define MD_H

#include <stdint.h>
#include <stddef.h>

#define MD_MAX_SIZE 256

typedef struct _md md_t;
typedef struct _md_ctx md_ctx_t;

struct _md {
  size_t size;
  void (*init)(void *ctx);
  void (*update)(void *ctx, const uint8_t *data, size_t len);
  void (*finalize)(void *ctx, uint8_t *digest);
};

struct _md_ctx {
  const md_t *md;
  uint8_t state[MD_MAX_SIZE];
};

void md_reset(md_ctx_t *ctx);
void md_init(md_ctx_t *ctx, const md_t *md);
void md_update(md_ctx_t *ctx, const uint8_t *msg, size_t len);
void md_finalize(md_ctx_t *ctx, uint8_t *digest, size_t *len);
void md_hash(uint8_t *digest, size_t *len, const md_t *md, const uint8_t *msg, size_t msg_len);

#undef MD_MAX_SIZE

#endif  /* MD_H */
