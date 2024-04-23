#pragma once

#include <kinc/global.h>

/*! \file acceleration.h
    \brief Provides data provided by acceleration-sensors.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Sets the acceleration-callback which is called with measured acceleration-data in three dimensions.
/// </summary>
/// <param name="value">The callback</param>
KINC_FUNC void kinc_acceleration_set_callback(void (*value)(float /*x*/, float /*y*/, float /*z*/));

void kinc_internal_on_acceleration(float x, float y, float z);

#ifdef KINC_IMPLEMENTATION_INPUT
#define KINC_IMPLEMENTATION
#endif

#ifdef KINC_IMPLEMENTATION

#include <memory.h>
#include <stddef.h>

void (*acceleration_callback)(float /*x*/, float /*y*/, float /*z*/) = NULL;

void kinc_acceleration_set_callback(void (*value)(float /*x*/, float /*y*/, float /*z*/)) {
	acceleration_callback = value;
}

void kinc_internal_on_acceleration(float x, float y, float z) {
	if (acceleration_callback != NULL) {
		acceleration_callback(x, y, z);
	}
}

#endif

#ifdef __cplusplus
}
#endif
