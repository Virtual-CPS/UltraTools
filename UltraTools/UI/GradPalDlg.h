//===========================================================================
//    (c) Copyright 2010, AnHui GuoGuang & HFUT, all rights reserved
//===========================================================================
//    File           : GradPalDlg.h
//    $Header: $
//    Author         : Busy_fish@hotmail.com
//    Creation       : 2010.01.26
//    Remake         : 
//------------------------------- Description -------------------------------
//
//    定义对话框背景渐变类【CGradPalDlg】
//
//------------------------------ Modifications ------------------------------
//    $Log: $  
//===========================================================================


#ifndef GRADPALDLG_H_
#define GRADPALDLG_H_

// m_nPaintDir:
// GPD_TTOB - top to bottom,
// GPD_BTOT - bottom to top,
// GPD_LTOR - left to right,
// GPD_RTOL - right to left
#define    GPD_TTOB			0
#define    GPD_BTOT			1
#define    GPD_LTOR			2
#define    GPD_RTOL			3

// m_nPaintRGB: a combination of one or more of the following values
// i.e.:
// GPC_RED							-	Red
// GPC_GREEN						-	Green
// GPC_BLUE							-	Blue
// GPC_RED | GPC_GREEN				-	Yellow
// GPC_RED | GPC_BLUE				-	Purple
// GPC_GREEN | GPC_BLUE				-	Cyan
// GPC_RED | GPC_GREEN | GPC_BLUE	-	Grey
#define    GPC_RED			0x0001
#define    GPC_GREEN 		0x0002
#define    GPC_BLUE			0x0004


class CGradPalDlg : public CBCGPDialog
{
	public:
		CGradPalDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);
		//inline BOOL CreateWnd();
		BOOL	m_bFullScreenModeOn;
	
	protected:
		afx_msg BOOL OnQueryNewPalette();
		afx_msg void OnPaletteChanged(CWnd *pFocusWnd);
		afx_msg void OnPaint();
		
		DECLARE_MESSAGE_MAP()

		void FullScreenModeOn(void);

	private:
		const int m_nPaintSteps, m_nPaintDir;
		const UINT m_nPaintRGB;

		CPalette m_Pal;
		HBRUSH m_brush;
		
		void PaintGradiantRect(CDC *pDC, const RECT &rect) const;
		BOOL CreateGradPalette();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
};

/*
inline BOOL CGradPalDlg::CreateWnd()
{
	return this->Create(NULL, _T("Gradient Palette"));
}
*/
#endif  // #ifndef GRADPALDLG_H_