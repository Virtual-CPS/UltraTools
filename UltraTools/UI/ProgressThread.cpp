// ProgressThread.CPP

#include "stdafx.h"
#include "ProgressThread.h"



//////////////////// Defines ////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CProgressThread, CWinThread)
	//{{AFX_MSG_MAP(CProgressThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CProgressThread, CWinThread)

CProgressThread::CProgressThread()
{
	CBCGPProgressDlgParams params;
	params.m_strMessage = "...";
	params.m_strCaption = "ÇëµÈ´ý";
	params.m_bDisableParentWnd = TRUE;
	params.m_bShowCancel = FALSE;
	params.m_bShowProgress = TRUE;
	params.m_bShowPercentage = TRUE;

	m_pProgressDlg = new CBCGPProgressDlg;
	m_pProgressDlg->EnableVisualManagerStyle();
	m_pProgressDlg->Create (params, NULL);
}

CProgressThread::~CProgressThread()
{
	if (m_pProgressDlg!=NULL && m_pProgressDlg->GetSafeHwnd()!=NULL)
	{
		m_pProgressDlg->DestroyWindow();
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}
}

BOOL CProgressThread::InitInstance()
{
	if(m_pProgressDlg != NULL)
		return TRUE;

	CBCGPProgressDlgParams params;
	params.m_strMessage = "...";
	params.m_strCaption = "ÇëµÈ´ý";
	params.m_bDisableParentWnd = TRUE;
	params.m_bShowCancel = FALSE;
	params.m_bShowProgress = TRUE;
	params.m_bShowPercentage = TRUE;

	m_pProgressDlg = new CBCGPProgressDlg;
	m_pProgressDlg->EnableVisualManagerStyle();
	BOOL bCreated = m_pProgressDlg->Create (params, NULL);
	VERIFY(bCreated);
//	m_pMainWnd = m_pProgressDlg;
	return bCreated;

}

void CProgressThread::CloseProgressDlg()
{
	//Wait until the ProgressDlg has been created
	//before trying to close it
	while (!m_pProgressDlg->GetSafeHwnd());

	m_pProgressDlg->SendMessage(WM_CLOSE);
	if (m_pProgressDlg!=NULL && m_pProgressDlg->GetSafeHwnd()!=NULL)
	{
		m_pProgressDlg->DestroyWindow();
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}
}

void CProgressThread::SetMessage(const CString& sMessage)
{
	m_pProgressDlg->SetMessage(sMessage);
	m_pProgressDlg->PostMessage(WM_ERASEBKGND);
}

void CProgressThread::SetPos(UINT nPos)
{
	m_pProgressDlg->SetPos(nPos);
	m_pProgressDlg->PostMessage(WM_ERASEBKGND);
}


BOOL CProgressThread::PumpMessage()
{
	MSG message;
	if(::PeekMessage(&message,NULL,0,0,PM_REMOVE)) 
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
	return CWinThread::PumpMessage();
}
