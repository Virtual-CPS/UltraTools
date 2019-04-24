// LoadImageForAnalysis.cpp : implementation of the CLoadImageForAnalysisWnd class
//

#include "stdafx.h"
#include "UltraTools.h"
#include "LoadImageForAnalysisWnd.h"
#include "Utility/MemDC.h"

#include "MainFrm.h"
#include "SDK/DefinesForFlirCamera.h"
#include "SDK/FlirFpfImage.h"

#include "ThermalImage.h"
#include "UI/PaletteColorTable.h"
#include "GlobalDefsForSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CLoadImageForAnalysisWnd, CBCGPDialog)

CLoadImageForAnalysisWnd ::CLoadImageForAnalysisWnd (CWnd* pParent /*=NULL*/)
{
	EnableVisualManagerStyle();

	m_pOwner = NULL;

	m_bIsActive = TRUE;
	m_bModified = FALSE;
	m_bShowPalette = TRUE;
	m_nCurPaletteIndex = 2;
	m_nPaleteWidth = PALETTE_DEFAULT_WIDTH;

	m_bCameraTypeIsIR = TRUE;

	m_hFlirTemperatureImage = NULL;
	m_hFlirLUT = NULL;
	m_nFlirTemperatureImageSize = 0;

	m_pIRImage = NULL;
	m_pVLImage = NULL;
	m_sCurrentIRFile = _T("");
	m_sCurrentVLFile = _T("");
	m_fZoomFactor = 1.0f;

	m_fMaxTemperature = 0.f;
	m_fMinTemperature = 0.f;

	m_bWinInitialized = FALSE;
}

CLoadImageForAnalysisWnd::~CLoadImageForAnalysisWnd()
{
	ClearIrImageInfo(TRUE);

	if( m_pVLImage != NULL )
	{
		::delete m_pVLImage;
		m_pVLImage = NULL;
	}
}

void CLoadImageForAnalysisWnd::OnDestroy()
{
	if(::IsWindow(m_ctrlFlirCamera.GetSafeHwnd()))
		m_ctrlFlirCamera.Disconnect();

	CBCGPDialog::OnDestroy();
}

void CLoadImageForAnalysisWnd::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadImageForAnalysisWnd)	
	DDX_Control(pDX, IDC_IMAGE, m_wndImage);
	DDX_Control(pDX, IDC_PALETTE, m_ctrlPalette);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_FLIR_CAM_CTRL, m_ctrlFlirCamera);
}


BEGIN_MESSAGE_MAP(CLoadImageForAnalysisWnd , CBCGPDialog)
	//{{AFX_MSG_MAP(CLoadImageForAnalysisWnd )
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLoadImageForAnalysisWnd  message handlers

BOOL CLoadImageForAnalysisWnd::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	m_ctrlFlirCamera.ShowWindow(SW_HIDE);
	m_bWinInitialized = TRUE;
	m_wndImage.m_pOwner = this;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLoadImageForAnalysisWnd::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	if(!m_bWinInitialized)
		return;
	if(GetSafeHwnd() == NULL)
		return;

	RepositionCtrls();
}

// 重新排列界面中的控件
void CLoadImageForAnalysisWnd::RepositionCtrls()
{
	if (m_pIRImage == NULL)
	{
		CRect rectDummy;
		rectDummy.SetRectEmpty();
		m_wndImage.MoveWindow(rectDummy);
		m_ctrlPalette.MoveWindow(rectDummy);
		Invalidate(TRUE);
		return;
	}

	CRect rectImage, rectPalette, rect;

	int x = (int)(m_pIRImage->GetWidth() * m_fZoomFactor);
	int y = (int)(m_pIRImage->GetHeight() * m_fZoomFactor);

	rectImage = CRect(0, 0, x, y);
	rect = rectImage;
	m_wndImage.MoveWindow(rect, FALSE);

	// 确定调色板位置（不改变图谱大小，尽量与图谱不重叠）
	m_nPaleteWidth = 0;
	if(IsIrImage())
	{
		if(m_bShowPalette)
		{
			if(m_ctrlPalette.IsShowCaclibration())	// 显示刻度
				m_nPaleteWidth = 40;
			else if(m_ctrlPalette.IsShowLimits())	// 显示上下限，不显示刻度
				m_nPaleteWidth = 40;
			else
				m_nPaleteWidth = 10;

			//rectPalette = CRect(x-5, 5, x-m_nPaleteWidth, y-5);
			rectPalette = rectImage;
			rectPalette.left = rectPalette.right;
			rectPalette.right = rectPalette.left + m_nPaleteWidth;
			m_ctrlPalette.MoveWindow(rectPalette, FALSE);
			m_ctrlPalette.ShowWindow(SW_SHOW);
		}
		else
			m_ctrlPalette.ShowWindow(SW_HIDE);
	}
	else
		m_ctrlPalette.ShowWindow(SW_HIDE);

	m_ctrlPalette.DrawPalette(m_nCurPaletteIndex);
	m_ctrlPalette.Invalidate(TRUE);

	rect = CRect(0, 0, x+m_nPaleteWidth, y);
	CWnd* pParent = GetParent();
	if(pParent)
	{
		CString sClassName(pParent->GetRuntimeClass()->m_lpszClassName);
		sClassName.MakeUpper();
		if(sClassName.Find(_T("VIEW")) < 0)
			ScreenToClient(rect);			// FormView不需，否则偏移
	}
	MoveWindow(rect);
}

void CLoadImageForAnalysisWnd::Zoomin()
{
	if (m_fZoomFactor >= 16)
		return;

	if(m_fZoomFactor == 0.25f)
		m_fZoomFactor = 0.50f;
	else if(m_fZoomFactor == 0.50f)
		m_fZoomFactor = 0.75f;
	else if(m_fZoomFactor == 0.75f)
		m_fZoomFactor = 1.00f;
	else if (m_fZoomFactor == 1.00f)
		m_fZoomFactor = 1.25f;
	else if (m_fZoomFactor == 1.25f)
		m_fZoomFactor = 1.50f;
	else if (m_fZoomFactor == 1.50f)
		m_fZoomFactor = 1.75f;
	else if (m_fZoomFactor == 1.75f)
		m_fZoomFactor = 2.00f;
	else
		m_fZoomFactor *= 2.0f;
}

void CLoadImageForAnalysisWnd::Zoomout()
{
	int nWidth = 0;
	if(m_pIRImage)
		nWidth = (int)(m_pIRImage->GetWidth() * m_fZoomFactor);
	if (m_fZoomFactor<=0.0625 || nWidth<=160)
		return;

	if(m_fZoomFactor == 2.00f)
		m_fZoomFactor = 1.75f;
	else if(m_fZoomFactor == 1.75f)
		m_fZoomFactor = 1.50f;
	else if (m_fZoomFactor == 1.50f)
		m_fZoomFactor = 1.25f;
	else if (m_fZoomFactor == 1.25f)
		m_fZoomFactor = 1.00f;
	else if (m_fZoomFactor == 1.00f)
		m_fZoomFactor = 0.75f;
	else if (m_fZoomFactor == 0.75f)
		m_fZoomFactor = 0.50f;
	else if (m_fZoomFactor == 0.50f)
		m_fZoomFactor = 0.25f;
	else
		m_fZoomFactor /= 2.0f;
}

