#include "CreateMapGameState.h"
#include "tinyxml.h"
#include "MessageBox.h"
#include "TextEntryBox.h"

CreateMapGameState::CreateMapGameState(void)
{
	IW_UI_CREATE_VIEW_SLOT1(this, "CreateMapGameState", CreateMapGameState, OnClickBack, CIwUIElement*)

	g_pDialogMain = (CIwUIElement*)IwGetResManager()->GetResNamed("createmap\\panel", "CIwUIElement");
	g_pDialogMain->SetVisible(false);
	IwGetUIView()->AddElement(g_pDialogMain);
	IwGetUIView()->AddElementToLayout(g_pDialogMain);

	CIwImage img, img2;
	img.LoadFromFile("cursor.png");
	gCursor = Iw2DCreateImage(img);

	img2.LoadFromFile("mark.png");
	gMark = Iw2DCreateImage(img2);

	g_location.x = g_location.y = 0;
}

CreateMapGameState::~CreateMapGameState(void)
{
	delete gCursor;
	delete gMark;
}

void CreateMapGameState::OnClickBack(CIwUIElement* Clicked)
{
	g_downPos.clear();
	g_pMapGame->SetGameState(GPS_GameState_SelectMap, AnimDir_Left);
}

void CreateMapGameState::PerformUpdate()
{
	// Allow for scrolling
	bool mouseClick = false;
	CIwSVec2 clickPos;

	IwGetUIController()->Update();
	IwGetUIView()->Update(1000/20);

	if (g_tickCount == 1)
	{
		g_location = g_pMapGame->GetPosition();
		g_tickCount = 0;
	}

	if (!g_pMapGame->GetBackground()->IsScaled())
	{
		g_pMapGame->GetBackground()->UpdatePositionFromMouseAndKeyboard(clickPos, &mouseClick);

		if (mouseClick)
		{
			// we had a click
			s3eLocation location;
			g_pMapGame->GetBackground()->GetScaler()->PositionToLocation(clickPos, &location);

			g_downPos.push_back(location);

			if (g_downPos.size() == 4)
			{
				std::list<s3eLocation>::iterator iter = g_downPos.begin();
				s3eLocation* topLeft, *topRight, *bottomLeft, *bottomRight;

				topLeft = &(*iter);
				iter++;
				topRight = &(*iter);
				iter++;
				bottomLeft = &(*iter);
				iter++;
				bottomRight = &(*iter);

				float zoom = g_pMapGame->GetBackground()->GetZoom();
				g_pMapGame->GetBackground()->SetScaledCorners(*topLeft, *topRight, *bottomLeft, *bottomRight, zoom);
			}
		}
	}
	else
	{
		if (g_downPos.size() == 4 && !g_pMapGame->GetBackground()->IsAnimating())
		{
			bool result = MessageBox::Show((char*)"Check Field", (char*)"Is the field above displayed correctly?", (char*)"Yes", (char*)"No", GameState::MessageRenderBackground, this);
			g_downPos.clear();
			
			if (result)
			{
				TextEntryBox::Show((char*)"Save Field", (char*)"Please enter a tile for this field:", (char*)"Save Changes", GameState::MessageRenderBackground, CreateMapGameState::TextEntryClosed, this);
			}
			else
			{
				this->g_pMapGame->GetBackground()->ClearScaledCorners();
			}
		}
		else
		{
			g_pMapGame->GetBackground()->Update(false);
		}
	}
}

void CreateMapGameState::TextEntryClosed(void * pParam, const char* szText)
{
	int x = 0;
	CreateMapGameState* pGame = (CreateMapGameState*)pParam;
	// Prompt here for saving the thing

	char szPath[200];
	strcpy(szPath, "maps/");
	strcat(szPath, szText);
	strcat(szPath, ".map");

	double zoom = pGame->g_pMapGame->GetBackground()->GetScaler()->GetZoom();

	TiXmlDocument doc;

	TiXmlElement pKmlNode("kml");
	TiXmlElement pDocumentNode("Document");
	
	GPSRectangle scaledCorners = pGame->g_pMapGame->GetBackground()->GetScaler()->GetScaledCorners();

	for (int i = 0; i < 4; ++i)
	{
		s3eLocation location;

		switch (i)
		{
			case 0 :
			{
				location = scaledCorners.topLeft;
			}
			break;
			case 1 :
			{
				location = scaledCorners.topRight;
			}
			break;
			case 2 :
			{
				location = scaledCorners.bottomLeft;
			}
			break;
			case 3 :
			{
				location = scaledCorners.bottomRight;
			}
			break;
		}

		TiXmlElement pPlacemarkNode("Placemark");
		TiXmlElement pPointNode("Point");
		TiXmlElement pCoordinatesNode("coordinates");
		
		char szValue[200];
		sprintf(szValue, "%f,%f", location.m_Latitude, location.m_Longitude);

		TiXmlText pValueNode(szValue);

		pCoordinatesNode.InsertEndChild(pValueNode);
		pPointNode.InsertEndChild(pCoordinatesNode);
		pPlacemarkNode.InsertEndChild(pPointNode);
		pDocumentNode.InsertEndChild(pPlacemarkNode);
	}

	char szZoom[20];
	sprintf(szZoom, "%f", zoom);
	pKmlNode.SetAttribute("zoom", szZoom);
	pKmlNode.InsertEndChild(pDocumentNode);
	doc.InsertEndChild(pKmlNode);
	doc.SaveFile(szPath);

	pGame->g_pMapGame->GetBackground()->ClearScaledCorners();
	pGame->g_pMapGame->SetGameState(GPS_GameState_SelectMap, AnimDir_Left);
}

