#pragma once

#include "Resource.h"
#include "GlobalDefsForSys.h"
#include "IRCameraMeasurementGridCtrl.h"

class CAnalysisView;

// CIRCameraMeasurementObjParametersPage �Ի���

class CIRCameraMeasurementObjParametersPage : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIRCameraMeasurementObjParametersPage)

public:
	CIRCameraMeasurementObjParametersPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIRCameraMeasurementObjParametersPage();

// �Ի�������
	enum { IDD = IDD_IR_CAMERA_MEASUREMENT_OBJ_PARAMETERS_PAGE };
	float			m_fEmissivity;			// ������
	float			m_fReflectedTemp;		// �����¶�
	float			m_fAtmosphericTemp;		// �����¶�
	float			m_fRelativeHumidity;	// ���ʪ��
	float			m_fDistance;			// ����

	float			m_fEstAtmTransm;		// ����������ƣ���Ϊ�㣬FLIR�����ݴ����¶ȡ������ʪ�ȼ���
	float			m_fExtOptTemp;			// �����ѧ�¶�
	float			m_fExtOptTransm;		// �����ѧ����

	CStatic							m_wndListPosition;
	CBCGPEdit						m_edtEmissivity;
	CBCGPMenuButton					m_btnEmissivity;
	CIRCameraMeasurementGridCtrl	m_List;

	float			m_fReferenceTemp;			// �����������¶�
	float			m_fFeverPartTemp;			// ���ȵ��¶�
	float			m_fNormalTemp;				// �������Ӧ���¶�
	float			m_fTempDifference;			// �²�
	float			m_fRelativeTempDifference;	// ����²�

	void UpdateCameraMeasurementParameters(tagIRCameraMeasurementParameters& parameters);
	void UpdateMeasurementDataList(CAnalysisView* pView);

protected:
	CStringArray	m_saEmissivity;
	CMenu			m_menuEmissivity;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	void InitMeasurementDataList();
	void CreateEmissivityMenu();
	afx_msg void OnBtnApply();
	afx_msg void OnBtnEmissivity();

	DECLARE_MESSAGE_MAP()

};
