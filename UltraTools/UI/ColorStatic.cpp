// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic()
{
	m_TextColor = RGB(0,0,0);
	m_bTextFontChanged = FALSE;
}

CColorStatic::~CColorStatic()
{
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers

int CColorStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}

void CColorStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);
	dc.SetBkColor(m_BackColor);
	dc.SetBkMode(TRANSPARENT);

	CFont *pOldFont;
	pOldFont = m_bTextFontChanged ? dc.SelectObject(&m_TextFont) :
									dc.SelectObject(GetParent()->GetFont());
	GetWindowText(m_strCaption);
	BOOL bIsEnabled = IsWindowEnabled();
	dc.SetTextColor(m_TextColor);

	//dc.DrawText(m_strCaption,&rect,DT_VCENTER|DT_CENTER);	// Busy Fish modify to the following lines 2006.07.28:
	// Center text
	CRect centerRect = rect;
	dc.DrawText(m_strCaption, -1, rect, DT_SINGLELINE|DT_CALCRECT);
	//rect.OffsetRect((centerRect.Width() - rect.Width())/2, (centerRect.Height() - rect.Height())/2);
	rect.OffsetRect(0, (centerRect.Height() - rect.Height())/2);
	dc.DrawState(rect.TopLeft(), rect.Size(), (LPCTSTR)m_strCaption, (bIsEnabled ? DSS_NORMAL : DSS_DISABLED), 
		TRUE, 0, (CBrush*)NULL);

	dc.SelectObject(pOldFont);

	// Do not call CStatic::OnPaint() for painting messages
}

void CColorStatic::Create(CString strCaption, COLORREF BackColor)
{
}

void CColorStatic::Create(LPCTSTR szLabel, DWORD dwStyle, const RECT &rect, CWnd *pParent)
{
	CStatic::Create(szLabel, dwStyle, rect, pParent);
}

void CColorStatic::SetCaption(CString strCaption)
{
	m_strCaption = strCaption;
}

void CColorStatic::SetBackColor(COLORREF BackColor)
{
	m_BackColor = BackColor;
}

void CColorStatic::SetTextColor(COLORREF TextColor)
{
	m_TextColor = TextColor;
}

void CColorStatic::SetTextFont(CONST LOGFONT* pLogFont)
{
	if (pLogFont != NULL)
	{
		m_bTextFontChanged = m_TextFont.CreateFontIndirect(pLogFont);
	}
}
