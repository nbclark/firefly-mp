#ifndef UTILS
#define UTILS
#include "IwGx.h"
#include "IwGxFontContainer.h"
#include "Iw2D.h"
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include "s3eLocation.h"

class Utils
{
public:
	static double ScaleWidth;
	static double ScaleHeight;
	static int SidePadding;
	static int BottomPadding;
	static int FPS;
	static char ResourceFile[100];

	static void AlphaRenderImage(CIwTexture* pTexture, CIwSVec2& location, double alpha);
	static void AlphaRenderImage(CIwTexture* pTexture, CIwRect& bounds, double alpha);
	static void AlphaRenderAndRotateImage(CIwTexture* pTexture, CIwRect& bounds, double alpha, double rotationAngle);

	static bool LoadMap(const char* szData, std::list<s3eLocation>* pCorners, float* pZoom);
	static char* GetMapData(const char* szMap);
	static void SaveMapData(const char* szMap, const char* szData);

	
	static CIwGxFont* GetFont(bool isLarge)
	{
		if (!g_pFont)
		{
			int minRes = MIN(Iw2DGetSurfaceWidth(), Iw2DGetSurfaceHeight());

			if (minRes < 480)
			{
				g_pFont = (CIwGxFont*)IwGetResManager()->GetResNamed("font_medium", "CIwGxFont");
				g_pFontLarge = (CIwGxFont*)IwGetResManager()->GetResNamed("font_large", "CIwGxFont");
			}
			else
			{
				g_pFont = (CIwGxFont*)IwGetResManager()->GetResNamed("font_large", "CIwGxFont");
				g_pFontLarge = (CIwGxFont*)IwGetResManager()->GetResNamed("font_huge", "CIwGxFont");
			}
		}
		if (isLarge)
		{
			return g_pFontLarge;
		}
		else
		{
			return g_pFont;
		}
	}
	static float GetTextScalingFactor()
	{
		if (!g_fTScale)
		{
			double xScale = Iw2DGetSurfaceWidth() / 640.0;
			double yScale = Iw2DGetSurfaceHeight() / 960.0;

			g_fTScale = MIN(xScale, yScale);
		}
		return g_fTScale;
	}

	static float GetImageScalingFactor()
	{
		if (!g_fScale)
		{
			double xScale = Iw2DGetSurfaceWidth() / ScaleWidth;
			double yScale = Iw2DGetSurfaceHeight() / ScaleHeight;
			//double xScale = Iw2DGetSurfaceWidth() / 640.0;
			//double yScale = Iw2DGetSurfaceHeight() / 960.0;

			g_fScale = MIN(xScale, yScale);
		}
		return g_fScale;
	}

	static float g_fScale;
	static float g_fTScale;
	static CIwGxFont* g_pFont;
	static CIwGxFont* g_pFontLarge;
};

#endif

