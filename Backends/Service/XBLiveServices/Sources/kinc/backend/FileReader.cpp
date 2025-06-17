#include "XboxUser.h"

#include <kinc/io/filereader.h>
#include <kinc/error.h>
#include <kinc/math/core.h>
#include <kinc/system.h>
#include <kinc/log.h>

#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "max_save_size.h"

#include <Windows.h>
//#include <concrt.h>
#include <io.h>
#include <ppltasks.h>
#include <robuffer.h>

#include <xgamesavewrappers.hpp>

using namespace Kore;

Microsoft::Xbox::Wrappers::GameSave::Provider *currentStorage = nullptr;

static char c_scid[37] = "00000000-0000-0000-0000-0000" KINC_XBOX_TITLEID;

bool initXboxStorage(XUserHandle user) {
	Microsoft::Xbox::Wrappers::GameSave::Provider *provider = nullptr;
	provider = new Microsoft::Xbox::Wrappers::GameSave::Provider();
	HRESULT result = provider->Initialize(user, c_scid );
	if (result == S_OK) {
		currentStorage = provider;
		kinc_event_signal(&kinc_internal_xbox_storage_initialized);
		return true;
	}
	else {
		kinc_log(KINC_LOG_LEVEL_ERROR, "Failed initializing save system.");
		return false;
	}
}
