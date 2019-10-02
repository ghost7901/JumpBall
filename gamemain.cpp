#include "gamemain.h"
#include "common.h"
#include "gui.h"
#include "play.h"
#include "edit.h"
#include <gstresource.h>
#include <gststrings.h>

#define BUTTON_PLAY       1
#define BUTTON_OPTION     2
#define BUTTON_EDIT       3
#define BUTTON_HELP       4
#define BUTTON_EXIT       5
#define BUTTON_RETURN     6
#define BUTTON_USEMUSIC   7
#define BUTTON_USESOUND   8
#define BUTTON_FULLSCREEN 9
#define BUTTON_CROSS      10

#define SLIDER_MUSICVOL   18
#define SLIDER_SOUNDVOL   19
#define SLIDER_ANIMSPEED  20

gstResourceManager *g_resmanager;
gstStringTable *g_strtable;
gstGUI *g_gamemenu;
gstGUI *g_optionmenu;
gstGUI *g_helpmenu;
gstSprite *g_bgspr;
gstSprite *g_loadingspr;
gstSprite *g_paper;
gstSprite *g_cursor;
gstSprite *g_clouds1;
gstSprite *g_clouds2;
gstSprite *g_helpdlg;
gstSprite *g_optiondlg;
gstSprite *g_button;
gstSprite *g_levelbgspr;
gstSprite *g_title;
gstSprite *g_cell;
gstSprite *g_cross;
gstSprite *g_timebar;
HTEXTURE g_levelbacktex;
HTEXTURE g_levelnexttex;
HTEXTURE g_explodetex;
HTEXTURE g_checkbox;
HTEXTURE g_slider;
HTEXTURE g_sliderthumb;
gstAnimation *g_timerball;
gstAnimation *g_disposableball;
gstAnimation *g_forwardball;
gstAnimation *g_razorballh;
gstAnimation *g_razorballv;
gstAnimation *g_simpleball;
gstAnimation *g_staticball;
HEFFECT  g_music1;
HEFFECT  g_music2;
HEFFECT  g_explsnd;
HEFFECT  g_levcomplsnd;
HEFFECT  g_readysnd;
HEFFECT  g_staticsnd;
HEFFECT  g_twsnd;
HEFFECT  g_lostsnd;
HCHANNEL g_music1canl;
HCHANNEL g_music2canl;
gstFont  *g_font;
gstFont  *g_levelfont;
gstFont  *g_timerfont;
bool g_bUseMusic;
bool g_bUseSound;
bool g_bFullScreen;
bool g_bCross;
unsigned char g_byMusicVolume;
unsigned char g_bySoundVolume;
unsigned char g_byAnimSpeed;

enum SYS_STATE
{
	SS_LOADING,
	SS_FADING,
	SS_NONE,
};

enum GAME_STATE
{
	GS_MENU,
	GS_PLAY,
	GS_OPTION,
	GS_HELP,
	GS_EDIT,
};

int g_sysstate=SS_LOADING;
int g_state=GS_MENU;
bool g_edit=false;

float g_cloudsdt=0;
float g_fadedt=1;

int g_cellSelect=-1;
int g_cellObject=-1;

char g_path[MAX_PATH];

