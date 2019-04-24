#include "stdafx.h"
#include "GlobalDefsForSys.h"
#include "ThermalImage.h"
#include "UI/PaletteColorTable.h"
#include "SDK/FlirFpfImage.h"

#include "ExplorerView.h"
#include "AnalysisView.h"
#include "ReportView.h"

#include "Utility/HvUtility.h"


CReportView* g_pReportView;

//#######################################################################################
//										文件存储
//#######################################################################################

tagDocType docTypes[CMAX_IMAGE_FORMATS] =
{
	{ -1, TRUE, TRUE, _T("支持的文件类型"), _T("*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras;*.mng;*.jng;*.ska;*.nef;*.crw;*.cr2;*.mrw;*.raf;*.erf;*.3fr;*.dcr;*.raw;*.dng;*.pef;*.x3f;*.arw;*.sr2;*.mef;*.orf") },
#if THERMAL_IMAGE_SUPPORT_BMP
	{ THERMAL_IMAGE_FORMAT_BMP, TRUE, TRUE, _T("BMP文件(*.bmp)"), _T("*.bmp") },
#endif
#if THERMAL_IMAGE_SUPPORT_GIF
	{ THERMAL_IMAGE_FORMAT_GIF, TRUE, TRUE, _T("GIF文件(*.gif)"), _T("*.gif") },
#endif
#if THERMAL_IMAGE_SUPPORT_JPG
	{ THERMAL_IMAGE_FORMAT_JPG, TRUE, TRUE, _T("JPG文件(*.jpg;*.jpeg)"), _T("*.jpg;*.jpeg") },
#endif
#if THERMAL_IMAGE_SUPPORT_PNG
	{ THERMAL_IMAGE_FORMAT_PNG, TRUE, TRUE, _T("PNG文件(*.png)"), _T("*.png") },
#endif
#if THERMAL_IMAGE_SUPPORT_MNG
	{ THERMAL_IMAGE_FORMAT_MNG, TRUE, TRUE, _T("MNG文件(*.mng;*.jng;*.png)"), _T("*.mng;*.jng;*.png") },
#endif
#if THERMAL_IMAGE_SUPPORT_ICO
	{ THERMAL_IMAGE_FORMAT_ICO, TRUE, TRUE, _T("图标或光标文件(*.ico;*.cur)"), _T("*.ico;*.cur") },
#endif
#if THERMAL_IMAGE_SUPPORT_TIF
	{ THERMAL_IMAGE_FORMAT_TIF, TRUE, TRUE, _T("TIF文件(*.tif;*.tiff)"), _T("*.tif;*.tiff") },
#endif
#if THERMAL_IMAGE_SUPPORT_TGA
	{ THERMAL_IMAGE_FORMAT_TGA, TRUE, TRUE, _T("TGA文件(*.tga)"), _T("*.tga") },
#endif
#if THERMAL_IMAGE_SUPPORT_PCX
	{ THERMAL_IMAGE_FORMAT_PCX, TRUE, TRUE, _T("PCX文件(*.pcx)"), _T("*.pcx") },
#endif
#if THERMAL_IMAGE_SUPPORT_WBMP
	{ THERMAL_IMAGE_FORMAT_WBMP, TRUE, TRUE, _T("WBMP文件(*.wbmp)"), _T("*.wbmp") },
#endif
#if THERMAL_IMAGE_SUPPORT_WMF
	{ THERMAL_IMAGE_FORMAT_WMF, TRUE, FALSE, _T("WMF EMF文件(*.wmf;*.emf)"), _T("*.wmf;*.emf") },
#endif
#if THERMAL_IMAGE_SUPPORT_JBG
	{ THERMAL_IMAGE_FORMAT_JBG, TRUE, TRUE, _T("JBG文件(*.jbg)"), _T("*.jbg") },
#endif
#if THERMAL_IMAGE_SUPPORT_JP2
	{ THERMAL_IMAGE_FORMAT_JP2, TRUE, TRUE, _T("JP2文件(*.j2k;*.jp2)"), _T("*.j2k;*.jp2") },
#endif
#if THERMAL_IMAGE_SUPPORT_JPC
	{ THERMAL_IMAGE_FORMAT_JPC, TRUE, TRUE, _T("JPC文件(*.j2c;*.jpc)"), _T("*.j2c;*.jpc") },
#endif
#if THERMAL_IMAGE_SUPPORT_PGX
	{ THERMAL_IMAGE_FORMAT_PGX, TRUE, TRUE, _T("PGX文件(*.pgx)"), _T("*.pgx") },
#endif
#if THERMAL_IMAGE_SUPPORT_RAS
	{ THERMAL_IMAGE_FORMAT_RAS, TRUE, TRUE, _T("RAS文件(*.ras)"), _T("*.ras") },
#endif
#if THERMAL_IMAGE_SUPPORT_PNM
	{ THERMAL_IMAGE_FORMAT_PNM, TRUE, TRUE, _T("PNM文件(*.pnm;*.pgm;*.ppm)"), _T("*.pnm;*.pgm;*.ppm") },
#endif
#if THERMAL_IMAGE_SUPPORT_SKA
	{ THERMAL_IMAGE_FORMAT_SKA, TRUE, TRUE, _T("SKA文件(*.ska)"), _T("*.ska") },
#endif
#if THERMAL_IMAGE_SUPPORT_RAW
	{ THERMAL_IMAGE_FORMAT_RAW, TRUE, FALSE, _T("RAW文件(*.nef;*.crw;*.cr2;...)"), _T("*.nef;*.crw;*.cr2;*.mrw;*.raf;*.erf;*.3fr;*.dcr;*.raw;*.dng;*.pef;*.x3f;*.arw;*.sr2;*.mef;*.orf") }
#endif
};

