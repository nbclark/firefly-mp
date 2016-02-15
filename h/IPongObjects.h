#pragma once
#include "IGameObject.h"

class CBall : public IGameObject
{
public:
	CBall(IGameHandler* pGameState) : IGameObject(pGameState)
	{
		CIwImage img;
		img.LoadFromFile("images/pong/ball.png");

		g_Image = new CIwTexture();
		g_Image->CopyFromImage(&img);
		g_Image->Upload();

		g_bRadialInsersection = false;

		Reset();
	}
	void PushPaddle(IGameObject* pPaddle)
	{
		g_paddles.push_back(pPaddle);
	}
	CIwFVec2* GetVelocity()
	{
		return &g_ballVelocity;
	}
	virtual void Reset()
	{
		g_location.x = MIN(g_surfaceWidth-(int)g_Image->GetWidth(), MAX(0, (rand() & 0xFF)));
		g_location.y = 20;
		g_width = (int)g_Image->GetWidth();
		g_height = (int)g_Image->GetHeight();
		g_ballPosition.x = (float)g_location.x;
		g_ballPosition.y = (float)g_location.y;

		// TODO, make these values scale based on field size
		g_ballVelocity.x = g_ballVelocity.y	 = 4.0;
	}
	virtual bool Update(bool* pbVictory)
	{
		//	move it around here a bit based on the velocity
		CIwFVec2 newLocation(g_ballPosition.x + g_ballVelocity.x, g_ballPosition.y + g_ballVelocity.y);

		// We have our new location. Check collisions
		if (newLocation.x < 0)
		{
			newLocation.x = 0;
			g_ballVelocity.x = -g_ballVelocity.x;
		}
		if (newLocation.x > (g_surfaceWidth-(int)g_Image->GetWidth()))
		{
			newLocation.x = (float)(g_surfaceWidth-(int)g_Image->GetWidth());
			g_ballVelocity.x = -g_ballVelocity.x;
		}
		if (newLocation.y < 0)
		{
			newLocation.y = 0;
			g_ballVelocity.y = -g_ballVelocity.y;

			*pbVictory = true;
			return true;
		}
		if (newLocation.y > (g_surfaceHeight-(int)g_Image->GetHeight()))
		{
			newLocation.y = (float)(g_surfaceHeight-(int)g_Image->GetHeight());
			g_ballVelocity.y = -g_ballVelocity.y;

			*pbVictory = false;
			return true;
		}

		g_ballPosition.x = newLocation.x;
		g_ballPosition.y = newLocation.y;

		g_location.x = (int16)g_ballPosition.x;
		g_location.y = (int16)g_ballPosition.y;

		std::list<IGameObject*>::iterator iter = g_paddles.begin();

		while (iter != g_paddles.end())
		{
			IGameObject* pPaddle = *iter;

			if (pPaddle->InsersectsWith(this))
			{
				CIwRect* pRect = pPaddle->GetBounds();
				// we always intersect in the Y direction
				if (g_ballVelocity.y > 0)
				{
					newLocation.y = (float)(pRect->y - (int)g_Image->GetHeight());
				}
				else
				{
					newLocation.y = (float)(pRect->y + pRect->h);
				}
				g_ballVelocity.y = -g_ballVelocity.y;
				break;
			}

			iter++;
		}

		g_ballPosition.x = newLocation.x;
		g_ballPosition.y = newLocation.y;

		g_location.x = (int16)g_ballPosition.x;
		g_location.y = (int16)g_ballPosition.y;

		return false;
	}
protected:
	CIwFVec2 g_ballPosition;
	CIwFVec2 g_ballVelocity;
	std::list<IGameObject*> g_paddles;
};

class CCompPaddle : public IGameObject
{
public:
	CCompPaddle(IGameHandler* pGameState, CBall* pBallObject, bool isTop) : IGameObject(pGameState)
	{
		g_ballObject = pBallObject;

		CIwImage img;
		img.LoadFromFile("images/pong/paddle.png");

		g_Image = new CIwTexture();
		g_Image->CopyFromImage(&img);
		g_Image->Upload();

		g_bIsTop = isTop;

		g_bRadialInsersection = false;

		Reset();
	}
	virtual void Reset()
	{
		g_location.x = (int16)((g_surfaceWidth - (int)g_Image->GetWidth()) / 2);
		g_location.y = (int16)((g_bIsTop) ? 0 : (g_surfaceHeight - (int)g_Image->GetHeight()));
		g_currLocation.x = (float)g_location.x;
		g_currLocation.y = (float)g_location.y;
		g_width = (int)g_Image->GetWidth();
		g_height = (int)g_Image->GetHeight();

		// TODO, make these values scale based on field size
		g_maxVelocity.x = g_maxVelocity.y = 2.0;
		g_bNeedsCalculation = true;
	}
	virtual bool Update(bool* pbVictory)
	{
		// move it around here a bit based on the ball trajectory
		CIwRect* pBallRect = g_ballObject->GetBounds();
		CIwFVec2* pBallVelocity = g_ballObject->GetVelocity();

		// if the ball is moving towards us
		// TODO: this assumes the computer is on top
		bool canCalculate = (g_location.y == 0) ? (pBallVelocity->y < 0 && pBallRect->y < (2*g_surfaceHeight / 3)) : (pBallVelocity->y > 0 && pBallRect->y > (1*g_surfaceHeight / 3));
		if (canCalculate)
		{
			if (g_bNeedsCalculation)
			{
				float slope = (pBallVelocity->y / pBallVelocity->x);
				float ballEndX = (pBallRect->x+pBallRect->w/2) - (pBallRect->y - (int)g_Image->GetHeight()) * slope;

				if (g_location.y != 0)
				{
					ballEndX = (pBallRect->x+pBallRect->w/2) + (g_surfaceHeight - (pBallRect->y+pBallRect->h) - (int)g_Image->GetHeight()) * slope;
				}

				// if ballEndX < 0 OR > Width, it will bounce
				while (ballEndX < 0 || ballEndX > (g_surfaceWidth - pBallRect->w))
				{
					if (ballEndX < 0)
					{
						ballEndX = -ballEndX;
					}
					else
					{
						ballEndX = (g_surfaceWidth - pBallRect->w) - (ballEndX - (g_surfaceWidth - pBallRect->w));
					}
				}

				// Center of the paddle
				ballEndX += (int)g_Image->GetWidth() / 2;

				g_destLocation.y = 0;
				g_destLocation.x = ballEndX;
				g_bNeedsCalculation = false;
			}

			float xDiff = g_destLocation.x - g_currLocation.x;
			float tickDist = MAX(MIN(g_maxVelocity.x, xDiff), -g_maxVelocity.x);

			g_currLocation.x += tickDist;
			g_location.x = (int16)g_currLocation.x;
		}
		else
		{
			g_bNeedsCalculation = true;
		}

		return false;
	}
	bool g_bNeedsCalculation;
	bool g_bIsTop;
	CBall* g_ballObject;
	CIwFVec2 g_currLocation;
	CIwFVec2 g_destLocation;
	CIwFVec2 g_maxVelocity;
};

class CGPSPaddle : public CGPSObject
{
public:
	CGPSPaddle(IGameHandler* pGameState) : CGPSObject(pGameState, (char*)"images/pong/paddle.png", true)
	{
		g_bRadialInsersection = false;
	}
};
