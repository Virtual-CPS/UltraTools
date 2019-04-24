 // IRCameraMeasureWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UltraTools.h"
#include "AnalysisView.h"
#include "IRCameraMeasurementAnalysisPage.h"

#include "Utility/HvUtility.h"


// CIRCameraMeasurementAnalysisPage �Ի���

IMPLEMENT_DYNAMIC(CIRCameraMeasurementAnalysisPage, CBCGPDialog)

CIRCameraMeasurementAnalysisPage::CIRCameraMeasurementAnalysisPage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CIRCameraMeasurementAnalysisPage::IDD, pParent)
{
	EnableVisualManagerStyle ();

	m_sTsName = _T("");
	m_sEquipmentUnitName = _T("");
	m_sEquipmentName = _T("");
	m_sFeverPartName = _T("");
	m_sTroubleTypeName = _T("-1");				// Ĭ�ϡ��޹��ϡ�
	m_sTroubleJudgeName = _T("");
	m_sDisposalOpinion = m_sRemark = _T("");
	m_sTaker = m_sRecordor = m_sAuditor = m_sApproval = _T("");
//	m_sAnalyst = theApp.m_tsLoginName;		// Ĭ�Ϸ�����
	m_sVoltageGradeID = _T("");
	m_fRatedCurrent = m_fLoadCurrent = 0.f;
	m_sWeather = _T("����");					// Ĭ������
	m_nWindSpeed = 0;

	m_nCurTroubleJudge = -1;
	m_nCurTroubleLevel = -1;

	m_fTempDifference = 0.f;
	m_fRelativeTempDifference = 0.f;

	m_bInitOK = FALSE;
}

CIRCameraMeasurementAnalysisPage::~CIRCameraMeasurementAnalysisPage()
{
}

void CIRCameraMeasurementAnalysisPage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TS, m_sTsName);
	DDX_Text(pDX, IDC_EDIT_EQUIPMENT_UNIT, m_sEquipmentUnitName);
	DDX_Text(pDX, IDC_EDIT_EQUIPMENT, m_sEquipmentName);
	DDX_Control(pDX, IDC_COMBO_TROUBLE_TYPE, m_comTroubleType);
	DDX_Control(pDX, IDC_COMBO_TROUBLE_JUDGE, m_comTroubleJudge);
	DDX_CBIndex(pDX, IDC_COMBO_TROUBLE_JUDGE, m_nCurTroubleJudge);
	DDX_CBIndex(pDX, IDC_COMBO_TROUBLE_LEVEL, m_nCurTroubleLevel);
	DDX_Text(pDX, IDC_EDIT_DISPOSAL_OPINION, m_sDisposalOpinion);
	DDV_MaxChars(pDX, m_sDisposalOpinion, 200);
	DDX_Text(pDX, IDC_EDIT_REMARK, m_sRemark);
	DDV_MaxChars(pDX, m_sRemark, 50);
	DDX_Text(pDX, IDC_EDIT_TAKER, m_sTaker);
	DDV_MaxChars(pDX, m_sTaker, 30);
	DDX_Text(pDX, IDC_EDIT_RECORDOR, m_sRecordor);
	DDV_MaxChars(pDX, m_sRecordor, 30);
	DDX_Text(pDX, IDC_EDIT_ANALYST, m_sAnalyst);
	DDV_MaxChars(pDX, m_sAnalyst, 30);
	DDX_Text(pDX, IDC_EDIT_AUDITOR, m_sAuditor);
	DDV_MaxChars(pDX, m_sAuditor, 30);
	DDX_Text(pDX, IDC_EDIT_APPROVAL, m_sApproval);
	DDV_MaxChars(pDX, m_sApproval, 30);
	DDX_Text(pDX, IDC_EDIT_VOLTAGE_GRADE_ID, m_sVoltageGradeID);
	DDX_Text(pDX, IDC_EDIT_RATED_CURRENT, m_fRatedCurrent);
	DDV_MinMaxFloat(pDX, m_fRatedCurrent, 0.f, 99999.f);
	DDX_Text(pDX,IDC_EDIT_LOAD_CURRENT, m_fLoadCurrent);
	DDV_MinMaxFloat(pDX, m_fLoadCurrent, 0.f, 99999.f);
	DDX_Text(pDX, IDC_EDIT_WEATHER, m_sWeather);
	DDV_MaxChars(pDX, m_sWeather, 20);
	DDX_Text(pDX, IDC_EDIT_WIND_SPEED, m_nWindSpeed);
	DDV_MinMaxInt(pDX, m_nWindSpeed, 0, 255);
}


