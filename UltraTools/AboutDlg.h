// CAboutDlg dialog used for App About

#ifndef ABOUT_DLG_H_
#define ABOUT_DLG_H_
#include "afxcmn.h"
#include "UI//CreditStatic.h"

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
	CBCGPURLLinkButton	m_btnURL;
	CBCGPButton			m_btnLogo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CCreditStatic	m_wndCredit;
	CString			m_sUserName;
	CString			m_sAuthorizerID;
	CString			m_sAuthorizerPassword;

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLogo();
	afx_msg void OnBtnAuthorization();
	afx_msg void OnBtnApply();
};


#endif