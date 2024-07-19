#include "steam.h"
#include "steam/isteamuser.h"
#include "steam/isteamuserstats.h"
#include "steam/steam_api.h"

#ifndef STATS_AND_ACHIEVEMENTS_H
#define STATS_AND_ACHIEVEMENTS_H

enum EAchievements
{
	ACH_Finish_Game = 0,
	ACH_Finish_Game_Full = 1,
};

struct Achievement_t
{
	EAchievements m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class CSteamAchievements
{
private:
	int64_t m_iAppID; // Our current AppID
	Achievement_t *m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CSteamAchievements(Achievement_t *Achievements, int NumAchievements);
	~CSteamAchievements();
	void EvaluateAchievement(Achievement_t &achievement);
	void UnlockAchievement(Achievement_t &achievement);
	bool SetAchievement(const char* ID);

    // Steam User interface
	ISteamUser* m_pSteamUser;

	// Steam UserStats interface
	ISteamUserStats* m_pSteamUserStats;
};

#endif // STATS_AND_ACHIEVEMENTS_H