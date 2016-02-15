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
// Examples main header
//-----------------------------------------------------------------------------

#ifndef EXAMPLES_MAIN_H
#define EXAMPLES_MAIN_H
#include "IGameEngine.h"

// Attempt to lock to 30 frames per second
#define FRAMES_PER_S 30
#define	MS_PER_FRAME (1000 / FRAMES_PER_S)

//#define GAME_TRIALMODE
#define GAME_URL	"http://www.mobilesrc.com"

#ifdef GAME_TRIALMODE
#define GAME_VERSION	2.0
#define GAME_ID			10	// firefly
#endif

#ifndef GAME_TRIALMODE
#define GAME_VERSION	2.0
#define GAME_ID			1	// firefly
#endif

// This mode is for the samsung bada OS as it struggles to keep up with FPS
//#define HIGH_FPS_MODE

//#define VIDEOMODE

int GameMain(IGameEngine* pGameEngine);

#endif /* !EXAMPLES_MAIN_H */
