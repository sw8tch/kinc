#pragma once

#include "kinc/service.h"
#include "kinc/log.h"
#include "XBLiveServices.h"
#include "XboxUser.h"

#include <kinc/system.h>
#include <kinc/threads/atomic.h>
#include <kinc/threads/mutex.h>
#include <kinc/service.h>

#include <wrl.h>

#include <GameInput.h>

#include <Shlobj.h>
#include <XGameRuntimeInit.h>
#include <XGameUI.h>
#include <XLauncher.h>
#include <XPackage.h>
#include <XStore.h>
#include <xsapi-c/services_c.h>

int kinc_service_init()
{
	XGameRuntimeInitialize();
	initXboxUser();
	//kinc_service_login();
	return 1;
}

void kinc_service_update()
{
	checkXboxUser();
}
void kinc_service_shutdown()
{
	closeUserHandle();
}

void kinc_service_set_achievement(const char *achievementID)
{
	kinc_log(KINC_LOG_LEVEL_INFO, "\nKinc XBLiveServices : Called achievement by name %s", achievementID);
	//kinc_service_unlock_achievement(atoi(achievementID)+1);

}

void kinc_service_set_achievementByID(int id)
{
	kinc_log(KINC_LOG_LEVEL_INFO, "\nKinc XBLiveServices : Called achievement by id %i", id);
	kinc_service_unlock_achievement(id+1);

}

void kinc_service_set_rich_presence(const char* key, const char* value)
{

}
void kinc_service_clear_rich_presence()
{

}

const char *kinc_service_get_language()
{
	return "";
}
