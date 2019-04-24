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
		sTem.Format(_T("����ʣ��%d��"), theApp.m_nDateCntToExpired);
	else
		sTem.Format(_T("��Ȩʣ��%d��"), theApp.m_nDateCntToExpired);
	m_sUserName.Format(_T("%s[%s]��%s"), theApp.m_tsHostName.c_str(), theApp.m_tsHostID.c_str(), sTem);
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

	m_btnURL.SetTooltip(_T("������ʿ�������վ"));

	m_btnLogo.SetWindowText(_T(""));
	m_btnLogo.SetImage(IDB_LOGO);
	m_btnLogo.EnableWinXPTheme(FALSE);
	m_btnLogo.SizeToContent();
	m_btnLogo.SetTooltip(_T("ӥ�ӿƼ�"));

	//m_wndCredit.SubclassDlgItem(IDC_STATIC_CREDIT,this);
	CString sCredit;
	sCredit = _T("|����ͼ��UltraToolsϵͳ\t||UltralTools 1.9.0.163202|Copyright (C) 2015-2016||");
	sCredit += _T("����ӥ�ӿƼ����޹�˾���ｨ��|Hawk Vision Technology (HVT)|��Ȩ����    ��Ȩ�ؾ�||");
	sCredit += _T("IDB_LOGO2_BMP^||");
	sCredit += _T("�з��Ŷ�\r||IDB_LOGO3_BMP^|ӥ�ӿƼ�|||");
	sCredit += _T("�����з�����\r||���վ�ȹ��Ϻ������߼��|�Ⱥ�������ǡ�ƫ�������|����й©���Զ�������|||");
	sCredit += _T("�Ŷӵ�ַ\r||�Ϸ�����Ϫ·193�� �Ϸʹ�ҵ��ѧ ��¥719/�ݷ�¥509���ʱࣺ230009��|||");
	sCredit += _T("��ַ\r||http://www.hfut.edu.cn|||");
	sCredit += _T("email\r||hfutzrb@163.com|||");
	sCredit += _T("�绰\r||0551-62901380��13956956705|||");
	sCredit += _T("����\r||0551-62901380|||");
	sCredit += _T("��  ��  ��  ��  �� \n||||||||||");
	m_wndCredit.SetCredits(sCredit,'|');
	m_wndCredit.SetSpeed(DISPLAY_FAST);
	m_wndCredit.SetTransparent();
	m_wndCredit.SetColor(BACKGROUND_COLOR, RGB(255,255,255));
	//m_wndCredit.SetBkImage(IDB_CREDIT_BACKGROUND);
	m_wndCredit.StartScrolling();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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

	CFileDialog dlgFile(TRUE);		// TRUE: Open��FALSE: Save as
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	dlgFile.m_ofn.nFilterIndex = 0;
	dlgFile.m_ofn.lpstrFilter = _T("��Ȩ�ļ�(*.auf)\0*.auf\0\0");
	dlgFile.m_ofn.lpstrTitle = _T("��ѡ����Ȩ�ļ�");
	dlgFile.m_ofn.lpstrInitialDir = sCurPath.GetLength()>0 ? sCurPath : NULL;
	dlgFile.m_ofn.lpstrFile = sFileName.GetBuffer(_MAX_PATH);	// �ش��ļ�������·����

	BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;
	sFileName.ReleaseBuffer();
	if (bRet)
		sLastPath = dlgFile.GetPathName();			// �����ļ���
	else
		return;

	HINSTANCE hInstance = AfxGetInstanceHandle();
	tstring tsRegFile = sFileName;
	HvUtility::SetHostID(hInstance, theApp.m_tsHostID, false);
	if (!HvUtility::Register(hInstance, tsRegFile, false, false))
	{
		AfxMessageBox(_T("��Ȩ�ļ���Ȩʧ��!"));
		return;
	}

	tstring tsAuthorizerID(m_sAuthorizerID), tsAuthorizerPassword(m_sAuthorizerPassword);
	nRet = theApp.m_SysDatabase.AuthorizeHost(theApp.m_tsHostName, theApp.m_tsHostID,
												tsAuthorizerID, tsAuthorizerPassword);
	if (HOST_AUTHORIZED == nRet)
		sTem.Format(_T("�����¼������Ȩ�ɹ���\n\n%s\n\n����������Ը�����Ϣ��"),
		theApp.m_tsHostName.c_str());
	else
		sTem.Format(_T("�����¼������Ȩʧ��[err:%d]��\n\n%s\n"), nRet, theApp.m_tsHostName.c_str());
	AfxMessageBox(sTem);
}

void CAboutDlg::OnBtnApply()
{
	CString sFileName, sTem;
	int nType, nRet;
	tstring tsFileName, tsDateSpan, tsPriKey, tsPubKey, tsTem;
	bool bRet;

	// ѡ����Ȩ�����ļ��洢�ļ��У���ָ���ļ���
	TCHAR szPath[MAX_PATH];								// ���ѡ���Ŀ¼·�� 
	ZeroMemory(szPath, sizeof(szPath));
	BROWSEINFO bi;
	bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("��ѡ�񱣴���Ȩ�����ļ���Ŀ¼��");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	// ����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if (lp && SHGetPathFromIDList(lp, szPath))
		sTem = szPath;
	else
		return;
	// �ĵ�����ʽ���ͻ���λ�����������������������AppName.apf
	sFileName.Format(_T("%s%s��%s��%s��%s.dat"), sTem, theApp.m_tsOrganizationName.c_str(),
		theApp.m_tsHostName.c_str(), theApp.m_tsLoginName.c_str(), theApp.m_tsAppName.c_str());
	tsFileName = sFileName;

	HINSTANCE hInstance = AfxGetInstanceHandle();

	HvUtility::SetAppName(NULL, theApp.m_tsAppName, false);
	HvUtility::SetOrganizationID(NULL, theApp.m_tsOrganizationID, false);
	HvUtility::SetHostID(hInstance, theApp.m_tsHostID, false);
	bRet = HvUtility::Init(hInstance, nType, tsDateSpan, tsPriKey, tsPubKey);
	if (!bRet)
	{
		sTem.Format(_T("��Ȩ����ʧ��[err:%d]"), nType);
		AfxMessageBox(sTem);
		return;
	}
	HvUtility::SetStaffID(hInstance, theApp.m_tsLoginID, false);
	tsTem = _T("");
	nRet = HvUtility::ApplyForAuthorization(hInstance, tsFileName, theApp.m_tsLoginName, theApp.m_tsHostName,
											tsPriKey, tsTem, true);
	if (0 != nRet)
	{
		sTem.Format(_T("��Ȩ����ʧ��[err2:%d]"), nRet);
		AfxMessageBox(sTem);
		return;
	}
	sTem.Format(_T("��Ȩ�����ļ��ѱ�����ָ���ļ��У�\n\n%s\n\n��������ļ��������ļ����͸���Ȩ�ߡ�"),
		tsFileName.c_str());
	AfxMessageBox(sTem);
}
