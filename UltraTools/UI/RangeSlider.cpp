// RangeSlider.cpp : implementation file
//

#include "stdafx.h"
#include "RangeSlider.h"
#include "../Utility/MemDC.h"

#include <math.h> // for fabs

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RANGESLIDER_CLASSNAME _T("MFCRangeSlider")

const UINT WM_RANGE_CHANGED = 
   ::RegisterWindowMessage(_T("WM_RANGE_CHANGED{2E7439A4-12DB-42a2-BB95-3462B3EC39CE}CRangeSlider"));
/////////////////////////////////////////////////////////////////////////////
// CRangeSlider

CRangeSlider::CRangeSlider()
{
	RegisterWindowClass();
	m_fMin = 0.0;
	m_fMax = 1.0;
	m_fLeft = 0.0;
	m_fRight = 1.0;
	m_bHorizontal = TRUE;
	m_nArrowWidth = 10;
	m_bTracking = FALSE;
	m_bVisualMinMax = TRUE;
	m_bInvertedMode = FALSE;
	m_crVisualColor = RGB(0, 255, 0);

	m_fVisualMin = m_fMin;
	m_fVisualMax = m_fMax;

	m_bRangeChanged = FALSE;
}

CRangeSlider::~CRangeSlider()
{
}

// Relay call to create
void CRangeSlider::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID, CCreateContext *pContext) {
	CWnd::Create(NULL, _T(""), dwStyle, rect, pParentWnd, nID, pContext);
}


// Register the window class, so you can use this class as a custom control.
BOOL CRangeSlider::RegisterWindowClass(void)
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, RANGESLIDER_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = NULL; // No Background brush (gives flicker)
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = RANGESLIDER_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CRangeSlider, CWnd)
	//{{AFX_MSG_MAP(CRangeSlider)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRangeSlider message handlers
void CRangeSlider::OnPaint() 
{
	CPaintDC RawDC(this); // device context for painting
	CMyMemDC dc(&RawDC);
	dc.SetMapMode(MM_TEXT);

	if (m_bHorizontal)
	{
		OnPaintHorizontal(dc);
	}
	else
	{ // Vertical Mode
		OnPaintVertical(dc);
	}
}

