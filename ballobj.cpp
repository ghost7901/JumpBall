#include <list>
#include "ballobj.h"
#include "cell.h"
#include "explode.h"
#include <gst.h>
#include <gstfont.h>
#include <gstsprite.h>

extern GST *g_pGST;
extern gstFont *g_timerfont;
extern gstSprite *g_cross;
extern HEFFECT g_staticsnd;
extern int g_cellSelect;
extern std::list<CBallBase *> g_ballList;
extern bool g_bUseSound;
extern unsigned char g_bySoundVolume;
extern unsigned char g_byAnimSpeed;

enum Direction_State
{
	DS_NONE,
	DS_UP,
	DS_DOWN,
	DS_LEFT,
	DS_RIGHT,
};

void KillBall(int cx,int cy)
{
	for(std::list<CBallBase *>::iterator it=g_ballList.begin(); it!=g_ballList.end(); it++)
	{
		if((*it)->TestPoint(cx,cy)) (*it)->Kill();
	}
}

void CBallBase::SetAni(gstAnimation *ani)
{
	m_ani=new gstAnimation(ani->GetTexture(),ani->GetFrames(),g_byAnimSpeed,0,0,32,32);/////Wrong?
	m_ani->Play();
}

void CBallBase::Kill()
{
	m_bKill=true;
	g_cellInfo[m_cx+m_cy*12].nType=0;
	ExplodeAdd(m_x+17,m_y+17);
	if(IsActive()) Active(false);
}

bool CBallBase::IsActive()
{
	return (m_cx==g_cellSelect%12 && m_cy==g_cellSelect/12);
}

void CBallBase::Active(bool bActive)
{
	if(bActive)
	{
		if(IsActive())
			g_cellSelect=-1;
		else
			g_cellSelect=m_cx+m_cy*12;
	}
	else
		g_cellSelect=-1;
}

void CBallBase::Update(float dt,float x,float y)
{
	if(g_cellInfo[m_cx+m_cy*12].nType==-1)
	{
		Kill();
		return;
	}

	if(IsActive()) m_ani->Update(dt);

	if(g_pGST->Input_KeyDown(GSTK_LBUTTON))
	{		
		if(x>m_x && x<m_x+32 && y>m_y && y<m_y+32) Active(true);
		if(IsActive() && IsMove((int)((x-CELLX)/36),(int)((y-CELLY)/36))) m_bMove=true;
	}

	if(m_bMove && Moving(dt)) m_bMove=false;
}

void CBallBase::Render()
{
	switch(m_direction)
	{
	case DS_UP:
		m_ani->Render(m_x,m_y+m_ds);
		break;
	case DS_DOWN:
		m_ani->Render(m_x,m_y-m_ds);
		break;
	case DS_LEFT:
		m_ani->Render(m_x+m_ds,m_y);
		break;
	case DS_RIGHT:
		m_ani->Render(m_x,m_y);
		break;
	default:
		m_ani->Render(m_x,m_y);
		break;
	}
}

void CBallBase::RenderCross()
{
	if(m_cx > 1 && g_cellInfo[m_cx - 1 + 12 * m_cy].nType!=0 && g_cellInfo[m_cx - 2 + 12 * m_cy].nType==0 &&
		g_cellInfo[m_cx - 2 + 12 * m_cy].bIsCell) g_cross->Render(m_x - 72 - 1,m_y - 1);
	if (m_cx < 10 && g_cellInfo[m_cx + 1 + 12 * m_cy].nType != 0 && g_cellInfo[m_cx + 2 + 12 * m_cy].nType == 0  && 
		g_cellInfo[m_cx + 2 + 12 * m_cy].bIsCell) g_cross->Render(m_x + 72 - 1, m_y - 1);
	if (m_cy > 1 && g_cellInfo[m_cx + 12 * (m_cy - 1)].nType != 0 && g_cellInfo[m_cx + 12 * (m_cy - 2)].nType == 0 && 
		g_cellInfo[m_cx + 12 * (m_cy - 2)].bIsCell) g_cross->Render(m_x - 1, m_y - 72 - 1);
	if (m_cy < 9 && g_cellInfo[m_cx + 12 * (m_cy + 1)].nType != 0 && g_cellInfo[m_cx + 12 * (m_cy + 2)].nType == 0 && 
		g_cellInfo[m_cx + 12 * (m_cy + 2)].bIsCell) g_cross->Render(m_x - 1, m_y + 72 - 1);
}

