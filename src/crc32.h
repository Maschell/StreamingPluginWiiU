#ifndef __CRC32_H_
#define __CRC32_H_

#include <inttypes.h>
#include <dirent.h>

typedef struct crc32 {
    uint32_t table[256];
    uint32_t value;
} crc32_t;

#define CRC32_INITIAL 0xedb88320

void crc32_init(crc32_t* crc);

uint32_t crc32_crc(crc32_t* c, uint8_t* buf, size_t len);

#endif
