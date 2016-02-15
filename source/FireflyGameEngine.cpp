#include "FireflyGameEngine.h"
#include "MessageBox.h"
#include "Utils.h"
#include <s3eVibra.h>

#define FIREFLY_MAX_LEVEL	6

FireflyGameEngine::FireflyGameEngine()
{
}

void FireflyGameEngine::Init(void* pGameStateVoid)
{
	IW_UI_CREATE_VIEW_SLOT1(this, "FireflyGameEngine", FireflyGameEngine, OnClickResume, CIwUIElement*)

	g_pDialogMain = (CIwUIElement*)IwGetResManager()->GetResNamed("pause\\panel", "CIwUIElement");
	g_pResume = (CIwUIElement*)g_pDialogMain->GetChildNamed("Resume_Button", true, false);

	g_pDialogMain->SetVisible(false);
	IwGetUIView()->AddElement(g_pDialogMain);
	IwGetUIView()->AddElementToLayout(g_pDialogMain);

	IGameHandler* pGameState = (IGameHandler*)pGameStateVoid;
	g_pGameHandler = pGameState;

	/*
	g_pFontHuge = (CIwGxFont*)IwGetResManager()->GetResNamed("font_large", "CIwGxFont");
	g_pFontSmall = (CIwGxFont*)IwGetResManager()->GetResNamed("font_small", "CIwGxFont");
	*/

	g_pFontSmall = Utils::GetFont(false);
	g_pFontHuge = Utils::GetFont(true);
	
	CIwImage imgFirefly, imgFireflyGlow, imgFireflyStun, imgBolt, imgMine, imgBonusFirefly, imgBonusFireflyGlow, imgLevel, imgLevel1, imgLevel2, imgLevel3, imgLevel4, imgLevel5, imgLevel6, imgLevel7;
	//imgFirefly.LoadFromFile("images/fireflies/firefly.png");
	//imgFireflyGlow.LoadFromFile("images/fireflies/firefly_glow.png");
	//imgFireflyStun.LoadFromFile("images/fireflies/firefly_blue.png");
	//imgBolt.LoadFromFile("images/fireflies/bolt.png");
	//imgMine.LoadFromFile("images/fireflies/mine.png");
	//imgBonusFirefly.LoadFromFile("images/fireflies/bonusFirefly.png");
	//imgBonusFireflyGlow.LoadFromFile("images/fireflies/bonusFirefly_glow.png");
	imgLevel.LoadFromFile("level.tga");
	imgLevel1.LoadFromFile("level1.png");
	imgLevel2.LoadFromFile("level2.png");
	imgLevel3.LoadFromFile("level3.png");
	imgLevel4.LoadFromFile("level4.png");
	imgLevel5.LoadFromFile("level5.png");
	imgLevel6.LoadFromFile("level6.png");
	imgLevel7.LoadFromFile("level7.png");

	g_pTextureFirefly = (CIwTexture*)IwGetResManager()->GetResNamed("firefly", "CIwTexture");//new CIwTexture();
	//g_pTextureFirefly->CopyFromImage(&imgFirefly);
	//g_pTextureFirefly->Upload();

	g_pTextureFireflyGlow = (CIwTexture*)IwGetResManager()->GetResNamed("firefly_glow", "CIwTexture");//new CIwTexture();
	//g_pTextureFireflyGlow->CopyFromImage(&imgFireflyGlow);
	//g_pTextureFireflyGlow->Upload();

	g_pTextureFireflyStun = (CIwTexture*)IwGetResManager()->GetResNamed("firefly_blue", "CIwTexture");//new CIwTexture();
	//g_pTextureFireflyStun->CopyFromImage(&imgFireflyStun);
	//g_pTextureFireflyStun->Upload();

	g_pTextureBolt = (CIwTexture*)IwGetResManager()->GetResNamed("bolt", "CIwTexture");//new CIwTexture();
	//g_pTextureBolt->CopyFromImage(&imgBolt);
	//g_pTextureBolt->Upload();

	g_pTextureMine = (CIwTexture*)IwGetResManager()->GetResNamed("mine", "CIwTexture");//new CIwTexture();
	//g_pTextureMine->CopyFromImage(&imgMine);
	//g_pTextureMine->Upload();

	g_pTextureBonusFirefly = (CIwTexture*)IwGetResManager()->GetResNamed("bonusFirefly", "CIwTexture");//new CIwTexture();
	//g_pTextureBonusFirefly->CopyFromImage(&imgBonusFirefly);
	//g_pTextureBonusFirefly->Upload();

	g_pTextureBonusFireflyGlow = (CIwTexture*)IwGetResManager()->GetResNamed("bonusFirefly_glow", "CIwTexture");//new CIwTexture();
	//g_pTextureBonusFireflyGlow->CopyFromImage(&imgBonusFireflyGlow);
	//g_pTextureBonusFireflyGlow->Upload();

	g_pTextureLevel = new CIwTexture();
	g_pTextureLevel->CopyFromImage(&imgLevel);
	g_pTextureLevel->Upload();

	g_pTextureLevel1 = new CIwTexture();
	g_pTextureLevel1->CopyFromImage(&imgLevel1);
	g_pTextureLevel1->Upload();

	g_pTextureLevel2 = new CIwTexture();
	g_pTextureLevel2->CopyFromImage(&imgLevel2);
	g_pTextureLevel2->Upload();

	g_pTextureLevel3 = new CIwTexture();
	g_pTextureLevel3->CopyFromImage(&imgLevel3);
	g_pTextureLevel3->Upload();

	g_pTextureLevel4 = new CIwTexture();
	g_pTextureLevel4->CopyFromImage(&imgLevel4);
	g_pTextureLevel4->Upload();

	g_pTextureLevel5 = new CIwTexture();
	g_pTextureLevel5->CopyFromImage(&imgLevel5);
	g_pTextureLevel5->Upload();

	g_pTextureLevel6 = new CIwTexture();
	g_pTextureLevel6->CopyFromImage(&imgLevel6);
	g_pTextureLevel6->Upload();

	g_pTextureLevel7 = new CIwTexture();
	g_pTextureLevel7->CopyFromImage(&imgLevel7);
	g_pTextureLevel7->Upload();

	for (uint32 i = 0; i < 20; ++i)
	{
		CFirefly* pObject = new CFirefly(pGameState, g_pTextureFirefly, g_pTextureFireflyGlow, g_pTextureFireflyStun);
		g_masterGameObjects.append(pObject);
		g_activeFireflies.push_back(pObject);
	}

	for (uint32 i = 0; i < 15; ++i)
	{
		CBonusFirefly* pObject = new CBonusFirefly(pGameState, g_pTextureBonusFirefly, g_pTextureBonusFireflyGlow, g_pTextureFireflyStun);
		g_masterGameObjects.append(pObject);
		g_activeBonusFireflys.push_back(pObject);
	}

	for (uint32 i = 0; i < 15; ++i)
	{
		CMine* pObject = new CMine(pGameState, g_pTextureMine);
		g_masterGameObjects.append(pObject);
		g_activeMines.push_back(pObject);
	}
	g_pUser = NULL;
	g_pPulse = NULL;
}

