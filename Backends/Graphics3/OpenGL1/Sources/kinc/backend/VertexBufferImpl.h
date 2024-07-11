#pragma once

#include <Kore/Graphics4/VertexStructure.h>

namespace Kore {
	namespace Graphics3 {
		class VertexBuffer;
	}

	class VertexBufferImpl {
	protected:
		VertexBufferImpl(int count, int instanceDataStepRate);
		void unset();
		float *data;
		int myCount;
		int myStride;
		uint bufferId;
		// #if defined KINC_ANDROID || defined KINC_EMSCRIPTEN
		Graphics4::VertexStructure structure;
		// #endif
		int instanceDataStepRate;
		int setVertexAttributes(int offset);
#ifndef NDEBUG
		bool initialized;
#endif
	public:
		static Graphics3::VertexBuffer *current;
	};
}
