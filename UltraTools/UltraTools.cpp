// UltraTools.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "UltraTools.h"
#include "MainFrm.h"
#include "UltraToolsDoc.h"
#include "AboutDlg.h"

#include "ExplorerChildFrm.h"
#include "ExplorerView.h"
#include "AnalysisChildFrm.h"
#include "AnalysisView.h"
#include "ReportChildFrm.h"
#include "ReportView.h"

#include "Utility/HvUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUltraToolsApp

BEGIN_MESSAGE_MAP(CUltraToolsApp, CBCGPWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CBCGPWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CBCGPWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CUltraToolsApp construction

CUltraToolsApp::CUltraToolsApp()
	: m_nDateCntToExpired(0)
	, m_bOnTrial(TRUE)
{
	AddVisualTheme(BCGP_VISUAL_THEME_DEFAULT, IDM_VIEW_APPLOOK_WINDOWS_NATIVE);
	AddVisualTheme(BCGP_VISUAL_THEME_VS_2010, IDM_VIEW_APPLOOK_VS2010);
	AddVisualTheme(BCGP_VISUAL_THEME_VS_2012_LIGHT, IDM_VIEW_APPLOOK_VS2012_LIGHT);
	AddVisualTheme(BCGP_VISUAL_THEME_VS_2012_DARK, IDM_VIEW_APPLOOK_VS2012_DARK);
	AddVisualTheme(BCGP_VISUAL_THEME_VS_2012_BLUE, IDM_VIEW_APPLOOK_VS2012_BLUE);
	AddVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_BLUE, IDM_VIEW_APPLOOK_2010_BLUE);
	AddVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_BLACK, IDM_VIEW_APPLOOK_2010_BLACK);
	AddVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_SILVER, IDM_VIEW_APPLOOK_2010_SILVER);
	AddVisualTheme(BCGP_VISUAL_THEME_CARBON, IDM_VIEW_APPLOOK_CARBON);

	SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_BLUE);
	m_bContextMenuManager = FALSE;
	m_bKeyboardManager = FALSE;

	m_bOnTrial = TRUE;
	m_sTemFilePath = _T("");
}


// The one and only CUltraToolsApp object

CUltraToolsApp theApp;


// CUltraToolsApp initialization

BOOL CUltraToolsApp::InitInstance()
{
	if (!InitSys())
	{
		return FALSE;
	}

	// 资源管理器
	m_pDocTemplateExplorer = new CMultiDocTemplate(
							IDR_EXPLORER,
							RUNTIME_CLASS(CUltraToolsDoc),
							RUNTIME_CLASS(CExplorerChildFrame),	// 自定义 MDI 子框架
							RUNTIME_CLASS(CExplorerView));
	if (!m_pDocTemplateExplorer)
		return FALSE;
	AddDocTemplate(m_pDocTemplateExplorer);

	// 红外图谱分析
	m_pDocTemplateAnalysis = new CMultiDocTemplate(
							IDR_ANALYSIS,
							RUNTIME_CLASS(CUltraToolsDoc),
							RUNTIME_CLASS(CAnalysisChildFrame),	// 自定义 MDI 子框架
							RUNTIME_CLASS(CAnalysisView));
	if (!m_pDocTemplateAnalysis)
		return FALSE;
	AddDocTemplate(m_pDocTemplateAnalysis);

	// 分析报表
	m_pDocTemplateReport = new CMultiDocTemplate(
							IDR_REPORT,
							RUNTIME_CLASS(CUltraToolsDoc),
							RUNTIME_CLASS(CReportChildFrame),// 自定义 MDI 子框架
							RUNTIME_CLASS(CReportView));
	if (!m_pDocTemplateReport)
		return FALSE;
	AddDocTemplate(m_pDocTemplateReport);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	//cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;// 启动时不自动打开一个空文档 
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		m_pDocTemplateExplorer->OpenDocumentFile(NULL);
		//m_pDocTemplateAnalysis->OpenDocumentFile (NULL);
	}
	else
	{
		// 调度在命令行中指定的命令。如果
		// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	return TRUE;
}

