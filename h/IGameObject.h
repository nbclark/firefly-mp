#pragma once
#include "IwGx.h"
#include "IwDebugPrim.h"
#include "IGameHandler.h"
#include "IwMultiplayerHandler.h"
#include "Utils.h"
#include <stdlib.h>
#include <list>

struct GameObjectMessage
{
	int32 locationX;
	int32 locationY;
	int16 active;
};

inline void PrepareSend_GameObjectMessage(GameObjectMessage& host, GameObjectMessage& network)
{
	network.locationX = s3eInetHtonl(host.locationX);
	network.locationY = s3eInetHtonl(host.locationY);
	network.active = s3eInetHtons(host.active);
}

inline void PrepareReceive_GameObjectMessage(GameObjectMessage* pNetwork, GameObjectMessage* pHost)
{
	//IwTrace("FF", ("PrepareReceive_GameObjectMessage"));
	pHost->locationX = s3eInetNtohl(pNetwork->locationX);
	//IwTrace("FF", ("PrepareReceive_GameObjectMessage: locationX: %x", pHost->locationX));
	pHost->locationY = s3eInetNtohl(pNetwork->locationY);
	//IwTrace("FF", ("PrepareReceive_GameObjectMessage: locationY: %x", pHost->locationY));
	pHost->active = s3eInetNtohs(pNetwork->active);
	//IwTrace("FF", ("PrepareReceive_GameObjectMessage: active: %x", pHost->active));
}

struct GPSObjectMessage
{
	int32 locationX;
	int32 locationY;
	int32 prevLocationX;
	int32 prevLocationY;
	int32 sizeX;
	int32 sizeY;
	int16 score;
};

inline void PrepareSend_GPSObjectMessage(GPSObjectMessage& host, GPSObjectMessage& network)
{
	char* szByte = (char*)&host;
	for (uint32 i = 0; i < sizeof(GPSObjectMessage); ++i)
	{
		//IwTrace("FF", ("PrepareSend_GPSObjectMessage: %d - %d", i, szByte[i]));
	}
	
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: locationX - %x", host.locationX));
	network.locationX = s3eInetHtonl(host.locationX);
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: locationY - %x", host.locationY));
	network.locationY = s3eInetHtonl(host.locationY);
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: prevLocationX - %x", host.prevLocationX));
	network.prevLocationX = s3eInetHtonl(host.prevLocationX);
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: prevLocationY - %x", host.prevLocationY));
	network.prevLocationY = s3eInetHtonl(host.prevLocationY);
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: sizeX - %x", host.sizeX));
	network.sizeX = s3eInetHtonl(host.sizeX);
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: sizeY - %x", host.sizeY));
	network.sizeY = s3eInetHtonl(host.sizeY);
	//IwTrace("FF", ("PrepareSend_GPSObjectMessage: score - %x", host.score));
	network.score = s3eInetHtons(host.score);
}

inline void PrepareReceive_GPSObjectMessage(GPSObjectMessage* pNetwork, GPSObjectMessage* pHost)
{
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage"));
	char* szByte = (char*)pNetwork;
	for (uint32 i = 0; i < sizeof(GPSObjectMessage); ++i)
	{
		//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: %d - %d", i, szByte[i]));
	}
	pHost->locationX = s3eInetNtohl((uint32)pNetwork->locationX);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: locationX: %x", pHost->locationX));
	pHost->locationY = s3eInetNtohl((uint32)pNetwork->locationY);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: locationY: %x", pHost->locationY));
	pHost->prevLocationX = s3eInetNtohl(pNetwork->prevLocationX);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: prevLocationX: %x", pHost->prevLocationX));
	pHost->prevLocationY = s3eInetNtohl(pNetwork->prevLocationY);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: prevLocationY: %x", pHost->prevLocationY));
	pHost->sizeX = s3eInetNtohl(pNetwork->sizeX);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: sizeX: %x", pHost->sizeX));
	pHost->sizeY = s3eInetNtohl(pNetwork->sizeY);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: sizeY: %x", pHost->sizeY));
	pHost->score = s3eInetNtohs(pNetwork->score);
	//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: score: %x", pHost->score));
}

