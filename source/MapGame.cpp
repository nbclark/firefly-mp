#include "Main.h"
#include "MapGame.h"
#include "s3e.h"
#include "IwUI.h"
#include "MessageBox.h"
#include "IntroGameState.h"
#include "CreateMapGameState.h"
#include "SelectMapGameState.h"
#include "SelectGameGameState.h"
#include "ActiveGameState.h"
#include "HighScoreGameState.h"
#include "StaticContentGameState.h"


s3eFile* g_pFile = NULL;

CIwMapGame::CIwMapGame()
{
	g_szLoadedMap = new char[200];
	g_szLoadedMap[0] = 0;
	g_bNeedsExit = false;

	g_direction = AnimDir_Initial;
	g_pFile = s3eFileOpen("coords.txt", "w");
	g_bUseTilt = false;
	g_bTrackingDistance = false;
	g_lastGpsLookup = 0;
	g_xTiltOffset = g_yTiltOffset = 0;
}

CIwMapGame::~CIwMapGame()
{
	delete[] g_szLoadedMap;

	std::list<GameStatePair*>::iterator iter = g_gameStates.begin();

	while (iter != g_gameStates.end())
	{
		GameStatePair* pPair = *iter;
		delete pPair->pGameState;
		delete pPair;
		iter++;
	}
	s3eFileFlush(g_pFile);
	s3eFileClose(g_pFile);
}

void CIwMapGame::Init()
{
	g_mapBackground = new MapBackground;
	g_gameState = NULL;

	g_mapBackground->Init();

	GameStatePair* pIntro = new GameStatePair;
	pIntro->gameState = GPS_GameState_Intro;
	pIntro->pGameState = new IntroGameState(this);
	pIntro->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pIntro);

	GameStatePair* pSelectMap = new GameStatePair;
	pSelectMap->gameState = GPS_GameState_SelectMap;
	pSelectMap->pGameState = new SelectMapGameState;
	pSelectMap->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pSelectMap);

	GameStatePair* pCreateMap = new GameStatePair;
	pCreateMap->gameState = GPS_GameState_CreateMap;
	pCreateMap->pGameState = new CreateMapGameState;
	pCreateMap->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pCreateMap);

	GameStatePair* pActive = new GameStatePair;
	pActive->gameState = GPS_GameState_Active;
	pActive->pGameState = new ActiveGameState;
	pActive->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pActive);

	GameStatePair* pSelectGame = new GameStatePair;
	pSelectGame->gameState = GPS_GameState_SelectGame;
	pSelectGame->pGameState = new SelectGameGameState(this);
	pSelectGame->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pSelectGame);

	GameStatePair* pHighScore = new GameStatePair;
	pHighScore->gameState = GPS_GameState_HighScore;
	pHighScore->pGameState = new HighScoreGameState();
	pHighScore->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pHighScore);

	GameStatePair* pStaticContent = new GameStatePair;
	pStaticContent->gameState = GPS_GameState_StaticContent;
	pStaticContent->pGameState = new StaticContentGameState();
	pStaticContent->pGameState->SetGameHandler(this);
	g_gameStates.push_back(pStaticContent);

	g_pDialogCurrent = NULL;
	g_bRenderBackground = false;

	g_cursorIter = 1;

	g_bShowCursor = true;

	// Get the current location
	if (S3E_RESULT_SUCCESS != s3eLocationGet(&gLocation))
	{
		//47.76216699999999
		//-122.160358
		gLocation.m_Latitude = 47.7710083;
		gLocation.m_Longitude = -122.1588533;
	}

	g_mapBackground->SetLocation((float)gLocation.m_Longitude, (float)gLocation.m_Latitude);

	SetGameState(GPS_GameState_Intro, AnimDir_Initial);
}

void CIwMapGame::Exit()
{
	g_bNeedsExit = true;
}