// 禁止用esc停止本窗口
// 激活相关操作，
BOOL CLoadImageForAnalysisWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE ||
		pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		return TRUE;
	}
	else if (pMsg->message==WM_LBUTTONDOWN || pMsg->wParam==WM_RBUTTONDOWN ||
		pMsg->message==WM_LBUTTONDBLCLK)
	{
		m_bIsActive = TRUE;
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CLoadImageForAnalysisWnd ::OnPaint() 
{
	CPaintDC dc(this);

	//DrawCameraImage(&dc, clrFont, clrFrame);
	m_wndImage.Invalidate();		// 强制重绘测温objs
}

BOOL CLoadImageForAnalysisWnd::GetImageXY(long *x, long *y)
{
	if(NULL == m_pIRImage)
		return FALSE;

	float fx =(float)(*x);
	float fy =(float)(*y);

	fx /= m_fZoomFactor;
	fy /= m_fZoomFactor;

	*x = (long)fx;
	*y = (long)fy;

	return m_pIRImage->IsInside(*x, *y);
}

// 判断文档类型并加载图谱
BOOL CLoadImageForAnalysisWnd::LoadTargetImage(CString sImageFilePathAndName, BOOL bForcedAsVLImage)
{
	BOOL bRet = TRUE;
	int nType;
	CString sExt, sInfo;

	if(sImageFilePathAndName.GetLength() <=0)
	{
		bRet = FALSE;
		goto EXIT_LOAD_IMAGE;
	}

	// 获取文件扩展名和文件类型
	nType = GetTypeFromFileName(sImageFilePathAndName, sExt);

	if(bForcedAsVLImage)
		goto LOAD_IMAGE_AS_VL;

	// 判断文件类型并加载图谱
	// 特殊扩展名
	if (sExt == "fpf")
	{
		bRet = LoadIrImage(sImageFilePathAndName, IMAGE_FILE_TYPE_FLIR_FPF);
		goto EXIT_LOAD_IMAGE;
	}
	if (sExt == "img")
	{
		bRet = LoadIrImage(sImageFilePathAndName, IMAGE_FILE_TYPE_FLIR_IMG);
		goto EXIT_LOAD_IMAGE;
	}
	if (sExt == "seq")
	{
		bRet = LoadIrImage(sImageFilePathAndName, IMAGE_FILE_TYPE_FLIR_SEQ);
		goto EXIT_LOAD_IMAGE;
	}

	// 常规扩展名
	if(nType == THERMAL_IMAGE_FORMAT_JPG)			// EXIF jpegs
	{
/*		FILE* hfile;
		//hfile = _tfopen(sImageFilePathAndName, _T("rb"));
		_tfopen_s(&hfile, sImageFilePathAndName, _T("rb"));
		if (hfile)
		{
			// FLIR利用常规的JPG存储红外图谱，故用m_exif判断JPG是否是红外图谱
			CxImageJPG exif;
			ZeroMemory(exif.m_exifinfo.CameraMake, sizeof(exif.m_exifinfo.CameraMake));
			ZeroMemory(exif.m_exifinfo.CameraModel, sizeof(exif.m_exifinfo.CameraModel));
			exif.DecodeExif(hfile);
			fclose(hfile);
			sInfo.Format(_T("%s"), exif.m_exifinfo.CameraMake);
			sInfo.MakeUpper();
			if (sInfo.Find(_T("FLIR")) >= 0)
			{
				bRet = LoadIrImage(sImageFilePathAndName, IMAGE_FILE_TYPE_FLIR_JPG);
				if(bRet)
					goto EXIT_LOAD_IMAGE;

				// 否则按常规图谱加载
			}
		}
*/	}

LOAD_IMAGE_AS_VL:

	if(THERMAL_IMAGE_FORMAT_UNKNOWN == nType)
	{
		bRet = FALSE;
		goto EXIT_LOAD_IMAGE;
	}

	//-------------------------------------------
	// 加载常规图谱
	if (m_pIRImage == NULL)						// 尚未加载红外图谱
	{
		m_fZoomFactor = 1.0f;					// 非红外图谱初始放大比例
		m_nCurMainImageType = IMAGE_FILE_TYPE_NORMAL;
		m_bCameraTypeIsIR = FALSE;
		m_wndImage.OnDelAll();					// 清除全部测温工具
		m_wndImage.m_bCameraTypeIsIR = FALSE;
		m_IrFileBaseInfo.Reset();				// 清空
		m_IrMeasurementParameters.Reset();
	}
	if(m_pVLImage != NULL)
	{
		::delete m_pVLImage;
		m_pVLImage = NULL;
		m_sCurrentVLFile = _T("");
	}

	m_pVLImage = new CThermalImage(sImageFilePathAndName, nType, false, false);
	if (!m_pVLImage->IsValid())
	{
		CString sInfo(m_pVLImage->GetLastError());
		AfxMessageBox(sInfo);
		::delete m_pVLImage;
		m_pVLImage = NULL;
		bRet = FALSE;
		goto EXIT_LOAD_IMAGE;
	}
	else
		bRet = TRUE;
	m_sCurrentVLFile = sImageFilePathAndName;

EXIT_LOAD_IMAGE:

	return bRet;
}

// 清除已有红外图谱及温度信息
void CLoadImageForAnalysisWnd::ClearIrImageInfo(BOOL bForDestroy)
{
	// 清除全部测温工具，析构函数中调用导致无效窗口错误
	if(!bForDestroy)
		m_wndImage.OnDelAll();

	m_nCurMainImageType = IMAGE_FILE_TYPE_INVALID;
	m_bCameraTypeIsIR = FALSE;
	m_wndImage.m_bCameraTypeIsIR = FALSE;
	m_IrFileBaseInfo.Reset();				// 清空
	m_IrMeasurementParameters.Reset();

	if(m_pIRImage != NULL)
	{
		::delete m_pIRImage;
		m_pIRImage = NULL;
		m_sCurrentIRFile = _T("");
	}
	if (m_hFlirLUT)
	{
		GlobalFree(m_hFlirLUT);
		m_hFlirLUT = NULL;
	}
	if (m_hFlirTemperatureImage)
	{
		GlobalFree(m_hFlirTemperatureImage);
		m_hFlirTemperatureImage = NULL;
	}
}

// 确认是否放弃分析诊断结果、是否保存
// 返回TRUE=保存；FALSE=丢弃分析结果/不保存
int CLoadImageForAnalysisWnd::ConfirmSaveAnalysisResult()
{
	if(NULL==m_pIRImage || !m_bModified)
		return IDNO;

	CString sFileName, sTem;
	int nPos;
	nPos = m_sCurrentIRFile.ReverseFind('\\');
	if(nPos > 0)
		sFileName = m_sCurrentIRFile.Mid(nPos+1);
	else
		sFileName = m_sCurrentIRFile;
	sTem.Format(_T("%s\n\n保存针对以上红外图谱的分析诊断？"), sFileName);
	return MessageBox(sTem, _T("请确认是否保存"), MB_YESNOCANCEL|MB_ICONQUESTION);
}

// 加载红外图谱
BOOL CLoadImageForAnalysisWnd::LoadIrImage(CString sImageFilePathAndName, ImageFileType eType)
{
	// 检查是否已处理先前加载的红外图谱
	int nRet = ConfirmSaveAnalysisResult();
	if(IDCANCEL == nRet)
		return FALSE;
	if(IDYES == nRet)
//		OnSaveAnalysisResult();
		AfxMessageBox(_T("CLoadImageForAnalysisWnd::LoadIrImage()"));

	BOOL bRet = FALSE;
	int nPos;

	// 清除已有红外图谱及温度信息
	ClearIrImageInfo();
	m_fZoomFactor = 1.0f;					// 红外图谱初始放大比例
	SetModified(FALSE);// 避免放弃保存时“漏置”标记，只能放这，因m_wndImage.OnDelAll()重置m_bModified

	switch (eType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
	case IMAGE_FILE_TYPE_FLIR_FPF:
		bRet = LoadFlirImageFile(sImageFilePathAndName, eType);
		break;
	}

	if(bRet && eType==IMAGE_FILE_TYPE_FLIR_JPG && m_IrFileBaseInfo.sLens.GetLength()<2)
	{	// FLIR热像仪拍摄的普光图谱
		if(m_pIRImage)
		{
			::delete m_pIRImage;
			m_pIRImage = NULL;
		}
		bRet = FALSE;
	}

	if(bRet)
	{
		m_bCameraTypeIsIR = TRUE;
		m_wndImage.m_bCameraTypeIsIR = TRUE;
		m_nCurMainImageType = eType;
		m_sCurrentIRFile = sImageFilePathAndName;

		nPos = sImageFilePathAndName.ReverseFind('\\');
		if(nPos >= 0)
			m_IrFileBaseInfo.sFileName = sImageFilePathAndName.Mid(nPos+1);
		else
			m_IrFileBaseInfo.sFileName = sImageFilePathAndName;
	}

	if(m_bCameraTypeIsIR)
		GetTemperatureRange();

	return bRet;
}

HANDLE g_hDm60CallbackHandle;					//用于保存回调函数中得到的参数。
void * g_pDm60CallbackStream = NULL;			//用于保存回调函数中得到的参数。
int g_nDm60CallbackLen, g_nDm60CallbackTime;	//用于保存回调函数中得到的参数。

// DM60视频流回调函数
int CALLBACK  DM60StreamCallbackFunc(HANDLE hHandle, void *stream, int len, int nTime)
{
	g_pDm60CallbackStream = stream;
	g_nDm60CallbackLen = len;
	g_nDm60CallbackTime = nTime;
	g_hDm60CallbackHandle = hHandle;

	return 1;
}

// 初始化FLIR控件
BOOL CLoadImageForAnalysisWnd::InitFlirCameraCtrl()
{
	BOOL bRet = FALSE;
	short ret;

	// Get camera status
	VARIANT va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_STATUS);
	int nStatus = va.iVal;
	VariantClear(&va);
	if (nStatus == CAM_DISCONNECTED)
	{
		CString sIP = _T("127.0.0.1");
		//ret = m_ctrlFlirCamera.Connect(CAM_A320S, 0, DEVICE_FILE, CAM_INTF_NONE, sIP);
		ret = m_ctrlFlirCamera.Connect(CAM_THERMOVISION, 0, DEVICE_FILE, CAM_INTF_NONE, sIP);
		if (ret == IS_OK)
		{
			bRet = TRUE;
		}
		else
		{
			CString msg = _T("连接时发生错误:\n") + m_ctrlFlirCamera.GetError(ret);
			AfxMessageBox(msg, MB_OK | MB_ICONERROR);
		}
	}
	else
		bRet = TRUE;

	return bRet;
}

