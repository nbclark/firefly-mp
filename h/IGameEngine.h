#pragma once
#include "IwUI.h"

class IGameHost
{
public:
	virtual bool IsScore() { return false; }
	virtual void ShowScore() { }
	virtual void HideScore() { }

	virtual bool IsPaused() { return false; }
	virtual void ShowPause() { }
	virtual void HidePause() { }
};

class IGameEngine
{
public:
	virtual void Init(void* pGameState) {}
	virtual bool Update(uint64 uiGameTimer, bool* pbVictory, bool* pbHasUpdate) { return false; }
	virtual void RenderStatus(CIwUIRect& bounds) {}
	virtual void RenderGX() {}
	virtual void Render2D() {}

	virtual bool ClickToUnpause() { return true; }

	virtual void ShowPause(bool bVisible) {}
	virtual void ShowScore(bool bVisible) {}
	virtual bool RenderPause() { return false; }
	virtual bool RenderScore() { return false; }

	virtual void Activate(CIwRect bounds) { g_bounds = bounds; }
	virtual void DeActivate() {}
	virtual bool UpdateLevel() { return true; }
	virtual int GetScore() { return 0; }

	virtual void WriteLevelState() {}
	virtual void WriteGameState() {}

	virtual char* GetHighScoreFile() { return NULL; }

	void SetHost(IGameHost* pHost) { g_pHost = pHost; }

public :
	IGameHost* g_pHost;
	CIwRect g_bounds;
	CIwArray<void*> g_masterGameObjects;
	CIwArray<void*> g_slaveGameObjects;
	CIwArray<void*> g_userObjects;
};