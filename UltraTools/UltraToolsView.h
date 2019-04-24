// UltraToolsView.h : interface of the CUltraToolsView class
//


#pragma once


class CUltraToolsView : public CBCGPFormView
{
protected: // create from serialization only
	CUltraToolsView();
	DECLARE_DYNCREATE(CUltraToolsView)

public:
	enum{ IDD = IDD_ULTRA_REPORT_FORM };

// Attributes
public:
	CUltraToolsDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CUltraToolsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UltraToolsView.cpp
inline CUltraToolsDoc* CUltraToolsView::GetDocument() const
   { return reinterpret_cast<CUltraToolsDoc*>(m_pDocument); }
#endif

