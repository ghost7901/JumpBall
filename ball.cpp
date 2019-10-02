#include <list>
#include "ball.h"
#include "ballobj.h"
#include "cell.h"
#include "explode.h"
#include "gst.h"
#include "gstanim.h"

extern GST *g_pGST;
extern gstAnimation	*g_timerball;
extern gstAnimation	*g_disposableball;
extern gstAnimation	*g_forwardball;
extern gstAnimation	*g_razorballh;
extern gstAnimation	*g_razorballv;
extern gstAnimation	*g_simpleball;
extern gstAnimation	*g_staticball;
extern int			g_cellSelect;
extern bool         g_bCross;

std::list<CBallBase*> g_ballList;
int g_ballClear;

void BallRelease()
{
	for (std::list<CBallBase*>::iterator it = g_ballList.begin(); it != g_ballList.end(); it++) delete (*it);
	g_ballList.clear();
	ExplodeClear();
	g_ballClear = 1;
}

void BallReset()
{
	BallRelease();
	g_cellSelect = -1;

	CBallBase *ball;
	for (int y = 0; y < 11; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (g_cellInfo[x + y * 12].nType == 0) continue;
			switch (g_cellInfo[x + y * 12].nType)
			{
			case CO_SIMPLEBALL:
				ball = new CBallBase;
				ball->SetAni(g_simpleball);
				break;
			case CO_DISPOSABLEBALL:
				ball = new CDisposableBall;
				ball->SetAni(g_disposableball);
				break;
			case CO_FORWARDBALL:
				ball = new CForwardBall;
				ball->SetAni(g_forwardball);
				break;
			case CO_STATICBALL:
				ball = new CStaticBall;
				ball->SetAni(g_staticball);
				break;
			case CO_TIMERBALL:
				ball = new CTimerBall;
				ball->SetAni(g_timerball);
				ball->SetValue(g_cellInfo[x + y * 12].nValue);
				break;
			case CO_RAZORBALLH:
				ball = new CRazorBallH;
				ball->SetAni(g_razorballh);
				ball->SetValue(g_cellInfo[x + y * 12].nValue);
				g_cellInfo[x + y * 12].nType = 0;
				g_ballClear++;
				break;
			case CO_RAZORBALLV:
				ball = new CRazorBallV;
				ball->SetAni(g_razorballv);
				ball->SetValue(g_cellInfo[x + y * 12].nValue);
				g_cellInfo[x + y * 12].nType = 0;
				g_ballClear++;
				break;
			}

			if (ball != 0)
			{
				ball->SetPos(x, y);
				g_ballList.push_back(ball);
			}
		}
	}
}

void BallUpdate(float dt)
{
	for (std::list<CBallBase*>::iterator it = g_ballList.begin(); it != g_ballList.end();)
	{
		if ((*it)->IsKill())
		{
			delete (*it);
			it = g_ballList.erase(it);
		}
		else
		{
			float mx,my;
			g_pGST->Input_GetMousePos(&mx, &my);
			(*it)->Update(dt, mx, my);
			++it;
		}
	}
}

void BallRender()
{
	if(g_bCross)
	{
		for (std::list<CBallBase*>::iterator it = g_ballList.begin(); it != g_ballList.end(); it++)
		{
			if ((*it)->IsActive()) (*it)->RenderCross();
		}
	}

	for (std::list<CBallBase*>::iterator it = g_ballList.begin(); it != g_ballList.end(); it++)
	{
		(*it)->Render();
	}
}

bool BallIsClear()
{
	return (g_ballList.size() <= g_ballClear) && ExplodeFinished();
}