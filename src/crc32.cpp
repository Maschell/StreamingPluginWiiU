#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "crc32.h"

void crc32_init(crc32_t* crc) {
  uint32_t v;

  for(int i = 0; i < 256; ++i) {
    v = i;

    for(int j = 0; j < 8; ++j) {
      v = (v & 1) ? (CRC32_INITIAL ^ (v >> 1)) : (v >> 1);
    }
    crc->table[i] = v;
  }
}

void crc32_update(crc32_t* c, uint8_t* buf, size_t len) {
  for(size_t i = 0; i < len; ++i) {
    c->value = c->table[(c->value ^ buf[i]) & 0xFF] ^ (c->value >> 8);
  }
}

void crc32_start(crc32_t* c) {
  c->value = 0xfffffffful;
}

uint32_t crc32_finalize(crc32_t* c) {
  return c->value ^ 0xfffffffful;
}

uint32_t crc32_crc(crc32_t* c, uint8_t* buf, size_t len) {
  crc32_start(c);
  crc32_update(c, buf, len);
  return crc32_finalize(c);
}
