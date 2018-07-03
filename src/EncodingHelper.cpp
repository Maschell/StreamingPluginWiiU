/****************************************************************************
 * Copyright (C) 2018 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include <vector>
#include "EncodingHelper.h"
#include "MJPEGStreamServer.hpp"
#include "stream_utils.h"
#include "JpegInformation.h"
#include <gx2/event.h>
#include <gx2/surface.h>
#include <gx2/mem.h>

EncodingHelper *EncodingHelper::instance = NULL;

OSMessageQueue encodeQueue __attribute__((section(".data")));
OSMessage encodeQueueMessages[ENCODE_QUEUE_MESSAGE_COUNT] __attribute__((section(".data")));

void EncodingHelper::StartAsyncThread() {
    int32_t priority = 17;
    this->pThread = CThread::create(DoAsyncThread, this, CThread::eAttributeAffCore0, priority,0x40000);
    this->pThread->resumeThread();
}

void EncodingHelper::DoAsyncThread(CThread *thread, void *arg) {
    EncodingHelper * arg_instance = (EncodingHelper *) arg;
    return arg_instance->DoAsyncThreadInternal(thread);
}


JpegInformation * convertToJpeg(uint8_t * sourceBuffer, uint32_t width, uint32_t height, uint32_t pitch, uint32_t format, int quality) {
    if(sourceBuffer == NULL) {
        DEBUG_FUNCTION_LINE("path or buffer NULL\n");
        return NULL;
    }
    if((    format != GX2_SURFACE_FORMAT_SRGB_R8_G8_B8_A8 &&
            format != GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8)) {
        DEBUG_FUNCTION_LINE("Format not supported\n");
        return NULL;
    }

    tjhandle handle = tjInitCompress();

    if(handle == NULL) {
        const char *err = (const char *) tjGetErrorStr();
        DEBUG_FUNCTION_LINE("TJ Error: %s UNABLE TO INIT TJ Compressor Object\n",err);
        return NULL;
    }

    int jpegQual = quality;
    int nbands = 4;
    int flags = 0;
    unsigned char* jpegBuf = NULL;

    int pixelFormat = TJPF_GRAY;
    int jpegSubsamp = TJSAMP_GRAY;
    if(nbands == 4) {
        pixelFormat = TJPF_RGBA;
        jpegSubsamp = TJSAMP_411;
    }
    unsigned long jpegSize = 0;

    int tj_stat = tjCompress2( handle, sourceBuffer, width, pitch * nbands, height, pixelFormat, &(jpegBuf), &jpegSize, jpegSubsamp, jpegQual, flags);
    if(tj_stat != 0) {
        const char *err = (const char *) tjGetErrorStr();
        DEBUG_FUNCTION_LINE("TurboJPEG Error: %s UNABLE TO COMPRESS JPEG IMAGE\n", err);
        tjDestroy(handle);
    } else {
        return new JpegInformation(handle, jpegBuf, jpegSize);
    }

    return NULL;
}

void EncodingHelper::DoAsyncThreadInternal(CThread *thread) {
    serverRunning = true;

    OSMessage message;
    GX2ColorBuffer * colorBuffer = NULL;
    shouldExit = false;
    while(true) {
        //DEBUG_FUNCTION_LINE("Waiting for message in EncoderHelper\n");
        if(!OSReceiveMessage(&encodeQueue,&message,OS_MESSAGE_FLAGS_NONE)) {
            //DEBUG_FUNCTION_LINE("... %08X\n",this->shouldExit);
            if(this->shouldExit) {
                DEBUG_FUNCTION_LINE("We should stop\n");
                break;
            }
            OSSleepTicks(OSMicrosecondsToTicks(5000));
            continue;
        }
        DCFlushRange(&message,sizeof(OSMessage));

        //DEBUG_FUNCTION_LINE("Received message %08X: data1 %08X\n",message.message,message.args[1]);
        if((uint32_t) message.message == 0xDEADBEEF) {
            //DEBUG_FUNCTION_LINE("We should stop the server\n");
            break;
        }

        colorBuffer = (GX2ColorBuffer *) message.args[1];

        JpegInformation * info = convertToJpeg((uint8_t*) colorBuffer->surface.image,colorBuffer->surface.width,colorBuffer->surface.height,colorBuffer->surface.pitch,colorBuffer->surface.format,85);

        if(info != NULL ) {
            MJPEGStreamServer::getInstance()->streamJPEG(info);
        }

        //DEBUG_FUNCTION_LINE("We can now kill the colorBuffer\n",colorBuffer);
        if(colorBuffer->surface.image != NULL) {
            free(colorBuffer->surface.image);
            colorBuffer->surface.image = NULL;
            //DEBUG_FUNCTION_LINE("Free image data for %08X\n",colorBuffer);
        }
        free(colorBuffer);
        colorBuffer = NULL;
        //DEBUG_FUNCTION_LINE("Encoding Done.\n");
    }
    DEBUG_FUNCTION_LINE("Server not running anymore\n");
    serverRunning = false;
    DCFlushRange((void*)&serverRunning,sizeof(serverRunning));
}
