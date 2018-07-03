#include <wups.h>
#include <utils/logger.h>
#include "retain_vars.hpp"
#include "EncodingHelper.h"
#include "MJPEGStreamServer.hpp"

// Mandatory plugin information.
WUPS_PLUGIN_NAME("Gamepad streaming tool.");
WUPS_PLUGIN_DESCRIPTION("Streams the screen via HTTP to a browser. Open http://<ip of your wii u>:8000 on a browser in the same network.");
WUPS_PLUGIN_VERSION("v0.1");
WUPS_PLUGIN_AUTHOR("Maschell");
WUPS_PLUGIN_LICENSE("GPL");

// Something is using "write"...
WUPS_FS_ACCESS()

// Gets called once the loader exists.
INITIALIZE_PLUGIN() {
    socket_lib_init();

    log_init();
}

// Called whenever an application was started.
ON_APPLICATION_START(my_args) {
    socket_lib_init();
    log_init();

    gAppStatus = WUPS_APP_STATUS_FOREGROUND;

    EncodingHelper::getInstance()->StartAsyncThread();
    MJPEGStreamServer::getInstance();

    log_init();
}

ON_APP_STATUS_CHANGED(status) {
    gAppStatus = status;
    if(status == WUPS_APP_STATUS_CLOSED) {
        EncodingHelper::destroyInstance();
        MJPEGStreamServer::destroyInstance();
    }
}
