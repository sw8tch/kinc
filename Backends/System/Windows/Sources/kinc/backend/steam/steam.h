#pragma once

#include <kinc/global.h>
#include <stdbool.h>
#define KINC_IMPLEMENTATION_STEAM



#ifdef __cplusplus
extern "C" {
#endif


KINC_FUNC bool kinc_steam_init();
KINC_FUNC void kinc_steam_shutdown();
KINC_FUNC void kinc_steam_inputaction();
KINC_FUNC void kinc_steam_actions_register();
KINC_FUNC void kinc_steam_axis_register();
KINC_FUNC void kinc_steam_update();
KINC_FUNC bool kinc_steam_getDigitalStatus(int num);
KINC_FUNC void kinc_steam_getAnalogStatus(int num, float *x, float *y);
KINC_FUNC void kinc_steam_input_findcontroller();
KINC_FUNC void kinc_steam_richpresence_update(const char* key, const char* value);
KINC_FUNC void kinc_steam_richpresence_clear();
KINC_FUNC void kinc_steam_set_achievement(const char *achievementID);




#ifdef __cplusplus
}
#endif