FireflyGameEngine::~FireflyGameEngine(void)
{
	//delete g_pTextureFirefly;
	//delete g_pTextureFireflyGlow;
	//delete g_pTextureFireflyStun;
	//delete g_pTextureBolt;
	//delete g_pTextureMine;
	//delete g_pTextureBonusFirefly;
	//delete g_pTextureBonusFireflyGlow;
	delete g_pTextureLevel;
	delete g_pTextureLevel1;
	delete g_pTextureLevel2;
	delete g_pTextureLevel3;
	delete g_pTextureLevel4;
	delete g_pTextureLevel5;
	delete g_pTextureLevel6;
	delete g_pTextureLevel7;

	for (uint32 i = 0; i < g_activeFireflies.size(); ++i)
	{
		delete g_activeFireflies[i];
	}
	g_activeFireflies.clear();

	for (uint32 i = 0; i < g_activeBonusFireflys.size(); ++i)
	{
		delete g_activeBonusFireflys[i];
	}
	g_activeBonusFireflys.clear();

	for (uint32 i = 0; i < g_activeMines.size(); ++i)
	{
		delete g_activeMines[i];
	}
	g_activeMines.clear();

	DeActivate();
}

void FireflyGameEngine::ResetObjects()
{
	CIwFVec2 metersPerSecond(1, 1);
	float glowCycleSecondDuration = g_iFireflyGlowDuration;
	metersPerSecond.x = g_fFireflySpeed.x;
	metersPerSecond.y = g_fFireflySpeed.y;
	
	for (uint32 i = 0; i < g_activeFireflies.size(); ++i)
	{
		g_activeFireflies[i]->SetTopBound(g_bounds.y);
		g_activeFireflies[i]->SetActive(true);
		g_activeFireflies[i]->SetDifficulty(metersPerSecond, glowCycleSecondDuration);
	}

	CIwFVec2 metersPerSecondD(metersPerSecond.x * 3, metersPerSecond.y * 3);
	for (uint32 i = 0; i < g_activeBonusFireflys.size(); ++i)
	{
		g_activeBonusFireflys[i]->SetTopBound(g_bounds.y);
		g_activeBonusFireflys[i]->SetActive(true);
		g_activeBonusFireflys[i]->SetDifficulty(metersPerSecond, glowCycleSecondDuration / 2);
	}
	
	for (uint32 i = 0; i < g_activeMines.size(); ++i)
	{
		g_activeMines[i]->SetTopBound(g_bounds.y);
		g_activeMines[i]->SetActive(true);
		g_activeMines[i]->Reset();
	}

	g_pUser->Reset();
}

