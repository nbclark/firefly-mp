#include "ActiveGameState.h"
#include "MessageBox.h"
#include "Constants.h"

ActiveGameState::ActiveGameState(void)
{
	CIwImage img3, img4, img5;

	img3.LoadFromFile("3.png");
	g3 = Iw2DCreateImage(img3);
	img4.LoadFromFile("2.png");
	g2 = Iw2DCreateImage(img4);
	img5.LoadFromFile("1.png");
	g1 = Iw2DCreateImage(img5);

	g_iAnimationCount = 0;
	g_cursorIter = 0;
	g_iStartGameCount = 0;

	IW_UI_CREATE_VIEW_SLOT1(this, "ActiveGameState", ActiveGameState, OnClickBack, CIwUIElement*)

	g_pDialogMain = (CIwUIElement*)IwGetResManager()->GetResNamed("active\\panel", "CIwUIElement");
	g_pDialogMain->SetVisible(false);
	g_pTitleBar = g_pDialogMain->GetChildNamed("TitleLabel", true);

	IwGetUIView()->AddElement(g_pDialogMain);
	IwGetUIView()->AddElementToLayout(g_pDialogMain);

	g_bIsScore = false;
	g_bIsPaused = false;
	g_pFont = (CIwGxFont*)IwGetResManager()->GetResNamed("font_medium", "CIwGxFont");
	g_dAlpha = 100;
	g_bGameOver = false;
	g_pGameEngine = NULL;

	IwGetMultiplayerHandler()->RegisterModeChangedCallback(MultiplayerModeChanged, this);
	IwGetMultiplayerHandler()->RegisterDisconnectCallback(MultiplayerDisconnect, this);
}

ActiveGameState::~ActiveGameState(void)
{
	delete g1;
	delete g2;
	delete g3;
	delete g_pFont;

	IW_UI_DESTROY_VIEW_SLOTS(this);
}

void ActiveGameState::MultiplayerDisconnect(bool success, const char* szStatus, void* userData)
{
	ActiveGameState* pThis = (ActiveGameState*)userData;

	pThis->HidePause();
	pThis->HideScore();

	MessageBox::Show("Disconnected", (char*)szStatus, "OK", "Cancel", NULL, userData);
	pThis->g_pMapGame->SetGameState(GPS_GameState_SelectGame, AnimDir_Right);
}

void ActiveGameState::MultiplayerModeChanged(MultiplayerMode mode, void* userData)
{
	if (mode == MPM_IN_GAME)
	{
		// We have started our game
		// Send our state & transition
		ActiveGameState* pThis = (ActiveGameState*)userData;
		//pThis->g_pMapGame->SetGameState(GPS_GameState_Active, AnimDir_Right);
	}
}


void ActiveGameState::OnClickBack(CIwUIElement* Clicked)
{
	bool result = MessageBox::Show((char*)"Quit Game", (char*)"Are you sure you want to quit?", (char*)"Yes", (char*)"No", GameState::MessageRenderBackground, this);

	if (result)
	{
		g_pGameEngine->ShowPause(false);
		g_pGameEngine->ShowScore(false);
		IwGetMultiplayerHandler()->EndGame();
		g_pMapGame->SetGameState(GPS_GameState_Intro, AnimDir_Left);
	}
}

void ActiveGameState::PerformActivate()
{
	g_bGameOver = false;
	g_pGameEngine = g_pMapGame->GetGameEngine();
	g_pGameEngine->SetHost(this);

	g_bRenderGame = false;

	g_bMouseDown = false;
	g_iStartGameCount = 0;

	float zoom;
	std::list<s3eLocation>* pCorners = g_pMapGame->GetScaledCorners(&zoom);
	g_pMapGame->GetBackground()->ClearScaledCorners();

	if (pCorners->size() == 4)
	{
		s3eLocation topLeft, topRight, bottomLeft, bottomRight;

		std::list<s3eLocation>::iterator iter = pCorners->begin();

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
			iter++;
			i++;
		}

		g_pMapGame->GetBackground()->SetScaledCorners(topLeft, topRight, bottomLeft, bottomRight, zoom);
	}
	// Update the tiles we need
	g_pMapGame->GetBackground()->Update(true);
	g_pMapGame->GetBackground()->DownloadTiles();
	g_pMapGame->SetActiveUI(g_pDialogMain, NULL);

	g_bIsScore = false;
	g_bIsPaused = false;
	g_pMapGame->StartTrackingDistance();

	g_mpLastFlush = 0;

	CIwRect gameBounds(0, g_pTitleBar->GetFrame().h, Iw2DGetSurfaceWidth(), 0);
	gameBounds.h = Iw2DGetSurfaceHeight() - gameBounds.y;

	g_pGameEngine->Activate(gameBounds);
	g_pGameEngine->WriteLevelState();
	g_pGameEngine->WriteGameState();
	IwGetMultiplayerHandler()->Flush();

	g_mpLastFlush = s3eTimerGetMs();
}

