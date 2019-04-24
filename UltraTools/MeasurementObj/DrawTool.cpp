// drawtool.cpp - implementation for drawing tools

#include "stdafx.h"
#include "../DirectShow/VideoDirectShowWnd.h"
#include "DrawObj.h"
#include "DrawTool.h"

#ifdef TSMO_SYS
	#include "SingleVideoWnd.h"
#else ifdef IR_IMAGE_ANALYSI_SYS
	#include "../AnalysisView.h"
#endif


/////////////////////////////////////////////////////////////////////////////
// CDrawTool implementation

CPtrList CDrawTool::c_ptrListDrawTools;

static CSelectTool selectTool;
static CRectTool spotTool(DRAW_OBJ_SHAPE_SPOT);
static CRectTool lineTool(DRAW_OBJ_SHAPE_LINE);
static CRectTool rectTool(DRAW_OBJ_SHAPE_RECT);
static CRectTool roundRectTool(DRAW_OBJ_SHAPE_ROUND_RECT);
static CRectTool ellipseTool(DRAW_OBJ_SHAPE_ELLIPSE);
static CPolyTool polyTool;

CPoint CDrawTool::c_pointDown;
UINT CDrawTool::c_nDownFlags;
CPoint CDrawTool::c_pointLast;
DRAW_OBJ_SHAPE CDrawTool::c_drawShape = DRAW_OBJ_SHAPE_SELECTION;
int CDrawTool::c_nDragHandle;

CDrawTool::CDrawTool(DRAW_OBJ_SHAPE drawShape)
{
	m_drawShape = drawShape;
	c_ptrListDrawTools.AddTail(this);
	c_nDragHandle = 1;
}

CDrawTool* CDrawTool::FindTool(DRAW_OBJ_SHAPE drawShape)
{
	POSITION pos = c_ptrListDrawTools.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawTool* pTool = (CDrawTool*)c_ptrListDrawTools.GetNext(pos);
		if (pTool->m_drawShape == drawShape)
			return pTool;
	}

	return NULL;
}

void CDrawTool::OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	pVideoWnd->SetCapture();
	c_nDownFlags = nFlags;
	c_pointDown = point;
	c_pointLast = point;
}

void CDrawTool::OnLButtonDblClk(CVideoDirectShowWnd* /*pVideoWnd*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
}

void CDrawTool::OnLButtonUp(CVideoDirectShowWnd* /*pVideoWnd*/, UINT /*nFlags*/, const CPoint& point)
{
	ReleaseCapture();

	if (point == c_pointDown)
		c_drawShape = DRAW_OBJ_SHAPE_SELECTION;
}

void CDrawTool::OnMouseMove(CVideoDirectShowWnd* /*pVideoWnd*/, UINT /*nFlags*/, const CPoint& point)
{
	c_pointLast = point;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CDrawTool::OnCancel()
{
	c_drawShape = DRAW_OBJ_SHAPE_SELECTION;
}

////////////////////////////////////////////////////////////////////////////
// CResizeTool

enum SELECT_MODE
{
	SELECT_MODE_NONE,
	SELECT_MODE_NET_SELECT,
	SELECT_MODE_MOVE,
	SELECT_MODE_SIZE
};

SELECT_MODE selectMode = SELECT_MODE_NONE;

CPoint lastPoint;

CSelectTool::CSelectTool()
	: CDrawTool(DRAW_OBJ_SHAPE_SELECTION)
{
}

void CSelectTool::OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	CPoint local = point;

	CDrawObj* pObj;
	selectMode = SELECT_MODE_NONE;

	// Check for resizing (only allowed on single selections)
	if (pVideoWnd->m_objListSelection.GetCount() == 1)
	{
		pObj = pVideoWnd->m_objListSelection.GetHead();
		if(NULL != pObj)
		{
			c_nDragHandle = pObj->HitTest(local, pVideoWnd, TRUE);
			if (c_nDragHandle != 0)
				selectMode = SELECT_MODE_SIZE;
		}
	}

	// See if the click was on an object, select and start move if so
	if (selectMode == SELECT_MODE_NONE)
	{
		pObj = pVideoWnd->ObjectAt(local);

		if (pObj != NULL)
		{
			selectMode = SELECT_MODE_MOVE;

			if (!pVideoWnd->IsObjSelected(pObj))
				pVideoWnd->SelectObj(pObj, (nFlags & MK_SHIFT) != 0);
		}
	}

	// Click on background, start a net-selection
	if (selectMode == SELECT_MODE_NONE)
	{
		if ((nFlags & MK_SHIFT) == 0)
			pVideoWnd->SelectObj(NULL);

		selectMode = SELECT_MODE_NET_SELECT;

		CClientDC dc(pVideoWnd);
		CRect rect(point.x, point.y, point.x, point.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);
	}

	lastPoint = local;
	CDrawTool::OnLButtonDown(pVideoWnd, nFlags, point);
}

