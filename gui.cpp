#include "gui.h"
#include "gst.h"

extern GST *g_pGST;
/*
** CGUIButton
*/
CGUIButton::CGUIButton(int _id,float x,float y,float width,float height,gstSprite *spr,gstFont *fnt,char *str)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;
	m_fnt=fnt;
	m_str=str;
	m_spr=spr;
	m_bMouseOver=false;
	m_bClick=false;
	
	rect.Set(x,y,x+width,y+height);
}

bool CGUIButton::MouseLButton(bool bDown)
{
	 m_bClick=!bDown;
	 return !bDown;
}

void CGUIButton::Render()
{
	if(m_bMouseOver)
	{
		m_spr->Render(rect.x1,rect.y1);
		m_fnt->SetColor(ARGB(255,255,68,30));
		m_fnt->Render(rect.x1+(rect.x2-rect.x1)/2,rect.y1+8,GSTTEXT_CENTER,m_str);
	}
	else
	{
		m_spr->Render(rect.x1,rect.y1);
		m_fnt->SetColor(ARGB(255,149,26,5));
		m_fnt->Render(rect.x1+(rect.x2-rect.x1)/2,rect.y1+8,GSTTEXT_CENTER,m_str);
	}
}

/*
** CGUISlider
*/
CGUISlider::CGUISlider(int _id,float x,float y,
					   HTEXTURE stex,float sx,float sy,float sw,float sh,
					   HTEXTURE ttex,float tx,float ty,float tw,float th,
					   gstFont *font)
{
	id = _id;
	bStatic = false;
	bVisible = true;
	bEnabled = true;
	m_bPressed = false;
	m_pFont = font;
	rect.Set(x,y-(th-sh)/2,x+sw-sx,y+sh-sy+(th-sh)/2);

	m_fMin = 0; m_fMax = 100; m_fVal = 50;
	m_sprSlider = new gstSprite(stex, sx, sy, sw, sh);
	m_sprSliderThumb = new gstSprite(ttex, tx, ty, tw, th);
}

CGUISlider::~CGUISlider()
{
	if(m_sprSlider) delete m_sprSlider;
	if(m_sprSliderThumb) delete m_sprSliderThumb;
}

void CGUISlider::SetValue(float _fVal)
{
	if(_fVal < m_fMin)
		m_fVal = m_fMin;
	else if(_fVal>m_fMax)
		m_fVal = m_fMax;
	else
		m_fVal = _fVal;
}

void CGUISlider::Render()
{	
	m_sprSlider->Render(rect.x1,rect.y1+(m_sprSliderThumb->GetHeight()-m_sprSlider->GetHeight())/2);
	
	float x,y;	
	x = rect.x1 + (m_fVal - m_fMin) / (m_fMax - m_fMin) * (rect.x2 - rect.x1 - m_sprSliderThumb->GetWidth());
	y = rect.y1;
	m_sprSliderThumb->Render(x, y);

	if(m_pFont)
	{
		x = rect.x1 + (m_fVal - m_fMin) / (m_fMax - m_fMin) * (rect.x2 - rect.x1 - m_sprSliderThumb->GetWidth()) +m_sprSliderThumb->GetWidth()/2;
		y = rect.y1 - m_pFont->GetHeight();
		m_pFont->SetColor(ARGB(255,237,158,54));
		m_pFont->printf(x, y, GSTTEXT_CENTER, "%d", (int)m_fVal);
	}
}

bool CGUISlider::MouseLButton(bool bDown)
{
	m_bPressed = bDown;
	
	return false;
}

bool CGUISlider::MouseMove(float x,float y)
{
	if(m_bPressed)
	{
		if(x > (rect.x2 - rect.x1 - m_sprSliderThumb->GetWidth()))
			x = rect.x2 - rect.x1 - m_sprSliderThumb->GetWidth();
		if(x < 0) x = 0;
		m_fVal = m_fMin +  x / (rect.x2 - rect.x1 - m_sprSliderThumb->GetWidth()) * (m_fMax - m_fMin);

		return true;
	}
	return false;
}

/*
** CGUISlider
*/
CGUIEdit::CGUIEdit(int _id,float x,float y,float w,float h,gstFont *fnt,DWORD dwTextColor,DWORD dwFrameColor)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;
	rect.Set(x,y,x+w,y+h);

	m_nCurPos=0;
	m_fTime=0;
	m_bFocus=false;
	m_bBlink=false;
	m_bComposition=false;
	m_dwTextColor=dwTextColor;
	m_dwFrameColor=dwFrameColor;
	m_pFont=fnt;

	m_nAlign=GSTTEXT_LEFT;
	strcpy(m_szText,"");
	ClearCharArray();
}

CGUIEdit::~CGUIEdit(void)
{
	ClearCharArray();
}

void CGUIEdit::ClearCharArray()
{
	size_t size=m_pCharArray.size();
	for(size_t i=0;i<size;i++)
	{
		if(m_pCharArray[i]) delete m_pCharArray[i];
	}
	m_pCharArray.clear();
}

void CGUIEdit::SetAlign(int nAlign)
{
	m_nAlign=nAlign;
}

