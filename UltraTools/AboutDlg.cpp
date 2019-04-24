// CAboutDlg dialog used for App About

#include "stdafx.h"
#include "Resource.h"
#include "UltraTools.h"
#include "AboutDlg.h"

#include "Utility/HvUtility.h"


CAboutDlg::CAboutDlg() : CBCGPDialog(CAboutDlg::IDD)
{
	EnableVisualManagerStyle();
	
	CString sTem;
	if (theApp.m_bOnTrial)
		sTem.Format(_T("试用剩余%d天"), theApp.m_nDateCntToExpired);
	else
		sTem.Format(_T("授权剩余%d天"), theApp.m_nDateCntToExpired);
	m_sUserName.Format(_T("%s[%s]：%s"), theApp.m_tsHostName.c_str(), theApp.m_tsHostID.c_str(), sTem);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
	DDX_Control(pDX, IDC_BTN_LOGO, m_btnLogo);
	DDX_Control(pDX, IDC_STATIC_CREDIT, m_wndCredit);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_sUserName);
	DDX_Text(pDX, IDC_EDIT_AUTHORIZER_ID, m_sAuthorizerID);
	DDX_Text(pDX, IDC_EDIT_AUTHORIZER_PASSWORD, m_sAuthorizerPassword);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_LOGO, &CAboutDlg::OnBtnLogo)
	ON_BN_CLICKED(IDC_BTN_AUTHORIZATION, &CAboutDlg::OnBtnAuthorization)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CAboutDlg::OnBtnApply)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	m_btnURL.SetTooltip(_T("点击访问开发者网站"));

	m_btnLogo.SetWindowText(_T(""));
	m_btnLogo.SetImage(IDB_LOGO);
	m_btnLogo.EnableWinXPTheme(FALSE);
	m_btnLogo.SizeToContent();
	m_btnLogo.SetTooltip(_T("鹰视科技"));

	//m_wndCredit.SubclassDlgItem(IDC_STATIC_CREDIT,this);
	CString sCredit;
	sCredit = _T("|红外图谱UltraTools系统\t||UltralTools 1.9.0.163202|Copyright (C) 2015-2016||");
	sCredit += _T("安徽鹰视科技有限公司（筹建）|Hawk Vision Technology (HVT)|版权所有    侵权必究||");
	sCredit += _T("IDB_LOGO2_BMP^||");
	sCredit += _T("研发团队\r||IDB_LOGO3_BMP^|鹰视科技|||");
	sCredit += _T("技术研发领域\r||变电站热故障红外在线监测|热红外成像仪、偏振成像仪|气体泄漏定性定量检测等|||");
	sCredit += _T("团队地址\r||合肥市屯溪路193号 合肥工业大学 科楼719/逸夫楼509（邮编：230009）|||");
	sCredit += _T("网址\r||http://www.hfut.edu.cn|||");
	sCredit += _T("email\r||hfutzrb@163.com|||");
	sCredit += _T("电话\r||0551-62901380，13956956705|||");
	sCredit += _T("传真\r||0551-62901380|||");
	sCredit += _T("★  ★  ★  ★  ★ \n||||||||||");
	m_wndCredit.SetCredits(sCredit,'|');
	m_wndCredit.SetSpeed(DISPLAY_FAST);
	m_wndCredit.SetTransparent();
	m_wndCredit.SetColor(BACKGROUND_COLOR, RGB(255,255,255));
	//m_wndCredit.SetBkImage(IDB_CREDIT_BACKGROUND);
	m_wndCredit.StartScrolling();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::OnBtnLogo()
{
	SendMessage(WM_COMMAND, MAKEWPARAM(IDC_COMPANY_URL, BN_CLICKED), (LPARAM)m_btnURL.GetSafeHwnd());
}

