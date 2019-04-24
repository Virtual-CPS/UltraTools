//
// ColorSliderCtrl.h : header file
//

#if !defined(COLOR_SLIDER_CTRL_H_)
#define COLOR_SLIDER_CTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CColorSliderCtrl window

class CColorSliderCtrl : public CBCGPSliderCtrl
{
	DECLARE_DYNAMIC(CColorSliderCtrl)

// Construction
public:
	CColorSliderCtrl();
//	virtual ~CColorSliderCtrl();

// Attributes
	BOOL		m_bIsHueSlider;
	BOOL		m_bIsSaturationSlider;

// Operations
public:
	void SetImagesHue(double dblHue){m_dblHue = dblHue;}
	double GetImagesHue() const
	{
		return m_dblHue;
	}

	void SetImagesSaturation(double dblSaturation){m_dblSaturation = dblSaturation;}
	double GetImagesSaturation() const
	{
		return m_dblSaturation;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSliderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorSliderCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void DrawSliderChannel(CDC* pDC, BOOL bVert, CRect rect, BOOL bDrawOnGlass);
	void DrawSliderThumb (CDC* pDC,
						CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
						BOOL bVert, BOOL bLeftTop, BOOL bRightBottom, BOOL bDrawOnGlass);
	BOOL CanDrawImage () const
	{
#ifdef BCGP_EXCLUDE_PNG_SUPPORT
		return FALSE;
#else
		return globalData.m_nBitsPerPixel > 8 && 
			!globalData.IsHighContastMode ();
#endif
	}

protected:
	double					m_dblHue;
	double					m_dblSaturation;
	CBCGPToolBarImages		m_ThumbB;
	CBCGPToolBarImages		m_ThumbV;	

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(COLOR_SLIDER_CTRL_H_)
