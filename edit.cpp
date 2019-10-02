#include <windows.h>
#include <commdlg.h>
#include "edit.h"
#include "gui.h"
#include "cell.h"
#include "common.h"
#include "gst.h"
#include "gstsprite.h"
#include <gstanim.h>
#include <gstrect.h>
#include <gstvector.h>
//#include <guiappedit.h>

#define BUTTON_NEW      6
#define BUTTON_LOAD     7
#define BUTTON_SAVE     8
#define BUTTON_RETURN   9
#define BUTTON_INPUTBOX 10

extern gstSprite *g_button;
extern gstFont *g_font;
extern gstSprite *g_cell;
extern gstAnimation *g_timerball;
extern gstAnimation *g_disposableball;
extern gstAnimation *g_forwardball;
extern gstAnimation *g_razorballh;
extern gstAnimation *g_razorballv;
extern gstAnimation *g_simpleball;
extern gstAnimation *g_staticball;
extern gstFont *g_timerfont;
extern int g_cellSelect;
extern int g_cellObject;

gstGUI *g_editmenu;
CGUIEdit *g_inputbox;
//GUIAppEdit *g_inputbox;

void EditEnter()
{
	g_cellSelect=-1;
	g_cellObject=-1;
	g_editmenu->EnableCtrl(BUTTON_INPUTBOX,false);
	g_editmenu->ShowCtrl(BUTTON_INPUTBOX,false);
	CellInit();
}

