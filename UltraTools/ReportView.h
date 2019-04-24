// ReportView.h : interface of the CReportView class
//

#pragma once

class CUltraReportDoc;

class CReportView : public CFormView
{
protected: // 仅从序列化创建
	CReportView();
	DECLARE_DYNCREATE(CReportView)

public:
	enum { IDD = IDD_REPORT_FORM };

// 属性
public:
	CUltraToolsDoc* GetDocument() const;

// 操作
public:
	int ConfirmSaveReport();							// 确认是否将报表存储为正式报表

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate();						// 构造后第一次调用
	afx_msg void OnSaveReport();						// 更新报表的临时标记为正式报表
	afx_msg void OnUpdateSaveReport(CCmdUI *pCmdUI);

// 实现
public:
	virtual ~CReportView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg void OnReportPrev();
	afx_msg void OnReportNext();
	afx_msg void OnReportFirst();
	afx_msg void OnReportLast();
	afx_msg void OnReportPrint();
	afx_msg void OnUpdateReportPrev(CCmdUI *pCmdUI);
	afx_msg void OnUpdateReportNext(CCmdUI *pCmdUI);
	afx_msg void OnUpdateReportFirst(CCmdUI *pCmdUI);
	afx_msg void OnUpdateReportLast(CCmdUI *pCmdUI);
	afx_msg void OnUpdateReportPrint(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()

protected:
	CString							m_sAppPath;
	BOOL							m_bEndInit;
	BOOL							m_bReportSaved;

protected:
};

#ifndef _DEBUG  // ReportView.cpp 中的调试版本
inline CUltraToolsDoc* CReportView::GetDocument() const
   { return reinterpret_cast<CUltraToolsDoc*>(m_pDocument); }
#endif