void FireflyGameEngine::Activate(CIwRect bounds)
{
	g_bounds = bounds;
	DeActivate();

	g_iLevel = 0;
	g_iPulseCount = 2;
	g_iScore = g_iDisplayedScore = 0;
	g_handler.Start(1500, 3, 1000);

	// We need to wire up all of our objects here, I think
	for (uint32 i = 0; i < g_masterGameObjects.size(); ++i)
	{
		((IGameObject*)g_masterGameObjects[i])->ResetMultiplayer();
	}

	IwGetMultiplayerHandler()->RegisterCallback(0, ReceiveStatusUpdate, this);

	// Add multiplayer-users
	if (IwGetMultiplayerHandler()->IsMultiplayer())
	{
		CIwArray<CIwMultiplayerHandler::User*> users = IwGetMultiplayerHandler()->ListUsers();

		for (uint32 uu = 0; uu < users.size(); ++uu)
		{
#ifndef VIDEOMODE
			CGPSUser* pUser =  new CGPSUser(g_pGameHandler, users[uu], users[uu]->IsMe ? (char*)"images/fireflies/user.png" : (char*)"images/fireflies/userB.png", users[uu]->IsMe);
			CPulse* pPulse = new CPulse(g_pGameHandler, pUser, users[uu]->IsMe);
#endif
#ifdef VIDEOMODE
			CGPSUser* pUser =  new CGPSUser(g_pGameHandler, users[uu], (users[uu]->IsMe || uu == 0) ? (char*)"images/fireflies/user.png" : (char*)"images/fireflies/userB.png", false);
			CPulse* pPulse = new CPulse(g_pGameHandler, pUser, false);
#endif
			
#ifndef VIDEOMODE
			if (users[uu]->IsMe)
#endif
#ifdef VIDEOMODE
			if (users[uu]->IsMe || uu == 0)
#endif

			{
				g_pUser = pUser;
				g_pPulse = pPulse;
			}

			g_users.append(pUser);
			g_pulses.append(pPulse);

			g_userObjects.append(pUser);

			// Send all the user data and pulses
			g_masterGameObjects.append(pUser);
			g_masterGameObjects.append(pPulse);

			pUser->ResetMultiplayer();
			pPulse->ResetMultiplayer();
		}
	}
	else
	{
		g_pUser = new CGPSUser(g_pGameHandler, NULL, (char*)"images/fireflies/user.png", true);
		g_pPulse = new CPulse(g_pGameHandler, g_pUser, true);

		g_users.append(g_pUser);
		g_pulses.append(g_pPulse);
	}

	if (g_pGameHandler->IsTiltMode())
	{
		s3eLocationStop();
	}
	else
	{
		s3eLocationStart();
	}

	SetLevel();
}

void FireflyGameEngine::DeActivate()
{
	for (uint32 i = 0; i < g_users.size(); ++i)
	{
		g_masterGameObjects.find_and_remove(g_users[i]);
		delete g_users[i];
	}
	g_users.clear();

	for (uint32 i = 0; i < g_pulses.size(); ++i)
	{
		g_masterGameObjects.find_and_remove(g_pulses[i]);
		delete g_pulses[i];
	}
	g_pulses.clear();
	g_handler.Stop();

	g_pUser = NULL;
	g_pPulse = NULL;
}

bool FireflyGameEngine::UpdateLevel()
{
	g_iPulseCount++;
	g_iLevel = g_iLevel + 1;

#ifdef GAME_TRIALMODE
	if (g_iLevel > 1)
	{
		return true;
	}
#endif

	if (g_iLevel > FIREFLY_MAX_LEVEL)
	{
		// Signal game over
		return true;
	}

	SetLevel();
	return false;
}

int FireflyGameEngine::GetScore()
{
	return g_pUser->GetScore();
}

void FireflyGameEngine::SetLevel()
{
	g_bLevelOver = false;
	g_bGameOver = false;
	g_bRenderBonusFirefly = false;
	g_bRenderMine = false;
	g_iFireflyPoints = 100;
	g_iMinePoints = 800;
	g_iFireflyGlowDuration = 20;
	g_fFireflySpeed.x = 5;
	g_fFireflySpeed.y = 5;
	g_iMineCount = g_iBonusFireflyCount = 0;
	g_iStunDuration = 10;
	
	g_iBonusFireflyCount = g_iLevel + 1;
	g_bRenderBonusFirefly = true;

	g_iMineCount = g_iLevel + 1;
	g_bRenderMine = true;

	switch (g_iLevel)
	{
		case 1 :
			{
				g_iFireflyPoints = 200;
				g_iBonusFireflyCount = 0;
			}
			break;
		case 2 :
			{
				g_iFireflyPoints = 300;
				g_bRenderBonusFirefly = true;
			}
			break;
		case 3 :
			{
				g_iFireflyPoints = 400;
				g_bRenderBonusFirefly = true;
				g_bRenderMine = true;
				g_iMineCount = 5;
			}
			break;
		case 4 :
			{
				g_iFireflyPoints = 600;
				g_bRenderBonusFirefly = true;
				g_bRenderMine = true;

				g_iFireflyGlowDuration = 15;
				g_fFireflySpeed.x = g_fFireflySpeed.y = 7;
				g_iMineCount = 5;
			}
			break;
		case 5 :
			{
				g_iFireflyPoints = 800;
				g_bRenderBonusFirefly = true;
				g_bRenderMine = true;

				g_iFireflyGlowDuration = 10;
				g_fFireflySpeed.x = g_fFireflySpeed.y = 10;
				g_iMineCount = 10;
			}
			break;
		case 6 :
			{
				g_iFireflyPoints = 1000;
				g_bRenderBonusFirefly = true;
				g_bRenderMine = true;

				g_iFireflyGlowDuration = 5;
				g_fFireflySpeed.x = g_fFireflySpeed.y = 15;
				g_iMineCount = 15;
			}
			break;
		default :
			{
			}
			break;
	}
	g_iMinePoints = g_iFireflyPoints * 2;

	ResetObjects();

	// set the # of bonusFireflyflies
	for (uint32 i = 0; i < g_activeBonusFireflys.size(); ++i)
	{
		if (i >= g_iBonusFireflyCount)
		{
			g_activeBonusFireflys[i]->SetActive(false);
		}
		else
		{
			g_activeBonusFireflys[i]->SetActive(true);
		}
	}

	// set the # of mines
	for (uint32 i = 0; i < g_activeMines.size(); ++i)
	{
		if (i >= g_iMineCount)
		{
			g_activeMines[i]->SetActive(false);
		}
		else
		{
			g_activeMines[i]->SetActive(true);
		}
	}
	for (uint32 i = 0; i < g_activeFireflies.size(); ++i)
	{
		g_activeFireflies[i]->SetActive(true);
	}
}

