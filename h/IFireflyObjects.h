#pragma once
#include "IGameObject.h"
#include "main.h"

struct FireflyObjectMessage
{
	int32 locationX;
	int32 locationY;
	int32 prevLocationX;
	int32 prevLocationY;
	int16 iteration;
	int16 animIteration;
	int16 stunIteration;
	int16 stunIterationCount;
	int16 active;
	int16 isStunned;
};

inline void PrepareSend_FireflyObjectMessage(FireflyObjectMessage& host, FireflyObjectMessage& network)
{
	network.locationX = s3eInetHtonl(host.locationX);
	network.locationY = s3eInetHtonl(host.locationY);
	network.prevLocationX = s3eInetHtonl(host.prevLocationX);
	network.prevLocationY = s3eInetHtonl(host.prevLocationY);
	network.iteration = s3eInetHtons(host.iteration);
	network.animIteration = s3eInetHtons(host.animIteration);
	network.stunIteration = s3eInetHtons(host.stunIteration);
	network.stunIterationCount = s3eInetHtons(host.stunIterationCount);
	network.active = s3eInetHtons(host.active);
	network.isStunned = s3eInetHtons(host.isStunned);
}

inline void PrepareReceive_FireflyObjectMessage(FireflyObjectMessage* pNetwork, FireflyObjectMessage* pHost)
{
	pHost->locationX = s3eInetNtohl(pNetwork->locationX);
	pHost->locationY = s3eInetNtohl(pNetwork->locationY);
	pHost->prevLocationX = s3eInetNtohl(pNetwork->prevLocationX);
	pHost->prevLocationY = s3eInetNtohl(pNetwork->prevLocationY);
	pHost->iteration = s3eInetNtohs(pNetwork->iteration);
	pHost->animIteration = s3eInetNtohs(pNetwork->animIteration);
	pHost->stunIteration = s3eInetNtohs(pNetwork->stunIteration);
	pHost->stunIterationCount = s3eInetNtohs(pNetwork->stunIterationCount);
	pHost->active = s3eInetNtohs(pNetwork->active);
	pHost->isStunned = s3eInetNtohs(pNetwork->isStunned);
}

class CFirefly : public IGameObject
{
public:
	CFirefly(IGameHandler* pGameState, CIwTexture* pTexture, CIwTexture* pTextureGlow, CIwTexture* pTextureStun) : IGameObject(pGameState)
	{
		g_pTextureStun = pTextureStun;
		CIwFVec2 moveVelocity(5, 5);
		g_pTexture = pTexture;
		g_pTextureGlow = pTextureGlow;
		g_iTop = 0;

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * pTexture->GetWidth();
		g_height = fScale * pTexture->GetHeight();

		SetDifficulty(moveVelocity, 3);
	}

	virtual void SetDifficulty(CIwFVec2& moveMetersPerCycle, float glowCycleSecondDuration)
	{
		CIwFVec2 moveVelo = g_pGameState->GetScaler()->GetSpeedInPixels(moveMetersPerCycle);
		g_glowIterations = (int)(glowCycleSecondDuration * g_pGameState->GetFramesPerSecond());
		g_glowStepIterations = g_glowIterations / 3;

		// we change position once every cycle

		g_moveVelocity.x = moveVelo.x * glowCycleSecondDuration / 3;
		g_moveVelocity.y = moveVelo.y * glowCycleSecondDuration / 3;

		Reset();
	}

	inline bool ShouldRender()
	{
		return (g_bIsActive || (g_iCaptureCount > 0));
	}

	bool IsActive()
	{
		return g_bIsActive;
	}

	void MakeCapture(CIwVec2& endLocation)
	{
		if (g_iCaptureCount <= 0)
		{
			g_prevLocation.x = g_location.x;
			g_prevLocation.y = g_location.y;
			g_currLocation.x = endLocation.x + g_width / 2;
			g_currLocation.y = endLocation.y + g_height / 2;
			SetActive(false);
			SetRotationAngle(true);
			g_bStunned = true;
			g_iCaptureCount = 1;
		}
	}

	void SetTopBound(int iTop)
	{
		g_iTop = iTop;
	}

