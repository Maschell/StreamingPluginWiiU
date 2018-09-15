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

#ifndef __ENCODING_HELPER_H_
#define __ENCODING_HELPER_H_

#include <vector>
#include <algorithm>

#include <system/CThread.h>
#include <coreinit/cache.h>
#include <coreinit/messagequeue.h>
#include <utils/logger.h>
#include "MJPEGStreamServerUDP.hpp"

#define ENCODE_QUEUE_MESSAGE_COUNT 1

extern OSMessageQueue encodeQueue;
extern OSMessage encodeQueueMessages[ENCODE_QUEUE_MESSAGE_COUNT];


class EncodingHelper {
public:
    static EncodingHelper * getInstance() {
        if(!instance) {
            instance = new EncodingHelper();
        }

        return instance;
    }

    static void destroyInstance() {
        if(instance) {
            instance->StopAsyncThread();
            while(instance->serverRunning) {
                OSSleepTicks(OSMicrosecondsToTicks(1000));
            }
            OSSleepTicks(OSMillisecondsToTicks(500));
            delete instance;
            instance = NULL;
        }
    }

    static bool addFSQueueMSG(OSMessage message) {
        if(!OSSendMessage(&encodeQueue,&message,OS_MESSAGE_FLAGS_NONE)) {
            //DEBUG_FUNCTION_LINE("Dropping frame\n");
            return false;
        };
        return true;
    }

    void StartAsyncThread();

    void StopAsyncThread() {
        DEBUG_FUNCTION_LINE("StopAsyncThread\n");
        shouldExit = true;
        DCFlushRange((void*) &shouldExit,sizeof(shouldExit));
    }

    void setMJPEGStreamServer(MJPEGStreamServer * server){
        this->mjpegServer = server;
    }

    void setThreadPriority(int32_t priority){
        if(pThread != NULL){
            pThread->setThreadPriority(priority);
        }
    }

private:
    EncodingHelper() {
        OSInitMessageQueue(&encodeQueue, encodeQueueMessages, ENCODE_QUEUE_MESSAGE_COUNT);
        DEBUG_FUNCTION_LINE("Init queue done! \n");
    }

    static void DoAsyncThread(CThread *thread, void *arg);


    void DoAsyncThreadInternal(CThread *thread);

    CThread *pThread;

    MJPEGStreamServer * mjpegServer = NULL;

    volatile bool serverRunning = false;

    volatile bool shouldExit = false;

    static EncodingHelper * instance;
};
#endif // __ENCODING_HELPER_H_