void FireflyGameEngine::WriteLevelState()
{
	if (IwGetMultiplayerHandler()->IsMaster())
	{
		FireflyMessage collisionMessage;
		collisionMessage.ev = FFM_LEVEL;
		collisionMessage.dataA = g_iLevel;
		collisionMessage.dataB = g_pHost->IsScore();

		PrepareSend_FireflyMessage(collisionMessage, collisionMessage);
		IwGetMultiplayerHandler()->Send(0, (char*)&collisionMessage, sizeof(FireflyMessage), false);
	}
	else
	{
		// Write our position -- this might just be handled by our parent
		g_pUser->SendStatusUpdate(true);
	}
}

void FireflyGameEngine::WriteGameState()
{
	// When we create our objects, they register for status updates
	// Send out a status update here
	if (IwGetMultiplayerHandler()->IsMaster())
	{
		// Send out a status update
		// we want to send all of our objects
		for (uint32 i = 0; i < g_masterGameObjects.size(); ++i)
		{
			((IGameObject*)g_masterGameObjects[i])->SendStatusUpdate(true);
		}
	}
	else
	{
		g_pPulse->SendStatusUpdate(true);
	}
}

void FireflyGameEngine::ReceiveStatusUpdate(const char * Result, uint32 ResultLen, void* userData)
{
	FireflyGameEngine* pThis = (FireflyGameEngine*)userData;
	FireflyMessage* message = (FireflyMessage*)Result;

	IwAssertMsg("FF", sizeof(FireflyMessage) == ResultLen, ("sizeof(FireflyMessage) != ResultLen (%d - %d)", sizeof(FireflyMessage), ResultLen)); 

	FireflyMessage networkMessage;
	memcpy(&networkMessage, Result, sizeof(FireflyMessage));
	PrepareReceive_FireflyMessage(&networkMessage, &networkMessage);

	switch (networkMessage.ev)
	{
		case FFM_LEVEL :
			{
				// Force the level here
				if (networkMessage.dataA != pThis->g_iLevel)
				{
					pThis->g_iLevel = networkMessage.dataA;
					pThis->SetLevel();
				}

				bool showScore = (networkMessage.dataB != 0);
				if (showScore != pThis->g_pHost->IsScore())
				{
					if (showScore)
					{
						pThis->g_pHost->ShowScore();
					}
					else
					{
						pThis->g_bUnpause = true;
					}
				}
			}
			break;
		case FFM_COLLISION :
			{
				for (uint32 ff = 0; ff < pThis->g_activeFireflies.size(); ++ff)
				{
					if (networkMessage.dataA == pThis->g_activeFireflies[ff]->GetIdentifier())
					{
						if (pThis->g_activeFireflies[ff]->IsActive())
						{
							pThis->g_activeFireflies[ff]->SetActive(false);

							if (networkMessage.dataB == pThis->g_pUser->GetIdentifier())
							{
								pThis->g_activeFireflies[ff]->MakeCapture(pThis->g_locJar);
								int score = pThis->g_pUser->GetScore() + networkMessage.dataC;
								pThis->g_pUser->SetScore(score);
								break;
							}
						}
						break;
					}
				}
				for (uint32 ff = 0; ff < pThis->g_activeBonusFireflys.size(); ++ff)
				{
					if (networkMessage.dataA == pThis->g_activeBonusFireflys[ff]->GetIdentifier())
					{
						if (pThis->g_activeBonusFireflys[ff]->IsActive())
						{
							pThis->g_activeBonusFireflys[ff]->SetActive(false);

							if (networkMessage.dataB == pThis->g_pUser->GetIdentifier())
							{
								pThis->g_activeBonusFireflys[ff]->MakeCapture(pThis->g_locJar);
								int score = pThis->g_pUser->GetScore() + networkMessage.dataC;
								pThis->g_pUser->SetScore(score);
								break;
							}
						}
						break;
					}
				}
				for (uint32 mm = 0; mm < pThis->g_activeMines.size(); ++mm)
				{
					if (networkMessage.dataA == pThis->g_activeMines[mm]->GetIdentifier())
					{
						if (pThis->g_activeMines[mm]->IsActive())
						{
							pThis->g_activeMines[mm]->SetActive(false);
							if (networkMessage.dataB == pThis->g_pUser->GetIdentifier())
							{
								pThis->g_activeMines[mm]->MakeCapture(pThis->g_locJar);
								int score = pThis->g_pUser->GetScore() + networkMessage.dataC;
								pThis->g_pUser->SetScore(score);
								break;
							}
						}

						break;
					}
				}
			}
			break;
		case FFM_LEVEL_OVER :
			{
				// set the level flag and handle in update
				pThis->g_bLevelOver = true;
			}
			break;
		case FFM_GAME_OVER :
			{
				// set the game over flag and handle in update
				pThis->g_bGameOver = true;
			}
			break;
	}
}