	virtual void Reset()
	{
		int xInit = IwRandMinMax(0, g_surfaceWidth);
		int yInit = IwRandMinMax(g_iTop, g_surfaceHeight);
		g_iAnimationCount = IwRandMinMax(0, g_glowIterations);

		g_prevLocation.x = g_currLocation.x = (int16)(g_location.x = xInit);
		g_prevLocation.y = g_currLocation.y = (int16)(g_location.y = yInit);

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * g_pTexture->GetWidth();
		g_height = fScale * g_pTexture->GetHeight();

		g_iStunIterationCount = g_iStunIteration = 0;
		g_bStunned = false;

		g_tempRotationAngle = 0;
		g_rotationAngle = (IwRandMinMax(-1, 2) * 0.25);

		g_iCaptureCount = 0;

		BoundCoordinates(g_currLocation);
	}
	void HandleStatusUpdate(const char * Result, uint32 ResultLen)
	{
		if (g_iCaptureCount <= 0)
		{
			CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();

			IwAssertMsg("FF", sizeof(FireflyObjectMessage) == ResultLen, ("sizeof(FireflyObjectMessage) != ResultLen (%d - %d)", sizeof(FireflyObjectMessage), ResultLen)); 

			FireflyObjectMessage* pMessage = (FireflyObjectMessage*)Result;
			FireflyObjectMessage networkMessage;

			memcpy(&networkMessage, Result, sizeof(FireflyObjectMessage));
			PrepareReceive_FireflyObjectMessage(&networkMessage, &networkMessage);

			g_currLocation.x = (int16)(networkMessage.locationX / pRatio->x);
			g_currLocation.y = (int16)(networkMessage.locationY / pRatio->y);
			g_prevLocation.x = (int16)(networkMessage.prevLocationX / pRatio->x);
			g_prevLocation.y = (int16)(networkMessage.prevLocationY / pRatio->y);
			g_glowStepIterations = networkMessage.iteration;
			g_iStunIteration = networkMessage.stunIteration;
			g_iStunIterationCount = networkMessage.stunIterationCount;
			g_iAnimationCount = networkMessage.animIteration;
			g_bStunned = (networkMessage.isStunned != 0);

			if (g_bIsActive != (networkMessage.active != 0))
			{
				uint64 time = s3eTimerGetMs();

				if ((time - g_setInactive) > 500 || g_bIsActive)
				{
					g_bIsActive = (networkMessage.active != 0);
				}
			}
			SetRotationAngle(false);
		}
	}

	void SetRotationAngle(bool forceSet)
	{
		bool xPos = (g_currLocation.x > g_prevLocation.x);
		bool yPos = (g_currLocation.y > g_prevLocation.y);

		double proposedAngle = 0;
		if (xPos && yPos)
		{
			// right & down
			proposedAngle = -90 / 360.0;
		}
		else if (xPos && !yPos)
		{
			// right and up
			proposedAngle = 0.0;
		}
		else if (!xPos && yPos)
		{
			// left and down
			proposedAngle = 180.0 / 360.0;
		}
		else 
		{
			// left and up
			proposedAngle = 90.0 / 360.0;
		}

		static int xB = -10;
		static int yB = -10;

		if (!(g_currLocation.x - g_prevLocation.x) && !(g_currLocation.y - g_prevLocation.y))
		{
			proposedAngle = g_rotationAngle;
		}
		else if (!(g_currLocation.x - g_prevLocation.x))
		{
			proposedAngle = ((g_currLocation.y - g_prevLocation.y) > 0) ? (-135.0 / 360) : (45.0 / 360);
		}
		else if (!(g_currLocation.y - g_prevLocation.y))
		{
			proposedAngle = ((g_currLocation.x - g_prevLocation.x) > 0) ? (-45.0 / 360) : (135.0 / 360);
		}
		else
		{
			double slope = -(double)(g_currLocation.y - g_prevLocation.y) / (g_currLocation.x - g_prevLocation.x);
			double angle = atan(slope) * 180 / PI;

			if (((g_currLocation.y - g_prevLocation.y) > 0) && ((g_currLocation.x - g_prevLocation.x) < 0))
			{
				angle -= 180;
			}
			if (((g_currLocation.y - g_prevLocation.y) < 0) && ((g_currLocation.x - g_prevLocation.x) < 0))
			{
				angle -= 180;
			}

			proposedAngle = (angle-45);
			if (proposedAngle > 180)
			{
				proposedAngle = proposedAngle - 360;
			}
			proposedAngle = proposedAngle / 360;
		}
		if (forceSet || (g_rotationAngle != proposedAngle))
		{
			g_tempRotationAngle = g_rotationAngle;
			g_rotationAngle = proposedAngle;
			g_rotationIncrement = (g_rotationAngle - g_tempRotationAngle) / 20;
		}
	}

