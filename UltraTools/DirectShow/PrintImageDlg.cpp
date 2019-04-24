// PrintImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TSMO.h"
#include "PrintImageDlg.h"
#include "GlobalDefsForSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CTsmoApp theApp;


// BFish: 解决VS2005编译连接时报jpeg.lib没有实现__iob的错误
extern "C" { 
	FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]};
}

/////////////////////////////////////////////////////////////////////////////
// CPrintImageDlg dialog


CPrintImageDlg::CPrintImageDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPrintImageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintImageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	EnableVisualManagerStyle();

	// BFish: 在dlg被调用处应根据实际值再初始化以下变量
	m_nVideoIndex = 0;
	m_nChannelIndex = 0;
	m_sDateTime = _T("");
}

void CPrintImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintImageDlg)
	DDX_Control(pDX, IDC_PREVIEW_FRAME, m_PreviewWnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintImageDlg, CBCGPDialog)
	//{{AFX_MSG_MAP(CPrintImageDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnPreviewPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintImageDlg message handlers

BOOL CPrintImageDlg::OnInitDialog() 
{
	CBCGPDialog::OnInitDialog();

	m_nWidth = 320;
	m_nHeight = 240;

	if(m_SrcImage.GetWidth() > 600)
	{
		m_nWidth = 720;
		m_nHeight = g_nVideoFormat == NTSC ? 480 : 576;
	}
	else
	{
		m_nWidth = 320;
		m_nHeight = g_nVideoFormat == NTSC ? 240 : 288;
	}

	m_SrcImage.Resample(m_nWidth, m_nHeight, 1, &m_DstImage);
	m_pRGB = m_DstImage.GetBits();

	memset(&m_szCamera, 0x00, sizeof(m_szCamera));
	memset(&m_szTime, 0x00, sizeof(m_szTime));

	sprintf_s(m_szCamera, " : Cam %02d", m_nChannelIndex+1);
	sprintf_s(m_szTime,   " : %s",   (LPSTR)(LPCSTR)m_sDateTime);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintImageDlg::OnPaint() 
{
	BeginWaitCursor();

	CPaintDC dc(this); // device context for painting
	CDC* pDC = &dc;

	BITMAPINFO bmiTarget;
	memset(&bmiTarget, 0x00, sizeof(BITMAPINFO));

	bmiTarget.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmiTarget.bmiHeader.biWidth			= m_nWidth;
	bmiTarget.bmiHeader.biHeight		= m_nHeight;
	bmiTarget.bmiHeader.biPlanes		= 1;
	bmiTarget.bmiHeader.biBitCount		= 24;
	bmiTarget.bmiHeader.biCompression	= BI_RGB;
	bmiTarget.bmiHeader.biSizeImage		= m_nWidth * m_nHeight * 3;
	
	CPrintDialog dlgPrt(FALSE);

	HDC hdcPrint;
	CDC dcPrint;
	int nLogPixelsX;
	int nLogPixelsY;
	int nHorzRes;
	int nVertRes;

	if(!dlgPrt.GetDefaults())
	{
		nLogPixelsX	= 600;
		nLogPixelsY	= 600;
		nHorzRes	= 4600;
		nVertRes	= 6700;
	}
	else
	{
		hdcPrint = dlgPrt.GetPrinterDC();
		dcPrint.Attach(hdcPrint);
		nLogPixelsX	= dcPrint.GetDeviceCaps(LOGPIXELSX); 
		nLogPixelsY	= dcPrint.GetDeviceCaps(LOGPIXELSY); 
		nHorzRes	= dcPrint.GetDeviceCaps(HORZRES); 
		nVertRes	= dcPrint.GetDeviceCaps(VERTRES); 
	}

	int nScaleX = (nHorzRes - nLogPixelsX * 2) / 4;
	int nScaleY = (nVertRes - nLogPixelsY * 2) / 3;
	int nPrintWidth, nPrintHeight, nPrintLeft, nPrintTop;

	nPrintWidth		= (nHorzRes - nLogPixelsX);
	nPrintHeight	= (nVertRes - nLogPixelsY);
	
	nPrintLeft		= (nHorzRes - nPrintWidth)  / 2;
	nPrintTop		= (nVertRes - nPrintHeight) / 2;

	RECT rcClient;
	m_PreviewWnd.GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	int nWidth	= rcClient.right  - rcClient.left;
	int nHeight	= rcClient.bottom - rcClient.top;
	
	int nPaperWidth, nPaperHeight;

	if(nHorzRes < nVertRes)
	{
		nPaperHeight	= nHeight - 30;
		nPaperWidth		= (int)(((float)nHorzRes / (float)nVertRes) * (float)nPaperHeight);
	}
	else
	{
		nPaperWidth		= nWidth - 30;
		nPaperHeight	= (int)(((float)nVertRes / (float)nHorzRes) * (float)nPaperWidth);
	}

	// background
	pDC->SetROP2(R2_COPYPEN);
	pDC->SelectObject(GetStockObject(GRAY_BRUSH));
	pDC->Rectangle(&rcClient);

	// A4 Paper
	RECT rcPaper;
	rcPaper.top		= ((nHeight - nPaperHeight) / 2) + 2 + rcClient.top;
	rcPaper.left	= ((nWidth  - nPaperWidth)  / 2) + 2 + rcClient.left;
	rcPaper.bottom	= (rcPaper.top  + nPaperHeight) + 2;
	rcPaper.right	= (rcPaper.left + nPaperWidth)  + 2;
	pDC->SetROP2(R2_BLACK);
	pDC->Rectangle(&rcPaper);
	rcPaper.top		= ((nHeight - nPaperHeight) / 2) + rcClient.top;
	rcPaper.left	= ((nWidth  - nPaperWidth)  / 2) + rcClient.left;
	rcPaper.bottom	= rcPaper.top  + nPaperHeight;
	rcPaper.right	= rcPaper.left + nPaperWidth;
	pDC->SetROP2(R2_WHITE);
	pDC->Rectangle(&rcPaper);
	
	float nRatio = (float)nHorzRes / (float)nPaperWidth;
	nPrintTop	= (int)((float)nPrintTop    / nRatio);
	nPrintLeft	= (int)((float)nPrintLeft   / nRatio);
	nPrintWidth	= (int)((float)nPrintWidth  / nRatio);
	nPrintHeight= (int)((float)nPrintHeight / nRatio);
	nLogPixelsY = (int)((float)nLogPixelsY  / nRatio);
	
	pDC->SetMapMode(MM_ANISOTROPIC);

	nPrintTop	+= rcPaper.top;
	nPrintLeft	+= rcPaper.left;
	
	int nGridX = nPrintWidth  / 17;
	int nGridY = nPrintHeight / 26;

	CFont Font, *pFontOld;
	Font.CreateFont(nGridY, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "宋体");
	pFontOld = pDC->SelectObject(&Font);
	
	// title
	pDC->TextOut(nPrintLeft + (1 * nGridX), nPrintTop + (2 * nGridX), _T("数字视频截图"));
	pDC->SelectObject(pFontOld);
	Font.DeleteObject();

	// underline
	pDC->SetROP2(R2_BLACK);
	pDC->MoveTo(nPrintLeft + (1  * nGridX), nPrintTop + (3 * nGridY));
	pDC->LineTo(nPrintLeft + (16 * nGridX), nPrintTop + (3 * nGridY));

	// bitmap
	SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);
	StretchDIBits(
			pDC->GetSafeHdc(),
			nPrintLeft + (2 * nGridX), nPrintTop + (5 * nGridY), nGridX * 13, nGridY * 9,
			0, 0, m_nWidth, m_nHeight,
			m_pRGB,
			&bmiTarget,
			DIB_RGB_COLORS,
			SRCCOPY);

	// information	
	CFont Font1, *pFontOld1;
	Font1.CreateFont(nGridY / 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Lucida Console");
	pFontOld1 = pDC->SelectObject(&Font1);


	pDC->TextOut(nPrintLeft + (2 * nGridX), nPrintTop + (17 * nGridY), _T("【安徽国光电力】合肥植物园110kv变电站红外热成像在线监测系统"));

	pDC->TextOut(nPrintLeft + (4 * nGridX), nPrintTop + (19 * nGridY), _T("Camera"));
	pDC->TextOut(nPrintLeft + (7 * nGridX), nPrintTop + (19 * nGridY), m_szCamera);
	

	pDC->TextOut(nPrintLeft + (4 * nGridX), nPrintTop + (21 * nGridY), _T("Date&Time"));
	pDC->TextOut(nPrintLeft + (7 * nGridX), nPrintTop + (21 * nGridY), m_szTime);
	
	pDC->SelectObject(pFontOld1);
	Font1.DeleteObject(); 

	EndWaitCursor();
	// Do not call CBCGPDialog::OnPaint() for painting messages
}

BOOL CPrintImageDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
        // 13=Enter Key, 27=ESC Key
		if(pMsg->wParam == 13 || pMsg->wParam == 27) return TRUE;
    }
	
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CPrintImageDlg::OnPreviewPrint() 
{
	BeginWaitCursor();

	BITMAPINFO bmiTarget;
	memset(&bmiTarget, 0x00, sizeof(BITMAPINFO));

	bmiTarget.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmiTarget.bmiHeader.biWidth			= m_nWidth;
	bmiTarget.bmiHeader.biHeight		= m_nHeight;
	bmiTarget.bmiHeader.biPlanes		= 1;
	bmiTarget.bmiHeader.biBitCount		= 24;
	bmiTarget.bmiHeader.biCompression	= BI_RGB;
	bmiTarget.bmiHeader.biSizeImage		= m_nWidth * m_nHeight * 3;

	CPrintDialog PrintImageDlg(FALSE);

	if(PrintImageDlg.DoModal() != IDOK) return;

	HDC hdcPrint = PrintImageDlg.GetPrinterDC();
	CDC dcPrint;
	dcPrint.Attach(hdcPrint);
	int nLogPixelsX	= dcPrint.GetDeviceCaps(LOGPIXELSX);
	int nLogPixelsY	= dcPrint.GetDeviceCaps(LOGPIXELSY);
	int nHorzRes	= dcPrint.GetDeviceCaps(HORZRES);
	int nVertRes	= dcPrint.GetDeviceCaps(VERTRES);
	int nScaleX		= (nHorzRes - nLogPixelsX * 2) / 4;
	int nScaleY		= (nVertRes - nLogPixelsY * 2) / 3;
	int nPrintWidth, nPrintHeight, nPrintLeft, nPrintTop;
	
	nPrintWidth		= (nHorzRes - nLogPixelsX);
	nPrintHeight	= (nVertRes - nLogPixelsY);
	nPrintLeft		= (nHorzRes - nPrintWidth)  / 2;
	nPrintTop		= (nVertRes - nPrintHeight) / 2;

	DOCINFO docinfo;
	memset(&docinfo, 0x00, sizeof(DOCINFO));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = _T("Print ...");

	if(dcPrint.StartDoc(&docinfo) >= 0)
	{
		if(dcPrint.StartPage() >= 0)
		{
			int nGridX = nPrintWidth  / 17;
			int nGridY = nPrintHeight / 26;

			// title
			CFont Font, *pFontOld;
			Font.CreateFont(nGridY, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Verdana");
			pFontOld = dcPrint.SelectObject(&Font);
			dcPrint.TextOut(nPrintLeft + (1 * nGridX), nPrintTop + (2 * nGridX), _T("数字视频截图"));
			dcPrint.SelectObject(pFontOld);
			Font.DeleteObject();
			
			// information
			CFont Font1, *pFontOld1;
			Font1.CreateFont(nGridY / 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Lucida Console");
			pFontOld1 = dcPrint.SelectObject(&Font1);
			
			dcPrint.TextOut(nPrintLeft + (1 * nGridX), nPrintTop + (17 * nGridY), _T("【安徽国光电力】合肥植物园110kv变电站红外热成像在线监测系统"));

			dcPrint.TextOut(nPrintLeft + (4 * nGridX), nPrintTop + (19 * nGridY), _T("Camera"));
			dcPrint.TextOut(nPrintLeft + (7 * nGridX), nPrintTop + (19 * nGridY), m_szCamera);
			
			dcPrint.TextOut(nPrintLeft + (4 * nGridX), nPrintTop + (21 * nGridY), _T("Date&Time"));
			dcPrint.TextOut(nPrintLeft + (7 * nGridX), nPrintTop + (21 * nGridY), m_szTime);
			
			dcPrint.SelectObject(pFontOld1);
			Font1.DeleteObject(); 

			// underline
			CPen Pen, *pPenOld;
			Pen.CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
			pPenOld = dcPrint.SelectObject(&Pen);

			dcPrint.MoveTo(nPrintLeft + (1  * nGridX), nPrintTop + (3 * nGridY));
			dcPrint.LineTo(nPrintLeft + (16 * nGridX), nPrintTop + (3 * nGridY));

			dcPrint.SelectObject(pPenOld);

			// bitmap
			StretchDIBits(dcPrint.GetSafeHdc(),
							nPrintLeft + (2 * nGridX), nPrintTop + (5 * nGridY), nGridX * 13, nGridY * 9,
							0, 0, m_nWidth, m_nHeight,
							m_pRGB,
							&bmiTarget,
							DIB_RGB_COLORS,
							SRCCOPY);

			dcPrint.EndPage();
		}
		else
		{
			dcPrint.AbortDoc();
		}

		dcPrint.EndDoc();
	}

	EndWaitCursor();
}