// 加载FLIR红外图谱
BOOL CLoadImageForAnalysisWnd::LoadFlirImageFile(CString sImageFilePathAndName, ImageFileType eType)
{
	int nImageType, nStatus;
	VARIANT va;
	BOOL bRet = FALSE;

	// Get camera status
	va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_STATUS);
	nStatus = va.iVal;
	VariantClear(&va);
	if (nStatus == CAM_DISCONNECTED)
	{
		if(!InitFlirCameraCtrl())
			return FALSE;
	}

	if (eType==IMAGE_FILE_TYPE_FLIR_JPG ||
		eType==IMAGE_FILE_TYPE_FLIR_IMG ||
		eType==IMAGE_FILE_TYPE_FLIR_SEQ)
	{
		// Set IR file
		COleVariant irFile(sImageFilePathAndName);
		m_ctrlFlirCamera.SetCameraProperty(PROP_IRFILE, irFile);

		if(!GetFlirLUT())
			return FALSE;

		GetFlirCameraBaseInfo(eType, NULL);
		GetFlirCameraMeasurementParameters(eType, NULL);

		// Get image pixel size
		va = m_ctrlFlirCamera.GetCameraProperty(PROP_IMAGE_PIXSIZE);
		nImageType = (va.iVal == 0 ? 0 : 4);
		VariantClear(&va);

		va = m_ctrlFlirCamera.GetImage(20 + nImageType);
		if (va.vt == VT_I2) // Error
		{
			VariantClear(&va);
			return FALSE;
		}
		else if (va.vt != VT_I4)
		{
			VariantClear(&va);
			return FALSE;
		}

		HGLOBAL h = (HGLOBAL)va.lVal;
		bRet = ConvertFlirImage(nImageType, h);

		// Free memory
		GlobalFree(h);
		VariantClear(&va);
	}
	else if (eType == IMAGE_FILE_TYPE_FLIR_FPF)
	{
		// Set IR file
		COleVariant irFile(sImageFilePathAndName);
		m_ctrlFlirCamera.SetCameraProperty(PROP_IRFILE, irFile);

		if(!GetFlirLUT())
			return FALSE;

		FPF_IMAGE_DATA_T imgData;
		COleSafeArray sa;
		float* pDst;
		DWORD nRead, nImgSize, nElements[2];
		CFile fpfFile;
		if (!fpfFile.Open(sImageFilePathAndName, CFile::modeRead))
			return FALSE;

		// Read image header from file
		fpfFile.Seek(0, CFile::begin);
		nRead = fpfFile.Read(&imgData, sizeof(FPF_IMAGE_DATA_T));
		if (nRead != sizeof(FPF_IMAGE_DATA_T))
		{
			fpfFile.Close();
			return FALSE;
		}
		nImgSize = imgData.xSize * imgData.ySize * sizeof(float);

		if (!GetFlirCameraBaseInfo(eType, &fpfFile))
		{
			fpfFile.Close();
			return FALSE;
		}
		GetFlirCameraMeasurementParameters(eType, &fpfFile);

		nElements[0] = imgData.xSize;
		nElements[1] = imgData.ySize;
		sa.Create(VT_R4, 2, nElements);
		sa.AccessData((LPVOID*)&pDst);
		fpfFile.Seek(sizeof(FPFHEADER_T), CFile::begin);
		nRead = fpfFile.Read(pDst, nImgSize);
		if (nRead != nImgSize)
		{
			sa.UnaccessData();
			fpfFile.Close();
			return FALSE;
		}
		sa.UnaccessData();
		bRet = ConvertFlirImage(3, sa);

		fpfFile.Close();
	}

	return bRet;
}