bool CBallBase::IsMove(int cx,int cy)
{
	if(cx<0 || cx>11 || cy<0 || cy>10) return false;

	if (cy == m_cy)
	{
		if (m_cx < 10 && cx == m_cx + 2 && g_cellInfo[m_cx + 1 + 12 * m_cy].nType != 0 && 
			g_cellInfo[m_cx + 2 + 12 * m_cy].bIsCell && g_cellInfo[m_cx + 2 + 12 * m_cy].nType == 0)
		{
			m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
			g_cellInfo[m_cx + 1 + 12 * m_cy].nType = 0;
			g_cellInfo[m_cx + 12 * m_cy].nType = 0;
			KillBall(m_cx + 1, m_cy);
			SetPos(m_cx + 2, m_cy);
			m_direction = DS_RIGHT;
			m_ds = 72;
			return true;
		}
		else if (m_cx > 1 && cx == m_cx - 2 && g_cellInfo[m_cx - 1 + 12 * m_cy].nType != 0 && 
			g_cellInfo[m_cx - 2 + 12 * m_cy].bIsCell && g_cellInfo[m_cx - 2 + 12 * m_cy].nType == 0)
		{
			m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
			g_cellInfo[m_cx - 1 + 12 * m_cy].nType = 0;
			g_cellInfo[m_cx + 12 * m_cy].nType = 0;
			KillBall(m_cx - 1, m_cy);
			SetPos(m_cx - 2, m_cy);
			m_direction = DS_LEFT;
			m_ds = 72;
			return true;
		}
	}
	else if (cx == m_cx)
	{
		if (m_cy > 1 && cy == m_cy - 2 && g_cellInfo[m_cx + 12 * (m_cy - 1)].nType != 0 && 
			g_cellInfo[m_cx + 12 * (m_cy - 2)].bIsCell && g_cellInfo[m_cx + 12 * (m_cy - 2)].nType == 0)
		{
			m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
			g_cellInfo[m_cx + 12 * (m_cy - 1)].nType = 0;
			g_cellInfo[m_cx + 12 * m_cy].nType = 0;
			KillBall(m_cx, m_cy - 1);
			SetPos(m_cx, m_cy - 2);
			m_direction = DS_UP;
			m_ds = 72;
			return true;
		}
		if (m_cy < 9 && cy == m_cy + 2 && g_cellInfo[cx + 12 * (m_cy + 1)].nType != 0 && 
			g_cellInfo[m_cx + 12 * (m_cy + 2)].bIsCell && g_cellInfo[m_cx + 12 * (m_cy + 2)].nType == 0)
		{
			m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
			g_cellInfo[m_cx + 12 * (m_cy + 1)].nType = 0;
			g_cellInfo[m_cx + 12 * m_cy].nType = 0;
			KillBall(m_cx, m_cy + 1);
			SetPos(m_cx, m_cy + 2);
			m_direction = DS_DOWN;
			m_ds = 72;
			return true;
		}
	}

	return false;
}

bool CBallBase::Moving(float dt)
{
	m_ds -= dt * 10 * 72;

	if (m_ds < 0)
	{
		m_direction = DS_NONE;
		g_cellInfo[m_cx + 12 * m_cy].nType = m_type;
		Active(true);
		return true;
	}

	return false;
}

