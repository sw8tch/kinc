#include "kinc/steam/steam.h"
#include "kinc/steam/StatsAndAchievements.h"
#include "kinc/log.h"
#include <XInput.h>
#include "steam/isteaminput.h"
#include "steam/steam_api.h"
#include "steam/isteamfriends.h"
#include "steam/isteamuser.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamutils.h"

#define _ACH_ID(id, name){ id, #id, name, "", 0, 0 }

    CSteamAchievements *g_SteamAchievements = NULL;

    // Achievement array which will hold data about the achievements and their state
    Achievement_t g_Achievements[] = {
        _ACH_ID(ACH_Finish_Game, "WinGame"),
    };

    InputDigitalActionHandle_t m_ControllerDigitalActionHandles[16];
    InputAnalogActionHandle_t m_ControllerAnalogActionHandles[6];

	// A handle to the currently active Steam Controller.
	InputHandle_t m_ActiveControllerHandle;
    ControllerHandle_t pHandles[8];
    bool kinc_steam_init()
{
	SteamAPI_RestartAppIfNecessary(STEAMAPPID);
	if (!SteamAPI_Init()) {
		return 0;
	}
	SteamInput()->Init(false);
	const char *pchName = "Normal Mode";
	SteamFriends()->SetRichPresence("gamename", pchName);
	SteamFriends()->SetRichPresence("steam_display", "#StatusInGame");

	//SteamFriends()->SetPersonaName("OMG ! !  !");
	g_SteamAchievements = new CSteamAchievements(g_Achievements, 1);

	return 1;
}

void kinc_steam_actions_register() {
	m_ControllerDigitalActionHandles[0] = SteamInput()->GetDigitalActionHandle("ActionA");
	m_ControllerDigitalActionHandles[1] = SteamInput()->GetDigitalActionHandle("ActionB");
	m_ControllerDigitalActionHandles[2] = SteamInput()->GetDigitalActionHandle("ActionX");
	m_ControllerDigitalActionHandles[3] = SteamInput()->GetDigitalActionHandle("ActionY");
	m_ControllerDigitalActionHandles[4] = SteamInput()->GetDigitalActionHandle("LShoulder");
	m_ControllerDigitalActionHandles[5] = SteamInput()->GetDigitalActionHandle("RShoulder");
	m_ControllerDigitalActionHandles[6] = SteamInput()->GetDigitalActionHandle("LTrig");
	m_ControllerDigitalActionHandles[7] = SteamInput()->GetDigitalActionHandle("RTrig");
	m_ControllerDigitalActionHandles[8] = SteamInput()->GetDigitalActionHandle("select");
	m_ControllerDigitalActionHandles[9] = SteamInput()->GetDigitalActionHandle("pause_menu");
	m_ControllerDigitalActionHandles[10] = SteamInput()->GetDigitalActionHandle("LThumb");
	m_ControllerDigitalActionHandles[11] = SteamInput()->GetDigitalActionHandle("RThumb");
	m_ControllerDigitalActionHandles[12] = SteamInput()->GetDigitalActionHandle("DPadUp");
	m_ControllerDigitalActionHandles[13] = SteamInput()->GetDigitalActionHandle("DPadDown");
	m_ControllerDigitalActionHandles[14] = SteamInput()->GetDigitalActionHandle("DPadLeft");
	m_ControllerDigitalActionHandles[15] = SteamInput()->GetDigitalActionHandle("DPadRight");
}

void kinc_steam_axis_register() {
	m_ControllerAnalogActionHandles[0] = SteamInput()->GetAnalogActionHandle("Move");
	m_ControllerAnalogActionHandles[1] = SteamInput()->GetAnalogActionHandle("look");
	m_ControllerAnalogActionHandles[2] = SteamInput()->GetAnalogActionHandle("leftTrigger");
	m_ControllerAnalogActionHandles[3] = SteamInput()->GetAnalogActionHandle("rightTrigger");
}

void kinc_steam_inputaction()
{
	//m_ControllerDigitalActionHandles[0] = SteamInput()->GetDigitalActionHandle("ActionA");
	//m_ControllerDigitalActionHandles[1] = SteamInput()->GetDigitalActionHandle("ActionB");
}

void kinc_steam_update()
{
    kinc_steam_input_findcontroller();
	kinc_steam_actions_register();
	kinc_steam_axis_register();
    InputActionSetHandle_t actionset = SteamInput()->GetActionSetHandle("InGameControls");
    SteamInput()->ActivateActionSet(m_ActiveControllerHandle, actionset);
    SteamAPI_RunCallbacks();

}

//-----------------------------------------------------------------------------
// Purpose: Find out if a controller event is currently active
//-----------------------------------------------------------------------------
bool kinc_steam_getDigitalStatus(int num) {
	ControllerDigitalActionData_t digitalData = SteamInput()->GetDigitalActionData(m_ActiveControllerHandle, m_ControllerDigitalActionHandles[num]);

	// Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
	if (digitalData.bActive)
		return digitalData.bState;

	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Get the current x,y state of the analog action. Examples of an analog action are a virtual joystick on the trackpad or the real joystick.
//---------------------------------------------------------------------------------------------------------------------------------------------------
void kinc_steam_getAnalogStatus(int num, float *x, float *y) {
	ControllerAnalogActionData_t analogData = SteamInput()->GetAnalogActionData(m_ActiveControllerHandle, m_ControllerAnalogActionHandles[num]);

	// Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
	if (analogData.bActive) {
		*x = analogData.x;
		*y = analogData.y;
	}
	else {
		*x = 0.0f;
		*y = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Find an active Steam controller
//-----------------------------------------------------------------------------
void kinc_steam_input_findcontroller( )
{
	// Use the first available steam controller for all interaction. We can call this each frame to handle
	// a controller disconnecting and a different one reconnecting. Handles are guaranteed to be unique for
	// a given controller, even across power cycles.

	// See how many Steam Controllers are active. 
	
	int nNumActive = SteamInput()->GetConnectedControllers( pHandles );

	// If there's an active controller, and if we're not already using it, select the first one.
	if ( nNumActive && (m_ActiveControllerHandle != pHandles[0]) )
	{
		m_ActiveControllerHandle = pHandles[0];
	}
}

void kinc_steam_set_achievement(const char *achievementID)
{
	kinc_log(KINC_LOG_LEVEL_INFO, "\nKincSteam : Called achievement %s", achievementID);
	// Have we received a call back from Steam yet?
	if (true) {
		SteamUserStats()->SetAchievement(achievementID);
		SteamUserStats()->StoreStats();
	}

	//g_SteamAchievements->UnlockAchievement(g_Achievements[0]);
}