	void SendStatusUpdate(bool forceSend)
	{
		CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();

		FireflyObjectMessage message;
		message.locationX = (int32)(g_currLocation.x * pRatio->x);
		message.locationY = (int32)(g_currLocation.y * pRatio->y);
		message.prevLocationX = (int32)(g_prevLocation.x * pRatio->x);
		message.prevLocationY = (int32)(g_prevLocation.y * pRatio->y);

		message.iteration = g_glowStepIterations;
		message.animIteration = g_iAnimationCount;
		message.stunIteration = g_iStunIteration;
		message.stunIterationCount = g_iStunIterationCount;
		message.active = g_bIsActive;
		message.isStunned = g_bStunned;

		FireflyObjectMessage networkMessage;
		PrepareSend_FireflyObjectMessage(message, networkMessage);

		_SendStatusUpdate(&networkMessage, sizeof(FireflyObjectMessage), true);
	}

	virtual bool Update(bool* pbVictory)
	{
		int iterCount = g_glowStepIterations;
		// Animate the transitions between positions

		if (g_iCaptureCount > 0)
		{
			static int captureIterations = 10;
			if (g_iCaptureCount == captureIterations)
			{
				g_iCaptureCount = 0;
			}
			else
			{
				g_iCaptureCount++;
				float animPercent = (captureIterations - g_iCaptureCount) / (float)captureIterations;

				g_location.x = (int16)((g_currLocation.x) - (g_currLocation.x - g_prevLocation.x) * animPercent);
				g_location.y = (int16)((g_currLocation.y) - (g_currLocation.y - g_prevLocation.y) * animPercent);
			}
		}
		else if (g_iStunIteration < g_iStunIterationCount)
		{
			g_iStunIteration++;
		}
		else
		{
			g_bStunned = false;
			int iter = (g_iAnimationCount % iterCount);
			
			if (iter == 0)
			{
				// If we are the master, move the flies and send an update
				g_prevLocation = g_currLocation;

				if (0 == (g_iAnimationCount % g_glowIterations))
				{
					g_iAnimationCount = 0;
				}

				if (IwGetMultiplayerHandler()->IsMaster())
				{
					// TODO, make these values scale based on field size
					int xDiff = IwRandMinMax(-g_moveVelocity.x/2, g_moveVelocity.x/2);
					int yDiff = IwRandMinMax(-g_moveVelocity.y/2, g_moveVelocity.y/2);

					// every half second we change our position
					g_currLocation.x = g_prevLocation.x + xDiff;
					g_currLocation.y = g_prevLocation.y + yDiff;
				}

				// Bound our current location to inside the grid
				// Bound our current location to the bottom of the screen
				BoundCoordinates(g_currLocation);

				SetRotationAngle(true);
			}
			else
			{
				float animPercent = (iterCount - iter) / (float)iterCount;

				g_location.x = (int16)((g_currLocation.x) - (g_currLocation.x - g_prevLocation.x) * animPercent);
				g_location.y = (int16)((g_currLocation.y) - (g_currLocation.y - g_prevLocation.y) * animPercent);
			}

			//	move it around here a bit based on the GPS
			g_iAnimationCount++;
		}

		return false;
	}
	void Render()
	{
		CIwTexture* pTexture = (g_bStunned) ? g_pTextureStun : g_pTexture;

		if (NULL != pTexture)
		{
			double fAlpha = (g_bStunned) ? 255 : GetAlpha();

			if (g_tempRotationAngle < g_rotationAngle)
			{
				g_tempRotationAngle += g_rotationIncrement;

				if (g_tempRotationAngle > g_rotationAngle)
				{
					g_tempRotationAngle = g_rotationAngle;
				}
			}
			else if (g_tempRotationAngle > g_rotationAngle)
			{
				g_tempRotationAngle += g_rotationIncrement;
				g_tempRotationAngle += g_rotationIncrement;

				if (g_tempRotationAngle < g_rotationAngle)
				{
					g_tempRotationAngle = g_rotationAngle;
				}
			}

			if (fAlpha)
			{
				CIwRect bounds((int16)(g_location.x - g_width/2), (int16)(g_location.y - g_height/2), (int16)g_width, (int16)g_height);

				#ifdef HIGH_FPS_MODE
					Utils::AlphaRenderImage(g_pTextureGlow, bounds, fAlpha);
				#endif
				#ifndef HIGH_FPS_MODE
					Utils::AlphaRenderAndRotateImage(pTexture, bounds, fAlpha, g_tempRotationAngle);

					if (!g_bStunned && fAlpha > 64)
					{
						Utils::AlphaRenderAndRotateImage(g_pTextureGlow, bounds, (fAlpha - 55) * 1.275, g_tempRotationAngle);
					}
				#endif
			}
		}
	}
	virtual inline bool IsVisible()
	{
		int step = (g_iAnimationCount /  g_glowStepIterations) % 3; 

		return (step != 0);
	}
	void Stun(double duration)
	{
		g_iStunIterationCount = (int)(duration * FRAMES_PER_S);
		g_iStunIteration = 0;
		g_bStunned = true;
	}
protected :
	virtual inline void BoundCoordinates(CIwSVec2& coords)
	{
		coords.x = (int16)(MAX(0, MIN(coords.x, g_surfaceWidth)));
		coords.y = (int16)(MAX(g_iTop, MIN(coords.y, g_surfaceHeight)));
	}
	virtual inline double GetAlpha()
	{
		double fAlpha = 0;
		int step = (g_iAnimationCount /  g_glowStepIterations) % 3; 
		if (step == 0)
		{
			fAlpha = 0;
		}
		else if (step == 2)
		{
			int subStep = (g_iAnimationCount % g_glowStepIterations);
			fAlpha = 255 - (255 * subStep / (float)g_glowStepIterations);
		}
		else
		{
			int subStep = (g_iAnimationCount % g_glowStepIterations);
			fAlpha = (255 * subStep / (float)g_glowStepIterations);
		}
		return fAlpha;
	}
	int g_iStunIteration;
	int g_iStunIterationCount;
	int g_iAnimationCount;
	int g_glowIterations;
	int g_glowStepIterations;
	int g_iTop;
	bool g_bStunned;
	int g_iCaptureCount;
	double g_rotationAngle, g_tempRotationAngle, g_rotationIncrement;
	CIwVec2 g_moveVelocity;
	CIwTexture* g_pTexture, *g_pTextureStun, *g_pTextureGlow;
private:
	CIwSVec2 g_currLocation, g_prevLocation;
};


