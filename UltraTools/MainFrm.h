// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "ThermalImage.h"

class CMainFrame : public CBCGPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:
	void ShowProgress(BOOL bShow = TRUE);
	void SetStatusBarMsg(CString sMsg);
	afx_msg void OnCreateExplorerView();
	void CreateAnalysisView(CString sFileName);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
	int						m_nProgressValue;
	BOOL					m_bInfiniteProgressMode;
	BOOL					m_bViewFormsByTabStyle;
	CFont					m_LogoFont;
	CString					m_sLogoString;
	CThermalImage*			m_pBackgroundImage;
	CImageList				m_imlStatusAnimation;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual CBCGPMDIChildWnd* CreateDocumentWindow (LPCTSTR lpcszDocName, CObject* /*pObj*/);

	BOOL CreateStatusBar();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL OnEraseMDIClientBackground(CDC* pDC);
	virtual void OnSizeMDIClient(const CRect& rectOld, const CRect& rectNew);
	afx_msg void OnSysOptions();
	afx_msg void OnHelpGuide();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDummy();

	DECLARE_MESSAGE_MAP()

};
