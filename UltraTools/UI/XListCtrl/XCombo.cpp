// XCombo.cpp : implementation file
//

#include "stdafx.h"
#include "XCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT WM_XCOMBOLIST_COMPLETE  = ::RegisterWindowMessage(_T("WM_XCOMBOLIST_COMPLETE"));
//UINT WM_XCOMBOLIST_VK_ESCAPE = ::RegisterWindowMessage(_T("WM_XCOMBOLIST_VK_ESCAPE"));

/////////////////////////////////////////////////////////////////////////////
// CXCombo

BEGIN_MESSAGE_MAP(CXCombo, CAdvComboBox)
	//{{AFX_MSG_MAP(CXCombo)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXCombo::CXCombo(CWnd * pParent) :
	m_pParent(pParent),
	CAdvComboBox(FALSE, pParent)
{
	TRACE0("in CXCombo::CXCombo\n");
}

CXCombo::~CXCombo()
{
	TRACE0("in CXCombo::~CXCombo\n");
}

///////////////////////////////////////////////////////////////////////////////
// SendRegisteredMessage
void CXCombo::SendRegisteredMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWnd *pWnd = m_pParent;
	if (pWnd)
		pWnd->SendMessage(nMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// CXCombo message handlers

void CXCombo::OnKillFocus(CWnd* pNewWnd) 
{
	TRACE0("in CXCombo::OnKillFocus\n");

	CAdvComboBox::OnKillFocus(pNewWnd);

	if (pNewWnd != GetDropWnd())
	{
		SendRegisteredMessage(WM_XCOMBOLIST_COMPLETE, 0, 0);
	}
}

void CXCombo::OnEscapeKey()
{
	TRACE0("in CXCombo::OnEscapeKey\n");

	SendRegisteredMessage(WM_XCOMBOLIST_VK_ESCAPE, 0, 0);
}

void CXCombo::OnComboComplete()
{
	TRACE0("in CXCombo::OnComboComplete\n");

	SendRegisteredMessage(WM_XCOMBOLIST_COMPLETE, 0, 0);
}
