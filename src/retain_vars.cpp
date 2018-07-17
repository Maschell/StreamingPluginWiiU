#include "retain_vars.hpp"
wups_loader_app_status_t gAppStatus __attribute__((section(".data"))) = WUPS_APP_STATUS_UNKNOWN;
int32_t gResolution __attribute__((section(".data"))) = WUPS_STREAMING_RESOLUTION_240P;