void CGUIEdit::Render()
{
	//gst->Gfx_RenderLine(rect.x1-1, rect.y1-1, rect.x2+1, rect.y1-1, m_dwFrameColor);
	//gst->Gfx_RenderLine(rect.x2+1, rect.y1-1, rect.x2+1, rect.y2+1, m_dwFrameColor);
	//gst->Gfx_RenderLine(rect.x2+1, rect.y2+1, rect.x1-1, rect.y2+1, m_dwFrameColor);
	//gst->Gfx_RenderLine(rect.x1-1, rect.y2+1, rect.x1-1, rect.y1-1, m_dwFrameColor);
	gst->Gfx_RenderRectangle(rect.x1-1,rect.y1-1,rect.x2+1,rect.y2+1,m_dwFrameColor);

	DWORD dwLastColor=m_pFont->GetColor();
	m_pFont->SetColor(m_dwTextColor);
	float x,y;
	if(m_nAlign==GSTTEXT_LEFT)
	{
		x=rect.x1;
		y=rect.y1+2;
	}
	else if(m_nAlign=GSTTEXT_CENTER)
	{
		x=rect.x1+(rect.x2-rect.x1)/2;
		y=rect.y1+2;
	}
	else if(m_nAlign=GSTTEXT_RIGHT)
	{
		x=rect.x2;
		y=rect.y1+2;
	}
	m_pFont->Render(x,y,m_nAlign,m_szText);
	m_pFont->SetColor(dwLastColor);

	if(m_bFocus && m_bBlink) RedrawCursor();
}

void CGUIEdit::Update(float dt)
{
	if(m_bFocus)
	{
		m_fTime+=dt;
		if(m_fTime>0.3)
		{
			m_fTime=0;
			m_bBlink=!m_bBlink;
		}
	}
}

void CGUIEdit::Focus(bool bFocused)
{
	m_bFocus=bFocused;
}

bool CGUIEdit::KeyClick(int key,int chr)
{
	if(m_bFocus && !IsComposition())
	{
		int size=(int)m_pCharArray.size();		

		switch(key)
		{
		case GSTK_ENTER:
		case GSTK_TAB:
			Focus(false);
			break;
		case GSTK_HOME:
			m_nCurPos=0;
			break;
		case GSTK_END:
			m_nCurPos=size;
			break;
		case GSTK_LEFT:
			if(m_nCurPos>0) m_nCurPos--;
			break;
		case GSTK_RIGHT:
			if(m_nCurPos<size) m_nCurPos++;
			break;
		case GSTK_DELETE:
			DelChar(m_nCurPos);
			break;
		case GSTK_BACKSPACE:
			if(m_nCurPos==0) break;
			DelChar(--m_nCurPos);
			break;
		default:
			if(chr==0 || IsComposition() || size==MAX_STRING) break;
			AddChar(chr,m_nCurPos++);
			UpdataString();
			break;
		}

		m_bBlink=true;
		m_fTime=0;
	}
	return false;
}

void CGUIEdit::StartComposition()
{
	m_bComposition=true;
}

void CGUIEdit::EndComposition()
{
	m_bComposition=false;
	UpdataString();
}

void CGUIEdit::OnImeCharMsg(const char *szImeChar)
{
	if(m_bFocus) AddCharEx(szImeChar,m_nCurPos++);
}

bool CGUIEdit::AddCharEx(const char *szNewChar,int nPos)
{
	st_Char *_pChar=new st_Char;
	if(!_pChar) return false;

	if(strlen(szNewChar)>2 || strlen(szNewChar)==0) return false;

	_pChar->bIME=(strlen(szNewChar))==1 ? false : true;
	strcpy(_pChar->szChar,szNewChar);

	if(nPos<0)
	{
		m_pCharArray.push_back(_pChar);
	}
	else
	{
		size_t size=m_pCharArray.size();
		if(nPos>(int)size)
			AddCharEx(szNewChar);
		else
		{
			vector<st_Char *>::iterator it;
			it=m_pCharArray.begin()+nPos;//&m_pCharArray[nPos];//////
			m_pCharArray.insert(it,_pChar);
		}
	}

	return true;
}

bool CGUIEdit::AddChar(const char szNewChar,int nPos)
{
	char szBuf[3];

	szBuf[0]=szNewChar;
	szBuf[1]='\0';

	return AddCharEx(szBuf,nPos);
}

bool CGUIEdit::DelChar(int nPos)
{
	size_t size=m_pCharArray.size();
	if(nPos<(int)size && nPos>=0)
	{
		if(m_pCharArray[nPos])
		{
			vector<st_Char *>::iterator it;
			it=m_pCharArray.begin()+nPos;
			m_pCharArray.erase(it);
			UpdataString();
			return true;
		}
	}
	return false;
}

void CGUIEdit::RedrawCursor()
{
	if(IsComposition())	return;
	
	size_t size=m_pCharArray.size();
	if(size>=0)
	{
		char szChar[MAX_STRING+1];
		int i;
		for(i=0;i<m_nCurPos;i++) szChar[i]=m_szText[i];
		szChar[i]='\0';
		
		int nCurPos;
		if(m_nAlign==GSTTEXT_LEFT)
			nCurPos=m_pFont->GetStringWidth(szChar)+m_pFont->GetTracking()+m_pFont->GetSpacing();
		else if(m_nAlign=GSTTEXT_CENTER)
			nCurPos=(rect.x2-rect.x1)/2-m_pFont->GetStringWidth(m_szText)/2+m_pFont->GetStringWidth(szChar)+m_pFont->GetTracking()+m_pFont->GetSpacing();
		else if(m_nAlign==GSTTEXT_RIGHT)
			nCurPos=rect.x2-m_pFont->GetStringWidth(m_szText)+m_pFont->GetStringWidth(szChar)+m_pFont->GetTracking()+m_pFont->GetSpacing();

		gst->Gfx_RenderLine(rect.x1 + nCurPos, rect.y1 + 1, rect.x1 + nCurPos, rect.y2 - 1, m_dwTextColor);
	}
}

void CGUIEdit::UpdataString()
{
	size_t size=m_pCharArray.size();
	strcpy(m_szText,"");
	if(size>=0)
	{
		for(size_t i=0;i<size;i++)
		{
			st_Char *_pChar=m_pCharArray[i];
			if(_pChar) strcat(m_szText,_pChar->szChar);
		}
	}
}