bool FireflyGameEngine::Update(uint64 uiGameTimer, bool* pbVictory, bool* pbHasUpdate)
{
	bool hasUpdate = false;
	if (g_bLevelOver)
	{
		// Force the score to the actual value
		g_iDisplayedScore = g_iScore;

		*pbVictory = true;
		return true;
	}
	if (g_bGameOver)
	{
		s3eDeviceYield(0);
	}

	g_pUser->Update(pbVictory);

	if (s3ePointerGetState(S3E_POINTER_BUTTON_RIGHTMOUSE) & S3E_POINTER_STATE_RELEASED)
	{
		if (g_iPulseCount > 0)
		{
			g_iPulseCount--;
			g_pPulse->SetPulse(200, 0.25);
		}
	}

	bool isPulseActive = g_pPulse->IsActive();
	if (g_iPulseCount > 0 && !isPulseActive)
	{
		g_handler.Update();
		float magnitude = g_handler.GetShakeMagnitude();

		if (magnitude != 0)
		{
			g_iPulseCount--;
			g_pPulse->SetPulse((int)(75 * magnitude), 0.25);

			g_handler.Reset();
			s3eVibraVibrate(150, 250);

			hasUpdate = true;
		}
	}

	for (uint32 i = 0; i < g_users.size(); ++i)
	{
		g_users[i]->Update(pbVictory);
	}

	for (uint32 i = 0; i < g_pulses.size(); ++i)
	{
		if (g_pulses[i]->ShouldRender())
		{
			g_pulses[i]->Update(pbVictory);
		}
	}

	// Remove inactive objects
	for (uint32 i = 0; i < g_activeFireflies.size(); ++i)
	{
		if (g_activeFireflies[i]->ShouldRender())
		{
			g_activeFireflies[i]->Update(pbVictory);
		}
	}
	
	if (g_bRenderMine)
	{
		for (uint32 i = 0; i < g_activeMines.size(); ++i)
		{
			if (g_activeMines[i]->ShouldRender())
			{
				g_activeMines[i]->Update(pbVictory);
			}
		}
	}
	if (g_bRenderBonusFirefly)
	{
		for (uint32 i = 0; i < g_activeBonusFireflys.size(); ++i)
		{
			if (g_activeBonusFireflys[i]->ShouldRender())
			{
				g_activeBonusFireflys[i]->Update(pbVictory);
			}
		}
	}

	// All collision detection happens on the master
	//if (IwGetMultiplayerHandler()->IsMaster())
	{
		if (g_bRenderMine)
		{
			for (uint32 i = 0; i < g_activeMines.size(); ++i)
			{
				if (g_activeMines[i]->IsActive())
				{
					for (uint32 uu = 0; uu < g_users.size(); ++uu)
					{
						if (g_users[uu]->InsersectsWith(g_activeMines[i]))
						{
							g_activeMines[i]->SetActive(false);
							g_users[uu]->SetScore(MAX(g_users[uu]->GetScore()-g_iMinePoints, 0));

							// nclark - send a collision if we aren't game master but user owner
							if (IwGetMultiplayerHandler()->IsMaster() || g_users[uu]->IsMaster())
							{
								FireflyMessage collisionMessage;
								collisionMessage.ev = FFM_COLLISION;
								collisionMessage.dataA = g_activeMines[i]->GetIdentifier();
								collisionMessage.dataB = g_users[uu]->GetIdentifier();
								collisionMessage.dataC = g_iMinePoints;

								hasUpdate = true;
								PrepareSend_FireflyMessage(collisionMessage, collisionMessage);
								IwGetMultiplayerHandler()->Send(0, (char*)&collisionMessage, sizeof(FireflyMessage), true);
							}

							// TODO - move this elsewhere
							if (g_users[uu]->IsMaster())
							{
								g_activeMines[i]->MakeCapture(g_locJar);
								// if this is us -- vibrate
								s3eVibraVibrate(255, 500);
							}
							break;
						}
					}
				}
			}
		}

		for (uint32 uu = 0; uu < g_users.size(); ++uu)
		{
			if (g_bRenderBonusFirefly)
			{
				for (uint32 df = 0; df < g_activeBonusFireflys.size(); ++df)
				{
					CBonusFirefly* pBonusFirefly = g_activeBonusFireflys[df];

					if (pBonusFirefly->IsActive() && pBonusFirefly->IsVisible() && pBonusFirefly->InsersectsWith(g_users[uu]))
					{
						pBonusFirefly->SetActive(false);

						uint64 points = (g_iFireflyPoints * 4) - ( (uiGameTimer / 1000) / 5 );

						int score = g_users[uu]->GetScore() + (int)(MIN((g_iFireflyPoints * 4), MAX(0, points)));
						g_users[uu]->SetScore(score);

						if (IwGetMultiplayerHandler()->IsMaster() || g_users[uu]->IsMaster())
						{
							FireflyMessage collisionMessage;
							collisionMessage.ev = FFM_COLLISION;
							collisionMessage.dataA = g_users[uu]->GetIdentifier();
							collisionMessage.dataB = pBonusFirefly->GetIdentifier();
							collisionMessage.dataC = points;

							hasUpdate = true;
							PrepareSend_FireflyMessage(collisionMessage, collisionMessage);
							IwGetMultiplayerHandler()->Send(0, (char*)&collisionMessage, sizeof(FireflyMessage), true);
						}
						if (g_users[uu]->IsMaster())
						{
							pBonusFirefly->MakeCapture(g_locJar);
							s3eVibraVibrate(100, 100);
						}
					}
				}
			}
			for (uint32 ff = 0; ff < g_activeFireflies.size(); ++ff)
			{
				CFirefly* pFirefly = g_activeFireflies[ff];

				if (pFirefly->IsActive() && pFirefly->IsVisible())
				{
					if (pFirefly->InsersectsWith(g_users[uu]))
					{
						pFirefly->SetActive(false);

						// Vary this on time taken
						// Get the total seconds
						uint64 points = g_iFireflyPoints - ( (uiGameTimer / 1000) / 5 );

						int score = g_users[uu]->GetScore() + (int)(MIN(g_iFireflyPoints, MAX(0, points)));
						g_users[uu]->SetScore(score);

						// nclark - send a collision if we aren't game master but user owner
						if (IwGetMultiplayerHandler()->IsMaster() || g_users[uu]->IsMaster())
						{
							FireflyMessage collisionMessage;
							collisionMessage.ev = FFM_COLLISION;
							collisionMessage.dataA = pFirefly->GetIdentifier();
							collisionMessage.dataB = g_users[uu]->GetIdentifier();
							collisionMessage.dataC = points;

							hasUpdate = true;
							PrepareSend_FireflyMessage(collisionMessage, collisionMessage);
							IwGetMultiplayerHandler()->Send(0, (char*)&collisionMessage, sizeof(FireflyMessage), true);
						}

						if (g_users[uu]->IsMaster())
						{
							pFirefly->MakeCapture(g_locJar);
							s3eVibraVibrate(100, 100);
						}
					}
					else
					{
						bool wasHandled = false;

						if (!wasHandled)
						{
							for (uint32 pp = 0; pp< g_pulses.size(); ++pp)
							{
								if (g_pulses[pp]->IsActive() && g_pulses[pp]->InsersectsWith(pFirefly))
								{
									wasHandled = true;
									pFirefly->Stun(g_iStunDuration);
								}
							}
						}
					}
				}
			}
		}
	}
	
	g_iScore = g_pUser->GetScore();

	if (g_iDisplayedScore < g_iScore)
	{
		g_iDisplayedScore = MIN(g_iScore, g_iDisplayedScore + 10);
	}
	else if (g_iDisplayedScore > g_iScore)
	{
		g_iDisplayedScore = MAX(g_iScore, g_iDisplayedScore - 10);
	}

	int activeFireflies = 0;
	for (uint32 ff = 0; ff < g_activeFireflies.size(); ++ff)
	{
		if (g_activeFireflies[ff]->IsActive())
		{
			activeFireflies++;
		}
	}

	// If we are the master and are out of fireflies, end the level
	if (IwGetMultiplayerHandler()->IsMaster() && activeFireflies == 0)
	{
		// nclark - might need this
		// we removed it because we need to end the level *after* we get the score
		//if (IwGetMultiplayerHandler()->IsMultiplayer() && IwGetMultiplayerHandler()->IsMaster())
		//{
		//	FireflyMessage collisionMessage;
		//	collisionMessage.ev = FFM_LEVEL_OVER;

		//	hasUpdate = true;
		//	PrepareSend_FireflyMessage(collisionMessage, collisionMessage);
		//	IwGetMultiplayerHandler()->Send(0, (char*)&collisionMessage, sizeof(FireflyMessage), true);
		//}
		*pbVictory = true;

		// Force the score to the actual value
		g_iDisplayedScore = g_iScore;
		return true;
	}

	if (hasUpdate)
	{
		*pbHasUpdate = true;
	}

	return false;
}

