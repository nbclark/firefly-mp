#pragma once
#include "IwUI.h"
#include "MapBackground.h"
#include "IGameEngine.h"

typedef enum GPS_GameState
{
	GPS_GameState_Intro = 0,
	GPS_GameState_SelectMap = 1,
	GPS_GameState_CreateMap = 2,
	GPS_GameState_Active = 3,
	GPS_GameState_Pause = 4,
	GPS_GameState_SelectGame = 5,
	GPS_GameState_HighScore = 6,
	GPS_GameState_StaticContent = 7,
	GPS_GameState_End = 8
} GPS_GameState;

enum EAnimDirection
{
	AnimDir_Initial = 0,
	AnimDir_Left = 1,
	AnimDir_Right = 2
};

class IGameHandler
{
public:
	virtual void SetStaticContent(char*) {}
	virtual void SetGameState(GPS_GameState gameState, EAnimDirection direction) {}
	virtual void SetActiveUI(CIwUIElement* pDialogTemplate, IIwUIEventHandler* pEventHandler) {}
	virtual MapBackground* GetBackground() { return NULL; }
	virtual CoordinateScaler* GetScaler() { return NULL; }
	virtual void AddHighScore(int newScore, uint64 newTime, double distanceTravelled) {}

	virtual void StartTrackingDistance() {}
	virtual void StopTrackingDistance() {}
	virtual double GetDistanceTravelled() { return 0; }

	virtual int GetFramesPerSecond() { return 30; }

	virtual void SetLoadedMap(const char* szMap, bool bUseTilt) {}
	virtual char* GetLoadedMap() { return NULL; }
	virtual void Exit() {}

	virtual void SetGameEngine(IGameEngine* pGameEngine) {}
	virtual IGameEngine* GetGameEngine() { return NULL; }

	virtual void SetScaledCorners(std::list<s3eLocation>* pCorners, float zoom) { }
	virtual std::list<s3eLocation>* GetScaledCorners(float* pZoom) { return NULL; }

	virtual bool GetLocation(s3eLocation& location) { return false; }
	virtual CIwSVec2 GetPosition() { CIwSVec2 x; return x; }
	virtual CIwFVec2 GetAccuracy() { CIwFVec2 x; return x; }
	virtual bool IsTiltMode() { return false; }
	virtual bool IsGpsActive() { return false; }
};