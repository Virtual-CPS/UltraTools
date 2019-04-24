// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "UltraTools.h"

#include "MainFrm.h"
#include "GlobalDefsForSys.h"
#include "SysOptionsDlg.h"
#include "ExplorerView.h"
#include "ReportView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CBCGPMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()
	ON_COMMAND(IDM_SYS_OPTIONS, OnSysOptions)
	ON_COMMAND(IDM_HELP_GUIDE, OnHelpGuide)
	ON_COMMAND(ID_STATUSBAR_USER_NAME, OnDummy)
	ON_COMMAND(ID_STATUSBAR_COPYRIGHT, OnDummy)

	ON_COMMAND(IDM_VIEW_EXPLORER_FORMVIEW, OnCreateExplorerView)
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nProgressValue = -1;
	m_bInfiniteProgressMode = TRUE;
	m_bViewFormsByTabStyle = TRUE;
//	m_nStatusBarProcessTimer = 0;
//	m_nSysDatatimeTimer = 0;

	m_LogoFont.CreateFont(36, 0, 0, 0, FW_BOLD, 1, FALSE, FALSE,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					FIXED_PITCH | FF_ROMAN, _T("宋体"));	// Times New Roman

	m_sLogoString = _T("鹰视科技 ");

	//JPEG background
	m_pBackgroundImage = new CThermalImage();
	if (!m_pBackgroundImage->LoadResource(FindResource(NULL, _T("IDB_MDI_BACKGROUND"), _T("JPG")), THERMAL_IMAGE_FORMAT_JPG))
	{
		::delete m_pBackgroundImage;
		m_pBackgroundImage = NULL;
	}
	if (m_pBackgroundImage)
	{
		COLORREF rgb = GetSysColor(COLOR_APPWORKSPACE);
		RGBQUAD hsl = m_pBackgroundImage->RGBtoHSL(m_pBackgroundImage->RGBtoRGBQUAD(rgb));
		m_pBackgroundImage->Colorize(hsl.rgbRed, min(hsl.rgbGreen, 100));
	}
}

