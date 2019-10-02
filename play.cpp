#include "play.h"
#include "gui.h"
#include "cell.h"
#include "ball.h"
#include "explode.h"
#include "common.h"
#include <gst.h>
#include <gstsprite.h>
#include <gstanim.h>
#include <stdio.h>

extern gstSprite	*g_button;
extern gstSprite	*g_cell;
extern gstSprite	*g_helpdlg;
extern gstSprite	*g_levelbgspr;
extern gstSprite	*g_timebar;
extern gstSprite	*g_timebarfill;
extern HTEXTURE	    g_levelbacktex;
extern HTEXTURE	    g_levelnexttex;
extern gstAnimation	*g_timerball;
extern gstAnimation	*g_disposableball;
extern gstAnimation	*g_forwardball;
extern gstAnimation	*g_razorballh;
extern gstAnimation	*g_razorballv;
extern gstAnimation	*g_simpleball;
extern gstAnimation	*g_staticball;
extern gstAnimation	*g_explode;
extern gstFont		*g_font;
extern gstFont		*g_levelfont;
extern gstFont		*g_timerfont;
extern HEFFECT		g_levcomplsnd;
extern HEFFECT		g_readysnd;
extern HEFFECT		g_twsnd;
extern HEFFECT		g_lostsnd;
extern HEFFECT		g_music1;
extern HEFFECT		g_music2;
extern HCHANNEL     g_music1canl;
extern HCHANNEL     g_music2canl;
extern bool         g_bUseMusic;
extern bool         g_bUseSound;
extern BYTE         g_byMusicVolume;
extern BYTE         g_bySoundVolume;

extern int			g_cellSelect;
extern char			g_path[MAX_PATH];

#ifdef _DEBUG
#define GAME_TIME 600000
#else
#define GAME_TIME 60
#endif

enum PlayState
{
	PS_MENU,
	PS_PLAY,
	PS_GAMEOVER,
};

struct StageInfo
{
	int nStage;
	int nStageNum;
};

gstGUI			*g_playmenu1;
gstGUI			*g_playmenu2;
int				g_playstate = PS_MENU;
int				g_stageNum = 1;
StageInfo		g_stage;
bool			g_bPause = false;
float			g_gameTime;
bool			g_bTimeWarning;

void LoadLevel()
{
	char buf[256];
	sprintf(buf, "%s/levels/%d\.balls", g_path, g_stage.nStage);
	CellLoad(buf);
}

void PlayEnter()
{
	LoadLevel();

	g_playstate = PS_MENU;
	g_bPause	= false;
	g_bTimeWarning = false;
	g_cellSelect= -1;

	#ifdef _DEBUG
	g_playstate = PS_PLAY;
	g_gameTime = GAME_TIME;
	BallReset();
	#endif
}