void ActiveGameState::PerformDeActivate()
{
	g_pGameEngine->ShowPause(false);
	g_pGameEngine->ShowScore(false);

	g_pMapGame->StopTrackingDistance();
	g_pGameEngine->DeActivate();
	
	g_pGameEngine = NULL;
}

bool g_bRenderUI = false;

void ActiveGameState::PerformUpdate()
{
	s3eDeviceBacklightOn();
	// Update the background
	bool hasUpdate = false;
	bool renderUI = false;
	bool backClicked = false;
	bool becameScore = false;
	bool willFlush = false;
	bool shouldPause = s3eDeviceCheckPauseRequest();

	g_pMapGame->GetBackground()->Update(false);
	bool isLoading = !(g_iStartGameCount > 60 && !g_pMapGame->GetBackground()->IsAnimating());

	if (!isLoading && !g_bRenderGame)
	{
		g_uiStartTime = s3eTimerGetMs();
		g_bRenderGame = true;
	}
	
	int pulseInterval = (IwGetMultiplayerHandler()->IsMaster()) ? 500 : 500;

	if (IwGetMultiplayerHandler()->InSocketMode())
	{
		// TODO: increase interval if we are tilting
		pulseInterval = (IwGetMultiplayerHandler()->IsMaster()) ? 350 : 200;

		if (!becameScore && g_bIsScore)
		{
			pulseInterval = 2500;
		}
	}

	uint64 timer = s3eTimerGetMs();
	willFlush = ((timer - g_mpLastFlush) > pulseInterval);

	if (isLoading)
	{
		// show the 1 2 3 here
		g_iStartGameCount++;
	}
	else if (shouldPause)
	{
		g_bIsPaused = true;
	}
	else if (g_bIsPaused)
	{
		g_dAlpha = 75;
		
		CIwUIRect rect = g_pTitleBar->GetFrame();
		if ((s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED) && (s3ePointerGetY() < rect.h))
		{
			backClicked = true;
		}
		else if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED)
		{
			if (g_pGameEngine->ClickToUnpause())
			{
				g_pGameEngine->ShowPause(false);
				g_bIsPaused = false;
			}
		}

		if (willFlush)
		{
			g_pGameEngine->WriteGameState();
		}
	}
	else if (g_bIsScore)
	{
		g_dAlpha = 75;

		CIwUIRect rect = g_pTitleBar->GetFrame();
		if ((s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED) && (s3ePointerGetY() < rect.h))
		{
			backClicked = true;
		}
		else if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED)
		{
			if (g_pGameEngine->ClickToUnpause())
			{
				g_pGameEngine->ShowScore(false);
				g_bIsScore = false;
				hasUpdate = true;
			}
		}

		if (willFlush)
		{
			g_pGameEngine->WriteGameState();
		}
	}
	else if (g_bGameOver)
	{
		g_pGameEngine->ShowScore(false);
		this->GetGameHandler()->SetGameState(GPS_GameState_Intro, AnimDir_Left);
	}
	else
	{
		CIwUIRect rect = g_pTitleBar->GetFrame();
		g_dAlpha = 150;
		if ((s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED) && (s3ePointerGetY() < rect.h))
		{
			backClicked = true;
		}
		else if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED)
		{
			g_pGameEngine->ShowPause(true);
			g_bIsPaused = true;
		}
		else
		{
			renderUI = false;
			bool bVictory = false;

			uint64 elapsedTime = (s3eTimerGetMs() - g_uiStartTime);
			if (g_pGameEngine->Update(elapsedTime, &bVictory, &hasUpdate))
			{
				hasUpdate = true;
				g_dAlpha = 25;
				g_pGameEngine->ShowScore(true);
				g_bIsScore = true;
				becameScore = true;

				if (!bVictory || g_pGameEngine->UpdateLevel())
				{
					// Force a state flush
					g_mpLastFlush = -1;

					int score = g_pGameEngine->GetScore();
					// Add the high score here
					g_pMapGame->AddHighScore(score, elapsedTime, g_pMapGame->GetDistanceTravelled());

					if (IwGetMultiplayerHandler()->IsMultiplayer())
					{
						g_bGameOver = true;
						IwGetMultiplayerHandler()->EndGame();
					}
					else
					{
						// Game over
						bool result = MessageBox::Show((char*)"Game Over", (char*)"Play Again?", (char*)"Yes", (char*)"No", GameState::MessageRenderBackground, this);
						g_pGameEngine->ShowScore(false);

						if (result)
						{
							this->GetGameHandler()->SetGameState(GPS_GameState_Active, AnimDir_Initial);
						}
						else
						{
							this->GetGameHandler()->SetGameState(GPS_GameState_HighScore, AnimDir_Left);
						}
					}
				}
			}

			if (willFlush)
			{
				g_pGameEngine->WriteGameState();
			}
		}
	}

	// Always write level state
	if (willFlush || hasUpdate)
	{
		g_mpLastFlush = timer;
		g_pGameEngine->WriteLevelState();
		IwGetMultiplayerHandler()->Flush();
	}

	g_bRenderUI = renderUI;
	if (renderUI)
	{
		IwGetUIController()->Update();
		IwGetUIView()->Update(1000/FRAMES_PER_S);
	}
	if (backClicked)
	{
		OnClickBack(NULL);
	}
}

