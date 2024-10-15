#pragma once

#include <kinc/global.h>
#include <stdbool.h>
//#define KINC_IMPLEMENTATION_STEAM


#ifdef __cplusplus
extern "C" {
#endif

KINC_FUNC bool kinc_steam_update();
KINC_FUNC bool kinc_steam_getDigitalStatus(int gamepadid,int num);
KINC_FUNC void kinc_steam_getAnalogStatus(int gamepadid,int num, float *x, float *y);

typedef struct kinc_achievement {
	char m_pchAchievementID[128];
	char m_cName[256];
	char m_cDescription[256];
	bool m_bAchieved;
	bool m_bHidden;
	int m_iIconImage;
} kinc_achievement_t;

/// <summary>
/// Sets the steam game overlay callback which is called when toggling the steam overlay (usually bound on pause).
/// </summary>
/// <param name="value">The callback</param>
/// <param name="userdata">Userdata you will receive back as the 2nd callback parameter</param>
KINC_FUNC void kinc_steam_set_gameoverlayactivated_callback(void (*value)(bool /* active */, void* /* userdata*/),void* userdata);

void kinc_steam_internal_call_gameoverlayactivated_callback(bool active);
void kinc_service_internal_call_achievementupdate_callback(bool achievement);

static void (*steam_gameoverlayactivated_callback)(bool /* active */, void* /* userdata*/) = NULL;
static void *steam_gameoverlayactivated_callback_userdata = NULL;

#ifdef __cplusplus
}
#endif

