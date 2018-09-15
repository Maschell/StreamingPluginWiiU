#ifndef _RETAINS_VARS_H_
#define _RETAINS_VARS_H_

#include <wups.h>

#define WUPS_STREAMING_SCREEN_DRC 0
#define WUPS_STREAMING_SCREEN_TV 1


#define WUPS_STREAMING_RESOLUTION_240P 1
#define WUPS_STREAMING_RESOLUTION_360P 2
#define WUPS_STREAMING_RESOLUTION_480P 3

extern wups_loader_app_status_t gAppStatus;
extern int32_t gScreen;
extern int32_t gResolution;
extern int32_t gSendPriority;
extern int32_t gEncodePriority;

#endif // _RETAINS_VARS_H_
