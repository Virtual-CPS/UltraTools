 // IRCameraMeasureWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "IRCameraMeasurementWnd.h"
#include "IRCameraMeasurementDockingBar.h"
#include "AnalysisView.h"


// CIRCameraMeasurementWnd 对话框

IMPLEMENT_DYNAMIC(CIRCameraMeasurementWnd, CBCGPDialog)

CIRCameraMeasurementWnd::CIRCameraMeasurementWnd(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CIRCameraMeasurementWnd::IDD, pParent)
{
	EnableVisualManagerStyle ();
}

CIRCameraMeasurementWnd::~CIRCameraMeasurementWnd()
{
}

void CIRCameraMeasurementWnd::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_POSITION, m_wndTabPosition);
}


BEGIN_MESSAGE_MAP(CIRCameraMeasurementWnd, CBCGPDialog)
END_MESSAGE_MAP()


// CIRCameraMeasurementWnd 消息处理程序

BOOL CIRCameraMeasurementWnd::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	InitTab();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIRCameraMeasurementWnd::InitTab()
{
	if (m_wndTab.GetSafeHwnd() != NULL)
		return;

	CRect rectTab;

	m_wndTabPosition.GetWindowRect(&rectTab);
	ScreenToClient(&rectTab);
	//m_wndTabPosition.SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);

	m_wndTab.Create(CBCGPTabWnd::STYLE_3D, rectTab, this, 1, CBCGPTabWnd::LOCATION_TOP);
	m_wndTab.SetImageList(IDB_IR_CAMERA_MEASUREMENT_TAB_ICONS, 16, RGB (255, 0, 255));
	m_wndTab.ModifyTabStyle(CBCGPTabWnd::STYLE_3D_ONENOTE);
	m_wndTab.EnableTabSwap(FALSE);		// 不允许改变TAB先后顺序

	m_pageObjParameters.Create(CIRCameraMeasurementObjParametersPage::IDD, &m_wndTab);
	m_pageImageProcess.Create(CIRCameraMeasurementImageProcessPage::IDD, &m_wndTab);
	m_pageAnalysis.Create(CIRCameraMeasurementAnalysisPage::IDD, &m_wndTab);

	m_wndTab.AddTab (&m_pageObjParameters, _T("参数调整与测温"), 0, FALSE);
	m_wndTab.AddTab (&m_pageImageProcess, _T("图谱调整"), 1, FALSE);
	m_wndTab.AddTab (&m_pageAnalysis, _T("分析诊断"), 1, FALSE);
}

void CIRCameraMeasurementWnd::UpdateCameraInfo(tagIrFileBaseInfo& baseInfo, tagIRCameraMeasurementParameters& parameters)
{
	CWaitCursor waitCursor;

	CIRCameraMeasurementDockingBar* pWnd = (CIRCameraMeasurementDockingBar*)GetParent();
	if(pWnd && pWnd->GetSafeHwnd()!=NULL)
	{
		CAnalysisView* pView = (CAnalysisView*)pWnd->m_pOwner;
		if(pView)
		{
			if(pView->m_wndLoadImage.IsIrImage())
			{
				m_wndTab.ShowWindow(SW_SHOW);

				// 更新红外热像仪参数
				m_pageObjParameters.UpdateCameraMeasurementParameters(parameters);
				m_pageObjParameters.UpdateMeasurementDataList(pView);

				m_pageImageProcess.UpdateCameraInfo(baseInfo);
			}
			else
				m_wndTab.ShowWindow(SW_HIDE);
		}
	}
}
