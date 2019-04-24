/////////////////////////////////////////////////////////////////////////////
// CIRCameraMeasurementDockingBar frame
#if !defined(IR_CAMERA_MEASURE_DOCKING_CONTROL_BAR_H_)
#define IR_CAMERA_MEASURE_DOCKING_CONTROL_BAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "IRCameraMeasurementWnd.h"

class CIRCameraMeasurementDockingBar : public CBCGPDockingControlBar
{
	DECLARE_DYNAMIC(CIRCameraMeasurementDockingBar)

public:
	CIRCameraMeasurementDockingBar();

// Attributes
public:
	CIRCameraMeasurementWnd		m_wndIRCameraMeasurement;
	virtual ~CIRCameraMeasurementDockingBar();
	CWnd*		m_pOwner;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIRCameraMeasurementDockingBar)
	public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIRCameraMeasurementDockingBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif