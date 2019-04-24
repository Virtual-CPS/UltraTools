#if !defined(IMAGE_PREVIEW_DLG_H_)
#define IMAGE_PREVIEW_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImagePreviewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImagePreviewDlg window

class CImagePreviewDlg : public CDialog
{
// Construction
public:
	CImagePreviewDlg(CWnd* pOwner=NULL);

	CStatic m_wndCanvas;
	CString m_sFileName;
	BOOL	m_bOwnerIsExplorer;			// TRUE:=CExplorerView£»FALSE=CAnalysisAndImportView

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagePreviewDlg)
	//}}AFX_VIRTUAL

	virtual ~CImagePreviewDlg();

protected:
	CWnd* m_pOwner;

	//{{AFX_MSG(CImagePreviewDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(IMAGE_PREVIEW_DLG_H_)
