#if !defined(PALLETTE_STATICE_H_)
#define PALLETTE_STATICE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaletteStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaletteStatic window

class CPaletteStatic : public CStatic
{
// Construction
public:
	CPaletteStatic();

// Attributes
private:
	BOOL			m_bCreateBmp;
	BOOL			m_bShowCalibration;	// 右侧显示刻度
	BOOL			m_bShowLimits;		// 顶端和底端显示上下限
	float			m_fUpperLimit;
	float			m_fLowerLimit;
	CBitmap			m_Bitmap;
	LOGPEN			m_logpen;
	LOGBRUSH		m_logbrush;
	CFont			m_font;
	int				m_nPaletteID;
	BOOL			m_bPaletteReversed;	// 调色板取反

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawPalette(int nID, CRect* pDstRect=NULL, CDC* pDstDC=NULL);
	virtual ~CPaletteStatic();

	void SetLimits(float fUpper, float fLower)
	{
		m_fUpperLimit = fUpper;
		m_fLowerLimit = fLower;
		Invalidate();
	}

	void ShowCalibration(BOOL bShow=TRUE)
	{
		m_bShowCalibration = bShow;
		CreateBmp();
		DrawPalette(m_nPaletteID);
	}

	BOOL IsShowCaclibration(){return m_bShowCalibration;}

	void ShowLimits(BOOL bShow=TRUE)
	{
		m_bShowLimits=bShow;
		CreateBmp();
		DrawPalette(m_nPaletteID);
	}

	BOOL IsShowLimits(){return m_bShowLimits;}

	void SetPalette(int iPaletteIndex=2, BOOL bReversed=FALSE)
	{
		m_bPaletteReversed = bReversed;
		DrawPalette(iPaletteIndex);
	}

	BOOL IsPaletteReversed(){return m_bPaletteReversed;}

	// Generated message map functions
protected:
	//{{AFX_MSG(CPaletteStatic)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void CreateBmp();
	CRect CalculateBarRect(CRect rectClient);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(PALLETTE_STATICE_H_)