void CSelectTool::OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	if ((nFlags & MK_SHIFT) != 0)
	{
		// Shift+DblClk deselects object...
		CPoint local = point;
		CDrawObj* pObj = pVideoWnd->ObjectAt(local);
		if (pObj != NULL)
			pVideoWnd->DeselectObj(pObj);
	}
	else
	{
		// 双击修改属性
// 		if (pVideoWnd->m_objListSelection.GetCount() == 1)
// 			pVideoWnd->m_objListSelection.GetHead()->OnOpen(pVideoWnd);
	}

	CDrawTool::OnLButtonDblClk(pVideoWnd, nFlags, point);
}

void CSelectTool::OnLButtonUp(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	if (pVideoWnd->GetCapture() == pVideoWnd)
	{
		if (selectMode == SELECT_MODE_NET_SELECT)
		{
			CClientDC dc(pVideoWnd);
			CRect rect(c_pointDown.x, c_pointDown.y, c_pointLast.x, c_pointLast.y);
			rect.NormalizeRect();
			dc.DrawFocusRect(rect);

			pVideoWnd->SelectObjWithinRect(rect, TRUE);
		}
//  	else if (selectMode != SELECT_MODE_NONE)// BFish del the two lines 2010.07.21	
//  		pVideoWnd->Invalidate(FALSE);
	}

	// 取消编辑标记，以便采纳编辑结果(编辑过程中的临时位置不采纳).BFish add 2010.08.08
	POSITION pos = pVideoWnd->m_objListSelection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = pVideoWnd->m_objListSelection.GetNext(pos);
		CRect position = pObj->m_rectPosition;

		pObj->m_bIsEditing = FALSE;				// 取消编辑状态
		pObj->MoveTo(position, pVideoWnd);		// 激活采纳

		// 确定tipRect的大小和位置
		if (!pObj->m_bShowTip)
			continue;
		pObj->UpdateTipPosition(pVideoWnd);
	}

	// BFish add this line 2010.07.21
	// pVideoWnd为Dialog/View的child之后，已无任何作用，需invalidate其父.
//	pVideoWnd->Invalidate(FALSE);
	pVideoWnd->m_pOwner->Invalidate(FALSE);

	CDrawTool::OnLButtonUp(pVideoWnd, nFlags, point);
}

