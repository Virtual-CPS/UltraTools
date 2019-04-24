// IRCameraMeasurementImageProcessPage.cpp : 实现文件
//

#include "stdafx.h"
#include "UltraTools.h"
#include "IRCameraMeasurementImageProcessPage.h"
#include "IRCameraMeasurementDockingBar.h"
#include "AnalysisView.h"
#include "UI/PaletteColorTable.h"

#define IMAGE_EDIT_NULL			0
#define	IMAGE_EDIT_BRIGHTNESS	1
#define IMAGE_EDIT_CONTRAST		2
#define IMAGE_EDIT_THRESHOLD	3
#define	IMAGE_EDIT_HUE			4
#define IMAGE_EDIT_SATURATION	5
#define IMAGE_EDIT_PALETTE		6


// CIRCameraMeasurementImageProcessPage 对话框

IMPLEMENT_DYNAMIC(CIRCameraMeasurementImageProcessPage, CBCGPDialog)

CIRCameraMeasurementImageProcessPage::CIRCameraMeasurementImageProcessPage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CIRCameraMeasurementImageProcessPage::IDD, pParent)
{
	EnableVisualManagerStyle();

	m_bPaletteReversed = FALSE;
	m_bCheckThreshold = FALSE;
	m_nCurEditType = IMAGE_EDIT_NULL;
	m_bImageModified = FALSE;

	m_crThresholdColor = RGB(0, 255, 0);
	m_fLeft = m_fRight = 0.0f;
	m_fMinTemperature = m_fMaxTemperature = 0.0f;
}

CIRCameraMeasurementImageProcessPage::~CIRCameraMeasurementImageProcessPage()
{
}

void CIRCameraMeasurementImageProcessPage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CAMERA_TYPE, m_ctrlCameraType);
	DDX_Control(pDX, IDC_EDIT_CAMERA_SN, m_ctrlCameraSN);
	DDX_Control(pDX, IDC_EDIT_CAMERA_LENS, m_ctrlCameraLens);
	DDX_Control(pDX, IDC_EDIT_FILE_NAME, m_ctrlFileName);
	DDX_Control(pDX, IDC_EDIT_FILE_DATETIME, m_ctrlFileDatetime);

	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_sliderBrightness);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_sliderContrast);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD, m_sliderThreshold);
	DDX_Check(pDX, IDC_CHECK_REVERSED, m_bPaletteReversed);
	DDX_Check(pDX, IDC_CHECK_THRESHOLD, m_bCheckThreshold);
	DDX_Control(pDX, IDC_SLIDER_SATURATION, m_sliderSaturation);
	DDX_Control(pDX, IDC_SLIDER_HUE, m_sliderHue);
	DDX_CBIndex(pDX, IDC_COMBO_PALETTE, m_nCurPalette);
	DDX_Control(pDX, IDC_COMBO_PALETTE, m_comPalette);
	DDX_Control(pDX, IDC_BTN_ABANDON_MODIFIED, m_btnAbandonModified);

	DDX_Text(pDX, IDC_EDIT_LEFT, m_fLeft);
	DDX_Text(pDX, IDC_EDIT_RIGHT, m_fRight);
}

BEGIN_MESSAGE_MAP(CIRCameraMeasurementImageProcessPage, CBCGPDialog)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_PALETTE, OnSelchangePalette)
	ON_BN_CLICKED(IDC_CHECK_REVERSED, OnCheckPaletteReversed)
	ON_BN_CLICKED(IDC_CHECK_THRESHOLD, OnCheckThreshold)
	ON_BN_CLICKED(IDC_BTN_ABANDON_MODIFIED, OnBtnAbandonModified)
	ON_BN_CLICKED(IDC_BTN_SAVE_TEMPORARILY, OnBtnSaveTemporarily)
	ON_REGISTERED_MESSAGE(WM_RANGE_CHANGED, OnRangeChange)
END_MESSAGE_MAP()

// CIRCameraMeasurementImageProcessPage 消息处理程序

BOOL CIRCameraMeasurementImageProcessPage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	m_sliderBrightness.SetRange(-100,100);
	m_sliderBrightness.SetTicFreq(10);
	m_sliderBrightness.m_bDrawFocus = FALSE;

	m_sliderContrast.SetRange(-100,100);
	m_sliderContrast.SetTicFreq(10);
	m_sliderContrast.m_bDrawFocus = FALSE;