//////////////////////////////////////////////////////////////////////////////
// prompt for file name - used for open and save as
BOOL PromptForFileName(CString& sFileName, CString& sDefaultPath, CString sTitle,
								 DWORD dwFlags, BOOL bOpenFileDialog, int* pType)
{
	CFileDialog dlgFile(bOpenFileDialog);

	dlgFile.m_ofn.Flags |= dwFlags;

	int nDocType = (pType != NULL) ? *pType : THERMAL_IMAGE_FORMAT_JPG;
	if (nDocType == 0)
		nDocType = 1;

	int nIndex = GetIndexFromType(nDocType, bOpenFileDialog);
	if (nIndex == -1)
		nIndex = 0;

	dlgFile.m_ofn.nFilterIndex = nIndex +1;
	// sDefExt is necessary to hold onto the memory from GetExtFromType
	CString sDefExt = GetExtFromType(nDocType).Mid(2,3);
	dlgFile.m_ofn.lpstrDefExt = sDefExt;

	CString sFilter = GetFileTypes(bOpenFileDialog);
	dlgFile.m_ofn.lpstrFilter = sFilter;
	dlgFile.m_ofn.lpstrTitle = sTitle;
	dlgFile.m_ofn.lpstrInitialDir = sDefaultPath.GetLength()>0? sDefaultPath : NULL;
	dlgFile.m_ofn.lpstrFile = sFileName.GetBuffer(_MAX_PATH);	// 回传文件名

	BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;
	sFileName.ReleaseBuffer();
	if (bRet)
	{
		if (pType != NULL)
		{
			int nIndex = (int)dlgFile.m_ofn.nFilterIndex - 1;
			ASSERT(nIndex >= 0);
			*pType = GetTypeFromIndex(nIndex, bOpenFileDialog);
		}

		sDefaultPath = dlgFile.GetPathName();					// 回传路径
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////////
int GetIndexFromType(int nDocType, BOOL bOpenFileDialog)
{
	int nCnt = 0;
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++)
	{
		if (bOpenFileDialog ? docTypes[i].bRead : docTypes[i].bWrite)
		{
			if (docTypes[i].nID == nDocType)
				return nCnt;
			nCnt++;
		}
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////////
int GetTypeFromIndex(int nIndex, BOOL bOpenFileDialog)
{
	int nCnt = 0;
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++)
	{
		if (bOpenFileDialog ? docTypes[i].bRead : docTypes[i].bWrite)
		{
			if (nCnt == nIndex)
				return docTypes[i].nID;
			nCnt++;
		}
	}
	ASSERT(FALSE);
	return -1;
}
//////////////////////////////////////////////////////////////////////////////
CString GetExtFromType(int nDocType)
{
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++)
	{
		if (docTypes[i].nID == nDocType)
			return docTypes[i].ext;
	}
	return CString(_T(""));
}
//////////////////////////////////////////////////////////////////////////////
CString GetDescFromType(int nDocType)
{
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++)
	{
		if (docTypes[i].nID == nDocType)
			return docTypes[i].description;
	}
	return CString(_T(""));
}
//////////////////////////////////////////////////////////////////////////////
BOOL GetWritableType(int nDocType)
{
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++)
	{
		if (docTypes[i].nID == nDocType)
			return docTypes[i].bWrite;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
CString GetFileTypes(BOOL bOpenFileDialog)
{
	CString str = _T("");
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++)
	{
		if (bOpenFileDialog && docTypes[i].bRead)
		{
			str += docTypes[i].description;
			str += (TCHAR)NULL;
			str += docTypes[i].ext;
			str += (TCHAR)NULL;
		}
		else if (!bOpenFileDialog && docTypes[i].bWrite)
		{
			str += docTypes[i].description;
			str += (TCHAR)NULL;
			str += docTypes[i].ext;
			str += (TCHAR)NULL;
		}
	}
	return str;
}