struct PulseObjectMessage
{
	int32 locationX;
	int32 locationY;
	int32 radius;
	int32 iteration;
	int16 duration;
	int16 active;
};

inline void PrepareSend_PulseObjectMessage(PulseObjectMessage& host, PulseObjectMessage& network)
{
	network.locationX = s3eInetHtonl(host.locationX);
	network.locationY = s3eInetHtonl(host.locationY);
	network.radius = s3eInetHtonl(host.radius);
	network.iteration = s3eInetHtonl(host.iteration);
	network.duration = s3eInetHtons(host.duration);
	network.active = s3eInetHtons(host.active);
}

inline void PrepareReceive_PulseObjectMessage(PulseObjectMessage* pNetwork, PulseObjectMessage* pHost)
{
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage"));
	pHost->locationX = s3eInetNtohl(pNetwork->locationX);
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage: locationX: %x", pHost->locationX));
	pHost->locationY = s3eInetNtohl(pNetwork->locationY);
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage: locationY: %x", pHost->locationY));
	pHost->radius = s3eInetNtohl(pNetwork->radius);
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage: radius: %x", pHost->radius));
	pHost->iteration = s3eInetNtohl(pNetwork->iteration);
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage: iteration: %x", pHost->iteration));
	pHost->duration = s3eInetNtohs(pNetwork->duration);
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage: duration: %x", pHost->duration));
	pHost->active = s3eInetNtohs(pNetwork->active);
	//IwTrace("FF", ("PrepareReceive_PulseObjectMessage: active: %x", pHost->active));
}

class IGameObject
{
public:
	IGameObject(IGameHandler* pGameState, bool isMaster = false)
	{
		g_Image = NULL;
		g_pGameState = pGameState;

		g_location.x = g_location.y = 0;
		g_surfaceWidth = Iw2DGetSurfaceWidth();
		g_surfaceHeight = Iw2DGetSurfaceHeight();
		g_bDeleteImage = true;
		g_bRadialInsersection = true;
		g_updateFrequency = 1000;
		g_lastUpdate = 0;

		g_bIsActive = true;
		g_setInactive = 0;
		
		g_isMaster = isMaster;

		g_fAlpha = 255;
	}
	virtual ~IGameObject(void)
	{
		if (g_bDeleteImage && g_Image)
		{
			delete g_Image;
		}
	}
	virtual void ResetMultiplayer()
	{
		g_identifier = IwGetMultiplayerHandler()->RequestIdentifier();
		IwGetMultiplayerHandler()->RegisterCallback(g_identifier, ReceiveStatusUpdate, this);
	}
	bool IsMaster() { return g_isMaster; }
	int GetIdentifier() { return g_identifier; }

	static void ReceiveStatusUpdate(const char * Result, uint32 ResultLen, void* userData)
	{
		IGameObject* pObject = (IGameObject*)userData;
		pObject->HandleStatusUpdate(Result, ResultLen);
	}
	virtual void HandleStatusUpdate(const char * Result, uint32 ResultLen)
	{
		CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();
		GameObjectMessage networkMessage;

		IwAssertMsg("FF", sizeof(GameObjectMessage) == ResultLen, ("sizeof(GameObjectMessage) != ResultLen (%d - %d)", sizeof(GameObjectMessage), ResultLen)); 

		memcpy(&networkMessage, Result, sizeof(GameObjectMessage));
		PrepareReceive_GameObjectMessage(&networkMessage, &networkMessage);

		g_location.x = (int32)(networkMessage.locationX / pRatio->x);
		g_location.y = (int32)(networkMessage.locationY / pRatio->y);
		
		if (g_bIsActive != (networkMessage.active != 0))
		{
			uint64 time = s3eTimerGetMs();

			if ((time - g_setInactive) > 500 || g_bIsActive)
			{
				g_bIsActive = (networkMessage.active != 0);
			}
		}
	}
	virtual void SendStatusUpdate(bool forceSend)
	{
		CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();

		GameObjectMessage message;
		message.locationX = (int32)(g_location.x * pRatio->x);
		message.locationY = (int32)(g_location.y * pRatio->y);
		message.active = g_bIsActive;

		GameObjectMessage networkMessage;
		PrepareSend_GameObjectMessage(message, networkMessage);

		_SendStatusUpdate(&networkMessage, sizeof(GameObjectMessage), forceSend);
	}
	void _SendStatusUpdate(void* szMessage, int len, bool forceSend)
	{
		// if we are the slave, we need to send our update periodically
		uint64 time = s3eTimerGetMs();

		if (forceSend || (time - g_lastUpdate) > g_updateFrequency)
		{
			IwGetMultiplayerHandler()->Send(g_identifier, (char*)szMessage, len);

			g_lastUpdate = time;
		}
	}