bool GameLoop()
{
	#ifdef _DEBUG
	if(g_pGST->Input_GetKeyState(GSTK_ESCAPE)) return true;
	#endif

	if(g_pGST->Input_GetKeyState(GSTK_ALT) && g_pGST->Input_GetKeyState(GSTK_ENTER))
		g_pGST->System_SetState(GST_WINDOWED,!g_pGST->System_GetState(GST_WINDOWED));



	g_pGST->Gfx_BeginScene();
	g_bgspr->Render(0.0f,0.0f);

	if(g_sysstate==SS_LOADING)
	{
		g_loadingspr->Render(144.0f,550.0f);

		int progress=g_resmanager->CacheProcess();
		g_pGST->Gfx_RenderRectangle( 165.0f, 560.0f, 165.0f + 470.0f * progress / 100.0f, 572.0f, 0x80ff0000 );

		if(progress==100)
		{
			g_sysstate=SS_FADING;

			ResourceInit();
			if(g_bUseMusic) g_music1canl=g_pGST->Effect_PlayEx(g_music1,g_byMusicVolume,0,1.0f,true);

			if(!EditInit()) return true;
			if(!PlayInit()) return true;
		}
	}
	else
	{
		g_cloudsdt+=g_pGST->Timer_GetDelta();
		float cloudsdt1=g_cloudsdt*15;
		float cloudsdt2=g_cloudsdt*5;
		g_clouds1->SetTextureRect(cloudsdt1,0,256,256);
		g_clouds1->RenderStretch(0.0f,0.0f,800,600);
		g_clouds2->SetTextureRect(cloudsdt2,0,1024,1024);
		g_clouds2->RenderStretch(0.0f,0.0f,800,600);
		g_paper->Render(0.0f,0.0f);

		switch(g_state)
		{
		case GS_MENU:
			if(DoCommands(g_gamemenu->Update(g_pGST->Timer_GetDelta()))) return true;			

			g_title->Render(230,128);
			g_gamemenu->Render();
			break;
		case GS_PLAY:
			if(PlayLoop()) g_state=GS_MENU;
			break;
		case GS_OPTION:
			DoCommands(g_optionmenu->Update(g_pGST->Timer_GetDelta()));			

			g_optiondlg->Render(144,44);
			g_optionmenu->Render();

			g_font->SetColor(ARGB(255,149,26,5));
			g_font->Render(400,57,GSTTEXT_CENTER,"O p t i o n");
			
			g_simpleball->Update(g_pGST->Timer_GetDelta());
			g_simpleball->Render(441,383);

			break;
		case GS_EDIT:
			if(EditLoop()) g_state=GS_MENU;
			break;
		case GS_HELP:
			g_helpmenu->Update(g_pGST->Timer_GetDelta());
			if(((CGUIButton *)g_helpmenu->GetCtrl(BUTTON_RETURN))->GetState())
			{
				g_state=GS_MENU;
				g_simpleball->SetSpeed(32);
			}

			g_simpleball->Update(g_pGST->Timer_GetDelta());
			g_disposableball->Update(g_pGST->Timer_GetDelta());
			g_forwardball->Update(g_pGST->Timer_GetDelta());
			g_razorballh->Update(g_pGST->Timer_GetDelta());
			g_razorballv->Update(g_pGST->Timer_GetDelta());
			g_timerball->Update(g_pGST->Timer_GetDelta());
			g_staticball->Update(g_pGST->Timer_GetDelta());

			g_helpdlg->Render(144,44);
			g_helpmenu->Render();
			
			g_font->SetColor(ARGB(255,149,26,5));
			g_font->Render(400,57,GSTTEXT_CENTER,"H e l p");

			g_font->SetColor(ARGB(255,255, 205, 90));
			g_font->printf(400,85,GSTTEXT_CENTER,g_strtable->GetString("about"));

			g_pGST->Gfx_RenderLine(176,150,618,150,ARGB(255,255,205,90));

			g_simpleball->Render(176,155);
			g_disposableball->Render(176,203);
			g_forwardball->Render(176,251);
			g_staticball->Render(176,299);
			g_timerball->Render(176,347);
			g_razorballh->Render(176,395);

			g_font->printf(218,158,GSTTEXT_LEFT,g_strtable->GetString("help"));
			g_font->printf(176,436,GSTTEXT_LEFT,g_strtable->GetString("description"));

			break;
		}
	}

	float x,y;
	g_pGST->Input_GetMousePos(&x,&y);
	if(g_pGST->Input_IsMouseOver() && g_cursor) g_cursor->Render(x,y);

	if(g_sysstate==SS_FADING)
	{
		g_fadedt-=g_pGST->Timer_GetDelta();
		if(g_fadedt<=0)
			g_sysstate=SS_NONE;
		else
		{
			g_bgspr->SetColor(ARGB(255*g_fadedt,255,255,255));
			g_bgspr->Render(0.0f,0.0f);
			g_bgspr->SetColor(0xffffffff);
		}
	}

	#ifdef _DEBUG
	if(g_timerfont)
	{
	//	g_timerfont->SetColor(0xFFFFFFFF);
		g_timerfont->printf(10,5,GSTTEXT_LEFT,"%d",g_pGST->Timer_GetFPS());
		g_timerfont->printf(10,25,GSTTEXT_LEFT,"%f",g_pGST->Timer_GetDelta());
	}
	#endif

	g_pGST->Gfx_EndScene();

	return false;
}