//==================================================================================
bool CForwardBall::IsMove(int cx, int cy)
{
	if (cx < 0 || cx > 11 || cy < 0 || cy > 10) return false;

	int i, n;
	if (cy == m_cy)
	{
		if (m_cx > 1 && cx < m_cx - 1)
		{
			for (i = m_cx - 1; i > cx; i--)
			{
				if (!g_cellInfo[i + 12 * m_cy].bIsCell) goto break1;
				if (g_cellInfo[i + 12 * m_cy].nType != 0)
				{
					for (n = i - 1; n >= cx; n--)
						if (!g_cellInfo[n + 12 * m_cy].bIsCell || g_cellInfo[n + 12 * m_cy].nType != 0) goto break1;

					m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
					g_cellInfo[m_cx + 12 * m_cy].nType = 0;

					m_ds = 36 * abs(cx - m_cx);
					m_dx = m_ds;
					g_cellInfo[i + 12 * m_cy].nType = 0;
					KillBall(i, m_cy);
					SetPos(cx, m_cy);
					m_direction = DS_LEFT;
					return true;
				}
			}
			break1:;
		}
		else if (m_cx < 10 && cx > m_cx + 1)
		{
			for (i = m_cx + 1; i < cx; i++)
			{
				if (!g_cellInfo[i + 12 * m_cy].bIsCell) goto break2;
				if (g_cellInfo[i + 12 * m_cy].nType != 0)
				{
					for (n = i + 1; n <= cx; n++)
						if (!g_cellInfo[n + 12 * m_cy].bIsCell || g_cellInfo[n + 12 * m_cy].nType != 0) goto break2;

					m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
					g_cellInfo[m_cx + 12 * m_cy].nType = 0;

					m_ds = 36 * abs(cx - m_cx);
					m_dx = m_ds;
					g_cellInfo[i + 12 * m_cy].nType = 0;
					KillBall(i, m_cy);
					SetPos(cx, m_cy);
					m_direction = DS_RIGHT;
					return true;
				}
			}
			break2:;
		}
	}
	else if (cx == m_cx)
	{
		if (m_cy > 1 && cy < m_cy - 1)
		{
			for (i = m_cy - 1; i > cy; i--)
			{
				if (!g_cellInfo[m_cx + 12 * i].bIsCell) goto break3;
				if (g_cellInfo[m_cx + 12 * i].nType != 0)
				{
					for (n = i - 1; n >= cy; n--)
						if (!g_cellInfo[m_cx + 12 * n].bIsCell || g_cellInfo[m_cx + 12 * n].nType != 0) goto break3;

					m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
					g_cellInfo[m_cx + 12 * m_cy].nType = 0;

					m_ds = 36 * abs(m_cy - cy);
					m_dx = m_ds;
					g_cellInfo[m_cx + 12 * i].nType = 0;
					KillBall(m_cx, i);
					SetPos(m_cx, cy);
					m_direction = DS_UP;
					return true;
				}
			}
			break3:;
		}
		else if (m_cy < 9 && cy > m_cy + 1)
		{
			for (i = m_cy + 1; i < cy; i++)
			{
				if (!g_cellInfo[m_cx + 12 * i].bIsCell) goto break4;
				if (g_cellInfo[m_cx + 12 * i].nType != 0)
				{
					for (n = i + 1; n <= cy; n++)
						if (!g_cellInfo[m_cx + 12 * n].bIsCell || g_cellInfo[m_cx + 12 * n].nType != 0) goto break4;

					m_type = g_cellInfo[m_cx + 12 * m_cy].nType;
					g_cellInfo[m_cx + 12 * m_cy].nType = 0;

					m_ds = 36 * abs(m_cy - cy);
					m_dx = m_ds;
					g_cellInfo[m_cx + 12 * i].nType = 0;
					KillBall(m_cx, i);
					SetPos(m_cx, cy);
					m_direction = DS_DOWN;
					return true;
				}
			}
			break4:;
		}
	}

	return false;
}

void CForwardBall::RenderCross()
{
	int i, n;
	if (m_cx > 1)
	{
		for (i = m_cx - 1; i > 0; i--)
		{
			if (!g_cellInfo[i + 12 * m_cy].bIsCell) goto break1;
			if (g_cellInfo[i + 12 * m_cy].nType != 0)
			{
				for (n = i - 1; n >= 0; n--)
				{
					if (!g_cellInfo[n + 12 * m_cy].bIsCell || g_cellInfo[n + 12 * m_cy].nType != 0) goto break1;
					if (g_cellInfo[n + 12 * m_cy].nType == 0) g_cross->Render(m_x - 36 * (m_cx - n) - 1, m_y - 1);
				}
			}
		}
		break1:;
	}
	if (m_cx < 10)
	{
		for (i = m_cx + 1; i < 11; i++)
		{
			if (!g_cellInfo[i + 12 * m_cy].bIsCell) goto break2;
			if (g_cellInfo[i + 12 * m_cy].nType != 0)
			{
				for (n = i + 1; n <= 11; n++)
				{
					if (!g_cellInfo[n + 12 * m_cy].bIsCell || g_cellInfo[n + 12 * m_cy].nType != 0) goto break2;
					if (g_cellInfo[n + 12 * m_cy].nType == 0) g_cross->Render(m_x + 36 * (n - m_cx) - 1, m_y - 1);
				}
			}
		}
		break2:;
	}
	if (m_cy > 1)
	{
		for (i = m_cy - 1; i > 0; i--)
		{
			if (!g_cellInfo[m_cx + 12 * i].bIsCell) goto break3;
			if (g_cellInfo[m_cx + 12 * i].nType != 0)
			{
				for (n = i - 1; n >= 0; n--)
				{
					if (!g_cellInfo[m_cx + 12 * n].bIsCell || g_cellInfo[m_cx + 12 * n].nType != 0) goto break3;
					if (g_cellInfo[m_cx + 12 * n].nType == 0) g_cross->Render(m_x - 1, m_y - 36 * (m_cy - n) - 1);
				}
			}
		}
		break3:;
	}
	if (m_cy < 9)
	{
		for (i = m_cy + 1; i < 10; i++)
		{
			if (!g_cellInfo[m_cx + 12 * i].bIsCell) goto break4;
			if (g_cellInfo[m_cx + 12 * i].nType != 0)
			{
				for (n = i + 1; n <= 10; n++)
				{
					if (!g_cellInfo[m_cx + 12 * n].bIsCell || g_cellInfo[m_cx + 12 * n].nType != 0) goto break4;
					if (g_cellInfo[m_cx + 12 * n].nType == 0) g_cross->Render(m_x - 1, m_y + 36 * (n - m_cy) - 1);
				}
			}
		}
		break4:;
	}
}

