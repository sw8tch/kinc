#include <kinc/backend/steam.h>
#include <steam/isteamuser.h>
#include <steam/isteamuserstats.h>
#include <steam/steam_api.h>
#include "kinc/service.h"
#include "kinc/log.h"
#include <vector>

#ifndef STATS_AND_ACHIEVEMENTS_H
#define STATS_AND_ACHIEVEMENTS_H

static void kinc_service_internal_call_achievementupdate_callback(bool achievement);
static void kinc_service_internal_call_userstatupdate_callback(int statid, int statvalue);

class CSteamAchievements
{
private:
	int64_t m_iAppID; // Our current AppID
	//Achievement_t *m_pAchievements; // Achievements data
	std::vector<kinc_achievement_t> m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CSteamAchievements();
	~CSteamAchievements();

	STEAM_CALLBACK(CSteamAchievements, OnUserAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);

    void Update();

	void CacheAchievements();
	void EvaluateAchievement(kinc_achievement_t &achievement);
	void UnlockAchievement(kinc_achievement_t &achievement);
	bool SetAchievement(const char* ID);
	bool FindAchievementByID(const char *ID, kinc_achievement_t &achievement);

    // Steam User interface
	ISteamUser* m_pSteamUser;

	// Steam UserStats interface
	ISteamUserStats* m_pSteamUserStats;
};

#endif // STATS_AND_ACHIEVEMENTS_H