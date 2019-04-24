#pragma once

#include "Resource.h"
#include "GlobalDefsForSys.h"
// #include "IRCameraMeasurementGridCtrl.h"
#include "UI/ColorSliderCtrl.h"
#include "UI/RangeSlider.h"
#include "ThermalImage.h"


// CIRCameraMeasurementImageProcessPage 对话框

class CAnalysisView;

class CIRCameraMeasurementImageProcessPage : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIRCameraMeasurementImageProcessPage)

public:
	CIRCameraMeasurementImageProcessPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIRCameraMeasurementImageProcessPage();

	void Reset(float fMinTemp, float fMaxTemp, int nDefaultPalette=-1, BOOL bChangeDefaultPalette=FALSE);
	void InitRange(float fMinTemp, float fMaxTemp);
	void UpdateCameraInfo(tagIrFileBaseInfo& baseInfo);

// 对话框数据
	enum { IDD = IDD_IR_CAMERA_MEASUREMENT_IMAGE_PROCESS_PAGE };
	CBCGPEdit			m_ctrlCameraType;
	CBCGPEdit			m_ctrlCameraSN;
	CBCGPEdit			m_ctrlCameraLens;
	CBCGPEdit			m_ctrlFileName;
	CBCGPEdit			m_ctrlFileDatetime;
	float				m_fLeft, m_fRight;				// 温区

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangePalette();
	afx_msg void OnCheckPaletteReversed();
	afx_msg void OnCheckThreshold();
	afx_msg void OnThresholdColorPicker();
	afx_msg void OnBtnAbandonModified();
	afx_msg void OnBtnSaveTemporarily();

	DECLARE_MESSAGE_MAP()

	CAnalysisView* GetParentView();
	LRESULT OnRangeChange(WPARAM wParam, LPARAM lParam);
	void MarkTempRangeInImage();

protected:
	int							m_nCurPalette;
	BOOL						m_bPaletteReversed;
	BOOL						m_bCheckThreshold;
	CPalette					m_palSys;
	CColorSliderCtrl			m_sliderBrightness;
	CColorSliderCtrl			m_sliderContrast;
	CColorSliderCtrl			m_sliderHue;
	CColorSliderCtrl			m_sliderSaturation;
	CBCGPComboBox				m_comPalette;
	CRangeSlider				m_sliderThreshold;
	CBCGPButton					m_btnAbandonModified;

	COLORREF					m_crThresholdColor;						// 温区颜色
	float						m_fMinTemperature, m_fMaxTemperature;	// 幅面内最高最低温度(range)

	CThermalImage				m_imgSource;
	CThermalImage				m_imgModified;
	int							m_nCurEditType;
	BOOL						m_bImageModified;
	int							m_nPaletteIndexBeforeModified;
};
