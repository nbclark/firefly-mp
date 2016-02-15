#include "Utils.h"
#include "tinyxml.h"

char g_szMapData[1000];
CIwGxFont* Utils::g_pFont = 0;
CIwGxFont* Utils::g_pFontLarge = 0;
float Utils::g_fScale = 0;
float Utils::g_fTScale = 0;

double Utils::ScaleWidth = 640.0;
double Utils::ScaleHeight = 960.0;
int Utils::SidePadding = 64;
int Utils::BottomPadding = 100;
int Utils::FPS = 30;
char Utils::ResourceFile[100] = { "GpsGameUI.group" };

char* Utils::GetMapData(const char* szMap)
{
	s3eFile* pFile = s3eFileOpen(szMap, "r");

	if (pFile)
	{
		uint32 gResultLen = s3eFileGetSize(pFile);

		s3eFileRead(g_szMapData, sizeof(char), gResultLen, pFile);

		s3eFileClose(pFile);
		return &g_szMapData[0];
	}

	return NULL;
}

void Utils::SaveMapData(const char* szMap, const char* szData)
{
	s3eFile* pFile = s3eFileOpen(szMap, "w");
	s3eFileWrite(szData, sizeof(char), strlen(szData)+1, pFile);
	s3eFileClose(pFile);
}

