// PreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExplorerView.h"
#include "ImagePreviewDlg.h"
#include "Utility/MemDC.h"
#include "CxImage/xImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagePreviewDlg
CImagePreviewDlg::CImagePreviewDlg(CWnd* pOwner)
{
	m_pOwner = pOwner;
	m_bOwnerIsExplorer = TRUE;
	m_sFileName = _T("");
}

CImagePreviewDlg::~CImagePreviewDlg()
{
}

BEGIN_MESSAGE_MAP(CImagePreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CImagePreviewDlg)
	ON_WM_CREATE()
	ON_WM_DRAWITEM()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePreviewDlg message handlers
int CImagePreviewDlg::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if( CDialog::OnCreate(lpCreateStruct) == -1 )
		return -1;
	
	CRect rect;
	GetClientRect(&rect);

	m_wndCanvas.Create( _T("Image Preview Canvas"),
		                WS_VISIBLE | WS_CHILD | SS_OWNERDRAW,
						rect, this, 70);

	return 0;
}

void CImagePreviewDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE )
	{
		CMyMemDC *pMemDC = NULL;
		pMemDC = new CMyMemDC( lpDrawItemStruct->hDC );

		CRect rect;
		GetClientRect(rect);
		HBRUSH hBrush = ::CreateSolidBrush( RGB(255, 255, 255) );
		
		::FillRect(pMemDC->m_hDC, rect, hBrush );
		
		DeleteObject( hBrush );
		CxImage* pSelectedImage = NULL;
		if (m_bOwnerIsExplorer)
		{
			CExplorerView* pView = (CExplorerView*)m_pOwner;
			ASSERT(pView);
			pSelectedImage = pView->GetImage();
		}
		m_wndCanvas.GetClientRect(rect);
		if( pSelectedImage != NULL )
			pSelectedImage->Draw(pMemDC->m_hDC, rect, NULL, TRUE);

		delete pMemDC;
	}
}

void CImagePreviewDlg::OnSize( UINT nType, int cx, int cy ) 
{
	CDialog::OnSize(nType, cx, cy);

	CString sCaption = _T("Í¼Æ×Ô¤ÀÀ");
	int nLen = m_sFileName.GetLength();
	CString sInfo, sTem;
	if(nLen >0)
	{
		if(nLen > 50)
		{
			int nPos, nPos2;
			nPos = m_sFileName.Find('\\');
			nPos2 = m_sFileName.ReverseFind('\\');
			if (nPos2 > 0)
			{
				sTem = m_sFileName.Mid(0, nPos2);
				nPos2 = sTem.ReverseFind('\\');
			}

			if (nPos2 <= nPos)
				sTem = _T("...") + m_sFileName.Right(50);
			else
				sTem = m_sFileName.Mid(0, nPos+1) + _T("...") + m_sFileName.Mid(nPos2);
		}
		else
			sTem = m_sFileName;
		sInfo.Format(_T("%s--%s"), sCaption, sTem);
		sCaption = sInfo;
	}
	SetWindowText(sCaption);
	
	CRect rc; GetClientRect( rc );

	CxImage* pSelectedImage = NULL;
	if (m_bOwnerIsExplorer)
	{
		CExplorerView* pView = (CExplorerView*)m_pOwner;
		ASSERT(pView);
		pSelectedImage = pView->GetImage();
	}

	if( pSelectedImage == NULL )
	{
		rc.SetRectEmpty();
		m_wndCanvas.MoveWindow( rc );
	}
	else
	{
		const int   nWidth    = rc.Width();
		const int   nHeight   = rc.Height();
		const float fRatio    = (float)nHeight/nWidth;
		const float fImgRatio = (float)pSelectedImage->GetHeight()/pSelectedImage->GetWidth();

		int XDest, YDest, nDestWidth, nDestHeight;
	
		if( fImgRatio > fRatio )
		{
			nDestWidth  = (int)(nHeight/fImgRatio);
			XDest       = (nWidth-nDestWidth)/2;
			YDest       = 0;
			nDestHeight = nHeight;
		}
		else
		{
			XDest       = 0;
			nDestWidth  = nWidth;
			nDestHeight = (int)(nWidth*fImgRatio);
			YDest       = (nHeight-nDestHeight)/2;
		}

		CRect rect(XDest, YDest, XDest+nDestWidth, YDest+nDestHeight);
		m_wndCanvas.MoveWindow( rect );
		m_wndCanvas.Invalidate();
	}
}

HBRUSH CImagePreviewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	switch (nCtlColor) 
	{
	case CTLCOLOR_DLG:
		pDC->SetBkColor( RGB(255,255,255) );
		return (HBRUSH)GetStockObject( WHITE_BRUSH );
	default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}
