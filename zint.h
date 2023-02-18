#ifndef ZINT_H
#define ZINT_H

#include <stdint.h>

/* 64-bit integers */

static inline uint64_t loadbe64(const uint8_t b[8]) {
  uint64_t r = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
  uint32_t t = (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | b[7];
  return (r << 32) | t;
}

static inline uint64_t loadle64(const uint8_t b[8]) {
  uint64_t r = (b[7] << 24) | (b[6] << 16) | (b[5] << 8) | b[4];
  uint32_t t = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
  return (r << 32) | t;
}

static inline void dumpbe64(uint8_t b[8], uint64_t r) {
  b[0] = r >> 56;
  b[1] = r >> 48;
  b[2] = r >> 40;
  b[3] = r >> 32;
  b[4] = r >> 24;
  b[5] = r >> 16;
  b[6] = r >> 8;
  b[7] = r;
}

static inline void dumple64(uint8_t b[8], uint64_t r) {
  b[0] = r;
  b[1] = r >> 8;
  b[2] = r >> 16;
  b[3] = r >> 24;
  b[4] = r >> 32;
  b[5] = r >> 40;
  b[6] = r >> 48;
  b[7] = r >> 56;
}

static inline uint64_t ror64(uint64_t r, uint8_t k) {
  return (r >> k) | (r << (sizeof(r) * 8 - k));
}

static inline uint64_t rol64(uint64_t r, uint8_t k) {
  return (r << k) | (r >> (sizeof(r) * 8 - k));
}

/* 32-bit integers */

static inline uint32_t loadbe32(const uint8_t b[4]) {
  uint32_t r = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
  return r;
}

static inline uint32_t loadle32(const uint8_t b[4]) {
  uint32_t r = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
  return r;
}

static inline void dumpbe32(uint8_t b[4], uint32_t r) {
  b[0] = r >> 24;
  b[1] = r >> 16;
  b[2] = r >> 8;
  b[3] = r;
}

static inline void dumple32(uint8_t b[4], uint32_t r) {
  b[0] = r;
  b[1] = r >> 8;
  b[2] = r >> 16;
  b[3] = r >> 24;
}

static inline uint32_t ror32(uint32_t r, uint8_t k) {
  return (r >> k) | (r << (sizeof(r) * 8 - k));
}

static inline uint32_t rol32(uint32_t r, uint8_t k) {
  return (r << k) | (r >> (sizeof(r) * 8 - k));
}

/* 16-bit integers */

static inline uint16_t loadbe16(const uint8_t b[2]) {
  uint16_t r = (b[0] << 8) | b[1];
  return r;
}

static inline uint16_t loadle16(const uint8_t b[2]) {
  uint16_t r = (b[1] << 8) | b[0];
  return r;
}

static inline void dumpbe16(uint8_t b[2], uint16_t r) {
  b[0] = r >> 8;
  b[1] = r;
}

static inline void dumple16(uint8_t b[2], uint16_t r) {
  b[0] = r;
  b[1] = r >> 8;
}

static inline uint16_t ror16(uint16_t r, uint8_t k) {
  return (r >> k) | (r << (sizeof(r) * 8 - k));
}

static inline uint16_t rol16(uint16_t r, uint8_t k) {
  return (r << k) | (r >> (sizeof(r) * 8 - k));
}

#endif  /* ZINT_H */
