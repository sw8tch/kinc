#include "XboxUser.h"

#include <kinc/input/gamepad.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <kinc/threads/atomic.h>
#include <kinc/threads/mutex.h>
#include <kinc/service.h>

#define NOMINMAX

#include <wrl.h>

#include <GameInput.h>

#include <Shlobj.h>
#include <XGameRuntimeInit.h>
#include <XGameUI.h>
#include <XLauncher.h>
#include <XPackage.h>
#include <XStore.h>
#include <xsapi-c/services_c.h>



static kinc_mutex_t mutex;

static volatile XUserHandle currentUser = nullptr;
static volatile XUserHandle favoriteUser = nullptr;

static volatile XblContextHandle currentContext = nullptr;

kinc_event_t kinc_internal_xbox_storage_initialized;

static void UserChangeEventHandler(void *context, XUserLocalId userLocalId, XUserChangeEvent event) {
	if (event == XUserChangeEvent::SignedOut) {
		if (currentUser != nullptr) {
			XUserLocalId currentUserId;
			XUserGetLocalId(currentUser, &currentUserId);
			if (currentUserId.value == userLocalId.value) {
				kinc_event_reset(&kinc_internal_xbox_storage_initialized);
				currentUser = nullptr;
			}
		}
	}
}

static void DeviceAssociationChangedEventHandler(_In_opt_ void *context, _In_ const XUserDeviceAssociationChange *change) {}

void initXboxUser() {
	kinc_mutex_init(&mutex);
	kinc_event_init(&kinc_internal_xbox_storage_initialized, false);

	{
		XTaskQueueRegistrationToken token;
		XUserRegisterForChangeEvent(nullptr, nullptr, UserChangeEventHandler, &token);
	}

	{
		XTaskQueueRegistrationToken token;
		XUserRegisterForDeviceAssociationChanged(nullptr, nullptr, DeviceAssociationChangedEventHandler, &token);
	}

	XblInitArgs xblArgs = {};
	xblArgs.scid = MSSCID;
	XblInitialize(&xblArgs);
}

static volatile int32_t waiting_for_account_picker = 0;

bool initXboxStorage(XUserHandle user);

extern "C" void kinc_internal_login_callback();
extern "C" void kinc_internal_logout_callback();

static bool logged_in = false;

void checkXboxUser() {
	if (logged_in) {
		if (currentUser == nullptr) {
			logged_in = false;
			kinc_internal_logout_callback();
		}
	}
	else {
		if (currentUser != nullptr) {
			logged_in = true;
			kinc_internal_login_callback();
		}
	}
}

bool kinc_service_waiting_for_login() {
	return waiting_for_account_picker != 0;
}

void kinc_service_login() {
	KINC_ATOMIC_EXCHANGE_32(&waiting_for_account_picker, 1);

	XAsyncBlock *asyncBlock = new XAsyncBlock;
	ZeroMemory(asyncBlock, sizeof(*asyncBlock));
	asyncBlock->queue = nullptr;
	asyncBlock->context = nullptr;
	asyncBlock->callback = [](XAsyncBlock *asyncBlock) {
		XUserHandle user;
		HRESULT result = XUserAddResult(asyncBlock, &user);

		delete asyncBlock;

		if (result == S_OK) {
			bool wrongUser = false;
			if (favoriteUser != nullptr) {
				uint64_t favoriteUserId;
				XUserGetId(favoriteUser, &favoriteUserId);
				uint64_t userId;
				XUserGetId(user, &userId);

				if (favoriteUserId != userId) {
					wrongUser = true;
				}
			}

			if (!wrongUser && initXboxStorage(user)) {
				currentUser = user;
				XblContextHandle context = nullptr;
				result = XblContextCreateHandle(user, &context);
				if (result == S_OK) {
					currentContext = context;
				}
				KINC_ATOMIC_EXCHANGE_32(&waiting_for_account_picker, 0);
			}
			else {
				kinc_service_login();
			}
		}
		else {
			kinc_service_login();
		}
	};

	XUserAddAsync(XUserAddOptions::AddDefaultUserAllowingUI, asyncBlock);
}

void kinc_service_disallow_user_change() {
	favoriteUser = currentUser;
}

void kinc_service_allow_user_change() {
	if (currentUser != nullptr) {
		favoriteUser = currentUser;
	}
}

void changeXboxUser() {
	currentUser = nullptr;
}

void ensureXboxUser() {
	if (currentUser == nullptr) {
		kinc_login();
	}
}

void kinc_service_unlock_achievement(int id) {
	if (currentUser != nullptr && currentContext != nullptr) {
		XAsyncBlock *asyncBlock = new XAsyncBlock;
		asyncBlock->queue = nullptr;
		asyncBlock->context = nullptr;
		asyncBlock->callback = [](XAsyncBlock *asyncBlock) {
			auto result = XAsyncGetStatus(asyncBlock, false);
			if (SUCCEEDED(result)) {
				// Achievement updated
			}
			else if (result == HTTP_E_STATUS_NOT_MODIFIED) {
				// Achievement not modified
			}
			else {
				// Achievement failed to update
			}
			delete asyncBlock;
		};

		uint64_t userId;
		XblContextGetXboxUserId(currentContext, &userId);

		char charid[64];
		_itoa(id, charid, 10);

		HRESULT result = XblAchievementsUpdateAchievementAsync(currentContext, userId, charid, 100, asyncBlock);
		if (result != S_OK) {
			kinc_log(KINC_LOG_LEVEL_WARNING, "Could not unlock achievement.");
		}
	}
}
