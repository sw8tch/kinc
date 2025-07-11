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
#include <XUser.h>
#include <xsapi-c/services_c.h>

#include "XGameSaveFiles.h"

#include <cstdlib>
#include <cstring>
#include <stdio.h>

static kinc_mutex_t mutex;

static volatile XUserHandle currentUser = nullptr;
static volatile XUserHandle favoriteUser = nullptr;

static volatile XblContextHandle currentContext = nullptr;

kinc_event_t kinc_internal_xbox_storage_initialized;

static volatile int32_t waiting_for_account_picker = 0;
static volatile int32_t waiting_for_save_storage = 0;

bool initXboxStorage(XUserHandle user);
//bool initXboxStorageWin32IO(XUserHandle user);
void GetContainerPath(char *containerPathBuffer);

extern "C" void kinc_internal_login_callback();
extern "C" void kinc_internal_logout_callback();
extern "C" void kinc_internal_save_mounted_callback();
extern "C" void kinc_internal_save_unmounted_callback();

static bool logged_in = false;

static char c_scid[37] = "00000000-0000-0000-0000-0000" KINC_XBOX_TITLEID;
static char saveFolderPath[MAX_PATH]{0};
static char currentGamertag[XUserGamertagComponentClassicMaxBytes + 1]{0};

static void UserChangeEventHandler(void *context, XUserLocalId userLocalId, XUserChangeEvent event) {
	// switch (event) {
	//       case XUserChangeEvent::SignedOut:
	//       case XUserChangeEvent::SigningOut:
	//       case XUserChangeEvent::SignedInAgain:
	//    default:
	//	    break;

	//   }
	if (event == XUserChangeEvent::SignedOut) {

		if (currentUser != nullptr) {
			XUserLocalId currentUserId;
			XUserGetLocalId(currentUser, &currentUserId);
			if (currentUserId.value == userLocalId.value) {
				kinc_event_reset(&kinc_internal_xbox_storage_initialized);

				// need to change the savedir ? $$TODO$$ might need to implement callbacks here to inform upper layers about the change in save path...
				// is the reset event enough ?
				// initXboxStorageWin32IO(currentUser);

				currentUser = nullptr;
				kinc_internal_logout_callback();
			}
		}
	}
	else if (event == XUserChangeEvent::SigningOut) {
		// Delay the user signing out just for fun
		XUserSignOutDeferralHandle deferral;
		if (SUCCEEDED(XUserGetSignOutDeferral(&deferral))) {
			// Hold the deferral for 5 seconds then close it
			XUserCloseSignOutDeferralHandle(deferral);
			// std::thread completeDeferralThread([deferral]() {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			//	XUserCloseSignOutDeferralHandle(deferral);
			// });
			// completeDeferralThread.detach();
		}
	}
	else if (event == XUserChangeEvent::SignedInAgain) {
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
	xblArgs.scid = c_scid;
	XblInitialize(&xblArgs);
}
void closeUserHandle()
{
	XUserCloseHandle(currentUser);
}

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

bool kinc_service_waiting_for_save_storage() {
	return waiting_for_save_storage != 0;
}

void GetContainerPath(char *containerPathBuffer) {
	// The folder path generated on Xbox does not have a \ at the end.
	// The folder path generated from windows has a \ at the end.
	// You should handle both cases either way.

	char lastChar = saveFolderPath[strlen(saveFolderPath) - 1];

	if (lastChar == '\\') {
		sprintf_s(containerPathBuffer, MAX_PATH, "%scontainer", saveFolderPath);
	}
	else {
		sprintf_s(containerPathBuffer, MAX_PATH, "%s\\container", saveFolderPath);
	}
}

const char *kinc_get_save_path(void) {
	static char savepath[MAX_PATH]{};
	GetContainerPath(savepath);
	kinc_log(KINC_LOG_LEVEL_INFO, "Save path is : %s", savepath);
	return savepath;
}

const char *kinc_service_get_username(int playerid) {
	return currentGamertag;
}

bool initXboxStorageWin32IO(XUserHandle user) {
	KINC_ATOMIC_EXCHANGE_32(&waiting_for_save_storage, 1);
	XAsyncBlock *asyncBlock = new XAsyncBlock;
	ZeroMemory(asyncBlock, sizeof(*asyncBlock));

	asyncBlock->queue = nullptr;
	asyncBlock->context = nullptr;
	asyncBlock->callback = [](XAsyncBlock *asyncBlock) {
		size_t folderSize = 0;
		HRESULT hrsize = XAsyncGetResultSize(asyncBlock, &folderSize);
		
		if (SUCCEEDED(hrsize)) {
			char folderResult[MAX_PATH]{};
			HRESULT hr = XGameSaveFilesGetFolderWithUiResult(asyncBlock, folderSize, folderResult);

			if (SUCCEEDED(hr)) {
				kinc_log(KINC_LOG_LEVEL_INFO, "Kinc : XGameSaveFilesGetFolderWithUiResult successful");
				kinc_log(KINC_LOG_LEVEL_INFO, "Kinc : Game save Dir : %s", folderResult);
				strcpy(saveFolderPath, folderResult);
				kinc_event_signal(&kinc_internal_xbox_storage_initialized);
				kinc_internal_save_mounted_callback();
				KINC_ATOMIC_EXCHANGE_32(&waiting_for_save_storage, 0);
			}
			else {
				kinc_log(KINC_LOG_LEVEL_ERROR, "Kinc : XGameSaveFilesGetFolderWithUiResult successful");
			}
		}
		delete asyncBlock; 
	};

	HRESULT hr = XGameSaveFilesGetFolderWithUiAsync(user, c_scid, asyncBlock);

	if (SUCCEEDED(hr)) {
		kinc_log(KINC_LOG_LEVEL_INFO, "Kinc : XGameSaveFilesGetFolderWithUiAsync callback is set.");
		//delete asyncBlock;
	}
	else {
		kinc_log(KINC_LOG_LEVEL_ERROR, "Kinc : XGameSaveFilesGetFolderWithUiAsync");
		return false;
	}
	return true;
}

void kinc_service_login() {
	KINC_ATOMIC_EXCHANGE_32(&waiting_for_account_picker, 1);
	KINC_ATOMIC_EXCHANGE_32(&waiting_for_save_storage, 1);

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

			//if (!wrongUser && initXboxStorage(user)) {
			if (!wrongUser ) {
				currentUser = user;
				strcpy(currentGamertag, getClassicGamerTag());
				uint64_t userId;
				XUserGetId(user, &userId);
                XblContextHandle context = nullptr;
				result = XblContextCreateHandle(user, &context);
				if (result == S_OK) {
					currentContext = context;
				}
				kinc_log(KINC_LOG_LEVEL_INFO, "Kinc XBLiveServices : User \"%llu\" Logged in",userId);
				
				KINC_ATOMIC_EXCHANGE_32(&waiting_for_account_picker, 0);
				initXboxStorageWin32IO(user);
			}
			else {
				kinc_service_login();
			}
		}
		else {
			kinc_service_login();
		}
	};

	XUserAddAsync(XUserAddOptions::AddDefaultUserAllowingUI,asyncBlock); 
    // XUserAddOptions::AddDefaultUserAllowingUI //XUserAddOptions::AddDefaultUserAllowingUI,
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

const char* getClassicGamerTag() {
	// XUserGetGamertag currentGamerTag = new XUserGetGamertag();
	size_t gamertagSize = 0;
	static char foundGamerTag[XUserGamertagComponentClassicMaxBytes + 1];

	XUserGetGamertag(currentUser, XUserGamertagComponent::Classic, XUserGamertagComponentClassicMaxBytes, foundGamerTag, &gamertagSize);
	return foundGamerTag;
}