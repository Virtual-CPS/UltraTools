// AnalysisChildFrm.cpp : implementation of the CAnalysisChildFrame class
//

#include "stdafx.h"
#include "Resource.h"
#include "UltraTools.h"
#include "AnalysisChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAnalysisChildFrame

IMPLEMENT_DYNCREATE(CAnalysisChildFrame, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(CAnalysisChildFrame, CBCGPMDIChildWnd)
	//{{AFX_MSG_MAP(CAnalysisChildFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDM_ANALYSIS_VIEW_TOOLBAR, OnViewChildbar)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_VIEW_TOOLBAR, OnUpdateViewChildbar)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_TOOLBARMENU, OnToolbarContextMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalysisChildFrame construction/destruction

CAnalysisChildFrame::CAnalysisChildFrame()
{
}

CAnalysisChildFrame::~CAnalysisChildFrame()
{
}

BOOL CAnalysisChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	cs.style &= ~WS_SYSMENU;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAnalysisChildFrame diagnostics

#ifdef _DEBUG
void CAnalysisChildFrame::AssertValid() const
{
	CBCGPMDIChildWnd::AssertValid();
}

void CAnalysisChildFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnalysisChildFrame message handlers

int CAnalysisChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	UINT uiToolbarHotID = bIsHighColor ? IDB_ANALYSIS_TOOLBAR256 : 0;
	UINT uiMenuID = bIsHighColor ? IDB_ANALYSIS_MENU256 : IDB_ANALYSIS_MENU16;

	if (!m_wndToolBar.Create (this,
		WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|
		CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D, IDM_ANALYSIS_VIEW_TOOLBAR) ||
		!m_wndToolBar.LoadToolBar(IDR_ANALYSIS, 0, uiMenuID, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetWindowText(_T("图谱分析工具栏"));

	m_wndToolBar.SetToolBarBtnText (m_wndToolBar.CommandToIndex (IDM_ANALYSIS_SELECT_VL_IMAGE),
		_T("配对图谱"));

	//------------------------------------------
	// 初始化报表模板comboBox
	CBCGPToolbarComboBoxButton comboReportTemplate(IDM_ANALYSIS_SELECT_REPORT_TEMPLATE,
		CImageHash::GetImageOfCommand (IDM_ANALYSIS_SELECT_REPORT_TEMPLATE, FALSE));
	comboReportTemplate.SetCenterVert();

	// 模板文件路径
	CString str, sTemplateFilePath;
	LPTSTR lpStr = str.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL, lpStr, MAX_PATH);
	CString appPath(lpStr);
	str.ReleaseBuffer();
	int nPos = appPath.ReverseFind('\\');
	if(nPos > 0)
		sTemplateFilePath = appPath.Mid(0, nPos+1);
	else
		sTemplateFilePath = "";
	sTemplateFilePath = sTemplateFilePath + _T("Template\\*.rpt");

	// 获取并填充模板文件名(不支持嵌套子文件夹)
	CFileFind fileFind;
	CString sFileName;
	comboReportTemplate.AddItem(_T("**自动选择模板**"));
	BOOL bFound = fileFind.FindFile(sTemplateFilePath, 0);
	while(bFound)
	{
		bFound = fileFind.FindNextFile();

		sFileName = fileFind.GetFileName();
		nPos = sFileName.ReverseFind('.');
		if(nPos > 0)
			sFileName = sFileName.Mid(0, nPos);
		comboReportTemplate.AddItem(sFileName);
	}
	fileFind.Close();
	comboReportTemplate.SetText(_T("**自动选择模板**"));		// CurSel
	m_wndToolBar.ReplaceButton (IDM_ANALYSIS_SELECT_REPORT_TEMPLATE, comboReportTemplate);
	//------------------------------------------

	m_wndToolBar.EnableDocking (CBRS_ALIGN_ANY);
	EnableDocking (CBRS_ALIGN_ANY);
	EnableAutoHideBars (CBRS_ALIGN_RIGHT);
	DockControlBar (&m_wndToolBar, AFX_IDW_DOCKBAR_BOTTOM);

	return 0;
}

void CAnalysisChildFrame::OnViewChildbar() 
{
	ShowControlBar (&m_wndToolBar,
					!(m_wndToolBar.GetStyle () & WS_VISIBLE),
					FALSE, TRUE);
}

void CAnalysisChildFrame::OnUpdateViewChildbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolBar.GetStyle () & WS_VISIBLE);
}


BOOL CAnalysisChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CBCGPMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	return TRUE;
}

void CAnalysisChildFrame::OnClose() 
{
	CBCGPMDIChildWnd::OnClose();
}

LRESULT CAnalysisChildFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
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