void CAboutDlg::OnBtnAuthorization()
{
	if (!UpdateData())
		return;

	static CString sLastPath;					// last access path
	CString sFileName(""), sCurPath, sTem;
	int nRet;

	sCurPath = sLastPath.IsEmpty() ? _T("") : sLastPath;

	CFileDialog dlgFile(TRUE);		// TRUE: Open；FALSE: Save as
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	dlgFile.m_ofn.nFilterIndex = 0;
	dlgFile.m_ofn.lpstrFilter = _T("授权文件(*.auf)\0*.auf\0\0");
	dlgFile.m_ofn.lpstrTitle = _T("请选择授权文件");
	dlgFile.m_ofn.lpstrInitialDir = sCurPath.GetLength()>0 ? sCurPath : NULL;
	dlgFile.m_ofn.lpstrFile = sFileName.GetBuffer(_MAX_PATH);	// 回传文件名（含路径）

	BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;
	sFileName.ReleaseBuffer();
	if (bRet)
		sLastPath = dlgFile.GetPathName();			// 完整文件名
	else
		return;

	HINSTANCE hInstance = AfxGetInstanceHandle();
	tstring tsRegFile = sFileName;
	HvUtility::SetHostID(hInstance, theApp.m_tsHostID, false);
	if (!HvUtility::Register(hInstance, tsRegFile, false, false))
	{
		AfxMessageBox(_T("授权文件授权失败!"));
		return;
	}

	tstring tsAuthorizerID(m_sAuthorizerID), tsAuthorizerPassword(m_sAuthorizerPassword);
	nRet = theApp.m_SysDatabase.AuthorizeHost(theApp.m_tsHostName, theApp.m_tsHostID,
												tsAuthorizerID, tsAuthorizerPassword);
	if (HOST_AUTHORIZED == nRet)
		sTem.Format(_T("给以下计算机授权成功：\n\n%s\n\n重启本软件以更新信息。"),
		theApp.m_tsHostName.c_str());
	else
		sTem.Format(_T("给以下计算机授权失败[err:%d]：\n\n%s\n"), nRet, theApp.m_tsHostName.c_str());
	AfxMessageBox(sTem);
}

void CAboutDlg::OnBtnApply()
{
	CString sFileName, sTem;
	int nType, nRet;
	tstring tsFileName, tsDateSpan, tsPriKey, tsPubKey, tsTem;
	bool bRet;

	// 选择授权申请文件存储文件夹，并指派文件名
	TCHAR szPath[MAX_PATH];								// 存放选择的目录路径 
	ZeroMemory(szPath, sizeof(szPath));
	BROWSEINFO bi;
	bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("请选择保存授权申请文件的目录：");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	// 弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if (lp && SHGetPathFromIDList(lp, szPath))
		sTem = szPath;
	else
		return;
	// 文档名格式：客户单位名，计算机名，申请人名，AppName.apf
	sFileName.Format(_T("%s%s，%s，%s，%s.dat"), sTem, theApp.m_tsOrganizationName.c_str(),
		theApp.m_tsHostName.c_str(), theApp.m_tsLoginName.c_str(), theApp.m_tsAppName.c_str());
	tsFileName = sFileName;

	HINSTANCE hInstance = AfxGetInstanceHandle();

	HvUtility::SetAppName(NULL, theApp.m_tsAppName, false);
	HvUtility::SetOrganizationID(NULL, theApp.m_tsOrganizationID, false);
	HvUtility::SetHostID(hInstance, theApp.m_tsHostID, false);
	bRet = HvUtility::Init(hInstance, nType, tsDateSpan, tsPriKey, tsPubKey);
	if (!bRet)
	{
		sTem.Format(_T("授权申请失败[err:%d]"), nType);
		AfxMessageBox(sTem);
		return;
	}
	HvUtility::SetStaffID(hInstance, theApp.m_tsLoginID, false);
	tsTem = _T("");
	nRet = HvUtility::ApplyForAuthorization(hInstance, tsFileName, theApp.m_tsLoginName, theApp.m_tsHostName,
											tsPriKey, tsTem, true);
	if (0 != nRet)
	{
		sTem.Format(_T("授权申请失败[err2:%d]"), nRet);
		AfxMessageBox(sTem);
		return;
	}
	sTem.Format(_T("授权申请文件已保存至指定文件夹：\n\n%s\n\n请勿更改文件名，将文件发送给授权者。"),
		tsFileName.c_str());
	AfxMessageBox(sTem);
}
