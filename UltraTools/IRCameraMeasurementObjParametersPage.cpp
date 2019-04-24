// IRCameraMeasurementObjParametersPage.cpp : 实现文件
//

#include "stdafx.h"
#include "UltraTools.h"
#include "IRCameraMeasurementObjParametersPage.h"
#include "IRCameraMeasurementDockingBar.h"
#include "AnalysisView.h"

// CIRCameraMeasurementObjParametersPage 对话框

IMPLEMENT_DYNAMIC(CIRCameraMeasurementObjParametersPage, CBCGPDialog)

CIRCameraMeasurementObjParametersPage::CIRCameraMeasurementObjParametersPage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CIRCameraMeasurementObjParametersPage::IDD, pParent)
{
	EnableVisualManagerStyle();

	m_fEmissivity = 0.001f;
	m_fReflectedTemp = 0.0f;
	m_fAtmosphericTemp = 0.0f;
	m_fRelativeHumidity = 0.0f;
	m_fDistance = 0.0f;

	m_fEstAtmTransm = 0.0f;
	m_fExtOptTemp = 1.0f;
	m_fExtOptTransm = 1.0f;

	m_fReferenceTemp = 0.0f;
	m_fFeverPartTemp = 0.0f;
	m_fNormalTemp = 0.0f;
	m_fTempDifference = 0.0f;
	m_fRelativeTempDifference = 0.0f;
}

CIRCameraMeasurementObjParametersPage::~CIRCameraMeasurementObjParametersPage()
{
	int nCnt = m_menuEmissivity.GetMenuItemCount();
	CMenu* pMenu;
	for (int i=nCnt; i>=0; i--)
	{
		pMenu = m_menuEmissivity.GetSubMenu(i);
		delete pMenu;
	}
}

void CIRCameraMeasurementObjParametersPage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EMISSIVITY, m_fEmissivity);
	DDV_MinMaxFloat(pDX, m_fEmissivity, 0.001f, 1.f);
	DDX_Text(pDX, IDC_EDIT_REFLECTED_TEMP, m_fReflectedTemp);
	DDV_MinMaxFloat(pDX, m_fReflectedTemp, -80.f, 2000.f);
	DDX_Text(pDX, IDC_EDIT_ATMOSPHERIC_TEMP, m_fAtmosphericTemp);
	DDV_MinMaxFloat(pDX, m_fAtmosphericTemp, -80.f, 300.f);
	DDX_Text(pDX, IDC_EDIT_RELATIVE_HUMIDITY, m_fRelativeHumidity);
	DDV_MinMaxFloat(pDX, m_fRelativeHumidity, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_fDistance);
	DDV_MinMaxFloat(pDX, m_fDistance, 0.f, 4000.f);

	DDX_Text(pDX, IDC_EDIT_EST_ATM_TRANSM, m_fEstAtmTransm);
	DDV_MinMaxFloat(pDX, m_fEstAtmTransm, 0.f, 4000.f);
	DDX_Text(pDX, IDC_EDIT_EXT_OPT_TEMP, m_fExtOptTemp);
	DDV_MinMaxFloat(pDX, m_fExtOptTemp, -80.f, 2000.f);
	DDX_Text(pDX, IDC_EDIT_EXT_OPT_TRANSM, m_fExtOptTransm);
	DDV_MinMaxFloat(pDX, m_fExtOptTransm, 0.001f, 1.f);

	DDX_Control(pDX, IDC_EDIT_EMISSIVITY, m_edtEmissivity);
	DDX_Control(pDX, IDC_BTN_EMISSIVITY, m_btnEmissivity);
	DDX_Control(pDX, IDC_LIST_POSITION, m_wndListPosition);

	DDX_Text(pDX, IDC_EDIT_REFERENCE_TEMP, m_fReferenceTemp);
	DDX_Text(pDX, IDC_EDIT_FEVER_PART_TEMP, m_fFeverPartTemp);
	DDX_Text(pDX, IDC_EDIT_NORMAL_TEMP, m_fNormalTemp);
	DDX_Text(pDX, IDC_EDIT_TEMP_DIFFERENCE, m_fTempDifference);
	DDX_Text(pDX, IDC_EDIT_RELATIVE_TEMP_DIFFERENCE, m_fRelativeTempDifference);
}

