   #if !defined(AFX_IRCamerMeasurementGridCtrl_H__158297E8_B1EB_4F44_AA8B_7DB752851B6E__INCLUDED_)
#define AFX_IRCamerMeasurementGridCtrl_H__158297E8_B1EB_4F44_AA8B_7DB752851B6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IRCameraMeasurementGridCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIRCameraMeasurementGridCtrl window

class CIRCameraMeasurementGridCtrl : public CBCGPGridCtrl
{
// Construction
public:
	CIRCameraMeasurementGridCtrl();

// Attributes
protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIRCameraMeasurementGridCtrl)
	//}}AFX_VIRTUAL

	virtual int CompareGroup (const CBCGPGridRow* pRow1, const CBCGPGridRow* pRow2, int iColumn);
	virtual CString GetGroupName (int nGroupCol, CBCGPGridItem* pItem);

// Implementation
public:
	virtual ~CIRCameraMeasurementGridCtrl();
	CBCGPGridRow* CreateNewRow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIRCameraMeasurementGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IRCamerMeasurementGridCtrl_H__158297E8_B1EB_4F44_AA8B_7DB752851B6E__INCLUDED_)
