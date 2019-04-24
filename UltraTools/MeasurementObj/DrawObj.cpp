// DrawObj.cpp - implementation for drawing objects

#include "stdafx.h"
#include "DrawObj.h"

#include "../DirectShow/VideoDirectShowWnd.h"
#include "../GlobalDefsForSys.h"

CDrawObj::CDrawObj()
{
}

CDrawObj::~CDrawObj()
{
}

CDrawObj::CDrawObj(const CRect& position)
{
	m_rectPosition = position;
	m_rectPosition.NormalizeRect();
	m_rectTip.SetRectEmpty();

#ifdef TSMO_SYS
	m_bShowTip = FALSE;
	m_bShowIndex = TRUE;
#else
	m_bShowTip = TRUE;		// tip中含有序号，可以同为FALSE，但不可同为TRUE
	m_bShowIndex = FALSE;
#endif

	m_pVideoWnd = NULL;

	m_bPen = TRUE;
	m_logpen.lopnStyle = PS_INSIDEFRAME;
	m_logpen.lopnWidth.x = 2;
	m_logpen.lopnWidth.y = 2;

	// 与视频背景色按位异或，实际XOR绘图时方可用指定的颜色
//	m_logpen.lopnColor = RGB(0, 255, 255)^RGB(255,255,255);
	m_logpen.lopnColor = RGB(120, 240, 240);
//	m_logpen.lopnColor = RGB(0, 255, 255);

	m_bBrush = FALSE;							// 无填充（空心矩形）
	m_logbrush.lbStyle = BS_SOLID;
	m_logbrush.lbColor = RGB(192, 192, 192);
	m_logbrush.lbHatch = HS_HORIZONTAL;

	// TRUE=move或改变大小时，处于编辑状态; FALSE=鼠标左键弹起，编辑结束
	m_bIsEditing = FALSE;

	m_nIndex = -1;
	m_faValues[0] = 0.f;
	m_faValues[1] = 0.f;
	m_faValues[2] = 0.f;
}

void CDrawObj::Remove()
{
	delete this;
}

void CDrawObj::Draw(CDC*)
{
}

void CDrawObj::DrawTracker(CDC* pDC, TrackerState state)
{
	ASSERT_VALID(this);

	switch (state)
	{
	case normal:
		break;

	case selected:
	case active:
		{
			int nHandleCount = GetHandleCount();
			for (int nHandleIndex = 1; nHandleIndex <= nHandleCount; nHandleIndex++)
			{
				CPoint handle = GetHandlePoint(nHandleIndex);
				pDC->PatBlt(handle.x-3, handle.y-3, 7, 7, DSTINVERT);
			}
		}
		break;
	}
}

void CDrawObj::MoveTo(const CRect& position, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);

	CRect rectLocal = position;

	if (pVideoWnd == NULL)
	{
		Invalidate();
		m_rectPosition = rectLocal;
		Invalidate();
	}
	else
	{
		pVideoWnd->InvalidateObj(this);
		m_rectPosition = rectLocal;
		pVideoWnd->InvalidateObj(this);
	}

	if(!m_bIsEditing)	// 移动路径上的中间位置，丢弃
		pVideoWnd->SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, this);
}

// 如果bSelected==TRUE，return鼠标所点击的矩形锚柄的序号(从左上角开始，顺时针编号，1,2,3...)
// 0 means no hit
// If !bSelected, 0 = no hit, 1 = hit (anywhere)
int CDrawObj::HitTest(CPoint point, CVideoDirectShowWnd* pVideoWnd, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pVideoWnd != NULL);

	if(((CDrawRect*)this)->GetShapType() != CDrawRect::RECT_SHAPE_LINE)
		m_rectPosition.NormalizeRect();

	CPoint ptLocal = point;
	//CPoint ptLocal = CalculateScaledPoint(point, TRUE);

	if (bSelected)
	{
		int nHandleCount = GetHandleCount();
		for (int nHandleIndex=1; nHandleIndex<=nHandleCount; nHandleIndex++)
		{
			// GetHandleRect returns in logical coords
			CRect rc = GetHandleRect(nHandleIndex, pVideoWnd);
			if(rc.PtInRect(ptLocal))
				return nHandleIndex;
		}
	}
	else
	{
		CRect rect(ptLocal, CSize(3, 3));
		rect.NormalizeRect();
		if (Intersects(rect))
			return 1;
	}

	return 0;
}