void ActiveGameState::PerformRender()
{
	//s3eDebugTraceLine("Active:Render");
	IwGxClear(IW_GX_DEPTH_BUFFER_F);
	IwGxSetScreenSpaceSlot(-1);
	RenderBackground();

	// Render 2-d stuff here
	if (!g_bRenderGame)
	{
		CIw2DImage* img;

		if (g_iStartGameCount < 20)
		{
			img = g3;
		}
		else if (g_iStartGameCount < 40)
		{
			img = g2;
		}
		else
		{
			img = g1;
		}
		CIwSVec2 pos;
		pos.x = (int16)((Iw2DGetSurfaceWidth() - img->GetWidth()) / 2);
		pos.y = (int16)((Iw2DGetSurfaceHeight() - img->GetHeight()) / 2);

		Iw2DSetColour(0xFFFFFFFF);
		IwGxSetColClear(0, 0, 0, 0);
		Iw2DDrawImage(img, pos);
	}
	else if (g_bIsScore)
	{
		IwGxFlush();
		IwGxClear(IW_GX_DEPTH_BUFFER_F);
		IwGxSetScreenSpaceSlot(0);

		if (g_pGameEngine->RenderScore())
		{
			g_pGameEngine->ShowScore(false);
			g_bIsScore = false;
		}
		CIwUIRect rect = g_pTitleBar->GetFrame();
  		g_pGameEngine->RenderStatus(rect);
	}
	else if (g_bIsPaused)
	{
		IwGxFlush();
		IwGxClear(IW_GX_DEPTH_BUFFER_F);
		IwGxSetScreenSpaceSlot(0);

		if (g_pGameEngine->RenderPause())
		{
			g_pGameEngine->ShowPause(false);
			g_bIsPaused = false;
		}
		CIwUIRect rect = g_pTitleBar->GetFrame();
  		g_pGameEngine->RenderStatus(rect);
	}
	else
	{
		IwGxSetScreenSpaceSlot(0);
		g_pGameEngine->RenderGX();

		if (g_bRenderUI)
		{
			IwGxFlush();
			IwGxClear(IW_GX_DEPTH_BUFFER_F);

			IwGxSetScreenSpaceSlot(0);
			IwGetUIView()->Render();
		}
		CIwUIRect rect = g_pTitleBar->GetFrame();
  		g_pGameEngine->RenderStatus(rect);

		IwGxFlush();

		g_pGameEngine->Render2D();
	}
}

void ActiveGameState::RenderBackground()
{
	g_pMapGame->GetBackground()->SetAlpha(g_dAlpha);
	g_pMapGame->GetBackground()->Render();
}