bool EditLoop()
{
	float mx,my;
	g_pGST->Input_GetMousePos(&mx,&my);

	g_editmenu->Update(g_pGST->Timer_GetDelta());
	if(((CGUIButton *)(g_editmenu->GetCtrl(BUTTON_NEW)))->GetState())
		CellInit();
	else if(((CGUIButton *)(g_editmenu->GetCtrl(BUTTON_LOAD)))->GetState())
	{
		char filename[MAX_PATH]="";
		OPENFILENAME ofn;
		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.lStructSize=sizeof(OPENFILENAME);
		ofn.hwndOwner=g_pGST->System_GetState(GST_HWND);
		ofn.lpstrFilter="*.balls\0*.balls\0\0";
		ofn.nMaxFile=MAX_PATH;
		ofn.nMaxFileTitle=MAX_PATH;
		ofn.lpstrDefExt="balls";
		ofn.lpstrFile=filename;

		if(GetOpenFileName(&ofn)) CellLoad(filename);
	}
	else if(((CGUIButton *)(g_editmenu->GetCtrl(BUTTON_SAVE)))->GetState())
	{
		char filename[MAX_PATH]="";
		OPENFILENAME ofn;
		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.lStructSize=sizeof(OPENFILENAME);
		ofn.hwndOwner=g_pGST->System_GetState(GST_HWND);
		ofn.lpstrFilter="*.balls\0*.balls\0\0";
		ofn.nMaxFile=MAX_PATH;
		ofn.lpstrDefExt="balls";
		ofn.lpstrFile=filename;

		if(GetSaveFileName(&ofn)) CellSave(filename);
	}
	else if(((CGUIButton *)(g_editmenu->GetCtrl(BUTTON_RETURN)))->GetState())
		return true;

	g_simpleball->Update(g_pGST->Timer_GetDelta());
	g_disposableball->Update(g_pGST->Timer_GetDelta());
	g_forwardball->Update(g_pGST->Timer_GetDelta());
	g_razorballh->Update(g_pGST->Timer_GetDelta());
	g_razorballv->Update(g_pGST->Timer_GetDelta());
	g_timerball->Update(g_pGST->Timer_GetDelta());
	g_staticball->Update(g_pGST->Timer_GetDelta());

	if(g_pGST->Input_GetKeyState(GSTK_LBUTTON) && mx>138 && mx<138+34)
	{
		if(my>CELLY && my<CELLY+40)
			g_cellObject=CO_CELL;
		else if(my>CELLY+40 && my<CELLY+80)
			g_cellObject=CO_SIMPLEBALL;
		else if(my>CELLY+80 && my<CELLY+120)
			g_cellObject=CO_DISPOSABLEBALL;
		else if(my>CELLY+120 && my<CELLY+160)
			g_cellObject=CO_FORWARDBALL;
		else if(my>CELLY+160 && my<CELLY+200)
			g_cellObject=CO_STATICBALL;
		else if(my>CELLY+200 && my<CELLY+240)
			g_cellObject=CO_TIMERBALL;
		else if(my>CELLY+240 && my<CELLY+280)
			g_cellObject=CO_RAZORBALLH;
		else if(my>CELLY+280 && my<CELLY+320)
			g_cellObject=CO_RAZORBALLV;
	}

	g_cell->Render(138,CELLY);
	g_simpleball->Render(138,CELLY+40+2);
	g_disposableball->Render(138,CELLY+80+2);
	g_forwardball->Render(138,CELLY+120+2);
	g_staticball->Render(138,CELLY+160+2);
	g_timerball->Render(138,CELLY+200+2);
	g_razorballh->Render(138,CELLY+240+2);
	g_razorballv->Render(138,CELLY+280+2);
	if(g_cellObject>=0) g_pGST->Gfx_RenderRectangle(136,CELLY+g_cellObject*40,136+38,CELLY+g_cellObject*40+38,0x8ff00000);

	for(int i=0;i<=12;i++)
	{
		if(i<12) g_pGST->Gfx_RenderLine(CELLX,CELLY+i*36,CELLX+36*12,CELLY+i*36);
		g_pGST->Gfx_RenderLine(CELLX+i*36,CELLY,CELLX+i*36,CELLY+36*11);
	}

	for(int y=0;y<11;y++)
	{
		for(int x=0;x<12;x++)
		{
			if(g_cellInfo[x+y*12].bIsCell) g_cell->Render(CELLX+x*36,CELLY+y*36);
			switch(g_cellInfo[x+y*12].nType)
			{
			case CO_SIMPLEBALL:
				g_simpleball->Render(CELLX+x*36+2,CELLY+y*36+2);
				break;
			case CO_DISPOSABLEBALL:
				g_disposableball->Render(CELLX+x*36+2,CELLY+y*36+2);
				break;
			case CO_FORWARDBALL:
				g_forwardball->Render(CELLX+x*36+2,CELLY+y*36+2);
				break;
			case CO_STATICBALL:
				g_staticball->Render(CELLX+x*36+2,CELLY+y*36+2);
				break;
			case CO_TIMERBALL:
				if (g_cellSelect == x + y * 12)
				{
					if (g_inputbox->IsDone()) 
					{
						g_cellSelect = -1;
						g_cellInfo[x + y * 12].nValue = atoi(g_inputbox->GetText());
					}
				}
				g_timerball->Render(CELLX+x*36+2,CELLY+y*36+2);
				g_timerfont->printf(CELLX+x*36+18,CELLY+y*36+10,GSTTEXT_LEFT,"%d",g_cellInfo[x+y*12].nValue);
				break;
			case CO_RAZORBALLH:
				/////Wait Join
				g_razorballh->Render(CELLX+x*36+2,CELLY+y*36+2);
				g_timerfont->printf(CELLX+x*36+18,CELLY+y*36+10,GSTTEXT_LEFT,"%d",g_cellInfo[x+y*12].nValue);
				break;
			case CO_RAZORBALLV:
				///Wait Join
				g_razorballv->Render(CELLX+x*36+2,CELLY+y*36+2);
				g_timerfont->printf(CELLX+x*36+18,CELLY+y*36+10,GSTTEXT_LEFT,"%d",g_cellInfo[x+y*12].nValue);
				break;
			}
		}
	}

	g_editmenu->Render();

	if(g_cellObject>=0 && mx>CELLX && mx<CELLX+36*12 && my>CELLY && my<CELLY+36*11)
	{
		int row=(int)(mx-CELLX)/36;
		int col=(int)(my-CELLY)/36;
		mx=CELLX+row*36;
		my=CELLY+col*36;

		if(g_cellObject==CO_CELL)
		{
			if(g_pGST->Input_GetKeyState(GSTK_LBUTTON))
				g_cellInfo[row+col*12].bIsCell=true;
			else if(g_pGST->Input_GetKeyState(GSTK_RBUTTON))
			{
				g_cellInfo[row+col*12].bIsCell=false;
				g_cellInfo[row+col*12].nType=CO_CELL;
				g_cellInfo[row+col*12].nValue=0;
				if(g_cellSelect==row+col*12)
				{
					g_cellSelect=-1;
					g_editmenu->EnableCtrl(BUTTON_INPUTBOX,false);
					g_editmenu->ShowCtrl(BUTTON_INPUTBOX,false);
				}
			}
		}
		else if(g_cellInfo[row+col*12].bIsCell)
		{
			if(g_cellInfo[row+col*12].nType==CO_CELL)
			{
				if(g_pGST->Input_GetKeyState(GSTK_LBUTTON)) g_cellInfo[row+col*12].nType=g_cellObject;
			}
			else
			{
				if(g_pGST->Input_GetKeyState(GSTK_LBUTTON))
				{
					if(g_cellSelect!=row+col*12)
					{
						switch(g_cellInfo[row+col*12].nType)
						{
						case CO_TIMERBALL:
						case CO_RAZORBALLH:
						case CO_RAZORBALLV:
							g_cellSelect=row+col*12;
							g_inputbox->Clear();
//							g_inputbox->rect.MoveTo(CELLX+row*36+10,CELLY+col*36+10);
							g_editmenu->EnableCtrl(BUTTON_INPUTBOX,true);
							g_editmenu->ShowCtrl(BUTTON_INPUTBOX,true);
							g_editmenu->SetFocus(BUTTON_INPUTBOX);
							break;
						default:
							g_cellSelect=-1;
							g_editmenu->EnableCtrl(BUTTON_INPUTBOX,false);
							g_editmenu->ShowCtrl(BUTTON_INPUTBOX,false);
							break;
						}
					}
					else
					{
						g_cellSelect=-1;
						g_editmenu->EnableCtrl(BUTTON_INPUTBOX,false);
						g_editmenu->ShowCtrl(BUTTON_INPUTBOX,false);
					}
				}
				else if(g_pGST->Input_GetKeyState(GSTK_RBUTTON))
				{
					g_cellInfo[row+col*12].nType=CO_CELL;
					g_cellInfo[row+col*12].nValue=0;
					g_cellSelect=-1;
					g_editmenu->EnableCtrl(BUTTON_INPUTBOX,false);
					g_editmenu->ShowCtrl(BUTTON_INPUTBOX,false);
				}
			}
		}

		g_pGST->Gfx_RenderRectangle(mx,my,mx+36,my+36,0x800000ff);
	}

	return false;
}

bool EditInit()
{
	g_editmenu=new gstGUI();
	if(g_editmenu==0) return false;

	g_editmenu->AddCtrl(new CGUIButton(BUTTON_NEW,620,340,132,36,g_button,g_font,"New"));
	g_editmenu->AddCtrl(new CGUIButton(BUTTON_LOAD,620,340+40,132,36,g_button,g_font,"Load"));
	g_editmenu->AddCtrl(new CGUIButton(BUTTON_SAVE,620,340+80,132,36,g_button,g_font,"Save"));
	g_editmenu->AddCtrl(new CGUIButton(BUTTON_RETURN,620,340+120,132,36,g_button,g_font,"Return"));
	g_inputbox=new CGUIEdit(BUTTON_INPUTBOX,0,0,16,7,g_timerfont,0,0);
	g_editmenu->AddCtrl(g_inputbox);
	g_editmenu->EnableCtrl(BUTTON_INPUTBOX,false);
	g_editmenu->ShowCtrl(BUTTON_INPUTBOX,false);

	CellInit();
	return true;
}

void EditRelease()
{
	SAFE_DELETE(g_editmenu);
}