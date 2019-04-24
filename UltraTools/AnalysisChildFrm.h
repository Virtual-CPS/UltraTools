// AnalysisChildFrm.h : interface of the CAnalysisChildFrame class
//

#pragma once

class CAnalysisChildFrame : public CBCGPMDIChildWnd
{
	DECLARE_DYNCREATE(CAnalysisChildFrame)
public:
	CAnalysisChildFrame();

// Attributes
public:
	CBCGPToolBar		m_wndToolBar;
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CAnalysisChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewChildbar();
	afx_msg void OnUpdateViewChildbar(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM lp);

	DECLARE_MESSAGE_MAP()
};
