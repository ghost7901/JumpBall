#pragma once
#include <gstguictrls.h>
#include <gstfont.h>

class CGUIButton:public gstGUIObject
{
public:
	CGUIButton(int _id,float x,float y,float width,float height,gstSprite *spr,gstFont *fnt,char *str);
	bool GetState() { bool bState=m_bClick; m_bClick=false; return bState; }
	virtual void Render();
	virtual void MouseOver(bool bOver) { m_bMouseOver=bOver; }
	virtual bool MouseLButton(bool bDown);

protected:
	gstFont *m_fnt;
	gstSprite *m_spr;
	char *m_str;
	bool m_bMouseOver;
	bool m_bClick;
};

class CGUISlider:public gstGUIObject
{
public:
	CGUISlider(int _id, float x, float y,
		HTEXTURE stex, float sx, float fy, float sw, float sh,
		HTEXTURE ttex, float tx, float ty, float tw, float th,
		gstFont *font = 0);
	virtual ~CGUISlider();

	void SetValue(float _fVal);
	float GetValue() { return m_fVal; }

	virtual void Render();
	virtual bool MouseMove(float x, float y);
	virtual bool MouseLButton(bool bDown);

private:
	bool m_bPressed;
	float m_fMin,m_fMax,m_fVal;
	gstSprite *m_sprSlider,*m_sprSliderThumb;
	gstFont *m_pFont;
};

#define MAX_STRING 16
#include <vector>
using namespace std;

struct st_Char
{
	bool bIME;
	char szChar[3];
};

class CGUIEdit:public gstGUIObject
{
public:
	CGUIEdit(int _id,float x,float y,float w,float h,gstFont *fnt,DWORD dwTextColor,DWORD dwFrameColor);
	~CGUIEdit(void);

	void SetAlign(int nAlign);
	char *GetText() { return m_szText; }
	void Clear() { strcpy(m_szText,""); }
	bool IsFocus() { return m_bFocus; }

	virtual void Render();
	virtual void Update(float dt);
	virtual void Focus(bool bFocused);
	virtual bool KeyClick(int key,int chr);

protected:
	bool m_bFocus;
	char m_szText[MAX_STRING];
	DWORD m_dwTextColor;
	DWORD m_dwFrameColor;
	float m_fTime;
	bool m_bBlink;
	int m_nCurPos;
	int m_nAlign;
	bool m_bComposition;
	gstFont *m_pFont;

	vector<st_Char *> m_pCharArray;

	bool AddCharEx(const char *szNewChar,int nPos=-1);
	bool AddChar(const char szNewChar,int nPos=-1);
	bool DelChar(int nPos);
	void ClearCharArray();
	void RedrawCursor();
	void UpdataString();

public:
	void StartComposition();
	void EndComposition();
	bool IsComposition() { return m_bComposition;}
	void OnImeCharMsg(const char *szNewChar);
};