void FireflyGameEngine::RenderStatus(CIwUIRect& bounds)
{
	Iw2DSetColour(0x4F000000);
	Iw2DFillRect(CIwSVec2(bounds.x, bounds.x), CIwSVec2(bounds.w, bounds.h));

	IwGxLightingOn();
	IwGxFontSetFont(g_pFontHuge);
	IwGxFontSetCol(0xffffffff);
	IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE);
	IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT);

	CIwRect rect((int16)(bounds.x+5), (int16)bounds.y, (int16)(bounds.w-5), (int16)bounds.h);
	IwGxFontSetRect(rect);
	sprintf(g_szStatus, "%06d", g_iDisplayedScore);
	IwGxFontDrawText(g_szStatus);
	
	IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE);
	IwGxFontSetFont(g_pFontSmall);
	
	int activeFireflies = 0;
	for (uint32 ff = 0; ff < g_activeFireflies.size(); ++ff)
	{
		if (g_activeFireflies[ff]->IsActive())
		{
			activeFireflies++;
		}
	}

	int ffWidth = g_pTextureFireflyGlow->GetWidth() * Utils::GetImageScalingFactor();
	int ffHeight = g_pTextureFireflyGlow->GetHeight() * Utils::GetImageScalingFactor();

	int bWidth = g_pTextureBolt->GetWidth() * Utils::GetImageScalingFactor();
	int bHeight = g_pTextureBolt->GetHeight() * Utils::GetImageScalingFactor();

	rect.w = ffWidth;
	rect.x = (int16)(bounds.w - ffWidth);
	IwGxFontSetRect(rect);
	sprintf(g_szStatus, "%02d", activeFireflies);
	IwGxFontDrawText(g_szStatus);

	rect.x = (int16)(bounds.w - ffWidth*3);
	IwGxFontSetRect(rect);
	sprintf(g_szStatus, "%02d", g_iPulseCount);
	IwGxFontDrawText(g_szStatus);

	IwGxLightingOff();

	CIwRect locFly((int16)(bounds.w - ffWidth*2), (int16)((bounds.h - ffWidth) / 2), ffWidth, ffHeight);
	Utils::AlphaRenderImage(g_pTextureFireflyGlow, locFly, 255);
	
	g_locJar.x = locFly.x;
	g_locJar.y = locFly.y;

	CIwRect locBolt((int16)(bounds.w - ffWidth*4), (int16)((bounds.h - bHeight) / 2), bWidth, bHeight);
	Utils::AlphaRenderImage(g_pTextureBolt, locBolt, 255);
}