BEGIN_MESSAGE_MAP(CIRCameraMeasurementAnalysisPage, CBCGPDialog)
	ON_CBN_SELENDOK(IDC_COMBO_EQUIPMENT_FEVER_PART, OnSelchangeFeverPart)
	ON_CBN_SELCHANGE(IDC_COMBO_TROUBLE_TYPE, OnSelchangeTroubleType)
	ON_CBN_SELCHANGE(IDC_COMBO_TROUBLE_JUDGE, OnSelchangeTroubleJudge)
END_MESSAGE_MAP()


// CIRCameraMeasurementAnalysisPage ��Ϣ�������

BOOL CIRCameraMeasurementAnalysisPage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
/*
	CStringArray saID, saName, saFatherID;
	CString sSQL, sTem;
	int nCnt, nCurSel=-1, i;
	HTREEITEM hNode;
	_RecordsetPtr* prsRecSet;
	CBFRstValues rsvRecValue;

	//---------------------------------------------------------------
	// ��ʼ�����Ȳ�λ��״combo
	m_comEquipmentFeverPart.SetHasCheckboxes(FALSE);				// Must set before tree is created
	m_comEquipmentFeverPart.SetTreeType(CComboTree::EQUIPMENT_FEVER_PART_TREE);
	m_comEquipmentFeverPart.SubclassDlgItem(IDC_COMBO_EQUIPMENT_FEVER_PART, this);

	hNode = m_comEquipmentFeverPart.InitTree(CComboTree::EQUIPMENT_FEVER_PART_TREE, "", m_sFeverPartID, TRUE);
	if (hNode != NULL)
	{
		CComboEquipmentFeverPartTreeDropList* pTree = m_comEquipmentFeverPart.GetEquipmentFeverPartTree();
		pTree->SelectItem(hNode);

		CString sTem;
		sTem = pTree->GetItemText(hNode);
		if(sTem.GetLength() >0)
		{
			m_comEquipmentFeverPart.SetWindowText(sTem);
		}
	}

	//---------------------------------------------------------------
	// ��ʼ����������combo
	InitTroubleTypeComboBox();

	//---------------------------------------------------------------
	// ��ʼ�������о�combo
	InitTroubleJudgeComboBox();
*/
	m_bInitOK = TRUE;		// ���Գ�ʼ��EquipmentUnitComboBox��EquipmentComboBox

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

CAnalysisView* CIRCameraMeasurementAnalysisPage::GetParentView()
{
	CAnalysisView* pView = NULL;

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
				pView = (CAnalysisView *)(pBar->m_pOwner);
			}
		}
	}

	return pView;
}

// �����豸���Ȳ�λ���࣬��ʼ����������comboBox
void CIRCameraMeasurementAnalysisPage::InitTroubleTypeComboBox()
{
	int nLen, nCnt, i;
	CString sTem;
	CStringArray saID, saName;

	m_comTroubleType.ResetContent();
	m_comTroubleType.AddString(_T("<�޹���>"));
/*
	nLen = m_sFeverPartID.GetLength();
	if(nLen <= 0)
		return;

	// ��ȡ����ͨ�����ID����β����zero�滻��ͨ�����
	GetIdFilter(m_sFeverPartID, '_', -1, sTem);
	theApp.m_SysDatabase.GetEquipmentFeverPartTroubleType(sTem, saID, saName, TRUE);
	nCnt = (int)saID.GetCount();
	for(i=0; i<nCnt; i++)
	{
		sTem.Format("[%s]%s", saID.GetAt(i), saName.GetAt(i));
		m_comTroubleType.AddString(sTem);
	}
	SetComboBoxWidthEnoughToShowAllItems(&m_comTroubleType);
*/}

