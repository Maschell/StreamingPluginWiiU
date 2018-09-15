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
#include "HeartBeatServer.hpp"
#include "MJPEGStreamServerUDP.hpp"
#include "EncodingHelper.h"
#include "turbojpeg.h"
#include <malloc.h>
#include <network/net.h>
#include <stdio.h>
#include <string.h>

#include <utils/logger.h>
#include <utils/StringTools.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>


HeartBeatServer * HeartBeatServer::instance = NULL;

HeartBeatServer::HeartBeatServer(int32_t port): TCPServer(port,HeartBeatServer::getPriority()) {

}

HeartBeatServer::~HeartBeatServer() {
    if(mjpegStreamServer != NULL) {
        delete mjpegStreamServer;
        mjpegStreamServer = NULL;
    }
}

BOOL HeartBeatServer::whileLoop() {
    int32_t ret;
    int32_t clientfd = getClientFD();

    while (1) {
        if(shouldExit()) {
            break;
        }
        ret = checkbyte(clientfd);
        if (ret < 0) {
            if(socketlasterr() != 6) {
                return false;
            }
            OSSleepTicks(OSMillisecondsToTicks(1000));
            continue;
        }
        if(ret == 0x15) {
            sendbyte(clientfd,0x16);
        }
    }
    return true;
}

BOOL HeartBeatServer::acceptConnection() {
    int32_t clientfd = getClientFD();
    struct sockaddr_in sockaddr = getSockAddr();

    int32_t connectedIP = sockaddr.sin_addr.s_addr;

    EncodingHelper::getInstance()->setMJPEGStreamServer(NULL);

    if(mjpegStreamServer != NULL) {
        delete mjpegStreamServer;
        mjpegStreamServer = NULL;
    }

    mjpegStreamServer = MJPEGStreamServerUDP::createInstance(connectedIP, DEFAULT_UDP_CLIENT_PORT);
    EncodingHelper::getInstance()->setMJPEGStreamServer(mjpegStreamServer);

    DEBUG_FUNCTION_LINE("Handshake done! Success!\n");
    return true;
}


void HeartBeatServer::onConnectionClosed() {
    DEBUG_FUNCTION_LINE("disconnected\n");
    EncodingHelper::getInstance()->setMJPEGStreamServer(NULL);
    if(mjpegStreamServer != NULL) {
        delete mjpegStreamServer;
        mjpegStreamServer = NULL;
    }
}
