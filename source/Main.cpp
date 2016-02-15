/*
 * This file is part of the Airplay SDK Code Samples.
 *
 * Copyright (C) 2001-2009 Ideaworks Labs.
 * All Rights Reserved.
 *
 * This source code is intended only as a supplement to Ideaworks Labs
 * Development Tools and/or on-line documentation.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */
// Examples main file
//-----------------------------------------------------------------------------

#include "s3e.h"
#include "s3esocket.h"
#include "IwDebug.h"
#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwTexture.h"
#include "IwMaterial.h"
#include "IwHTTPQueue.h"
#include "IwMultiplayerHandler.h"
#include "IwNotificationHandler.h"
#include "MapGame.h"
#include "AccelerometerHandler.h"
#include "Main.h"

CIwMat		 s_ViewMatrix;
int downloadCount = 0;
uint64 firstGet = 0;
bool inAction = false;
uint64 starttimer = 0;

void blah(void * Argument, const char* szContentType, const char * Result, uint32 ResultLen)
{
	if (!firstGet)
	{
		starttimer = firstGet = s3eTimerGetMs();
	}
	downloadCount++;
	inAction = false;
}
void blahError(void * Argument)
{
	s3eDeviceYield(0);
	inAction = false;
}

static int32 func(s3eSocket* g_Sock, void* sys, void* data)
{
    s3eResult res = *(s3eResult*)sys;

	if (res == S3E_RESULT_SUCCESS)
	{
		char szBuffer[100];
		strcpy(szBuffer, "mobilesrc");
		int16 size = 30; 

		memcpy((void*)&szBuffer[9], &size, sizeof(int16));

		for (int i = 0; i < 30; ++i)
		{
			szBuffer[11+i] = i;
		}

		int32 count = s3eSocketSend(g_Sock, szBuffer, 41, 0);
		s3eDeviceYield(0);
	}

    return 0;
}


//-----------------------------------------------------------------------------
// Main global function
//-----------------------------------------------------------------------------
int GameMain(IGameEngine* pGameEngine)
{
	//s3eSocket* sa = s3eSocketCreate(S3E_SOCKET_TCP, 0);

	//s3eInetAddress addr;
	//memset(&addr, 0, sizeof(addr));
	//s3eInetLookup("localhost", &addr, NULL, NULL);
	//addr.m_Port = s3eInetHtons(81);

	//s3eResult x = s3eSocketConnect(sa, &addr, func, NULL);
	//s3eSocketErrors err = s3eSocketGetError();

	//while (true)
	//{
	//	s3eDeviceYield(100);
	//}

	//DisplayMessage("testing");
	s3eFileMakeDirectory("tiles");
	s3eFileMakeDirectory("maps");
	s3eFileMakeDirectory("scores");

	int32 a = s3eMemoryGetInt(S3E_MEMORY_USED);
	IwGetResManager()->LoadGroup(Utils::ResourceFile);
	int32 b = s3eMemoryGetInt(S3E_MEMORY_USED);

	int minRes = MIN(Iw2DGetSurfaceWidth(), Iw2DGetSurfaceHeight());

	CIwResource* pResource;
	if (minRes < 320)
	{
		pResource = IwGetResManager()->GetResNamed("tiny", IW_UI_RESTYPE_STYLESHEET);
	}
	else if (minRes < 480)
	{
		pResource = IwGetResManager()->GetResNamed("small", IW_UI_RESTYPE_STYLESHEET);
		//pResource = IwGetResManager()->GetResNamed("medium", IW_UI_RESTYPE_STYLESHEET);
	}
	else
	{
		//pResource = IwGetResManager()->GetResNamed("small", IW_UI_RESTYPE_STYLESHEET);
		pResource = IwGetResManager()->GetResNamed("large", IW_UI_RESTYPE_STYLESHEET);
	}
	CIwUIStylesheet* pStyle = IwSafeCast<CIwUIStylesheet*>(pResource);
	IwGetUIStyleManager()->SetStylesheet(pStyle);

	IwDebugTraceLinePrintf("Creating Notification Object");
	CIwNotificationHandler* pNotHandler = new CIwNotificationHandler;

	IwDebugTraceLinePrintf("Creating Multiplayer Object");
	CIwMultiplayerHandler* pMPHandler = new CIwMultiplayerHandler;
	
	IwDebugTraceLinePrintf("Creating HTTP Queue Object");
	CIwHTTPQueue* pDownloader = new CIwHTTPQueue;
	
	IwDebugTraceLinePrintf("Creating MapGame Object");
	CIwMapGame* pBackground = new CIwMapGame;

	pMPHandler->SetGameHandler(pBackground);
	
	// Example main loop
	IwDebugTraceLinePrintf("Initializing MapGame");
	pBackground->Init();
	// Set screen clear colour
	IwGxSetColClear(0xff, 0xff, 0xff, 0xff);
	IwGxPrintSetColour(128, 128, 128);

	// Initialize the game
	IwDebugTraceLinePrintf("Initializing Game Engine");
	pGameEngine->Init(pBackground);
	pBackground->SetGameEngine(pGameEngine);

	// Set field of view
	IwGxSetPerspMul(0x200);

	// TODO - seed this in multiplayer games
	IwRandSeed((int32)s3eTimerGetMs());
	
	s_ViewMatrix.SetIdentity();
	s_ViewMatrix.t.x = 0;
	s_ViewMatrix.t.y = 0;
	s_ViewMatrix.t.z = -0x200;

	IwGxSetViewMatrix(&s_ViewMatrix);

	uint64 startA = s3eTimerGetMs();
	int id = 1;
	float msPF = 1000.0f / Utils::FPS;
	while (1)
	{
		s3eDeviceYield(0);
		s3eKeyboardUpdate();
		s3ePointerUpdate();

		int64 start = s3eTimerGetMs();

		IwGetHTTPQueue()->Update();
		IwGetMultiplayerHandler()->Update();
		IwGetNotificationHandler()->Update();

		bool result = true;
		result = pBackground->Update();
		if	(
			(result == false) ||
			(s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN) ||
			(s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN) ||
			(s3eDeviceCheckQuitRequest())
			)
		{
			break;
		}

		// Clear the screen
		
		IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
		pBackground->Render();
		IwGetNotificationHandler()->Render();

		//IwGxPrintFrameRate(0, 0);
		IwGxFlush();
		IwGxSwapBuffers();

		// Attempt frame rate
		while ((s3eTimerGetMs() - start) < msPF)
		{
			int32 yield = (int32) (msPF - (s3eTimerGetMs() - start));
			if (yield<0)
			{
				break;
			}
			s3eDeviceYield(yield);
		}
		s3eDebugTraceFlush();
	}
	pBackground->ShutDown();
	IwGetUIAnimManager()->StopAllAnims();

	delete pBackground;
	delete pDownloader;
	delete pMPHandler;
	delete pNotHandler;

	return 0;
}
