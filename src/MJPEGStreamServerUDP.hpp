/****************************************************************************
 * Copyright (C) 2016,2017 Maschell
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
#ifndef _UDPCLIENT_WINDOW_H_
#define _UDPCLIENT_WINDOW_H_

#define DEFAULT_UDP_CLIENT_PORT    9445

#include <coreinit/messagequeue.h>
#include <coreinit/cache.h>
#include <utils/logger.h>
#include <system/CThread.h>
#include <nsysnet/socket.h>
#include "crc32.h"
#include "JpegInformation.h"
#include "MJPEGStreamServer.hpp"

#define DATA_SEND_QUEUE_MESSAGE_COUNT 1

class MJPEGStreamServerUDP : public MJPEGStreamServer {
public:
    ~MJPEGStreamServerUDP();

    static MJPEGStreamServerUDP *createInstance(int32_t ip, int32_t port) {
        return new MJPEGStreamServerUDP(ip, port);
    }

    void StartAsyncThread();

    static void DoAsyncThread(CThread *thread, void *arg);

    void DoAsyncThreadInternal(CThread *thread);

    void StopAsyncThread() {
        DEBUG_FUNCTION_LINE("StopAsyncThread\n");
        shouldExit = true;
        DCFlushRange((void*) &shouldExit,sizeof(shouldExit));
    }

    void setThreadPriority(int priority) {
        if(pThread != NULL) {
            pThread->setThreadPriority(priority);
        }
    }

    void proccessData(CThread *thread, void *arg);

    bool streamJPEG(JpegInformation * info);

    bool streamJPEGThreaded(JpegInformation * info);

    void sendJPEG(uint8_t * buffer, uint64_t size);

    bool sendData(uint8_t * data,int32_t length);

    volatile int32_t sockfd = -1;

    static MJPEGStreamServerUDP *instance;

    crc32_t crc32Buffer;

private:
    MJPEGStreamServerUDP(uint32_t ip,int32_t port);


    bool shouldExit = false;
    CThread * pThread = NULL;

    OSMessageQueue dataQueue;
    OSMessage dataQueueMessages[DATA_SEND_QUEUE_MESSAGE_COUNT];
};

#endif //_UDPClient_WINDOW_H_