BOOL CLoadImageForAnalysisWnd::ConvertFlirImage(int nImageType, COleSafeArray& sa)
{
	long w, h;
	sa.GetUBound(1, &w);
	sa.GetUBound(2, &h);
	w++;
	h++;

	if (m_hFlirTemperatureImage)
	{
		GlobalFree(m_hFlirTemperatureImage);
		m_hFlirTemperatureImage = NULL;
	}

	// Calculate size and allocate memory
	HANDLE hDIB;
	int imgSize;
	int xAlignSize = ((w + 3) & ~3);  // Must be 4 byte aligned
	imgSize  = sizeof(BITMAPINFOHEADER);
	imgSize += 256 * sizeof(RGBQUAD);
	imgSize += (xAlignSize * h);
	hDIB = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, imgSize);
	if (hDIB == NULL)
	{
		AfxMessageBox(_T("分配内存失败!"));
		return FALSE;
	}

	BYTE* pDIB = (BYTE*)GlobalLock(hDIB);
	BYTE* pBits = pDIB + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	// Allocate and copy image to local buffer
	VARTYPE vt = (nImageType < 2 ? VT_I2 : (nImageType < 4 ? VT_R4 : VT_UI1));
	int pixSize = (vt == VT_UI1 ? 1 : (vt == VT_I2 ? 2 : 4));
	imgSize = xAlignSize * h * pixSize;
	m_hFlirTemperatureImage = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, imgSize);
	m_nFlirTemperatureImageSize = imgSize;
	if (m_hFlirTemperatureImage == NULL)
	{
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		AfxMessageBox(_T("图谱内存分配失败!"));
		return FALSE;
	}

	BYTE* pImage = (BYTE *)GlobalLock(m_hFlirTemperatureImage);

	if (vt == VT_UI1)
	{
		BYTE HUGEP *pSrc;
		sa.AccessData((void HUGEP**)&pSrc);
		int rowSize = xAlignSize;
		for (int y = 0; y < h; y++)
			memcpy(&pImage[y * rowSize], &pSrc[y * w], rowSize);
		sa.UnaccessData();
	}
	else if (vt == VT_I2)
	{
		WORD HUGEP *pSrc;
		sa.AccessData((void HUGEP**)&pSrc);
		int rowSize = xAlignSize * pixSize;
		for (int y = 0; y < h; y++)
			memcpy(&pImage[y * rowSize], &pSrc[y * w], rowSize);
		sa.UnaccessData();
	}
	else if (vt == VT_R4)
	{
		float HUGEP *pSrc;
		sa.AccessData((void HUGEP**)&pSrc);
		int rowSize = xAlignSize * pixSize;
		for (int y = 0; y < h; y++)
			memcpy(&pImage[y * rowSize], &pSrc[y * w], rowSize);
		sa.UnaccessData();
	}

	// Copy pixel data
	BYTE* pDst = pBits;
	int sz = xAlignSize * h;
	if (vt == VT_UI1)
	{
		sz = xAlignSize * h;
		BYTE* pSrc = (BYTE *)pImage;
		while (sz--)
		{
			*pDst++ = *pSrc++;
		}
	}
	else if (vt == VT_I2)
	{
		WORD smin = 65535, smax = 0;
		WORD* pSrc = (WORD *)pImage;
		while (sz--)
		{
			smin = __min(*pSrc, smin);
			smax = __max(*pSrc, smax);
			pSrc++;
		}

		sz = xAlignSize * h;
		pSrc = (WORD *)pImage;
		WORD span = smax - smin;
		if (span == 0) span = 1;
		while (sz--)
		{
			long sample = ((*pSrc++ - smin) * 256 / span );
			*pDst++ = (BYTE)sample;
		}
	}
	else if (vt == VT_R4)
	{
		// Temperature or object value image
		float smin = 5000.0, smax = 0.0;
		float* pSrc = (float *)pImage;
		while (sz--)
		{
			smin = __min(*pSrc, smin);
			smax = __max(*pSrc, smax);
			pSrc++;
		}

		sz = xAlignSize * h;
		pSrc = (float *)pImage;
		float span = smax - smin;
		if (span < 1) span = 1.0;

		while (sz--)
		{
			BYTE sample = (BYTE)((*pSrc++ - smin) * 256.0 / span );
			*pDst++ = sample;
		}
	}

	// Set bitmap info header
	BITMAPINFOHEADER   bihS;  // Temporary structure
	LPBITMAPINFOHEADER bitHeadP = (LPBITMAPINFOHEADER)pDIB;

	bihS.biSize          = sizeof(BITMAPINFOHEADER);
	bihS.biWidth         = w;
	// Specifies the height of the bitmap, in pixels.
	// If biHeight is positive, the bitmap is a bottom-up DIB and
	// its origin is the lower-left corner. If biHeight is negative,
	// the bitmap is a top-down DIB and its origin is the upper-left corner. 
	bihS.biHeight        = -h;
	bihS.biPlanes        = 1;
	bihS.biBitCount      = 8;
	bihS.biCompression   = BI_RGB;
	bihS.biSizeImage     = xAlignSize * h;
	bihS.biXPelsPerMeter = 0;
	bihS.biYPelsPerMeter = 0;
	bihS.biClrUsed       = 256;
	bihS.biClrImportant  = 0;
	*bitHeadP            = bihS;  // Copy struct into DIB

	// Set palette
	MakePalette((void *)&((LPBITMAPINFO)pDIB)->bmiColors[0]);

	CDC* pDC = GetDC();
	HDC hBufDC;   
	HBITMAP hBitmap, hBitTemp;   

	//创建设备上下文(HDC)   
	hBufDC = CreateCompatibleDC(pDC->m_hDC);   

	//创建HBITMAP   
	hBitmap = CreateCompatibleBitmap(pDC->m_hDC, w, h);   
	hBitTemp = (HBITMAP) SelectObject(hBufDC, hBitmap);   

	//得到位图缓冲区   
	int oldMode = SetStretchBltMode(hBufDC, COLORONCOLOR);
	int ret = ::StretchDIBits(hBufDC,
		0, 0, w, h,
		0, 0, w, h, pBits, (LPBITMAPINFO)pDIB, DIB_RGB_COLORS, SRCCOPY);
	SetStretchBltMode(hBufDC, oldMode); // Restore mode

	//得到最终的位图信息   
	hBitmap = (HBITMAP) SelectObject(hBufDC, hBitTemp);
	m_pIRImage = new CThermalImage;
	m_pIRImage->CreateFromHBITMAP(hBitmap);
	BOOL bRet = TRUE;
	if (!m_pIRImage->IsValid())
	{
		CString sInfo(m_pIRImage->GetLastError());
		AfxMessageBox(sInfo);
		::delete m_pIRImage;
		m_pIRImage = NULL;
		bRet = FALSE;
	}
	else
		SetPalette(m_nCurPaletteIndex);			// BPP统一为8

	//释放内存   
	DeleteObject(hBitmap);
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	ReleaseDC(pDC);

	GlobalUnlock(m_hFlirTemperatureImage);
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	return bRet;
}

BOOL CLoadImageForAnalysisWnd::ConvertFlirImage(int nImageType, HGLOBAL hMem) 
{
	BOOL bRet = TRUE;
	int w, h;
	VARIANT va;

	va = m_ctrlFlirCamera.GetCameraProperty(PROP_IMAGE_WIDTH);
	w = (int)va.iVal;
	VariantClear(&va);
	va = m_ctrlFlirCamera.GetCameraProperty(PROP_IMAGE_HEIGHT);
	h = (int)va.iVal;
	VariantClear(&va);

	if (m_hFlirTemperatureImage)
	{
		GlobalFree(m_hFlirTemperatureImage);
		m_hFlirTemperatureImage = NULL;
	}

	// Calculate size and allocate memory
	HANDLE hDIB;
	int imgSize;
	int xAlignSize = ((w + 3) & ~3);				// Must be 4 byte aligned
	imgSize  = sizeof(BITMAPINFOHEADER);
	imgSize += 256 * sizeof(RGBQUAD);
	imgSize += (xAlignSize * h);
	hDIB = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, imgSize);
	if (hDIB == NULL)
	{
		AfxMessageBox(_T("分配内存失败!"));
		return FALSE;
	}

	BYTE* pDIB = (BYTE*)GlobalLock(hDIB);
	BYTE* pBits = pDIB + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	// Allocate and copy image to local buffer
	VARTYPE vt = (nImageType < 2 ? VT_I2 : (nImageType < 4 ? VT_R4 : VT_UI1));
	int pixSize = (vt == VT_UI1 ? 1 : (vt == VT_I2 ? 2 : 4));

	imgSize = xAlignSize * h * pixSize;
	m_hFlirTemperatureImage = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, imgSize);
	m_nFlirTemperatureImageSize = imgSize;
	if (m_hFlirTemperatureImage == NULL)
	{
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		AfxMessageBox(_T("图谱内存分配失败!"));
		return FALSE;
	}

	BYTE* pImage = (BYTE *)GlobalLock(m_hFlirTemperatureImage);

	if (vt == VT_UI1)
	{
		BYTE *pSrc;
		pSrc = (BYTE*)GlobalLock(hMem);
		int rowSize = xAlignSize;
		for (int y = 0; y < h; y++)
			memcpy(&pImage[y * rowSize], &pSrc[y * w], rowSize);
		GlobalUnlock(hMem);
	}
	else if (vt == VT_I2)
	{
		WORD *pSrc;
		pSrc = (WORD*)GlobalLock(hMem);

		int rowSize = xAlignSize * pixSize;
		for (int y = 0; y < h; y++)
			memcpy(&pImage[y * rowSize], &pSrc[y * w], rowSize);
		GlobalUnlock(hMem);
	}
	else if (vt == VT_R4)
	{
		float *pSrc;
		pSrc = (float*)GlobalLock(hMem);

		int rowSize = xAlignSize * pixSize;
		for (int y = 0; y < h; y++)
			memcpy(&pImage[y * rowSize], &pSrc[y * w], rowSize);
		GlobalUnlock(hMem);
	}

	// Copy pixel data
	BYTE* pDst = pBits;
	int sz = xAlignSize * h;
	if (vt == VT_UI1)
	{
		sz = xAlignSize * h;
		BYTE* pSrc = (BYTE *)pImage;
		while (sz--)
		{
			*pDst++ = *pSrc++;
		}
	}
	else if (vt == VT_I2)
	{
		WORD smin = 65535, smax = 0;
		WORD* pSrc = (WORD *)pImage;
		while (sz--)
		{
			smin = __min(*pSrc, smin);
			smax = __max(*pSrc, smax);
			pSrc++;
		}

		sz = xAlignSize * h;
		pSrc = (WORD *)pImage;
		WORD span = smax - smin;
		if (span == 0) span = 1;
		while (sz--)
		{
			long sample = ((*pSrc++ - smin) * 256 / span );
			*pDst++ = (BYTE)sample;
		}
	}
	else if (vt == VT_R4)
	{
		// Temperature or object value image
		float smin = 5000.0, smax = 0.0;
		float* pSrc = (float *)pImage;
		while (sz--)
		{
			smin = __min(*pSrc, smin);
			smax = __max(*pSrc, smax);
			pSrc++;
		}

		sz = xAlignSize * h;
		pSrc = (float *)pImage;
		float span = smax - smin;
		if (span < 1)
			span = 1.0;

		while (sz--)
		{
			BYTE sample = (BYTE)((*pSrc++ - smin) * 256.0 / span );
			*pDst++ = sample;
		}
	}

	// Set bitmap info header
	BITMAPINFOHEADER   bihS;  // Temporary structure
	LPBITMAPINFOHEADER bitHeadP = (LPBITMAPINFOHEADER)pDIB;

	bihS.biSize          = sizeof(BITMAPINFOHEADER);
	bihS.biWidth         = w;
	// Specifies the height of the bitmap, in pixels.
	// If biHeight is positive, the bitmap is a bottom-up DIB and
	// its origin is the lower-left corner. If biHeight is negative,
	// the bitmap is a top-down DIB and its origin is the upper-left corner. 
	bihS.biHeight        = -h;
	bihS.biPlanes        = 1;
	bihS.biBitCount      = 8;
	bihS.biCompression   = BI_RGB;
	bihS.biSizeImage     = xAlignSize * h;
	bihS.biXPelsPerMeter = 0;
	bihS.biYPelsPerMeter = 0;
	bihS.biClrUsed       = 256;
	bihS.biClrImportant  = 0;
	*bitHeadP            = bihS;  // Copy struct into DIB

	// Set palette
	MakePalette((void *)&((LPBITMAPINFO)pDIB)->bmiColors[0]);

	CDC* pDC = GetDC();
	HDC hBufDC;   
	HBITMAP hBitmap, hBitTemp;   

	//创建设备上下文(HDC)   
	hBufDC = CreateCompatibleDC(pDC->m_hDC);   

	//创建HBITMAP   
	hBitmap = CreateCompatibleBitmap(pDC->m_hDC, w, h);   
	hBitTemp = (HBITMAP) SelectObject(hBufDC, hBitmap);   

	//得到位图缓冲区   
	int oldMode = SetStretchBltMode(hBufDC, COLORONCOLOR);
	int ret = ::StretchDIBits(hBufDC,
		0, 0, w, h,
		0, 0, w, h, pBits, (LPBITMAPINFO)pDIB, DIB_RGB_COLORS, SRCCOPY);
	SetStretchBltMode(hBufDC, oldMode); // Restore mode

	//得到最终的位图信息   
	hBitmap = (HBITMAP) SelectObject(hBufDC, hBitTemp);
	m_pIRImage = new CThermalImage;
	m_pIRImage->CreateFromHBITMAP(hBitmap);
	if (!m_pIRImage->IsValid())
	{
		CString sInfo(m_pIRImage->GetLastError());
		AfxMessageBox(sInfo);
		delete m_pIRImage;
		m_pIRImage = NULL;
		bRet = FALSE;
	}
	else
		SetPalette(m_nCurPaletteIndex);			// BPP统一为8

	//释放内存   
	DeleteObject(hBitmap);
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	ReleaseDC(pDC);

	GlobalUnlock(m_hFlirTemperatureImage);
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	return bRet;
}

