#ifndef _STREAM_UTILS_H_
#define _STREAM_UTILS_H_

#include <gx2/surface.h>
#include <utils/logger.h>

bool copyBuffer(GX2ColorBuffer * sourceBuffer, GX2ColorBuffer * targetBuffer, uint32_t targetWidth, uint32_t targetHeight);

bool streamVideo(GX2ColorBuffer *srcBuffer);

#endif
