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
	CThermalImage*		m_pIRImage;				// ����ͼ�ף�main image��
	CThermalImage*		m_pVLImage;				// �ɼ���ͼ�ף�assistant image��
	CVideoDirectShowWnd	m_wndImage;				// ��ʾ����ͼ�׵Ļ���
	float				m_fMaxTemperature;		// ����ͼ�׵����/����¶�
	float				m_fMinTemperature;
	CString				m_sCurrentIRFile;		// ��ǰ�ļ�����·������Ϊ�˱��ڴ洢����������ñ�����doc�в�ͬ��ˢ��
	CString				m_sCurrentVLFile;

	BOOL				m_bModified;			// �Ƿ��ѽ����˷������(ֻ��¼����ͼ�׵Ĵ���״̬)

	CFlirCameraCtrl		m_ctrlFlirCamera;
	CPaletteStatic		m_ctrlPalette;
	int					m_nCurPaletteIndex;
	BOOL				m_bShowPalette;
	int					m_nPaleteWidth;
	float				m_fZoomFactor;			// ��ͼ��(����ͼ��)���ű���
	tagIrFileBaseInfo	m_IrFileBaseInfo;
	ImageFileType		m_nCurMainImageType;	// ��ǰ���ص�main image����
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

	BOOL LoadTargetImage(CString sImageFilePathAndName, BOOL bForcedAsVLImage=FALSE);	// �ж��ĵ����Ͳ�����ͼ��

	void RepositionCtrls();								// �������н����еĿؼ�
	void Zoomin();
	void Zoomout();
	void SetPalette(int iPaletteIndex=2, BOOL bReversed=FALSE);
	void SetModified(BOOL bModified=TRUE)
	{
		m_bModified = bModified;
	}

	int ConfirmSaveAnalysisResult();					// ȷ���Ƿ����������Ͻ�����Ƿ񱣴�

// Implementation

protected:

	HWND				m_hOwner;
	CWnd*				m_pOwner;
	BOOL				m_bWinInitialized;
	BOOL				m_bCameraTypeIsIR;		// ����main image�жϣ���Ҫ��Ϊ��TSMO���ݣ���ʵ������;

	HANDLE				m_hFlirTemperatureImage;		// ����FLIRͼ���ڴ�ת��������������
	HANDLE				m_hFlirLUT;						// ���ڼ���FLIR���������
	int					m_nFlirTemperatureImageSize;	// ������תFLIR�¶�ͼ��(��FLIRͼ������/pixel�йأ����ǵ����ļ���С)

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
	// ͼ�׼���
public:
	void ClearIrImageInfo(BOOL bForDestroy=FALSE);				// ���������е���ʱ����ΪTRUE
protected:
	BOOL LoadIrImage(CString sImageFilePathAndName, ImageFileType eType);				// ���غ���ͼ��
	// ����FLIR����ͼ��
	BOOL InitFlirCameraCtrl();															// ��ʼ��FLIR�ؼ�
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