void FireflyGameEngine::RenderGX()
{
	CIwMat viewMat = IwGxGetViewMatrix();
	CIwMat fullMat;
	fullMat.SetIdentity();
	fullMat.t.z = viewMat.t.z;
	IwGxSetViewMatrix(&fullMat);

	for (uint32 i = 0; i < g_activeFireflies.size(); ++i)
	{
		if (g_activeFireflies[i]->ShouldRender())
		{
			g_activeFireflies[i]->Render();
		}
	}

	for (uint32 i = 0; i < g_activeBonusFireflys.size(); ++i)
	{
		if (g_activeBonusFireflys[i]->ShouldRender())
		{
			g_activeBonusFireflys[i]->Render();
		}
	}

	for (uint32 i = 0; i < g_activeMines.size(); ++i)
	{
		if (g_activeMines[i]->ShouldRender())
		{
			g_activeMines[i]->Render();
		}
	}

	IwGxSetViewMatrix(&viewMat);

	for (uint32 i = 0; i < g_users.size(); ++i)
	{
		g_users[i]->Render();
	}
}

void FireflyGameEngine::Render2D()
{
	for (uint32 i = 0; i < g_pulses.size(); ++i)
	{
		if (g_pulses[i]->IsActive())
		{
			g_pulses[i]->Render();
		}
	}
}

void FireflyGameEngine::OnClickResume(CIwUIElement* Clicked)
{
	g_pDialogMain->SetVisible(false);
	g_bUnpause = true;

	// Force a double-read here, so we don't go into pause again right after.
	s3ePointerUpdate();
	s3ePointerUpdate();
}

void FireflyGameEngine::ShowPause(bool bVisible)
{
	g_bUnpause = !bVisible;
	g_pDialogMain->SetVisible(bVisible);
	g_pResume->SetVisible(true);

	ShowScoreAndPause(bVisible);
}

void FireflyGameEngine::ShowScore(bool bVisible)
{
	g_bUnpause = !bVisible;
	g_pDialogMain->SetVisible(bVisible);
	g_pResume->SetVisible(IwGetMultiplayerHandler()->IsMaster());

	ShowScoreAndPause(bVisible);
}

void FireflyGameEngine::ShowScoreAndPause(bool bVisible)
{
	if (bVisible)
	{
		static CIwMultiplayerHandler::User me;
		me.Accepted = true;
		strcpy(me.szDevice, "Me");
		strcpy(me.szName, "Me");
		me.Score = g_iScore;
		me.IsMe = true;

		g_bUnpause = false;
		
		CIwUIElement* pItems = g_pDialogMain->GetChildNamed("MiddleLayer")->GetChild(0)->GetChildNamed("HelpUsers", true, true);
		CIwUIElement* pButtonTemplate = (CIwUIElement*)IwGetResManager()->GetResNamed("UserScoreTemplate", "CIwUIElement");
		
		CIwArray<CIwMultiplayerHandler::User*> pMPUsers = IwGetMultiplayerHandler()->ListUsers();

		for (int i = 0; i < pItems->GetNumChildren(); ++i)
		{
			CIwUIElement* pElem = pItems->GetChild(i);
			pItems->RemoveChild(pElem);

			delete pElem;
			i--;
		}

		CIwArray<CIwMultiplayerHandler::User*> pUsers;
		if (pMPUsers.size() == 0)
		{
			pUsers.append(&me);
		}
		else
		{
			for (uint32 i = 0; i < pMPUsers.size(); ++i)
			{
				pUsers.append(pMPUsers[i]);
			}
		}
		for (uint32 i = 0; i < pUsers.size(); ++i)
		{
			CIwUIElement* pTemplateNew = pButtonTemplate->Clone();
			CIwUIButton* pButtonNew = (CIwUIButton*)pTemplateNew->GetChildNamed("ButtonBase");
			CIwUIImage* pImage = (CIwUIImage*)pButtonNew->GetChildNamed("ButtonTemplate_Image", true, false);
			CIwUILabel* pLabelText = (CIwUILabel*)pButtonNew->GetChildNamed("ButtonTemplate_Text", true, false);
			CIwUILabel* pLabelDistance = (CIwUILabel*)pButtonNew->GetChildNamed("ButtonTemplate_Status", true, false);

			const char* szDevice = pUsers[i]->szDevice;
			const char* szName = pUsers[i]->szName;
			
			pTemplateNew->SetName(szDevice);
			pLabelText->SetCaption(szName);

			char szScore[10];
			sprintf(szScore, "%06d", pUsers[i]->Score);
			pLabelDistance->SetCaption(szScore);

			if (pUsers[i]->IsMe)
			{
				CIwTexture* x = (CIwTexture*)IwGetResManager()->GetResNamed("user", "CIwTexture");
				pImage->SetTexture((CIwTexture*)IwGetResManager()->GetResNamed("user", "CIwTexture"));
			}
			else
			{
				pImage->SetTexture((CIwTexture*)IwGetResManager()->GetResNamed("userB", "CIwTexture"));
			}
			
			pItems->GetLayout()->AddElement(pTemplateNew);
		}
	}
}