bool CForwardBall::Moving(float dt)
{
	m_ds -= dt * 10 * m_dx;

	if (m_ds < 0)
	{
		m_direction = DS_NONE;
		g_cellInfo[m_cx + 12 * m_cy].nType = m_type;
		Active(true);
		return true;
	}

	return false;
}

void CStaticBall::Active(bool bActive)
{ 
	CBallBase::Active(false); 
	m_ani->Play(); 
	if(g_bUseSound) g_pGST->Effect_PlayEx(g_staticsnd,g_bySoundVolume);
}

void CTimerBall::Render()
{
	CBallBase::Render();
	g_timerfont->printf(m_x + 17, m_y + 9, GSTTEXT_CENTER, "%d", (int)m_value);
}

void CTimerBall::SetValue(int value)
{
	m_value = value - g_pGST->Random_Float(0.0f, 0.8f);
}

void CRazorBallH::SetValue(int value)
{
	m_value = value;
	if (value == 0)
	{
		m_ani->SetMode(GSTANIM_REV|GSTANIM_LOOP);
		m_value = 72;
	}
	else
	{
		m_ani->SetMode(GSTANIM_FWD|GSTANIM_LOOP);
		m_value = -72;
	}
	m_ani->Play();
}

void CRazorBallH::Update(float dt, float x, float y)
{
	CBallBase::Update(dt, x, y);
	m_ani->Update(dt);

	m_x += m_value * dt;

	int cx = (int)(m_x - CELLX) / 36;
	if (m_value > 0)
	{
		if (!g_cellInfo[cx + 1 + m_cy * 12].bIsCell || m_x >= CELLX + 12 * 36 - 35)
		{
			m_value = -m_value;
			m_ani->SetMode(GSTANIM_FWD|GSTANIM_LOOP);
		}
		else KillBall(cx + 1, m_cy);
	}
	else
	{
		if (!g_cellInfo[cx + m_cy * 12].bIsCell || m_x <= CELLX)
		{
			m_value = -m_value;
			m_ani->SetMode(GSTANIM_REV|GSTANIM_LOOP);
		}
		else KillBall(cx, m_cy);
	}
}

void CRazorBallV::SetValue(int value)
{
	m_value = value;
	if (value == 0)
	{
		m_ani->SetMode(GSTANIM_FWD|GSTANIM_LOOP);
		m_value = 72;
	}
	else
	{
		m_ani->SetMode(GSTANIM_REV|GSTANIM_LOOP);
		m_value = -72;
	}
	m_ani->Play();
}

void CRazorBallV::Update(float dt, float x, float y)
{
	CBallBase::Update(dt, x, y);
	m_ani->Update(dt);

	m_y += m_value * dt;

	int cy = ((int)(m_y - CELLY)) / 36;
	if (m_value > 0)
	{
		if (!g_cellInfo[(cy + 1) * 12 + m_cx].bIsCell || m_y >= CELLY + 11 * 36 - 35)
		{
			m_value = -m_value;
			m_ani->SetMode(GSTANIM_REV|GSTANIM_LOOP);
		}
		else 
			KillBall(m_cx, cy + 1);
	}
	else
	{
		if (!g_cellInfo[cy * 12 + m_cx].bIsCell || m_y <= CELLY)
		{
			m_value = -m_value;
			m_ani->SetMode(GSTANIM_FWD|GSTANIM_LOOP);
		}
		else KillBall(m_cx, cy);
	}
}