BOOL CDrawObj::Intersects(const CRect& rect)
{
	ASSERT_VALID(this);

	CRect rectPos = m_rectPosition;
	rectPos.NormalizeRect();
	CRect rectTip = m_rectTip;
	rectTip.NormalizeRect();
	CRect rectT = rect;
	rectT.NormalizeRect();

	return !(rectT & rectPos).IsRectEmpty() || !(rectT & rectTip).IsRectEmpty();
}

int CDrawObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 8;
}

// 获取nHandleIndex指定Handle的中心点坐标
CPoint CDrawObj::GetHandlePoint(int nHandleIndex)
{
	ASSERT_VALID(this);
	int x, y, xCenter, yCenter;

	CRect rect = m_rectPosition;
	rect.NormalizeRect();

	// this gets the center regardless of left/right and top/bottom ordering
	xCenter = rect.left + rect.Width() / 2;
	yCenter = rect.top + rect.Height() / 2;

	switch (nHandleIndex)
	{
	default:
		ASSERT(FALSE);

	case 1:
		x = rect.left;
		y = rect.top;
		break;

	case 2:
		x = xCenter;
		y = rect.top;
		break;

	case 3:
		x = rect.right;
		y = rect.top;
		break;

	case 4:
		x = rect.right;
		y = yCenter;
		break;

	case 5:
		x = rect.right;
		y = rect.bottom;
		break;

	case 6:
		x = xCenter;
		y = rect.bottom;
		break;

	case 7:
		x = rect.left;
		y = rect.bottom;
		break;

	case 8:
		x = rect.left;
		y = yCenter;
		break;
	}

	return CPoint(x, y);
}

// return rectange of handle
CRect CDrawObj::GetHandleRect(int nHandleID, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);
	ENSURE(pVideoWnd != NULL);

	CRect rect;

	// get the center of the handle
	CPoint point = GetHandlePoint(nHandleID);
	rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);

	return rect;
}

HCURSOR CDrawObj::GetHandleCursor(int nHandleIndex)
{
	ASSERT_VALID(this);

	LPCTSTR id;
	switch (nHandleIndex)
	{
	default:
		ASSERT(FALSE);

	case 1:
	case 5:
		id = IDC_SIZENWSE;
		break;

	case 2:
	case 6:
		id = IDC_SIZENS;
		break;

	case 3:
	case 7:
		id = IDC_SIZENESW;
		break;

	case 4:
	case 8:
		id = IDC_SIZEWE;
		break;
	}

	return AfxGetApp()->LoadStandardCursor(id);
}

void CDrawObj::MoveHandleTo(int nHandleIndex, CPoint point, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);

	CRect position = m_rectPosition;
	CPoint ptLocal = point;

	switch (nHandleIndex)
	{
	default:
		ASSERT(FALSE);

	case 1:
		position.left = ptLocal.x;
		position.top = ptLocal.y;
		break;

	case 2:
		position.top = ptLocal.y;
		break;

	case 3:
		position.right = ptLocal.x;
		position.top = ptLocal.y;
		break;

	case 4:
		position.right = ptLocal.x;
		break;

	case 5:
		position.right = ptLocal.x;
		position.bottom = ptLocal.y;
		break;

	case 6:
		position.bottom = ptLocal.y;
		break;

	case 7:
		position.left = ptLocal.x;
		position.bottom = ptLocal.y;
		break;

	case 8:
		position.left = ptLocal.x;
		break;
	}

	MoveTo(position, pVideoWnd);
}

void CDrawObj::Invalidate()
{
	ASSERT_VALID(this);

	if(m_pVideoWnd != NULL)
		m_pVideoWnd->InvalidateObj(this);
}

// 根据m_rectPosition更新m_rectTip
void CDrawObj::UpdateTipPosition(CVideoDirectShowWnd* pVideoWnd)
{
}