BOOL CLoadImageForAnalysisWnd::GetFlirLUT()
{
	// Get camera status
	VARIANT va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_STATUS);
	int nStatus = va.iVal;
	VariantClear(&va);
	if (nStatus == CAM_DISCONNECTED)
	{
		AfxMessageBox(_T("No camera connected"));
		return FALSE;
	}

	// get 16 bit transformation table converting raw pixel
	// to Kelvin temperature
	va = m_ctrlFlirCamera.GetLUT(1);

	if (va.vt != (VT_ARRAY | VT_R4))
	{
		VariantClear(&va);
		return FALSE;
	}

	COleSafeArray sa;
	sa.Attach(va);

	DWORD lutSize = sa.GetOneDimSize() * sizeof(float);

	if (m_hFlirLUT)
	{
		GlobalFree(m_hFlirLUT);
		m_hFlirLUT = NULL;
	}

	// Allocate and copy LUT to local buffer
	m_hFlirLUT = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, lutSize);
	if (m_hFlirLUT == NULL)
	{
		VariantClear(&va);
		AfxMessageBox(_T("创建LUT失败。"));
		return FALSE;
	}

	BYTE* pLUT = (BYTE *)GlobalLock(m_hFlirLUT);
	WORD HUGEP *pSrc;
	sa.AccessData((void HUGEP**)&pSrc);
	memcpy(pLUT, pSrc, lutSize);
	sa.UnaccessData();
	GlobalUnlock(m_hFlirLUT);
	VariantClear(&va);

	return TRUE;
}

BOOL CLoadImageForAnalysisWnd::GetFlirCameraBaseInfo(ImageFileType eType, CFile* pFile) 
{
	switch (eType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
		{
			// Get camera status
			VARIANT va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_STATUS);
			int nStatus = va.iVal;
			VariantClear(&va);
			if (nStatus == CAM_DISCONNECTED)
			{
				AfxMessageBox(_T("No camera connected"));
				return FALSE;
			}

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_NAME);
			m_IrFileBaseInfo.sCameraType.Format(_T("%s"), CString(va.bstrVal));

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_SN);
			m_IrFileBaseInfo.sCameraSN.Format(_T("%d"), (int)(va.lVal));

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_LENS_NAME);
			m_IrFileBaseInfo.sLens.Format(_T("%s"), CString(va.bstrVal));

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_IMAGE_TIME);
			COleDateTime t = COleDateTime(va.date);
			m_IrFileBaseInfo.sFileDatetime = t.Format();

			VariantClear(&va);
		}
		break;
	case IMAGE_FILE_TYPE_FLIR_FPF:
		{
			if(NULL == pFile)
				return FALSE;

			FPF_CAMDATA_T camData;
			FPF_DATETIME_T datetime;
			DWORD nRead;

			m_IrFileBaseInfo.nFileSize = (int)(pFile->GetLength() / 10240.f) * 10;

			// Read camera header from file
			pFile->Seek(sizeof(FPF_IMAGE_DATA_T), CFile::begin);
			nRead = pFile->Read(&camData, sizeof(FPF_CAMDATA_T));
			if (nRead != sizeof(FPF_CAMDATA_T))
				return FALSE;
			m_IrFileBaseInfo.sCameraType = CString(camData.camera_name);
			m_IrFileBaseInfo.sCameraSN = CString(camData.camera_sn);
			m_IrFileBaseInfo.sLens = CString(camData.lens_name);

			// Read image time from file
			pFile->Seek(sizeof(FPF_IMAGE_DATA_T)+sizeof(FPF_CAMDATA_T)+sizeof(FPF_OBJECT_PAR_T), CFile::begin);
			nRead = pFile->Read(&datetime, sizeof(FPF_DATETIME_T));
			if (nRead != sizeof(FPF_DATETIME_T))
				return FALSE;
			m_IrFileBaseInfo.sFileDatetime.Format(_T("%d-%02d-%02d %d:%d:%d"),
								datetime.Year, datetime.Month, datetime.Day,
								datetime.Hour, datetime.Minute, datetime.Second);
		}
		break;
	}

	return TRUE;
}

