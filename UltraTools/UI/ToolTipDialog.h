// File : ToolTipDialog.h

#ifndef TOOL_TIP_DIALOG_H_
#define TOOL_TIP_DIALOG_H_

#ifndef __AFXWIN_H__
#include <AfxWin.h>
#endif

#ifndef __AFXCMN_H__
#include <AfxCmn.h>
#endif

class CToolTipDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(CToolTipDialog)
	DECLARE_MESSAGE_MAP()
public:
	CToolTipDialog();
	CToolTipDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CToolTipDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CToolTipDialog();
	static void EnableToolTips(BOOL bEnable);
protected:
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	CToolTipCtrl m_wndToolTip;
	static BOOL c_bShowToolTips;
private:
	void ListBoxTextExpand(MSG* pMsg);
};

// #else
// #error file ToolTipDialog.h included more than once.
#endif