void CSelectTool::OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	CPoint local = point;

	if (pVideoWnd->GetCapture() != pVideoWnd)
	{
		if (c_drawShape == DRAW_OBJ_SHAPE_SELECTION && pVideoWnd->m_objListSelection.GetCount() == 1)
		{
			CDrawObj* pObj = pVideoWnd->m_objListSelection.GetHead();
			int nHandleIndex = pObj->HitTest(local, pVideoWnd, TRUE);	//point是否在handles中
			if (nHandleIndex != 0)
			{
				SetCursor(pObj->GetHandleCursor(nHandleIndex));
				return; // bypass CDrawTool
			}
		}

		if (c_drawShape == DRAW_OBJ_SHAPE_SELECTION && pVideoWnd->m_objListSelection.GetCount() > 0)
		{
			POSITION pos = pVideoWnd->m_objListSelection.GetHeadPosition();
			CDrawObj* pObj = NULL;
			int nHandleIndex = 0;
			while (pos != NULL)
			{
				pObj = pVideoWnd->m_objListSelection.GetNext(pos);
				if(NULL != pObj)
					nHandleIndex = pObj->HitTest(local, pVideoWnd, FALSE);	//point是否在rect中
				if (nHandleIndex != 0)
				{
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
					return; // bypass CDrawTool
				}
			}
		}

		if (c_drawShape == DRAW_OBJ_SHAPE_SELECTION)
			CDrawTool::OnMouseMove(pVideoWnd, nFlags, point);

		return;
	}

	if (selectMode == SELECT_MODE_NET_SELECT)
	{
		CClientDC dc(pVideoWnd);
		CRect rect(c_pointDown.x, c_pointDown.y, c_pointLast.x, c_pointLast.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);
		rect.SetRect(c_pointDown.x, c_pointDown.y, point.x, point.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);

		CDrawTool::OnMouseMove(pVideoWnd, nFlags, point);
		return;
	}

	local = point;
	CPoint delta = (CPoint)(local - lastPoint);

	POSITION pos = pVideoWnd->m_objListSelection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = pVideoWnd->m_objListSelection.GetNext(pos);
		CRect position = pObj->m_rectPosition;

		if (selectMode == SELECT_MODE_MOVE)
		{
			pObj->m_bIsEditing = TRUE;

			position += delta;
			pObj->MoveTo(position, pVideoWnd);
		}
		else if (c_nDragHandle != 0)
		{
			pObj->m_bIsEditing = TRUE;

			pObj->MoveHandleTo(c_nDragHandle, local, pVideoWnd);
		}
	}

	lastPoint = local;

	if (selectMode == SELECT_MODE_SIZE && c_drawShape == DRAW_OBJ_SHAPE_SELECTION)
	{
		c_pointLast = point;
		SetCursor(pVideoWnd->m_objListSelection.GetHead()->GetHandleCursor(c_nDragHandle));
		return; // bypass CDrawTool
	}

	c_pointLast = point;

	if (c_drawShape == DRAW_OBJ_SHAPE_SELECTION)
		CDrawTool::OnMouseMove(pVideoWnd, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CRectTool (does rectangles, round-rectangles, and ellipses)

CRectTool::CRectTool(DRAW_OBJ_SHAPE drawShape)
	: CDrawTool(drawShape)
{
}

void CRectTool::OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	CDrawTool::OnLButtonDown(pVideoWnd, nFlags, point);

	CPoint local = point;

	CDrawRect* pObj = new CDrawRect(CRect(local, CSize(0, 0)));
	if(m_drawShape == DRAW_OBJ_SHAPE_SPOT)
	{
		CRect rect;
		rect.left = local.x - pObj->m_nSpotSize/2;
		rect.top = local.y - pObj->m_nSpotSize/2;
		rect.right = local.x + pObj->m_nSpotSize/2;
		rect.bottom = local.y + pObj->m_nSpotSize/2;
		pObj->m_rectPosition = rect;
	}

	switch (m_drawShape)
	{
	default:
		ASSERT(FALSE); // 不支持的drawShape!

	case DRAW_OBJ_SHAPE_RECT:
		pObj->m_nShape = CDrawRect::RECT_SHAPE_RECTANGLE;
		break;

	case DRAW_OBJ_SHAPE_ROUND_RECT:
		pObj->m_nShape = CDrawRect::RECT_SHAPE_ROUND_RECTANGLE;
		break;

	case DRAW_OBJ_SHAPE_ELLIPSE:
		pObj->m_nShape = CDrawRect::RECT_SHAPE_ELLIPSE;
		break;

	case DRAW_OBJ_SHAPE_LINE:
		pObj->m_nShape = CDrawRect::RECT_SHAPE_LINE;
		break;

	case DRAW_OBJ_SHAPE_SPOT:
		pObj->m_nShape = CDrawRect::RECT_SHAPE_SPOT;
		break;
	}
	pVideoWnd->AddObj(pObj);
	pVideoWnd->SelectObj(pObj);

	if (m_drawShape == DRAW_OBJ_SHAPE_SPOT)
		selectMode = SELECT_MODE_MOVE;
	else
		selectMode = SELECT_MODE_SIZE;
	c_nDragHandle = 1;
	lastPoint = local;
}

