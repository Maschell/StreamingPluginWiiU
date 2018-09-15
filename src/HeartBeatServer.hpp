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
#ifndef _HEARTBEAT_SERVER_H_
#define _HEARTBEAT_SERVER_H_

#include <utils/TCPServer.hpp>
#include "MJPEGStreamServerUDP.hpp"

#define DEFAULT_TCP_PORT    8092

class HeartBeatServer: TCPServer {

public:
    static HeartBeatServer *getInstance() {
        if(!instance) {
            instance = new HeartBeatServer(DEFAULT_TCP_PORT);
        }
        return instance;
    }

    static void destroyInstance() {
        if(instance) {
            delete instance;
            instance = NULL;
        }
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

    volatile void setMJPEGServerThreadPriority(int32_t priority) {
        if(mjpegStreamServer != NULL) {
            mjpegStreamServer->setThreadPriority(priority);
        }
    }

    HeartBeatServer(int32_t port);
    virtual ~HeartBeatServer();

    MJPEGStreamServer *  getMJPEGServer(){
        return this->mjpegStreamServer;
    }

    virtual BOOL whileLoop();

    virtual BOOL acceptConnection();

    virtual void onConnectionClosed();

    static HeartBeatServer * instance;
    MJPEGStreamServerUDP * mjpegStreamServer = NULL;
};

#endif //_MJPEG_STREAM_SERVER_H_