void CDrawObj::SetLineColor(COLORREF color)
{
	ASSERT_VALID(this);

	m_logpen.lopnColor = color;
	Invalidate();
}

void CDrawObj::SetFillColor(COLORREF color)
{
	ASSERT_VALID(this);

	m_logbrush.lbColor = color;
	Invalidate();
}

void CDrawObj::SetLineWeight (int nWidth)
{
	ASSERT_VALID(this);

	if (m_bPen)
	{
		m_bPen = nWidth > 0;
	}

	if (m_bPen)
	{
		m_logpen.lopnWidth.x = nWidth;
		m_logpen.lopnWidth.y = nWidth;
	}	

	Invalidate();
}

void CDrawObj::EnableFill (BOOL bEnable)
{
	m_bBrush = bEnable;
}

void CDrawObj::EnableLine (BOOL bEnable)
{
	m_bPen = bEnable;
}

// index和tip，显示其一
void CDrawObj::EnableShowTip (BOOL bEnable)
{
	m_bShowTip = bEnable;
	m_bShowIndex = !bEnable;
}

#ifdef _DEBUG
void CDrawObj::AssertValid()
{
	ASSERT(m_rectPosition.left <= m_rectPosition.right);
	ASSERT(m_rectPosition.bottom <= m_rectPosition.top);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CDrawRect


CDrawRect::CDrawRect()
{
}

CDrawRect::CDrawRect(const CRect& position)
	: CDrawObj(position)
{
	ASSERT_VALID(this);

	m_nShape = RECT_SHAPE_RECTANGLE;
	m_ptRoundness.x = 16;
	m_ptRoundness.y = 16;
	m_nSpotSize = 48;			// 选4和6的公倍数
}

void CDrawRect::Draw(CDC* pDC)
{
	ASSERT_VALID(this);

	CBrush brush;
	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;
	int olDrop2;
	CString sInfo;
	int nHeight, nWidth;

	if (!brush.CreateBrushIndirect(&m_logbrush))
		return;
	if (!pen.CreatePenIndirect(&m_logpen))
		return;

	if (m_bBrush)
		pOldBrush = pDC->SelectObject(&brush);
	else
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

	if (m_bPen)
		pOldPen = pDC->SelectObject(&pen);
	else
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

	if(m_bIsEditing)
	{
		olDrop2 = pDC->GetROP2();				//取得原来屏幕画图方式
		pDC->SetROP2(R2_XORPEN);				//设置异或屏幕画图方式
	}

	CRect rect = m_rectPosition;
	switch (m_nShape)
	{
	case RECT_SHAPE_RECTANGLE:
		{
			pDC->Rectangle(rect);

			if (m_bShowIndex && !m_bIsEditing)								// 绘制序号标志
			{
				nHeight = rect.Height();
				nWidth = rect.Width();
				if(nWidth<10 || nHeight<10)
					break;

				sInfo.Format(_T("%d"), m_nIndex + 1);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(m_logpen.lopnColor);
				pDC->TextOut(rect.right-20, rect.bottom-20, sInfo);
			}
			else if(m_bShowTip && !m_bIsEditing && !m_rectTip.IsRectEmpty())// 绘制tip
			{
				m_rectTip.NormalizeRect();
				CBCGPDrawManager dm (*pDC);
				dm.FillGradient(m_rectTip, m_logpen.lopnColor, RGB(255, 255, 255), FALSE);
				if(m_sTip.GetLength() > 0)
					sInfo = m_sTip;
				else
					sInfo.Format(_T("区温%d：最高%.1f，最低%.1f，平均%.1f (℃)"),
									m_nIndex+1, m_faValues[0],m_faValues[1],m_faValues[2]);
				pDC->SetBkMode(TRANSPARENT);
				CFont* pOldFont = pDC->SelectObject(&globalData.fontRegular);
				pDC->TextOut(m_rectTip.left+3, m_rectTip.bottom-16, sInfo);
				pDC->SelectObject(pOldFont);
			}
		}
		break;

	case RECT_SHAPE_ROUND_RECTANGLE:
			pDC->RoundRect(rect, m_ptRoundness);
		break;

	case RECT_SHAPE_ELLIPSE:
		pDC->Ellipse(rect);
		break;

	case RECT_SHAPE_LINE:
		{
			if (rect.top > rect.bottom)
			{
				rect.top -= m_logpen.lopnWidth.y / 2;
				rect.bottom += (m_logpen.lopnWidth.y + 1) / 2;
			}
			else
			{
				rect.top += (m_logpen.lopnWidth.y + 1) / 2;
				rect.bottom -= m_logpen.lopnWidth.y / 2;
			}

			if (rect.left > rect.right)
			{
				rect.left -= m_logpen.lopnWidth.x / 2;
				rect.right += (m_logpen.lopnWidth.x + 1) / 2;
			}
			else
			{
				rect.left += (m_logpen.lopnWidth.x + 1) / 2;
				rect.right -= m_logpen.lopnWidth.x / 2;
			}

			CPoint ptFrom, ptTo;
			ptFrom = rect.TopLeft();
			ptTo = rect.BottomRight();
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);

			if(m_bShowTip && !m_bIsEditing && !m_rectTip.IsRectEmpty())// 绘制tip
			{
				m_rectTip.NormalizeRect();
				CBCGPDrawManager dm (*pDC);
				dm.FillGradient(m_rectTip, m_logpen.lopnColor, RGB(255, 255, 255), FALSE);
				if(m_sTip.GetLength() > 0)
					sInfo = m_sTip;
				else
					sInfo.Format(_T("线温%d：最高%.1f，最低%.1f，平均%.1f (℃)"),
									m_nIndex+1, m_faValues[0],m_faValues[1],m_faValues[2]);
				pDC->SetBkMode(TRANSPARENT);
				CFont* pOldFont = pDC->SelectObject(&globalData.fontRegular);
				pDC->TextOut(m_rectTip.left+3, m_rectTip.bottom-16, sInfo);
				pDC->SelectObject(pOldFont);
			}
		}
		break;

	case RECT_SHAPE_SPOT:
		{
			CPoint ptFrom, ptTo, ptCenter;
			nHeight = rect.Height();// spot大小不因zoom而改变
			nWidth = rect.Width();
			ptCenter = rect.CenterPoint();

			// 绘制水平线
			ptFrom = CPoint(ptCenter.x-nWidth/2, ptCenter.y);
			ptTo = CPoint(ptCenter.x-nWidth/6, ptCenter.y);
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
			ptFrom = CPoint(ptCenter.x+nWidth/6, ptCenter.y);
			ptTo = CPoint(ptCenter.x+nWidth/2, ptCenter.y);
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);

			// 绘制铅垂线
			ptFrom = CPoint(ptCenter.x, ptCenter.y-nHeight/2);
			ptTo = CPoint(ptCenter.x, ptCenter.y-nHeight/6);
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
			ptFrom = CPoint(ptCenter.x, ptCenter.y+nHeight/6);
			ptTo = CPoint(ptCenter.x, ptCenter.y+nHeight/2);
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);

			// 绘制序号标志
			if (m_bShowIndex && !m_bIsEditing)
			{
				sInfo.Format(_T("%d"), m_nIndex + 1);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(m_logpen.lopnColor);
				pDC->TextOut(ptCenter.x+nWidth/4, ptCenter.y+nHeight/4, sInfo);
			}
			else if(m_bShowTip && !m_bIsEditing && !m_rectTip.IsRectEmpty())// 绘制tip
			{
				m_rectTip.NormalizeRect();
				CBCGPDrawManager dm (*pDC);
				dm.FillGradient(m_rectTip, m_logpen.lopnColor, RGB(255, 255, 255), FALSE);
				if(m_sTip.GetLength() > 0)
					sInfo = m_sTip;
				else
					sInfo.Format(_T("点温%d：%.1f ℃"), m_nIndex+1, m_faValues[0]);
				pDC->SetBkMode(TRANSPARENT);
				CFont* pOldFont = pDC->SelectObject(&globalData.fontRegular);
				pDC->TextOut(m_rectTip.left+3, m_rectTip.bottom-16, sInfo);
				pDC->SelectObject(pOldFont);
			}
		}
		break;
	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	if(m_bIsEditing)
		pDC->SetROP2(olDrop2);	//恢复原来绘图方式 
}

