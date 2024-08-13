//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================


#include "StatsAndAchievements.h"
#include "steam.h"
#include <kinc/service.h>
#include <math.h>




#define _ACH_ID(id, name) { id, name, "", 0, 0 }

CSteamAchievements::CSteamAchievements()
    : m_pSteamUserStats(NULL),
    m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
    m_CallbackAchievementStored(this, &CSteamAchievements::OnUserAchievementStored),
    m_iAppID(SteamUtils()->GetAppID()),
	m_pSteamUser(SteamUser())
{
	m_pSteamUserStats = SteamUserStats();
	CacheAchievements();

    kinc_achievement_t caca;

    FindAchievementByID("ACH_Finish_Normal", caca);

}



void CSteamAchievements::Update()
{
	bool bSuccess = m_pSteamUserStats->RequestCurrentStats();
}

void CSteamAchievements::CacheAchievements()
{
	int numAchievements = m_pSteamUserStats->GetNumAchievements();
	for (int i = 0; i < numAchievements; ++i) {
		const char *achName = SteamUserStats()->GetAchievementName(i);
		if (achName) {
			kinc_achievement_t newAchievement;//= kinc_achievement_t(achName);
			if (m_pSteamUserStats->GetUserAchievement(m_pSteamUser->GetSteamID(), achName, &newAchievement.m_bAchieved))
            {
				strcpy(newAchievement.m_pchAchievementID, achName);
				strcpy(newAchievement.m_cDescription, m_pSteamUserStats->GetAchievementDisplayAttribute(achName, "desc"));
				strcpy(newAchievement.m_cName, m_pSteamUserStats->GetAchievementDisplayAttribute(achName, "name"));
				//newAchievement.m_cDescription = m_pSteamUserStats->GetAchievementDisplayAttribute(achName, "desc");
				//newAchievement.m_cName = m_pSteamUserStats->GetAchievementDisplayAttribute(achName, "name");
				newAchievement.m_bHidden = m_pSteamUserStats->GetAchievementDisplayAttribute(achName, "hidden") == "1" ? true:false;
				newAchievement.m_iIconImage = m_pSteamUserStats->GetAchievementIcon(achName);
				m_pAchievements.push_back(newAchievement);
            }
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: see if we should unlock this achievement
//-----------------------------------------------------------------------------
void CSteamAchievements::EvaluateAchievement(kinc_achievement_t &achievement) {
	// Already have it?
    if (!achievement.m_bAchieved)
    {
		UnlockAchievement(achievement);
    }
}


//-----------------------------------------------------------------------------
// Purpose: Unlock this achievement
//-----------------------------------------------------------------------------
void CSteamAchievements::UnlockAchievement(kinc_achievement_t &achievement) {
	achievement.m_bAchieved = true;

	// the icon may change once it's unlocked
	achievement.m_iIconImage = 0;

	// mark it down
	m_pSteamUserStats->SetAchievement( achievement.m_pchAchievementID );
	m_pSteamUserStats->StoreStats();
}

bool CSteamAchievements::SetAchievement(const char *ID) {
	kinc_achievement_t achievement;
	if (FindAchievementByID(ID, achievement)) {
		EvaluateAchievement(achievement);
		return true;
	}
    else {
		return false;
    }
}

bool CSteamAchievements::FindAchievementByID(const char *ID, kinc_achievement_t &achievement) {
	auto result =
	    std::find_if(m_pAchievements.begin(), m_pAchievements.end(), [&ID](kinc_achievement_t &ach) { return std::strcmp(ach.m_pchAchievementID, ID) == 0; });
	if (result != m_pAchievements.end()) {
		achievement = *result;
        return true;
	}
    else
    {
		return false;
    }
}

//-----------------------------------------------------------------------------
// Purpose: Achievement Stored callback
//-----------------------------------------------------------------------------
void CSteamAchievements::OnUserAchievementStored(UserAchievementStored_t *pCallback) {
	//kinc_service_internal_call_achievementupdate_callback(true);
}

//-----------------------------------------------------------------------------
// Purpose: received user stats callback
//-----------------------------------------------------------------------------
void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback) {
	//kinc_service_internal_call_achievementupdate_callback(true);
}

//-----------------------------------------------------------------------------
// Purpose: Stored user stats callback
//-----------------------------------------------------------------------------
void CSteamAchievements::OnUserStatsStored(UserStatsStored_t *pCallback) {
	//kinc_service_internal_call_achievementupdate_callback(true);
}

static void kinc_service_internal_call_achievementupdate_callback(bool achievement) {
	kinc_log(KINC_LOG_LEVEL_INFO, "achievement is %s", achievement ? "unlocked" : "locked");
}

static void kinc_service_internal_call_userstatupdate_callback(int statid, int statvalue)
{
	kinc_log(KINC_LOG_LEVEL_INFO, "stat %i updated to %i", statid, statvalue);
}