bool FireflyGameEngine::RenderPause()
{
	return RenderScoreAndPause(false);
}

bool FireflyGameEngine::RenderScore()
{
	return RenderScoreAndPause(true);
}

bool FireflyGameEngine::RenderScoreAndPause(bool isScore)
{
	IwGetUIController()->Update();
	IwGetUIView()->Update(1000/20);
	
	IwGxSetScreenSpaceSlot(0);
	IwGetUIView()->Render();

	return g_bUnpause;

	/*
	int width = Iw2DGetSurfaceWidth();
	int height = Iw2DGetSurfaceHeight();

	int iconWidth = 40;
	int iconHeight = 40;

	CIwRect bounds(15, height - 170, width - 30, iconHeight);

	IwGxLightingOn();
	IwGxFontSetFont(g_pFont);
	IwGxFontSetCol(0xffffffff);

	CIwRect rect(0, 0, width, bounds.y - g_pTextureLevel->GetHeight());
	IwGxFontSetRect(rect);
	IwGxFontAlignHor alignH = IwGxFontGetAlignmentHor();
	IwGxFontAlignVer alignV = IwGxFontGetAlignmentVer();

	IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE);
	IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE);
	IwGxFontDrawText("Tap the screen to resume...");
	
	if (IwGetMultiplayerHandler()->IsMultiplayer())
	{
		// Show a list of all the users & scores here. Use the help UI perhaps?
		CIwArray<CIwMultiplayerHandler::User*> users = IwGetMultiplayerHandler()->ListUsers();

		for (uint32 i = 0; i < users.size(); ++i)
		{
		}
	}

	CIwTexture* pCurrLevel = g_pTextureLevel1;

	switch (g_iLevel)
	{
		case 0 :
		{
			pCurrLevel = g_pTextureLevel1;
		}
		break;
		case 1 :
		{
			pCurrLevel = g_pTextureLevel2;
		}
		break;
		case 2 :
		{
			pCurrLevel = g_pTextureLevel3;
		}
		break;
		case 3 :
		{
			pCurrLevel = g_pTextureLevel4;
		}
		break;
		case 4 :
		{
			pCurrLevel = g_pTextureLevel5;
		}
		break;
		case 5 :
		{
			pCurrLevel = g_pTextureLevel6;
		}
		break;
		case 6 :
		{
			pCurrLevel = g_pTextureLevel7;
		}
		break;
	}


	int totalWidth = g_pTextureLevel->GetWidth() + pCurrLevel->GetWidth() + 10;
	int totalHeight = g_pTextureLevel->GetHeight();

	CIwSVec2 locLevel((width - totalWidth) / 2, rect.h - totalHeight);
	Utils::AlphaRenderImage(g_pTextureLevel, locLevel, 255);

	locLevel.x += (int16)(g_pTextureLevel->GetWidth() + 10);
	Utils::AlphaRenderImage(pCurrLevel, locLevel, 255);

	Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
	Iw2DFillRect(CIwSVec2(10, height - 170), CIwSVec2(width - 20, 160));
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);

	IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE);
	IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT);
	IwGxFontSetFont(g_pFontSmall);

	CIwRect fontBounds(15 + iconWidth + 10, height - 170, width - 40 - iconWidth, iconHeight);
	CIwSVec2 loc1(bounds.x + (iconWidth - g_pTextureFirefly->GetWidth()) / 2, bounds.y + (iconHeight - g_pTextureFirefly->GetHeight()) / 2);
	Utils::AlphaRenderImage(g_pTextureFirefly, loc1, 255);
	
	IwGxFontSetRect(fontBounds);
	IwGxFontDrawText("Capture firelfies to gain points");

	bounds.y += 40;
	fontBounds.y += 40;
	CIwSVec2 loc2(bounds.x + (iconWidth - g_pTextureBonusFirefly->GetWidth()) / 2, bounds.y + (iconHeight - g_pTextureBonusFirefly->GetHeight()) / 2);
	Utils::AlphaRenderImage(g_pTextureBonusFirefly, loc2, 255);
	
	IwGxFontSetRect(fontBounds);
	IwGxFontDrawText("BonusFireflyflies capture fireflies");

	bounds.y += 40;
	fontBounds.y += 40;
	CIwSVec2 loc3(bounds.x + (iconWidth - g_pTextureMine->GetWidth()) / 2, bounds.y + (iconHeight - g_pTextureMine->GetHeight()) / 2);
	Utils::AlphaRenderImage(g_pTextureMine, loc3, 255);
	
	IwGxFontSetRect(fontBounds);
	IwGxFontDrawText("Mines take away your points");

	bounds.y += 40;
	fontBounds.y += 40;
	CIwSVec2 loc4(bounds.x + (iconWidth - g_pTextureBolt->GetWidth()) / 2, bounds.y + (iconHeight - g_pTextureBolt->GetHeight()) / 2);
	Utils::AlphaRenderImage(g_pTextureBolt, loc4, 255);
	
	IwGxFontSetRect(fontBounds);
	IwGxFontDrawText("Shake your phone to stun fireflies");

	IwGxLightingOff();
	*/
}