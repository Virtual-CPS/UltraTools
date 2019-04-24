// ReportView.cpp : implementation of the CReportView class
//

#include "stdafx.h"
#include "UltraTools.h"
#include "UltraToolsDoc.h"
#include "MainFrm.h"
#include "ReportView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CReportView

IMPLEMENT_DYNCREATE(CReportView, CFormView)

BEGIN_MESSAGE_MAP(CReportView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(IDM_REPORT_SAVE, &CReportView::OnSaveReport)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_SAVE, &CReportView::OnUpdateSaveReport)

	ON_COMMAND(IDM_REPORT_PREV, &CReportView::OnReportPrev)
	ON_COMMAND(IDM_REPORT_NEXT, &CReportView::OnReportNext)
	ON_COMMAND(IDM_REPORT_FIRST, &CReportView::OnReportFirst)
	ON_COMMAND(IDM_REPORT_LAST, &CReportView::OnReportLast)
	ON_COMMAND(IDM_REPORT_PRINT, &CReportView::OnReportPrint)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_PREV, &CReportView::OnUpdateReportPrev)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_NEXT, &CReportView::OnUpdateReportNext)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_FIRST, &CReportView::OnUpdateReportFirst)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_LAST, &CReportView::OnUpdateReportLast)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_PRINT, &CReportView::OnUpdateReportPrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportView construction/destruction

CReportView::CReportView()
	: CFormView(CReportView::IDD)
{
	m_sAppPath = "";

	m_bEndInit = FALSE;
	m_bReportSaved = TRUE;
}

CReportView::~CReportView()
{
}

void CReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CReportView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CFormView::PreCreateWindow(cs);
}

void CReportView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CString str;
	LPTSTR lpStr = str.GetBuffer(MAX_PATH);    
	GetModuleFileName(NULL, lpStr, MAX_PATH);
	CString appPath(lpStr);
	str.ReleaseBuffer();
	int nPos = appPath.ReverseFind('\\');
	if(nPos > 0)
		m_sAppPath = appPath.Mid(0, nPos+1);
	else
		m_sAppPath = "";

	extern CReportView* g_pReportView;
	g_pReportView = this;

	CDocument* pDoc = GetDocument();
	ASSERT_VALID (pDoc);
	pDoc->SetTitle (_T("����"));

	m_bEndInit = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CReportView diagnostics

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	CFormView::AssertValid();
}

void CReportView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CUltraToolsDoc* CReportView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUltraToolsDoc)));
	return (CUltraToolsDoc*)m_pDocument;
}
#endif //_DEBUG

// CReportView ��Ϣ�������

void CReportView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (GetSafeHwnd()==NULL || !m_bEndInit)
	{
		return;
	}
	if(cx<200 || cy<200)
		return;
}

void CReportView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if(!bActivate)
		return;

	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

// ȷ���Ƿ񽫱���洢Ϊ��ʽ����
// ����TRUE=���棻FALSE=����/������
int CReportView::ConfirmSaveReport()
{
	if(m_bReportSaved)
		return IDNO;

	CString sTitle, sTem;
	CDocument* pDoc = GetDocument();
	ASSERT_VALID (pDoc);
	sTitle = pDoc->GetTitle();
	sTem.Format(_T("��ʱ����%s����δ���档\n\n���棿"), sTitle);
	return MessageBox(sTem, _T("��ȷ���Ƿ񱣴�"), MB_YESNOCANCEL|MB_ICONQUESTION);
}

// ���±������ʱ���Ϊ��ʽ����
void CReportView::OnSaveReport()
{
	ASSERT(!m_bReportSaved);

//	theApp.m_SysDatabase.m_adoDatabaseConn.ExecuteSQL(m_sSqlForSaveReport);
	m_bReportSaved = TRUE;
}

void CReportView::OnUpdateSaveReport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bReportSaved);
}

void CReportView::OnReportPrev()
{
}

void CReportView::OnReportNext()
{
}

void CReportView::OnReportFirst()
{
}

void CReportView::OnReportLast()
{
}

void CReportView::OnReportPrint()
{
}

void CReportView::OnUpdateReportPrev(CCmdUI *pCmdUI)
{
}

void CReportView::OnUpdateReportNext(CCmdUI *pCmdUI)
{
}

void CReportView::OnUpdateReportFirst(CCmdUI *pCmdUI)
{
}

void CReportView::OnUpdateReportLast(CCmdUI *pCmdUI)
{
}

void CReportView::OnUpdateReportPrint(CCmdUI *pCmdUI)
{
}
