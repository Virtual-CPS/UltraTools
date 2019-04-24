// drawobj.h - interface for CDrawObj and derivatives

#ifndef __DRAWOBJ_H__
#define __DRAWOBJ_H__

class CVideoDirectShowWnd;

/////////////////////////////////////////////////////////////////////////////
// CDrawObj - base class for all 'drawable objects'

class CDrawObj : public CObject
{
protected:
	CDrawObj();

// Constructors
public:
	CDrawObj(const CRect& position);

// Attributes
	CRect					m_rectPosition;
	CRect					m_rectTip;
	CVideoDirectShowWnd*	m_pVideoWnd;
	BOOL					m_bIsEditing;	//在OnMouseMove()中置TRUE，在OnLBtnUp()中置FALSE
	BOOL					m_bShowIndex;
	BOOL					m_bShowTip;		// 显示rectTip
	int						m_nIndex;
	float					m_faValues[3];	// max/min/avg
	CString					m_sTip;			// 为空时显示温度信息，否则显示m_sTip内容

	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandleIndex);
	CRect GetHandleRect(int nHandleID, CVideoDirectShowWnd* pVideoWnd);
	virtual HCURSOR GetHandleCursor(int nHandleIndex);
	virtual void SetLineColor(COLORREF color);
	virtual void SetFillColor(COLORREF color);

// Operations
	virtual void Draw(CDC* pDC);
	enum TrackerState { normal, selected, active };
	virtual void DrawTracker(CDC* pDC, TrackerState state);
	virtual void MoveTo(const CRect& positon, CVideoDirectShowWnd* pVideoWnd = NULL);
	virtual int HitTest(CPoint point, CVideoDirectShowWnd* pVideoWnd, BOOL bSelected);
	virtual BOOL Intersects(const CRect& rect);
	virtual void MoveHandleTo(int nHandleIndex, CPoint point, CVideoDirectShowWnd* pVideoWnd = NULL);
	virtual void Remove();
	void Invalidate();
	virtual void UpdateTipPosition(CVideoDirectShowWnd* pVideoWnd = NULL);	// 根据m_rectPosition更新m_rectTip

	COLORREF GetLineColor () const
	{
		return m_logpen.lopnColor;
	}

	int GetLineWeight () const
	{
		return m_logpen.lopnWidth.x;
	}

	void SetLineWeight (int nWidth);

	COLORREF GetFillColor () const
	{
		return m_logbrush.lbColor;
	}

	BOOL IsEnableFill () const
	{
		return m_bBrush;
	}

	void EnableFill (BOOL bEnable=TRUE);

	BOOL IsEnableLine () const
	{
		return m_bPen;
	}

	void EnableLine (BOOL bEnable=TRUE);

	BOOL IsEnableShowTip() const
	{
		return m_bShowTip;
	}

	void EnableShowTip(BOOL bEnable=TRUE);

	void SetTipString(CString sTip)
	{
		m_sTip = sTip;
	}
	void DelTipString()
	{
		m_sTip = "";
	}

	virtual BOOL CanChangeFillColor () const
	{
		return TRUE;
	}
	
	virtual BOOL CanChangeLineColor () const
	{
		return TRUE;
	}
	
	virtual BOOL CanChangeLineWeight () const
	{
		return TRUE;
	}

// Implementation
public:
	virtual ~CDrawObj();
#ifdef _DEBUG
	void AssertValid();
#endif

	// implementation data
protected:
	BOOL		m_bPen;
	LOGPEN		m_logpen;
	BOOL		m_bBrush;
	LOGBRUSH	m_logbrush;
};

// special 'list' class for this application (requires afxtempl.h)
typedef CTypedPtrList<CObList, CDrawObj*> CDrawObjList;

////////////////////////////////////////////////////////////////////////
// specialized draw objects

class CDrawRect : public CDrawObj
{
protected:
	CDrawRect();

public:
	CDrawRect(const CRect& position);

	enum Shape
	{
		RECT_SHAPE_RECTANGLE,
		RECT_SHAPE_ROUND_RECTANGLE,
		RECT_SHAPE_ELLIPSE,
		RECT_SHAPE_LINE,
		RECT_SHAPE_SPOT
	};

// Implementation
public:
	virtual void Draw(CDC* pDC);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandleIndex);
	virtual HCURSOR GetHandleCursor(int nHandleIndex);
	virtual void MoveHandleTo(int nHandleIndex, CPoint point, CVideoDirectShowWnd* pVideoWnd = NULL);
	virtual BOOL Intersects(const CRect& rect);
	virtual void UpdateTipPosition(CVideoDirectShowWnd* pVideoWnd = NULL);	// 根据m_rectPosition更新m_rectTip

	virtual BOOL CanChangeFillColor () const
	{
		return (m_nShape!=RECT_SHAPE_LINE && m_nShape!=RECT_SHAPE_SPOT);
	}

	Shape GetShapType(){return m_nShape;}

protected:
	Shape		m_nShape;

	CPoint		m_ptRoundness;	// for roundRect corners
	int			m_nSpotSize;	// RECT_SHAPE_SPOT时点的大小

	friend class CRectTool;
};

/////////////////////////////////////////////////////////////////////////////

class CDrawPoly;

class CDrawPoly : public CDrawObj
{
protected:
	CDrawPoly();

public:
	CDrawPoly(const CRect& position);

// Operations
	void AddPoint(const CPoint& point, CVideoDirectShowWnd* pVideoWnd = NULL);
	BOOL RecalcBounds(CVideoDirectShowWnd* pVideoWnd = NULL);

// Implementation
public:
	virtual ~CDrawPoly();
	virtual void Draw(CDC* pDC);
	virtual void MoveTo(const CRect& position, CVideoDirectShowWnd* pVideoWnd = NULL);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandleIndex);
	virtual HCURSOR GetHandleCursor(int nHandleIndex);
	virtual void MoveHandleTo(int nHandleIndex, CPoint point, CVideoDirectShowWnd* pVideoWnd = NULL);
	virtual BOOL Intersects(const CRect& rect);

protected:
	int m_nPoints;
	int m_nAllocPoints;
	CPoint* m_points;
	CDrawPoly* m_pDrawObj;

	friend class CPolyTool;
};

#endif // __DRAWOBJ_H__