BEGIN_MESSAGE_MAP(CIRCameraMeasurementObjParametersPage, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	ON_BN_CLICKED(IDC_BTN_EMISSIVITY, OnBtnEmissivity)
END_MESSAGE_MAP()

// CIRCameraMeasurementObjParametersPage 消息处理程序

BOOL CIRCameraMeasurementObjParametersPage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CreateEmissivityMenu();
	m_btnEmissivity.SetTooltip(_T("根据材质选择发射率"));
	InitMeasurementDataList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 只在初始化对话框时初始化menu一次
void CIRCameraMeasurementObjParametersPage::CreateEmissivityMenu()
{
/*	CStringArray saMaterialID, saMaterialName, saEmissivityID, saEmissivityName, saEmissivityMaterialID;
	CStringArray saTem1, saTem2, saTem3;

	theApp.m_SysDatabase.GetMaterialType(saMaterialID, saMaterialName, saTem1);
	theApp.m_SysDatabase.GetEmissivity(_T(""), saEmissivityID, saEmissivityName, saEmissivityMaterialID,
		m_saEmissivity, saTem1, saTem2, saTem3);

	int nMaterialCnt, nEmissivityCnt;
	nMaterialCnt = (int)(saMaterialID.GetCount());
	nEmissivityCnt = (int)(m_saEmissivity.GetCount());
	if(nMaterialCnt<=0 || nEmissivityCnt<=0)
		return;

	VERIFY(m_menuEmissivity.CreateMenu());

	CString sMenuItem, sTem;
	for (int i=0; i<nMaterialCnt; i++)
	{
		CMenu* pMenu = new(CMenu);

		VERIFY(pMenu->CreatePopupMenu());
		BOOL bAdded = FALSE;

		for (int j=0; j<nEmissivityCnt; j++)
		{
			if(saMaterialID.GetAt(i) == saEmissivityMaterialID.GetAt(j))
			{
				sMenuItem.Format("%s =%s", saEmissivityName.GetAt(j), m_saEmissivity.GetAt(j));
				pMenu->AppendMenu(MF_STRING, IDM_EMISSIVITY_MENU_BEGIN+j, sMenuItem);
				bAdded = TRUE;
			}
		}

		if(bAdded)
		{
			sMenuItem = saMaterialName.GetAt(i);
			m_menuEmissivity.AppendMenu(MF_POPUP, (UINT_PTR)pMenu->GetSafeHmenu(), sMenuItem);
		}
		else
			delete pMenu;
	}

	m_btnEmissivity.m_hMenu = m_menuEmissivity.GetSafeHmenu ();
	m_btnEmissivity.m_bOSMenu = FALSE;
	m_btnEmissivity.m_bRightArrow = TRUE;
	m_btnEmissivity.m_bDefaultClick = FALSE;
*/}

// 设置测温参数
void CIRCameraMeasurementObjParametersPage::OnBtnApply()
{
	if(!UpdateData())
		return;

	tagIRCameraMeasurementParameters parameters;
	parameters.Reset();

	parameters.fEmissivity = m_fEmissivity;
	parameters.fReflectedTemp = m_fReflectedTemp;
	parameters.fAtmosphericTemp = m_fAtmosphericTemp;
	parameters.fRelativeHumidity = m_fRelativeHumidity / 100.f;
	parameters.fDistance = m_fDistance;

	parameters.fEstAtmTransm = m_fEstAtmTransm;
	parameters.fExtOptTemp = m_fExtOptTemp;
	parameters.fExtOptTransm = m_fExtOptTransm;

	CWnd* pParent = GetParent();
	if(pParent)										// TAB
	{
		pParent = pParent->GetParent();
		if(pParent)									// MesurementWnd
		{
			pParent = pParent->GetParent();
			if(pParent)								// dockingBar
			{
				//AfxMessageBox(pParent->GetRuntimeClass()->m_lpszClassName);
				CIRCameraMeasurementDockingBar* pBar = (CIRCameraMeasurementDockingBar *)pParent;
				((CAnalysisView *)pBar->m_pOwner)->m_wndLoadImage.SetMeasurementParameters(parameters);
			}
		}
	}
}

// 获取测温参数
void CIRCameraMeasurementObjParametersPage::UpdateCameraMeasurementParameters(tagIRCameraMeasurementParameters& parameters)
{
	m_fEmissivity = parameters.fEmissivity;
	m_fReflectedTemp = parameters.fReflectedTemp;
	m_fAtmosphericTemp = parameters.fAtmosphericTemp;
	m_fRelativeHumidity = parameters.fRelativeHumidity * 100.f;
	m_fDistance = parameters.fDistance;

	m_fEstAtmTransm = parameters.fEstAtmTransm;
	m_fExtOptTemp = parameters.fExtOptTemp;
	m_fExtOptTransm = parameters.fExtOptTransm;

	UpdateData(FALSE);
}

void CIRCameraMeasurementObjParametersPage::InitMeasurementDataList()
{
	CRect rectList;
	m_wndListPosition.GetClientRect (&rectList);
	m_wndListPosition.MapWindowPoints (this, &rectList);
	//m_wndListPosition.SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);

	m_List.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectList, this, (UINT)-1);

	//-------------
	// Add columns:
	//-------------
	m_List.InsertColumn (0, _T("分组"), 10);
	m_List.InsertColumn (1, _T("标签"), 70);
	m_List.InsertColumn (2, _T("最大值"), 70);
	m_List.InsertColumn (3, _T("最小值"), 70);
	m_List.InsertColumn (4, _T("平均值"), 70);

	m_List.SetScrollBarsStyle (CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);
	//m_List.EnableHeader (TRUE, ~BCGP_GRID_HEADER_MOVE_ITEMS & ~BCGP_GRID_HEADER_SORT);
	m_List.EnableHeader (TRUE, ~BCGP_GRID_HEADER_MOVE_ITEMS);

	//-----------------------
	// Group by first column:
	//-----------------------
	//m_List.InsertGroupColumn (0, 0);
	m_List.SetColumnVisible(0, FALSE);
	m_List.SetSortColumn(1);				// 强制排序

	m_List.AdjustLayout ();
}

