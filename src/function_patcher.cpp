#include <wups.h>
#include <utils/logger.h>
#include <gx2/surface.h>
#include <utils/StringTools.h>
#include <fs/FSUtils.h>
#include "stream_utils.h"
#include "retain_vars.hpp"

uint32_t count = 0;

DECL_FUNCTION(void, GX2CopyColorBufferToScanBuffer, const GX2ColorBuffer *colorBuffer, int32_t scan_target) {
    if(gAppStatus == WUPS_APP_STATUS_FOREGROUND) {

        if(scan_target == 4  /*&&  (count++ % 4 == 0)*/  && colorBuffer != NULL ) {
            count = 0;
            streamVideo((GX2ColorBuffer *)colorBuffer);
        }
    }
    real_GX2CopyColorBufferToScanBuffer(colorBuffer,scan_target);
}

WUPS_MUST_REPLACE(GX2CopyColorBufferToScanBuffer,   WUPS_LOADER_LIBRARY_GX2,    GX2CopyColorBufferToScanBuffer);
