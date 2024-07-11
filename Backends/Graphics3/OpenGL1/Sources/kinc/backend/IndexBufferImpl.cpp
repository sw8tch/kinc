#include "ogl.h"

#include <Kore/Graphics3/Graphics.h>

using namespace Kore;

Graphics3::IndexBuffer *IndexBufferImpl::current = nullptr;

IndexBufferImpl::IndexBufferImpl(int count) : myCount(count) {}

Graphics3::IndexBuffer::IndexBuffer(int indexCount) : IndexBufferImpl(indexCount) {
	glGenBuffers(1, &bufferId);
	glCheckErrors();
	data = new int[indexCount];
#if defined(KINC_ANDROID) || defined(KINC_RASPBERRY_PI)
	shortData = new u16[indexCount];
#endif
}

Graphics3::IndexBuffer::~IndexBuffer() {
	unset();
	delete[] data;
}

int *Graphics3::IndexBuffer::lock() {
	return data;
}

void Graphics3::IndexBuffer::unlock() {
#if defined(KINC_ANDROID) || defined(KINC_RASPBERRY_PI)
	for (int i = 0; i < myCount; ++i)
		shortData[i] = (u16)data[i];
#endif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	glCheckErrors();
#if defined(KINC_ANDROID) || defined(KINC_RASPBERRY_PI)
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myCount * 2, shortData, GL_STATIC_DRAW);
	glCheckErrors();
#else
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myCount * 4, data, GL_STATIC_DRAW);
	glCheckErrors();
#endif
}

void Graphics3::IndexBuffer::_set() {
	current = this;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	glCheckErrors();
}

void IndexBufferImpl::unset() {
	if ((void *)current == (void *)this)
		current = nullptr;
}

int Graphics3::IndexBuffer::count() {
	return myCount;
}