	virtual void SetActive(bool active)
	{
		g_setInactive = s3eTimerGetMs();
		g_bIsActive = active;
	}

	virtual bool IsActive()
	{
		return g_bIsActive;
	}

	virtual bool ShouldRender()
	{
		return IsActive();
	}

	virtual void GotStatusUpdate(char* szData, int len) { }
	virtual void Reset() { }
	virtual void Render()
	{
		if (NULL != g_Image)
		{
			CIwRect bounds((int16)(g_location.x - g_width/2), (int16)(g_location.y - g_height/2), (int16)g_width, (int16)g_height);
			Utils::AlphaRenderImage(g_Image, bounds, g_fAlpha);
		}
	}
	virtual bool Update(bool* pbVictory) { return false; }

	virtual bool InsersectsWith(IGameObject* pGameObject)
	{
		// do radius intersections

		if (g_bRadialInsersection && (g_width == g_height))
		{
			return InsersectsCircularWith(pGameObject);
		}

		CIwRect* pObjBounds = pGameObject->GetBounds();
		int objLeft = pObjBounds->x;
		int objTop = pObjBounds->y;
		int objRight = pObjBounds->x + pObjBounds->w;
		int objBottom = pObjBounds->y + pObjBounds->h;
		
		int thisLeft = g_location.x;
		int thisTop = g_location.y;
		int thisRight = g_location.x + g_width;
		int thisBottom = g_location.y + g_height;

		if (objRight < thisLeft || thisRight < objLeft)
		{
			return false;
		}
		if (objBottom < thisTop || thisBottom < objTop)
		{
			return false;
		}
		return true;
	}

	virtual bool InsersectsCircularWith(IGameObject* pGameObject)
	{
		int radius = MIN(g_width, g_height) / 2;

		int objRadius = 0;
		CIwVec2* pObjBounds = pGameObject->GetBoundingCircle(&objRadius);

		// Get the distance
		int distanceSquared = (int)(pow(g_location.x - pObjBounds->x, 2) + pow(g_location.y - pObjBounds->y, 2));
		int radiusSquared = (int)pow((radius + objRadius), 2);

		return (distanceSquared < radiusSquared);
	}

	CIwRect* GetBounds()
	{
		g_tempBounds.x = (int16)g_location.x;
		g_tempBounds.y = (int16)g_location.y;
		g_tempBounds.w = (int16)g_width;
		g_tempBounds.h = (int16)g_height;

		return &g_tempBounds;
	}

	CIwVec2* GetBoundingCircle(int* pRadius)
	{
		*pRadius = MAX(g_width, g_height) / 2;
		return &g_location;
	}

	void GetSize(CIwVec2* pSize)
	{
		pSize->x = g_width;
		pSize->y = g_height;
	}

protected:
	float g_fAlpha;
	uint64 g_setInactive;
	int g_width;
	int g_height;
	int g_surfaceWidth;
	int g_surfaceHeight;
	CIwRect g_tempBounds;
	CIwVec2 g_location;
	IGameHandler* g_pGameState;
	CIwTexture* g_Image;
	bool g_bDeleteImage;
	bool g_bRadialInsersection;
	bool g_bIsActive;
	uint64 g_updateFrequency, g_lastUpdate;
	uint32 g_identifier;
	bool g_isMaster;
};