BOOL CLoadImageForAnalysisWnd::GetFlirCameraMeasurementParameters(ImageFileType eType, CFile* pFile)
{
	m_IrMeasurementParameters.Reset();

	switch (eType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
		{
			// Get camera status
			VARIANT va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_STATUS);
			int nStatus = va.iVal;
			VariantClear(&va);
			if (nStatus == CAM_DISCONNECTED)
			{
				AfxMessageBox(_T("No camera connected"));
				return FALSE;
			}

			//-------------------------------------------------------------
// 			CString resource;
// 			COleVariant prop;
// 			//VARIANT va;
// 
// 			// Get area result
// 			//resource = _T(".image.sysimg.basicImgData.objectParams.emissivity");
// 			resource = _T(".image.objpar.emissivity");
// 			prop = COleVariant(resource);
// 			int nRet = m_ctrlFlirCamera.SetCameraProperty(PROP_RESOURCE_PATH, prop);
// 			va = m_ctrlFlirCamera.GetCameraProperty(PROP_RESOURCE_VALUE);
// 			m_IrMeasurementParameters.fEmissivity = va.fltVal;
// 			VariantClear(&va);
// 
// 
			va = m_ctrlFlirCamera.GetCameraProperty(PROP_EMIS);
			m_IrMeasurementParameters.fEmissivity = va.fltVal;
			//----------------------------------------------------------------------------

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_TRFL); // Reflected Temperature (in Kelvin)
			m_IrMeasurementParameters.fReflectedTemp = va.fltVal - 273.15f;

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_TATM); // Atmospheric Temperature (in Kelvin)
			m_IrMeasurementParameters.fAtmosphericTemp = va.fltVal - 273.15f;

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_DOBJ); // Object Distance (in Meter)
			m_IrMeasurementParameters.fDistance = va.fltVal;

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_RHUM); // Relative Humidity
			m_IrMeasurementParameters.fRelativeHumidity = va.fltVal;

// 			va = m_ctrlFlirCamera.GetCameraProperty(PROP_TREF); // Reference temperature
// 			m_IrMeasurementParameters.fEstAtmTransm = va.fltVal;// ?????

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_EOTEMP); // External Optics Temperature (in Kelvin)
			m_IrMeasurementParameters.fExtOptTemp = va.fltVal - 273.15f;

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_EOTAO); // External Optics Transmission
			m_IrMeasurementParameters.fExtOptTransm = va.fltVal;

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_SCLO); // Low scale
			m_IrMeasurementParameters.fScaleLow = va.fltVal - 273.15f;

			va = m_ctrlFlirCamera.GetCameraProperty(PROP_SCHI); // High scale
			m_IrMeasurementParameters.fScaleHigh = va.fltVal - 273.15f;

			VariantClear(&va);
		}
		break;
	case IMAGE_FILE_TYPE_FLIR_FPF:
		{
			if(NULL == pFile)
				return FALSE;

			FPF_OBJECT_PAR_T objData;
			FPF_SCALING_T scaling;
			DWORD nRead;

			// Read object parameters from file
			pFile->Seek(sizeof(FPF_IMAGE_DATA_T)+sizeof(FPF_CAMDATA_T), CFile::begin);
			nRead = pFile->Read(&objData, sizeof(FPF_OBJECT_PAR_T));
			if (nRead != sizeof(FPF_OBJECT_PAR_T))
				return FALSE;
			m_IrMeasurementParameters.fEmissivity = objData.emissivity;
			m_IrMeasurementParameters.fDistance = objData.objectDistance;
			m_IrMeasurementParameters.fAtmosphericTemp = objData.atmTemp - 273.15f;
			m_IrMeasurementParameters.fReflectedTemp = objData.ambTemp - 273.15f;
			m_IrMeasurementParameters.fRelativeHumidity = objData.relHum;
			m_IrMeasurementParameters.fExtOptTemp = objData.extOptTemp - 273.15f;
			m_IrMeasurementParameters.fExtOptTransm = objData.extOptTrans;
			// fpfdlg.m_CompTao = objData.compuTao;
			// fpfdlg.m_EstimTao = objData.estimTao;
			// fpfdlg.m_RefTemp = objData.refTemp;

			// Read image time from file
			pFile->Seek(sizeof(FPF_IMAGE_DATA_T)+sizeof(FPF_CAMDATA_T)+sizeof(FPF_DATETIME_T), CFile::begin);
			nRead = pFile->Read(&scaling, sizeof(FPF_SCALING_T));
			if (nRead != sizeof(FPF_SCALING_T))
				return FALSE;
			m_IrMeasurementParameters.fScaleLow = scaling.tMinScale - 273.15f;
			m_IrMeasurementParameters.fScaleHigh = scaling.tMaxScale - 273.15f;
// 			fpfdlg.m_CalcMin		= scaling.tMinCalc - 273.15f;
// 			fpfdlg.m_CalcMax		= scaling.tMaxCalc - 273.15f;
// 			fpfdlg.m_CamScaleMin	= scaling.tMinCam - 273.15f;
// 			fpfdlg.m_CamScaleMax	= scaling.tMaxCam - 273.15f;
		}
		break;
	}

	return TRUE;
}

// 利用预置调色表生成并更新图谱调色板
void CLoadImageForAnalysisWnd::MakePalette(void *ptr, BOOL bReversed)
{
	RGBQUAD *pPal = (RGBQUAD*)ptr;

	if (m_nCurPaletteIndex>=0 && m_nCurPaletteIndex<PALLETTE_COLOR_SCALE_COUNT)
	{
		int nFrom, nTo;
		BOOL bAdd = bReversed ? FALSE : TRUE;
		if(bAdd)
		{
			nFrom = 0;		nTo = 255;
		}
		else
		{
			nFrom = 255;	nTo = 0;

		}

		for(int i=nFrom; bAdd?i<=nTo:i>=nTo; bAdd?i++:i--)
		{
			pPal->rgbBlue = PaletteColorsTable[m_nCurPaletteIndex][i][2];
			pPal->rgbGreen = PaletteColorsTable[m_nCurPaletteIndex][i][1]; 
			pPal->rgbRed = PaletteColorsTable[m_nCurPaletteIndex][i][0];
			pPal->rgbReserved = 0;
			pPal++;
		}
	}
}

// 更改当前使用的调色板序号，并显示调色板
void CLoadImageForAnalysisWnd::SetPalette(int iPaletteIndex, BOOL bReversed)
{
	m_nCurPaletteIndex = iPaletteIndex;
	//m_ctrlPalette.DrawPalette(m_nCurPaletteIndex);
	m_ctrlPalette.SetPalette(iPaletteIndex, bReversed);

	if (NULL == m_pIRImage)
		return;

	if(m_pIRImage->GetNumColors() <=0)
	{
// 		m_pIRImage->GrayScale();			// 24->8:丢掉了部分细节
// 		RGBQUAD pal[256];
// 		RGBQUAD* pPal = pal;
// 		MakePalette((void *)pPal, bReversed);
// 		m_pIRImage->SetPalette(pal);

		long bit = 8;
		long method = 1;
		bool errordiffusion = false;
		long colors = 256;
		bool status = false;

		RGBQUAD c = m_pIRImage->GetTransColor();

		RGBQUAD* pPal = NULL;
		if (method==1)
		{
/*			CQuantizer q(colors,(colors>16 ? 7 : 8));
			q.ProcessImage(m_pIRImage->GetDIB());
			pPal=(RGBQUAD*)calloc(256*sizeof(RGBQUAD), 1);
			q.SetColorTable(pPal);
			status = m_pIRImage->DecreaseBpp(bit, errordiffusion, pPal, colors);
*/		}
		else
			status = m_pIRImage->DecreaseBpp(bit, errordiffusion, 0);

		if (!status)
		{
			CString sInfo(m_pIRImage->GetLastError());
			AfxMessageBox(sInfo);
		}

		if (!m_pIRImage->AlphaPaletteIsValid())
			m_pIRImage->AlphaPaletteEnable(0);

		if (m_pIRImage->IsTransparent())
			m_pIRImage->SetTransIndex(m_pIRImage->GetNearestIndex(c));

		MakePalette((void *)pPal, bReversed);

		if (pPal)
			free(pPal);
	}
	else
	{
		RGBQUAD pal[256];
		RGBQUAD* pPal = pal;
		MakePalette((void *)pPal, bReversed);
		m_pIRImage->SetPalette(pal);
	}

	m_wndImage.Invalidate(TRUE);
}

void CLoadImageForAnalysisWnd::GetTemperatureRange()
{
	if(NULL == m_pIRImage)
		return;

	int nWidth, nHeight;
	nWidth = m_pIRImage->GetWidth();
	nHeight = m_pIRImage->GetHeight();
	CRect rectImage = CRect(0, 0, nWidth, nHeight);
	float fMax, fMin, fAvg;

	if (GetAreaTemperature(rectImage, fMax, fMin, fAvg, TRUE))	// TRUE:基于原始图谱计算，提高速度
	{
		m_fMaxTemperature = fMax;
		m_fMinTemperature = fMin;
		m_ctrlPalette.SetLimits(fMax, fMin);
		//m_ctrlPalette.SetLimits(m_IrMeasurementParameters.fScaleHigh, m_IrMeasurementParameters.fScaleLow);
		m_ctrlPalette.DrawPalette(m_nCurPaletteIndex);
	}
}

