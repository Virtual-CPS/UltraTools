// SPLASHER.CPP

#include "stdafx.h"
#include "resource.h"
#include "Splasher.h"

#include "DrawWireRects.h"

//////////////////// Defines ////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MOUSE_MOVE 0xF012

BEGIN_MESSAGE_MAP(CSplashThread, CWinThread)
	//{{AFX_MSG_MAP(CSplashThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSplashThread, CWinThread)

CSplashThread::CSplashThread()
{
}

CSplashThread::~CSplashThread()
{
}

BOOL CSplashThread::InitInstance()
{
	BOOL bCreated = m_SplashScreen.Create();
	VERIFY(bCreated);
	m_pMainWnd = &m_SplashScreen;
	return bCreated;
}

void CSplashThread::HideSplash()
{
	//Wait until the splash screen has been created
	//before trying to close it
	while (!m_SplashScreen.GetSafeHwnd());

	m_SplashScreen.SetOKToClose();
	m_SplashScreen.SendMessage(WM_CLOSE);
}

void CSplashThread::SetBitmapToUse(const CString& sFilename)
{
	m_SplashScreen.SetBitmapToUse(sFilename);
}

void CSplashThread::SetBitmapToUse(UINT nResourceID)
{
	m_SplashScreen.SetBitmapToUse(nResourceID);
}

void CSplashThread::SetBitmapToUse(LPCTSTR pszResourceName)
{
	m_SplashScreen.SetBitmapToUse(pszResourceName);
}



BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSplashWnd::CSplashWnd()
{
	m_bOKToClose = FALSE;
	m_nHeight = 0;
	m_nWidth = 0;
}

CSplashWnd::~CSplashWnd()
{
	//destroy our invisible owner when we're done
	if (m_wndOwner.m_hWnd != NULL)
		m_wndOwner.DestroyWindow();
}

BOOL CSplashWnd::LoadBitmap()
{     
	//Use LoadImage to get the image loaded into a DIBSection
	HBITMAP hBitmap;
	if (m_bUseFile)
		hBitmap = (HBITMAP) ::LoadImage(NULL, m_sFilename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	else
		hBitmap = (HBITMAP) ::LoadImage(AfxGetResourceHandle(), m_pszResourceName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

	//Check that we could load it up  
	if (hBitmap == NULL)       
		return FALSE;

	//Get the color depth of the DIBSection
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	m_nHeight = bm.bmHeight;
	m_nWidth = bm.bmWidth;

	//Covert from the SDK bitmap handle to the MFC equivalent
	m_Bitmap.Attach(hBitmap);

	//If the DIBSection is 256 color or less, it has a color table
	if ((bm.bmBitsPixel * bm.bmPlanes) <= 8 )     
	{
		//Create a memory DC and select the DIBSection into it
		CDC memDC;
		memDC.CreateCompatibleDC(NULL);
		CBitmap* pOldBitmap = memDC.SelectObject(&m_Bitmap);

		//Get the DIBSection's color table
		RGBQUAD rgb[256];
		::GetDIBColorTable(memDC.m_hDC, 0, 256, rgb);

		//Create a palette from the color table
		LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE) + (256*sizeof(PALETTEENTRY))];
		pLogPal->palVersion = 0x300;       
		pLogPal->palNumEntries = 256;

		for (WORD i=0; i<256; i++)       
		{
			pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
			pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
			pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
		}
		VERIFY(m_Palette.CreatePalette(pLogPal));
    
		//Clean up
		delete pLogPal;
		memDC.SelectObject(pOldBitmap);
	}
	else  //It has no color table, so use a halftone palette     
	{
		CDC* pRefDC = GetDC();
		m_Palette.CreateHalftonePalette(pRefDC);
		ReleaseDC(pRefDC);     
	}     

	return TRUE;   
}