void CIwMapGame::ClearTilt()
{
	g_xTiltOffset = g_yTiltOffset = 0;
	g_xTiltVelo = g_yTiltVelo = 0;

	g_xTiltPos = Iw2DGetSurfaceWidth() / 2.0f;
	g_yTiltPos = Iw2DGetSurfaceHeight() / 2.0f;
}

CIwFVec2 CIwMapGame::GetAccuracy()
{
	s3eLocation loc;
	CIwFVec2 accuracy;

	if (g_bUseTilt)
	{
		accuracy.x = accuracy.y = 1;
	}
	else if (GetLocation(loc))
	{
		accuracy.x = (float)loc.m_HorizontalAccuracy;
		accuracy.y = (float)loc.m_VerticalAccuracy;
	}
	else
	{
		accuracy.x = accuracy.y = 100;
	}

	return accuracy;
}

bool CIwMapGame::IsTiltMode()
{
	return g_bUseTilt;
}

CIwSVec2 CIwMapGame::GetPosition()
{
	CIwSVec2 pos;
	s3eLocation loc;

	static float tiltVeloScale = 50.0 / Utils::GetTextScalingFactor();
	
	if (g_bUseTilt)
	{
		// Turn off location
		uint64 timer = s3eTimerGetMs();

		if ((timer - g_lastAccelLookup) > 100)
		{
			int32 orientation = s3eSurfaceGetInt(S3E_SURFACE_DEVICE_BLIT_DIRECTION);

			// do some processing
			g_lastAccelLookup = timer;
			int32 xAcc = s3eAccelerometerGetX();
			int32 yAcc = s3eAccelerometerGetY();

			if (orientation == S3E_SURFACE_BLIT_DIR_ROT180)
			{
				yAcc = -yAcc;
				xAcc = -xAcc;
			}

			// Scale to approximately gravity, invert Y
			g_xTiltVelo = (xAcc / tiltVeloScale);
			g_yTiltVelo = (-yAcc / tiltVeloScale);

			g_xTiltVelo = (float)((g_xTiltVelo > 0) ? pow(g_xTiltVelo, 1.5) : -pow(ABS(g_xTiltVelo), 1.5));
			g_yTiltVelo = (float)((g_yTiltVelo > 0) ? pow(g_yTiltVelo, 1.5) : -pow(ABS(g_yTiltVelo), 1.5));

			g_xTiltPos += (g_xTiltVelo / 1);
			g_yTiltPos += (g_yTiltVelo / 1);

			g_xTiltPos = MIN(MAX(g_xTiltPos, 0), Iw2DGetSurfaceWidth());
			g_yTiltPos = MIN(MAX(g_yTiltPos, 0), Iw2DGetSurfaceHeight());
		}
		pos.x = (int)g_xTiltPos;
		pos.y = (int)g_yTiltPos;
	}
	else
	{
		if (GetLocation(loc))
		{
			if (s3eKeyboardGetState(s3eKeyRight) & S3E_KEY_STATE_DOWN)
			{
				g_xTiltOffset += 30;
			}
			if (s3eKeyboardGetState(s3eKeyLeft) & S3E_KEY_STATE_DOWN)
			{
				g_xTiltOffset -= 30;
			}
			if (s3eKeyboardGetState(s3eKeyDown) & S3E_KEY_STATE_DOWN)
			{
				g_yTiltOffset += 30;
			}
			if (s3eKeyboardGetState(s3eKeyUp) & S3E_KEY_STATE_DOWN)
			{
				g_yTiltOffset -= 30;
			}

			g_mapBackground->GetScaler()->LocationToPosition(loc, &pos);

			pos.x += g_xTiltOffset;
			pos.y += g_yTiltOffset;
		}
		else
		{
			s3eDebugTraceLine("Failed to get position");
			pos.x = pos.y = 0;
		}
	}

	return pos;
}

bool CIwMapGame::IsGpsActive()
{
	if (s3eLocationAvailable())
	{
		s3eLocation loc;
		
		s3eResult result = s3eLocationGet(&loc);
		if (S3E_RESULT_SUCCESS == result)
		{
			return true;
		}
	}
	s3eLocationStart();

	return false;
}

