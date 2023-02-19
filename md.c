#include <memory.h>

#include "md.h"

void md_reset(md_ctx_t *ctx) {
  ctx->md->init(ctx->state);
}

void md_init(md_ctx_t *ctx, const md_t *md) {
  ctx->md = md;
  md->init(ctx->state);
}

void md_update(md_ctx_t *ctx, const uint8_t *msg, size_t len) {
  if (msg && len) {
    ctx->md->update(ctx->state, msg, len);
  }
}

void md_finalize(md_ctx_t *ctx, uint8_t *digest, size_t *plen) {
  if (digest) {
    ctx->md->finalize(ctx->state, digest);
  }
  if (plen) {
    *plen = ctx->md->size;
  }
}

void md_hash(uint8_t *digest, size_t *len, const md_t *md, const uint8_t *msg, size_t msg_len) {
  md_ctx_t ctx;
  md_init(&ctx, md);
  md_update(&ctx, msg, msg_len);
  md_finalize(&ctx, digest, len);
  memset(&ctx, 0, sizeof(ctx));
}