bool Utils::LoadMap(const char* szData, std::list<s3eLocation>* pCorners, float* pZoom)
{
	pCorners->clear();

	TiXmlDocument doc;
	doc.Parse(szData);

	TiXmlElement* pRootNode = doc.RootElement();
	TiXmlNode* pDocumentNode;
	TiXmlNode* pPlacemarkNode;
	TiXmlNode* pPointNode;
	TiXmlNode* pCoordinateNode;

	if (pRootNode && pRootNode->ToElement())
	{
		pDocumentNode = pRootNode->FirstChild("Document");

		if (pDocumentNode && pDocumentNode->ToElement())
		{
			if (pZoom)
			{
				TiXmlAttribute* pAttribute = pRootNode->FirstAttribute();
				if (pAttribute)
				{
					*pZoom = (float)pAttribute->DoubleValue();
				}
				else
				{
					*pZoom = 19;
				}
			}
			for (pPlacemarkNode = pDocumentNode->FirstChild("Placemark"); pPlacemarkNode; pPlacemarkNode = pPlacemarkNode->NextSibling("Placemark"))
			{
				pPointNode = pPlacemarkNode->FirstChild("Point");

				if (pPointNode)
				{
					pCoordinateNode = pPointNode->FirstChild("coordinates");

					if (pCoordinateNode)
					{
						const char* szData = pCoordinateNode->FirstChild()->Value();

						if (strstr(szData, ","))
						{
							// we have coordinates
							float latitude, longitude;
							if (2 == sscanf(szData, "%f,%f", &latitude, &longitude))
							{
								// this one is good
								s3eLocation location;
								location.m_Latitude = latitude;
								location.m_Longitude = longitude;

								pCorners->push_back(location);
							}
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}

void Utils::AlphaRenderImage(CIwTexture* pTexture, CIwSVec2& location, double alpha)
{
	int width = pTexture->GetWidth();
	int height = pTexture->GetHeight();

	CIwRect rect(location.x, location.y, width, height);
	Utils::AlphaRenderImage(pTexture, rect, alpha);
}

void Utils::AlphaRenderImage(CIwTexture* pTexture, CIwRect& bounds, double alpha)
{
	int xOffset = IwGxGetScreenWidth() / 2;
	int yOffset = IwGxGetScreenHeight() / 2;

	int width = bounds.w << 3;
	int height = bounds.h << 3;
	
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);

	// Use Texture on Material
	pMat->SetTexture(pTexture);
	IwGxSetMaterial(pMat);

	int wo2 = bounds.x << 3;
	int ho2 = bounds.y << 3;

	CIwSVec3* pWSCoords= IW_GX_ALLOC(CIwSVec3, 4);
	pWSCoords[0].x = wo2; pWSCoords[0].y = ho2;
	pWSCoords[1].x = wo2; pWSCoords[1].y = ho2 + height;
	pWSCoords[2].x = wo2 + width; pWSCoords[2].y = ho2 + height;
	pWSCoords[3].x = wo2 + width; pWSCoords[3].y = ho2;
	pWSCoords[0].z = pWSCoords[1].z = pWSCoords[2].z = pWSCoords[3].z = 0;

	CIwSVec2* pWSCoords2= IW_GX_ALLOC(CIwSVec2, 4);
	pWSCoords2[0].x = wo2; pWSCoords2[0].y = ho2;
	pWSCoords2[1].x = wo2; pWSCoords2[1].y = ho2 + height;
	pWSCoords2[2].x = wo2 + width; pWSCoords2[2].y = ho2 + height;
	pWSCoords2[3].x = wo2 + width; pWSCoords2[3].y = ho2;

	IwGxSetVertStreamScreenSpaceSubPixel(pWSCoords2, 4);
	//IwGxSetVertStreamWorldSpace(pWSCoords, 4);

	static CIwSVec2 uvs[4] =
	{
		CIwSVec2(0 << 12, 0 << 12),
		CIwSVec2(0 << 12, 1 << 12),
		CIwSVec2(1 << 12, 1 << 12),
		CIwSVec2(1 << 12, 0 << 12),
	};

	CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
	for (int i = 0; i < 4; ++i)
	{
		cols[i].r = cols[i].g = cols[i].b = 0xff;
		cols[i].a = (uint8)alpha;
	}

	IwGxSetUVStream(uvs);
	IwGxSetColStream(cols);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void Utils::AlphaRenderAndRotateImage(CIwTexture* pTexture, CIwRect& bounds, double alpha, double rotationAngle)
{
	static int xOffset = IwGxGetScreenWidth() / 2;
	static int yOffset = IwGxGetScreenHeight() / 2;

	int width = bounds.w;
	int height = bounds.h;
	
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);

	// Use Texture on Material
	pMat->SetTexture(pTexture);
	IwGxSetMaterial(pMat);

	int wo2 = width / 2;
	int ho2 = height / 2;

	CIwSVec3* pWSCoords= IW_GX_ALLOC(CIwSVec3, 4);
	pWSCoords[0].x = -wo2; pWSCoords[0].y = -ho2;
	pWSCoords[1].x = -wo2; pWSCoords[1].y = -ho2 + height;
	pWSCoords[2].x = -wo2 + width; pWSCoords[2].y = -ho2 + height;
	pWSCoords[3].x = -wo2 + width; pWSCoords[3].y = -ho2;
	pWSCoords[0].z = pWSCoords[1].z = pWSCoords[2].z = pWSCoords[3].z = 0;

	CIwSVec2* pWSCoords2= IW_GX_ALLOC(CIwSVec2, 4);
	pWSCoords2[0].x = wo2; pWSCoords2[0].y = ho2;
	pWSCoords2[1].x = wo2; pWSCoords2[1].y = ho2 + height;
	pWSCoords2[2].x = wo2 + width; pWSCoords2[2].y = ho2 + height;
	pWSCoords2[3].x = wo2 + width; pWSCoords2[3].y = ho2;

	CIwMat viewMat = IwGxGetViewMatrix();

	CIwMat test = viewMat;
	test.t.x = -(bounds.x + wo2 - xOffset);
	test.t.y = -(bounds.y + ho2 - yOffset);

	IwGxSetViewMatrix(&test);

	CIwMat modelMat;
	modelMat.SetIdentity();
	modelMat.t.x = 0;
	modelMat.t.y = 0;

	iwangle degAng = (iwangle)(IW_GEOM_ONE * rotationAngle);

	CIwMat rotZ;
	rotZ.SetRotZ(degAng);
	modelMat.CopyRot(rotZ);

	IwGxSetModelMatrix(&modelMat);
	IwGxSetVertStreamModelSpace(pWSCoords, 4);

	static CIwSVec2 uvs[4] =
	{
		CIwSVec2((0 << 12) + 50, (0 << 12) + 50),
		CIwSVec2((0 << 12) + 50, (1 << 12) - 50),
		CIwSVec2((1 << 12) - 50, (1 << 12) - 50),
		CIwSVec2((1 << 12) - 50, (0 << 12) + 50),
	};

	CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
	for (int i = 0; i < 4; ++i)
	{
		cols[i].r = cols[i].g = cols[i].b = 0xff;
		cols[i].a = (uint8)alpha;
	}

	IwGxSetUVStream(uvs);
	IwGxSetColStream(cols);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

	IwGxSetViewMatrix(&viewMat);
}