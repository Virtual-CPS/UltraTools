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

	// ��Դ������
	m_pDocTemplateExplorer = new CMultiDocTemplate(
							IDR_EXPLORER,
							RUNTIME_CLASS(CUltraToolsDoc),
							RUNTIME_CLASS(CExplorerChildFrame),	// �Զ��� MDI �ӿ��
							RUNTIME_CLASS(CExplorerView));
	if (!m_pDocTemplateExplorer)
		return FALSE;
	AddDocTemplate(m_pDocTemplateExplorer);

	// ����ͼ�׷���
	m_pDocTemplateAnalysis = new CMultiDocTemplate(
							IDR_ANALYSIS,
							RUNTIME_CLASS(CUltraToolsDoc),
							RUNTIME_CLASS(CAnalysisChildFrame),	// �Զ��� MDI �ӿ��
							RUNTIME_CLASS(CAnalysisView));
	if (!m_pDocTemplateAnalysis)
		return FALSE;
	AddDocTemplate(m_pDocTemplateAnalysis);

	// ��������
	m_pDocTemplateReport = new CMultiDocTemplate(
							IDR_REPORT,
							RUNTIME_CLASS(CUltraToolsDoc),
							RUNTIME_CLASS(CReportChildFrame),// �Զ��� MDI �ӿ��
							RUNTIME_CLASS(CReportView));
	if (!m_pDocTemplateReport)
		return FALSE;
	AddDocTemplate(m_pDocTemplateReport);

	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	//cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;// ����ʱ���Զ���һ�����ĵ� 
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		m_pDocTemplateExplorer->OpenDocumentFile(NULL);
		//m_pDocTemplateAnalysis->OpenDocumentFile (NULL);
	}
	else
	{
		// ��������������ָ����������
		// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	// �������ѳ�ʼ���������ʾ����������и���
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

	// ֻ�����е���ʵ��, �û��������жϳ����Ƿ�������
	//m_hMutex = CreateMutex(NULL,TRUE, m_pszAppName); 
	m_hMutex = CreateMutex(NULL, TRUE, _T("UltraTools"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(_T("��⵽��ϵͳ��������!"), MB_OK);
		return FALSE;
	}

	// ������ʱ�ļ�·��
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
	// ��ȡ��¼��Ϣ
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
	if (gethostname((char*)(szHostName), 128) == 0)		// ȡ�����������ɹ�
		m_tsHostName = HvUtility::Char2String(szHostName);
	else
		m_tsHostName = _T("");

	HINSTANCE hInstance = AfxGetInstanceHandle();

	// ��ʼ���ӽ��ܲ���
	m_tsAppName = _T("UltraTools");
	m_tsManufacturerID = _T("HawkVision");
	HvUtility::SetAppName(NULL, m_tsAppName, false);
	HvUtility::SetOrganizationID(NULL, m_tsOrganizationID, false);

	int nType;
	tstring tsDateSpan, tsPriKey, tsPubKey, tsTem;
	HvUtility::GetHostID(NULL, m_tsHostID, nType, tsDateSpan, tsPriKey, tsPubKey, true);	// ��ȡ����ID
	HvUtility::SetHostID(hInstance, m_tsHostID);

	// ��ע��������Ƿ���ɱ����־
	BOOL bDeleteMe = TRUE;
	if (HvUtility::Init(hInstance, nType, tsDateSpan, tsPriKey, tsPubKey))
	{
		HvUtility::Decrypt(tsDateSpan, tsTem);
		m_nDateCntToExpired = _tstoi(tsTem.c_str());
	}
	else
	{
		// ����Ԥ��Ȩ
		CString sRegFile;
		tstring tsRegFile;
		sRegFile.Format(_T("%s%s��%s.paf"), m_sAppPath, m_tsOrganizationName.c_str(), m_tsAppName.c_str());
		tsRegFile.assign(sRegFile);
		HvUtility::SetAppName(NULL, m_tsAppName, false);					// ����ǰ��Init()ʧ�ܵ��µ�Ӱ��
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

	// ����������ӵ�����ǽ�ġ����⡱
	//CString sAppName;
	//sAppName.LoadString(AFX_IDS_APP_TITLE);
	//AddAppToFW(sAppName);

	//---------------------------------------------------------------------------
	// ����ǰ�����õı�־�������Ƿ���ɱ��
	if (bDeleteMe)
	{
		HvUtility::DeleteMe();		// ��������ݿ��ʼ��֮ǰ��������ɱ���ˡ�Why?
		return FALSE;
	}
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	// ��ʼ�����ݿ�
	CString sInfo;

	HvUtility::SetAppName(hInstance, m_tsAppName);

	// �������ݿ�����
	if (!m_SysDatabase.InitDatabase())
	{
		sInfo = _T("���ݿ�����ʧ��!");
		sInfo += _T("\n����\n\t[��������]\n��\n\t[���ݿ����������]\n�Ƿ������⡣\n\n�˳�ϵͳ!");

		AfxMessageBox(sInfo, MB_OK);
		return FALSE;
	}

	// ��֤��ǰ����
	int nRet = m_SysDatabase.CheckHost(m_tsHostName, m_tsHostID);
	if (nRet)
	{
		if (nRet == HOST_INVALID || nRet == HOST_UNVERIFIED)
		{
			sInfo = _T("��ǰ�����Ϊ�Ƿ��û����뼰ʱϵͳ��ϵ������Ա��");
			AfxMessageBox(sInfo, MB_OK);
			return FALSE;
		}
		if (nRet == HOST_EXPIRED)
		{
			sInfo = _T("��ǰ������ѳ����������ڣ��뼰ʱϵͳ��ϵ������Ա��");
			AfxMessageBox(sInfo, MB_OK);
			return FALSE;
		}

		sInfo = _T("��ǰ�������δ��Ȩ���뼰ʱ��Ȩ������Ӱ��ʹ�ã�");
		AfxMessageBox(sInfo, MB_OK);
	}
	//---------------------------------------------------------------------------

	return TRUE;
}

// CUltraToolsApp message handlers

int CUltraToolsApp::ExitInstance() 
{
	CloseHandle(m_hMutex);

	// ɾ����ʱ�ļ���
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
