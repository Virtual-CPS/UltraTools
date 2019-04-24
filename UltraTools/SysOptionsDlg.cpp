// CSysOptionsDlg dialog used for App About

#include "stdafx.h"
#include "UltraTools.h"
#include "SysOptionsDlg.h"

CSysOptionsDlg::CSysOptionsDlg() : CBCGPDialog(CSysOptionsDlg::IDD)
{
	EnableVisualManagerStyle();
}

CSysOptionsDlg::~CSysOptionsDlg()
{
}

void CSysOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSysOptionsDlg, CBCGPDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CSysOptionsDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	SetIcon(AfxGetApp()->LoadIcon(IDI_MANAGE), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSysOptionsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 不为绘图消息调用 CBCGPDialog::OnPaint()
}

void CSysOptionsDlg::OnOK()
{
	if(!UpdateData())
		return;

	CBCGPDialog::OnOK();
}
