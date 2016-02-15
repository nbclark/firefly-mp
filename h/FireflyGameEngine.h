#pragma once
#include "IGameHandler.h"
#include "IGameEngine.h"
#include "IGameState.h"
#include "IFireflyObjects.h"
#include "AccelerometerHandler.h"

// These events represent game state changes
typedef enum FireflyGameEvent
{
	FFM_LEVEL			= 0,
	FFM_COLLISION		= 1,
	FFM_LEVEL_OVER		= 2,
	FFM_GAME_OVER		= 3
} FireflyGameEvents;

struct FireflyMessage
{
	char  ev;
	int16 dataA;
	int16 dataB;
	int16 dataC;
};

inline void PrepareSend_FireflyMessage(FireflyMessage& host, FireflyMessage& network)
{
	network.dataA = s3eInetHtons(host.dataA);
	network.dataB = s3eInetHtons(host.dataB);
}

inline void PrepareReceive_FireflyMessage(FireflyMessage* pNetwork, FireflyMessage* pHost)
{
	pHost->dataA = s3eInetNtohs(pNetwork->dataA);
	pHost->dataB = s3eInetNtohs(pNetwork->dataB);
}

class FireflyGameEngine :
	public IGameEngine
{
public:
	FireflyGameEngine();
	~FireflyGameEngine(void);

	void Init(void* pGameState);
	bool Update(uint64 uiGameTimer, bool* pbVictory, bool* pbHasUpdate);
	void RenderGX();
	void Render2D();
	bool RenderPause();
	
	void ShowPause(bool bVisible);
	void ShowScore(bool bVisible);
	void ShowScoreAndPause(bool bVisible);
	bool RenderScore();
	bool RenderScoreAndPause(bool isScore);
	void RenderStatus(CIwUIRect& bounds);

	void Activate(CIwRect bounds);
	void DeActivate();
	bool UpdateLevel();
	int GetScore();
	void WriteLevelState();
	void WriteGameState();

	bool ClickToUnpause() { return false; }
	char* GetHighScoreFile() { return "fireflies.xml"; }

private:
	void OnClickResume(CIwUIElement* Clicked);

	static void ReceiveStatusUpdate(const char * Result, uint32 ResultLen, void* userData);
	void ResetObjects();
	void SetLevel();

	CIwUIElement* g_pDialogMain, *g_pUsers, *g_pResume;

	CIwArray<CFirefly*> g_activeFireflies;
	CIwArray<CBonusFirefly*> g_activeBonusFireflys;
	CIwArray<CMine*> g_activeMines;
	CGPSUser* g_pUser;
	CPulse* g_pPulse;
	CIwVec2 g_locJar;

	int g_iPulseCount;
	int g_iScore, g_iDisplayedScore;
	int g_iLevel;
	int g_iFireflyPoints;
	int g_iMinePoints;
	float g_iFireflyGlowDuration;
	float g_iStunDuration;
	uint16 g_iMineCount;
	uint16 g_iBonusFireflyCount;
	CIwFVec2 g_fFireflySpeed;
	bool g_bRenderBonusFirefly;
	bool g_bRenderMine;
	bool g_bLevelOver;
	bool g_bGameOver;
	bool g_bUnpause;

	CIwGxFont* g_pFont;
	CIwGxFont* g_pFontHuge;
	CIwGxFont* g_pFontSmall;
	CIwTexture* g_pTextureFirefly;
	CIwTexture* g_pTextureFireflyGlow;
	CIwTexture* g_pTextureFireflyStun;
	CIwTexture* g_pTextureBolt;
	CIwTexture* g_pTextureMine;
	CIwTexture* g_pTextureBonusFirefly;
	CIwTexture* g_pTextureBonusFireflyGlow;
	CIwTexture* g_pTextureLevel;
	CIwTexture* g_pTextureLevel1;
	CIwTexture* g_pTextureLevel2;
	CIwTexture* g_pTextureLevel3;
	CIwTexture* g_pTextureLevel4;
	CIwTexture* g_pTextureLevel5;
	CIwTexture* g_pTextureLevel6;
	CIwTexture* g_pTextureLevel7;

	char g_szStatus[100];

	IGameHandler* g_pGameHandler;
	AccelerometerHandler g_handler;

	CIwArray<CGPSUser*> g_users;
	CIwArray<CPulse*> g_pulses;
};
