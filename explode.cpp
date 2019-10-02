#include <list>
#include "explode.h"
#include "gst.h"
#include "gstsprite.h"
#include "gstanim.h"

extern GST *g_pGST;
extern HTEXTURE g_explodetex;
extern HEFFECT g_explsnd;
extern bool g_bUseSound;
extern BYTE g_bySoundVolume;

class CExplode
{
public:
	CExplode(float x,float y,HTEXTURE tex):m_x(x),m_y(y),m_bKill(false)
	{
		m_ani=new gstAnimation(tex,42,32,0,0,64,64);
		m_ani->SetHotSpot(32,32);///
		m_ani->Play();
		m_ani->SetMode(GSTANIM_NOLOOP);
		if(g_bUseSound) g_pGST->Effect_PlayEx(g_explsnd,g_bySoundVolume);
	}
	virtual ~CExplode() { if(m_ani!=0) delete m_ani; }

	bool IsKill() { return m_bKill; }
	void Kill() { m_bKill=true; }

	void Update(float dt)
	{
		m_ani->Update(dt);
		if(!m_ani->IsPlaying()) Kill();
	}

	void Render()
	{
		m_ani->Render(m_x,m_y);
	}

protected:
	float m_x;
	float m_y;

	bool m_bKill;
	gstAnimation *m_ani;
};

std::list<CExplode *> g_explodeList;
void ExplodeClear()
{
	for(std::list<CExplode *>::iterator it=g_explodeList.begin(); it!=g_explodeList.end(); it++) delete (*it);
	g_explodeList.clear();
}

void ExplodeAdd(float x,float y)
{
	g_explodeList.push_back(new CExplode(x,y,g_explodetex));
}

void ExplodeUpdate(float dt)
{
	for(std::list<CExplode *>::iterator it=g_explodeList.begin(); it!=g_explodeList.end();)
	{
		if((*it)->IsKill())
		{
			delete (*it);
			it=g_explodeList.erase(it);
		}
		else
		{
			(*it)->Update(dt);
			++it;
		}
	}
}

void ExplodeRender()
{
	for(std::list<CExplode *>::iterator it=g_explodeList.begin(); it!=g_explodeList.end();it++)
	{
		if(!(*it)->IsKill()) (*it)->Render();
	}
}

bool ExplodeFinished()
{
	return (g_explodeList.size()==0);
}
