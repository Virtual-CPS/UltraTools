#pragma once

#include "Resource.h"
#include "GlobalDefsForSys.h"

// CIRCameraMeasurementAnalysisPage 对话框

class CAnalysisView;

class CIRCameraMeasurementAnalysisPage : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIRCameraMeasurementAnalysisPage)

public:
	CIRCameraMeasurementAnalysisPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIRCameraMeasurementAnalysisPage();
	void SetTemperatureCalculationResult(float fTempDifference, float fRelativeTempDifference);

// 对话框数据
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

	CStringArray		m_saTroubleJudgeID;				// 用于简化comboBox
	CStringArray		m_saDisposalOpinion;			// 热故障判据对应的处理建议，初始化判据comboBox时同步初始化
	CStringArray		m_saEquipmentID;				// 用于简化comboBox
	float				m_fTempDifference;				// 温差
	float				m_fRelativeTempDifference;		// 相对温差
	BOOL				m_bInitOK;	// 避免在初始化dialog过程中调用InitEquipmentUnitComboBox()和InitEquipmentComboBox()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangeFeverPart();
	afx_msg void OnSelchangeTroubleType();
	afx_msg void OnSelchangeTroubleJudge();
	void InitTroubleTypeComboBox();
	void InitTroubleJudgeComboBox();

	DECLARE_MESSAGE_MAP()

	CAnalysisView* GetParentView();
};