void CIwMapGame::SetGameEngine(IGameEngine* pGameEngine)
{
	g_pGameEngine = pGameEngine;
}

IGameEngine* CIwMapGame::GetGameEngine()
{
	return g_pGameEngine;
}

CoordinateScaler* CIwMapGame::GetScaler()
{
	return g_mapBackground->GetScaler();
}

bool CIwMapGame::GetLocation(s3eLocation& location)
{
	uint64 timer = s3eTimerGetMs();

	if (g_lastGpsLookup == 0 || (timer - g_lastGpsLookup) > 500)
	{
		if (S3E_RESULT_SUCCESS != s3eLocationGet(&location))
		{
			return false;
		}
		s3eFilePrintf(g_pFile, "%lld\t%lld\t%4.9f\t%4.9f\t%4.9f\t%4.9f\t%4.9f\r\n", timer, location.m_TimeStampUTC, location.m_Longitude, location.m_Latitude, location.m_Altitude, location.m_HorizontalAccuracy, location.m_VerticalAccuracy);
		s3eFileFlush(g_pFile);

		if (g_bTrackingDistance)
		{
			g_distanceTravelled += LiveMaps::CalculateDistance(g_lastLocation, location);
		}

		g_lastLocation = location;
		g_lastGpsLookup = timer;
		return true;
	}
	else
	{
		if (g_lastGpsLookup)
		{
			location = g_lastLocation;
			return true;
		}
		return false;
	}
}

int CIwMapGame::GetFramesPerSecond()
{
	return Utils::FPS;
}

char* CIwMapGame::GetLoadedMap()
{
	return g_szLoadedMap;
}

void CIwMapGame::SetLoadedMap(const char* szMap, bool bUseTilt)
{
	g_bUseTilt = bUseTilt;
	strncpy(g_szLoadedMap, szMap, 200);

	if (g_bUseTilt)
	{
		s3eLocationStop();
	}
	else
	{
		s3eLocationStart();
	}
}

void CIwMapGame::SetScaledCorners(std::list<s3eLocation>* pCorners, float zoom)
{
	g_curZoom = zoom;
	g_scaledCorners.clear();
	std::list<s3eLocation>::iterator iter = pCorners->begin();
	s3eLocation topLeft, topRight, bottomLeft, bottomRight;

	int i = 0;
	while (iter != pCorners->end())
	{
		switch (i)
		{
			case 0:
			{
				topLeft = *iter;
			}
			break;
			case 1:
			{
				topRight = *iter;
			}
			break;
			case 2:
			{
				bottomLeft = *iter;
			}
			break;
			case 3:
			{
				bottomRight = *iter;
			}
			break;
		}
		g_scaledCorners.push_back(*iter);
		iter++;
		i++;
	}

	g_mapBackground->ClearScaledCorners();
	g_mapBackground->SetScaledCorners(topLeft, topRight, bottomLeft, bottomRight, zoom);
	g_mapBackground->Update(true);
	g_mapBackground->DownloadTiles();
}

std::list<s3eLocation>* CIwMapGame::GetScaledCorners(float* pZoom)
{
	if (pZoom)
	{
		*pZoom = g_curZoom;
	}
	return &g_scaledCorners;
}

void CIwMapGame::AddHighScore(int newScore, uint64 newTime, double distanceTravelled)
{
	HighScoreGameState* pHighScore = (HighScoreGameState*)GetGameState(GPS_GameState_HighScore);
	pHighScore->AddScore(newScore, newTime, distanceTravelled);
}

void CIwMapGame::SetStaticContent(char* szPanel)
{
	StaticContentGameState* pStaticContent = (StaticContentGameState*)GetGameState(GPS_GameState_StaticContent);

	pStaticContent->SetContent(szPanel);
}


