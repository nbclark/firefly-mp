#pragma once
#include "gamestate.h"
#include "IGameObject.h"

class ActiveGameState :
	public GameState, public IGameHost
{
public:
	ActiveGameState(void);
	~ActiveGameState(void);

	virtual void PerformUpdate();
	virtual void PerformRender();

	virtual void PerformActivate();
	virtual void PerformDeActivate();
	
	virtual bool IsScore() { return g_bIsScore; }
	virtual void ShowScore()
	{
		if (g_pGameEngine)
		{
			g_pGameEngine->ShowScore(true);
			g_bIsScore = true;
		}
	}
	virtual void HideScore()
	{
		if (g_pGameEngine)
		{
			g_pGameEngine->ShowScore(false);
			g_bIsScore = false;
		}
	}

	virtual bool IsPaused() { return g_bIsPaused; }
	virtual void ShowPause()
	{
		if (g_pGameEngine)
		{
			g_pGameEngine->ShowScore(true);
		}
		g_bIsPaused = true;
	}
	virtual void HidePause()
	{
		if (g_pGameEngine)
		{
			g_pGameEngine->ShowScore(false);
		}
		g_bIsPaused = false;
	}

private:
	void RenderBackground();
	void OnClickBack(CIwUIElement* Clicked);
	static void MultiplayerDisconnect(bool success, const char* szStatus, void* userData);
	static void MultiplayerModeChanged(MultiplayerMode mode, void* userData);

	int g_cursorIter;
	CIw2DImage *g1, *g2, *g3;
	std::list<s3eLocation> g_downPos;

	bool g_bMouseDown, g_bRenderGame, g_bIsPaused, g_bIsScore, g_bGameOver;

	int g_iAnimationCount;
	int g_iStartGameCount;

	CBall* g_pBall;
	IGameObject* g_pUserPaddle;
	IGameObject* g_pCompPaddle;
	IGameEngine* g_pGameEngine;

	CIwUIElement* g_pTitleBar;
	CIwGxFont* g_pFont;
	float g_dAlpha;
	uint64 g_uiStartTime, g_mpLastFlush;
};
