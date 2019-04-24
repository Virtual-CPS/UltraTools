#if !defined(KEYDOWN_COMBOBOX_H_)
#define KEYDOWN_COMBOBOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeydownComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKeydownComboBox window

class CKeydownComboBox : public CBCGPComboBox
{
// Construction
public:
	CKeydownComboBox();
	virtual ~CKeydownComboBox();

// Attributes
public:
	BOOL		m_bAutoSearchChinese;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeydownComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	
protected:
	int			m_nNextIndex;
	CString		m_sKeyboardEntry;

	//{{AFX_MSG(CKeydownComboBox)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	void SearchItem(int nStart, BOOL bForward=TRUE);
	void GetFirstLetter(CString sChineseStr, CString& sFirstLetter, int nFirstLetterStrLen);
	void FirstLetter(int nCode, CString& sLetter);
	void RetainOnlyAlnum(CString &sStr);
	void RetainOnlyChinese(CString &sStr);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSetfocus();
	afx_msg void OnCbnSelendok();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(KEYDOWN_COMBOBOX_H_)
