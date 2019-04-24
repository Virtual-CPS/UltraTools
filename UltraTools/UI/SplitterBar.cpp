// CSplitterBar.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitterBar.h"

#ifdef TSMO_SYS
#include "GlobalDefsForSys.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterBar

IMPLEMENT_DYNAMIC(CSplitterBar, CWnd)

BEGIN_MESSAGE_MAP(CSplitterBar, CWnd)
	//{{AFX_MSG_MAP(CSplitterBar)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSplitterBar::CSplitterBar()
{
	m_bDragging = FALSE;
	m_pwndLeftPane = m_pwndRightPane = NULL;
}

CSplitterBar::~CSplitterBar()
{
}

BOOL CSplitterBar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bHorizontal)
{
	CWnd* pWnd = this;
	m_bHorizontal = bHorizontal;
	return pWnd->Create(NULL, _T(""), dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CSplitterBar message handlers

void CSplitterBar::OnPaint() 
{
	CRect rect;
	if (!GetUpdateRect(&rect))
		return;

	CPaintDC dc(this);
	GetClientRect(rect);
// 	dc.Draw3dRect(&rect,
// 				::GetSysColor(COLOR_BTNHIGHLIGHT),
// 				::GetSysColor(COLOR_BTNSHADOW));

	dc.Draw3dRect (rect, globalData.clrBarShadow, globalData.clrBarHilite);
	rect.InflateRect(-CX_BORDER, -CY_BORDER);
	dc.Draw3dRect (rect, globalData.clrBarFace, globalData.clrBarFace);
	GetClientRect(rect);
	dc.FillSolidRect (rect, globalData.clrBarFace);
}

LRESULT CSplitterBar::OnNcHitTest(CPoint point) 
{	
	return HTCLIENT;
}

int CSplitterBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	GetWindowRect(&m_rectSplitter);
	SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	//Initialize left most and right most coordinator
	CRect rectParent;
	CWnd* pParent = GetParent();
	if (pParent && pParent->GetSafeHwnd())
	{
		pParent->GetClientRect(rectParent);
		if(m_bHorizontal)
		{
			m_cxLeftMost = rectParent.top;
			m_cxRightMost = rectParent.bottom;
		}
		else
		{
			m_cxLeftMost = rectParent.left;
			m_cxRightMost = rectParent.right;
		}
	}

	return 0;
}

BOOL CSplitterBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint ptCursor = GetMessagePos();		
	if(IsCursorOverSplitter(ptCursor))
	{
		::SetCursor( AfxGetApp()->LoadCursor(m_bHorizontal ? AFX_IDC_VSPLITBAR : AFX_IDC_HSPLITBAR));	
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CSplitterBar::IsCursorOverSplitter( const CPoint& ptCursor )
{
	CRect rectSplitter;
	GetWindowRect(rectSplitter);
	return rectSplitter.PtInRect( ptCursor );
}

void CSplitterBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(nFlags & MK_LBUTTON && m_bDragging)
	{
		DrawDraggingBar(point);
		return;
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CSplitterBar::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	ClientToScreen(&point);
	if(IsCursorOverSplitter(point))
	{
		SetCapture();
		m_bDragging = TRUE;
		GetWindowRect(&m_rectSplitter);		
		ScreenToClient(&point);
		DrawDraggingBar(point, DRAG_ENTER);
		return;
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CSplitterBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
	{
		DrawDraggingBar(point, DRAG_EXIT);

		//Move the splitter here
		ClientToScreen(&point);

		//with in client area?
		UpdateUtmostCoordinatorOfPane();
		if(m_bHorizontal)
		{
			CPoint pointLeftMost;
			pointLeftMost.y = m_cxLeftMost;
			GetParent()->ClientToScreen(&pointLeftMost);
			CPoint pointRightMost;
			pointRightMost.y = m_cxRightMost;
			GetParent()->ClientToScreen(&pointRightMost);

			if(point.y < pointLeftMost.y)
				point.y = pointLeftMost.y;
			if(point.y > pointRightMost.y)
				point.y = pointRightMost.y;

			m_rectDragCurt = m_rectSplitter;
			m_rectDragCurt.top = point.y;
			m_rectDragCurt.bottom = point.y + m_rectSplitter.Height();
		}
		else
		{
			CPoint pointLeftMost;
			pointLeftMost.x = m_cxLeftMost;
			GetParent()->ClientToScreen(&pointLeftMost);
			CPoint pointRightMost;
			pointRightMost.x = m_cxRightMost;
			GetParent()->ClientToScreen(&pointRightMost);

			if(point.x < pointLeftMost.x)
				point.x = pointLeftMost.x;
			if(point.x > pointRightMost.x)
				point.x = pointRightMost.x;

			m_rectDragCurt = m_rectSplitter;
			m_rectDragCurt.left = point.x;
			m_rectDragCurt.right = point.x + m_rectSplitter.Width();
		}
		GetParent()->ScreenToClient(m_rectDragCurt);
		MoveWindow(m_rectDragCurt,TRUE);
		OnPaint();

		ReleaseCapture();
		m_bDragging = FALSE;
		MovePanes();
		GetParent()->SendMessage(WM_SPLITTER_MOVED, 0, 0L);
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CSplitterBar::DrawDraggingBar(CPoint point, DRAGFLAG df)
{
	ClientToScreen(&point);
	m_rectDragCurt = m_rectSplitter;
	if(m_bHorizontal)
	{
		m_rectDragCurt.top = point.y;
		m_rectDragCurt.bottom = point.y + m_rectSplitter.Height();
	}
	else
	{
		m_rectDragCurt.left = point.x;
		m_rectDragCurt.right = point.x + m_rectSplitter.Width();
	}

	CSize size(m_rectDragCurt.Width(), m_rectDragCurt.Height());

	CWnd *pParentWnd = GetParent();
	ASSERT(pParentWnd);
	CDC *pDC = pParentWnd->GetDC();	
	pParentWnd->ScreenToClient(m_rectDragCurt);
	switch(df)
	{
	case DRAG_ENTER:
		 pDC->DrawDragRect(m_rectDragCurt, size, NULL, size);
		 break;
	case DRAG_EXIT:	//fall through
	default:
		 pDC->DrawDragRect(m_rectDragCurt, size, m_rectDragPrev, size);
		 break;
	}

	pParentWnd->ReleaseDC(pDC);
	m_rectDragPrev = m_rectDragCurt;
}

void CSplitterBar::SetPanes(CWnd *pwndLeftPane, CWnd *pwndRightPane)
{
	ASSERT(pwndLeftPane);
	ASSERT(pwndRightPane);

	m_pwndLeftPane = pwndLeftPane;
	m_pwndRightPane = pwndRightPane;

	int nHBarWidth = GetSystemMetrics(SM_CYFRAME);
	int nVBarWidth = GetSystemMetrics(SM_CXFRAME);
// 	int nHBarWidth = VIDEO_PREVIEW_WND_INTERVAL;
// 	int nVBarWidth = VIDEO_PREVIEW_WND_INTERVAL;

	if(m_bHorizontal)
	{
		//Initialize splitter bar position & size
		CRect rectBar;
		pwndLeftPane->GetWindowRect(rectBar);
		GetParent()->ScreenToClient(rectBar);
		rectBar.top = rectBar.bottom;
		rectBar.top -= nHBarWidth/2;
		rectBar.bottom += nHBarWidth/2;
		MoveWindow(rectBar);
	}
	else
	{
		//Initialize splitter bar position & size
		CRect rectBar;
		pwndLeftPane->GetWindowRect(rectBar);
		GetParent()->ScreenToClient(rectBar);
		rectBar.left = rectBar.right;
		rectBar.left -= nVBarWidth/2;
		rectBar.right += nVBarWidth/2;
		MoveWindow(rectBar);
	}

	MovePanes();									//repostion panes
	UpdateUtmostCoordinatorOfPane();
}

// 更新极限位置坐标，是改变panes大小的依据
// 如果parent是Dialog，在SetPanes()调用一次即可；
// 如果parent是FormView之类的可变大小的，则如本类所示多处调用
void CSplitterBar::UpdateUtmostCoordinatorOfPane()
{
	CRect rectLeft, rectRight;

	if(m_bHorizontal)
	{
		//calculate top most and bottom most coordinator
		m_pwndLeftPane->GetWindowRect(rectLeft);
		GetParent()->ScreenToClient(rectLeft);
		m_cxLeftMost = rectLeft.top;

		m_pwndRightPane->GetWindowRect(rectRight);
		GetParent()->ScreenToClient(rectRight);
		m_cxRightMost = rectRight.bottom;
	}
	else
	{
		//calculate left most and right most coordinator
		m_pwndLeftPane->GetWindowRect(rectLeft);
		GetParent()->ScreenToClient(rectLeft);
		m_cxLeftMost = rectLeft.left;

		m_pwndRightPane->GetWindowRect(rectRight);
		GetParent()->ScreenToClient(rectRight);
		m_cxRightMost = rectRight.right;
	}
}

// reposition panes
void CSplitterBar::MovePanes()
{
	ASSERT(m_pwndLeftPane);
	ASSERT(m_pwndRightPane);

// 	int nHBorder = GetSystemMetrics(SM_CXBORDER);
// 	int nVBorder = GetSystemMetrics(SM_CYBORDER);
	int nHBorder = 0;
	int nVBorder = 0;

	//Get position of the splitter bar
	CRect rectBar;
	GetWindowRect(rectBar);
	GetParent()->ScreenToClient(rectBar);

	CRect rectLeft, rectRight;

	if(m_bHorizontal)
	{
		//reposition top pane
		m_pwndLeftPane->GetWindowRect(rectLeft);
		GetParent()->ScreenToClient(rectLeft);
		rectLeft.bottom = rectBar.top + nHBorder;
		m_pwndLeftPane->MoveWindow(rectLeft);

		//reposition bottom pane
		m_pwndRightPane->GetWindowRect(rectRight);
		GetParent()->ScreenToClient(rectRight);
		rectRight.top = rectBar.bottom - nHBorder;
		m_pwndRightPane->MoveWindow(rectRight);
	}
	else
	{
		//reposition left pane
		m_pwndLeftPane->GetWindowRect(rectLeft);
		GetParent()->ScreenToClient(rectLeft);
		rectLeft.right = rectBar.left + nVBorder;
		m_pwndLeftPane->MoveWindow(rectLeft);

		//reposition right pane
		m_pwndRightPane->GetWindowRect(rectRight);
		GetParent()->ScreenToClient(rectRight);
		rectRight.left = rectBar.right - nVBorder;
		m_pwndRightPane->MoveWindow(rectRight);
	}

	//repaint client area
	GetParent()->Invalidate();
}
