// ExplorerChildFrm.cpp : implementation of the CExplorerChildFrame class
//

#include "stdafx.h"
#include "Resource.h"
#include "ExplorerChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExplorerChildFrame

IMPLEMENT_DYNCREATE(CExplorerChildFrame, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(CExplorerChildFrame, CBCGPMDIChildWnd)
	//{{AFX_MSG_MAP(CExplorerChildFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDM_EXPLORER_VIEW_TOOLBAR, OnViewChildbar)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_VIEW_TOOLBAR, OnUpdateViewChildbar)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerChildFrame construction/destruction

CExplorerChildFrame::CExplorerChildFrame()
{
}

CExplorerChildFrame::~CExplorerChildFrame()
{
}

BOOL CExplorerChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	cs.style &= ~WS_SYSMENU;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CExplorerChildFrame diagnostics

#ifdef _DEBUG
void CExplorerChildFrame::AssertValid() const
{
	CBCGPMDIChildWnd::AssertValid();
}

void CExplorerChildFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExplorerChildFrame message handlers

int CExplorerChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	UINT uiToolbarHotID = bIsHighColor ? IDB_EXPLORER_TOOLBAR256 : 0;
	UINT uiMenuID = bIsHighColor ? IDB_EXPLORER_MENU256 : IDB_EXPLORER_MENU16;

	if (!m_wndToolBar.Create (this,
			WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|
			CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
			CBRS_GRIPPER | CBRS_BORDER_3D, IDM_EXPLORER_VIEW_TOOLBAR) ||
			!m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, uiMenuID, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetWindowText(_T("图谱管理工具栏")); 

	m_wndToolBar.SetToolBarBtnText (m_wndToolBar.CommandToIndex (IDM_EXPLORER_VIEW_TREE),
		_T("图谱树"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_VIEW_IMAGE_PREVIEW),
		_T("图谱预览"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_LIST_SORT_BY_NAME),
		_T("名称排序"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_LIST_SORT_BY_DATETIME),
		_T("时间排序"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_REPORT_PREVIEW),
		_T("预览报告"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_REPORT_GENERATE),
		_T("顺序生成报告"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_REPORT_GENERATE_REVERSELY),
		_T("逆序生成报告"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_EXPLORER_REPORT_MERGE),
		_T("合并报告"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_SYS_OPTIONS),
		_T("系统设置"));
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(IDM_VIEW_ANALYSIS_FORMVIEW),
		_T("分析诊断"));
/*
	// Replace "Views" button by the menu button:
	CMenu menuViews;
	menuViews.LoadMenu (IDR_VIEWS_POPUP);
	int nImage = (int)CImageHash::GetImageOfCommand (IDM_VIEW_FILE_STYLES);
	m_wndToolBar.ReplaceButton (IDM_VIEW_FILE_STYLES,
		CBCGPToolbarMenuButton((UINT)-1, menuViews, 
		nImage, _T("查看")));
*/
	m_wndToolBar.EnableDocking (CBRS_ALIGN_ANY);
	EnableDocking (CBRS_ALIGN_ANY);
	EnableAutoHideBars (CBRS_ALIGN_RIGHT);
	DockControlBar (&m_wndToolBar, AFX_IDW_DOCKBAR_BOTTOM);

	return 0;
}

void CExplorerChildFrame::OnViewChildbar() 
{
	ShowControlBar (&m_wndToolBar,
					!(m_wndToolBar.GetStyle () & WS_VISIBLE),
					FALSE, TRUE);
}

void CExplorerChildFrame::OnUpdateViewChildbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolBar.GetStyle () & WS_VISIBLE);
}


BOOL CExplorerChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CBCGPMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	return TRUE;
}

void CExplorerChildFrame::OnClose() 
{
	CBCGPMDIChildWnd::OnClose();
}

LRESULT CExplorerChildFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
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