bool GameInitiate()
{
	//g_pGST->Resource_AttachPack("data.dat");
	g_strtable=new gstStringTable("data/string.str");
	g_resmanager=new gstResourceManager("data/resource.ini");
	if(g_resmanager)
	{
		g_bgspr=g_resmanager->GetSprite("bgspr");
		g_loadingspr=g_resmanager->GetSprite("loadingspr");
		if(g_bgspr==0 || g_loadingspr==0) return false;

		//g_resmanager->Precache();
		g_resmanager->CacheBegin();

		GetCurrentDirectory(MAX_PATH,g_path);
		return true;
	}
	
	return false;
}

void GameRelease()
{
	EditRelease();
	PlayRelease();
	SAFE_DELETE(g_strtable);
	SAFE_DELETE(g_gamemenu);
	SAFE_DELETE(g_optionmenu);
	SAFE_DELETE(g_helpmenu);
	SAFE_DELETE(g_resmanager);
}

bool DoCommands(int id)
{
	switch(id)
	{
	case BUTTON_PLAY:
		g_state=GS_PLAY;
		PlayEnter();
		break;
	case BUTTON_OPTION:
		g_state=GS_OPTION;
		break;
	case BUTTON_EDIT:
		g_state=GS_EDIT;
		EditEnter();
		break;
	case BUTTON_HELP:
		g_state=GS_HELP;
		break;
	case BUTTON_EXIT:
		return true;


	case BUTTON_RETURN:
		g_state = GS_MENU;
		break;
	case BUTTON_USEMUSIC:
		g_bUseMusic = ((gstGUIButton *)g_optionmenu->GetCtrl(BUTTON_USEMUSIC))->GetState();
		if(g_bUseMusic)
		{
			g_music1canl = g_pGST->Effect_PlayEx(g_music1,g_byMusicVolume,0,1.0f,true);			
		}
		else
		{
			g_pGST->Channel_StopAll();
		}
		g_optionmenu->GetCtrl(SLIDER_MUSICVOL)->bEnabled = g_bUseMusic;
		break;
	case BUTTON_USESOUND:
		g_bUseSound = ((gstGUIButton *)g_optionmenu->GetCtrl(BUTTON_USESOUND))->GetState();
		g_optionmenu->GetCtrl(SLIDER_SOUNDVOL)->bEnabled = g_bUseSound;
		break;
	case BUTTON_FULLSCREEN:
		g_bFullScreen = ((gstGUIButton *)g_optionmenu->GetCtrl(BUTTON_FULLSCREEN))->GetState();
		break;
	case BUTTON_CROSS:
		g_bCross = ((gstGUIButton *)g_optionmenu->GetCtrl(BUTTON_CROSS))->GetState();
		break;

	case SLIDER_MUSICVOL:
		g_byMusicVolume = ((CGUISlider *)g_optionmenu->GetCtrl(SLIDER_MUSICVOL))->GetValue();
		g_pGST->Channel_SetVolume(g_music1canl,g_byMusicVolume);
		break;
	case SLIDER_SOUNDVOL:
		g_bySoundVolume = ((CGUISlider *)g_optionmenu->GetCtrl(SLIDER_SOUNDVOL))->GetValue();
		g_pGST->Effect_PlayEx(g_readysnd,g_bySoundVolume);
		break;
	case SLIDER_ANIMSPEED:
		g_byAnimSpeed = ((CGUISlider *)g_optionmenu->GetCtrl(SLIDER_ANIMSPEED))->GetValue();
		g_simpleball->SetSpeed(g_byAnimSpeed);
		break;
	}

	return false;
}