int CDrawRect::GetHandleCount()
{
	ASSERT_VALID(this);

	if(m_nShape == RECT_SHAPE_LINE)
		return 2;
	else if(m_nShape == RECT_SHAPE_SPOT)
		return 1;

	return CDrawObj::GetHandleCount() + (m_nShape == RECT_SHAPE_ROUND_RECTANGLE);
}

// returns center of handle
CPoint CDrawRect::GetHandlePoint(int nHandleIndex)
{
	ASSERT_VALID(this);

	CRect rectObjPosition = m_rectPosition;

	if (m_nShape == RECT_SHAPE_LINE)
	{
		if(nHandleIndex == 1)
			return rectObjPosition.TopLeft();
		else
			return rectObjPosition.BottomRight();
	}
	else if (m_nShape == RECT_SHAPE_SPOT)
	{
		return rectObjPosition.CenterPoint();
	}
	else if (m_nShape == RECT_SHAPE_ROUND_RECTANGLE && nHandleIndex == 9)
	{
		rectObjPosition.NormalizeRect();
		CPoint point = rectObjPosition.BottomRight();
		point.x -= m_ptRoundness.x / 2;
		point.y -= m_ptRoundness.y / 2;

		return point;
	}

	return CDrawObj::GetHandlePoint(nHandleIndex);
}