// ���ݹ������ͣ���ʼ�������о�comboBox
void CIRCameraMeasurementAnalysisPage::InitTroubleJudgeComboBox()
{
/*	int nCnt, i;
	CStringArray saName;

	m_comTroubleJudge.ResetContent();
	m_comTroubleJudge.AddString("<ģ���ж�>");
	m_nCurTroubleJudge = -1;

	if(m_sTroubleTypeID.GetLength() <= 0)
		return;

	theApp.m_SysDatabase.GetEquipmentFeverPartTroubleJudge(m_sTroubleTypeID, m_saTroubleJudgeID, saName, m_saDisposalOpinion);
	nCnt = (int)m_saTroubleJudgeID.GetCount();
	for(i=0; i<nCnt; i++)
	{
		m_comTroubleJudge.AddString(saName.GetAt(i));

		if(m_sTroubleTypeID == m_saTroubleJudgeID.GetAt(i))
			m_nCurTroubleJudge = i+1;
	}
	SetComboBoxWidthEnoughToShowAllItems(&m_comTroubleJudge);
	m_comTroubleJudge.SetCurSel(m_nCurTroubleJudge);
*/}

void CIRCameraMeasurementAnalysisPage::OnSelchangeFeverPart()
{
/*	CComboEquipmentFeverPartTreeDropList* pTree = m_comEquipmentFeverPart.GetEquipmentFeverPartTree();
	m_sFeverPartID = pTree->GetCurEquipmentFeverPartID();

	InitTroubleTypeComboBox();
	InitTroubleJudgeComboBox();

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;
	pView->SetModified(TRUE);
*/}

void CIRCameraMeasurementAnalysisPage::OnSelchangeTroubleType()
{
	if(!UpdateData())
		return;
/*
	int nPos, nPos2, nCurSel;
	CString sTem;

	nCurSel = m_comTroubleType.GetCurSel();
	if (nCurSel == 0)			// �޹���
	{
		m_sTroubleTypeID = "-1";
	}
	else
	{
		m_comTroubleType.GetLBText(nCurSel, sTem);
		nPos = sTem.Find('[');
		nPos2 = sTem.Find(']');
		if(nPos<0 || nPos2<=0 || nPos2<=nPos)
			m_sTroubleTypeID = "";
		else
			m_sTroubleTypeID = sTem.Mid(nPos+1, nPos2-nPos-1);
	}

	InitTroubleJudgeComboBox();

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;
	pView->SetModified(TRUE);
*/}

void CIRCameraMeasurementAnalysisPage::OnSelchangeTroubleJudge()
{
/*	m_sTroubleJudgeID = "";

	if(!UpdateData())
		return;
	if(m_nCurTroubleJudge < 0)
		return;

	if (m_nCurTroubleJudge == 0)			// ģ���о�
	{
		m_sDisposalOpinion = "��飬����";
	}
	else
	{
		m_sDisposalOpinion = m_saDisposalOpinion.GetAt(m_nCurTroubleJudge-1);
		m_sTroubleJudgeID = m_saTroubleJudgeID.GetAt(m_nCurTroubleJudge-1);
	}
	UpdateData(FALSE);

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;
	pView->SetModified(TRUE);
*/}

void CIRCameraMeasurementAnalysisPage::SetTemperatureCalculationResult(float fTempDifference, float fRelativeTempDifference)
{
	m_fTempDifference = fTempDifference;
	m_fRelativeTempDifference = fRelativeTempDifference;

	if(m_fRelativeTempDifference >= 95.f)
		m_nCurTroubleLevel = 3;						// Σ��ȱ��
	else if(m_fRelativeTempDifference >= 80.f)
		m_nCurTroubleLevel = 2;						// ����ȱ��
	else if(m_fTempDifference < 10)
		m_nCurTroubleLevel = 0;						// �޹���
	else
		m_nCurTroubleLevel = 1;						// һ��ȱ��

	if(m_comTroubleJudge.GetCount() > 0)
		m_nCurTroubleJudge = 0;						// ģ�����

	UpdateData(FALSE);
	OnSelchangeTroubleJudge();
}