class CGPSObject : public IGameObject
{
public:
	CGPSObject(IGameHandler* pGameState, char* szImage, bool isMaster) : IGameObject(pGameState, isMaster)
	{
		if (strstr(szImage, "user.png"))
		{
			g_Image = (CIwTexture*)IwGetResManager()->GetResNamed("user", "CIwTexture");
		}
		else
		{
			g_Image = (CIwTexture*)IwGetResManager()->GetResNamed("userB", "CIwTexture");
		}

		g_width = (int)g_Image->GetWidth();
		g_height = (int)g_Image->GetHeight();

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * g_width;
		g_height = fScale * g_height;
	}
	virtual void Render()
	{
		if (NULL != g_Image)
		{
			CIwRect bounds((int16)(g_location.x - g_width/2), (int16)(g_location.y - g_height/2), (int16)g_width, (int16)g_height);
			Utils::AlphaRenderImage(g_Image, bounds, 255.0);
		}
	}
	virtual void Reset()
	{
		g_currLocation = g_pGameState->GetPosition();
		g_location.x = g_currLocation.x;
		g_location.y = g_currLocation.y;

		g_width = (int)g_Image->GetWidth();
		g_height = (int)g_Image->GetHeight();

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * g_width;
		g_height = fScale * g_height;

		g_iAnimationCount = 0;
	}
	virtual bool Update(bool* pbVictory)
	{
		// Animate the transitions between positions
		int iter = (g_iAnimationCount % 10);
		
		if (iter == 0)
		{
			g_prevLocation = g_currLocation;

			if (g_isMaster)
			{
				// every half second we check our GPS
				g_currLocation = g_pGameState->GetPosition();
				g_accuracy = g_pGameState->GetAccuracy();

				// Bound our current location to inside the grid
				FixBoundedLocation(g_currLocation, g_accuracy);
			}
		}
		else
		{
			float animPercent = (10 - iter) / 10.0f;

			g_location.x = (int16)((g_currLocation.x) - (g_currLocation.x - g_prevLocation.x) * animPercent);
			g_location.y = (int16)((g_currLocation.y) - (g_currLocation.y - g_prevLocation.y) * animPercent);
		}

		//	move it around here a bit based on the GPS
		g_iAnimationCount++;

		return false;
	}
protected :
	virtual void FixBoundedLocation(CIwSVec2& location, CIwFVec2& accuracy)
	{
		location.x = MAX(0, MIN(location.x, g_surfaceWidth-(int)g_width));
		location.y = g_surfaceHeight - (int)g_height;
	}
	int g_iAnimationCount;
	CIwSVec2 g_currLocation, g_prevLocation;
	CIwFVec2 g_accuracy;
};


class CGPSUser : public CGPSObject
{
public:
	CGPSUser(IGameHandler* pGameState, CIwMultiplayerHandler::User* pUser, char* szUser, bool isMaster) : CGPSObject(pGameState, szUser, isMaster)
	{
		g_pUser = pUser;
		SetScore(0);
		g_prevLocation.x = g_prevLocation.y = 0;
		g_currLocation.x = g_currLocation.y = 0;
	}
	int GetScore() { return g_score; }
	void SetScore(int score)
	{
		g_score = score;
		if (g_pUser)
		{
			IwGetMultiplayerHandler()->SetUserScore(g_pUser, g_score);
		}
	}
	
