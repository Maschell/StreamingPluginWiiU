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
#ifndef _MJPEG_STREAM_SERVER_WINDOW_H_
#define _MJPEG_STREAM_SERVER_WINDOW_H_

#include "JpegInformation.h"

class MJPEGStreamServer {
public:
    MJPEGStreamServer(){
    }
    virtual ~MJPEGStreamServer(){
    }

    virtual bool streamJPEG(JpegInformation * info) = 0;
};

#endif //_MJPEG_STREAM_SERVER_WINDOW_H_