BOOL CUltraToolsApp::InitSys()
{
	if (!globalData.bIsWindows2000)
	{
		AfxMessageBox(_T("UltraTools requires Windows 2000 or higher"));
		return FALSE;
	}

	SetRegistryKey(_T("HawkVision"));
	SetRegistryBase(_T("UiSettings"));

	// 只能运行单个实例, 用互斥量来判断程序是否已运行
	//m_hMutex = CreateMutex(NULL,TRUE, m_pszAppName); 
	m_hMutex = CreateMutex(NULL, TRUE, _T("UltraTools"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(_T("检测到本系统正在运行!"), MB_OK);
		return FALSE;
	}

	// 创建临时文件路径
	TCHAR szPath[MAX_PATH];
	GetTempPath(MAX_PATH, szPath);
	m_sTemFilePath.Format(_T("%sUltraReport\\"), szPath);
	CreateDirectory(m_sTemFilePath, NULL);

	if (GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		CString sTem(szPath);
		m_sAppPath = sTem.Left(sTem.ReverseFind('\\') + 1);
	}
	else
		m_sAppPath = _T("");

	//---------------------------------------------------------------------------
	// 获取登录信息
	TCHAR szTem[100];
	CString sSysIniFile = m_sAppPath + _T("Data\\Sys.ini");
	GetPrivateProfileString(_T("Login"), _T("ID"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	m_tsLoginID.assign(szTem);
	GetPrivateProfileString(_T("Login"), _T("Name"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	m_tsLoginName.assign(szTem);
	GetPrivateProfileString(_T("Login"), _T("OrganizationForShort"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	m_sOrganizationForShort = szTem;
	GetPrivateProfileString(_T("Login"), _T("OrganizationID"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	m_tsOrganizationID.assign(szTem);
	GetPrivateProfileString(_T("Login"), _T("OrganizationName"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	m_tsOrganizationName.assign(szTem);

	char szHostName[128];
	WSADATA wsData;
	::WSAStartup(MAKEWORD(2, 2), &wsData);
	if (gethostname((char*)(szHostName), 128) == 0)		// 取本机主机名成功
		m_tsHostName = HvUtility::Char2String(szHostName);
	else
		m_tsHostName = _T("");

	HINSTANCE hInstance = AfxGetInstanceHandle();

	// 初始化加解密参数
	m_tsAppName = _T("UltraTools");
	m_tsManufacturerID = _T("HawkVision");
	HvUtility::SetAppName(NULL, m_tsAppName, false);
	HvUtility::SetOrganizationID(NULL, m_tsOrganizationID, false);

	int nType;
	tstring tsDateSpan, tsPriKey, tsPubKey, tsTem;
	HvUtility::GetHostID(NULL, m_tsHostID, nType, tsDateSpan, tsPriKey, tsPubKey, true);	// 获取本机ID
	HvUtility::SetHostID(hInstance, m_tsHostID);

	// 读注册表，设置是否“自杀”标志
	BOOL bDeleteMe = TRUE;
	if (HvUtility::Init(hInstance, nType, tsDateSpan, tsPriKey, tsPubKey))
	{
		HvUtility::Decrypt(tsDateSpan, tsTem);
		m_nDateCntToExpired = _tstoi(tsTem.c_str());
	}
	else
	{
		// 尝试预授权
		CString sRegFile;
		tstring tsRegFile;
		sRegFile.Format(_T("%s%s，%s.paf"), m_sAppPath, m_tsOrganizationName.c_str(), m_tsAppName.c_str());
		tsRegFile.assign(sRegFile);
		HvUtility::SetAppName(NULL, m_tsAppName, false);					// 消除前面Init()失败导致的影响
		HvUtility::SetOrganizationID(NULL, m_tsOrganizationID, false);
		HvUtility::SetHostID(NULL, m_tsHostID, false);
		if (HvUtility::Register(hInstance, tsRegFile, true, false))
		{
			HvUtility::SetHostID(hInstance, m_tsHostID);
			if (HvUtility::Init(hInstance, nType, tsDateSpan, tsPriKey, tsPubKey))
			{
				HvUtility::Decrypt(tsDateSpan, tsTem);
				m_nDateCntToExpired = _tstoi(tsTem.c_str());
			}
		}
	}
	if (nType != 2 && nType != 4 || theApp.m_nDateCntToExpired <= 0)
		bDeleteMe = TRUE;
	else
		bDeleteMe = FALSE;
	if (4 == nType)
		m_bOnTrial = FALSE;

	//---------------------------------------------------------------------------

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	CoInitialize(NULL);
	AfxEnableControlContainer();

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();
	InitKeyboardManager();
	CBCGPVisualManager::SetDefaultManager(RUNTIME_CLASS(CBCGPVisualManager2007));

	// Remove this if you don't want extended tooltips:
	InitTooltipManager();
	CBCGPToolTipParams params;
	params.m_bVislManagerTheme = TRUE;
	GetTooltipManager()->SetTooltipParams(
										BCGP_TOOLTIP_TYPE_ALL,
										RUNTIME_CLASS(CBCGPToolTipCtrl),
										//RUNTIME_CLASS (CRibbonTooltipCtrl),
										&params);

	InitShellManager();

	// 将本程序添加到防火墙的“例外”
	//CString sAppName;
	//sAppName.LoadString(AFX_IDS_APP_TITLE);
	//AddAppToFW(sAppName);

	//---------------------------------------------------------------------------
	// 根据前面设置的标志，决定是否“自杀”
	if (bDeleteMe)
	{
		HvUtility::DeleteMe();		// 必须放数据库初始化之前，否则，自杀不了。Why?
		return FALSE;
	}
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	// 初始化数据库
	CString sInfo;

	HvUtility::SetAppName(hInstance, m_tsAppName);

	// 建立数据库连接
	if (!m_SysDatabase.InitDatabase())
	{
		sInfo = _T("数据库连接失败!");
		sInfo += _T("\n请检查\n\t[网络连接]\n或\n\t[数据库服务器设置]\n是否有问题。\n\n退出系统!");

		AfxMessageBox(sInfo, MB_OK);
		return FALSE;
	}

	// 验证当前主机
	int nRet = m_SysDatabase.CheckHost(m_tsHostName, m_tsHostID);
	if (nRet)
	{
		if (nRet == HOST_INVALID || nRet == HOST_UNVERIFIED)
		{
			sInfo = _T("当前计算机为非法用户，请及时系统联系开发人员！");
			AfxMessageBox(sInfo, MB_OK);
			return FALSE;
		}
		if (nRet == HOST_EXPIRED)
		{
			sInfo = _T("当前计算机已超过了试用期，请及时系统联系开发人员！");
			AfxMessageBox(sInfo, MB_OK);
			return FALSE;
		}

		sInfo = _T("当前计算机尚未授权，请及时授权，以免影响使用！");
		AfxMessageBox(sInfo, MB_OK);
	}
	//---------------------------------------------------------------------------

	return TRUE;
}

// CUltraToolsApp message handlers

int CUltraToolsApp::ExitInstance() 
{
	CloseHandle(m_hMutex);

	// 删除临时文件夹
	tstring tsTem(m_sTemFilePath);
	HvUtility::DeleteMultiDirectory(tsTem);

	return CBCGPWinApp::ExitInstance();
}

// App command to run the dialog
void CUltraToolsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
