#include <kinc/audio2/audio.h>
#include <kinc/system.h>

#include <kinc/backend/SystemMicrosoft.h>
#include <kinc/backend/Windows.h>

#include <dsound.h>

#include <assert.h>

namespace {
	IDirectSound8 *dsound = nullptr;
	IDirectSoundBuffer *dbuffer = nullptr;
	const DWORD dsize = 50 * 1024;
	const int samplesPerSecond = 44100;
	const int bitsPerSample = 16;

	DWORD lastPlayPosition = 0;
	bool secondHalfFilled = false;

	const int gap = 10 * 1024;
	DWORD writePos = gap;

	kinc_a2_buffer_t a2_buffer;
}

static bool initialized = false;

void kinc_a2_init() {
	if (initialized) {
		return;
	}

	kinc_a2_internal_init();
	initialized = true;

	a2_buffer.read_location = 0;
	a2_buffer.write_location = 0;
	a2_buffer.data_size = 128 * 1024;
	a2_buffer.channel_count = 2;
	a2_buffer.channels[0] = new float[a2_buffer.data_size];
	a2_buffer.channels[1] = new float[a2_buffer.data_size];

	kinc_microsoft_affirm(DirectSoundCreate8(nullptr, &dsound, nullptr));
	// TODO (DK) only for the main window?
	kinc_microsoft_affirm(dsound->SetCooperativeLevel(kinc_windows_window_handle(0), DSSCL_PRIORITY));

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = samplesPerSecond;
	waveFormat.wBitsPerSample = bitsPerSample;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes = dsize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	kinc_microsoft_affirm(dsound->CreateSoundBuffer(&bufferDesc, &dbuffer, nullptr));

	DWORD size1;
	uint8_t *buffer1 = NULL;
	kinc_microsoft_affirm(dbuffer->Lock(writePos, gap, (void **)&buffer1, &size1, nullptr, nullptr, 0));
	assert(buffer1 != NULL);
	for (DWORD i = 0; i < size1; ++i) {
		buffer1[i] = 0;
	}
	kinc_microsoft_affirm(dbuffer->Unlock(buffer1, size1, nullptr, 0));

	kinc_microsoft_affirm(dbuffer->Play(0, 0, DSBPLAY_LOOPING));
}

uint32_t kinc_a2_samples_per_second(void) {
	return samplesPerSecond;
}

namespace {
	void copySample(uint8_t *buffer, DWORD &index, bool left) {
		float value = *(float *)&a2_buffer.channels[left ? 0 : 1][a2_buffer.read_location];
		if (!left) {
			a2_buffer.read_location += 1;
			if (a2_buffer.read_location >= a2_buffer.data_size) {
				a2_buffer.read_location = 0;
			}
		}
		*(int16_t *)&buffer[index] = static_cast<int16_t>(value * 32767);
		index += 2;
	}
}

void kinc_a2_update() {
	DWORD playPosition;
	DWORD writePosition;
	kinc_microsoft_affirm(dbuffer->GetCurrentPosition(&playPosition, &writePosition));

	int dif;
	if (writePos >= writePosition) {
		dif = writePos - writePosition;
	}
	else {
		dif = dsize - writePosition + writePos;
	}

	if (dif < gap) {
		return;
	}
	if (writePos + gap >= dsize) {
		if (playPosition >= writePos || playPosition <= gap) {
			return;
		}
		if (writePosition >= writePos || writePosition <= gap) {
			return;
		}
	}
	else {
		if (playPosition >= writePos && playPosition <= writePos + gap) {
			return;
		}
		if (writePosition >= writePos && writePosition <= writePos + gap) {
			return;
		}
	}

	kinc_a2_internal_callback(&a2_buffer, (uint32_t)(gap / 4));

	DWORD size1;
	uint8_t *buffer1;
	kinc_microsoft_affirm(dbuffer->Lock(writePos, gap, (void **)&buffer1, &size1, NULL, NULL, 0));

	for (DWORD i = 0; i < size1 - (bitsPerSample / 8 - 1);) {
		copySample(buffer1, i, ((writePos + i) / 2) % 2 == 0);
	}
	writePos += size1;

	kinc_microsoft_affirm(dbuffer->Unlock(buffer1, size1, NULL, 0));

	if (writePos >= dsize) {
		writePos -= dsize;
	}
}

void kinc_a2_shutdown() {
	if (dbuffer != nullptr) {
		dbuffer->Release();
		dbuffer = nullptr;
	}
	if (dsound != nullptr) {
		dsound->Release();
		dsound = nullptr;
	}
}
