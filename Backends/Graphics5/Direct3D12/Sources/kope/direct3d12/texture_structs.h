#ifndef KOPE_D3D12_TEXTURE_STRUCTS_HEADER
#define KOPE_D3D12_TEXTURE_STRUCTS_HEADER

#include "d3d12mini.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kope_d3d12_texture {
	struct ID3D12Resource *resource;

	uint32_t width;
	uint32_t height;
	uint32_t depth_or_array_layers;
	uint32_t mip_level_count;

	uint32_t resource_states[64];
	uint32_t format;

	// used for framebuffer-synchronization
	uint64_t in_flight_frame_index;
} kope_d3d12_texture;

#ifdef __cplusplus
}
#endif

#endif
