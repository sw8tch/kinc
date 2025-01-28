#ifndef KOPE_D3D12_DEVICE_STRUCTS_HEADER
#define KOPE_D3D12_DEVICE_STRUCTS_HEADER

#include "d3d12mini.h"

#include <kope/graphics5/commandlist.h>
#include <kope/graphics5/texture.h>
#include <kope/util/indexallocator.h>
#include <kope/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ID3D12CommandAllocator;
struct ID3D12DescriptorHeap;
struct ID3D12QueryHeap;

#define KOPE_D3D12_FRAME_COUNT 2

typedef struct kope_d3d12_device {
	struct ID3D12Device5 *device;
	struct ID3D12CommandQueue *graphics_queue;
	struct ID3D12CommandQueue *compute_queue;
	struct ID3D12CommandQueue *copy_queue;
	struct IDXGISwapChain *swap_chain;

	uint32_t cbv_srv_uav_increment;
	uint32_t sampler_increment;

	uint32_t framebuffer_index;
	kope_g5_texture framebuffer_textures[KOPE_D3D12_FRAME_COUNT];

	struct ID3D12Fence *frame_fence;
	HANDLE frame_event;
	uint64_t current_frame_index;

	struct ID3D12Fence *execution_fence;
	HANDLE execution_event;
	uint64_t execution_index;

	struct ID3D12DescriptorHeap *descriptor_heap;
	oa_allocator_t descriptor_heap_allocator;

	struct ID3D12DescriptorHeap *sampler_heap;
	oa_allocator_t sampler_heap_allocator;

	struct ID3D12DescriptorHeap *all_samplers;
	kope_index_allocator sampler_index_allocator;

	kope_g5_command_list management_list;
} kope_d3d12_device;

typedef struct kope_d3d12_query_set {
	struct ID3D12QueryHeap *query_heap;
	uint8_t query_type;
} kope_d3d12_query_set;

typedef struct kope_d3d12_raytracing_volume {
	kope_g5_buffer *vertex_buffer;
	uint64_t vertex_count;
	kope_g5_buffer *index_buffer;
	uint32_t index_count;

	kope_g5_buffer scratch_buffer;
	kope_g5_buffer acceleration_structure;
} kope_d3d12_raytracing_volume;

typedef struct kope_d3d12_raytracing_hierarchy {
	uint32_t volumes_count;
	kope_g5_buffer instances;

	kope_g5_buffer scratch_buffer;
	kope_g5_buffer update_scratch_buffer;
	kope_g5_buffer acceleration_structure;
} kope_d3d12_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
