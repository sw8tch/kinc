//#if defined(KINC_USE_STEAM)

#include "kinc/steam/steam.h"

#include "steam/isteaminput.h"
#include "steam/steam_api.h"
#include "steam/isteamfriends.h"



    InputDigitalActionHandle_t m_ControllerDigitalActionHandles[2];
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
	SteamFriends()->SetRichPresence("StatusInGame", "LOL ! ! !");
	
	//SteamFriends()->SetPersonaName("OMG ! !  !");
	m_ControllerDigitalActionHandles[0] = SteamInput()->GetDigitalActionHandle("Action_A");
	m_ControllerDigitalActionHandles[1] = SteamInput()->GetDigitalActionHandle("Action_B");
	return 1;
}
void kinc_steam_inputaction()
{
    //m_ControllerActionSetHandles[0] = SteamInput()->GetDigitalActionHandle( "#Action_A" );
	//m_ControllerActionSetHandles[1] = SteamInput()->GetDigitalActionHandle( "#Action_B" );
}

void kinc_steam_update()
{
    kinc_steam_input_findcontroller();
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
//#endif