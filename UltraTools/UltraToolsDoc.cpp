// UltraToolsDoc.cpp : implementation of the CUltraToolsDoc class
//

#include "stdafx.h"
#include "UltraTools.h"

#include "UltraToolsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUltraToolsDoc

IMPLEMENT_DYNCREATE(CUltraToolsDoc, CDocument)

BEGIN_MESSAGE_MAP(CUltraToolsDoc, CDocument)
END_MESSAGE_MAP()


// CUltraToolsDoc construction/destruction

CUltraToolsDoc::CUltraToolsDoc()
{
	m_nCurItem = -1;
	m_sRootDirectory = _T("");
	m_sCurrentFile = _T("");
}

CUltraToolsDoc::~CUltraToolsDoc()
{
}

BOOL CUltraToolsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CUltraToolsDoc serialization

void CUltraToolsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CUltraToolsDoc diagnostics

#ifdef _DEBUG
void CUltraToolsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUltraToolsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUltraToolsDoc commands
BOOL CUltraToolsDoc::CanCloseFrame(CFrameWnd* pFrame)
{
/*	int nRet;

	if (IsModified())
	{
		if (pFrame->GetActiveView()->GetRuntimeClass()->m_lpszClassName == "CAnalysisView")
		{
			CAnalysisView* pView = (CAnalysisView*)(pFrame->GetActiveView());
			nRet = pView->m_wndLoadImage.ConfirmSaveAnalysisResult();
			if (IDCANCEL == nRet)
				return FALSE;
			else if (IDYES == nRet)
			{
				// 置不打开报表标志
				BOOL bOpenReport = theApp.m_bOpenReportViewAfterSaveAnalysisResult;
				theApp.m_bOpenReportViewAfterSaveAnalysisResult = FALSE;

				// 保存
				pView->SendMessage(WM_COMMAND, (WPARAM)IDM_ANALYSIS_SAVE, (LPARAM)0);
				theApp.m_bOpenReportViewAfterSaveAnalysisResult = bOpenReport;
			}

			SetModifiedFlag(FALSE);
			return TRUE;
		}
		else if (pFrame->GetActiveView()->GetRuntimeClass()->m_lpszClassName == "CAnalysisReportView")
		{
			CAnalysisReportView* pView = (CAnalysisReportView*)(pFrame->GetActiveView());
			nRet = pView->ConfirmSaveReport();
			if (IDCANCEL == nRet)
				return FALSE;
			else if (IDYES == nRet)
				pView->SendMessage(WM_COMMAND, (WPARAM)IDM_REPORT_SAVE, (LPARAM)0);

			SetModifiedFlag(FALSE);
			return TRUE;
		}
	}
*/
	return CDocument::CanCloseFrame(pFrame);
}

BOOL CUltraToolsDoc::SaveModified()
{
	POSITION pos = GetFirstViewPosition();
	ASSERT(pos != NULL);

	CView* pSrcView = GetNextView(pos);
	ASSERT_VALID(pSrcView);

	//CFrameWnd* pFrame = pSrcView->GetParentFrame ();
	//ASSERT_VALID (pFrame);

	//return CanCloseFrame(pFrame);	// 直接调用CanCloseFrame()导致死循环，why?
/*
	int nRet;

	if (pSrcView->IsKindOf(RUNTIME_CLASS(CAnalysisView)))
	{
		CAnalysisView* pView = (CAnalysisView*)pSrcView;
		nRet = pView->m_wndLoadImage.ConfirmSaveAnalysisResult();
		if (IDCANCEL == nRet)
			return FALSE;
		else if (IDYES == nRet)
		{
			// 置不打开报表标志
			BOOL bOpenReport = theApp.m_bOpenReportViewAfterSaveAnalysisResult;
			theApp.m_bOpenReportViewAfterSaveAnalysisResult = FALSE;

			// 保存
			pView->SendMessage(WM_COMMAND, (WPARAM)IDM_ANALYSIS_SAVE, (LPARAM)0);
			theApp.m_bOpenReportViewAfterSaveAnalysisResult = bOpenReport;
		}

		SetModifiedFlag(FALSE);
		return TRUE;
	}
	else if (pSrcView->IsKindOf(RUNTIME_CLASS(CAnalysisReportView)))
	{
		CAnalysisReportView* pView = (CAnalysisReportView*)pSrcView;
		nRet = pView->ConfirmSaveReport();
		if (IDCANCEL == nRet)
			return FALSE;
		else if (IDYES == nRet)
			pView->SendMessage(WM_COMMAND, (WPARAM)IDM_REPORT_SAVE, (LPARAM)0);

		SetModifiedFlag(FALSE);
		return TRUE;
	}
*/
	return CDocument::SaveModified();
}