	void SendStatusUpdate(bool forceSend)
	{
		CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();

		GPSObjectMessage message;
		message.locationX = (int32)(g_currLocation.x * pRatio->x);
		message.locationY = (int32)(g_currLocation.y * pRatio->y);
		message.prevLocationX = (int32)(g_prevLocation.x * pRatio->x);
		message.prevLocationY = (int32)(g_prevLocation.y * pRatio->y);

		message.sizeX = g_width;
		message.sizeY = g_height;
		message.score = g_score;

		GPSObjectMessage networkMessage;
		PrepareSend_GPSObjectMessage(message, networkMessage);
		_SendStatusUpdate(&networkMessage, sizeof(GPSObjectMessage), forceSend);
	}
	virtual void HandleStatusUpdate(const char * Result, uint32 ResultLen)
	{
		//IwTrace("FF", ("GPSUser - HandleStatusUpdate(%d-%d)", ResultLen, sizeof(GPSObjectMessage)));
		
		for (uint32 i = 0; i < sizeof(GPSObjectMessage); ++i)
		{
			//IwTrace("FF", ("PrepareReceive_GPSObjectMessage: %d - %d", i, Result[i]));
		}

		//GPSObjectMessage* pMessage = (GPSObjectMessage*)Result;
		//IwTrace("FF", ("GPSUser - memcpy"));
		GPSObjectMessage networkMessage;

		//IwTrace("FF", ("sizeof(GPSObjectMessage) != ResultLen (%d - %d)", sizeof(GPSObjectMessage), ResultLen));
		IwAssertMsg("FF", sizeof(GPSObjectMessage) == ResultLen, ("sizeof(GPSObjectMessage) != ResultLen (%d - %d)", sizeof(GPSObjectMessage), ResultLen)); 
		memcpy(&networkMessage, Result, sizeof(GPSObjectMessage));
		
		//IwTrace("FF", ("GPSUser - PrepareReceive_GPSObjectMessage"));
		PrepareReceive_GPSObjectMessage(&networkMessage, &networkMessage);

		//IwTrace("FF", ("GPSUser - HandleStatusUpdate"));
		if (!IsMaster())
		{
			CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();
			
			//IwTrace("FF", ("GPSUser - ratiox = %d, ratioy = %d, locX = %d, locY = %d, sizeX = %d, sizeY = %d, score = %d", pRatio->x, pRatio->y, networkMessage.locationX, networkMessage.locationY, networkMessage.sizeX, networkMessage.sizeY, networkMessage.score));

			g_prevLocation.x = g_currLocation.x;
			g_prevLocation.y = g_currLocation.y;

			g_currLocation.x = (int16)(networkMessage.locationX / pRatio->x);
			g_currLocation.y = (int16)(networkMessage.locationY / pRatio->y);

			// fscale
			//g_width = networkMessage.sizeX;
			//g_height = networkMessage.sizeY;

			g_iAnimationCount = 1;
		}
		
		if (!IwGetMultiplayerHandler()->IsMaster())
		{
			//IwTrace("FF", ("GPSUser - SetScore"));
			// Only accept the score if we aren't the game master
			SetScore(networkMessage.score);
		}
		//IwTrace("FF", ("GPSUser - Done"));
	}

protected:
	void FixBoundedLocation(CIwSVec2& location, CIwFVec2& accuracy)
	{
		// Scale the size of the piece from g_Image.width to g_Image.width * 3

		// Scale from 1 to 3
		int scalingFactor = (int)(MAX(25, MIN3(accuracy.x, accuracy.y, 75)) / 25);

		// fscale
		//g_width = g_Image->GetWidth() * scalingFactor;
		//g_height = g_Image->GetHeight() * scalingFactor;

		location.x = MAX(0, MIN(location.x, g_surfaceWidth));
		location.y = MAX(0, MIN(location.y, g_surfaceHeight));
	}
	int g_score;
	CIwMultiplayerHandler::User* g_pUser;
};

class CPulse : public IGameObject
{
public:
	CPulse(IGameHandler* pGameState, IGameObject* pUser, bool isMaster) : IGameObject(pGameState, isMaster)
	{
		g_iIter = 1;
		g_iTotalIter = 0;
		g_iRadius = 0;
		g_iCurRadius = 0;
		g_pUser = pUser;
		g_duration = 0;
	}

	bool IsActive()
	{
		return (g_iIter <= g_iTotalIter);
	}

	void SendStatusUpdate(bool forceSend)
	{
		// TODO - maybe send this a little less often, or just send that is started and emulate on the other end
		CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();

		PulseObjectMessage message;
		message.locationX = (int32)(g_location.x * pRatio->x);
		message.locationY = (int32)(g_location.y * pRatio->y);

		message.radius = g_iRadius;
		message.duration = (uint16)(g_duration * 100);
		message.active = g_bIsActive;
		message.iteration = g_iIter;

		PulseObjectMessage networkMessage;
		PrepareSend_PulseObjectMessage(message, networkMessage);
		_SendStatusUpdate(&networkMessage, sizeof(PulseObjectMessage), forceSend);
	}

	void SetPulse(int iRadius, double duration)
	{
		g_duration = duration;
		g_iIter = 0;
		g_iCurRadius = 0;
		g_iDelayIter = (int)(0.25 * FRAMES_PER_S);
		g_iTotalIter = (int)(duration * FRAMES_PER_S) + g_iDelayIter;
		g_iRadius = iRadius;
	}

