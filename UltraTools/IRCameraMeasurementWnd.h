#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "GlobalDefsForSys.h"
#include "IRCameraMeasurementObjParametersPage.h"
#include "IRCameraMeasurementImageProcessPage.h"
#include "IRCameraMeasurementAnalysisPage.h"

// CIRCameraMeasurementWnd �Ի���

class CIRCameraMeasurementWnd : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIRCameraMeasurementWnd)

public:
	CIRCameraMeasurementWnd(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIRCameraMeasurementWnd();

// �Ի�������
	enum { IDD = IDD_IR_CAMERA_MEASUREMENT_WND };

	CBCGPTabWnd								m_wndTab;
	CIRCameraMeasurementObjParametersPage	m_pageObjParameters;
	CIRCameraMeasurementImageProcessPage	m_pageImageProcess;
	CIRCameraMeasurementAnalysisPage		m_pageAnalysis;

protected:

	CStatic									m_wndTabPosition;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void InitTab();

	DECLARE_MESSAGE_MAP()


public:
	virtual BOOL OnInitDialog();
	void UpdateCameraInfo(tagIrFileBaseInfo& baseInfo, tagIRCameraMeasurementParameters& parameters);
};