class CBonusFirefly : public CFirefly
{
public:
	CBonusFirefly(IGameHandler* pGameState, CIwTexture* pTexture, CIwTexture* pTextureGlow, CIwTexture* pTextureStun) : CFirefly(pGameState, pTexture, pTextureGlow, pTextureStun)
	{
		g_pTextureStun = pTextureStun;
		CIwFVec2 moveVelocity(5, 5);
		g_pTexture = pTexture;

		SetDifficulty(moveVelocity, 3);
	}

	virtual void SetDifficulty(CIwFVec2& moveMetersPerCycle, float glowCycleSecondDuration)
	{
		CIwFVec2 moveVelo = g_pGameState->GetScaler()->GetSpeedInPixels(moveMetersPerCycle);
		g_glowIterations = (int)(glowCycleSecondDuration * g_pGameState->GetFramesPerSecond());
		g_glowStepIterations = g_glowIterations / 3;

		// we change position once every cycle

		g_moveVelocity.x = 100 * moveVelo.x / g_pGameState->GetFramesPerSecond();
		g_moveVelocity.y = 100 * moveVelo.y / g_pGameState->GetFramesPerSecond();

		if (g_moveVelocity.x < 0)
		{
			g_moveVelocity.x = IwRandMinMax(g_moveVelocity.x, g_moveVelocity.x / 2);
		}
		else
		{
			g_moveVelocity.x = IwRandMinMax(g_moveVelocity.x / 2, g_moveVelocity.x);
		}
		if (g_moveVelocity.y < 0)
		{
			g_moveVelocity.y = IwRandMinMax(g_moveVelocity.y, g_moveVelocity.y / 2);
		}
		else
		{
			g_moveVelocity.y = IwRandMinMax(g_moveVelocity.y / 2, g_moveVelocity.y);
		}
		g_moveVelocity.x /= 100;
		g_moveVelocity.y /= 100;

		Reset();

		g_actualLocation.x = g_location.x;
		g_actualLocation.y = g_location.y;
	}