HCURSOR CDrawRect::GetHandleCursor(int nHandleIndex)
{
	ASSERT_VALID(this);

	if (m_nShape == RECT_SHAPE_LINE && nHandleIndex == 2)
		nHandleIndex = 5;
	if(m_nShape == RECT_SHAPE_SPOT)
		return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
	else if (m_nShape == RECT_SHAPE_ROUND_RECTANGLE && nHandleIndex == 9)
		return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

	return CDrawObj::GetHandleCursor(nHandleIndex);
}

void CDrawRect::MoveHandleTo(int nHandleIndex, CPoint point, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);

	CPoint ptLocal = point;

	if (m_nShape == RECT_SHAPE_SPOT)
	{
		int nHeight, nWidth;
		nHeight = m_rectPosition.Height();
		nWidth = m_rectPosition.Width();
		m_rectPosition.left = ptLocal.x - nWidth/2;
		m_rectPosition.top = ptLocal.y - nHeight/2;

		// 避免宽高因除法取整误差而改变
		m_rectPosition.right = m_rectPosition.left + nWidth;
		m_rectPosition.bottom = m_rectPosition.top + nHeight;

		if (pVideoWnd == NULL)
			Invalidate();
		else
			pVideoWnd->InvalidateObj(this);

		pVideoWnd->SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, this);
		return;
	}

	if (m_nShape == RECT_SHAPE_LINE && nHandleIndex == 2)
		nHandleIndex = 5;
	else if (m_nShape == RECT_SHAPE_ROUND_RECTANGLE && nHandleIndex == 9)
	{
		CRect rect = m_rectPosition;
		rect.NormalizeRect();
		if (ptLocal.x > rect.right - 1)
			ptLocal.x = rect.right - 1;
		else if (ptLocal.x < rect.left + rect.Width() / 2)
			ptLocal.x = rect.left + rect.Width() / 2;
		if (ptLocal.y > rect.bottom - 1)
			ptLocal.y = rect.bottom - 1;
		else if (ptLocal.y < rect.top + rect.Height() / 2)
			ptLocal.y = rect.top + rect.Height() / 2;
		m_ptRoundness.x = 2 * (rect.right - ptLocal.x);
		m_ptRoundness.y = 2 * (rect.bottom - ptLocal.y);
		if (pVideoWnd == NULL)
			Invalidate();
		else
			pVideoWnd->InvalidateObj(this);

		pVideoWnd->SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, this);
		return;
	}

	CDrawObj::MoveHandleTo(nHandleIndex, point, pVideoWnd);
}

