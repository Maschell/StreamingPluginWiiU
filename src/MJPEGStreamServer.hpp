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
#ifndef _MJPEG_STREAM_SERVER_H_
#define _MJPEG_STREAM_SERVER_H_

#include <utils/TCPServer.hpp>
#include <network/net.h>
#include <coreinit/messagequeue.h>
#include "turbojpeg.h"
#include "JpegInformation.h"

#define STREAM_SEND_QUEUE_MESSAGE_COUNT 1

extern OSMessageQueue streamSendQueue;
extern OSMessage streamSendQueueMessages[STREAM_SEND_QUEUE_MESSAGE_COUNT];

extern uint32_t frame_counter_skipped;

class MJPEGStreamServer: TCPServer {

public:
    static MJPEGStreamServer *getInstance() {
        if(!instance) {
            instance = new MJPEGStreamServer(8080);
        }
        return instance;
    }

    static void destroyInstance() {
        if(instance) {
            instance->StopAsyncThread();
            while(instance->isConnected()) {
                OSSleepTicks(OSMicrosecondsToTicks(1000));
            }
            OSSleepTicks(OSMillisecondsToTicks(500));
            delete instance;
            instance = NULL;
        }
    }

    void StopAsyncThread() {
        DEBUG_FUNCTION_LINE("StopAsyncThread\n");
        OSMessage message;
        message.message = (void *)0xDEADBEEF;
        OSSendMessage(&streamSendQueue,&message,OS_MESSAGE_FLAGS_BLOCKING);
    }


    static int32_t getPriority() {
        return 31;
    }

    static volatile bool isInstanceConnected() {
        if(instance) {
            return instance->isConnected();
        }
        return false;
    }

    MJPEGStreamServer(int32_t port);

    void sendJPEG(uint8_t * buffer, uint64_t size);

    virtual bool streamJPEG(JpegInformation * info) {
        if(this->isConnected()) {
            OSMessage message;
            message.message = (void *) 0x11111;
            message.args[0] = (uint32_t) info;
            if(!OSSendMessage(&streamSendQueue,&message,OS_MESSAGE_FLAGS_NONE)) {
                frame_counter_skipped++;
                //DEBUG_FUNCTION_LINE("Dropping frame\n");
                delete info;
                return false;
            };
        } else {
            delete info;
        }
        return true;
    }

    virtual ~MJPEGStreamServer();

    virtual BOOL whileLoop();

    virtual BOOL acceptConnection();

    virtual void onConnectionClosed();

    static MJPEGStreamServer * instance;
};

#endif //_MJPEG_STREAM_SERVER_H_