	// We will update the same as a firefly, but let's just bound around a wall
	virtual bool Update(bool* pbVictory)
	{
		if (g_iCaptureCount > 0)
		{
			return CFirefly::Update(pbVictory);
		}
		else
		{
			int iterCount = g_glowStepIterations;
			// Animate the transitions between positions

			int iter = (g_iAnimationCount % iterCount);
			
			if (0 == (g_iAnimationCount % g_glowIterations))
			{
				g_iAnimationCount = 0;
			}

			g_actualLocation.x = g_actualLocation.x + g_moveVelocity.x;
			g_actualLocation.y = g_actualLocation.y + g_moveVelocity.y;

			if (g_actualLocation.x < 0)
			{
				g_actualLocation.x = 0;
				g_moveVelocity.x = -g_moveVelocity.x;
			}
			if (g_actualLocation.x > g_surfaceWidth)
			{
				g_actualLocation.x = g_surfaceWidth;
				g_moveVelocity.x = -g_moveVelocity.x;
			}
			if (g_actualLocation.y < g_iTop)
			{
				g_actualLocation.y = g_iTop;
				g_moveVelocity.y = -g_moveVelocity.y;
			}
			if (g_actualLocation.y > g_surfaceHeight)
			{
				g_actualLocation.y = g_surfaceHeight;
				g_moveVelocity.y = -g_moveVelocity.y;
			}

			g_location.x = g_actualLocation.x;
			g_location.y = g_actualLocation.y;

			bool xPos = (g_moveVelocity.x > 0);
			bool yPos = (g_moveVelocity.y > 0);

			g_tempRotationAngle = g_rotationAngle;
			if (xPos && yPos)
			{
				// right & down
				g_rotationAngle = -90 / 360.0;
			}
			else if (xPos && !yPos)
			{
				// right and up
				g_rotationAngle = 0.0;
			}
			else if (!xPos && yPos)
			{
				// left and down
				g_rotationAngle = 180.0 / 360.0;
			}
			else 
			{
				// left and up
				g_rotationAngle = 90.0 / 360.0;
			}

			//	move it around here a bit based on the GPS
			g_iAnimationCount++;
			return false;
		}
	}
protected :
	CIwFVec2 g_actualLocation, g_moveVelocity;
};