BOOL CDrawRect::Intersects(const CRect& rect)
{
	ASSERT_VALID(this);

	CRect rectT = rect;
	rectT.NormalizeRect();

	CRect rectPos = m_rectPosition;
	rectPos.NormalizeRect();
	CRect rectTip = m_rectTip;
	rectTip.NormalizeRect();
	if ((rectT & rectPos).IsRectEmpty() && (rectT & rectTip).IsRectEmpty())
		return FALSE;

	CRgn rgn;
	switch (m_nShape)
	{
	case RECT_SHAPE_RECTANGLE:
		return TRUE;

	case RECT_SHAPE_ROUND_RECTANGLE:
		rgn.CreateRoundRectRgn(rectPos.left, rectPos.top, rectPos.right, rectPos.bottom,
			m_ptRoundness.x, m_ptRoundness.y);
		break;

	case RECT_SHAPE_ELLIPSE:
		rgn.CreateEllipticRgnIndirect(rectPos);
		break;

	case RECT_SHAPE_SPOT:
		return TRUE;

	case RECT_SHAPE_LINE:
		{
			int x = (m_logpen.lopnWidth.x + 10) / 2;
			int y = (m_logpen.lopnWidth.y + 10) / 2;
			POINT points[4];
			CPoint ptFrom, ptTo;
			CRect rectObjPosition = m_rectPosition;
			ptFrom = rectObjPosition.TopLeft();
			ptTo = rectObjPosition.BottomRight();

			points[0].x = ptFrom.x;
			points[0].y = ptFrom.y;
			points[1].x = ptFrom.x;
			points[1].y = ptFrom.y;
			points[2].x = ptTo.x;
			points[2].y = ptTo.y;
			points[3].x = ptTo.x;
			points[3].y = ptTo.y;

			if (ptFrom.x < ptTo.x)
			{
				points[0].x -= x;
				points[1].x += x;
				points[2].x += x;
				points[3].x -= x;
			}
			else
			{
				points[0].x += x;
				points[1].x -= x;
				points[2].x -= x;
				points[3].x += x;
			}

			if (ptFrom.y < ptTo.y)
			{
				points[0].y -= y;
				points[1].y += y;
				points[2].y += y;
				points[3].y -= y;
			}
			else
			{
				points[0].y += y;
				points[1].y -= y;
				points[2].y -= y;
				points[3].y += y;
			}

			CRgn rgn1, rgn2;
			rgn.CreatePolygonRgn(points, 4, ALTERNATE);
			if (m_bShowTip)
			{
				rgn1.CreatePolygonRgn(points, 4, ALTERNATE);
				rgn2.CreateRectRgn(rectTip.left, rectTip.top, rectTip.right, rectTip.bottom);
				rgn.CombineRgn(&rgn1, &rgn2, RGN_OR);
			}
		}
		break;
	}
	return rgn.RectInRegion(rectT);
}

