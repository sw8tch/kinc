#pragma once

#include <kinc/global.h>

#include <kinc/color.h>

#include <assert.h>

/*! \file graphics.h
    \brief Supports a very basic pixel-setting API.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	KINC_G1_TEXTURE_FILTER_POINT,
	KINC_G1_TEXTURE_FILTER_LINEAR,
	KINC_G1_TEXTURE_FILTER_ANISOTROPIC,
} kinc_g1_texture_filter_t;

typedef enum {
	KINC_G1_MIPMAP_FILTER_NONE,
	KINC_G1_MIPMAP_FILTER_POINT,
	KINC_G1_MIPMAP_FILTER_LINEAR,
} kinc_g1_mipmap_filter_t;

/// <summary>
/// Initializes the G1-API.
/// </summary>
/// <param name="width">The width to be used by the G1-API - typically the window-width</param>
/// <param name="height">The height to be used by the G1-API - typically the window-height</param>
KINC_FUNC void kinc_g1_init(int width, int height);

/// <summary>
/// Typically called once per frame before other G1-functions are called.
/// </summary>
KINC_FUNC void kinc_g1_begin(void);

/// <summary>
/// Typically called once per frame after all G1-drawing is finished. This also swaps the framebuffers
/// so an equivalent call to kinc_g4_swap_buffers is not needed.
/// </summary>
KINC_FUNC void kinc_g1_end(void);

extern uint32_t *kinc_internal_g1_image;
extern int kinc_internal_g1_w, kinc_internal_g1_h, kinc_internal_g1_tex_width;
extern kinc_g1_texture_filter_t kinc_internal_g1_texture_filter_min;
extern kinc_g1_texture_filter_t kinc_internal_g1_texture_filter_mag;
extern kinc_g1_mipmap_filter_t kinc_internal_g1_mipmap_filter;

#if defined(KINC_DYNAMIC_COMPILE) || defined(KINC_DYNAMIC) || defined(KINC_DOCS)

/// <summary>
/// Sets a single pixel to a color.
/// </summary>
/// <param name="x">The x-component of the pixel-coordinate to set</param>
/// <param name="y">The y-component of the pixel-coordinate to set</param>
/// <param name="red">The red-component between 0 and 1</param>
/// <param name="green">The green-component between 0 and 1</param>
/// <param name="blue">The blue-component between 0 and 1</param>
KINC_FUNC void kinc_g1_set_pixel(int x, int y, float red, float green, float blue);

/// <summary>
/// Returns the width used by G1.
/// </summary>
/// <returns>The width</returns>
KINC_FUNC int kinc_g1_width(void);

/// <summary>
/// Returns the height used by G1.
/// </summary>
/// <returns>The height</returns>
KINC_FUNC int kinc_g1_height(void);

/// <summary>
/// Set the texture-sampling-mode for upscaled textures.
/// </summary>
/// <param name="unit">The texture-unit to set the texture-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KINC_FUNC void kinc_g1_set_texture_magnification_filter(kinc_g1_texture_filter_t filter);

/// <summary>
/// Set the texture-sampling-mode for downscaled textures.
/// </summary>
/// <param name="unit">The texture-unit to set the texture-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KINC_FUNC void kinc_g1_set_texture_minification_filter(kinc_g1_texture_filter_t filter);

/// <summary>
/// Sets the mipmap-sampling-mode which defines whether mipmaps are used at all and if so whether the two neighbouring mipmaps are linearly interpolated.
/// </summary>
/// <param name="unit">The texture-unit to set the mipmap-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KINC_FUNC void kinc_g1_set_texture_mipmap_filter(kinc_g1_mipmap_filter_t filter);

#else

// implementation moved to the header to allow easy inlining

static inline void kinc_g1_set_pixel(int x, int y, float red, float green, float blue) {
	assert(x >= 0 && x < kinc_internal_g1_w && y >= 0 && y < kinc_internal_g1_h);
	int r = (int)(red * 255);
	int g = (int)(green * 255);
	int b = (int)(blue * 255);
	kinc_internal_g1_image[y * kinc_internal_g1_tex_width + x] = 0xff << 24 | b << 16 | g << 8 | r;
}

static inline int kinc_g1_width(void) {
	return kinc_internal_g1_w;
}

static inline int kinc_g1_height(void) {
	return kinc_internal_g1_h;
}

static inline void kinc_g1_set_texture_magnification_filter(kinc_g1_texture_filter_t filter) {
	kinc_internal_g1_texture_filter_mag = filter;
}

static inline void kinc_g1_set_texture_minification_filter(kinc_g1_texture_filter_t filter) {
	kinc_internal_g1_texture_filter_min = filter;
}

static inline void kinc_g1_set_texture_mipmap_filter(kinc_g1_mipmap_filter_t filter) {
	kinc_internal_g1_mipmap_filter = filter;
}

#endif

#ifdef __cplusplus
}
#endif
