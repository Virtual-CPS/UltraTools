// UltraToolsView.cpp : implementation of the CUltraToolsView class
//

#include "stdafx.h"
#include "UltraTools.h"

#include "UltraToolsDoc.h"
#include "UltraToolsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUltraToolsView

IMPLEMENT_DYNCREATE(CUltraToolsView, CBCGPFormView)

BEGIN_MESSAGE_MAP(CUltraToolsView, CBCGPFormView)
END_MESSAGE_MAP()

// CUltraToolsView construction/destruction

CUltraToolsView::CUltraToolsView()
	: CBCGPFormView(CUltraToolsView::IDD)
{

	EnableVisualManagerStyle();

	// TODO: add construction code here

}

CUltraToolsView::~CUltraToolsView()
{
}

void CUltraToolsView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView::DoDataExchange(pDX);
}

BOOL CUltraToolsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CBCGPFormView::PreCreateWindow(cs);
}

void CUltraToolsView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();
	ResizeParentToFit();

}


// CUltraToolsView diagnostics

#ifdef _DEBUG
void CUltraToolsView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

void CUltraToolsView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}

CUltraToolsDoc* CUltraToolsView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUltraToolsDoc)));
	return (CUltraToolsDoc*)m_pDocument;
}
#endif //_DEBUG


// CUltraToolsView message handlers
