// LoadImageForAnalysis.h : interface of the CLoadImageForAnalysisWnd class
//

#pragma once

#include "UltraTools.h"
#include "UI/PaletteStatic.h"
#include "DirectShow/VideoDirectShowWnd.h"
#include "SDK/FlirCamCtrl.h"
#include "ThermalImageJPG.h"
#include "IRCameraMeasurementDockingBar.h"
#include "GlobalDefsForSys.h"

class CUltraToolsDoc;


class CLoadImageForAnalysisWnd : public CBCGPDialog
{
	DECLARE_DYNAMIC(CLoadImageForAnalysisWnd)

	// Construction
public:
	CLoadImageForAnalysisWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadImageForAnalysisWnd();

	// Dialog Data
public:
	//{{AFX_DATA(CLoadImageForAnalysisWnd)
	enum { IDD = IDD_LOAD_IMAGE_FOR_ANALYSIS_WND };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CLoadImageForAnalysisWnd)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:
	BOOL				m_bIsActive;
	CThermalImage*		m_pIRImage;				// 红外图谱（main image）
	CThermalImage*		m_pVLImage;				// 可见光图谱（assistant image）
	CVideoDirectShowWnd	m_wndImage;				// 显示红外图谱的画布
	float				m_fMaxTemperature;		// 整幅图谱的最高/最低温度
	float				m_fMinTemperature;
	CString				m_sCurrentIRFile;		// 当前文件（含路径），为了便于存储分析结果，该变量与doc中不同步刷新
	CString				m_sCurrentVLFile;

	BOOL				m_bModified;			// 是否已进行了分析诊断(只记录红外图谱的处理状态)

	CFlirCameraCtrl		m_ctrlFlirCamera;
	CPaletteStatic		m_ctrlPalette;
	int					m_nCurPaletteIndex;
	BOOL				m_bShowPalette;
	int					m_nPaleteWidth;
	float				m_fZoomFactor;			// 主图谱(红外图谱)缩放比例
	tagIrFileBaseInfo	m_IrFileBaseInfo;
	ImageFileType		m_nCurMainImageType;	// 当前加载的main image类型
	tagIRCameraMeasurementParameters	m_IrMeasurementParameters;

	ImageFileType GetCurImageType(){ return m_nCurMainImageType; }
	BOOL IsIrImage(){ return m_bCameraTypeIsIR; }

// Operations
public:
	BOOL GetImageXY(long *x, long *y);
	BOOL GetSpotTemperature(CPoint ptSpot, float& fTemp);
	BOOL GetLineTemperature(CPoint ptFrom, CPoint ptTo, float& fMaxT, float& fMinT, float& fAvgT);
	BOOL GetAreaTemperature(CRect rectBox, float& fMaxT, float& fMinT, float& fAvgT, BOOL bRawImage=FALSE);
	BOOL RotateCameraTemperatureImage(BOOL bClockwise=TRUE);
	BOOL SetMeasurementParameters(tagIRCameraMeasurementParameters& parameters);

	BOOL LoadTargetImage(CString sImageFilePathAndName, BOOL bForcedAsVLImage=FALSE);	// 判断文档类型并加载图谱

	void RepositionCtrls();								// 重新排列界面中的控件
	void Zoomin();
	void Zoomout();
	void SetPalette(int iPaletteIndex=2, BOOL bReversed=FALSE);
	void SetModified(BOOL bModified=TRUE)
	{
		m_bModified = bModified;
	}

	int ConfirmSaveAnalysisResult();					// 确认是否放弃分析诊断结果、是否保存

// Implementation

protected:

	HWND				m_hOwner;
	CWnd*				m_pOwner;
	BOOL				m_bWinInitialized;
	BOOL				m_bCameraTypeIsIR;		// 根据main image判断，主要是为与TSMO兼容，无实质性用途

	HANDLE				m_hFlirTemperatureImage;		// 用于FLIR图谱内存转换及求光标跟随点温
	HANDLE				m_hFlirLUT;						// 用于计算FLIR光标跟随点温
	int					m_nFlirTemperatureImageSize;	// 用于旋转FLIR温度图谱(与FLIR图谱类型/pixel有关，不是单纯文件大小)

	//{{AFX_MSG(CLoadImageForAnalysisWnd )
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:

	//----------
	// 图谱加载
public:
	void ClearIrImageInfo(BOOL bForDestroy=FALSE);				// 析构函数中调用时必须为TRUE
protected:
	BOOL LoadIrImage(CString sImageFilePathAndName, ImageFileType eType);				// 加载红外图谱
	// 加载FLIR红外图谱
	BOOL InitFlirCameraCtrl();															// 初始化FLIR控件
	BOOL LoadFlirImageFile(CString sImageFilePathAndName, ImageFileType eType);
	BOOL ConvertFlirImage(int nImageType, COleSafeArray& sa);
	BOOL ConvertFlirImage(int nImageType, HGLOBAL hMem);
	BOOL GetFlirLUT();
	BOOL GetFlirCameraBaseInfo(ImageFileType eType, CFile* pFile=NULL);
	BOOL GetFlirCameraMeasurementParameters(ImageFileType eType, CFile* pFile=NULL);
	void GetTemperatureRange();
	BOOL SetFlirCameraMeasurementParameters(tagIRCameraMeasurementParameters& parameters);

	void MakePalette(void *ptr, BOOL bReversed=FALSE);
};

