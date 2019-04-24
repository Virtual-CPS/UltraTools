#if !defined(COLOR_STATIC_H_)
#define COLOR_STATIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
// Construction
public:
	CColorStatic();
	virtual ~CColorStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Create(CString strCaption,COLORREF BackColor);
	void CColorStatic::Create(LPCTSTR szLabel, DWORD dwStyle, const RECT &rect, CWnd *pParent);

	void SetTextColor(COLORREF TextColor);
	void SetBackColor(COLORREF BackColor);
	void SetCaption(CString strCaption);
	void SetTextFont(CONST LOGFONT* pLogFont);

	COLORREF		m_TextColor;
	COLORREF		m_BackColor;
	CString			m_strCaption;
	CFont			m_TextFont;
	BOOL			m_bTextFontChanged;

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatic)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(COLOR_STATIC_H_)