void CRectTool::OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	CDrawTool::OnLButtonDblClk(pVideoWnd, nFlags, point);
}

void CRectTool::OnLButtonUp(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	if(m_drawShape == DRAW_OBJ_SHAPE_SPOT)
	{
		selectTool.OnLButtonUp(pVideoWnd, nFlags, point);
		return;
	}

	if (point == c_pointDown)
	{
		// 拒绝建立empty objects，删除之...
		CDrawObj *pObj = pVideoWnd->m_objListSelection.GetTail();
		pVideoWnd->RemoveObj(pObj);

		selectTool.OnLButtonDown(pVideoWnd, nFlags, point); // try a select!
	}

	selectTool.OnLButtonUp(pVideoWnd, nFlags, point);
}

void CRectTool::OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
//	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
	selectTool.OnMouseMove(pVideoWnd, nFlags, point);
}


////////////////////////////////////////////////////////////////////////////
// CPolyTool

CPolyTool::CPolyTool()
	: CDrawTool(DRAW_OBJ_SHAPE_POLY)
{
	m_pDrawObj = NULL;
}

void CPolyTool::OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	CDrawTool::OnLButtonDown(pVideoWnd, nFlags, point);

	CPoint local = point;

	if (m_pDrawObj == NULL)
	{
		pVideoWnd->SetCapture();

		m_pDrawObj = new CDrawPoly(CRect(local, CSize(0, 0)));
		pVideoWnd->AddObj(m_pDrawObj);
		pVideoWnd->SelectObj(m_pDrawObj);
		m_pDrawObj->AddPoint(local, pVideoWnd);
	}
	else if (local == m_pDrawObj->m_points[0])
	{
		// Stop when the first point is repeated...
		ReleaseCapture();
		m_pDrawObj->m_nPoints -= 1;
		if (m_pDrawObj->m_nPoints < 2)
		{
			m_pDrawObj->Remove();
		}
		else
		{
			pVideoWnd->InvalidateObj(m_pDrawObj);
		}
		m_pDrawObj = NULL;
		c_drawShape = DRAW_OBJ_SHAPE_SELECTION;
		return;
	}

	local.x += 1; // adjacent points can't be the same!
	m_pDrawObj->AddPoint(local, pVideoWnd);

	selectMode = SELECT_MODE_SIZE;
	c_nDragHandle = m_pDrawObj->GetHandleCount();
	lastPoint = local;
}

void CPolyTool::OnLButtonUp(CVideoDirectShowWnd* /*pVideoWnd*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
	// Don't release capture yet!
}

void CPolyTool::OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point)
{
	if (m_pDrawObj != NULL && (nFlags & MK_LBUTTON) != 0)
	{
		CPoint local = point;
		m_pDrawObj->AddPoint(local);
		c_nDragHandle = m_pDrawObj->GetHandleCount();
		lastPoint = local;
		c_pointLast = point;
		SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
	}
	else
	{
//		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
		SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
		selectTool.OnMouseMove(pVideoWnd, nFlags, point);
	}
}

void CPolyTool::OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT , const CPoint& )
{
	ReleaseCapture();

	int nPoints = m_pDrawObj->m_nPoints;
	if (nPoints > 2 &&
		(m_pDrawObj->m_points[nPoints - 1] == m_pDrawObj->m_points[nPoints - 2] ||
		m_pDrawObj->m_points[nPoints - 1].x - 1 == m_pDrawObj->m_points[nPoints - 2].x &&
		m_pDrawObj->m_points[nPoints - 1].y == m_pDrawObj->m_points[nPoints - 2].y))
	{
		// Nuke the last point if it's the same as the next to last...
		m_pDrawObj->m_nPoints -= 1;
		pVideoWnd->InvalidateObj(m_pDrawObj);
	}

	m_pDrawObj = NULL;
	c_drawShape = DRAW_OBJ_SHAPE_SELECTION;
}

void CPolyTool::OnCancel()
{
	CDrawTool::OnCancel();

	m_pDrawObj = NULL;
}

/////////////////////////////////////////////////////////////////////////////