bool PlayLoop()
{
	static bool bLLastPressed = false;
	switch (g_playstate)
	{
	case PS_MENU:
		g_playmenu1->Update(g_pGST->Timer_GetDelta());

		if (((CGUIButton*)(g_playmenu1->GetCtrl(1)))->GetState())
		{
			g_bTimeWarning = false;
			g_playstate = PS_PLAY;
			g_gameTime = GAME_TIME;
			if(g_bUseSound) g_pGST->Effect_PlayEx(g_readysnd,g_bySoundVolume);
			if(g_bUseMusic)
			{
				g_pGST->Channel_Stop(g_music1canl);
				g_music2canl = g_pGST->Effect_PlayEx(g_music2,g_byMusicVolume,0,1.0f,true);
			}
			LoadLevel();
			BallReset();
		}
		else if (((CGUIButton*)(g_playmenu1->GetCtrl(2)))->GetState()) return true;
		else if (!bLLastPressed && ((gstGUIButton*)(g_playmenu1->GetCtrl(3)))->GetState() && g_stage.nStage - 1 >= 1)
		{
			g_stage.nStage--;
			LoadLevel();
		}
		else if (!bLLastPressed && ((gstGUIButton*)(g_playmenu1->GetCtrl(4)))->GetState() && g_stage.nStage + 1 <= g_stage.nStageNum)
		{
			g_stage.nStage++;
			LoadLevel();
		}
		bLLastPressed = g_pGST->Input_GetKeyState(GSTK_LBUTTON);

		g_helpdlg->Render(144, 44);

		g_font->SetColor(ARGB(255, 149, 26, 5));
		g_font->Render(400, 57, GSTTEXT_CENTER, "Stage");

		g_levelbgspr->Render(300, 112);
		g_levelfont->SetColor(ARGB(255, 255, 205, 90));
		g_levelfont->printf(400, 128, GSTTEXT_CENTER, "%d / %d", g_stage.nStage, g_stageNum);

		int cx, cy;
		for (int y = 0; y < 11; y++)
		{
			for (int x = 0; x < 12; x++)
			{
				cx = CELLX * 1.5f + x * 18 + 16;
				cy = CELLY * 1.5f + y * 18 + 54;
				if (g_cellInfo[x + y * 12].bIsCell) g_cell->RenderEx(cx, cy, 0, 0.5f);
				switch (g_cellInfo[x + y * 12].nType)
				{
				case CO_SIMPLEBALL:
					g_simpleball->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				case CO_DISPOSABLEBALL:
					g_disposableball->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				case CO_FORWARDBALL:
					g_forwardball->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				case CO_STATICBALL:
					g_staticball->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				case CO_TIMERBALL:
					g_timerball->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				case CO_RAZORBALLH:
					g_razorballh->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				case CO_RAZORBALLV:
					g_razorballv->RenderEx(cx + 1, cy + 1, 0, 0.5f);
					break;
				}
			}
		}

		g_playmenu1->Render();
		break;
	case PS_PLAY:
		gstInputEvent ent;
		ZeroMemory(&ent,sizeof(gstInputEvent));
		if(g_pGST->Input_GetEvent(&ent) && ent.type==INPUT_KEYUP && ent.flags==GSTINP_CTRL && ent.chr&GSTK_T)
			g_gameTime=60;

		if (g_bPause)
		{
			g_levelfont->SetColor(ARGB(255, 149, 26, 5));
			g_levelfont->Render(400, 260, GSTTEXT_CENTER, "Game Pause\nPress any key to continue");
			if (g_pGST->Input_GetKey()) g_bPause = false;
			break;
		}

		g_gameTime -= g_pGST->Timer_GetDelta();
		if (g_gameTime <= 0)
		{
			if(g_bUseSound) g_pGST->Effect_PlayEx(g_lostsnd,g_bySoundVolume);
			g_playstate = PS_GAMEOVER;
			break;
		}

		if (BallIsClear())
		{
			g_playstate = PS_MENU;
			if (g_stage.nStage == g_stage.nStageNum && g_stage.nStageNum < g_stageNum) g_stage.nStageNum++;
			if (g_stage.nStage < g_stageNum) g_stage.nStage++;
			LoadLevel();
			BallReset();
			if(g_bUseSound) g_pGST->Effect_PlayEx(g_levcomplsnd,g_bySoundVolume);
			if(g_bUseMusic)
			{
				g_pGST->Channel_Stop(g_music2canl);
				g_music1canl=g_pGST->Effect_PlayEx(g_music1,g_byMusicVolume);
			}
			break;
		}

		g_playmenu2->Update(g_pGST->Timer_GetDelta());

		if (((CGUIButton*)(g_playmenu2->GetCtrl(1)))->GetState())
		{
			LoadLevel();
			BallReset();
			g_bTimeWarning = false;
			if(g_bUseSound) g_pGST->Effect_PlayEx(g_readysnd,g_bySoundVolume);
			g_gameTime = GAME_TIME;
		}
		else if (((CGUIButton*)(g_playmenu2->GetCtrl(2)))->GetState()) g_bPause = !g_bPause;
		else if (((CGUIButton*)(g_playmenu2->GetCtrl(3)))->GetState())
		{
			LoadLevel();
			BallReset();
			g_playstate = PS_MENU;
			if(g_bUseMusic)
			{
				g_pGST->Channel_Stop(g_music2canl);
				g_music1canl=g_pGST->Effect_PlayEx(g_music1,g_byMusicVolume);
			}
		}

		BallUpdate(g_pGST->Timer_GetDelta());
		ExplodeUpdate(g_pGST->Timer_GetDelta());

		g_playmenu2->Render();

		for (int y = 0; y < 11; y++)
			for (int x = 0; x < 12; x++)
				if (g_cellInfo[x + y * 12].bIsCell) g_cell->Render(CELLX + x * 36, CELLY + y * 36);

		BallRender();
		ExplodeRender();

		g_timebar->Render(628, 128);
		g_pGST->Gfx_RenderRectangle(628 + 30, 128 + 9 + 276 * ((GAME_TIME - g_gameTime) / GAME_TIME), 628 + 38, 128 + 285, 0x80ff0000);
		g_levelfont->SetColor(ARGB(255, 255, 205, 90));
		g_levelfont->printf(662, 426, GSTTEXT_CENTER, "%d", (int)g_gameTime);

		if (g_gameTime <= 10 && !g_bTimeWarning)
		{
			if(g_bUseSound) g_pGST->Effect_PlayEx(g_twsnd,g_bySoundVolume);
			g_bTimeWarning = true;
		}

		break;

	case PS_GAMEOVER:
		g_levelfont->SetColor(ARGB(255, 149, 26, 5));
		g_levelfont->Render(400, 256, GSTTEXT_CENTER, "Game time is over\nPress any key to restart");
		if (g_pGST->Input_GetKey())
		{
			LoadLevel();
			BallReset();
			g_gameTime = GAME_TIME;
			g_bTimeWarning = false;
			g_playstate = PS_PLAY;
			if(g_bUseSound) g_pGST->Effect_PlayEx(g_readysnd,g_bySoundVolume);
		}
		break;
	}

	return false;
}

