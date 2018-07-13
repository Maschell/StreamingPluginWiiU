#include "stream_utils.h"
#include "EncodingHelper.h"
#include "MJPEGStreamServer.hpp"
#include <fs/FSUtils.h>
#include <malloc.h>

#include <gx2/event.h>
#include <gx2/surface.h>
#include <gx2/mem.h>

#ifdef __cplusplus
extern "C" {
#endif
void
GX2ResolveAAColorBuffer(const GX2ColorBuffer * srcColorBuffer,
                        GX2Surface * dstSurface,
                        uint32_t dstMip,
                        uint32_t dstSlice);

#ifdef __cplusplus
}
#endif

bool copyBuffer(GX2ColorBuffer * sourceBuffer, GX2ColorBuffer * targetBuffer, uint32_t targetWidth, uint32_t targetHeight) {
    // Making sure the buffers are not NULL
    if (sourceBuffer != NULL && targetBuffer != NULL) {
        targetBuffer->surface.use =         (GX2SurfaceUse) (GX2_SURFACE_USE_COLOR_BUFFER | GX2_SURFACE_USE_TEXTURE);
        targetBuffer->surface.dim =         GX2_SURFACE_DIM_TEXTURE_2D;
        targetBuffer->surface.width =       targetWidth;
        targetBuffer->surface.height =      targetHeight;
        targetBuffer->surface.depth =       1;
        targetBuffer->surface.mipLevels =   1;
        targetBuffer->surface.format =      GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        targetBuffer->surface.aa =          GX2_AA_MODE1X;
        targetBuffer->surface.tileMode =    GX2_TILE_MODE_LINEAR_ALIGNED;
        targetBuffer->viewMip =             0;
        targetBuffer->viewFirstSlice =      0;
        targetBuffer->viewNumSlices =       1;

        GX2CalcSurfaceSizeAndAlignment(&targetBuffer->surface);
        GX2InitColorBufferRegs(targetBuffer);

        // Let's allocate the memory.
        targetBuffer->surface.image = memalign(targetBuffer->surface.alignment,targetBuffer->surface.imageSize);
        if(targetBuffer->surface.image == NULL) {
            DEBUG_FUNCTION_LINE("failed to allocate memory.\n");
            return false;
        }
        //DEBUG_FUNCTION_LINE("Allocated image data buffer. data %08X  size %08X \n",targetBuffer->surface.image,targetBuffer->surface.imageSize);

        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, targetBuffer->surface.image, targetBuffer->surface.imageSize);
        if (sourceBuffer->surface.aa == GX2_AA_MODE1X) {
            // If AA is disabled, we can simply use GX2CopySurface.
            GX2CopySurface(&sourceBuffer->surface,
                           sourceBuffer->viewMip,
                           sourceBuffer->viewFirstSlice,
                           &targetBuffer->surface, 0, 0);
        } else {
            // If AA is enabled, we need to resolve the AA buffer.
            GX2Surface tempSurface;
            tempSurface = sourceBuffer->surface;
            tempSurface.aa = GX2_AA_MODE1X;
            GX2CalcSurfaceSizeAndAlignment(&tempSurface);

            tempSurface.image = memalign(tempSurface.alignment,tempSurface.imageSize);
            if(tempSurface.image == NULL) {
                DEBUG_FUNCTION_LINE("failed to allocate data AA.\n");
                if(targetBuffer->surface.image != NULL) {
                    free(targetBuffer->surface.image);
                    targetBuffer->surface.image = NULL;
                }
                return false;
            }
            GX2ResolveAAColorBuffer(sourceBuffer,&tempSurface, 0, 0);
            GX2CopySurface(&tempSurface, 0, 0,&targetBuffer->surface, 0, 0);

            // Sync CPU and GPU
            GX2DrawDone();

            if(tempSurface.image != NULL) {
                free(tempSurface.image);
                tempSurface.image = NULL;
            }
        }
        return true;
    } else {
        DEBUG_FUNCTION_LINE("Couldn't copy buffer, pointer was NULL\n");
        return false;
    }
}

