#include "common.h"
#include "gamemain.h"
#include "resource.h"

GST *g_pGST=NULL;
extern bool g_bUseMusic;
extern bool g_bUseSound;
extern bool g_bFullScreen;
extern BYTE g_byMusicVolume;
extern BYTE g_bySoundVolume;

int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd)
{
	g_pGST=gstCreate(GST_VERSION);

	g_pGST->System_SetState(GST_INIFILE,"./config.ini");
	IniLoad();
	g_pGST->System_SetState(GST_FRAMEFUNC,GameLoop);
	g_pGST->System_SetState(GST_SCREENWIDTH,800);
	g_pGST->System_SetState(GST_SCREENHEIGHT,600);
	g_pGST->System_SetState(GST_HIDEMOUSE,true);
	g_pGST->System_SetState(GST_TITLE,"JumpBall 1.0");
	g_pGST->System_SetState(GST_LOGFILE,"game.log");
	g_pGST->System_SetState(GST_SHOWSPLASH,false);
	
	g_pGST->System_SetState(GST_WINDOWED,!g_bFullScreen);
	g_pGST->System_SetState(GST_ICON,MAKEINTRESOURCE(IDI_ICON1));
	g_pGST->System_SetState(GST_USESOUND,true);
	g_pGST->System_SetState(GST_FPS,g_pGST->Ini_GetInt("initiate","fps",100));
	g_pGST->System_SetState(GST_SCREENBPP,g_pGST->Ini_GetInt("initiate","bpp",32));

	if(g_pGST->System_Initiate() && GameInitiate())
	{
		g_pGST->System_Start();
	}

	IniSave();
	GameRelease();
	g_pGST->System_Shutdown();
	g_pGST->Release();

	return 1;
}