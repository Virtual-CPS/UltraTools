/////////////////////////////////////////////////////////////////////////////
// CIRCameraMeasurementDockingBar

#include "stdafx.h"
#include "IRCameraMeasurementDockingBar.h"

IMPLEMENT_DYNAMIC(CIRCameraMeasurementDockingBar, CBCGPDockingControlBar)

CIRCameraMeasurementDockingBar::CIRCameraMeasurementDockingBar()
{
	m_pOwner = NULL;
}

CIRCameraMeasurementDockingBar::~CIRCameraMeasurementDockingBar()
{
}


BEGIN_MESSAGE_MAP(CIRCameraMeasurementDockingBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CIRCameraMeasurementDockingBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIRCameraMeasurementDockingBar message handlers

void CIRCameraMeasurementDockingBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
	
	if (m_wndIRCameraMeasurement.GetSafeHwnd() != NULL)
	{
		m_wndIRCameraMeasurement.SetWindowPos (NULL, -1, -1, cx, cy,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

BOOL CIRCameraMeasurementDockingBar::Create(const RECT& rect, CWnd* pParentWnd)
{
	ASSERT(pParentWnd);
	m_pOwner = pParentWnd;

	CString strClassName = ::AfxRegisterWndClass (
			CS_SAVEBITS,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE + 1), NULL);

	CString sTitle;
	sTitle = _T("∫ÏÕ‚Õº∆◊∑÷Œˆ√Ê∞Â");
	if (!CBCGPDockingControlBar::Create(sTitle, pParentWnd, rect,
									TRUE , 
									IDM_ANALYSIS_VIEW_MEASUREMENT_BAR, 
									WS_CHILD | WS_VISIBLE,
									CBRS_BCGP_REGULAR_TABS,
									CBRS_BCGP_CLOSE | CBRS_BCGP_FLOAT))
	{
		return FALSE;
	}
	EnableDocking (0);
	FloatControlBar (rect);

	return TRUE;
}

int CIRCameraMeasurementDockingBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectWindow;
	GetClientRect(rectWindow);
	m_wndIRCameraMeasurement.Create (CIRCameraMeasurementWnd::IDD, this);
	m_wndIRCameraMeasurement.MoveWindow(rectWindow, TRUE);

	return 0;
}

void CIRCameraMeasurementDockingBar::OnPaint()
{
	m_wndIRCameraMeasurement.Invalidate();
}