// 	m_sliderThreshold.SetRange(1, 255);
// 	m_sliderThreshold.SetTicFreq(10);
// 	m_sliderThreshold.m_bDrawFocus = FALSE;
	m_sliderThreshold.EnableWindow(m_bCheckThreshold);

	m_sliderHue.m_bIsHueSlider = TRUE;
	m_sliderHue.SetRange(0,360);
	m_sliderHue.SetTicFreq(10);
	m_sliderHue.m_bDrawFocus = FALSE;

	m_sliderSaturation.m_bIsSaturationSlider = TRUE;
	m_sliderSaturation.SetRange(0, 200);
	m_sliderSaturation.SetTicFreq(10);
	m_sliderSaturation.m_bDrawFocus = FALSE;

	Reset(m_fMinTemperature, m_fMaxTemperature);

	// 初始化调色板
	for (int i=0; i<PALLETTE_COLOR_SCALE_COUNT; i++)
	{
		m_comPalette.AddString(PaletteColorScaleName[i]);
	}
	m_comPalette.SetCurSel(2);

	m_btnAbandonModified.EnableWindow(m_bImageModified);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIRCameraMeasurementImageProcessPage::OnSelchangePalette()
{
	int nCur = m_comPalette.GetCurSel();
	if(nCur == m_nCurPalette)
		return;

	m_nCurPalette = nCur;

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;

	if (!m_imgSource.IsValid())
	{
		m_imgSource.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_bImageModified = FALSE;
		m_nPaletteIndexBeforeModified = m_comPalette.GetCurSel();
	}
	if(!m_imgSource.IsValid() || !m_imgModified.IsValid())
		return;

	pView->m_wndLoadImage.SetPalette(m_nCurPalette, m_bPaletteReversed);

	m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
	m_nCurEditType = IMAGE_EDIT_PALETTE;
	m_bImageModified = TRUE;
	pView->SetModified(TRUE);
	m_btnAbandonModified.EnableWindow(TRUE);
}

/*********************************************************
Function:	OnHScroll
Desc:		drag mouse response function, setting image effect
Input:	nSBCode, code of scroll;
nPos, the position of scroll;
pScrollBar, point to the scroll bar;
Output:	none
Return:	none	
**********************************************************/
void CIRCameraMeasurementImageProcessPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(!UpdateData())
		return;

	if (pScrollBar == NULL)
		return CBCGPDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;

	if (!m_imgSource.IsValid())
	{
		m_imgSource.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_bImageModified = FALSE;
		m_nPaletteIndexBeforeModified = m_comPalette.GetCurSel();
	}
	if(!m_imgSource.IsValid() || !m_imgModified.IsValid())
		return;

	m_bImageModified = TRUE;
	m_btnAbandonModified.EnableWindow(m_bImageModified);

	CWnd* pWnd;
	CThermalImage tmpImage;
	int nValue;
	bool status = true;

	pWnd = &m_sliderBrightness;
	if(pScrollBar == pWnd)
	{
		if(IMAGE_EDIT_BRIGHTNESS == m_nCurEditType)
			tmpImage.Copy(m_imgModified);
		else
		{
			m_nCurEditType = IMAGE_EDIT_BRIGHTNESS;
			m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
			tmpImage.Copy(m_imgModified);
		}

		nValue = m_sliderBrightness.GetPos();
		status = tmpImage.Light(nValue);
	}

	pWnd = &m_sliderContrast;
	if(pScrollBar == pWnd)
	{
		if(IMAGE_EDIT_CONTRAST == m_nCurEditType)
			tmpImage.Copy(m_imgModified);
		else
		{
			m_nCurEditType = IMAGE_EDIT_CONTRAST;
			m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
			tmpImage.Copy(m_imgModified);
		}

		nValue = m_sliderContrast.GetPos();
		status = tmpImage.Light(0, nValue);
	}

	pWnd = &m_sliderHue;
	if(pScrollBar == pWnd)
	{
		if(IMAGE_EDIT_HUE == m_nCurEditType)
			tmpImage.Copy(m_imgModified);
		else
		{
			m_nCurEditType = IMAGE_EDIT_HUE;
			m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
			tmpImage.Copy(m_imgModified);
		}

		nValue = m_sliderHue.GetPos();
		status = tmpImage.Colorize((BYTE)nValue, (BYTE)m_sliderSaturation.GetPos());
	}

	pWnd = &m_sliderSaturation;
	if(pScrollBar == pWnd)
	{
		if(IMAGE_EDIT_HUE == m_nCurEditType)
			tmpImage.Copy(m_imgModified);
		else
		{
			m_nCurEditType = IMAGE_EDIT_HUE;
			m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
			tmpImage.Copy(m_imgModified);
		}

		nValue = m_sliderSaturation.GetPos();
		status = tmpImage.Saturate(nValue, 2);		// 色彩空间1，或2
	}

	int nHue = m_sliderHue.GetPos();
	int nSaturation = m_sliderSaturation.GetPos();
	m_sliderSaturation.SetImagesHue((double)(nHue - 180));
	m_sliderSaturation.SetImagesSaturation((double)(sqrt (nSaturation / 100.0) - 1.0));
	m_sliderSaturation.Invalidate(TRUE);

	if (!status)
	{
		CString s(tmpImage.GetLastError());
		AfxMessageBox(s);
		return;
	}
	if(tmpImage.IsValid())
		pView->m_wndLoadImage.m_pIRImage->Transfer(tmpImage);

	pView->m_wndLoadImage.m_wndImage.Invalidate(TRUE);
	pView->SetModified(TRUE);
	CBCGPDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CIRCameraMeasurementImageProcessPage::OnBtnAbandonModified()
{
	if(!m_imgSource.IsValid())
		return;

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;

	pView->m_wndLoadImage.m_pIRImage->Transfer(m_imgSource);
	pView->m_wndLoadImage.m_wndImage.Invalidate(TRUE);

	Reset(m_fMinTemperature, m_fMaxTemperature);
}