// 根据m_rectPosition更新m_rectTip
void CDrawRect::UpdateTipPosition(CVideoDirectShowWnd* pVideoWnd)
{
	if(!m_bShowTip || pVideoWnd==NULL)
		return;
	ASSERT_VALID(this);

	CRect rectPos = m_rectPosition;
	CRect rectClient;
	pVideoWnd->GetClientRect(rectClient);
	rectPos.NormalizeRect();
	rectClient.NormalizeRect();

	if(rectClient.IsRectEmpty() || rectPos.IsRectEmpty())
		return;

	int nWidth, nHeight;
	CString sInfo;
	CSize sz;
	TEXTMETRIC tm;
	CDC* pDC = pVideoWnd->GetDC();
	CFont* pOldFont = pDC->SelectObject(&globalData.fontRegular);
	pDC->GetTextMetrics(&tm);					// Get the text metrics for avg char width

	if(m_nShape == CDrawRect::RECT_SHAPE_SPOT)
	{
		if(m_sTip.GetLength() > 0)
			sInfo = m_sTip;
		else
			sInfo.Format(_T("点温%d：%.1f ℃"), m_nIndex+1, m_faValues[0]);
		sz = pDC->GetTextExtent(sInfo);
		sz.cx = sz.cx + tm.tmAveCharWidth +3;		// Add the avg width to prevent clipping
		nWidth = sz.cx; nHeight = 20;

		CPoint point = rectPos.CenterPoint();

		if (point.y+nHeight+6 > rectClient.bottom)	// 默认在point的右下方
			m_rectTip.top = point.y - nHeight - 6;
		else
			m_rectTip.top = point.y + 6;
		m_rectTip.bottom = m_rectTip.top + nHeight;

		if(point.x+nWidth+6+PALETTE_DEFAULT_WIDTH > rectClient.right)
		{
			m_rectTip.right = point.x - 6;
			m_rectTip.left = m_rectTip.right - nWidth;
		}
		else
		{
			m_rectTip.left = point.x + 6;
			m_rectTip.right = m_rectTip.left + nWidth;
		}
	}
	else if (m_nShape == CDrawRect::RECT_SHAPE_LINE)
	{
		rectPos = m_rectPosition;					// 非normalized
		CPoint point = rectPos.BottomRight();

		if(m_sTip.GetLength() > 0)
			sInfo = m_sTip;
		else
			sInfo.Format(_T("线温%d：最高%.1f，最低%.1f，平均%.1f (℃)"),
						m_nIndex+1, m_faValues[0],m_faValues[1],m_faValues[2]);
		sz = pDC->GetTextExtent(sInfo);
		sz.cx = sz.cx + tm.tmAveCharWidth +3;		// Add the avg width to prevent clipping
		nWidth = sz.cx; nHeight = 20;

		if (point.y+nHeight+6 > rectClient.bottom)	// 默认在point的右下方
			m_rectTip.top = point.y - nHeight - 6;
		else
			m_rectTip.top = point.y + 6;
		m_rectTip.bottom = m_rectTip.top + nHeight;

		if(point.x+nWidth+6+PALETTE_DEFAULT_WIDTH > rectClient.right)
		{
			m_rectTip.right = point.x - 6;
			m_rectTip.left = m_rectTip.right - nWidth;
		}
		else
		{
			m_rectTip.left = point.x + 6;
			m_rectTip.right = m_rectTip.left + nWidth;
		}
	}
	else if (m_nShape == CDrawRect::RECT_SHAPE_RECTANGLE)
	{
		if(m_sTip.GetLength() > 0)
			sInfo = m_sTip;
		else
			sInfo.Format(_T("区温%d：最高%.1f，最低%.1f，平均%.1f (℃)"),
							m_nIndex+1, m_faValues[0],m_faValues[1],m_faValues[2]);
		sz = pDC->GetTextExtent(sInfo);
		sz.cx = sz.cx + tm.tmAveCharWidth +3;		// Add the avg width to prevent clipping
		nWidth = sz.cx; nHeight = 20;

		if (rectPos.top-nHeight < rectClient.top)	// 默认在rectPos的左上角
			m_rectTip.top = rectPos.bottom;
		else
			m_rectTip.top = rectPos.top - nHeight;
		m_rectTip.bottom = m_rectTip.top + nHeight;

		if(rectPos.left+nWidth+PALETTE_DEFAULT_WIDTH > rectClient.right)
		{
			m_rectTip.right = rectPos.right;
			m_rectTip.left = m_rectTip.right - nWidth;
		}
		else
		{
			m_rectTip.left = rectPos.left;
			m_rectTip.right = m_rectTip.left + nWidth;
		}
	}

	pDC->SelectObject(pOldFont);
	pVideoWnd->ReleaseDC(pDC);
}

////////////////////////////////////////////////////////////////////////////
// CDrawPoly


CDrawPoly::CDrawPoly()
{
	m_points = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;
}

CDrawPoly::CDrawPoly(const CRect& position)
	: CDrawObj(position)
{
	m_points       = NULL;
	m_nPoints      = 0;
	m_nAllocPoints = 0;

	m_bPen         = TRUE;
	m_bBrush       = FALSE;
}

CDrawPoly::~CDrawPoly()
{
	if (m_points != NULL)
		delete[] m_points;
}

