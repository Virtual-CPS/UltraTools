// CSysOptionsDlg dialog used for App About

#ifndef SYS_OPTIONS_DLG_H_
#define SYS_OPTIONS_DLG_H_

#include "afxcmn.h"

class CSysOptionsDlg : public CBCGPDialog
{
public:
	CSysOptionsDlg();
	virtual ~CSysOptionsDlg();

// Dialog Data
	enum { IDD = IDD_SYS_OPTIONS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};


#endif