void CreateMapGameState::PerformRender()
{
	//s3eDebugTraceClearFile();
	////s3eDebugTraceLine("Render");
	IwGxSetScreenSpaceSlot(-1);
	RenderBackground();
	IwGxFlush();
	IwGxClear(IW_GX_DEPTH_BUFFER_F);
	IwGxSetScreenSpaceSlot(0);
	IwGetUIView()->Render();
	RenderClicks();
	RenderCursor();
}

void CreateMapGameState::RenderCursor()
{
	////s3eDebugTraceLine("RenderCursor");
	g_cursorIter++;
	////s3eDebugTraceLine("1");
	CIwSVec2 topLeft;
	CIwVec2 topLeft1;

	double rotation = 0;

	if (g_cursorIter >= FRAMES_PER_S * 5.0)
	{
		g_cursorIter = 0;
		rotation = 0;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 3.5)
	{
		rotation = 315;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 3.0)
	{
		rotation = 270;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 2.5)
	{
		rotation = 225;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 2.0)
	{
		rotation = 180;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 1.5)
	{
		rotation = 135;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 1.0)
	{
		rotation = 90;
	}
	else if (g_cursorIter >= FRAMES_PER_S * 0.5)
	{
		rotation = 45;
	}
	else
	{
		rotation = 0;
	}

	rotation = ((g_cursorIter/5) % 10) * 36;
	
	CIwMat2D rot;
	CIwVec2 vec2;
	CIwSVec2 pos;

	int imageSize = gCursor->GetWidth() / 2;
	int width = Iw2DGetSurfaceWidth();
	int height = Iw2DGetSurfaceHeight();

	pos.x = g_location.x - imageSize;
	pos.y = g_location.y - imageSize;

	vec2.x = g_location.x;
	vec2.y = g_location.y;

	if (!(vec2.x < -imageSize || vec2.y < -imageSize || vec2.x > (imageSize+width) || vec2.y > (imageSize+height)))
	{
		rot.SetRot((iwangle)(IW_GEOM_ONE * rotation / 360), vec2);
		Iw2DSetTransformMatrix(rot);
		Iw2DDrawImage(gCursor, pos);
		Iw2DSetTransformMatrix(CIwMat2D::g_Identity);
	}
	
	g_cursorIter++;
}

void CreateMapGameState::RenderBackground()
{
	////s3eDebugTraceLine("RenderBackground");
	g_pMapGame->GetBackground()->Render();
}

void CreateMapGameState::RenderClicks()
{
	////s3eDebugTraceLine("RenderClicks");
	std::list<s3eLocation>::iterator downiter = g_downPos.begin();
	CIwSVec2 vec;
	CIwVec2 vec2;
	s3eLocation location;

	CIwMat2D rot;

	int size = gMark->GetWidth() / 2;
	int width = Iw2DGetSurfaceWidth();
	int height = Iw2DGetSurfaceHeight();

	while (downiter != g_downPos.end())
	{
		location = *downiter;
		g_pMapGame->GetBackground()->GetScaler()->LocationToPosition(location, &vec);

		vec -= CIwSVec2(size, size);

		vec2.x = vec.x + size;
		vec2.y = vec.y + size;
		
		if (!(vec2.x < -size || vec2.y < -size || vec2.x > (size+width) || vec2.y > (size+height)))
		{
			rot.SetRot((iwangle)(IW_GEOM_ONE * 45.0 / 360), vec2);
			Iw2DSetTransformMatrix(rot);
			Iw2DDrawImage(gMark, vec);
		}

		downiter++;
	}
	Iw2DSetTransformMatrix(CIwMat2D::g_Identity);
}

void CreateMapGameState::PerformActivate()
{
	////s3eDebugTraceLine("Activate");
	g_cursorIter = 0;
	g_downPos.clear();
	
	s3eLocation loc;
	
	if (g_pMapGame->GetLocation(loc))
	{
		g_pMapGame->GetBackground()->SetLocation(loc.m_Longitude, loc.m_Latitude);
	}
	g_pMapGame->GetBackground()->ClearScaledCorners();
	g_pMapGame->SetActiveUI(g_pDialogMain, NULL);
}

void CreateMapGameState::PerformDeActivate()
{
}