bool IniLoad()
{
	g_bFullScreen = g_pGST->Ini_GetInt("initiate","fullscreen",1) == 1 ? true : false;
	g_bUseMusic = g_pGST->Ini_GetInt("initiate","usemusic",1) == 1 ? true : false;
	g_bUseSound = g_pGST->Ini_GetInt("initiate","usesound",1) == 1 ? true : false;
	g_byMusicVolume = g_pGST->Ini_GetInt("initiate","musicvolume",100);
	g_bySoundVolume = g_pGST->Ini_GetInt("initiate","soundvolume",100);
	g_bCross = g_pGST->Ini_GetInt("initiate","cross",1) == 1 ? true : false;
	g_byAnimSpeed = g_pGST->Ini_GetInt("initiate","animspeed",32);

	return true;
}

bool IniSave()
{
	g_pGST->Ini_SetInt("initiate","fullscreen",g_bFullScreen);
	g_pGST->Ini_SetInt("initiate","usemusic",g_bUseMusic);
	g_pGST->Ini_SetInt("initiate","usesound",g_bUseSound);
	g_pGST->Ini_SetInt("initiate","musicvolume",g_byMusicVolume);
	g_pGST->Ini_SetInt("initiate","soundvolume",g_bySoundVolume);
	g_pGST->Ini_SetInt("initiate","cross",g_bCross);
	g_pGST->Ini_SetInt("initiate","animspeed",g_byAnimSpeed);

	return true;
}