bool StageInfoLoad()
{
	char buf[256];
	sprintf(buf, "%s/levels/levels.dat", g_path);
	FILE *fp = fopen(buf, "rb+");
	if (fp != 0) fread(&g_stage, sizeof(g_stage), 1, fp);
	else return false;

	fclose(fp);
	return true;
}

void StageInfoSave()
{
	char buf[256];
	sprintf(buf, "%s/levels/levels.dat", g_path);
	FILE *fp = fopen(buf, "wb+");
	if (fp != 0) fwrite(&g_stage, sizeof(g_stage), 1, fp);
	fclose(fp);
}

bool PlayInit()
{
	g_playmenu1 = new gstGUI();
	g_playmenu2 = new gstGUI();

	if (g_playmenu1 == 0 || g_playmenu2 == 0) return false;

	g_playmenu1->AddCtrl(new CGUIButton(1, 220, 480, 132, 36, g_button, g_font, "Start"));
	g_playmenu1->AddCtrl(new CGUIButton(2, 448, 480, 132, 36, g_button, g_font, "Return"));
	g_playmenu1->AddCtrl(new gstGUIButton(3, 250, 112, 60, 56, g_levelbacktex, 0, 0));
	g_playmenu1->AddCtrl(new gstGUIButton(4, 490, 112, 60, 56, g_levelnexttex, 0, 0));

	g_playmenu2->AddCtrl(new CGUIButton(1, 32, 380, 132, 36, g_button, g_font, "Reset"));
	g_playmenu2->AddCtrl(new CGUIButton(2, 32, 420, 132, 36, g_button, g_font, "Pause"));
	g_playmenu2->AddCtrl(new CGUIButton(3, 32, 460, 132, 36, g_button, g_font, "Return"));

	g_stageNum = g_pGST->Ini_GetInt("initiate", "stage", 1);

	if (!StageInfoLoad())
	{
		g_stage.nStage = 1;
		g_stage.nStageNum = 1;
		StageInfoSave();
	}

	#ifdef _DEBUG
	PlayEnter();
	#endif

	return true;
}

void PlayRelease()
{
	BallRelease();
	StageInfoSave();
	SAFE_DELETE(g_playmenu1);
	SAFE_DELETE(g_playmenu2);
}