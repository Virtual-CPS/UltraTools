#pragma once

#include "Resource.h"
#include "GlobalDefsForSys.h"
#include "IRCameraMeasurementGridCtrl.h"

class CAnalysisView;

// CIRCameraMeasurementObjParametersPage 对话框

class CIRCameraMeasurementObjParametersPage : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIRCameraMeasurementObjParametersPage)

public:
	CIRCameraMeasurementObjParametersPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIRCameraMeasurementObjParametersPage();

// 对话框数据
	enum { IDD = IDD_IR_CAMERA_MEASUREMENT_OBJ_PARAMETERS_PAGE };
	float			m_fEmissivity;			// 发射率
	float			m_fReflectedTemp;		// 反射温度
	float			m_fAtmosphericTemp;		// 大气温度
	float			m_fRelativeHumidity;	// 相对湿度
	float			m_fDistance;			// 距离

	float			m_fEstAtmTransm;		// 大气传输估计，若为零，FLIR将根据大气温度、距离和湿度计算
	float			m_fExtOptTemp;			// 表面光学温度
	float			m_fExtOptTransm;		// 表面光学传输

	CStatic							m_wndListPosition;
	CBCGPEdit						m_edtEmissivity;
	CBCGPMenuButton					m_btnEmissivity;
	CIRCameraMeasurementGridCtrl	m_List;

	float			m_fReferenceTemp;			// 环境参照体温度
	float			m_fFeverPartTemp;			// 发热点温度
	float			m_fNormalTemp;				// 正常相对应点温度
	float			m_fTempDifference;			// 温差
	float			m_fRelativeTempDifference;	// 相对温差

	void UpdateCameraMeasurementParameters(tagIRCameraMeasurementParameters& parameters);
	void UpdateMeasurementDataList(CAnalysisView* pView);

protected:
	CStringArray	m_saEmissivity;
	CMenu			m_menuEmissivity;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void InitMeasurementDataList();
	void CreateEmissivityMenu();
	afx_msg void OnBtnApply();
	afx_msg void OnBtnEmissivity();

	DECLARE_MESSAGE_MAP()

};