bool ResourceInit()
{
	g_paper=g_resmanager->GetSprite("paperspr");
	g_cursor=g_resmanager->GetSprite("cursorspr");
	g_clouds1=g_resmanager->GetSprite("clouds1spr");
	g_clouds2=g_resmanager->GetSprite("clouds2spr");
	g_helpdlg=g_resmanager->GetSprite("helpdlgspr");
	g_optiondlg=g_resmanager->GetSprite("optiondlgspr");
	g_button=g_resmanager->GetSprite("buttonspr");
	g_title=g_resmanager->GetSprite("titlespr");
	g_cell=g_resmanager->GetSprite("cellspr");
	g_cross=g_resmanager->GetSprite("crossspr");
	g_timebar=g_resmanager->GetSprite("timebarspr");
	g_levelbgspr=g_resmanager->GetSprite("levelbgspr");
	g_cross->SetColor(0x80ff0000);

	g_levelbacktex=g_resmanager->GetTexture("levelbacktex");
	g_levelnexttex=g_resmanager->GetTexture("levelnexttex");
	g_checkbox=g_resmanager->GetTexture("checkbox");
	g_slider=g_resmanager->GetTexture("slider");
	g_sliderthumb=g_resmanager->GetTexture("sliderthumb");
	g_explodetex=g_resmanager->GetTexture("explodetex");
	
	g_timerball=g_resmanager->GetAnimation("timerball");
	g_disposableball=g_resmanager->GetAnimation("disposableball");
	g_forwardball=g_resmanager->GetAnimation("forwardball");
	g_razorballh=g_resmanager->GetAnimation("razorballh");
	g_razorballv=g_resmanager->GetAnimation("razorballv");
	g_simpleball=g_resmanager->GetAnimation("simpleball");
	g_staticball=g_resmanager->GetAnimation("staticball");


	g_timerball->Play();
	g_disposableball->Play();
	g_forwardball->Play();
	g_razorballh->Play();
	g_razorballv->Play();
	g_simpleball->Play();
	g_staticball->Play();

	g_font=g_resmanager->GetFont("gfont");
	g_levelfont=g_resmanager->GetFont("levelfont");
	g_timerfont=g_resmanager->GetFont("timerfont");
	g_timerfont->SetColor(ARGB(255,255,205,90));

	g_music1=g_resmanager->GetEffect("music1");
	g_music2=g_resmanager->GetEffect("music2");

	g_explsnd=g_resmanager->GetEffect("expl");
	g_levcomplsnd=g_resmanager->GetEffect("levcompl");
	g_readysnd=g_resmanager->GetEffect("ready");
	g_staticsnd=g_resmanager->GetEffect("static");
	g_twsnd=g_resmanager->GetEffect("timewarning");
	g_lostsnd=g_resmanager->GetEffect("lost");

	g_gamemenu=new gstGUI();
	g_gamemenu->AddCtrl(new CGUIButton(BUTTON_PLAY,334,270,132,36,g_button,g_font,"START"));
	g_gamemenu->AddCtrl(new CGUIButton(BUTTON_OPTION,334,315,132,36,g_button,g_font,"OPTIONS"));
	g_gamemenu->AddCtrl(new CGUIButton(BUTTON_EDIT,334,360,132,36,g_button,g_font,"EDIT"));
	g_gamemenu->AddCtrl(new CGUIButton(BUTTON_HELP,334,405,132,36,g_button,g_font,"HELP"));
	g_gamemenu->AddCtrl(new CGUIButton(BUTTON_EXIT,334,450,132,36,g_button,g_font,"EXIT"));

	gstGUIButton *button;
	CGUISlider *slider;

	g_optionmenu=new gstGUI();

	g_optionmenu->AddCtrl(new CGUIButton(BUTTON_RETURN,334,500,132,36,g_button,g_font,"OK"));

	button=new gstGUIButton(BUTTON_USEMUSIC,187,115,40,40,g_checkbox,0,0);
	button->SetMode(true);
	button->SetState(g_bUseMusic);			
	g_optionmenu->AddCtrl(button);

	button=new gstGUIButton(BUTTON_USESOUND,187,216,40,40,g_checkbox,0,0);
	button->SetMode(true);
	button->SetState(g_bUseSound);
	g_optionmenu->AddCtrl(button);

	button=new gstGUIButton(BUTTON_FULLSCREEN,187,317,40,40,g_checkbox,0,0);
	button->SetMode(true);
	button->SetState(g_bFullScreen);
	g_optionmenu->AddCtrl(button);

	button=new gstGUIButton(BUTTON_CROSS,187,418,40,40,g_checkbox,0,0);
	button->SetMode(true);
	button->SetState(g_bCross);
	g_optionmenu->AddCtrl(button);


	slider=new CGUISlider(SLIDER_MUSICVOL,250,178,g_slider,0,0,256,23,g_sliderthumb,0,0,29,35,g_font);
	slider->SetValue(g_byMusicVolume);
	g_optionmenu->AddCtrl(slider);

	slider=new CGUISlider(SLIDER_SOUNDVOL,250,279,g_slider,0,0,256,23,g_sliderthumb,0,0,29,35,g_font);
	slider->SetValue(g_bySoundVolume);
	g_optionmenu->AddCtrl(slider);

	slider=new CGUISlider(SLIDER_ANIMSPEED,330,423,g_slider,0,0,256,23,g_sliderthumb,0,0,29,35);
	slider->SetValue(g_byAnimSpeed);
	g_optionmenu->AddCtrl(slider);


	g_helpmenu=new gstGUI();
	g_helpmenu->AddCtrl(new CGUIButton(BUTTON_RETURN,334,512,132,36,g_button,g_font,"Return"));

	return true;
}