void CIRCameraMeasurementImageProcessPage::OnBtnSaveTemporarily()
{
	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;

	m_imgSource.Copy(*pView->m_wndLoadImage.m_pIRImage);
	m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
	m_nPaletteIndexBeforeModified = m_comPalette.GetCurSel();

	Reset(m_fMinTemperature, m_fMaxTemperature);
}

void CIRCameraMeasurementImageProcessPage::OnCheckPaletteReversed()
{
	if(!UpdateData())
		return;

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;

	if (!m_imgSource.IsValid())
	{
		m_imgSource.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_bImageModified = FALSE;
		m_nPaletteIndexBeforeModified = m_comPalette.GetCurSel();
	}
	if(!m_imgSource.IsValid() || !m_imgModified.IsValid())
		return;

	pView->m_wndLoadImage.SetPalette(m_nCurPalette, m_bPaletteReversed);

	m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
	m_nCurEditType = IMAGE_EDIT_PALETTE;
	m_bImageModified = TRUE;
	pView->SetModified(TRUE);
	m_btnAbandonModified.EnableWindow(TRUE);
}

void CIRCameraMeasurementImageProcessPage::OnCheckThreshold()
{
	MarkTempRangeInImage();
}

CAnalysisView* CIRCameraMeasurementImageProcessPage::GetParentView()
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

void CIRCameraMeasurementImageProcessPage::Reset(float fMinTemp, float fMaxTemp, int nDefaultPalette, BOOL bChangeDefaultPalette)
{
	m_imgSource.Destroy();
	m_imgModified.Destroy();
	m_bImageModified = FALSE;
	m_nCurEditType = IMAGE_EDIT_NULL;
	m_btnAbandonModified.EnableWindow(FALSE);

	if(bChangeDefaultPalette)
	{
		m_comPalette.SetCurSel(nDefaultPalette);
		m_nCurPalette = nDefaultPalette;
	}
	else
	{
		m_comPalette.SetCurSel(m_nPaletteIndexBeforeModified);
		m_nCurPalette = m_nPaletteIndexBeforeModified;
	}
	m_nPaletteIndexBeforeModified = -1;

	m_sliderBrightness.SetPos(0);
	m_sliderContrast.SetPos(0);
	m_sliderHue.SetPos((int)(m_sliderHue.GetImagesHue() + 180.0));

	double value = m_sliderSaturation.GetImagesSaturation() + 1.0;
	value = value * value * 100.0;
	m_sliderSaturation.SetPos(100);

	m_bPaletteReversed = FALSE;
	m_bCheckThreshold = FALSE;

	InitRange(fMinTemp, fMaxTemp);

	UpdateData(FALSE);
	OnCheckThreshold();
}

void CIRCameraMeasurementImageProcessPage::InitRange(float fMinTemp, float fMaxTemp)
{
	if(fMinTemp > fMaxTemp)
	{
		m_fMinTemperature = m_fMaxTemperature = 0;
	}
	else
	{
		m_fMinTemperature = fMinTemp;
		m_fMaxTemperature = fMaxTemp;
	}
	m_sliderThreshold.SetMinMax(m_fMinTemperature, m_fMaxTemperature);

	float delta = m_fMaxTemperature - m_fMinTemperature;
	m_fRight = m_fMaxTemperature;
	m_fLeft = m_fRight - delta * 0.2f;
	m_sliderThreshold.SetRange(m_fLeft, m_fRight);

	m_sliderThreshold.SetVisualMinMax(m_fLeft, m_fRight);
	m_sliderThreshold.GetVisualMinMax(m_fLeft, m_fRight);
	m_fLeft = ((int)(m_fLeft*10+0.5))/10.0f;
	m_fRight = ((int)(m_fRight*10+0.5))/10.0f;
	UpdateData(FALSE);

	m_crThresholdColor = m_sliderThreshold.GetVisualColor();
}