class CDragon : public IGameObject
{
public:
	CDragon(IGameHandler* pGameState, CIwTexture* pTexture) : IGameObject(pGameState)
	{
		g_bDeleteImage = false;
		g_Image = pTexture;

		g_width = g_Image->GetWidth();
		g_height = g_Image->GetHeight();

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * g_width;
		g_height = fScale * g_height;

		g_minX = g_width / 2;
		g_maxX = Iw2DGetSurfaceWidth() - g_minX;
		g_minY = g_height / 2;
		g_maxY = Iw2DGetSurfaceHeight() - g_minY;
	}
	void SetDifficulty(CIwFVec2& moveMetersPerCycle, float pathCycleSecondDuration)
	{
		CIwFVec2 moveVelo = g_pGameState->GetScaler()->GetSpeedInPixels(moveMetersPerCycle);
		g_recalculateIterations = (int)(pathCycleSecondDuration * g_pGameState->GetFramesPerSecond());

		g_moveVelocity.x = (int32)(MAX(moveVelo.x, 2));
		g_moveVelocity.y = (int32)(MAX(moveVelo.y, 2));

		Reset();
	}
	void Reset()
	{
		g_width = g_Image->GetWidth();
		g_height = g_Image->GetHeight();

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * g_width;
		g_height = fScale * g_height;

		g_currLocation.x = g_prevLocation.x = (int16)(g_location.x = IwRandMinMax(g_minX, g_maxX));
		g_currLocation.y = g_prevLocation.y = (int16)(g_location.y = IwRandMinMax(g_minY, g_maxY));

		g_iAnimationCount = 0;
	}
	void HandleStatusUpdate(const char * Result, uint32 ResultLen)
	{
		CIwFVec2* pRatio = IwGetMultiplayerHandler()->GetScaleRatio();
		FireflyObjectMessage networkMessage;

		IwAssertMsg("FF", sizeof(FireflyObjectMessage) == ResultLen, ("sizeof(FireflyObjectMessage) != ResultLen (%d - %d)", sizeof(FireflyObjectMessage), ResultLen)); 

		memcpy(&networkMessage, Result, sizeof(FireflyObjectMessage));
		PrepareReceive_FireflyObjectMessage(&networkMessage, &networkMessage);

		g_prevLocation.x = (int16)(networkMessage.prevLocationX / pRatio->x);
		g_prevLocation.y = (int16)(networkMessage.prevLocationY / pRatio->y);
		g_currLocation.x = (int16)(networkMessage.locationX / pRatio->x);
		g_currLocation.y = (int16)(networkMessage.locationY / pRatio->y);
		g_iAnimationCount = networkMessage.animIteration;
		
		float animPercent = (g_recalculateIterations - g_iAnimationCount) / (float)g_recalculateIterations;
		g_location.x = (int16)((g_currLocation.x) - (g_currLocation.x - g_prevLocation.x) * animPercent);
		g_location.y = (int16)((g_currLocation.y) - (g_currLocation.y - g_prevLocation.y) * animPercent);

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

		FireflyObjectMessage message;
		message.locationX = (int32)(g_currLocation.x * pRatio->x);
		message.locationY = (int32)(g_currLocation.y * pRatio->y);
		message.prevLocationX = (int32)(g_prevLocation.x * pRatio->x);
		message.prevLocationY = (int32)(g_prevLocation.y * pRatio->y);
		message.animIteration = g_iAnimationCount;
		message.active = g_bIsActive;

		FireflyObjectMessage networkMessage;
		PrepareSend_FireflyObjectMessage(message, networkMessage);

		_SendStatusUpdate(&networkMessage, sizeof(FireflyObjectMessage), forceSend);
	}
	bool Update(bool* pbVictory)
	{
		int iter = (g_iAnimationCount % g_recalculateIterations);

		if (iter == 0)
		{
			g_iAnimationCount = 0;

			g_prevLocation.x = g_currLocation.x;
			g_prevLocation.y = g_currLocation.y;

			if (IwGetMultiplayerHandler()->IsMaster())
			{
				int xMove = IwRandMinMax(-g_moveVelocity.x, g_moveVelocity.x);
				int yMove = IwRandMinMax(-g_moveVelocity.y, g_moveVelocity.y);

				g_currLocation.x = (int16)MAX(MIN(g_location.x + xMove, g_maxX), g_minX);
				g_currLocation.y = (int16)MAX(MIN(g_location.y + yMove, g_maxY), g_minY);
			}
		}
		else
		{
			float animPercent = (g_recalculateIterations - g_iAnimationCount) / (float)g_recalculateIterations;

			g_location.x = (int16)((g_currLocation.x) - (g_currLocation.x - g_prevLocation.x) * animPercent);
			g_location.y = (int16)((g_currLocation.y) - (g_currLocation.y - g_prevLocation.y) * animPercent);
		}
		g_iAnimationCount++;

		return false;
	}
private:
	int g_recalculateIterations, g_iAnimationCount, g_maxX, g_minX, g_maxY, g_minY;
	CIwVec2 g_moveVelocity;
	CIwSVec2 g_currLocation, g_prevLocation;
};

class CMine : public IGameObject
{
public:
	CMine(IGameHandler* pGameState, CIwTexture* pTexture) : IGameObject(pGameState)
	{
		g_bDeleteImage = false;
		g_Image = pTexture;
		g_iTop = 0;
	}