// Paint in Horizontal Mode.
void CRangeSlider::OnPaintHorizontal(CDC &dc)
{
	ASSERT(m_bHorizontal);

	// Calculate Arrow width and x-axis width.
	CRect ClientRect;
	GetClientRect(&ClientRect);

	int dy = ClientRect.Height();
	m_nArrowWidth = dy * 3 / 4;

	m_fx = ClientRect.Width() - 2 * m_nArrowWidth;

	CRect ClipBox;
	dc.GetClipBox(&ClipBox);

	// Erase Background.
	CGdiObject *pBr = dc.SelectStockObject(GRAY_BRUSH);
	CGdiObject *pPen = dc.SelectStockObject(BLACK_PEN);
	dc.Rectangle(&ClipBox);

	// Check if [Left, Right] is contained in [Min, Max].
	ASSERT(m_fMin <= m_fLeft);
	ASSERT(m_fLeft <= m_fRight);
	ASSERT(m_fRight <= m_fMax);

	// Position of Left and Right
	int x1 = static_cast<int>((m_fLeft - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);
	int x2 = static_cast<int>((m_fRight - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);
	dc.SelectStockObject(BLACK_PEN);
	CBrush ButtonFace;
	ButtonFace.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	dc.SelectObject(ButtonFace);

	// Draw Left Arrow
	m_RectLeft = CRect (CPoint(x1,0), CSize(m_nArrowWidth, dy));
	{ 
		CRect & rect = m_RectLeft;
		dc.Rectangle(&rect);
		rect.DeflateRect(1,1,2,2);
		// Make appearance 3D
		if (!m_bTracking || m_TrackMode!= TRACK_LEFT)
		{
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SelectObject(pen1);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
			dc.SelectObject(pen2);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
		}

		// Draw arrow
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		CPoint poly [] =
		{ 
			CPoint(x1 + m_nArrowWidth / 3, dy / 3), 
			CPoint(x1 + m_nArrowWidth / 3, dy*2/3),
			CPoint(x1 + m_nArrowWidth*2/3, dy / 2),
		};
		dc.Polygon(poly, 3);
	}

	// Draw Right Arrow
	m_RectRight = CRect(CPoint(m_nArrowWidth + x2, 0), CSize(m_nArrowWidth, dy));
	{
		CRect & rect = m_RectRight;
		dc.SelectObject(ButtonFace);
		dc.Rectangle(&rect);
		// Make appearance 3D
		if (!m_bTracking || m_TrackMode!= TRACK_RIGHT)
		{
			rect.DeflateRect(1,1,2,2);
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SelectObject(pen1);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
			dc.SelectObject(pen2);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
		}

		// Draw Arrow
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		CPoint poly [] =
		{ 
			CPoint(m_nArrowWidth + x2 + m_nArrowWidth * 2 / 3,    dy / 3),
			CPoint(m_nArrowWidth + x2 + m_nArrowWidth * 2 / 3,    dy * 2 / 3),
			CPoint(m_nArrowWidth + x2 + m_nArrowWidth / 3,        dy / 2),
		};
		dc.Polygon(poly, 3);
	}

	dc.SelectStockObject(WHITE_BRUSH);
	// Draw Area in between.
	CRect rect = CRect(CPoint(m_nArrowWidth + x1 + 1, 1), CPoint (m_nArrowWidth + x2, dy - 1));

	dc.SelectStockObject(NULL_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);

	// Store value for drawing the focus rectangle
	CRect focusRect = rect;

	// Draw Area of InnerMin, Max.
	if (m_bVisualMinMax)
	{
		ASSERT(m_fMin <= m_fVisualMax);
		ASSERT(m_fVisualMin <= m_fVisualMax);
		ASSERT(m_fVisualMax <= m_fMax);

		// See wether [InnerMin, InnerMax] geschnitten [Left, Right] is non-empty.
		// Then Draw
		if (m_fVisualMin < m_fRight && m_fVisualMax > m_fLeft)
		{
			int visualMin = static_cast<int>((m_fVisualMin - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);
			int visualMax = static_cast<int>((m_fVisualMax - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);

			// Stay in [m_fLeft, m_fRight] intervall.
			visualMin = max(visualMin + m_nArrowWidth, x1 + m_nArrowWidth);
			visualMax = min(visualMax + m_nArrowWidth, x2 + m_nArrowWidth);

			rect = CRect(visualMin, 1, visualMax, dy - 1);

			CBrush br;
			br.CreateSolidBrush(m_crVisualColor);
			dc.SelectObject(br);
			dc.SelectStockObject(NULL_PEN);
			dc.Rectangle(&rect);
			CPen pen;
			dc.SelectStockObject(HOLLOW_BRUSH);
		}
	}
	if (GetFocus() == this)
	{
		focusRect.DeflateRect(3,3);
		dc.DrawFocusRect(&focusRect);
	}
	dc.SelectObject(pBr);
	dc.SelectObject(pPen);
}

// Paint in Vertical Mode.
void CRangeSlider::OnPaintVertical(CDC &dc)
{
	ASSERT(!m_bHorizontal);
	CRect ClientRect;
	GetClientRect(&ClientRect);

	int dy = ClientRect.Width();
	m_nArrowWidth = dy * 3 / 4;

	m_fx = ClientRect.Height() - 2 * m_nArrowWidth;

	CRect ClipBox;
	dc.GetClipBox(&ClipBox);

	// Erase Background.
	CGdiObject *pBr  = dc.SelectStockObject(GRAY_BRUSH);
	CGdiObject *pPen = dc.SelectStockObject(BLACK_PEN);
	dc.Rectangle(&ClipBox);

	// Check if [Left, Right] is contained in [Min, Max].
	ASSERT(m_fMin <= m_fLeft);
	ASSERT(m_fLeft <= m_fRight);
	ASSERT(m_fRight <= m_fMax);

	// Position of Left and Right
	int x1 = static_cast<int>((m_fLeft - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);
	int x2 = static_cast<int>((m_fRight - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);
	dc.SelectStockObject(BLACK_PEN);
	CBrush ButtonFace;
	ButtonFace.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	dc.SelectObject(ButtonFace);

	// Draw Left Arrow
	m_RectLeft = CRect (CPoint(0, x1), CSize(dy, m_nArrowWidth));
	{ 
		CRect & rect = m_RectLeft;
		dc.Rectangle(&rect);
		// Make appearance 3D
		if (!m_bTracking || m_TrackMode != TRACK_LEFT)
		{
			rect.DeflateRect(1,1,2,2);
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SelectObject(pen1);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
			dc.SelectObject(pen2);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
		}

		// Draw Arrow
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		CPoint poly [] =
		{ 
			CPoint(dy / 3, x1 + m_nArrowWidth / 3), 
			CPoint(dy*2/3, x1 + m_nArrowWidth / 3),
			CPoint(dy / 2, x1 + m_nArrowWidth*2/3),
		};
		dc.Polygon(poly, 3);
	}

	// Draw Right Arrow
	m_RectRight = CRect(CPoint(0,m_nArrowWidth + x2), CSize(dy, m_nArrowWidth));
	{
		CRect & rect = m_RectRight;
		dc.SelectObject(ButtonFace);
		dc.Rectangle(&rect);

		// Make appearance 3D
		if (!m_bTracking || m_TrackMode != TRACK_RIGHT)
		{
			rect.DeflateRect(1,1,2,2);
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SelectObject(pen1);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
			dc.SelectObject(pen2);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
		}

		// Draw Arrow
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		CPoint poly [] =
		{ 
			CPoint(dy / 3, m_nArrowWidth + x2 + m_nArrowWidth * 2 / 3),
			CPoint(dy*2/3, m_nArrowWidth + x2 + m_nArrowWidth * 2 / 3),
			CPoint(dy / 2, m_nArrowWidth + x2 + m_nArrowWidth / 3),
		};
		dc.Polygon(poly, 3);
	}

	dc.SelectStockObject(WHITE_BRUSH);
	// Draw Area in between.
	CRect rect = CRect(CPoint(1, m_nArrowWidth + x1 + 1), CPoint (dy - 1, m_nArrowWidth + x2));

	dc.SelectStockObject(NULL_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);

	// Store value for drawing the focus rectangle
	CRect focusRect = rect;

	// Draw Area of InnerMin, Max.
	if (m_bVisualMinMax)
	{
		ASSERT(m_fMin <= m_fVisualMax);
		ASSERT(m_fVisualMin <= m_fVisualMax);
		ASSERT(m_fVisualMax <= m_fMax);

		// See wether [InnerMin, InnerMax] geschnitten [Left, Right] is non-empty.
		// Then Draw
		if (m_fVisualMin < m_fRight && m_fVisualMax > m_fLeft)
		{
			int visualMin = static_cast<int>((m_fVisualMin - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);
			int visualMax = static_cast<int>((m_fVisualMax - m_fMin)/(m_fMax - m_fMin) * m_fx + 0.5);

			// Stay in [m_fLeft, m_fRight] intervall.
			visualMin = max(visualMin + m_nArrowWidth, x1 + m_nArrowWidth);
			visualMax = min(visualMax + m_nArrowWidth, x2 + m_nArrowWidth);

			rect = CRect(dy - 1, visualMin, 1, visualMax);
			CBrush br;
			br.CreateSolidBrush(m_crVisualColor);
			dc.SelectObject(br);
			dc.SelectStockObject(NULL_PEN);
			dc.Rectangle(&rect);
			dc.SelectStockObject(HOLLOW_BRUSH);
		}
	}
	if (GetFocus() == this)
	{
		focusRect.DeflateRect(3,3);
		dc.DrawFocusRect(&focusRect);
	}
	dc.SelectObject(pBr);
	dc.SelectObject(pPen);
}


void CRangeSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	TRACE(_T("Down Point %d, %d\n"), point.x, point.y);
	SetFocus();
	Invalidate();

	if (!m_bTracking)
	{
		// Hit Testing into Rects.
		// Left, Middle or Right?
		CRect rect;
		GetClientRect(&rect);
		CRect middleRect (0,0,0,0);
		if (m_bHorizontal)
			middleRect = CRect(m_RectLeft.right + 1, 0, m_RectRight.left - 1, rect.bottom);
		else 
			middleRect = CRect(0, m_RectLeft.bottom + 1, m_RectLeft.right, m_RectRight.top - 1);
		if (m_RectLeft.PtInRect(point))
		{
			m_bTracking = TRUE;
			m_TrackMode = TRACK_LEFT;
			m_ClickOffset = point - m_RectLeft.CenterPoint();
		}
		else if (m_RectRight.PtInRect(point))
		{
			m_bTracking = TRUE;
			m_TrackMode = TRACK_RIGHT;
			m_ClickOffset = point - m_RectRight.CenterPoint();
		}
		else if (middleRect.PtInRect(point))
		{
			m_bTracking = TRUE;
			m_TrackMode = TRACK_MIDDLE;
			m_ClickOffset = point - middleRect.CenterPoint();
		}
		if (m_bTracking)
			SetCapture();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CRangeSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		int x = 0;
		if (m_bHorizontal)
			x = point.x - m_ClickOffset.x;
		else
			x = point.y - m_ClickOffset.y;

		CRect rect;
		WPARAM changed = 0;
		switch (m_TrackMode)
		{
		case TRACK_LEFT:
			{
				float oldLeft = m_fLeft;
				m_fLeft = static_cast<float>(x - m_nArrowWidth / 2) / m_fx * (m_fMax - m_fMin) + m_fMin; 
				if (m_fLeft >= m_fRight)
					m_fLeft = m_fRight;
				if (m_fLeft <= m_fMin)
					m_fLeft = m_fMin;

				if (oldLeft != m_fLeft)
					changed = RS_LEFTCHANGED;
				rect = m_RectLeft;
			}
			break;
		case TRACK_RIGHT:
			{
				float oldRight = m_fRight;
				m_fRight = static_cast<float>(x - m_nArrowWidth * 3 / 2) / m_fx * (m_fMax - m_fMin) + m_fMin;
				if (m_fRight <= m_fLeft)
					m_fRight = m_fLeft;
				if (m_fRight >= m_fMax)
					m_fRight = m_fMax;
				if (oldRight != m_fRight)
					changed = RS_RIGHTCHANGED;
				rect = m_RectRight;
			}
			break;
		case TRACK_MIDDLE:
			{
				float delta = m_fRight - m_fLeft;
				ASSERT(delta >= 0.0);
				m_fLeft  = static_cast<float>(x - m_nArrowWidth) / m_fx * (m_fMax - m_fMin) + m_fMin - delta/2.0f;
				m_fRight = static_cast<float>(x - m_nArrowWidth) / m_fx * (m_fMax - m_fMin) + m_fMin + delta/2.0f; 
				if (m_fLeft <= m_fMin)
				{
					m_fLeft = m_fMin;
					m_fRight = m_fLeft + delta;
				}
				if (m_fRight >= m_fMax)
				{
					m_fRight = m_fMax;
					m_fLeft = m_fRight - delta;
				}
				changed = RS_BOTHCHANGED;
			}
			break;
		default:
			TRACE(_T("Unknown Track Mode\n"));
			ASSERT(FALSE);
			break;
		}
		::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, changed, static_cast<LPARAM>(1));	// 1=正在改变
		m_bRangeChanged = TRUE;
		Invalidate();
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CRangeSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		m_bTracking = FALSE;
		::ReleaseCapture();
		if (m_bRangeChanged)
		{
			::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_BOTHCHANGED, static_cast<LPARAM>(0));// 0=已改变
			m_bRangeChanged = FALSE;
		}
		Invalidate();
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CRangeSlider::SetMinMax(float min, float max)
{
	if (min > max)
	{
		float x = max;
		max = min;
		min = x;
	}

	m_fMin = (m_bInvertedMode) ? -max : min;
	m_fMax = (m_bInvertedMode) ? -min : max;
	Normalize();
	Invalidate();
}

void CRangeSlider::SetRange(float left, float right)
{
	m_fLeft = (m_bInvertedMode) ? -right : left;
	m_fRight = (m_bInvertedMode) ? -left : right;
	Normalize();
	Invalidate();
}

void CRangeSlider::NormalizeVisualMinMax(void)
{
	// Exchange if reversed.
	if (m_fVisualMax < m_fVisualMin)
	{
		float dummy = m_fVisualMin;
		m_fVisualMin = m_fVisualMax;
		m_fVisualMax = dummy;
	}
	if (m_fVisualMin < m_fMin)
	{
		m_fVisualMin = m_fMin;
		if (m_fVisualMax < m_fVisualMin) 
			m_fVisualMax = m_fVisualMin;
	}
	if (m_fVisualMax > m_fMax)
	{
		m_fVisualMax = m_fMax;
		if (m_fVisualMin > m_fVisualMax)
			m_fVisualMin = m_fVisualMax;
	}
}

void CRangeSlider::Normalize(void)
{
	if (m_fLeft < m_fMin)
	{
		m_fLeft = m_fMin;
		::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_LEFTCHANGED, 0);
		if (m_fRight < m_fLeft)
		{
			m_fRight = m_fLeft;
			::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_RIGHTCHANGED, 0);
		}
	}
	if (m_fRight > m_fMax)
	{
		m_fRight = m_fMax;
		::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_RIGHTCHANGED, 0);
		if (m_fLeft > m_fRight)
		{
			m_fLeft = m_fRight;
			::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_LEFTCHANGED, 0);
		}
	}
}

void CRangeSlider::SetVisualMode(BOOL bVisualMinMax)
{
	if (m_bVisualMinMax != bVisualMinMax) 
		Invalidate();

	m_bVisualMinMax = bVisualMinMax;
}

void CRangeSlider::SetVisualColor(COLORREF cr)
{
	if(m_crVisualColor != cr)
	{
		m_crVisualColor = cr;
		Invalidate();
	}
}

void CRangeSlider::SetVisualMinMax(float VisualMin, float VisualMax)
{
	m_fVisualMin = (m_bInvertedMode) ? -VisualMax : VisualMin;
	m_fVisualMax = (m_bInvertedMode) ? -VisualMin : VisualMax;
	NormalizeVisualMinMax();
	Invalidate();
}


UINT CRangeSlider::OnGetDlgCode() 
{
	return DLGC_WANTARROWS;
}

void CRangeSlider::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bCtrl = (::GetKeyState(VK_CONTROL) & 0x8000)!= 0; // T if Ctrl is pressed.

	UINT Key1, Key2;
	if (m_bHorizontal)
	{
		Key1 = VK_RIGHT; Key2 = VK_LEFT;
	}
	else
	{
		Key1 = VK_DOWN; Key2 = VK_UP;
	}
	
	if ((nChar == Key1 || nChar == Key2) && !bCtrl)
	{
		float dx = (m_fMax - m_fMin) / m_fx;
		if (dx != 0.0 )
		{
			int left = static_cast<int>((m_fLeft - m_fMin) / dx + 0.5);
			int right = static_cast<int>((m_fRight - m_fMin) / dx + 0.5);
			BOOL bShift = ::GetKeyState(VK_SHIFT) < 0; // T if Shift is pressed.
			if (nChar == Key2)
			{
				if (!bShift) // Shift not pressed => move intervall
					left--;
				right--; 
			}
			else
			{
				if (!bShift) // Shift not pressed => move intervall
					left++;
				right++;
			}
			if (left >= 0 && right <= m_fx)
			{
				m_fLeft = m_fMin + left * dx;
				m_fRight = m_fMin + right * dx;
			}
			if (!bShift)
				::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_BOTHCHANGED, 0);
			else 
				::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_LEFTCHANGED, 0);
			Invalidate();
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CRangeSlider::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	
	Invalidate();
}

void CRangeSlider::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	Invalidate();	
}

