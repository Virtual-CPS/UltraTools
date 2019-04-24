//===========================================================================
//    (c) Copyright 2010, AnHui GuoGuang & HFUT, all rights reserved
//===========================================================================
//    File           : GradPalDlg.CPP
//    $Header: $
//    Author         : Busy_fish@hotmail.com
//    Creation       : 2010.01.26
//    Remake         : 
//------------------------------- Description -------------------------------
//
//    实现对话框背景渐变类【CGradPalDlg】
//
//------------------------------ Modifications ------------------------------
//    $Log: $  
//===========================================================================


#include "stdafx.h"
#include "GradPalDlg.H"

CGradPalDlg::CGradPalDlg(UINT nIDTemplate, CWnd* pParentWnd) :
						CBCGPDialog(nIDTemplate, pParentWnd),
	m_nPaintSteps(236),		// the number of steps
	m_nPaintDir(GPD_BTOT),	// the direction
	m_nPaintRGB(GPC_BLUE)	// the color
{
	CreateGradPalette();
	VERIFY(m_brush=(HBRUSH)GetStockObject(HOLLOW_BRUSH));
	m_bFullScreenModeOn = FALSE;
	//EnableVisualManagerStyle();
}

BEGIN_MESSAGE_MAP(CGradPalDlg, CBCGPDialog)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CGradPalDlg::OnQueryNewPalette()
{
	CClientDC dc(this);
	
	CPalette *pPalOld = dc.SelectPalette(&m_Pal, FALSE);
	
	BOOL bRet = dc.RealizePalette();
	
	dc.SelectPalette(pPalOld, FALSE);
	
	if (bRet)
		// some colors changed
		this->Invalidate();
	
	return bRet;
}

void CGradPalDlg::OnPaletteChanged(CWnd *pFocusWnd)
{
	if (pFocusWnd != this)
		this->OnQueryNewPalette();
}

void CGradPalDlg::OnPaint()
{
	CPaintDC dc(this);

	CPalette *pPalOld = dc.SelectPalette(&m_Pal, FALSE);
	dc.RealizePalette();

	RECT rect;
	GetClientRect(&rect);
	PaintGradiantRect(&dc, rect);

	dc.SelectPalette(pPalOld, FALSE);
}

void CGradPalDlg::PaintGradiantRect(CDC *pDC, const RECT &rect) const
{
	ASSERT(pDC != NULL);
	ASSERT_KINDOF(CDC, pDC);

	CBCGPDrawManager dm (*pDC);
	dm.FillGradient2(rect, RGB(113, 175, 210), RGB(255, 255, 255), 270);
}

BOOL CGradPalDlg::CreateGradPalette()
{
	if (m_Pal.GetSafeHandle() != NULL)
		return FALSE;

	BOOL bSucc = FALSE;
	const int nNumColors = 236;
	LPLOGPALETTE lpPal = (LPLOGPALETTE)new BYTE[sizeof(LOGPALETTE) +
												sizeof(PALETTEENTRY) *
												nNumColors];

	if (lpPal != NULL)
	{
		lpPal->palVersion = 0x300;
		lpPal->palNumEntries = nNumColors;
		PALETTEENTRY *ppe = lpPal->palPalEntry;
		
		for (int nIndex = 0; nIndex < nNumColors; nIndex++)
		{
			const int nColor = ::MulDiv(nIndex, 255, nNumColors);

			ppe->peRed = (BYTE)(m_nPaintRGB & GPC_RED ? nColor : 0);
			ppe->peGreen = (BYTE)(m_nPaintRGB & GPC_GREEN ? nColor : 0);
			ppe->peBlue = (BYTE)(m_nPaintRGB & GPC_BLUE ? nColor : 0);
			ppe->peFlags = (BYTE)0;

			ppe++;
		}

		bSucc = m_Pal.CreatePalette(lpPal);
		delete [](PBYTE)lpPal;
	}

	return bSucc;
}

HBRUSH CGradPalDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// 更改 DC 的任何属性
	pDC->SetBkMode(TRANSPARENT);

	// 返回所需画笔
	return m_brush;
}

//***************************************************************************************
// 全屏幕显示
//
void CGradPalDlg::FullScreenModeOn()
{
	// remove the caption of the mainWnd:
	LONG style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style &= ~WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);

	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	// resize:
	SetWindowPos(NULL,0,0,screenX,screenY,SWP_NOZORDER);
	this->ShowWindow(SW_SHOWMAXIMIZED);
	//RecalcLayout();
}


BOOL CGradPalDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	if(m_bFullScreenModeOn)
		FullScreenModeOn();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
