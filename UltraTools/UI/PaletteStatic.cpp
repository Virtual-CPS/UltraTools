// PaletteStatic.cpp : implementation file
//

#include "stdafx.h"
#include "PaletteStatic.h"
#include "PaletteColorTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LIMIT_RECT_HEIGHT	16

/////////////////////////////////////////////////////////////////////////////
// CPaletteStatic

CPaletteStatic::CPaletteStatic()
{
	m_bCreateBmp = FALSE;
	m_bShowCalibration = FALSE;
	m_bShowLimits = FALSE;
	m_fUpperLimit = 47.3f;
	m_fLowerLimit = 25.0f;

	m_logpen.lopnStyle = PS_INSIDEFRAME;
	m_logpen.lopnWidth.x = 1;
	m_logpen.lopnWidth.y = 1;
	m_logpen.lopnColor = RGB(0, 0, 0);

	m_logbrush.lbStyle = BS_SOLID;
	m_logbrush.lbColor = RGB(255, 255, 255);
	m_logbrush.lbHatch = HS_HORIZONTAL;

	VERIFY(m_font.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")));             // lpszFacename

	m_nPaletteID = -1;
	m_bPaletteReversed = FALSE;
}

CPaletteStatic::~CPaletteStatic()
{
	if (m_bCreateBmp == TRUE)
	{
		m_Bitmap.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CPaletteStatic, CStatic)
	//{{AFX_MSG_MAP(CPaletteStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteStatic message handlers

void CPaletteStatic::PreSubclassWindow() 
{
	CreateBmp();
	CStatic::PreSubclassWindow();
}

void CPaletteStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_bCreateBmp == FALSE)
	{
		return;
	}

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	
	BITMAP bmInfo;
	m_Bitmap.GetBitmap(&bmInfo);
	
	CBitmap* pOldBmp = MemDC.SelectObject(&m_Bitmap);

	dc.BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight, &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();

	// Do not call CStatic::OnPaint() for painting messages
}

