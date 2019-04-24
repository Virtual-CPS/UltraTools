#if !defined(PRINT_IMAGE_DLG_H_)
#define PRINT_IMAGE_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintImageDlg.h : header file
//

#include "../Resource.h"
#include "ThermalImage.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintImageDlg dialog

class CPrintImageDlg : public CBCGPDialog
{
// Construction
public:
	CPrintImageDlg(CWnd* pParent = NULL);   // standard constructor

// Attribute
	int			m_nVideoIndex;
	int			m_nWidth;
	int			m_nHeight;
	int			m_nChannelIndex;
	CString		m_sDateTime;

	char		m_szCamera[32];
	char		m_szTime[32];

	HBITMAP			m_hBitmap;
	CThermalImage	m_DstImage;
	CThermalImage	m_SrcImage;
	LPBYTE			m_pRGB;


// Dialog Data
	//{{AFX_DATA(CPrintImageDlg)
	enum { IDD = IDD_PRINT_IMAGE_DLG };
	CStatic	m_PreviewWnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintImageDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintImageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnPreviewPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(PRINT_IMAGE_DLG_H_)
