#include <kinc/graphics5/indexbuffer.h>

#include <kinc/graphics5/indexbuffer.h>
#include <stdlib.h>
#include <string.h>

extern WGPUDevice device;

kinc_g5_index_buffer_t *kinc_g5_internal_current_index_buffer = NULL;

void kinc_g5_index_buffer_init(kinc_g5_index_buffer_t *buffer, int count, kinc_g5_index_buffer_format_t format, bool gpuMemory) {
	buffer->impl.count = count;
	buffer->impl.format = format;
}

void kinc_g5_index_buffer_destroy(kinc_g5_index_buffer_t *buffer) {}

static int kinc_g5_internal_index_buffer_stride(kinc_g5_index_buffer_t *buffer) {
	return buffer->impl.format == KINC_G5_INDEX_BUFFER_FORMAT_16BIT ? 2 : 4;
}

void *kinc_g5_index_buffer_lock_all(kinc_g5_index_buffer_t *buffer) {
	kinc_g5_index_buffer_lock(buffer, 0, kinc_g5_index_buffer_count(buffer));
}

void *kinc_g5_index_buffer_lock(kinc_g5_index_buffer_t *buffer, int start, int count) {
	WGPUBufferDescriptor bDesc;
	memset(&bDesc, 0, sizeof(bDesc));
	bDesc.size = count * kinc_g5_internal_index_buffer_stride(buffer);
	bDesc.usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst;
	bDesc.mappedAtCreation = true;
	buffer->impl.buffer = wgpuDeviceCreateBuffer(device, &bDesc);
	return wgpuBufferGetMappedRange(buffer->impl.buffer, start * kinc_g5_internal_index_buffer_stride(buffer), bDesc.size);
}

void kinc_g5_index_buffer_unlock_all(kinc_g5_index_buffer_t *buffer) {
	wgpuBufferUnmap(buffer->impl.buffer);
}

void kinc_g5_index_buffer_unlock(kinc_g5_index_buffer_t *buffer, int count) {
	kinc_g5_index_buffer_unlock_all(buffer);
}

void kinc_g5_internal_index_buffer_set(kinc_g5_index_buffer_t *buffer) {}

int kinc_g5_index_buffer_count(kinc_g5_index_buffer_t *buffer) {
	return buffer->impl.count;
}