// 更新测量数据
void CIRCameraMeasurementObjParametersPage::UpdateMeasurementDataList(CAnalysisView* pView)
{
	if(pView == NULL)
		return;

	CBCGPGridRow* pRow;
	CString sGroup, sObjMark, sTem;
	POSITION pos;
	CDrawRect* pRect;
	CDrawRect::Shape eShape;
	int nIndex = 0;

	m_fReferenceTemp = 0.0f;
	m_fFeverPartTemp = 0.0f;
	m_fNormalTemp = 0.0f;
	m_fTempDifference = 0.0f;
	m_fRelativeTempDifference = 0.0f;

	// 分类更新Obj的序号（index based on zero）

	m_List.RemoveAll();
	m_List.AdjustLayout ();					// 及时刷新

//	m_List.SetSortColumn(1);				// 强制排序

	sGroup = _T("0图谱");
	sObjMark = _T("0-图谱");
	pRow = m_List.CreateNewRow();
	pRow->GetItem(0)->SetValue((LPCTSTR)sGroup);
	pRow->GetItem(1)->SetValue((LPCTSTR)sObjMark);
	sTem.Format(_T("%.1f"), pView->m_wndLoadImage.m_fMaxTemperature);
	pRow->GetItem(2)->SetValue((LPCTSTR)sTem);
	sTem.Format(_T("%.1f"), pView->m_wndLoadImage.m_fMinTemperature);
	pRow->GetItem(3)->SetValue((LPCTSTR)sTem);
	m_List.AddRow (pRow, FALSE);

	pos = pView->m_wndLoadImage.m_wndImage.m_objListObjects.GetHeadPosition();
	while (pos != NULL)
	{
		pRect =  (CDrawRect*)(pView->m_wndLoadImage.m_wndImage.m_objListObjects.GetNext(pos));
		eShape = pRect->GetShapType();

		if(nIndex == 0)
			m_fFeverPartTemp = pRect->m_faValues[0];
		else if(nIndex == 1)
			m_fReferenceTemp = pRect->m_faValues[0];
		else if(nIndex == 2)
			m_fNormalTemp = pRect->m_faValues[0];
		nIndex++;

		if(eShape == CDrawRect::RECT_SHAPE_SPOT)
		{
			sGroup = _T("3点温");
			sObjMark.Format(_T("3-点温%02d"), pRect->m_nIndex+1);
			pRow = m_List.CreateNewRow();

			pRow->GetItem(0)->SetValue((LPCTSTR)sGroup);
			pRow->GetItem(1)->SetValue((LPCTSTR)sObjMark);
			sTem.Format(_T("%.1f"), pRect->m_faValues[0]);
			pRow->GetItem(2)->SetValue((LPCTSTR)sTem);

			m_List.AddRow (pRow, FALSE);
		}
		else if (eShape == CDrawRect::RECT_SHAPE_LINE)
		{
			sGroup = _T("2线温");
			sObjMark.Format(_T("2-线温%02d"), pRect->m_nIndex+1);
			pRow = m_List.CreateNewRow();

			pRow->GetItem(0)->SetValue((LPCTSTR)sGroup);
			pRow->GetItem(1)->SetValue((LPCTSTR)sObjMark);
			sTem.Format(_T("%.1f"), pRect->m_faValues[0]);
			pRow->GetItem(2)->SetValue((LPCTSTR)sTem);
			sTem.Format(_T("%.1f"), pRect->m_faValues[1]);
			pRow->GetItem(3)->SetValue((LPCTSTR)sTem);
			sTem.Format(_T("%.1f"), pRect->m_faValues[2]);
			pRow->GetItem(4)->SetValue((LPCTSTR)sTem);

			m_List.AddRow (pRow, FALSE);
		}
		else if(eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
		{
			sGroup = _T("1区温");
			sObjMark.Format(_T("1-区温%02d"), pRect->m_nIndex+1);
			pRow = m_List.CreateNewRow();

			pRow->GetItem(0)->SetValue((LPCTSTR)sGroup);
			pRow->GetItem(1)->SetValue((LPCTSTR)sObjMark);
			sTem.Format(_T("%.1f"), pRect->m_faValues[0]);
			pRow->GetItem(2)->SetValue((LPCTSTR)sTem);
			sTem.Format(_T("%.1f"), pRect->m_faValues[1]);
			pRow->GetItem(3)->SetValue((LPCTSTR)sTem);
			sTem.Format(_T("%.1f"), pRect->m_faValues[2]);
			pRow->GetItem(4)->SetValue((LPCTSTR)sTem);

			m_List.AddRow (pRow, FALSE);
		}

		//m_List.AdjustLayout();				// 及时刷新
	}
	m_List.AdjustLayout();

	m_fTempDifference = m_fFeverPartTemp - m_fNormalTemp;
	if(m_fFeverPartTemp != m_fReferenceTemp)
		m_fRelativeTempDifference = (m_fFeverPartTemp-m_fNormalTemp)/(m_fFeverPartTemp-m_fReferenceTemp)*100.f;
	else
		m_fRelativeTempDifference = 0.f;
	UpdateData(FALSE);

	if(!(m_fFeverPartTemp==m_fReferenceTemp && m_fReferenceTemp==m_fNormalTemp))
	{											// 非初始化状态
		CWnd* pParent = GetParent();
		if(pParent)								// TAB
		{
			pParent = pParent->GetParent();
			if(pParent)							// MesurementWnd
			{
				//AfxMessageBox(pParent->GetRuntimeClass()->m_lpszClassName);
				CIRCameraMeasurementWnd* pMeasurementWnd = (CIRCameraMeasurementWnd *)pParent;
				pMeasurementWnd->m_pageAnalysis.SetTemperatureCalculationResult(m_fTempDifference, m_fRelativeTempDifference);
			}
		}
	}

//	m_List.RemoveSortColumn(1);				// 去掉排序标志
}

void CIRCameraMeasurementObjParametersPage::OnBtnEmissivity()
{
	int nSel = m_btnEmissivity.m_nMenuResult;
	nSel = nSel - (int)(IDM_EMISSIVITY_MENU_BEGIN);
	ASSERT(nSel>=0 && nSel<m_saEmissivity.GetCount());

	m_edtEmissivity.SetWindowText(m_saEmissivity.GetAt(nSel));
	m_edtEmissivity.SetFocus();
	m_edtEmissivity.SetSel(0, 999);
}