BOOL CSplashWnd::Create()
{                   
  //Load up the bitmap from file or from resource
  VERIFY(LoadBitmap());

  //Modify the owner window of the splash screen to be an invisible WS_POPUP 
  //window so that the splash screen does not appear in the task bar
  LPCTSTR pszWndClass = AfxRegisterWndClass(0);
  VERIFY(m_wndOwner.CreateEx(0, pszWndClass, _T(""), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0));

  //Create this window
  pszWndClass = AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  int y = (::GetSystemMetrics(SM_CYSCREEN) - m_nHeight) / 2; 
  int x = (::GetSystemMetrics(SM_CXSCREEN) - m_nWidth) / 2;
  VERIFY(CreateEx(0, pszWndClass, _T(""), WS_POPUP | WS_VISIBLE, x, y, m_nWidth, m_nHeight, m_wndOwner.GetSafeHwnd(), NULL));
  
  //realize the bitmap's palette into the DC
  OnQueryNewPalette();

  return TRUE;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
 
	int cy = ::GetSystemMetrics(SM_CYSCREEN) / 2; 
	int cx = ::GetSystemMetrics(SM_CXSCREEN) / 2; 

	CRect rectFrom(cx-5,cy-5,cx+5,cy+5);
	//CRect rectFrom(0,0,10,10);	
	m_rectFrom = rectFrom;

	if (!m_rectFrom.IsRectEmpty())
	{
		CRect rectTo(lpCreateStruct->x,lpCreateStruct->y, 
			lpCreateStruct->x + lpCreateStruct->cx,
			lpCreateStruct->y + lpCreateStruct->cy);

		//if (m_bWireFrame)
			DrawWireRects(m_rectFrom, rectTo, 20);
		//else
			//DrawAnimatedRects(m_hWnd, IDANI_CAPTION, m_rectFrom, rectTo);
	}

	//CenterWindow();

	return 0;
}

void CSplashWnd::OnPaint()
{
  CPaintDC dc(this);

  CDC memDC;
  memDC.CreateCompatibleDC(&dc);
  CBitmap* pOldBitmap = memDC.SelectObject(&m_Bitmap);
  CPalette* pOldPalette = dc.SelectPalette(&m_Palette, FALSE);
  dc.RealizePalette();
  dc.BitBlt(0, 0, m_nWidth, m_nHeight, &memDC, 0, 0, SRCCOPY);
  memDC.SelectObject(pOldBitmap);         
  dc.SelectPalette(pOldPalette, FALSE);
}

void CSplashWnd::OnClose() 
{
	if (m_bOKToClose)
	{
		if (!m_rectFrom.IsRectEmpty())
		{
			CRect rect;
			GetWindowRect(rect);

			//if (m_bWireFrame)
			{
				rect.DeflateRect(2,2);
				DrawWireRects(rect, m_rectFrom, 20);
			}
			//else
				//DrawAnimatedRects(m_hWnd,IDANI_CAPTION, rect, m_rectFrom);
		}
		CWnd::OnClose();
	}

}

BOOL CSplashWnd::SelRelPal(BOOL bForceBkgnd)
{
  // We are going active, so realize our palette.
  CDC* pDC = GetDC();

  CPalette* pOldPal = pDC->SelectPalette(&m_Palette, bForceBkgnd);
  UINT u = pDC->RealizePalette();
  pDC->SelectPalette(pOldPal, TRUE);
  pDC->RealizePalette();


  ReleaseDC(pDC);

  // If any colors have changed or we are in the
  // background, repaint the lot.
  if (u || bForceBkgnd) 
    InvalidateRect(NULL, TRUE); // Repaint.
  
  return (BOOL) u; // TRUE if some colors changed.
}

void CSplashWnd::OnPaletteChanged(CWnd* pFocusWnd)
{
  // See if the change was caused by us and ignore it if not.
  if (pFocusWnd != this) 
    SelRelPal(TRUE); // Realize in the background. 
}

BOOL CSplashWnd::OnQueryNewPalette()
{
  return SelRelPal(FALSE); // Realize in the foreground.
}

void CSplashWnd::SetBitmapToUse(const CString& sFilename)
{
  m_bUseFile = TRUE;
  m_sFilename = sFilename;
}

void CSplashWnd::SetBitmapToUse(UINT nResourceID)
{
  m_bUseFile = FALSE;
  m_pszResourceName = MAKEINTRESOURCE(nResourceID);
}

void CSplashWnd::SetBitmapToUse(LPCTSTR pszResourceName)
{
  m_bUseFile = FALSE;
  m_pszResourceName = pszResourceName;
}