CMainFrame::~CMainFrame()
{
//	if (0 != m_nStatusBarProcessTimer && GetSafeHwnd())
//		KillTimer(m_nStatusBarProcessTimer);
//	if (0 != m_nSysDatatimeTimer && GetSafeHwnd())
//		KillTimer(m_nSysDatatimeTimer);

	m_LogoFont.DeleteObject();
	if (m_pBackgroundImage)
		::delete m_pBackgroundImage;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableMDITabs (TRUE /* Enable */, TRUE /* With icons */, CBCGPTabWnd::LOCATION_TOP);
	GetMDITabs().EnableTabDocumentsMenu ();
	GetMDITabs().SetTabCloseButtonMode(CBCGPTabWnd::TAB_CLOSE_BUTTON_HIGHLIGHTED_COMPACT);

	// Menu will not take the focus on activation:
	CBCGPPopupMenu::SetForceMenuFocus (FALSE);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}
	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);
	// Remove menubar gripper and borders:
	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() &
		~(CBRS_GRIPPER | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	//---------------------------------
	// Set toolbar and menu image size:
	//---------------------------------
	CBCGPToolBar::SetSizes(CSize(32, 32), CSize(24, 24));
	CBCGPToolBar::SetMenuSizes(CSize(22, 22), CSize(16, 16));

	CClientDC dc(this);
	BOOL bIsHighColor = dc.GetDeviceCaps(BITSPIXEL) > 8;
	UINT uiToolbarHotID = bIsHighColor ? IDB_MAIN_FRAME_TOOLBAR256 : 0;
	UINT uiMenuID = bIsHighColor ? IDB_MAIN_FRAME_MENU256 : IDB_MAIN_FRAME_MENU16;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, uiMenuID, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	ShowControlBar(&m_wndToolBar,
				!(m_wndToolBar.GetStyle() & WS_VISIBLE),	// 隐藏
				FALSE, TRUE);
//	CString strMainToolbarTitle;
//	strMainToolbarTitle.LoadString(IDS_MAIN_TOOLBAR);
//	m_wndToolBar.SetWindowText(strMainToolbarTitle);

	// 创建状态栏
	if (!CreateStatusBar())
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

//-----------------
// Statusbar panes:
//-----------------
const int nStatusWebIcon = 0;
const int nStatusMsg = 1;
const int nStatusAnimation = 2;
const int nStatusProgress = 3;
const int nStatusUserIcon = 4;
const int nStatusUserName = 5;
const int nStatusCopyright = 6;

const int nStatusBarAnimationWith = 16;
const int nStatusBarAnimationHeight = 16;

static UINT indicators[] =
{
	ID_STATUSBAR_WEB_ICON,	// web icon
	ID_SEPARATOR,           // status line indicator
	ID_STATUSBAR_ANIMATION,	// animation pane
	ID_STATUSBAR_PROGRESS,	// progress bar
	ID_STATUSBAR_USER_ICON,	// current user
	ID_STATUSBAR_USER_NAME,
	ID_STATUSBAR_COPYRIGHT,
};

BOOL CMainFrame::CreateStatusBar()
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	HICON hIcon = (HICON) ::LoadImage(
					AfxGetResourceHandle(),
					MAKEINTRESOURCE(IDI_WEB),
					IMAGE_ICON,
					::GetSystemMetrics(SM_CXSMICON),
					::GetSystemMetrics(SM_CYSMICON),
					LR_SHARED);
	m_wndStatusBar.SetPaneStyle(nStatusWebIcon, SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneIcon(nStatusWebIcon, hIcon);
	m_wndStatusBar.SetTipText(nStatusWebIcon, _T("开发者：鹰视科技"));

	m_wndStatusBar.SetPaneStyle(nStatusMsg, SBPS_STRETCH | SBPS_NOBORDERS);

	if (m_imlStatusAnimation.GetSafeHandle() == NULL)
	{
		CBCGPPngImage animation;
		if (animation.Load(IDB_STATUS_BAR_ANIMATION))
		{
			m_imlStatusAnimation.Create(nStatusBarAnimationWith, nStatusBarAnimationHeight, ILC_COLOR32, 10, 0);
			m_imlStatusAnimation.Add(&animation, (CBitmap*)NULL);
		}
	}
	m_wndStatusBar.SetPaneStyle(nStatusAnimation, SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneWidth(nStatusAnimation, nStatusBarAnimationWith);

	m_wndStatusBar.SetPaneWidth(nStatusProgress, 80);

	hIcon = (HICON) ::LoadImage(
							AfxGetResourceHandle(),
							MAKEINTRESOURCE(IDI_LOGIN),
							IMAGE_ICON,
							::GetSystemMetrics(SM_CXSMICON),
							::GetSystemMetrics(SM_CYSMICON),
							LR_SHARED);
	m_wndStatusBar.SetPaneStyle(nStatusUserIcon, SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneIcon(nStatusUserIcon, hIcon);

	CString sInfo, sTip(_T("请在天数清零前及时申请新的授权"));
	sInfo.Format(_T("[%s]%s"), theApp.m_tsLoginID.c_str(), theApp.m_tsLoginName.c_str());
	m_wndStatusBar.SetPaneText(nStatusUserName, sInfo);
	sInfo.Format(_T("当前用户：[%s]%s"), theApp.m_tsOrganizationName.c_str(), theApp.m_tsLoginName.c_str());
	m_wndStatusBar.SetTipText(nStatusUserName, sInfo);

	if (theApp.m_bOnTrial)
		sInfo.Format(_T("试用剩余%d天"), theApp.m_nDateCntToExpired);
	else
	{
		if (theApp.m_nDateCntToExpired < 90)
			sInfo.Format(_T("授权剩余%d天"), theApp.m_nDateCntToExpired);
		else
		{
			sInfo.Format(_T("已授权：%s"), theApp.m_sOrganizationForShort);
			sTip.Format(_T("已授权给：%s"), theApp.m_tsOrganizationName.c_str());
		}
	}
	m_wndStatusBar.SetPaneText(nStatusCopyright, sInfo);
	m_wndStatusBar.SetTipText(nStatusCopyright, sTip);
	if (theApp.m_nDateCntToExpired < 90)
	{
		COLORREF clrText = RGB(255, 0, 0);
		m_wndStatusBar.SetPaneTextColor(nStatusCopyright, clrText);
	}

	m_wndStatusBar.EnablePaneDoubleClick();

	return TRUE;
}

CBCGPMDIChildWnd* CMainFrame::CreateDocumentWindow (LPCTSTR lpcszDocName, CObject* /*pObj*/)
{
	if (lpcszDocName != NULL && lpcszDocName [0] != '\0')
	{
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile (lpcszDocName);

		if (pDoc != NULL)
		{
			POSITION pos = pDoc->GetFirstViewPosition();

			if (pos != NULL)
			{
				CView* pView = pDoc->GetNextView (pos);
				if (pView == NULL)
				{
					return NULL;
				}

				return DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pView->GetParent ());
			}   
		}
	}

	return NULL;
}

// 限制主界面幅面大小必须大于等于800×600
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CRect rectWindow;
	GetWindowRect(&rectWindow);

	CRect rectClient;
	GetClientRect(&rectClient);
	// get offset of toolbars, scrollbars, etc.
	int nWidthOffset = rectWindow.Width() - rectClient.Width();
	int nHeightOffset = rectWindow.Height() - rectClient.Height();

	lpMMI->ptMinTrackSize.x = 850 + nWidthOffset;
	lpMMI->ptMinTrackSize.y = 600 + nHeightOffset;

	CBCGPMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::OnHelpGuide()
{
	CString str;
	LPTSTR lpStr = str.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL, lpStr, MAX_PATH);
	CString appPath(lpStr);
	str.ReleaseBuffer();
	int nPos = appPath.ReverseFind('\\');
	if (nPos > 0)
		appPath = appPath.Mid(0, nPos + 1);
	else
		appPath = _T("C:\\");

	appPath += "SysGuide.PDF";
	if (ShellExecute(m_hWnd, _T("open"), appPath, NULL, NULL, SW_SHOW) <= (HINSTANCE)32)
		AfxMessageBox(_T("没有找到帮助文件!"));
}

BOOL CMainFrame::OnEraseMDIClientBackground(CDC* pDC)
{
	ASSERT_VALID(pDC);

	CRect rect, rcDataBox;
	m_wndClientArea.GetClientRect(rect);
	rcDataBox = rect;

	if (m_pBackgroundImage == NULL)
	{
		CBCGPDrawManager dm(*pDC);
		dm.FillGradient2(rect, RGB(255, 255, 255), RGB(191, 219, 255), 45);
	}
	else
	{
		m_pBackgroundImage->Tile(pDC->GetSafeHdc(), &rect);
	}

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	CFont* oldFont = pDC->SelectObject(&m_LogoFont);
	CSize sz = pDC->GetTextExtent(m_sLogoString, m_sLogoString.GetLength());
	rcDataBox.left = rcDataBox.right - sz.cx - tm.tmAveCharWidth / 2;
	rcDataBox.top = rcDataBox.bottom - sz.cy - tm.tmHeight / 5;

	pDC->SetBkMode(TRANSPARENT);

	// shift logo box right, and print black...
	COLORREF oldColor = pDC->SetTextColor(RGB(0, 0, 0));
	pDC->DrawText(m_sLogoString, m_sLogoString.GetLength(), &rcDataBox, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

	// shift logo box left and print white
	rcDataBox.left -= 3 * tm.tmAveCharWidth / 5;
	pDC->SetTextColor(RGB(255, 255, 255));
	pDC->DrawText(m_sLogoString, m_sLogoString.GetLength(), &rcDataBox, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

	// Restore original location and print in the button face color
	rcDataBox.left += tm.tmAveCharWidth / 5;
	pDC->SetTextColor(GetSysColor(COLOR_BTNFACE));
	pDC->DrawText(m_sLogoString, m_sLogoString.GetLength(), &rcDataBox, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

	// restore the original properties and release resources...
	pDC->SelectObject(oldFont);
	pDC->SetTextColor(oldColor);

	return TRUE;
}

void CMainFrame::OnSizeMDIClient(const CRect& /*rectOld*/, const CRect& /*rectNew*/)
{
	m_wndClientArea.RedrawWindow();
}

void CMainFrame::OnDummy()
{
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_STATUSBAR_PROGRESS_START)
	{
		ASSERT_VALID(theApp.m_pDocTemplateExplorer);

		BOOL bIsRunning = FALSE;
		POSITION pos = theApp.m_pDocTemplateExplorer->GetFirstDocPosition();
		if (pos != NULL)
		{
			CDocument* pDoc = theApp.m_pDocTemplateExplorer->GetNextDoc(pos);
			ASSERT_VALID(pDoc);

			pos = pDoc->GetFirstViewPosition();
			ASSERT(pos != NULL);

			CExplorerView* pView = (CExplorerView*)(pDoc->GetNextView(pos));
			ASSERT_VALID(pView);

//			bIsRunning = pView->IsMakingReport();
		}

		m_nProgressValue++;
/*
		if (!bIsRunning || m_nProgressValue > 5000)
		{
			ShowProgress(FALSE);
		}
		else
		{
			CBCGPRibbonProgressBar* pProgress = DYNAMIC_DOWNCAST(
				CBCGPRibbonProgressBar,
				m_wndStatusBar.FindElement(ID_STATUSBAR_PROGRESS));
			ASSERT_VALID(pProgress);

			pProgress->SetPos(m_nProgressValue, TRUE);
		}
*/	}
}

void CMainFrame::ShowProgress(BOOL bShow)
{
/*	if (bShow)
	{
		int cxFree = m_wndStatusBar.GetSpace();
		if (cxFree < 20)
		{
			// Not enough space for progress bar
			return;
		}

		int cxProgress = min(cxFree, 200);

		if (m_nProgressValue >0)		// 正在显示
		{
			if (0 != m_nStatusBarProcessTimer)
			{
				KillTimer(m_nStatusBarProcessTimer);
				m_nStatusBarProcessTimer = 0;
			}
			m_wndStatusBar.RemoveElement(ID_STATUSBAR_PROGRESS);
		}

		CBCGPRibbonProgressBar* pProgressBar = new CBCGPRibbonProgressBar(ID_STATUSBAR_PROGRESS, cxProgress);
		pProgressBar->SetInfiniteMode(m_bInfiniteProgressMode);
		m_wndStatusBar.AddDynamicElement(pProgressBar);
		m_nProgressValue = 0;

		m_nStatusBarProcessTimer = (UINT)SetTimer(TIMER_STATUSBAR_PROGRESS_START, 100, NULL);
	}
	else
	{
		if (0 != m_nStatusBarProcessTimer)
		{
			KillTimer(m_nStatusBarProcessTimer);
			m_nStatusBarProcessTimer = 0;
		}
		m_wndStatusBar.RemoveElement(ID_STATUSBAR_PROGRESS);
		m_nProgressValue = -1;
	}

	m_wndStatusBar.RecalcLayout();
	m_wndStatusBar.RedrawWindow();
*/}

void CMainFrame::SetStatusBarMsg(CString sMsg)
{
/*	CBCGPRibbonStatusBarPane* pPane = DYNAMIC_DOWNCAST(
				CBCGPRibbonStatusBarPane,
				m_wndStatusBar.FindElement(ID_STATUSBAR_MSG));
	ASSERT_VALID(pPane);
	pPane->SetText(sMsg);
	pPane->Redraw();
*/
	m_wndStatusBar.SetPaneText(nStatusMsg, sMsg);
}

void CMainFrame::OnCreateExplorerView()
{
	ASSERT_VALID(theApp.m_pDocTemplateExplorer);

	POSITION pos = theApp.m_pDocTemplateExplorer->GetFirstDocPosition();
	if (pos == NULL)
	{
		theApp.m_pDocTemplateExplorer->OpenDocumentFile(NULL);
		return;
	}

	CDocument* pDoc = theApp.m_pDocTemplateExplorer->GetNextDoc(pos);
	ASSERT_VALID(pDoc);

	pos = pDoc->GetFirstViewPosition();
	ASSERT(pos != NULL);

	CView* pView = pDoc->GetNextView(pos);
	ASSERT_VALID(pView);

	CFrameWnd* pFrame = pView->GetParentFrame();
	ASSERT_VALID(pFrame);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	ASSERT_VALID(pMainFrame);

	::SendMessage(pMainFrame->m_hWndMDIClient, WM_MDIACTIVATE,
		(WPARAM)pFrame->GetSafeHwnd(), 0);
}

void CMainFrame::CreateAnalysisView(CString sFileName)
{
	ASSERT_VALID(theApp.m_pDocTemplateAnalysis);

	theApp.m_pDocTemplateAnalysis->OpenDocumentFile(sFileName);
}

void CMainFrame::OnSysOptions()
{
	CSysOptionsDlg dlg;
	dlg.DoModal();
}