//
// IRCameraMeasurementGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "IRCameraMeasurementGridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning (default : 4245)

/////////////////////////////////////////////////////////////////////////////
// CIRCameraMeasurementGridCtrl

CIRCameraMeasurementGridCtrl::CIRCameraMeasurementGridCtrl()
{
}

CIRCameraMeasurementGridCtrl::~CIRCameraMeasurementGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CIRCameraMeasurementGridCtrl, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CIRCameraMeasurementGridCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIRCameraMeasurementGridCtrl message handlers

int CIRCameraMeasurementGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

int CIRCameraMeasurementGridCtrl::CompareGroup (const CBCGPGridRow* pRow1, const CBCGPGridRow* pRow2, int /*iColumn*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pRow1);
	ASSERT_VALID (pRow2);

	CString sStr1 = (CString) pRow1->GetItem (0)->GetValue ();
	CString sStr2 = (CString) pRow2->GetItem (0)->GetValue ();

	return sStr1.CompareNoCase(sStr2);
}

CString CIRCameraMeasurementGridCtrl::GetGroupName (int /*iColumn*/, CBCGPGridItem* pItem)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pItem);

	CString str = (CString) pItem->GetValue ();

	return str;
}

CBCGPGridRow* CIRCameraMeasurementGridCtrl::CreateNewRow ()
{
	int nColCnt = GetColumnCount();
	CBCGPGridRow* pRow = CreateRow(nColCnt);

	for (int i=0; i<nColCnt; i++)
	{
		pRow->GetItem(i)->AllowEdit(FALSE);
	}

	return pRow;
}