	void SetTopBound(int iTop)
	{
		g_iTop = iTop;
	}

	inline bool ShouldRender()
	{
		return (g_bIsActive || (g_iCaptureCount > 0));
	}

	bool IsActive()
	{
		return g_bIsActive;
	}

	void MakeCapture(CIwVec2& endLocation)
	{
		if (g_iCaptureCount <= 0)
		{
			SetActive(false);
			g_iCaptureCount = 1;
		}
	}

	bool Update(bool* pbVictory)
	{
		if (g_iCaptureCount > 0)
		{
			static int captureIterations = 10;
			if (g_iCaptureCount == captureIterations)
			{
				g_iCaptureCount = 0;
			}
			else
			{
				float animPercent = 1 + (5 * g_iCaptureCount / (float)captureIterations);
				g_fAlpha = 255 * (1 - (g_iCaptureCount / (float)captureIterations));


				float fScale = Utils::GetImageScalingFactor();

				g_width = g_Image->GetWidth() * animPercent * fScale;
				g_height = g_Image->GetHeight() * animPercent * fScale;

				g_iCaptureCount++;
			}
			return false;
		}
		else
		{
			return IGameObject::Update(pbVictory);
		}
	}

	void Reset()
	{
		g_fAlpha = 255;
		g_width = g_Image->GetWidth();
		g_height = g_Image->GetHeight();

		float fScale = Utils::GetImageScalingFactor();
		g_width = fScale * g_width;
		g_height = fScale * g_height;

		g_location.x = IwRandMinMax(g_width / 2, g_surfaceWidth - g_width / 2);
		g_location.y = MAX(g_iTop, IwRandMinMax(g_height / 2, g_surfaceHeight - g_height / 2));

		g_iCaptureCount = 0;
	}
private:
	int g_iTop;
	int g_iCaptureCount;
};

class CIntroFirefly : public CFirefly
{
public:
	CIntroFirefly(IGameHandler* pGameState, CIwTexture* pTexture, int quad) : CFirefly(pGameState, pTexture, pTexture, pTexture)
	{
		g_quadrant = quad;
	}

	virtual void SetDifficulty(CIwFVec2& moveMetersPerCycle, float glowCycleSecondDuration)
	{
		CIwFVec2 moveVelo = g_pGameState->GetScaler()->GetSpeedInPixels(moveMetersPerCycle);
		g_glowIterations = (int)(glowCycleSecondDuration * g_pGameState->GetFramesPerSecond());
		g_glowStepIterations = g_glowIterations / 3;

		g_moveVelocity.x = moveVelo.x * glowCycleSecondDuration / 3;
		g_moveVelocity.y = moveVelo.y * glowCycleSecondDuration / 3;

		Reset();
	}
	inline bool IsVisible()
	{
		return false;
	}
protected :
	virtual inline void BoundCoordinates(CIwSVec2& coords)
	{
		int maxX, minX, maxY, minY;

		if (g_quadrant % 2 == 0)
		{
			minX = 0;
			maxX = g_surfaceWidth / 2;
		}
		else
		{
			minX = g_surfaceWidth / 2;
			maxX = g_surfaceWidth;
		}
		if (g_quadrant / 2 == 0)
		{
			minY = 0;
			maxY = g_surfaceHeight / 2;
		}
		else
		{
			minY = g_surfaceHeight / 2;
			maxY = g_surfaceHeight;
		}

		coords.x = (int16)(MAX(minX, MIN(coords.x, maxX-(int)g_width)));
		coords.y = (int16)(MAX(minY, MIN(coords.y, maxY-(int)g_height)));
	}
	virtual inline double GetAlpha()
	{
		double fAlpha = 0;
		int step = (g_iAnimationCount /  g_glowStepIterations) % 3; 
		if (step == 0)
		{
			fAlpha = 0;
		}
		else if (step == 2)
		{
			int subStep = (g_iAnimationCount % g_glowStepIterations);
			fAlpha = 255 - (255 * subStep / (float)g_glowStepIterations);
		}
		else
		{
			int subStep = (g_iAnimationCount % g_glowStepIterations);
			fAlpha = (255 * subStep / (float)g_glowStepIterations);
		}
		return fAlpha;
	}
	int g_quadrant;
};