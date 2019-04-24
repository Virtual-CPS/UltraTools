// ReportChildFrm.cpp : implementation of the CReportChildFrame class
//

#include "stdafx.h"
#include "Resource.h"
#include "ReportChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CReportChildFrame

IMPLEMENT_DYNCREATE(CReportChildFrame, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(CReportChildFrame, CBCGPMDIChildWnd)
	//{{AFX_MSG_MAP(CReportChildFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDM_REPORT_VIEW_TOOLBAR, OnViewChildbar)
	ON_UPDATE_COMMAND_UI(IDM_REPORT_VIEW_TOOLBAR, OnUpdateViewChildbar)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_TOOLBARMENU, OnToolbarContextMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportChildFrame construction/destruction

CReportChildFrame::CReportChildFrame()
{
}

CReportChildFrame::~CReportChildFrame()
{
}

BOOL CReportChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	cs.style &= ~WS_SYSMENU;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CReportChildFrame diagnostics

#ifdef _DEBUG
void CReportChildFrame::AssertValid() const
{
	CBCGPMDIChildWnd::AssertValid();
}

void CReportChildFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportChildFrame message handlers

int CReportChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	UINT uiToolbarHotID = bIsHighColor ? IDB_REPORT_TOOLBAR256 : 0;
	UINT uiMenuID = bIsHighColor ? IDB_REPORT_MENU256: IDB_REPORT_MENU16;

	if (!m_wndToolBar.Create (this,
		WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|
		CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D, IDM_REPORT_VIEW_TOOLBAR) ||
		!m_wndToolBar.LoadToolBar(IDR_REPORT, 0, uiMenuID, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetWindowText(_T("报表工具栏"));

	m_wndToolBar.EnableDocking (CBRS_ALIGN_ANY);
	EnableDocking (CBRS_ALIGN_ANY);
	EnableAutoHideBars (CBRS_ALIGN_RIGHT);
	DockControlBar (&m_wndToolBar, AFX_IDW_DOCKBAR_BOTTOM);

	return 0;
}

void CReportChildFrame::OnViewChildbar() 
{
	ShowControlBar (&m_wndToolBar,
					!(m_wndToolBar.GetStyle () & WS_VISIBLE),
					FALSE, TRUE);
}

void CReportChildFrame::OnUpdateViewChildbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolBar.GetStyle () & WS_VISIBLE);
}


BOOL CReportChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CBCGPMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	return TRUE;
}

void CReportChildFrame::OnClose() 
{
	CBCGPMDIChildWnd::OnClose();
}

LRESULT CReportChildFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
{
// 	CPoint point (BCG_GET_X_LPARAM(lp), BCG_GET_Y_LPARAM(lp));
// 
// 	CMenu menu;
// 	VERIFY(menu.LoadMenu (IDR_POPUP_MDI_TOOLBAR));
// 
// 	CMenu* pPopup = menu.GetSubMenu(0);
// 	ASSERT(pPopup != NULL);
// 
// 	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
// 	pPopupMenu->Create (this, point.x, point.y, pPopup->Detach ());

	return 0;
}