	void HandleStatusUpdate(const char * Result, uint32 ResultLen)
	{
		if (!IsMaster())
		{
			CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();

			//PulseObjectMessage* pMessage = (PulseObjectMessage*)Result;
			PulseObjectMessage networkMessage;

			//IwTrace("FF", ("sizeof(PulseObjectMessage) != ResultLen (%d - %d)", sizeof(PulseObjectMessage), ResultLen));
			IwAssertMsg("FF", sizeof(PulseObjectMessage) == ResultLen, ("sizeof(PulseObjectMessage) != ResultLen (%d - %d)", sizeof(PulseObjectMessage), ResultLen)); 
			memcpy(&networkMessage, Result, sizeof(PulseObjectMessage));
			
			//IwTrace("FF", ("GPSUser - PrepareReceive_GPSObjectMessage"));
			PrepareReceive_PulseObjectMessage(&networkMessage, &networkMessage);

			g_location.x = (int32)(networkMessage.locationX / pRatio->x);
			g_location.y = (int32)(networkMessage.locationY / pRatio->y);
			g_duration = networkMessage.duration / 100.0;
			g_iIter = networkMessage.iteration;

			g_iCurRadius = 0;
			g_iDelayIter = (int)(0.25 * FRAMES_PER_S);
			g_iTotalIter = (int)(g_duration * FRAMES_PER_S) + g_iDelayIter;
			g_iRadius = networkMessage.radius;
		}
	}

	bool InsersectsWith(IGameObject* pGameObject)
	{
		// If any of the 4 corners is in the circle, we intersect
		int objRadius;
		CIwVec2* pObjBounds = pGameObject->GetBoundingCircle(&objRadius);

		int objCenterX = pObjBounds->x;
		int objCenterY = pObjBounds->y;

		int userRadius;
		CIwVec2* bounds = g_pUser->GetBoundingCircle(&userRadius);
		int x = bounds->x;
		int y = bounds->y;
		
		int radSquared = (int)pow(g_iCurRadius + objRadius, 2);
		int disCenterSquared = (int)(pow(objCenterX-x,2) + pow(objCenterY-y,2));

		if (disCenterSquared <= radSquared)
		{
			return true;
		}
		return false;
	}

	bool Update(bool* pbVictory)
	{
		if (g_iIter <= g_iTotalIter)
		{
			g_iIter++;

			if (g_iIter < g_iDelayIter)
			{
				g_iCurRadius = 0;
			}
			else
			{
				g_iCurRadius = (int)(g_iRadius * ((double)(g_iIter-g_iDelayIter) / (g_iTotalIter-g_iDelayIter)));
			}
		}
		else
		{
			g_iCurRadius = 0;
		}

		return false;
	}
	void Render()
	{
		if (g_iCurRadius > 0)
		{
			int userRadius = 0;
			CIwVec2* bounds = g_pUser->GetBoundingCircle(&userRadius);

			Iw2DSetColour(0xFFFFFFFF);
			Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
			Iw2DFillArc(CIwSVec2((int16)bounds->x, (int16)bounds->y), CIwSVec2((int16)g_iCurRadius, (int16)g_iCurRadius), 0, IW_ANGLE_PI*2, 0);
			Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
		}
	}
private:
	IGameObject* g_pUser;
	int g_iRadius, g_iIter, g_iTotalIter, g_iCurRadius, g_iDelayIter;
	double g_duration;
};

class StaticTextureHolder
{
public :
	CIwTexture* g_pTexture;
	StaticTextureHolder()
	{
		g_isInit = false;
		g_pTexture = NULL;
	}
	void Init(char* szTexture)
	{
		if (!g_isInit)
		{
			CIwImage img;
			img.LoadFromFile(szTexture);
			//g_Image = Iw2DCreateImage(img);
			g_pTexture = new CIwTexture();
			g_pTexture->CopyFromImage(&img);
			g_pTexture->Upload();

			g_isInit = true;
		}
	}
	void Cleanup()
	{
		if (g_pTexture)
		{
			delete g_pTexture;
			g_pTexture = NULL;

			g_isInit = false;
		}
	}
private :
	bool g_isInit;
};
