#pragma once
#include "cell.h"
#include "gstanim.h"


class CBallBase
{
public:
	CBallBase()								{ m_bKill = false; m_ani = 0; m_bMove = false; m_direction = 0; m_ds = 0; }
	virtual ~CBallBase()					{ if (m_ani != 0) delete m_ani; }

	virtual void SetValue(int value)		{ m_value = value; }
	virtual void SetPos(int x, int y)		{ m_cx = x, m_cy = y; m_x = CELLX + x * 36 + 2; m_y = CELLY + y * 36 + 2; }
	virtual void SetAni(gstAnimation *ani);

	virtual bool IsKill()					{ return m_bKill; }
	virtual void Kill();

	virtual bool IsActive();
	virtual void Active(bool bActive);

	virtual void RenderCross();
	virtual bool TestPoint(int cx, int cy)	{ return (cx == m_cx && cy == m_cy); }

	virtual bool IsMove(int cx, int cy);
	virtual bool Moving(float dt);

	virtual void Update(float dt, float x, float y)	;
	virtual void Render();

protected:
	float	m_x;
	float	m_y;
	int		m_cx;
	int		m_cy;
	float	m_value;
	bool	m_bKill;
	bool	m_bMove;
	int		m_type;
	int		m_direction;
	float	m_ds;
	gstAnimation *m_ani;
};

//CDisposableBall
class CDisposableBall:public CBallBase
{
public:
	CDisposableBall():m_bMoved(false) {}

	void Update(float dt, float x, float y) { CBallBase::Update(dt, x, y); if (m_bMove) m_bMoved = true; if (m_bMove != m_bMoved) Kill(); }

private:
	bool m_bMoved;
};

//CForwardBall
class CForwardBall:public CBallBase
{
public:
	bool IsMove(int cx, int cy);
	void RenderCross();
	bool Moving(float dt);

private:
	float m_dx;
};

//CStaticBall
class CStaticBall:public CBallBase
{
public:
	void SetAni(gstAnimation *ani)			{ CBallBase::SetAni(ani); m_ani->SetMode(GSTANIM_NOLOOP|GSTANIM_NOPINGPONG); m_ani->Stop(); }
	void Update(float dt, float x, float y)	{ CBallBase::Update(dt, x, y); m_ani->Update(dt); }
	void Active(bool bActive);				
};

//CTimerBall
class CTimerBall:public CBallBase
{
public:
	void SetValue(int value);
	void Update(float dt, float x, float y)	{ CBallBase::Update(dt, x, y); m_value -= dt; if (m_value < 0) Kill(); }
	void Render();
};

//CRazorBallH
class CRazorBallH:public CBallBase
{
public:
	void SetValue(int value);
	void Update(float dt, float x, float y);

	void Kill()					{}
	void Active(bool bActive)	{}
	bool IsMove(int cx, int cy)	{ return false; }
	bool Moving(float dt)		{ return false; }
};

//CRazorBallV
class CRazorBallV:public CBallBase
{
public:
	void SetValue(int value);
	void Update(float dt, float x, float y);

	void Kill()					{}
	void Active(bool bActive)	{}
	bool IsMove(int cx, int cy)	{ return false; }
	bool Moving(float dt)		{ return false; }
};