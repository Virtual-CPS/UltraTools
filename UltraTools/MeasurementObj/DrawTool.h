// drawtool.h - interface for CDrawTool and derivatives
//

#ifndef __DRAWTOOL_H__
#define __DRAWTOOL_H__

#include "drawobj.h"

class CVideoDirectShowWnd;

enum DRAW_OBJ_SHAPE
{
	DRAW_OBJ_SHAPE_SELECTION,
	DRAW_OBJ_SHAPE_SPOT,
	DRAW_OBJ_SHAPE_LINE,
	DRAW_OBJ_SHAPE_RECT,
	DRAW_OBJ_SHAPE_ROUND_RECT,
	DRAW_OBJ_SHAPE_ELLIPSE,
	DRAW_OBJ_SHAPE_POLY
};

class CDrawTool
{
// Constructors
public:
	CDrawTool(DRAW_OBJ_SHAPE nDrawShape);

// Overridables
	virtual void OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnCancel();

// Attributes
	DRAW_OBJ_SHAPE m_drawShape;

	static CDrawTool* FindTool(DRAW_OBJ_SHAPE drawShape);
	static CPtrList c_ptrListDrawTools;
	static CPoint c_pointDown;
	static UINT c_nDownFlags;
	static CPoint c_pointLast;
	static DRAW_OBJ_SHAPE c_drawShape;
	static int c_nDragHandle;
};

class CSelectTool : public CDrawTool
{
// Constructors
public:
	CSelectTool();

// Implementation
	virtual void OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
};

class CRectTool : public CDrawTool
{
// Constructors
public:
	CRectTool(DRAW_OBJ_SHAPE drawShape);

// Implementation
	virtual void OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
};

class CPolyTool : public CDrawTool
{
// Constructors
public:
	CPolyTool();

// Implementation
	virtual void OnLButtonDown(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CVideoDirectShowWnd* pVideoWnd, UINT nFlags, const CPoint& point);
	virtual void OnCancel();
private:
	CDrawPoly* m_pDrawObj;
};

////////////////////////////////////////////////////////////////////////////

#endif // __DRAWTOOL_H__
