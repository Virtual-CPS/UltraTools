#if !defined(AFX_RANGESLIDER_H__CEC8D48F_0FDC_415C_A751_3AF12AF8B736__INCLUDED_)
#define AFX_RANGESLIDER_H__CEC8D48F_0FDC_415C_A751_3AF12AF8B736__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RangeSlider.h : header file
//

extern const UINT WM_RANGE_CHANGED;
/*
   Registered Window Message

   WPARAM: One of enum _RangeMessages
   LPARAM: NULL (all the time)
 */

/////////////////////////////////////////////////////////////////////////////
// CRangeSlider window

enum _RangeMessages {
	RS_LEFTCHANGED,
	RS_RIGHTCHANGED,
	RS_BOTHCHANGED,
};

class CRangeSlider : public CWnd
{
// Construction
public:
	CRangeSlider();

	void Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID, CCreateContext *pContext = NULL);
// Attributes
public:
	void SetMinMax(float min, float max);   //< Set Intervall [Min, Max] of RangeSlider
	void SetRange(float left, float right); //< Set Selected Range in [Min, Max]

	float GetMin(void) { return (m_bInvertedMode) ? -m_fMax : m_fMin; };    
	float GetMax(void) { return (m_bInvertedMode) ? -m_fMin : m_fMax; };
	float GetLeft(void) { return (m_bInvertedMode) ? -m_fRight : m_fLeft; };
	float GetRight(void) { return (m_bInvertedMode) ? -m_fLeft : m_fRight; };

	void GetMinMax(float &min, float &max)
	{ 
		min = (m_bInvertedMode) ? -m_fMax : m_fMin; 
		max = (m_bInvertedMode) ? -m_fMin : m_fMax; 
	};
	void GetRange(float &left, float &right)
	{ 
		left = (m_bInvertedMode) ? -m_fRight : m_fLeft; 
		right = (m_bInvertedMode) ? -m_fLeft : m_fRight; 
	};

	float GetVisualMin(void) { return (m_bInvertedMode) ? -m_fVisualMax : m_fVisualMin; }; 
	float GetVisualMax(void) { return (m_bInvertedMode) ? -m_fVisualMin : m_fVisualMax; };
	void GetVisualMinMax(float &VisualMin, float &VisualMax)
	{ 
		VisualMin = (m_bInvertedMode) ? -m_fVisualMax : m_fVisualMin; 
		VisualMax = (m_bInvertedMode) ? -m_fVisualMin : m_fVisualMax; 
	};

	void SetVisualMinMax(float InnerMin, float InnerMax);

// Operations
public:
	void SetVisualMode(BOOL bVisualMinMax = TRUE);
	BOOL GetVisualMode(void) { return m_bVisualMinMax; };

	void SetVisualColor(COLORREF cr);
	COLORREF GetVisualColor(void) { return m_crVisualColor; };

	void SetVerticalMode(BOOL bVerticalMode = TRUE);
	BOOL GetVerticalMode(void) { return !m_bHorizontal; };

	void SetInvertedMode(BOOL bInvertedMode = TRUE);
	BOOL GetInvertedMode(void) { return m_bInvertedMode; };
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRangeSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRangeSlider();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRangeSlider)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Data
	float m_fMin, m_fMax;				// Outer Edges of the Control
	float m_fLeft, m_fRight;			// Position of Arrows
	float m_fVisualMin, m_fVisualMax;	// Filled Rectangle between Arrows
	COLORREF m_crVisualColor;			// Fill color between arrows

	// Displaying
	void Normalize(void);				// Make intervall [Left, Right] containing in [Min, Max]
	void NormalizeVisualMinMax(void);	// Make intervall [InnerMin, InnerMax] containing in [Min, Max]

	int m_nArrowWidth;					// width of arrow buttons in pixels.

	BOOL m_bHorizontal;					// Horinzontal or Vertical Display of the range.
	void OnPaintHorizontal(CDC &dc);
	void OnPaintVertical(CDC &dc);
	
	BOOL m_bVisualMinMax;				// Display Inner MinMax Range
	BOOL m_bInvertedMode;				// Mirror the control (exchange left and right) 

	// Tracking the Mouse
	BOOL m_bTracking;					// Follow Mouse Input if T
	enum _TrackMode
	{
		TRACK_LEFT,						// Left Arrow is being slided
		TRACK_RIGHT,					// Right Arrow is being slided
		TRACK_MIDDLE,					// Middle Area is being slided
	} m_TrackMode; 

	CRect m_RectLeft, m_RectRight;		// Rectangles of the Left and Right Arrow
										// For Hit Testing.
	int m_fx;							// Size of the Window in x-direction (long axis).
	CPoint m_ClickOffset;				// Drag mouse at point of first click.

	BOOL m_bRangeChanged;				// 只在发生变化时才SendMessage()

	BOOL RegisterWindowClass(void);		// Make it work as custom control
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RANGESLIDER_H__CEC8D48F_0FDC_415C_A751_3AF12AF8B736__INCLUDED_)