void CDrawPoly::Draw(CDC* pDC)
{
	ASSERT_VALID(this);

	CBrush brush;
	if (!brush.CreateBrushIndirect(&m_logbrush))
		return;
	CPen pen;
	if (!pen.CreatePenIndirect(&m_logpen))
		return;

	CBrush* pOldBrush;
	CPen* pOldPen;

	if (m_bBrush)
		pOldBrush = pDC->SelectObject(&brush);
	else
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

	if (m_bPen)
		pOldPen = pDC->SelectObject(&pen);
	else
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

	pDC->Polygon(m_points, m_nPoints);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CDrawPoly::MoveTo(const CRect& position, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);

	CRect rectObjPosition = m_rectPosition;

	if (position == rectObjPosition)
		return;

	if (pVideoWnd == NULL)
		Invalidate();
	else
		pVideoWnd->InvalidateObj(this);

	for (int i = 0; i < m_nPoints; i++)
	{
		m_points[i].x += position.left - rectObjPosition.left;
		m_points[i].y += position.top - rectObjPosition.top;
	}

	m_rectPosition = position;

	if (pVideoWnd == NULL)
		Invalidate();
	else
		pVideoWnd->InvalidateObj(this);

	pVideoWnd->SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, this);
}

int CDrawPoly::GetHandleCount()
{
	return m_nPoints;
}

CPoint CDrawPoly::GetHandlePoint(int nHandleIndex)
{
	ASSERT_VALID(this);

	ASSERT(nHandleIndex >= 1 && nHandleIndex <= m_nPoints);
	return m_points[nHandleIndex - 1];
}

HCURSOR CDrawPoly::GetHandleCursor(int )
{
	return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

void CDrawPoly::MoveHandleTo(int nHandleIndex, CPoint point, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);
	ASSERT(nHandleIndex >= 1 && nHandleIndex <= m_nPoints);
	if (m_points[nHandleIndex - 1] == point)
		return;

	m_points[nHandleIndex - 1] = point;
	RecalcBounds(pVideoWnd);

	if (pVideoWnd == NULL)
		Invalidate();
	else
		pVideoWnd->InvalidateObj(this);

	pVideoWnd->SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, this);
}

BOOL CDrawPoly::Intersects(const CRect& rect)
{
	ASSERT_VALID(this);
	CRgn rgn;
	rgn.CreatePolygonRgn(m_points, m_nPoints, ALTERNATE);
	return rgn.RectInRegion(rect);
}

void CDrawPoly::AddPoint(const CPoint& point, CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		if (m_points != NULL)
		{
#if _MSC_VER < 1400
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
#else
			memcpy_s(newPoints, sizeof(CPoint) * (m_nAllocPoints + 10), m_points, sizeof(CPoint) * m_nAllocPoints);
#endif
			delete[] m_points;
		}
		m_points = newPoints;
		m_nAllocPoints += 10;
	}

	if (m_nPoints == 0 || m_points[m_nPoints - 1] != point)
	{
		m_points[m_nPoints++] = point;
		if (!RecalcBounds(pVideoWnd))
		{
			if (pVideoWnd == NULL)
				Invalidate();
			else
				pVideoWnd->InvalidateObj(this);
		}

		pVideoWnd->SetObjModifiedFlag(OBJ_MODIFIED_FLAG_MODIFY, this);
	}
}

BOOL CDrawPoly::RecalcBounds(CVideoDirectShowWnd* pVideoWnd)
{
	ASSERT_VALID(this);

	if (m_nPoints == 0)
		return FALSE;

	CRect bounds(m_points[0], CSize(0, 0));
	for (int i = 1; i < m_nPoints; ++i)
	{
		if (m_points[i].x < bounds.left)
			bounds.left = m_points[i].x;
		if (m_points[i].x > bounds.right)
			bounds.right = m_points[i].x;
		if (m_points[i].y < bounds.top)
			bounds.top = m_points[i].y;
		if (m_points[i].y > bounds.bottom)
			bounds.bottom = m_points[i].y;
	}

	if (bounds == m_rectPosition)
		return FALSE;

	if (pVideoWnd == NULL)
		Invalidate();
	else
		pVideoWnd->InvalidateObj(this);

	m_rectPosition = bounds;

	if (pVideoWnd == NULL)
		Invalidate();
	else
		pVideoWnd->InvalidateObj(this);

	return TRUE;
}
