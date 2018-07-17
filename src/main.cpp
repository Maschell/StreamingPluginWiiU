#include <map>
#include <string>
#include <wups.h>
#include <wups/config.h>
#include <wups/config/WUPSConfig.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
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

void resolutionChanged(int32_t newResolution) {
    DEBUG_FUNCTION_LINE("Resolution changed %d \n",newResolution);
    gResolution = newResolution;

    // Restart server.
    EncodingHelper::destroyInstance();
    MJPEGStreamServer::destroyInstance();

    EncodingHelper::getInstance()->StartAsyncThread();
    MJPEGStreamServer::getInstance();
}


WUPS_GET_CONFIG() {
    WUPSConfig* config = new WUPSConfig("Streaming Plugin");
    WUPSConfigCategory* catOther = config->addCategory("Main");

    std::map<int32_t,std::string> resolutionValues;
    resolutionValues[WUPS_STREAMING_RESOLUTION_240P] = "240p";
    resolutionValues[WUPS_STREAMING_RESOLUTION_360P] = "360p";
    resolutionValues[WUPS_STREAMING_RESOLUTION_480P] = "480p";

    //                    item Type             config id           displayed name              default value  onChangeCallback.
    catOther->addItem(new WUPSConfigItemMultipleValues("resolution", "Streaming resolution", gResolution, resolutionValues, resolutionChanged));

    return config;
}

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