void CIRCameraMeasurementImageProcessPage::UpdateCameraInfo(tagIrFileBaseInfo& baseInfo)
{
	CWaitCursor waitCursor;

	m_ctrlCameraType.SetWindowText(baseInfo.sCameraType);
	m_ctrlCameraSN.SetWindowText(baseInfo.sCameraSN);
	m_ctrlCameraLens.SetWindowText(baseInfo.sLens);
	m_ctrlFileName.SetWindowText(baseInfo.sFileName);
	m_ctrlFileDatetime.SetWindowText(baseInfo.sFileDatetime);
}

LRESULT CIRCameraMeasurementImageProcessPage::OnRangeChange(WPARAM wParam, LPARAM lParam)
{
	UpdateData();
	m_sliderThreshold.GetRange(m_fLeft, m_fRight);
	m_sliderThreshold.SetVisualMinMax(m_fLeft, m_fRight);
	m_sliderThreshold.GetVisualMinMax(m_fLeft, m_fRight);
	m_fLeft = ((int)(m_fLeft*10+0.5))/10.0f;
	m_fRight = ((int)(m_fRight*10+0.5))/10.0f;
	UpdateData(FALSE);

	BOOL bIsChanging = (BOOL)lParam;
	if(!bIsChanging)
	{
		m_crThresholdColor = m_sliderThreshold.GetVisualColor();
		MarkTempRangeInImage();
	}

	return static_cast<LRESULT>(0);
}

// 在图谱中用指定的颜色标注温度范围内的像素
void CIRCameraMeasurementImageProcessPage::MarkTempRangeInImage()
{
	CWaitCursor wCursor;

	if(!UpdateData())
		return;
	m_sliderThreshold.EnableWindow(m_bCheckThreshold);

	CAnalysisView* pView = GetParentView();
	if(pView==NULL || pView->m_wndLoadImage.m_pIRImage==NULL)
		return;

	if (!m_imgSource.IsValid())
	{
		m_imgSource.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_bImageModified = FALSE;
		m_nPaletteIndexBeforeModified = m_comPalette.GetCurSel();
	}
	if(!m_imgSource.IsValid() || !m_imgModified.IsValid())
		return;

	if(m_bCheckThreshold && m_fLeft<m_fRight)
	{
		m_nCurEditType = IMAGE_EDIT_THRESHOLD;

		if(m_imgModified.IsValid())
			pView->m_wndLoadImage.m_pIRImage->Copy(m_imgModified);

		DWORD nWidth = pView->m_wndLoadImage.m_pIRImage->GetWidth();
		DWORD nHeight = pView->m_wndLoadImage.m_pIRImage->GetHeight();

		if (pView->m_wndLoadImage.m_pIRImage->GetNumColors() >0)
		{
			CThermalImage output;
			output.Create(nWidth, nHeight, 24, THERMAL_IMAGE_SUPPORT_BMP);  //1 4 8 24
			for(DWORD y=0; y<nHeight; y++)
			{
				for(DWORD x=0; x<nWidth; x++)
				{
					RGBQUAD rgbQUAD;
					rgbQUAD = pView->m_wndLoadImage.m_pIRImage->GetPixelColor(x, y);
					output.SetPixelColor(x,y,rgbQUAD); 
				}
			}
			pView->m_wndLoadImage.m_pIRImage->Copy(output);
		}
		 
		CPoint point;
		float fTemp;
		long x, y, yFlip;
		for(DWORD j=0; j<nHeight; j++)
		{
			for (DWORD i=0; i<nWidth; i++)
			{
				point.x = x = i;
				point.y = y = j;
				if(pView->m_wndLoadImage.GetSpotTemperature(point, fTemp))
				{
					if(fTemp>=m_fLeft && fTemp<=m_fRight)
					{
						pView->GetImageXY(&x, &y);
						yFlip = nHeight - y - 1;
						pView->m_wndLoadImage.m_pIRImage->SetPixelColor(x, yFlip, m_crThresholdColor);
					}
				}
			}
		}

		//m_imgModified.Copy(*pView->m_wndLoadImage.m_pIRImage);
		m_nCurEditType = IMAGE_EDIT_THRESHOLD;
		m_bImageModified = TRUE;
		pView->SetModified(TRUE);
		m_btnAbandonModified.EnableWindow(TRUE);
	}
	else if(m_imgModified.IsValid())
		pView->m_wndLoadImage.m_pIRImage->Copy(m_imgModified);

	pView->m_wndLoadImage.m_wndImage.Invalidate(TRUE);
}