//#############################################################################
// 利用预置调色表生成调色板，以便更新图谱
void MakePalette(void *ptr)
{
	RGBQUAD *palP = (RGBQUAD*)ptr;
	int nCurPaletteIndex = 2;

	if (nCurPaletteIndex>=0 && nCurPaletteIndex<PALLETTE_COLOR_SCALE_COUNT)
	{
		for(WORD i = 0; i < 256; i++)
		{
			palP->rgbBlue = PaletteColorsTable[nCurPaletteIndex][i][2];
			palP->rgbGreen = PaletteColorsTable[nCurPaletteIndex][i][1]; 
			palP->rgbRed = PaletteColorsTable[nCurPaletteIndex][i][0];
			palP->rgbReserved = 0;
			palP++;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// 获取文件类型(基于CxImage)及扩展名
int GetTypeFromFileName(const CString& sFileName, CString& sExt)
{
	int nType, nPos;

	sExt = _T("");
	if(sFileName.GetLength() < 4)
		return THERMAL_IMAGE_FORMAT_UNKNOWN;

	// 获取文件扩展名
	nPos = sFileName.ReverseFind('.');
	if (nPos <= 0)
		return THERMAL_IMAGE_FORMAT_UNKNOWN;
	sExt = sFileName.Mid(nPos+1);
	sExt.MakeLower();

	if (sExt == _T("bmp"))					nType = THERMAL_IMAGE_FORMAT_BMP;
#if THERMAL_IMAGE_SUPPORT_JPG
	else if (sExt == _T("jpg") || sExt == _T("jpeg"))	nType = THERMAL_IMAGE_FORMAT_JPG;
#endif
#if THERMAL_IMAGE_SUPPORT_GIF
	else if (sExt == _T("gif"))				nType = THERMAL_IMAGE_FORMAT_GIF;
#endif
#if THERMAL_IMAGE_SUPPORT_PNG
	else if (sExt == _T("png"))				nType = THERMAL_IMAGE_FORMAT_PNG;
#endif
#if THERMAL_IMAGE_SUPPORT_MNG
	else if (sExt == _T("mng") || sExt == _T("jng"))	nType = THERMAL_IMAGE_FORMAT_MNG;
#endif
#if THERMAL_IMAGE_SUPPORT_ICO
	else if (sExt == _T("ico"))				nType = THERMAL_IMAGE_FORMAT_ICO;
#endif
#if THERMAL_IMAGE_SUPPORT_TIF
	else if (sExt == _T("tiff") || sExt == _T("tif"))	nType = THERMAL_IMAGE_FORMAT_TIF;
#endif
#if THERMAL_IMAGE_SUPPORT_TGA
	else if (sExt == _T("tga"))				nType = THERMAL_IMAGE_FORMAT_TGA;
#endif
#if THERMAL_IMAGE_SUPPORT_PCX
	else if (sExt == _T("pcx"))				nType = THERMAL_IMAGE_FORMAT_PCX;
#endif
#if THERMAL_IMAGE_SUPPORT_WBMP
	else if (sExt == _T("wbmp"))				nType = THERMAL_IMAGE_FORMAT_WBMP;
#endif
#if THERMAL_IMAGE_SUPPORT_WMF
	else if (sExt == _T("wmf") || sExt == _T("emf"))	nType = THERMAL_IMAGE_FORMAT_WMF;
#endif
#if THERMAL_IMAGE_SUPPORT_J2K
	else if (sExt==_T("j2k")||sExt==_T("jp2"))	nType = THERMAL_IMAGE_FORMAT_J2K;
#endif
#if THERMAL_IMAGE_SUPPORT_JBG
	else if (sExt==_T("jbg"))				nType = THERMAL_IMAGE_FORMAT_JBG;
#endif
#if THERMAL_IMAGE_SUPPORT_JP2
	else if (sExt == _T("jp2") || sExt == _T("j2k"))	nType = THERMAL_IMAGE_FORMAT_JP2;
#endif
#if THERMAL_IMAGE_SUPPORT_JPC
	else if (sExt == _T("jpc") || sExt == _T("j2c"))	nType = THERMAL_IMAGE_FORMAT_JPC;
#endif
#if THERMAL_IMAGE_SUPPORT_PGX
	else if (sExt == _T("pgx"))				nType = THERMAL_IMAGE_FORMAT_PGX;
#endif
#if THERMAL_IMAGE_SUPPORT_RAS
	else if (sExt == _T("ras"))				nType = THERMAL_IMAGE_FORMAT_RAS;
#endif
#if THERMAL_IMAGE_SUPPORT_PNM
	else if (sExt == _T("pnm") || sExt == _T("pgm") || sExt == _T("ppm")) nType = THERMAL_IMAGE_FORMAT_PNM;
#endif
	else nType = THERMAL_IMAGE_FORMAT_UNKNOWN;

	return nType;
}

///////////////////////////////////////////////////////////////////////////////
// 加载并生成FLIR FPF类型文件的缩略图
BOOL MakeFpfFileThumbnail(CString sFileName, CThermalImage* pImage)
{
	ASSERT(pImage);

	FPF_IMAGE_DATA_T imgData;
	COleSafeArray sa;
	float* pDstFloat;
	DWORD nRead, nImgSize, nElements[2];
	CFile fpfFile;
	if (!fpfFile.Open(sFileName, CFile::modeRead))
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

	nElements[0] = imgData.xSize;
	nElements[1] = imgData.ySize;
	sa.Create(VT_R4, 2, nElements);
	sa.AccessData((LPVOID*)&pDstFloat);
	fpfFile.Seek(sizeof(FPFHEADER_T), CFile::begin);
	nRead = fpfFile.Read(pDstFloat, nImgSize);
	if (nRead != nImgSize)
	{
		sa.UnaccessData();
		fpfFile.Close();
		return FALSE;
	}
	sa.UnaccessData();

	//-----------------------------------------------------------------
//	bRet = ConvertFlirImage(3, sa);
	long w, h;
	sa.GetUBound(1, &w);
	sa.GetUBound(2, &h);
	w++;
	h++;

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
		fpfFile.Close();
		AfxMessageBox(_T("分配内存失败!"));
		return FALSE;
	}

	BYTE* pDIB = (BYTE*)GlobalLock(hDIB);
	BYTE* pBits = pDIB + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	int nImageType = 3;

	// Allocate and copy image to local buffer
	VARTYPE vt = (nImageType < 2 ? VT_I2 : (nImageType < 4 ? VT_R4 : VT_UI1));
	int pixSize = (vt == VT_UI1 ? 1 : (vt == VT_I2 ? 2 : 4));
	imgSize = xAlignSize * h * pixSize;
	HANDLE hCameraTemperatureImage;
	hCameraTemperatureImage = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, imgSize);
	if (hCameraTemperatureImage == NULL)
	{
		fpfFile.Close();
		AfxMessageBox(_T("图谱内存分配失败!"));
		return FALSE;
	}

	BYTE* pTemImage = (BYTE *)GlobalLock(hCameraTemperatureImage);

	if (vt == VT_UI1)
	{
		BYTE HUGEP *pSrc;
		sa.AccessData((void HUGEP**)&pSrc);
		int rowSize = xAlignSize;
		for (int y = 0; y < h; y++)
			memcpy(&pTemImage[y * rowSize], &pSrc[y * w], rowSize);
		sa.UnaccessData();
	}
	else if (vt == VT_I2)
	{
		WORD HUGEP *pSrc;
		sa.AccessData((void HUGEP**)&pSrc);
		int rowSize = xAlignSize * pixSize;
		for (int y = 0; y < h; y++)
			memcpy(&pTemImage[y * rowSize], &pSrc[y * w], rowSize);
		sa.UnaccessData();
	}
	else if (vt == VT_R4)
	{
		float HUGEP *pSrc;
		sa.AccessData((void HUGEP**)&pSrc);
		int rowSize = xAlignSize * pixSize;
		for (int y = 0; y < h; y++)
			memcpy(&pTemImage[y * rowSize], &pSrc[y * w], rowSize);
		sa.UnaccessData();
	}

	// Copy pixel data
	BYTE* pDst = pBits;
	int sz = xAlignSize * h;
	if (vt == VT_UI1)
	{
		sz = xAlignSize * h;
		BYTE* pSrc = (BYTE *)pTemImage;
		while (sz--)
		{
			*pDst++ = *pSrc++;
		}
	}
	else if (vt == VT_I2)
	{
		WORD smin = 65535, smax = 0;
		WORD* pSrc = (WORD *)pTemImage;
		while (sz--)
		{
			smin = __min(*pSrc, smin);
			smax = __max(*pSrc, smax);
			pSrc++;
		}

		sz = xAlignSize * h;
		pSrc = (WORD *)pTemImage;
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
		float* pSrc = (float *)pTemImage;
		while (sz--)
		{
			smin = __min(*pSrc, smin);
			smax = __max(*pSrc, smax);
			pSrc++;
		}

		sz = xAlignSize * h;
		pSrc = (float *)pTemImage;
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

	HDC hdc = GetDC(NULL);
	HDC hBufDC;   
	HBITMAP hBitmap, hBitTemp;   

	//创建设备上下文(HDC)   
	hBufDC = CreateCompatibleDC(hdc);   

	//创建HBITMAP   
	hBitmap = CreateCompatibleBitmap(hdc, w, h);   
	hBitTemp = (HBITMAP) SelectObject(hBufDC, hBitmap);   

	//得到位图缓冲区   
	int oldMode = SetStretchBltMode(hBufDC, COLORONCOLOR);
	int ret = ::StretchDIBits(hBufDC,
		0, 0, w, h,
		0, 0, w, h, pBits, (LPBITMAPINFO)pDIB, DIB_RGB_COLORS, SRCCOPY);
	SetStretchBltMode(hBufDC, oldMode); // Restore mode

	//得到最终的位图信息   
	hBitmap = (HBITMAP) SelectObject(hBufDC, hBitTemp);
	pImage->CreateFromHBITMAP(hBitmap);

	//释放内存   
	DeleteObject(hBitmap);
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	GlobalUnlock(hCameraTemperatureImage);
	GlobalFree(hCameraTemperatureImage);

	fpfFile.Close();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 编辑/修改图谱线程
void /*unsigned long _stdcall*/ RunImageEditThread(void *lpParam)
{
	CAnalysisView* pView = (CAnalysisView *)lpParam;
	if (pView == NULL)
		return;
	if (pView->m_wndLoadImage.m_pIRImage == NULL)
	{
		pView->m_hImageEditThread = NULL;
		return;
	}

	//	pView->SubmitUndo();

	bool status = true;

	switch (pView->m_MenuCommand)
	{
		// 	case ID_CXIMAGE_THRESHOLD:
		// 		if (theApp.m_Filters.ThreshPreserveColors){
		// 			RGBQUAD c = {255,255,255,0};
		// 			status = pView->m_wndLoadImage.m_pIRImage->Threshold2(theApp.m_Filters.ThreshLevel,true,c,true);
		// 		} else {
		// 			status = pView->m_wndLoadImage.m_pIRImage->Threshold(theApp.m_Filters.ThreshLevel);
		// 		}
		// 		break;
		// 	case ID_COLORS_ADAPTIVETHRESHOLD:
		// 		status = pView->m_wndLoadImage.m_pIRImage->AdaptiveThreshold();
		// 		break;
		// 	case ID_CXIMAGE_COLORIZE:
		// 		switch (theApp.m_Filters.ColorMode)
		// 		{
		// 		case 1:
		// 			status = pView->m_wndLoadImage.m_pIRImage->Colorize(theApp.m_Filters.ColorHSL.rgbRed,theApp.m_Filters.ColorHSL.rgbGreen,theApp.m_Filters.ColorHSL.rgbBlue/100.0f);
		// 			break;
		// 		case 2:
		// 			status = pView->m_wndLoadImage.m_pIRImage->Solarize(theApp.m_Filters.ColorSolarLevel,theApp.m_Filters.ColorSolarLink!=0);
		// 			break;
		// 		default:
		// 			status = pView->m_wndLoadImage.m_pIRImage->ShiftRGB(theApp.m_Filters.ColorRed,theApp.m_Filters.ColorGreen,theApp.m_Filters.ColorBlue);
		// 		}
		// 		break;
		// 	case ID_CXIMAGE_LIGHTEN:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Light(10);
		// 		break;
		// 	case ID_CXIMAGE_DARKEN:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Light(-10);
		// 		break;
		// 	case ID_CXIMAGE_CONTRAST:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Light(0,10);
		// 		break;
		// 	case ID_CXIMAGE_LESSCONTRAST:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Light(0,-10);
		// 		break;
		// 	case ID_COLORS_MORESATURATIONHSL:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Saturate(25,1);
		// 		break;
		// 	case ID_COLORS_MORESATURATIONYUV:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Saturate(25,2);
		// 		break;
		// 	case ID_COLORS_LESSSATURATION:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Saturate(-20,2);
		// 		break;
		// 	case ID_CXIMAGE_DILATE:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Dilate(3);
		// 		break;
		// 	case ID_CXIMAGE_ERODE:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Erode(3);
		// 		break;
		// 	case ID_CXIMAGE_CONTOUR:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Contour();
		// 		break;
		// 	case ID_CXIMAGE_ADDNOISE:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Noise(25);
		// 		break;
		// 	case ID_CXIMAGE_JITTER:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Jitter();
		// 		break;
		// 	case ID_CXIMAGE_TEXTBLUR:
		// 		status = pView->m_wndLoadImage.m_pIRImage->TextBlur(100,2,3,true,true);
		// 		break;
		// 	case ID_CXIMAGE_BLURSELBORDER:
		// 		{
		// 			CxImage iSel1,iSel2;
		// 			pView->m_wndLoadImage.m_pIRImage->SelectionSplit(&iSel1);
		// 			pView->m_wndLoadImage.m_pIRImage->SelectionSplit(&iSel2);
		// 			iSel2.Edge();
		// 			//iSel2.Erode();
		// 			iSel2.Negative();
		// 			pView->m_wndLoadImage.m_pIRImage->SelectionSet(iSel2);
		// 			pView->m_wndLoadImage.m_pIRImage->GaussianBlur();
		// 			pView->m_wndLoadImage.m_pIRImage->SelectionSet(iSel1);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_SELECTIVEBLUR:
		// 		status = pView->m_wndLoadImage.m_pIRImage->SelectiveBlur(1,25);
		// 		break;
		// 	case ID_CXIMAGE_REDEYEREMOVE:
		// 		status = pView->m_wndLoadImage.m_pIRImage->RedEyeRemove();
		// 		break;
		// 	case ID_FILTERS_NONLINEAR_EDGE:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Edge();
		// 		break;
		// 	case ID_CXIMAGE_CIRCLETRANSFORM_CYLINDER:
		// 		status = pView->m_wndLoadImage.m_pIRImage->CircleTransform(3,0,100);
		// 		break;
		// 	case ID_CXIMAGE_CIRCLETRANSFORM_PINCH:
		// 		status = pView->m_wndLoadImage.m_pIRImage->CircleTransform(1,0,100);
		// 		break;
		// 	case ID_CXIMAGE_CIRCLETRANSFORM_PUNCH:
		// 		status = pView->m_wndLoadImage.m_pIRImage->CircleTransform(0,0,100);
		// 		break;
		// 	case ID_CXIMAGE_CIRCLETRANSFORM_SWIRL:
		// 		status = pView->m_wndLoadImage.m_pIRImage->CircleTransform(2,0,100);
		// 		break;
		// 	case ID_CXIMAGE_CIRCLETRANSFORM_BATHROOM:
		// 		status = pView->m_wndLoadImage.m_pIRImage->CircleTransform(4);
		// 		break;
		// 	case ID_CXIMAGE_EMBOSS:
		// 		{
		// 			long kernel[]={0,0,-1,0,0,0,1,0,0};
		// 			status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,3,-1,127);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_BLUR:
		// 		{
		// 			long kernel[]={1,1,1,1,1,1,1,1,1};
		// 			status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,3,9,0);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_GAUSSIAN3X3:
		// 		{
		// 			//long kernel[]={1,2,1,2,4,2,1,2,1};
		// 			//status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,3,16,0);
		// 			status = pView->m_wndLoadImage.m_pIRImage->GaussianBlur(1.0f);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_GAUSSIAN5X5:
		// 		{
		// 			//long kernel[]={0,1,2,1,0,1,3,4,3,1,2,4,8,4,2,1,3,4,3,1,0,1,2,1,0};
		// 			//status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,5,52,0);
		// 			status = pView->m_wndLoadImage.m_pIRImage->GaussianBlur(2.0f);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_SOFTEN:
		// 		{
		// 			long kernel[]={1,1,1,1,8,1,1,1,1};
		// 			status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,3,16,0);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_SHARPEN:
		// 		{
		// 			long kernel[]={-1,-1,-1,-1,15,-1,-1,-1,-1};
		// 			status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,3,7,0);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_EDGE:
		// 		{
		// 			long kernel[]={-1,-1,-1,-1,8,-1,-1,-1,-1};
		// 			status = pView->m_wndLoadImage.m_pIRImage->Filter(kernel,3,-1,255);
		// 			break;
		// 		}
		// 	case ID_FILTERS_LINEAR_CUSTOM: // [Priyank Bolia]
		// 		{
		// 			if(theApp.m_Filters.kSize==3)
		// 				pView->m_wndLoadImage.m_pIRImage->Filter(theApp.m_Filters.Kernel3x3,3,theApp.m_Filters.kDivisor,theApp.m_Filters.kBias);
		// 			else
		// 				pView->m_wndLoadImage.m_pIRImage->Filter(theApp.m_Filters.Kernel5x5,5,theApp.m_Filters.kDivisor,theApp.m_Filters.kBias);
		// 			break;
		// 		}
		// 	case ID_CXIMAGE_MEDIAN:
		// 		status = pView->m_wndLoadImage.m_pIRImage->Median(3);
		// 		break;
		// 	case ID_CXIMAGE_UNSHARPMASK:
		// 		status = pView->m_wndLoadImage.m_pIRImage->UnsharpMask();
		// 		break;
		// 	case ID_CXIMAGE_GAMMA:
		// 		if (theApp.m_Filters.GammaLink){
		// 			status = pView->m_wndLoadImage.m_pIRImage->GammaRGB(theApp.m_Filters.GammaR,theApp.m_Filters.GammaG,theApp.m_Filters.GammaB);
		// 		} else {
		// 			status = pView->m_wndLoadImage.m_pIRImage->Gamma(theApp.m_Filters.GammaLevel);
		// 		}
		// 		break;
	case IDM_ANALYSIS_ROTATE_ANTICLOCKWISE:		// 逆时针旋转
		pView->m_wndLoadImage.RotateCameraTemperatureImage(FALSE);
		status = pView->m_wndLoadImage.m_pIRImage->RotateLeft();
		break;

	case IDM_ANALYSIS_ROTATE_CLOCKWISE:
		pView->m_wndLoadImage.RotateCameraTemperatureImage(TRUE);
		status = pView->m_wndLoadImage.m_pIRImage->RotateRight();
		break;

	}

	//	pView->m_wndLoadImage.m_pIRImage->SetProgress(100);

	if (!status)
	{
		CString s(pView->m_wndLoadImage.m_pIRImage->GetLastError());
		AfxMessageBox(s);
	}

	pView->m_hImageEditThread = NULL;

	pView->SendMessage(WM_UPDATE_IMAGE, 0, 0);
	_endthread();

	return;
}
