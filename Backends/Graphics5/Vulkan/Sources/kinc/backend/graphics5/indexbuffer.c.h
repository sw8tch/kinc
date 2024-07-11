#include "vulkan.h"

#include <kinc/graphics5/indexbuffer.h>

bool memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

kinc_g5_index_buffer_t *currentIndexBuffer = NULL;

static void unset(kinc_g5_index_buffer_t *buffer) {
	if (currentIndexBuffer == buffer) {
		currentIndexBuffer = NULL;
	}
}

void kinc_g5_index_buffer_init(kinc_g5_index_buffer_t *buffer, int indexCount, kinc_g5_index_buffer_format_t format, bool gpuMemory) {
	buffer->impl.count = indexCount;
	buffer->impl.format = format;

	VkBufferCreateInfo buf_info = {0};
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buf_info.pNext = NULL;
	buf_info.size = format == KINC_G5_INDEX_BUFFER_FORMAT_16BIT ? indexCount * sizeof(uint16_t) : indexCount * sizeof(uint32_t);
	buf_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
#ifdef KINC_VKRT
	buf_info.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
#endif
	buf_info.flags = 0;

	memset(&buffer->impl.mem_alloc, 0, sizeof(VkMemoryAllocateInfo));
	buffer->impl.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	buffer->impl.mem_alloc.pNext = NULL;
	buffer->impl.mem_alloc.allocationSize = 0;
	buffer->impl.mem_alloc.memoryTypeIndex = 0;

	buffer->impl.buf = NULL;
	buffer->impl.mem = NULL;

	VkResult err = vkCreateBuffer(vk_ctx.device, &buf_info, NULL, &buffer->impl.buf);
	assert(!err);

	VkMemoryRequirements mem_reqs = {0};
	vkGetBufferMemoryRequirements(vk_ctx.device, buffer->impl.buf, &mem_reqs);

	buffer->impl.mem_alloc.allocationSize = mem_reqs.size;
	bool pass = memory_type_from_properties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &buffer->impl.mem_alloc.memoryTypeIndex);
	assert(pass);

#ifdef KINC_VKRT
	VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {0};
	memory_allocate_flags_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memory_allocate_flags_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
	buffer->impl.mem_alloc.pNext = &memory_allocate_flags_info;
#endif

	err = vkAllocateMemory(vk_ctx.device, &buffer->impl.mem_alloc, NULL, &buffer->impl.mem);
	assert(!err);

	err = vkBindBufferMemory(vk_ctx.device, buffer->impl.buf, buffer->impl.mem, 0);
	assert(!err);
}

void kinc_g5_index_buffer_destroy(kinc_g5_index_buffer_t *buffer) {
	unset(buffer);
	vkFreeMemory(vk_ctx.device, buffer->impl.mem, NULL);
	vkDestroyBuffer(vk_ctx.device, buffer->impl.buf, NULL);
}

static int kinc_g5_internal_index_buffer_stride(kinc_g5_index_buffer_t *buffer) {
	return buffer->impl.format == KINC_G5_INDEX_BUFFER_FORMAT_16BIT ? 2 : 4;
}

void *kinc_g5_index_buffer_lock_all(kinc_g5_index_buffer_t *buffer) {
	return kinc_g5_index_buffer_lock(buffer, 0, kinc_g5_index_buffer_count(buffer));
}

void *kinc_g5_index_buffer_lock(kinc_g5_index_buffer_t *buffer, int start, int count) {
	uint8_t *data;
	VkResult err = vkMapMemory(vk_ctx.device, buffer->impl.mem, 0, buffer->impl.mem_alloc.allocationSize, 0, (void **)&data);
	assert(!err);
	return &data[start * kinc_g5_internal_index_buffer_stride(buffer)];
}

void kinc_g5_index_buffer_unlock_all(kinc_g5_index_buffer_t *buffer) {
	vkUnmapMemory(vk_ctx.device, buffer->impl.mem);
}

void kinc_g5_index_buffer_unlock(kinc_g5_index_buffer_t *buffer, int count) {
	kinc_g5_index_buffer_unlock_all(buffer);
}

int kinc_g5_index_buffer_count(kinc_g5_index_buffer_t *buffer) {
	return buffer->impl.count;
}
