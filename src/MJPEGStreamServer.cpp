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
#include "MJPEGStreamServer.hpp"
#include "turbojpeg.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <utils/logger.h>
#include <utils/StringTools.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>

OSMessageQueue streamSendQueue __attribute__((section(".data")));
OSMessage streamSendQueueMessages[STREAM_SEND_QUEUE_MESSAGE_COUNT] __attribute__((section(".data")));

MJPEGStreamServer * MJPEGStreamServer::instance = NULL;

MJPEGStreamServer::MJPEGStreamServer(int32_t port): TCPServer(port,MJPEGStreamServer::getPriority()) {
    OSInitMessageQueue(&streamSendQueue, streamSendQueueMessages, STREAM_SEND_QUEUE_MESSAGE_COUNT);
}

MJPEGStreamServer::~MJPEGStreamServer() {

}

/**
    The sendwait from <network/net.h> is reaaally slow.
**/
int32_t mysendwait(int32_t sock, const void *buffer, int32_t len) {
    int32_t ret;
    while (len > 0) {
        ret = send(sock, buffer, len, 0);
        if(ret < 0) {
            return ret;
        }
        len -= ret;
        buffer =  (void *)(((char *) buffer) + ret);
    }
    return 0;
}

void MJPEGStreamServer::sendJPEG(uint8_t * buffer, uint64_t size) {
    int32_t clientfd = getClientFD();

    char str[90];
    snprintf(str, 90, "\r\n--boundary\r\nContent-Type: image/jpeg\r\nContent-Length:  %llu \r\n\r\n", size);


    mysendwait(clientfd, str, strlen(str));
    mysendwait(clientfd, buffer, size);

    //DEBUG_FUNCTION_LINE("Send frame\n");
}


BOOL MJPEGStreamServer::whileLoop() {
    int32_t ret;
    int32_t clientfd = getClientFD();

    OSMessage message;

    while (1) {
        ret = checkbyte(clientfd);
        if (ret < 0) {
            if(socketlasterr() != 6) {
                // Ending Server on error.
                return false;
            }
        }

        //DEBUG_FUNCTION_LINE("Waiting\n",message.message,message.data1);
        while(!OSReceiveMessage(&streamSendQueue,&message,OS_MESSAGE_FLAGS_NONE)) {
            if(shouldExit()) {
                break;
            }
            OSSleepTicks(OSMicrosecondsToTicks(500));
        }

        if((uint32_t) message.message == 0xDEADBEEF) {
            DEBUG_FUNCTION_LINE("We should exit\n");
            break;
        }

        DCFlushRange(&message,sizeof(OSMessage));

        JpegInformation * info = (JpegInformation *) message.args[0];
        if(info != NULL) {
            DCFlushRange(info,sizeof(JpegInformation));
            sendJPEG(info->getBuffer(),info->getSize());
            delete info;
        }
    }
    return true;
}

const char * headerHTTP = "HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=--boundary\r\n";

BOOL MJPEGStreamServer::acceptConnection() {
    int32_t clientfd = getClientFD();
    DEBUG_FUNCTION_LINE("TCP Connection accepted! \n");

    mysendwait(clientfd, headerHTTP, strlen(headerHTTP));

    // Consume the first response of the browser.
    while(checkbyte(clientfd) > 0);

    DEBUG_FUNCTION_LINE("Handshake done! Success!\n");
    return true;
}

void MJPEGStreamServer::onConnectionClosed() {
    DEBUG_FUNCTION_LINE("disconnected\n");
}