IGameState* CIwMapGame::GetGameState(GPS_GameState gameState)
{
	std::list<GameStatePair*>::iterator iter = g_gameStates.begin();
	while (iter != g_gameStates.end())
	{
		GameStatePair* pPair = *iter;
		if (pPair->gameState == gameState)
		{
			return pPair->pGameState;
		}
		iter++;
	}
	return NULL;
}

void CIwMapGame::SetGameState(GPS_GameState gameState, EAnimDirection direction)
{
	g_direction = direction;

	if (g_gameState)
	{
		g_gameState->pGameState->DeActivate();
		g_gameState = NULL;
	}

	if (gameState == GPS_GameState_Active)
	{
		ClearTilt();
	}


	std::list<GameStatePair*>::iterator iter = g_gameStates.begin();
	while (iter != g_gameStates.end())
	{
		GameStatePair* pPair = *iter;
		if (pPair->gameState == gameState)
		{
			g_gameState = pPair;
			g_mapBackground->ClearScaledCorners();
			pPair->pGameState->Activate();
		}
		iter++;
	}
}

void CIwMapGame::SetActiveUI(CIwUIElement* pDialogTemplate, IIwUIEventHandler* pEventHandler)
{
	CIwUIElement* pBackground = NULL;
	
	if (g_pDialogCurrent)
	{
		//IwGetUIAnimManager()->PlayAnim("slideOutAnim", g_pDialogCurrent, false);
		g_pDialogCurrent->SetVisible(false);

		pBackground = NULL;//g_pDialogCurrent->GetChildNamed("Background");

		if (pBackground)
		{
			pBackground->RemoveChild(g_mapBackground);
		}
	}

	//And instantiate it
	pBackground = NULL;//pDialogTemplate->GetChildNamed("Background");

	if (pBackground)
	{
		g_mapBackground->SetPos(CIwVec2(0, 0), false);
		g_mapBackground->SetSize(CIwVec2(Iw2DGetSurfaceWidth(), Iw2DGetSurfaceHeight()));
		pBackground->AddChild((CIwUIElement*)g_mapBackground);
	}

	if (pEventHandler)
	{
		IwGetUIController()->AddEventHandler(pEventHandler);
	}
	g_pDialogCurrent = pDialogTemplate;
	IwGetUIAnimManager()->StopAnim(pDialogTemplate);
	pDialogTemplate->SetVisible(false);

	switch (g_direction)
	{
		case AnimDir_Left :
		{
			IwGetUIAnimManager()->PlayAnim("slideInAnim", pDialogTemplate, false);
		}
		break;
		case AnimDir_Right :
		{
			IwGetUIAnimManager()->PlayAnim("slideOutAnim", pDialogTemplate, false);
		}
		break;
		default :
		{
			IwGetUIAnimManager()->PlayAnim("zoominAnim", pDialogTemplate, false);
			pDialogTemplate->SetVisible(true);
		}
		break;
	}
	//pDialogTemplate->SetVisible(true);
}

//-----------------------------------------------------------------------------
void CIwMapGame::ShutDown()
{
	//s3eLocationStop();

	//delete g_pDialogMain;
	//delete g_pDialogSelectMap;

	CIwUIElement* pParent = g_mapBackground->GetParent();
	if (pParent)
	{
		pParent->RemoveChild(g_mapBackground);
	}

	g_mapBackground->ShutDown();

	delete g_mapBackground;
}

MapBackground* CIwMapGame::GetBackground()
{
	return g_mapBackground;
}

bool CIwMapGame::Update()
{
	if (g_gameState)
	{
		g_gameState->pGameState->Update();
	}
	return !g_bNeedsExit;
}

void CIwMapGame::Render()
{
	if (g_gameState)
	{
		g_gameState->pGameState->Render();

		if (g_pDialogCurrent)
		{
			if (!g_pDialogCurrent->IsVisible())
			{
				g_pDialogCurrent->SetVisible(true);
			}
		}
	}
}