void CRangeSlider::SetVerticalMode(BOOL bVerticalMode)
{
	if (m_bHorizontal != bVerticalMode) 
		return; 

	m_bHorizontal = !bVerticalMode;
	Invalidate();
}

void CRangeSlider::SetInvertedMode(BOOL bInvertedMode)
{
	if (m_bInvertedMode == bInvertedMode)
		return;

	m_bInvertedMode = bInvertedMode;

	float dummy = m_fLeft;
	m_fLeft = - m_fRight;
	m_fRight = -dummy;

	dummy = m_fMin;
	m_fMin = - m_fMax;
	m_fMax = - dummy;

	dummy = m_fVisualMin;
	m_fVisualMin = - m_fVisualMax;
	m_fVisualMax = - dummy;

	Invalidate();
}

void CRangeSlider::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CBCGPColorDialog dlg (m_crVisualColor, 0, this);
	if (dlg.DoModal () == IDOK)
	{
		m_crVisualColor = dlg.GetColor ();
		SetVisualColor(m_crVisualColor);
	}
	Invalidate();
	::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_BOTHCHANGED, 0);

// 	CWnd::OnLButtonDblClk(nFlags, point);
}

void CRangeSlider::OnRButtonUp(UINT nFlags, CPoint point)
{
	CBCGPColorDialog dlg (m_crVisualColor, 0, this);
	if (dlg.DoModal () == IDOK)
	{
		m_crVisualColor = dlg.GetColor ();
		SetVisualColor(m_crVisualColor);
	}
	Invalidate();
	::SendMessage(GetParent()->GetSafeHwnd(), WM_RANGE_CHANGED, RS_BOTHCHANGED, 0);

// 	CWnd::OnRButtonUp(nFlags, point);
}
