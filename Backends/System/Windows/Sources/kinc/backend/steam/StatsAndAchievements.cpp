//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================


#include "StatsAndAchievements.h"
#include "steam.h"
#include <math.h>




#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

Achievement_t g_rgAchievements[] =
{
	_ACH_ID( ACH_Finish_Game, "WinGame" ),
};


CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements)
    : m_pSteamUser(NULL),
    m_pSteamUserStats(NULL), m_iAppID(SteamUtils()->GetAppID()) {
	m_pSteamUser = SteamUser();
	m_pSteamUserStats = SteamUserStats();
}

//-----------------------------------------------------------------------------
// Purpose: see if we should unlock this achievement
//-----------------------------------------------------------------------------
void CSteamAchievements::EvaluateAchievement(Achievement_t &achievement) {
	// Already have it?
	if ( achievement.m_bAchieved )
		return;

	switch ( achievement.m_eAchievementID )
	{
	case ACH_Finish_Game:
		if ( !achievement.m_bAchieved )
		{
			UnlockAchievement( achievement );
		}
		break;
	default:
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Unlock this achievement
//-----------------------------------------------------------------------------
void CSteamAchievements::UnlockAchievement(Achievement_t &achievement) {
	achievement.m_bAchieved = true;

	// the icon may change once it's unlocked
	achievement.m_iIconImage = 0;

	// mark it down
	m_pSteamUserStats->SetAchievement( achievement.m_pchAchievementID );

}