// 传入的ptSpot是ClientRect坐标
BOOL CLoadImageForAnalysisWnd::GetSpotTemperature(CPoint ptSpot, float& fTemp)
{
	long nX, nY;

	nX = ptSpot.x;
	nY = ptSpot.y;

	if(NULL == m_pIRImage)
		return FALSE;
	int nRowSize = m_pIRImage->GetWidth();

	if(!GetImageXY(&nX, &nY))
		return FALSE;

	switch (m_nCurMainImageType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
		{
			if (m_hFlirTemperatureImage==NULL || m_hFlirLUT==NULL || nRowSize<=0)
				return FALSE;

			// last image acquired is a raw pixel image
			float* pLUT = (float *)GlobalLock(m_hFlirLUT);

			WORD* pImage = (WORD *)GlobalLock(m_hFlirTemperatureImage);
			WORD rawPix = pImage[nY*nRowSize + nX];
			GlobalUnlock(m_hFlirTemperatureImage);

			fTemp = pLUT[rawPix] - 273.15f;

			GlobalUnlock(m_hFlirLUT);
		}
		break;

	case IMAGE_FILE_TYPE_FLIR_FPF:
		{
			if (m_hFlirTemperatureImage==NULL || m_hFlirLUT==NULL || nRowSize<=0)
				return FALSE;

			// last image acquired is a Kelvin temperature image
			float* pImage = (float *)GlobalLock(m_hFlirTemperatureImage);
			fTemp = pImage[nY*nRowSize + nX] - 273.15f;
			GlobalUnlock(m_hFlirTemperatureImage);
		}
		break;
	}

	return TRUE;
}

BOOL CLoadImageForAnalysisWnd::GetLineTemperature(CPoint ptFrom, CPoint ptTo, float& fMaxT, float& fMinT, float& fAvgT)
{
	if(ptFrom == ptTo)
		return FALSE;

	if(NULL == m_pIRImage)
		return FALSE;
	int nRowSize = m_pIRImage->GetWidth();

	long nX, nY, nCnt, i, j;
	float fMax, fMin, fTotal, fValue;
	WORD rawPix;

	fMaxT=fMinT=fAvgT = 0.f;
	fMax=fMin=fTotal = 0.f;
	nCnt = 0;

	switch (m_nCurMainImageType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
		{
			if (m_hFlirTemperatureImage==NULL || m_hFlirLUT==NULL || nRowSize<=0)
				return FALSE;

			// last image acquired is a raw pixel image
			float* pLUT = (float *)GlobalLock(m_hFlirLUT);
			WORD* pImage = (WORD *)GlobalLock(m_hFlirTemperatureImage);

			fMax = -999999.f;
			fMin = 999999.f;
			BOOL bXAdd = ptFrom.x<=ptTo.x;
			BOOL bYAdd = ptFrom.y<=ptTo.y;
			for (i=ptFrom.x; bXAdd?i<=ptTo.x:i>=ptTo.x; bXAdd?i++:i--)
			{
				for (j=ptFrom.y; bYAdd?j<=ptTo.y:j>=ptTo.y; bYAdd?j++:j--)
				{
					if(ptFrom.x==ptTo.x || ptFrom.y==ptTo.y)	// 铅垂线或水平线
					{
						nX = i;
						nY = j;
					}
					else										// 一般直线: y=ax+b
					{
						float a = 1.0f * (ptFrom.y - ptTo.y) / (ptFrom.x - ptTo.x);
						float b = ptFrom.y - a * ptFrom.x;
						nX = i;
						nY = (long)(a*i + b);
					}

					if (GetImageXY(&nX, &nY))					// 只计算合法点
					{
						rawPix = pImage[nY*nRowSize + nX];
						fValue = pLUT[rawPix] - 273.15f;
						fMax = max(fMax, fValue);
						fMin = min(fMin, fValue);
						fTotal += fValue;
						nCnt++;
					}
				}
			}

			GlobalUnlock(m_hFlirTemperatureImage);
			GlobalUnlock(m_hFlirLUT);

			if(nCnt > 0)	// 避免除零错误
			{
				fMaxT = fMax;
				fMinT = fMin;
				fAvgT = fTotal / nCnt;
			}
		}
		break;

	case IMAGE_FILE_TYPE_FLIR_FPF:
		{
			if (m_hFlirTemperatureImage==NULL || m_hFlirLUT==NULL || nRowSize<=0)
				return FALSE;

			// last image acquired is a Kelvin temperature image
			float* pImage = (float *)GlobalLock(m_hFlirTemperatureImage);

			fMax = -999999.f;
			fMin = 999999.f;
			for (i=ptFrom.x; i<=ptTo.x; i++)
			{
				for (j=ptFrom.y; j<=ptTo.y; j++)
				{
					if(ptFrom.x==ptTo.x || ptFrom.y==ptTo.y)	// 铅垂线或水平线
					{
						nX = i;
						nY = j;
					}
					else										// 一般直线: y=ax+b
					{
						float a = 1.0f * (ptFrom.y - ptTo.y) / (ptFrom.x - ptTo.x);
						float b = ptFrom.y - a * ptFrom.x;
						nX = i;
						nY = (long)(a*i + b);
					}

					if (GetImageXY(&nX, &nY))					// 只计算合法点
					{
						fValue = pImage[nY*nRowSize + nX] - 273.15f;
						fMax = max(fMax, fValue);
						fMin = min(fMin, fValue);
						fTotal += fValue;
						nCnt++;
					}
				}
			}

			GlobalUnlock(m_hFlirTemperatureImage);

			if(nCnt > 0)	// 避免除零错误
			{
				fMaxT = fMax;
				fMinT = fMin;
				fAvgT = fTotal / nCnt;
			}
		}
		break;
	}

	return TRUE;
}

// bRawImage用于计算整幅图谱这类“大面积”的区温，
// bRawImage=FALSE:rectBox为缩放之后的ClientRect坐标(默认)
//          =TRUE:rectBox基于原始图谱大小(调用处保证rectBox的合法性)
BOOL CLoadImageForAnalysisWnd::GetAreaTemperature(CRect rectBox, float& fMaxT, float& fMinT, float& fAvgT, BOOL bRawImage)
{
	if(rectBox.IsRectEmpty())
		return FALSE;

	if(NULL == m_pIRImage)
		return FALSE;
	int nRowSize = m_pIRImage->GetWidth();

	long nX, nY, nCnt, nWidth, nHeight, i, j;
	float fMax, fMin, fTotal, fValue;
	WORD rawPix;
	BOOL bRet;

	fMaxT=fMinT=fAvgT = 0.f;
	fMax=fMin=fTotal = 0.f;
	nCnt = 0;
	rectBox.NormalizeRect();
	nWidth = rectBox.Width();
	nHeight = rectBox.Height();

	switch (m_nCurMainImageType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
		{
			if (m_hFlirTemperatureImage==NULL || m_hFlirLUT==NULL || nRowSize<=0)
				return FALSE;

			// last image acquired is a raw pixel image
			float* pLUT = (float *)GlobalLock(m_hFlirLUT);
			WORD* pImage = (WORD *)GlobalLock(m_hFlirTemperatureImage);

			fMax = -999999.f;
			fMin = 999999.f;
			for (i=0; i<nWidth; i++)
			{
				for (j=0; j<nHeight; j++)
				{
					nX = rectBox.left + i;
					nY = rectBox.top + j;
					if(!bRawImage)
						bRet = GetImageXY(&nX, &nY);			// 只计算合法点
					else
						bRet = TRUE;
					if (bRet)
					{
						rawPix = pImage[nY*nRowSize + nX];
						fValue = pLUT[rawPix] - 273.15f;
						fMax = max(fMax, fValue);
						fMin = min(fMin, fValue);
						fTotal += fValue;
						nCnt++;
					}
				}
			}

			GlobalUnlock(m_hFlirTemperatureImage);
			GlobalUnlock(m_hFlirLUT);

			if(nCnt > 0)	// 避免除零错误
			{
				fMaxT = fMax;
				fMinT = fMin;
				fAvgT = fTotal / nCnt;
			}
		}
		break;

	case IMAGE_FILE_TYPE_FLIR_FPF:
		{
			if (m_hFlirTemperatureImage==NULL || m_hFlirLUT==NULL || nRowSize<=0)
				return FALSE;

			// last image acquired is a Kelvin temperature image
			float* pImage = (float *)GlobalLock(m_hFlirTemperatureImage);

			fMax = -999999.f;
			fMin = 999999.f;
			for (i=0; i<nWidth; i++)
			{
				for (j=0; j<nHeight; j++)
				{
					nX = rectBox.left + i;
					nY = rectBox.top + j;
					if(!bRawImage)
						bRet = GetImageXY(&nX, &nY);			// 只计算合法点
					else
						bRet = TRUE;
					if (bRet)
					{
						fValue = pImage[nY*nRowSize + nX] - 273.15f;
						fMax = max(fMax, fValue);
						fMin = min(fMin, fValue);
						fTotal += fValue;
						nCnt++;
					}
				}
			}

			GlobalUnlock(m_hFlirTemperatureImage);

			if(nCnt > 0)	// 避免除零错误
			{
				fMaxT = fMax;
				fMinT = fMin;
				fAvgT = fTotal / nCnt;
			}
		}
		break;
	}

	return TRUE;
}

