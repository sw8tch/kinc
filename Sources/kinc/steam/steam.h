#pragma once

#include <kinc/global.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
KINC_FUNC bool kinc_steam_init();
KINC_FUNC void kinc_steam_inputaction();
KINC_FUNC void kinc_steam_update();
KINC_FUNC bool kinc_steam_getDigitalStatus(int num);
KINC_FUNC void kinc_steam_input_findcontroller();
#ifdef __cplusplus
}
#endif