uint32_t frame_counter = 0;
uint32_t frame_counter_skipped = 0;
int32_t curQuality = 50;
int32_t minQuality = 40;
int32_t maxQuality = 85;
int32_t stepQuality = 1;
int32_t maxFrameDropsQuality = 20;
int32_t minFrameDropsQuality = 95;


bool streamVideo(GX2ColorBuffer *srcBuffer) {
    if(srcBuffer == NULL) {
        return false;
    }

    if(!MJPEGStreamServer::isInstanceConnected()) {
        return false;
    }

    GX2ColorBuffer* colorBuffer  = (GX2ColorBuffer*) memalign(0x40,sizeof(GX2ColorBuffer));
    if(colorBuffer == NULL) {
        DEBUG_FUNCTION_LINE("Failed to allocate color buffer\n");
        return false;
    }
    //DEBUG_FUNCTION_LINE("allocated at %08X\n",colorBuffer);
    memset(colorBuffer,0,sizeof(GX2ColorBuffer));

    // keep dimensions
    //uint32_t width = srcBuffer->surface.width;
    //uint32_t height = srcBuffer->surface.height;
    //uint32_t width = 640;
    //uint32_t height = 360;
    uint32_t width = 428;
    uint32_t height = 240;

    bool valid = copyBuffer(srcBuffer,colorBuffer,width,height);
    if(!valid) {
        DEBUG_FUNCTION_LINE("Copy buffer failed.\n");
        if(colorBuffer->surface.image != NULL) {
            free(colorBuffer->surface.image);
            colorBuffer->surface.image = NULL;
        }
        if(colorBuffer != NULL) {
            free(colorBuffer);
            colorBuffer = NULL;
        }
        return false;
    }
    //DEBUG_FUNCTION_LINE("Copy buffer was successful.\n");

    // Flush out destinations caches
    GX2Invalidate(GX2_INVALIDATE_MODE_COLOR_BUFFER, colorBuffer->surface.image,colorBuffer->surface.imageSize);

    // Wait for GPU to finish
    GX2DrawDone();

    DCFlushRange(colorBuffer,sizeof(GX2ColorBuffer));
    DCFlushRange(&(colorBuffer->surface),sizeof(GX2Surface));

    OSMessage message;
    message.message = (void *) 0x1337;
    message.args[1] = (uint32_t) colorBuffer;
    frame_counter++;

    bool result = true;

    if(!EncodingHelper::addFSQueueMSG(message)) {
        frame_counter_skipped++;

        //DEBUG_FUNCTION_LINE("Adding to queue failed, free memory\n");
        if(colorBuffer->surface.image != NULL) {
            free(colorBuffer->surface.image);
            colorBuffer->surface.image = NULL;
        }
        if(colorBuffer != NULL) {
            free(colorBuffer);
            colorBuffer = NULL;
        }
        result = false;
    }

    if(frame_counter % 60 == 0) { // Print this every second.

        int32_t curRatio = (int32_t)100.f*(frame_counter_skipped*1.0f/frame_counter);
        int32_t curQualityOld = curQuality;
        if(curRatio > maxFrameDropsQuality) {  // Lower the quality if we drop more than [maxFrameDropsQuality]% of the frames.
            curQuality -= (curRatio - maxFrameDropsQuality);
        } else if(curRatio < minFrameDropsQuality) { // Increase the quality if we drop less than [minFrameDropsQuality]% of the frames.
            curQuality += stepQuality; // Increase the quality by [stepQuality]%
        }

        // Make sure to set the quality to at least [minQuality]%
        if(curQuality < minQuality) {
            curQuality = minQuality;
        }

        // Make sure to set the quality to at most [maxQuality]%
        if(curQuality >= maxQuality) {
            curQuality = maxQuality;
        }

        DEBUG_FUNCTION_LINE("Streaming at %d fps\n",frame_counter-frame_counter_skipped);

        frame_counter = 0;
        frame_counter_skipped = 0;

        if(curQualityOld != curQuality) {
            DEBUG_FUNCTION_LINE("Quality is now at %d%%\n",curQuality);
        }
    }

    return result;
}
