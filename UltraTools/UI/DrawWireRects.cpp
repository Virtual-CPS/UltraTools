#include    "stdafx.h"
#include    "DrawWireRects.h"

void WINAPI DrawWireRects(LPRECT lprcFrom, LPRECT lprcTo, UINT nMilliSecSpeed)
{
    const int nNumSteps = 20;

	GdiFlush();
    Sleep(50);  // Let the desktop window sort itself out

	// if hwnd is null - "you have the CON".
	HDC hDC = ::GetDC(NULL);

	// Pen size, urmmm not too thick
    HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0,0,0));

	int nMode = ::SetROP2(hDC, R2_NOT);
	HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);

	for (int i = 0; i < nNumSteps; i++)
	{
        double dFraction = (double) i / (double) nNumSteps;

        RECT transition;
        transition.left   = lprcFrom->left + (int)((lprcTo->left - lprcFrom->left) * dFraction);
        transition.right  = lprcFrom->right + (int)((lprcTo->right - lprcFrom->right) * dFraction);
        transition.top    = lprcFrom->top + (int)((lprcTo->top - lprcFrom->top) * dFraction);
        transition.bottom = lprcFrom->bottom + (int)((lprcTo->bottom - lprcFrom->bottom) * dFraction);

		POINT pt[5];
		pt[0] = CPoint(transition.left, transition.top);
		pt[1] = CPoint(transition.right,transition.top);
		pt[2] = CPoint(transition.right,transition.bottom);
		pt[3] = CPoint(transition.left, transition.bottom);
		pt[4] = CPoint(transition.left, transition.top);

		// We use Polyline because we can determine our own pen size
		// Draw Sides
		::Polyline(hDC,pt,5);

		GdiFlush();

		Sleep(nMilliSecSpeed);

		// UnDraw Sides
		::Polyline(hDC,pt,5);

		GdiFlush();
	}

	::SetROP2(hDC, nMode);
	::SelectObject(hDC, hOldPen);

	::ReleaseDC(NULL,hDC);
}