// ��ָ����Ŀ��CDC��rectDst��������Ŀ��CDC��ָ���������
// �����á����ء�CDC���Լ���client������
void CPaletteStatic::DrawPalette(int nID, CRect* pDstRect, CDC* pDstDC)
{
    if (nID < 0 || nID >= PALLETTE_COLOR_SCALE_COUNT)
    {
		return;
    }

	m_nPaletteID = nID;

	CDC* pDC;
	if(pDstDC == NULL)
		pDC= GetDC();
	else
		pDC = pDstDC;

	CBrush brush, *pOldBrush=NULL;
	CPen pen, *pOldPen=NULL;
	CFont *pOldFont=NULL;
	CBitmap *pOldBmp=NULL;
	CDC  MemDC;
	CString sInfo;
	long lLower, lUpper;
	int nH, i, nTickCnt, nTickMaster, nTickInter, nIndex;
	float fPixelsPerValue, fPixelsPerTick;
	float f, fFirstTickMasterPosition, fFirstTickInterPosition;
	int nValue, nValueStep;
	CRect rectClient, rectBar, rectTopLimit, rectBottomLimit, rectScale;
	CPoint ptFrom, ptTo;

	if(pDstRect==NULL || pDstRect->IsRectEmpty())
		GetClientRect(&rectClient);
	else
		rectClient = *pDstRect;
	rectClient.NormalizeRect();
	rectBar = CalculateBarRect(rectClient);
	nH = rectBar.Height();

	if(nH <= 0)
		return;

	MemDC.CreateCompatibleDC(pDC);

	//-----------------------------------------------------
	// ���Ƶ�ɫ��
	CPen penPalette[PALLETTE_COUNT];
	if (!m_bPaletteReversed)			// ������ʾ��ɫ��
	{
		for (i=0; i<PALLETTE_COUNT; i++)
		{
			(penPalette[PALLETTE_COUNT - i - 1]).CreatePen(PS_SOLID, 1, RGB( PaletteColorsTable[nID][i][0],
															PaletteColorsTable[nID][i][1], 
															PaletteColorsTable[nID][i][2]));
		}
	}
	else								// ȡ��
	{
		for (i=0; i<PALLETTE_COUNT; i++)
		{
			(penPalette[i]).CreatePen(PS_SOLID, 1, RGB( PaletteColorsTable[nID][i][0],
															PaletteColorsTable[nID][i][1], 
															PaletteColorsTable[nID][i][2]));
		}
	}

	pOldPen = MemDC.SelectObject(&(penPalette[0]));
	pOldBmp = MemDC.SelectObject(&m_Bitmap);

	float fDist = (float)nH / (float)PALLETTE_COUNT;
	for (i=0; i<nH; i++)
	{
		int nIndex = (int)((float)(i) / fDist);
		MemDC.SelectObject(&(penPalette[nIndex]));
		MemDC.MoveTo(rectBar.left, rectBar.top+i);
		MemDC.LineTo(rectBar.right, rectBar.top+i);
	}

	for (i=0; i<PALLETTE_COUNT; i++)
	{
		(penPalette[i]).DeleteObject();
	}
	//-----------------------------------------------------

	if (!brush.CreateBrushIndirect(&m_logbrush))
		goto EXIT_DRAW_PALETTE;
	if (!pen.CreatePenIndirect(&m_logpen))
		goto EXIT_DRAW_PALETTE;

	pOldBrush = MemDC.SelectObject(&brush);
	pOldFont = MemDC.SelectObject(&m_font);

	// ��ʾ����������
	if (m_bShowLimits)
	{
		// �����ޱ߿򡰱�����
		MemDC.SelectStockObject(NULL_PEN);
		rectTopLimit = rectClient;
		rectTopLimit.bottom = rectBar.top+1;
		MemDC.Rectangle(rectTopLimit);

		rectBottomLimit = rectClient;
		rectBottomLimit.top = rectBar.bottom;
		MemDC.Rectangle(rectBottomLimit);

		// ��ʾ������
		MemDC.SetBkMode(TRANSPARENT);
		MemDC.SetTextColor(m_logpen.lopnColor);
		sInfo.Format(_T("%.1f��"), m_fUpperLimit);
		MemDC.TextOut(rectTopLimit.left+2, rectTopLimit.top+2, sInfo);
		sInfo.Format(_T("%.1f��"), m_fLowerLimit);
		MemDC.TextOut(rectBottomLimit.left+2, rectBottomLimit.top+2, sInfo);
	}

	// ���ƿ̶�
	if(m_bShowCalibration)
	{
		// �����ޱ߿򡰱�����
		MemDC.SelectStockObject(NULL_PEN);
		rectScale = rectClient;
		if(m_bShowLimits)
		{
			rectScale.top = rectBar.top-2;
			rectScale.bottom = rectBar.bottom+2;
		}
		else
		{
			rectScale.top = rectBar.top;
			rectScale.bottom = rectBar.bottom;
		}
		rectScale.left = rectBar.right;
		MemDC.Rectangle(rectScale);

		MemDC.SelectObject(&pen);

		// ��������
		if(m_bShowLimits)
		{
			rectScale.top = rectBar.top;
			rectScale.bottom = rectBar.bottom;
		}
		rectScale.left = rectScale.left + 3;
		MemDC.MoveTo(rectScale.TopLeft());
		MemDC.LineTo(rectScale.left, rectScale.bottom);

		// ���ƺ���(�̶�)���̶���ֻ��ʾ����
		nH = rectScale.Height();

		// �Ŵ�Ϊ����������һ������ÿ��tickMark
		lUpper = (long)(m_fUpperLimit * 100);
		lLower = (long)(m_fLowerLimit * 100);
		fPixelsPerValue = (float)nH / (lUpper - lLower);

		i = (int)(m_fUpperLimit - m_fLowerLimit);	// ��������������
		if(i > 10)
		{
			nTickMaster = 10;	// ���̶ȸ���
			nTickInter = 5;		// ���̶�֮��������
		}
		else if(i > 5)
		{
			nTickMaster = 5;
			nTickInter = 10;
		}
		else
		{
			nTickMaster = 1;
			nTickInter = 20;
		}
		nTickCnt = (nTickMaster+1) * nTickInter;

		if(nTickMaster == 1)
		{
			fFirstTickMasterPosition = 0;
			nValueStep = 0;
			fPixelsPerTick = 1.0f * nH / nTickCnt;	// �ȷ�����height
			fFirstTickInterPosition = 0;
			nIndex = nTickCnt + 1;
		}
		else										// ���ܵȷ�����height
		{
			f = 100.f * (m_fUpperLimit - m_fLowerLimit) / nTickMaster;
			fFirstTickMasterPosition = f * fPixelsPerValue;
			nValueStep = (int)(f / 100.f);
			fPixelsPerTick = f / nTickInter * fPixelsPerValue;
			i = (int)(fFirstTickMasterPosition / fPixelsPerTick);
			fFirstTickInterPosition = fFirstTickMasterPosition - i*fPixelsPerTick;
			nIndex = nTickInter - i +1;
		}

		MemDC.SetBkMode(TRANSPARENT);
		MemDC.SetTextColor(m_logpen.lopnColor);
		nValue = (int)m_fLowerLimit + nValueStep;
		fFirstTickInterPosition = rectScale.bottom - fFirstTickInterPosition;
		ptFrom.x = rectScale.left;
		for (i=0; i<=nTickCnt; i++)					// Ӧ��һ��tick
		{
			ptFrom.y = (long)(fFirstTickInterPosition - i*fPixelsPerTick+0.5) -1;	// �ʿ�1
			if(ptFrom.y < rectScale.top)
				break;
			if(ptFrom.y > rectScale.bottom)			// ������������µĿ̶�
				continue;

			ptTo.y = ptFrom.y;
			if(nIndex % nTickInter == 0)
			{
				ptTo.x = ptFrom.x + 6;
				MemDC.MoveTo(ptFrom);
				MemDC.LineTo(ptTo);

				if(i==0 && !m_bShowLimits)
				{
					sInfo.Format(_T("%d��"), nValue);
					MemDC.TextOut(ptTo.x+2, ptTo.y-11, sInfo);
				}
				else if(i==nTickCnt && !m_bShowLimits)
				{
					sInfo.Format(_T("%d��"), nValue);
					MemDC.TextOut(ptTo.x+2, ptTo.y+6, sInfo);
				}
				else if(i!=0 && i!=nTickCnt)
				{
					sInfo.Format(_T("%d"), nValue);
					MemDC.TextOut(ptTo.x+2, ptTo.y-5, sInfo);
				}

				nValue += nValueStep;
			}
			else
			{
				ptTo.x = ptFrom.x + 3;
				MemDC.MoveTo(ptFrom);
				MemDC.LineTo(ptTo);
			}

			nIndex++;
		}
	}

	// ��������߿�
	MemDC.SelectObject(&pen);
	MemDC.SelectStockObject(NULL_BRUSH);
	MemDC.Rectangle(rectBar);
	MemDC.Rectangle(rectClient);

EXIT_DRAW_PALETTE:

	pDC->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), &MemDC, 0, 0, SRCCOPY);

	if(pOldFont)
		MemDC.SelectObject(pOldFont);
	if(pOldPen)
		MemDC.SelectObject(pOldPen);
	if(pOldBrush)
		MemDC.SelectObject(pOldBrush);
	if(pOldBmp)
		MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();

	if(pDstDC == NULL)
		ReleaseDC(pDC);
}

void CPaletteStatic::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	CreateBmp();
}

void CPaletteStatic::CreateBmp()
{
	if (m_bCreateBmp == TRUE)
	{
		m_Bitmap.DeleteObject();
	}

	// ����λͼ
	CRect rectClient;
	GetClientRect(&rectClient);

	CDC* pDC = GetDC();
	m_bCreateBmp = m_Bitmap.CreateCompatibleBitmap(pDC, rectClient.Width(), rectClient.Height());
	ReleaseDC(pDC);
}

CRect CPaletteStatic::CalculateBarRect(CRect rectClient)
{
	CRect rectBar;
	rectBar = rectClient;
	rectBar.NormalizeRect();

	int nWidth = rectClient.Width();

	if(m_bShowLimits)
	{
		rectBar.top = rectBar.top + LIMIT_RECT_HEIGHT;
		rectBar.bottom = rectBar.bottom - LIMIT_RECT_HEIGHT;
	}
	if (m_bShowCalibration)
	{
		rectBar.right = rectBar.right - nWidth * 3/4;
	}

	return rectBar;
}
