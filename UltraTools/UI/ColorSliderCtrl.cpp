//
// ColorSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "ColorSliderCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSliderCtrl

IMPLEMENT_DYNAMIC(CColorSliderCtrl, CBCGPSliderCtrl)

CColorSliderCtrl::CColorSliderCtrl()
{
	m_bIsHueSlider = FALSE;
	m_bIsSaturationSlider = FALSE;
	m_dblHue = 0;
	m_dblSaturation = 0;

	m_ThumbB.SetImageSize (CSize (8, 15));
	m_ThumbB.Load(IDB_THUMB_B);
	m_ThumbV.SetImageSize (CSize (16, 8));
	m_ThumbV.Load(IDB_THUMB_V);
}
// 
// CColorSliderCtrl::~CColorSliderCtrl()
// {
// }

BEGIN_MESSAGE_MAP(CColorSliderCtrl, CBCGPSliderCtrl)
	//{{AFX_MSG_MAP(CColorSliderCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSliderCtrl message handlers

void CColorSliderCtrl::OnPaint() 
{
	if (!m_bVisualManagerStyle)
	{
		Default ();
		return;
	}

	CPaintDC dc(this);
	
	CBCGPMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	if (!CBCGPVisualManager::GetInstance ()->OnFillParentBarBackground (this, pDC))
	{
		globalData.DrawParentBackground (this, pDC, NULL);
	}

	DWORD dwStyle = GetStyle ();
	BOOL bVert = (dwStyle & TBS_VERT);
	BOOL bLeftTop = (dwStyle & TBS_BOTH) || (dwStyle & TBS_LEFT);
	BOOL bRightBottom = (dwStyle & TBS_BOTH) || ((dwStyle & TBS_LEFT) == 0);
	BOOL bIsFocused = GetSafeHwnd () == ::GetFocus ();

	CRect rectChannel;
	GetChannelRect (rectChannel);

	if (bVert)
	{
		CRect rect = rectChannel;

		rectChannel.left = rect.top;
		rectChannel.right = rect.bottom;
		rectChannel.top = rect.left;
		rectChannel.bottom = rect.right;
	}

	DrawSliderChannel(pDC, bVert, rectChannel, m_bOnGlass);

	CRect rectThumb;
	GetThumbRect (rectThumb);

	DrawSliderThumb (pDC, rectThumb, m_bIsThumbHighligted || bIsFocused,
		m_bIsThumPressed, !IsWindowEnabled (),
		bVert, bLeftTop, bRightBottom, m_bOnGlass);

	if (bIsFocused && m_bDrawFocus)
	{
		CRect rectFocus;
		GetClientRect (rectFocus);

		pDC->DrawFocusRect (rectFocus);
	}
}

void CColorSliderCtrl::DrawSliderChannel (CDC* pDC, BOOL bVert, CRect rect, BOOL bDrawOnGlass)
{
	if (CanDrawImage ())
	{
		ASSERT_VALID (pDC);

		rect.OffsetRect (1, -1);
		rect.DeflateRect(2,0,3,0);
		int nSize = rect.Width ();

		int nOldBK = pDC->GetBkColor ();

		if (m_bIsHueSlider)
		{
			for (int i = 0; i < nSize; i++)
			{
				COLORREF clr = CBCGPDrawManager::HLStoRGB_TWO (i * 359.0 / (double)(nSize - 1) + 45.0, 0.5, 1.0);
				pDC->FillSolidRect (rect.left + i, rect.top, 1, rect.Height (), clr);
			}
		}
		else if(m_bIsSaturationSlider)
		{
			COLORREF clrSrc = CBCGPDrawManager::HLStoRGB_TWO (m_dblHue + 180.0 + 45.0, 0.5, 1.0);
			double clrSrcR = GetRValue (clrSrc);
			double clrSrcG = GetGValue (clrSrc);
			double clrSrcB = GetBValue (clrSrc);

			for (int i = 0; i < nSize; i++)
			{
				double value = i / (double)(nSize - 1);
				double value255 = 255.0 * (1.0 - value);

				COLORREF clr = RGB 
					(
					value255 + clrSrcR * value,
					value255 + clrSrcG * value,
					value255 + clrSrcB * value
					);
				pDC->FillSolidRect (rect.left + i, rect.top, 1, rect.Height (), clr);
			}
		}
		else
			CBCGPVisualManager::GetInstance ()->OnDrawSliderChannel (pDC, this, bVert, rect, bDrawOnGlass);

		pDC->SetBkColor (nOldBK);

		return;
	}

	CBCGPVisualManager::GetInstance ()->OnDrawSliderChannel (pDC, this, bVert, rect, bDrawOnGlass);
}

void CColorSliderCtrl::DrawSliderThumb (CDC* pDC,
										CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
										BOOL bVert, BOOL bLeftTop, BOOL bRightBottom, BOOL bDrawOnGlass)
{
	if (CanDrawImage ())
	{
		CRect rectSlider;
		GetClientRect (rectSlider);

		ASSERT_VALID (pDC);

		int nIndex = 0;
		if (bIsPressed)
		{
			nIndex = 2;
		}
		else if (bIsHighlighted)
		{
			nIndex = 1;
		}

		if (bVert)
		{
			if (bLeftTop && bRightBottom)
			{
				rect.left  = rectSlider.left-1;
				rect.right = rectSlider.right+16;
				m_ThumbV.DrawEx (pDC, rect, nIndex, 
					CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
			}
		}
		else
		{
			if (!bLeftTop && bRightBottom)
			{
				rect.top    = rectSlider.top;
				rect.bottom = rectSlider.bottom;
				m_ThumbB.DrawEx (pDC, rect, nIndex, 
					CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
			}
		}

		return;
	}
}