BOOL CLoadImageForAnalysisWnd::RotateCameraTemperatureImage(BOOL bClockwise)
{
	if(NULL == m_pIRImage)
		return FALSE;

	int nWidth, nHeight;
	nWidth = m_pIRImage->GetWidth();
	nHeight = m_pIRImage->GetHeight();

	// m_nFlirTemperatureImageSize的实际大小为nWidth*nHeight*nPixelSize
	// nPixelSize与图谱类型相关，其实只需判断m_hFlirTemperatureImage
	if(NULL==m_hFlirTemperatureImage || m_nFlirTemperatureImageSize<nWidth*nHeight)
		return FALSE;

	HANDLE hTem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, m_nFlirTemperatureImageSize);
	if (hTem == NULL)
	{
		AfxMessageBox(_T("图谱内存分配失败!"));
		return FALSE;
	}

	switch (m_nCurMainImageType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
		{
			WORD* pImage = (WORD *)GlobalLock(m_hFlirTemperatureImage);
			WORD* pTemp = (WORD *)GlobalLock(hTem);
			memcpy(pTemp, pImage, m_nFlirTemperatureImageSize);

			if(bClockwise)											// 顺时针旋转
			{
				for(int i=0; i<nHeight; i++)
				{
					for(int j=0; j<nWidth; j++)
					{
						pTemp[j*nHeight + nHeight-1-i] = pImage[i*nWidth + j];
					}
				}
			}
			else													// 逆时针旋转
			{
				for(int i=0; i<nHeight; i++)
				{
					for(int j=0; j<nWidth; j++)
					{
						pTemp[(nWidth-1-j)*nHeight + i] = pImage[i*nWidth + j];
					}
				}
			}

			memcpy(pImage, pTemp, m_nFlirTemperatureImageSize);	// 保存旋转结果
			GlobalUnlock(m_hFlirTemperatureImage);
			GlobalUnlock(hTem);
		}
		break;

	case IMAGE_FILE_TYPE_FLIR_FPF:
		{
			float* pImage = (float *)GlobalLock(m_hFlirTemperatureImage);
			float* pTemp = (float *)GlobalLock(hTem);
			memcpy(pTemp, pImage, m_nFlirTemperatureImageSize);

			if(bClockwise)											// 顺时针旋转
			{
				for(int i=0; i<nHeight; i++)
				{
					for(int j=0; j<nWidth; j++)
					{
						pTemp[j*nHeight + nHeight-1-i] = pImage[i*nWidth + j];
					}
				}
			}
			else													// 逆时针旋转
			{
				for(int i=0; i<nHeight; i++)
				{
					for(int j=0; j<nWidth; j++)
					{
						pTemp[(nWidth-1-j)*nHeight + i] = pImage[i*nWidth + j];
					}
				}
			}

			memcpy(pImage, pTemp, m_nFlirTemperatureImageSize);	// 保存旋转结果
			GlobalUnlock(m_hFlirTemperatureImage);
			GlobalUnlock(hTem);
		}
		break;
	}

	// 释放内存
	GlobalFree(hTem);
	return TRUE;
}

//*****************************************************************************
// Function:	SetMeasurementParameters
// Description: 设置测温参数
// Input:		none
// Output:		pParameters
// Return:		TRUE/FALSE
//*****************************************************************************
BOOL CLoadImageForAnalysisWnd::SetMeasurementParameters(tagIRCameraMeasurementParameters& parameters)
{
	if(NULL == m_pIRImage)
		return FALSE;

	BOOL bRet = FALSE;

	switch (m_nCurMainImageType)
	{
	case IMAGE_FILE_TYPE_FLIR_JPG:
	case IMAGE_FILE_TYPE_FLIR_IMG:
	case IMAGE_FILE_TYPE_FLIR_SEQ:
	case IMAGE_FILE_TYPE_FLIR_FPF:
		bRet = SetFlirCameraMeasurementParameters(parameters);
		if (bRet)
			GetFlirLUT();	// 重新获取LUT
		break;
	}

	if(bRet)
	{
		GetTemperatureRange();
		m_wndImage.SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, NULL);
	}

	return bRet;
}

//*****************************************************************************
// Function:	SetFlirCameraMeasurementParameters
// Description: 设置测温参数
// Input:		none
// Output:		pParameters
// Return:		TRUE/FALSE
//*****************************************************************************
BOOL CLoadImageForAnalysisWnd::SetFlirCameraMeasurementParameters(tagIRCameraMeasurementParameters& parameters)
{
	// Get camera status
	VARIANT va = m_ctrlFlirCamera.GetCameraProperty(PROP_CAMERA_STATUS);
	int nStatus = va.iVal;
	VariantClear(&va);
	if (nStatus == CAM_DISCONNECTED)
	{
		AfxMessageBox(_T("No camera connected"));
		return FALSE;
	}
	VariantClear(&va);

	COleVariant prop = COleVariant((float)(parameters.fEmissivity));
	int nRet = m_ctrlFlirCamera.SetCameraProperty(PROP_EMIS, prop);

	prop = COleVariant((float)(parameters.fReflectedTemp + 273.15f));	// Reflected Temperature (in Kelvin)
	m_ctrlFlirCamera.SetCameraProperty(PROP_TRFL, prop);

	prop = COleVariant((float)(parameters.fAtmosphericTemp + 273.15f)); // Atmospheric Temperature (in Kelvin)
	m_ctrlFlirCamera.SetCameraProperty(PROP_TATM, prop);

	prop = COleVariant((float)(parameters.fDistance));					// Object Distance (in Meter)
	m_ctrlFlirCamera.SetCameraProperty(PROP_DOBJ, prop);

	prop = COleVariant((float)(parameters.fRelativeHumidity));			// Relative Humidity
	m_ctrlFlirCamera.SetCameraProperty(PROP_RHUM, prop);

	// va = m_ctrlFlirCamera.GetCameraProperty(PROP_TREF); // Reference temperature
	// parameters.fEstAtmTransm = va.fltVal;// ?????

	prop = COleVariant((float)(parameters.fExtOptTemp + 273.15f));		// External Optics Temperature (in Kelvin)
	m_ctrlFlirCamera.SetCameraProperty(PROP_EOTEMP, prop);

	prop = COleVariant((float)(parameters.fExtOptTransm));				// External Optics Transmission
	m_ctrlFlirCamera.SetCameraProperty(PROP_EOTAO, prop);

	prop = COleVariant((float)(parameters.fScaleLow + 273.15f));		// Low scale
	m_ctrlFlirCamera.SetCameraProperty(PROP_SCLO, prop);

	prop = COleVariant((float)(parameters.fScaleHigh + 273.15f));		// High scale
	m_ctrlFlirCamera.SetCameraProperty(PROP_SCHI, prop);

	return TRUE;
}
