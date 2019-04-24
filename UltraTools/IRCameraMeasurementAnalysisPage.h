#pragma once

#include "Resource.h"
#include "GlobalDefsForSys.h"

// CIRCameraMeasurementAnalysisPage �Ի���

class CAnalysisView;

class CIRCameraMeasurementAnalysisPage : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIRCameraMeasurementAnalysisPage)

public:
	CIRCameraMeasurementAnalysisPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIRCameraMeasurementAnalysisPage();
	void SetTemperatureCalculationResult(float fTempDifference, float fRelativeTempDifference);

// �Ի�������
	enum { IDD = IDD_IR_CAMERA_MEASUREMENT_ANALYSIS_PAGE };

	CBCGPComboBox		m_comEquipmentFeverPart;
	CBCGPComboBox		m_comTroubleType;
	CBCGPComboBox		m_comTroubleJudge;

	int					m_nCurTroubleJudge;
	int					m_nCurTroubleLevel;

	CString				m_sTsName;
	CString				m_sEquipmentUnitName;
	CString				m_sEquipmentName;
	CString				m_sFeverPartName;
	CString				m_sTroubleTypeName;
	CString				m_sTroubleJudgeName;
	CString				m_sDisposalOpinion, m_sRemark;
	CString				m_sVoltageGradeID;
	float				m_fRatedCurrent, m_fLoadCurrent;
	CString				m_sWeather;
	int					m_nWindSpeed;
	CString				m_sTaker, m_sRecordor, m_sAnalyst, m_sAuditor, m_sApproval;


protected:

	CStringArray		m_saTroubleJudgeID;				// ���ڼ�comboBox
	CStringArray		m_saDisposalOpinion;			// �ȹ����оݶ�Ӧ�Ĵ����飬��ʼ���о�comboBoxʱͬ����ʼ��
	CStringArray		m_saEquipmentID;				// ���ڼ�comboBox
	float				m_fTempDifference;				// �²�
	float				m_fRelativeTempDifference;		// ����²�
	BOOL				m_bInitOK;	// �����ڳ�ʼ��dialog�����е���InitEquipmentUnitComboBox()��InitEquipmentComboBox()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangeFeverPart();
	afx_msg void OnSelchangeTroubleType();
	afx_msg void OnSelchangeTroubleJudge();
	void InitTroubleTypeComboBox();
	void InitTroubleJudgeComboBox();

	DECLARE_MESSAGE_MAP()

	CAnalysisView* GetParentView();
};
