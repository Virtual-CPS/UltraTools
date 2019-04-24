// ReportView.h : interface of the CReportView class
//

#pragma once

class CUltraReportDoc;

class CReportView : public CFormView
{
protected: // �������л�����
	CReportView();
	DECLARE_DYNCREATE(CReportView)

public:
	enum { IDD = IDD_REPORT_FORM };

// ����
public:
	CUltraToolsDoc* GetDocument() const;

// ����
public:
	int ConfirmSaveReport();							// ȷ���Ƿ񽫱���洢Ϊ��ʽ����

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate();						// ������һ�ε���
	afx_msg void OnSaveReport();						// ���±������ʱ���Ϊ��ʽ����
	afx_msg void OnUpdateSaveReport(CCmdUI *pCmdUI);

// ʵ��
public:
	virtual ~CReportView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // ReportView.cpp �еĵ��԰汾
inline CUltraToolsDoc* CReportView::GetDocument() const
   { return reinterpret_cast<CUltraToolsDoc*>(m_pDocument); }
#endif
