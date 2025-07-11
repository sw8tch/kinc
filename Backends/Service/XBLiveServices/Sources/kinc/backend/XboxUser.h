#pragma once

#include <kinc/threads/event.h>
#include <kinc/service.h>

void initXboxUser();
void ensureXboxUser();
void checkXboxUser();
void changeXboxUser();
void closeUserHandle();
const char *getClassicGamerTag();
void kinc_service_unlock_achievement(int id);

namespace Microsoft {
	namespace Xbox {
		namespace Wrappers {
			namespace GameSave {
				class Provider;
			}
		}
	}
}

extern kinc_event_t kinc_internal_xbox_storage_initialized;
extern Microsoft::Xbox::Wrappers::GameSave::Provider *currentStorage;
