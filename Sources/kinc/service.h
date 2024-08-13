#pragma once

#include <kinc/global.h>

#include <kinc/log.h>

/*! \file service.h
    \brief Provides game service functionality for achievements, leaderboards, etc.
*/

#ifdef __cplusplus
extern "C" {
#endif

struct kinc_achievement;

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC int kinc_service_init();

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC void kinc_service_update();

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC void kinc_service_shutdown();

/// <summary>
/// Set an achievement as unlocked into the service
/// </summary>
/// <returns></returns>
KINC_FUNC void kinc_service_set_achievement(char const *name);

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC void kinc_service_set_rich_presence(char const *key, char const *value);

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC void kinc_service_clear_rich_presence();

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC char const *kinc_service_get_language();

/// <summary>
/// </summary>
/// <returns></returns>
KINC_FUNC bool kinc_service_get_achievement(char const *achName, struct kinc_achievement *achievement);

#ifdef KINC_IMPLEMENTATION_SERVICE
#define KINC_IMPLEMENTATION
#endif

#ifdef KINC_IMPLEMENTATION

#if !defined(KINC_USE_STEAM)
int kinc_service_init() {
	return 1;
}

void kinc_service_update()
{

}
void kinc_service_shutdown() {
}

void kinc_service_set_achievement(char const *name) {
}

void kinc_service_set_rich_presence(char const *key, char const *value) {
}

void kinc_service_clear_rich_presence() {
}

char const *kinc_service_get_language() {
	static char const *empty = "";
	return empty;
}

bool kinc_service_get_achievement(char const *achName, struct kinc_achievement *achievement) {
	return false;
}

#endif

#endif

#ifdef __cplusplus
}
#endif
