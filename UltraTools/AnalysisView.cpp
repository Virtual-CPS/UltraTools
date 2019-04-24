// AnalysisView.cpp : implementation of the CAnalysisView class
//

#include "stdafx.h"
#include "UltraTools.h"
#include "UltraToolsDoc.h"
#include "AnalysisView.h"
#include "Utility/MemDC.h"

#include "MainFrm.h"
#include "ReportView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAnalysisView

IMPLEMENT_DYNCREATE(CAnalysisView, CScrollView)

BEGIN_MESSAGE_MAP(CAnalysisView, CScrollView)
 	ON_WM_CREATE()
	ON_MESSAGE(WM_UPDATE_IMAGE, OnUpdateImage)

	// 切换当前图谱
	ON_COMMAND(IDM_ANALYSIS_FIRST_IMAGE, OnFirstImage)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_FIRST_IMAGE, OnUpdateFirstImage)
	ON_COMMAND(IDM_ANALYSIS_PRE_IMAGE, OnPreImage)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_PRE_IMAGE, OnUpdatePreImage)
	ON_COMMAND(IDM_ANALYSIS_NEXT_IMAGE, OnNextImage)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_NEXT_IMAGE, OnUpdateNextImage)
	ON_COMMAND(IDM_ANALYSIS_LAST_IMAGE, OnLastImage)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_LAST_IMAGE, OnUpdateLastImage)
	ON_COMMAND(IDM_ANALYSIS_SELECT_VL_IMAGE, OnSelectAndLoadVLImage)
	ON_COMMAND(IDM_ANALYSIS_SELECT_REPORT_TEMPLATE, OnSelectReportTemplate)

	// 绘制测温objs
	ON_COMMAND(IDM_ANALYSIS_VIEW_MEASUREMENT_BAR, OnViewIRCameraMeasurementDockingBar)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_VIEW_MEASUREMENT_BAR, OnUpdateViewIRCameraMeasurementDockingBar)
	ON_COMMAND(IDM_ANALYSIS_SELECT, OnDrawSelect)
	ON_COMMAND(IDM_ANALYSIS_DEL_SELECTED_OBJS, OnDelSelected)
	ON_COMMAND(IDM_ANALYSIS_DEL_ALL_OBJS, OnDelAllObjs)
	ON_COMMAND(IDM_ANALYSIS_SPOT, OnDrawSpot)
	ON_COMMAND(IDM_ANALYSIS_LINE, OnDrawLine)
	ON_COMMAND(IDM_ANALYSIS_BOX, OnDrawRect)
	ON_COMMAND(IDM_ANALYSIS_SHOW_OBJS, OnShowObjs)
	ON_COMMAND_RANGE(IDM_ANALYSIS_OBJ_TIP_INDEX, IDM_ANALYSIS_OBJ_TIP_JUDGE, OnSetObjsTipType)
	ON_COMMAND(IDM_ANALYSIS_SHOW_PALETTE_SCALE, OnShowPaletteScale)
	ON_COMMAND(IDM_ANALYSIS_SHOW_PALETTE, OnShowPalette)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SELECT, OnUpdateDrawSelect)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_DEL_SELECTED_OBJS, OnUpdateDelSelected)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_DEL_ALL_OBJS, OnUpdateDelAllObjs)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SPOT, OnUpdateDrawSpot)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_LINE, OnUpdateDrawLine)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_BOX, OnUpdateDrawRect)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SHOW_OBJS, OnUpdateShowObjs)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_ANALYSIS_OBJ_TIP_INDEX, IDM_ANALYSIS_OBJ_TIP_JUDGE, OnUpdateSetObjsTipType)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SHOW_PALETTE_SCALE, OnUpdateShowPaletteScale)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SHOW_PALETTE, OnUpdateShowPalette)
	ON_MESSAGE(WM_OBJ_MODIFIED, OnObjModified)

	ON_COMMAND(IDM_ANALYSIS_ZOOM_IN, OnViewZoomin)
	ON_COMMAND(IDM_ANALYSIS_ZOOM_OUT, OnViewZoomout)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_ZOOM_IN, OnUpdateViewZoomin)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_ZOOM_OUT, OnUpdateViewZoomout)
	ON_COMMAND(IDM_ANALYSIS_ZOOM_ACTUAL, OnViewActualSize)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_ZOOM_ACTUAL, OnUpdateViewActualSize)
	ON_COMMAND(IDM_ANALYSIS_ROTATE_ANTICLOCKWISE, OnViewRotateAntiClockwise)
	ON_COMMAND(IDM_ANALYSIS_ROTATE_CLOCKWISE, OnViewRotateClockwise)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_ROTATE_ANTICLOCKWISE, OnUpdateEnableRotateImage)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_ROTATE_CLOCKWISE, OnUpdateEnableRotateImage)

	ON_COMMAND(IDM_ANALYSIS_CREATE_REPORT, OnCreateAnalysisReportView)
	ON_COMMAND(IDM_ANALYSIS_SAVE, OnSaveAnalysisResult)
//	ON_COMMAND(IDM_ANALYSIS_SAVE_AS, OnSaveAs)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_CREATE_REPORT, OnUpdateCreateAnalysisReportView)
	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SAVE, OnUpdateSaveAnalysisResult)
//	ON_UPDATE_COMMAND_UI(IDM_ANALYSIS_SAVE_AS, OnUpdateSaveAs)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CAnalysisView construction/destruction

CAnalysisView::CAnalysisView()
{
	SetScrollSizes(MM_TEXT, CSize(0, 0));

	m_bEndInit = FALSE;

	VERIFY(m_brHatch.CreateHatchBrush(HS_DIAGCROSS, RGB(191, 191, 191)));
	m_hImageEditThread = NULL;

	m_pIRCameraMeasurementDockingBar = NULL;

	m_nCurReportTemplateIndex = 0;
	m_sCurAnalyst = "";
}

CAnalysisView::~CAnalysisView()
{
	if (m_hImageEditThread)
	{
		WaitForSingleObject(m_hImageEditThread, INFINITE);
		CloseHandle(m_hImageEditThread);
	}

	//if (m_pIRCameraMeasurementDockingBar!=NULL && m_pIRCameraMeasurementDockingBar->GetSafeHwnd()!=NULL)
	if (m_pIRCameraMeasurementDockingBar != NULL)
	{
		delete m_pIRCameraMeasurementDockingBar;
		m_pIRCameraMeasurementDockingBar = NULL;
	}
}

BOOL CAnalysisView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

int CAnalysisView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_wndLoadImage.Create(IDD_LOAD_IMAGE_FOR_ANALYSIS_WND, this);
	ASSERT(::IsWindow(m_wndLoadImage.GetSafeHwnd()));
	return 0;
}

void CAnalysisView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	OnViewIRCameraMeasurementDockingBar();
 
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CString sFile = pDoc->m_sCurrentFile;
	LoadCoupleImage(sFile);

	// 始终只显示红外图谱文件名(短名)
	pDoc = GetDocument(FALSE);
	int nPos = m_wndLoadImage.m_sCurrentIRFile.ReverseFind('\\');
	if(nPos > 0)
		sFile = m_wndLoadImage.m_sCurrentIRFile.Mid(nPos+1);
	else
		sFile = m_wndLoadImage.m_sCurrentIRFile;
	pDoc->SetTitle(sFile);

	m_wndLoadImage.m_wndImage.SetObjTipType(OBJ_TIP_TYPE_JUDGE);
	m_bEndInit = TRUE;
}

// CAnalysisView drawing
void CAnalysisView::OnDraw(CDC* pDC)
{
	if(!m_bEndInit)
		return;

	CMyMemDC *pMemDC = NULL;
	pDC = pMemDC = new CMyMemDC(pDC);

	CRect rect;
	GetClientRect( rect );

	// 拦截OnEraseBkgnd()后，在部分机型上仍然屏幕闪烁，故不再绘制背景
	// 绘制背景
	if (m_brHatch.m_hObject)
	{
		rect.right  = max(rect.right , m_totalDev.cx);
		rect.bottom = max(rect.bottom, m_totalDev.cy);
		m_brHatch.UnrealizeObject();
		CPoint pt(0, 0);
		pDC->LPtoDP(&pt);
		pt = pDC->SetBrushOrg(pt.x % 8, pt.y % 8);
		CBrush* old = pDC->SelectObject(&m_brHatch);
		pDC->FillRect(&rect, &m_brHatch);
		pDC->SelectObject(old);
	}

	m_wndLoadImage.Invalidate(TRUE);
	//m_wndLoadImage.m_ctrlPalette.DrawPalette(m_wndLoadImage.m_nCurPaletteIndex);

	delete pMemDC;
}

void CAnalysisView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	if (m_wndLoadImage.m_pIRImage == NULL)
	{
		RepositionCtrls();
		return;
	}

	int px = GetScrollPos(SB_HORZ);
	int py = GetScrollPos(SB_VERT);
	CSize sz(GetTotalSize());
	int x, y;
	if(m_wndLoadImage.m_bShowPalette)
		x = (int)(m_wndLoadImage.m_pIRImage->GetWidth() * m_wndLoadImage.m_fZoomFactor) + m_wndLoadImage.m_nPaleteWidth;
	else
		x = (int)(m_wndLoadImage.m_pIRImage->GetWidth() * m_wndLoadImage.m_fZoomFactor);
	y = (int)(m_wndLoadImage.m_pIRImage->GetHeight() * m_wndLoadImage.m_fZoomFactor);
	SetScrollSizes(MM_TEXT,	CSize(x,y));

	CSize sz2(GetTotalSize());

	CWnd* pFrame = GetParentFrame();
	RECT rClient;
	pFrame->GetClientRect(&rClient);

	if (sz.cx!=0 && sz.cy!=0)
	{
		if (x>rClient.right)
			SetScrollPos(SB_HORZ, sz2.cx*px/sz.cx);
		else
			SetScrollPos(SB_HORZ, 0);

		if (y>rClient.bottom)
			SetScrollPos(SB_VERT, sz2.cy*py/sz.cy);
		else
			SetScrollPos(SB_VERT,0);
	}

	if (!(pFrame->IsIconic() || pFrame->IsZoomed()))
	{
		RECT rMainCl,rFrame,rMainFr;
		((CMainFrame *)(AfxGetApp()->m_pMainWnd))->GetClientRect(&rMainCl);
		((CMainFrame *)(AfxGetApp()->m_pMainWnd))->GetWindowRect(&rMainFr);
		pFrame->GetWindowRect(&rFrame);
		pFrame->SetWindowPos(0,0,0,
			(4+rFrame.right-rFrame.left-rClient.right+rClient.left)+
			min(rMainCl.right-(rFrame.left-rMainFr.left+12),x),
			(4+rFrame.bottom-rFrame.top-rClient.bottom+rClient.top)+
			min(rMainCl.bottom-(rFrame.top-rMainFr.top+12),y),
			SWP_NOMOVE|SWP_NOZORDER);
	}

#ifdef VATI_EXTENSIONS
		m_wndLoadImage.m_pIRImage->SetJpegQualityF(theApp.m_optJpegQuality);
#if THERMAL_IMAGE_SUPPORT_JPG
		m_wndLoadImage.m_pIRImage->SetCodecOption(theApp.m_optJpegOptions,THERMAL_IMAGE_FORMAT_JPG);
#endif
#if THERMAL_IMAGE_SUPPORT_RAW
		m_wndLoadImage.m_pIRImage->SetCodecOption(theApp.m_optRawOptions,THERMAL_IMAGE_FORMAT_RAW);
#endif
#endif

	RepositionCtrls();
}

LRESULT CAnalysisView::OnUpdateImage(WPARAM wParam, LPARAM lParam)
{
	OnUpdate(NULL, NULL, NULL);

	return 0;
}


// CAnalysisView diagnostics

#ifdef _DEBUG
void CAnalysisView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAnalysisView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CUltraToolsDoc* CAnalysisView::GetDocument(BOOL bGlobalDoc) const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUltraToolsDoc)));
	if(bGlobalDoc)
		return &theApp.m_docIrImage;
	return (CUltraToolsDoc*)m_pDocument;
}
#endif //_DEBUG


// CAnalysisView message handlers

void CAnalysisView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	m_wndLoadImage.m_bIsActive = bActivate;
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

// 根据图谱大小重新排列界面中的控件
void CAnalysisView::RepositionCtrls()
{
	if(m_wndLoadImage.GetSafeHwnd())
		m_wndLoadImage.SendMessage( WM_SIZE );

	return;

	if (m_wndLoadImage.m_pIRImage == NULL)
	{
		CRect rectDummy;
		rectDummy.SetRectEmpty();
		m_wndLoadImage.MoveWindow(rectDummy);
		m_wndLoadImage.m_ctrlPalette.MoveWindow(rectDummy);
		Invalidate(TRUE);
		return;
	}

	CRect rectImage, rectPalette, rect;

	int x = (int)(m_wndLoadImage.m_pIRImage->GetWidth() * m_wndLoadImage.m_fZoomFactor);
	int y = (int)(m_wndLoadImage.m_pIRImage->GetHeight() * m_wndLoadImage.m_fZoomFactor);

	rectImage = CRect(0, 0, x, y);
	rectPalette = CRect(x, 5, x+m_wndLoadImage.m_nPaleteWidth, y-5);
	m_wndLoadImage.MoveWindow(rectImage, FALSE);

	// 确定调色板位置（不改变图谱大小，尽量与图谱不重叠）
	if(m_wndLoadImage.IsIrImage())
	{
		if(m_wndLoadImage.m_bShowPalette)
		{
			m_wndLoadImage.m_ctrlPalette.MoveWindow(rectPalette, FALSE);
			m_wndLoadImage.m_ctrlPalette.ShowWindow(SW_SHOW);
		}
		else
			m_wndLoadImage.m_ctrlPalette.ShowWindow(SW_HIDE);
	}
	else
		m_wndLoadImage.m_ctrlPalette.ShowWindow(SW_HIDE);

	m_wndLoadImage.Invalidate(TRUE);
	m_wndLoadImage.m_ctrlPalette.DrawPalette(m_wndLoadImage.m_nCurPaletteIndex);
	m_wndLoadImage.m_ctrlPalette.Invalidate(TRUE);
	Invalidate(FALSE);
}

// 重新排列界面中的控件
void CAnalysisView::CenterImage()
{
	return;

	CRect rectClient, rectTab, rectImage, rectPalette, rect;
	int nWidth, nHeight, delt=0;
	int XDest, YDest, nDestWidth, nDestHeight;
	float fRatio, fImgRatio;

	if(m_wndLoadImage.m_pIRImage == NULL)
	{
		rectImage.SetRectEmpty();
		m_wndLoadImage.MoveWindow(rectImage);
		//m_wndLoadImage.m_ctrlPalette.MoveWindow(rectImage);
		return;
	}

	GetClientRect(rectClient);

	//-----------------------------------------------
	// 确定图谱位置
	nWidth = rectClient.Width();
	nHeight = rectClient.Height();
	fRatio = (float)nHeight/nWidth;
	fImgRatio = (float)m_wndLoadImage.m_pIRImage->GetHeight()/m_wndLoadImage.m_pIRImage->GetWidth();

	if(fImgRatio > fRatio)
	{
		nDestWidth  = (int)(nHeight/fImgRatio);
		XDest       = (nWidth-nDestWidth)/2;
		YDest       = 0;
		nDestHeight = nHeight;
	}
	else
	{
		XDest       = 0;
		nDestWidth  = nWidth;
		nDestHeight = (int)(nWidth*fImgRatio);
		YDest       = (nHeight-nDestHeight)/2;
	}

	rectImage = CRect(XDest, YDest, XDest+nDestWidth, YDest+nDestHeight);
	m_wndLoadImage.MoveWindow(rectImage, FALSE);
	//-----------------------------------------------

	// 确定调色板位置（不改变图谱大小，尽量与图谱不重叠）
	if(m_wndLoadImage.IsIrImage())
	{
		if(m_wndLoadImage.m_bShowPalette)
		{
			if(m_wndLoadImage.m_ctrlPalette.IsShowCaclibration())	// 显示刻度
				m_wndLoadImage.m_nPaleteWidth = 40;
			else if(m_wndLoadImage.m_ctrlPalette.IsShowLimits())	// 显示上下限，不显示刻度
				m_wndLoadImage.m_nPaleteWidth = 40;
			else
				m_wndLoadImage.m_nPaleteWidth = 10;

			if(rectClient.Width()-rectImage.Width() <= 0)
			{	// 图谱占满整宽，调色板重叠于图谱右侧
				rectPalette.right = rectImage.right;
				rectPalette.left = rectPalette.right - m_wndLoadImage.m_nPaleteWidth;
				rectPalette.top = rectImage.top + 5;
				rectPalette.bottom = rectImage.bottom - 5;
			}
			else if(rectClient.Width()-rectImage.Width() <= m_wndLoadImage.m_nPaleteWidth)
			{	// 图谱未占满整宽，调色板与图谱部分重叠
				// 图谱居左（再定位）
				rect = rectImage;
				rect.left = rectClient.left;
				rect.right = rect.left + rectImage.Width();
				rectImage = rect;
				m_wndLoadImage.MoveWindow(rect, FALSE);

				// 调色板居右
				rectPalette.right = rectTab.left;
				rectPalette.left = rectPalette.right - m_wndLoadImage.m_nPaleteWidth;
				rectPalette.top = rectImage.top + 5;
				rectPalette.bottom = rectImage.bottom - 5;
			}
			else
			{	// 图谱与调色板“并列”
				// 图谱居左（再定位）
				rect = rectImage;
				rect.left = rectImage.left - m_wndLoadImage.m_nPaleteWidth/2;
				rect.right = rect.left + rectImage.Width();
				rectImage = rect;
				m_wndLoadImage.MoveWindow(rect, FALSE);

				// 调色板居右
				rectPalette.left = rectImage.right;
				rectPalette.right = rectPalette.left + m_wndLoadImage.m_nPaleteWidth;
				rectPalette.top = rectImage.top + 5;
				rectPalette.bottom = rectImage.bottom - 5;
			}
			m_wndLoadImage.m_ctrlPalette.MoveWindow(rectPalette, FALSE);
			m_wndLoadImage.m_ctrlPalette.ShowWindow(SW_SHOW);
		}
		else
			m_wndLoadImage.m_ctrlPalette.ShowWindow(SW_HIDE);
	}
	else
		m_wndLoadImage.m_ctrlPalette.ShowWindow(SW_HIDE);

	m_wndLoadImage.Invalidate(TRUE);
	m_wndLoadImage.m_ctrlPalette.DrawPalette(m_wndLoadImage.m_nCurPaletteIndex);
	m_wndLoadImage.m_ctrlPalette.Invalidate(TRUE);
	Invalidate(TRUE);
}

//#############################################################################
//								切换当前图谱
//
// 假设即将加载的图谱是有效的红外图谱，无论能否加载成功，
// 均要检查是否已处理现有红外图谱；
// 由于要记录文档接换顺序，故必须在切换之前检查处理状态，在LoadIrImage()中再次检查，
// 以免“遗漏”用户以“配对图谱”等方式加载红外图谱时的检查
//###############################--BEGIN--#####################################
void CAnalysisView::OnFirstImage() 
{
	int nRet = m_wndLoadImage.ConfirmSaveAnalysisResult();
	if(IDCANCEL == nRet)
		return;
	if(IDYES == nRet)
		OnSaveAnalysisResult();
	SetModified(FALSE);

	CString sFile, sFilePathAndName;
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_nCurItem = 0;
	sFile = pDoc->m_vFileNames[0];
	sFilePathAndName.Format(_T("%s\\%s"), pDoc->m_sRootDirectory, sFile);
	LoadCoupleImage(sFilePathAndName);
}

void CAnalysisView::OnUpdateFirstImage(CCmdUI* pCmdUI) 
{
	CUltraToolsDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc!=NULL && pDoc->m_nCurItem>0);
}

void CAnalysisView::OnPreImage() 
{
	int nRet = m_wndLoadImage.ConfirmSaveAnalysisResult();
	if(IDCANCEL == nRet)
		return;
	if(IDYES == nRet)
		OnSaveAnalysisResult();
	SetModified(FALSE);

	CString sFile, sFilePathAndName;
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_nCurItem--;
	sFile = pDoc->m_vFileNames[pDoc->m_nCurItem];
	sFilePathAndName.Format(_T("%s\\%s"), pDoc->m_sRootDirectory, sFile);
	LoadCoupleImage(sFilePathAndName);
}

void CAnalysisView::OnUpdatePreImage(CCmdUI* pCmdUI) 
{
	CUltraToolsDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc!=NULL && pDoc->m_nCurItem>0);
}

void CAnalysisView::OnNextImage() 
{
	int nRet = m_wndLoadImage.ConfirmSaveAnalysisResult();
	if(IDCANCEL == nRet)
		return;
	if(IDYES == nRet)
		OnSaveAnalysisResult();
	SetModified(FALSE);

	CString sFile, sFilePathAndName;
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_nCurItem++;
	sFile = pDoc->m_vFileNames[pDoc->m_nCurItem];
	sFilePathAndName.Format(_T("%s\\%s"), pDoc->m_sRootDirectory, sFile);
	LoadCoupleImage(sFilePathAndName);
}

void CAnalysisView::OnUpdateNextImage(CCmdUI* pCmdUI) 
{
	CUltraToolsDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc!=NULL && pDoc->m_nCurItem<(int)pDoc->m_vFileNames.size()-1);
}

void CAnalysisView::OnLastImage() 
{
	int nRet = m_wndLoadImage.ConfirmSaveAnalysisResult();
	if(IDCANCEL == nRet)
		return;
	if(IDYES == nRet)
		OnSaveAnalysisResult();
	SetModified(FALSE);

	CString sFile, sFilePathAndName;
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_nCurItem = (int)pDoc->m_vFileNames.size()-1;
	sFile = pDoc->m_vFileNames[pDoc->m_nCurItem];
	sFilePathAndName.Format(_T("%s\\%s"), pDoc->m_sRootDirectory, sFile);
	LoadCoupleImage(sFilePathAndName);
}

void CAnalysisView::OnUpdateLastImage(CCmdUI* pCmdUI) 
{
	CUltraToolsDoc* pDoc = GetDocument();
	int nCnt = (int)pDoc->m_vFileNames.size();
	pCmdUI->Enable(pDoc!=NULL && pDoc->m_nCurItem<nCnt-1);
}
//################################--END--######################################
//								切换当前图谱
//#############################################################################

//#############################################################################
//								绘制测温工具
//###############################--BEGIN--#####################################
void CAnalysisView::OnDrawSelect()
{
	m_wndLoadImage.m_wndImage.OnDrawSelect();
}

void CAnalysisView::OnDelSelected()
{
	m_wndLoadImage.m_wndImage.OnDelSelected();
}

void CAnalysisView::OnDelAllObjs()
{
	m_wndLoadImage.m_wndImage.OnDelAll();
}

void CAnalysisView::OnDrawSpot()
{
	m_wndLoadImage.m_wndImage.OnDrawSpot();
}

void CAnalysisView::OnDrawLine()
{
	m_wndLoadImage.m_wndImage.OnDrawLine();
}

void CAnalysisView::OnDrawRect()
{
	m_wndLoadImage.m_wndImage.OnDrawRect();
}

void CAnalysisView::OnShowObjs()
{
	BOOL bShow = m_wndLoadImage.m_wndImage.ObjsIsVisible();
	bShow = !bShow;
	m_wndLoadImage.m_wndImage.ShowObjs(bShow);
	Invalidate(FALSE);
}

void CAnalysisView::OnSetObjsTipType(UINT id)
{
	int nType = OBJ_TIP_TYPE_JUDGE;
	switch (id)
	{
	case IDM_ANALYSIS_OBJ_TIP_INDEX:		nType = OBJ_TIP_TYPE_INDEX;			break;
	case IDM_ANALYSIS_OBJ_TIP_TEMPERATURE:	nType = OBJ_TIP_TYPE_TEMPERATURE;	break;
	case IDM_ANALYSIS_OBJ_TIP_JUDGE:		nType = OBJ_TIP_TYPE_JUDGE;			break;
	}

	m_wndLoadImage.m_wndImage.SetObjTipType(nType);
}

void CAnalysisView::OnShowPaletteScale()
{
	BOOL bShow = m_wndLoadImage.m_ctrlPalette.IsShowCaclibration() && m_wndLoadImage.m_bShowPalette;
	bShow = !bShow;
	if(bShow)
		m_wndLoadImage.m_bShowPalette = bShow;
	m_wndLoadImage.m_ctrlPalette.ShowCalibration(bShow);
	m_wndLoadImage.m_ctrlPalette.ShowLimits(bShow);

	RepositionCtrls();
}

void CAnalysisView::OnShowPalette()
{
	BOOL bShow = m_wndLoadImage.m_bShowPalette;
	m_wndLoadImage.m_bShowPalette = !bShow;
	RepositionCtrls();
}

void CAnalysisView::OnUpdateDrawSelect(CCmdUI* pCmdUI)
{
	m_wndLoadImage.m_wndImage.OnUpdateDrawSelect(pCmdUI);
}

void CAnalysisView::OnUpdateDelSelected(CCmdUI* pCmdUI)
{
	m_wndLoadImage.m_wndImage.OnUpdateDelSelected(pCmdUI);
}

void CAnalysisView::OnUpdateDelAllObjs(CCmdUI* pCmdUI)
{
	m_wndLoadImage.m_wndImage.OnUpdateDelAll(pCmdUI);
}

void CAnalysisView::OnUpdateDrawSpot(CCmdUI* pCmdUI)
{
	m_wndLoadImage.m_wndImage.OnUpdateDrawSpot(pCmdUI);
}

void CAnalysisView::OnUpdateDrawLine(CCmdUI* pCmdUI)
{
	m_wndLoadImage.m_wndImage.OnUpdateDrawLine(pCmdUI);
}

void CAnalysisView::OnUpdateDrawRect(CCmdUI* pCmdUI)
{
	m_wndLoadImage.m_wndImage.OnUpdateDrawRect(pCmdUI);
}

void CAnalysisView::OnUpdateShowObjs(CCmdUI* pCmdUI)
{
	if(m_wndLoadImage.IsIrImage())
	{
		if(m_wndLoadImage.m_wndImage.m_objListObjects.IsEmpty())
			pCmdUI->Enable(FALSE);
		else
		{
			BOOL bShow = m_wndLoadImage.m_wndImage.ObjsIsVisible();
			pCmdUI->SetCheck (bShow);
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

void CAnalysisView::OnUpdateSetObjsTipType(CCmdUI* pCmdUI)
{
	int nType = OBJ_TIP_TYPE_JUDGE;
	switch (pCmdUI->m_nID)
	{
	case IDM_ANALYSIS_OBJ_TIP_INDEX:		nType = OBJ_TIP_TYPE_INDEX;			break;
	case IDM_ANALYSIS_OBJ_TIP_TEMPERATURE:	nType = OBJ_TIP_TYPE_TEMPERATURE;	break;
	case IDM_ANALYSIS_OBJ_TIP_JUDGE:		nType = OBJ_TIP_TYPE_JUDGE;			break;
	}
	pCmdUI->SetRadio (m_wndLoadImage.m_wndImage.GetObjTipType() == nType);
}

void CAnalysisView::OnUpdateShowPaletteScale(CCmdUI* pCmdUI)
{
	if (m_wndLoadImage.IsIrImage())
	{
		BOOL bShow = m_wndLoadImage.m_ctrlPalette.IsShowCaclibration() && m_wndLoadImage.m_bShowPalette;
		pCmdUI->SetCheck (bShow);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CAnalysisView::OnUpdateShowPalette(CCmdUI* pCmdUI)
{
	if (m_wndLoadImage.IsIrImage())
	{
		BOOL bShow = m_wndLoadImage.m_bShowPalette;
		pCmdUI->SetCheck (bShow);
	}
	else
		pCmdUI->Enable(FALSE);
}

LRESULT CAnalysisView::OnObjModified(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);

	if (m_pIRCameraMeasurementDockingBar)
	{
		m_pIRCameraMeasurementDockingBar->m_wndIRCameraMeasurement.m_pageObjParameters.UpdateMeasurementDataList(this);
	}
	return 0;
}

//################################--END--######################################
//								绘制测温工具
//#############################################################################

void CAnalysisView::OnViewZoomin() 
{
	m_wndLoadImage.Zoomin();
	OnUpdate(NULL, NULL, NULL);
}

void CAnalysisView::OnViewZoomout() 
{
	m_wndLoadImage.Zoomout();
	OnUpdate(NULL, NULL, NULL);
}

void CAnalysisView::OnUpdateViewZoomin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL && m_wndLoadImage.m_fZoomFactor<16);
}

void CAnalysisView::OnUpdateViewZoomout(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL && m_wndLoadImage.m_fZoomFactor>0.125);
}

void CAnalysisView::OnViewActualSize() 
{
	m_wndLoadImage.m_fZoomFactor = 1.0f;
	OnUpdate(NULL, NULL, NULL);
}

void CAnalysisView::OnUpdateViewActualSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL && m_wndLoadImage.m_fZoomFactor!=1);
}

void CAnalysisView::OnUpdateEnableModify(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL && m_hImageEditThread==NULL);
}

void CAnalysisView::OnViewRotateAntiClockwise() 
{
	m_MenuCommand = IDM_ANALYSIS_ROTATE_ANTICLOCKWISE;
	m_hImageEditThread = (HANDLE)_beginthread(RunImageEditThread, 0, this);
}

void CAnalysisView::OnViewRotateClockwise() 
{
	m_MenuCommand = IDM_ANALYSIS_ROTATE_CLOCKWISE;
	m_hImageEditThread = (HANDLE)_beginthread(RunImageEditThread, 0, this);
}

void CAnalysisView::OnUpdateEnableRotateImage(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_wndLoadImage.m_pIRImage!=NULL && m_hImageEditThread==NULL;
	pCmdUI->Enable(bEnable);
}

BOOL CAnalysisView::GetImageXY(long *x, long *y)
{
	if(NULL == m_wndLoadImage.m_pIRImage)
		return FALSE;

	CPoint point = GetScrollPosition();
	float fx =(float)(*x + point.x);
	float fy =(float)(*y + point.y);

	fx /= m_wndLoadImage.m_fZoomFactor;
	fy /= m_wndLoadImage.m_fZoomFactor;

	*x = (long)fx;
	*y = (long)fy;

	return m_wndLoadImage.m_pIRImage->IsInside(*x, *y);
}

void CAnalysisView::OnViewIRCameraMeasurementDockingBar()
{
	if (m_pIRCameraMeasurementDockingBar == NULL)
	{
		m_pIRCameraMeasurementDockingBar = new CIRCameraMeasurementDockingBar;

		CRect rectWindow, rectBar;
		GetWindowRect (rectWindow);
		rectBar.right = rectWindow.right - 20;
		rectBar.left = rectBar.right - 335;
		rectBar.top = rectWindow.top;
		rectBar.bottom = rectBar.top + 530;

		m_pIRCameraMeasurementDockingBar->Create(rectBar, this);
	}
	else if(!m_pIRCameraMeasurementDockingBar->IsVisible())
	{
		((CMainFrame*)theApp.GetMainWnd())->ShowControlBar(m_pIRCameraMeasurementDockingBar,
												TRUE, FALSE, TRUE);
	}
	else
		((CMainFrame*)theApp.GetMainWnd())->ShowControlBar(m_pIRCameraMeasurementDockingBar,
												FALSE, FALSE, FALSE);

	if(m_pIRCameraMeasurementDockingBar->IsVisible())
		m_pIRCameraMeasurementDockingBar->SetFocus();
}

void CAnalysisView::OnUpdateViewIRCameraMeasurementDockingBar(CCmdUI* pCmdUI) 
{
	if(m_wndLoadImage.IsIrImage())
		pCmdUI->SetCheck (m_pIRCameraMeasurementDockingBar!=NULL && m_pIRCameraMeasurementDockingBar->IsVisible());
	else
		pCmdUI->Enable(FALSE);
}

// 用户选择加载的文件不一定是普光图谱，可能是红外图谱
// 故本函数实际支持用户任意选择配对的红外图谱或普光图谱
void CAnalysisView::OnSelectAndLoadVLImage()
{
	CString sCurPath, sFileName=_T(""), sTitle;
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	sCurPath = pDoc->m_sRootDirectory;
	sTitle = _T("选择与红外图谱配对的普光图谱");

	if(!PromptForFileName(sFileName, sCurPath, sTitle, OFN_HIDEREADONLY, TRUE))
		return;

//	m_wndLoadImage.LoadTargetImage(sFileName, theApp.m_bSelectCompanionImageAsVLImage);

	OnUpdate(NULL, NULL, NULL);
	m_wndLoadImage.Invalidate(FALSE);

	pDoc = GetDocument(FALSE);
	pDoc->SetTitle(m_wndLoadImage.m_IrFileBaseInfo.sFileName);

	if(m_pIRCameraMeasurementDockingBar)
	{
		m_pIRCameraMeasurementDockingBar->m_wndIRCameraMeasurement.UpdateCameraInfo(m_wndLoadImage.m_IrFileBaseInfo, m_wndLoadImage.m_IrMeasurementParameters);
		m_pIRCameraMeasurementDockingBar->m_wndIRCameraMeasurement.m_pageImageProcess.Reset(m_wndLoadImage.m_fMinTemperature,m_wndLoadImage.m_fMaxTemperature, m_wndLoadImage.m_nCurPaletteIndex,TRUE);
	}
}

// 加载参数指定的文件之后，尝试加载其配对文档
BOOL CAnalysisView::LoadCoupleImage(CString sImageFilePathAndName)
{
	BOOL bRet = m_wndLoadImage.LoadTargetImage(sImageFilePathAndName);

	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CString sFileName, sTem=_T("");
	int nPos = sImageFilePathAndName.ReverseFind('\\');
	if(nPos > 0)
		sFileName = sImageFilePathAndName.Mid(nPos+1);
	else
		sFileName = sImageFilePathAndName;
/*
	if (bRet && m_wndLoadImage.IsIrImage())		// 刚加载的文件为红外图谱
	{
		pDoc->GetCoupleImageName(sFileName, sTem);
		if(sTem.GetLength() > 4)
		{
			sTem = pDoc->m_sCurrentDirectory + "\\" + sTem;
			bRet = m_wndLoadImage.LoadTargetImage(sTem, TRUE);// 强制为普光图谱
		}
	}
	else if (bRet)						// 刚加载的文件为普光图谱
	{
		pDoc->GetCoupleImageName(sTem, sFileName);
		if(sTem.GetLength() > 4)
		{
			sTem = pDoc->m_sCurrentDirectory + "\\" + sTem;
			bRet = m_wndLoadImage.LoadTargetImage(sTem);
		}
	}
*/
	OnUpdate(NULL, NULL, NULL);
	m_wndLoadImage.Invalidate(FALSE);

	pDoc = GetDocument(FALSE);
	pDoc->SetTitle(m_wndLoadImage.m_IrFileBaseInfo.sFileName);

	if(m_pIRCameraMeasurementDockingBar)
	{
		m_pIRCameraMeasurementDockingBar->m_wndIRCameraMeasurement.UpdateCameraInfo(m_wndLoadImage.m_IrFileBaseInfo, m_wndLoadImage.m_IrMeasurementParameters);
		m_pIRCameraMeasurementDockingBar->m_wndIRCameraMeasurement.m_pageImageProcess.Reset(m_wndLoadImage.m_fMinTemperature,m_wndLoadImage.m_fMaxTemperature, m_wndLoadImage.m_nCurPaletteIndex,TRUE);
	}

	return bRet;
}

// 无法用消息调用本函数？
void CAnalysisView::OnSelectReportTemplate()
{
	CBCGPToolbarComboBoxButton* pSrcCombo = NULL;
	m_nCurReportTemplateIndex = -1;
	m_sCurReportTemplateName = _T("");

	CObList listButtons;
	if (CBCGPToolBar::GetCommandButtons (IDM_ANALYSIS_SELECT_REPORT_TEMPLATE, listButtons) > 0)
	{
		POSITION pos = listButtons.GetHeadPosition();
		for ( ; pSrcCombo==NULL && pos!=NULL; )
		{
			CBCGPToolbarComboBoxButton* pCombo = 
				DYNAMIC_DOWNCAST (CBCGPToolbarComboBoxButton, listButtons.GetNext(pos));

			if (pCombo != NULL)
			{
				pSrcCombo = pCombo;
			}
		}
	}

	if (pSrcCombo != NULL)
	{
		m_nCurReportTemplateIndex = pSrcCombo->GetCurSel();
		m_sCurReportTemplateName = pSrcCombo->GetItem(-1);	// CurSel
	}
}

// 创建视图，显示临时保存的报表
void CAnalysisView::OnCreateAnalysisReportView()
{
	if(NULL == m_wndLoadImage.m_pIRImage)		// 尚未加载红外图谱
		return;
/*
	CString sReportTemplateFile, sReportTitle, sReportAuthor, sSQL, sTem;
	CStringArray saDbTabName, saSQLExpressionFields;
	CString sTime, sAnalysisDateTime;

	CTime tTime = CTime::GetCurrentTime();
	sAnalysisDateTime = tTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	tTime = tTime - CTimeSpan(1,0,0,0);							// 1天前
	sTime = tTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	// 删除数据库中临时报表(为避免多人同步操作时相互删除，故只删除1天前的)
	if(!theApp.m_SysDatabase.m_adoDatabaseConn.OpenConnection(theApp.m_sAdoConnectDB))
		return;
	sSQL.Format("DELETE FROM tbl_Analysis_Result WHERE State<>1 AND Date_Time<=\'%s\'", sTime);
	theApp.m_SysDatabase.m_adoDatabaseConn.ExecuteSQL(sSQL);

	// 在数据库中创建临时报表内容
	if(!SaveAnalysisResultIntoDatabase(sAnalysisDateTime, FALSE))
		return;

	ASSERT_VALID (theApp.m_pDocTemplateReport);
	theApp.m_pDocTemplateReport->OpenDocumentFile(NULL);
	extern CAnalysisReportView* g_pAnalysisReportView;
	if(NULL == g_pAnalysisReportView)
		return;

	// 置报表保存标记和SQL
	g_pAnalysisReportView->m_bReportSaved = FALSE;
	sSQL.Format("UPDATE tbl_Analysis_Result SET State=1 WHERE State=0 AND Date_Time=\'%s\' AND Analyst_Name=\'%s\'",
		sAnalysisDateTime, m_sCurAnalyst);
	g_pAnalysisReportView->m_sSqlForSaveReport = sSQL;

	// 打开临时报表(必须唯一：某人在同一时间只能建立一份临时报表)
	OnSelectReportTemplate();							// 获取报表模板序号
	if("**自动选择模板**" == m_sCurReportTemplateName)	// 自动
	//if(m_nCurReportTemplateIndex <= 0)				// 自动
	{
		if(m_wndLoadImage.m_pVLImage)
			sReportTemplateFile = "红外图谱+普光图谱.rpt";
		else
			sReportTemplateFile = "仅单幅红外图谱.rpt";
	}
	else
		sReportTemplateFile = m_sCurReportTemplateName + ".rpt";
	static int nIndex = 0;
	nIndex++;
	if(theApp.m_bReportAutoSerialNumber)
		sReportTitle.Format("红外检测分析报告--%d", nIndex);
	else
		sReportTitle = "红外检测分析报告";
	sReportAuthor = theApp.m_tsLoginName;
	sSQL.Format("SELECT * FROM View_Analysis_Result WHERE State=0 AND Date_Time=\'%s\' AND Analyst_Name=\'%s\'",
				sAnalysisDateTime, m_sCurAnalyst);

	sTem = "View_Analysis_Result";
	sTem.MakeUpper();							// Oracle必须转换为大写
	saDbTabName.RemoveAll();
	saDbTabName.Add(sTem);

	saSQLExpressionFields.RemoveAll();
	g_pAnalysisReportView->OpenReportFile(sReportTemplateFile,sReportTitle,
		sReportAuthor,sSQL,saDbTabName,saSQLExpressionFields);
*/}

// 永久性保存分析结果，并生成报表
void CAnalysisView::OnSaveAnalysisResult()
{
	if(NULL == m_wndLoadImage.m_pIRImage)		// 尚未加载红外图谱
		return;
/*
	CString sReportTemplateFile, sReportTitle, sReportAuthor, sSQL;
	CStringArray saDbTabName, saSQLExpressionFields;
	CString sTime, sAnalysisDateTime;

	CTime tTime = CTime::GetCurrentTime();
	sAnalysisDateTime = tTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	tTime = tTime - CTimeSpan(1,0,0,0);						// 1天前
	sTime = tTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	// 删除数据库中临时报表(为避免多人同步操作时相互删除，故只删除1天前的)
	if(!theApp.m_SysDatabase.m_adoDatabaseConn.OpenConnection(theApp.m_sAdoConnectDB))
		return;
	sSQL.Format("DELETE FROM tbl_Analysis_Result WHERE State=0 AND Date_Time<=\'%s\'", sTime);
	theApp.m_SysDatabase.m_adoDatabaseConn.ExecuteSQL(sSQL);

	// 在数据库中创建正式报表内容
	if(!SaveAnalysisResultIntoDatabase(sAnalysisDateTime, TRUE))
		return;

	if(!theApp.m_bOpenReportViewAfterSaveAnalysisResult)	// 不打开报表
		return;

	ASSERT_VALID (theApp.m_pDocTemplateReport);
	theApp.m_pDocTemplateReport->OpenDocumentFile(NULL);
	extern CAnalysisReportView* g_pAnalysisReportView;
	if(NULL == g_pAnalysisReportView)
		return;

	// 打开正式报表(必须唯一：某人在同一时间只能建立一份正式报表)
	OnSelectReportTemplate();								// 获取报表模板序号
	if("**自动选择模板**" == m_sCurReportTemplateName)		// 自动
	{
		if(m_wndLoadImage.m_pVLImage)
			sReportTemplateFile = "红外图谱+普光图谱.rpt";
		else
			sReportTemplateFile = "仅单幅红外图谱.rpt";
	}
	else
		sReportTemplateFile = m_sCurReportTemplateName + ".rpt";
	sReportTitle = "电气设备红外检测报告";
	sReportAuthor = theApp.m_tsLoginName;
	sSQL.Format("SELECT * FROM View_Analysis_Result WHERE State=1 AND Date_Time=\'%s\' AND Analyst_Name=\'%s\'",
						sAnalysisDateTime, m_sCurAnalyst);
	saDbTabName.RemoveAll();
	saSQLExpressionFields.RemoveAll();
	g_pAnalysisReportView->OpenReportFile(sReportTemplateFile,sReportTitle,
					sReportAuthor,sSQL,saDbTabName,saSQLExpressionFields);
*/}

// 将分析处理后的红外图谱另存为
void CAnalysisView::OnSaveAs()
{
	if(NULL == m_wndLoadImage.m_pIRImage)		// 尚未加载红外图谱
		return;

	CString sFileName, sCurPath, sTitle;
	int nDocType = m_wndLoadImage.m_pIRImage->GetType();

	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	sCurPath = pDoc->m_sRootDirectory;
	sFileName = pDoc->m_sCurrentFile;
	int nPos = sFileName.ReverseFind('\\');
	if(nPos > 0)
		sFileName = sFileName.Mid(nPos+1);
	nPos = sFileName.ReverseFind('.');
	if(nPos > 0)
		sFileName = sFileName.Mid(0, nPos);

	sTitle = _T("将处理后的红外图谱另存为");
	if (!PromptForFileName(sFileName, sCurPath, sTitle, OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, FALSE, &nDocType))
		return;

	CString sExt, sTem;
	nDocType = GetTypeFromFileName(sFileName, sExt);
	if (nDocType == THERMAL_IMAGE_FORMAT_UNKNOWN)
	{
		sTem.Format(_T("指定的文件的类型【%s】无效!"), sExt);
		MessageBox(sTem, _T("存储文件失败"), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	//if(!m_wndLoadImage.m_pIRImage->Save(sFileName, nDocType))
	if(!SaveCurIRImageAsTempFile(sFileName, nDocType))
	{
		// be sure to delete the file
		TRY 
		{
			CFile::Remove(sFileName);
		}
		CATCH_ALL(e)
		{
			TRACE0("Warning: failed to delete file after failed SaveAs\n");
		}
		END_CATCH_ALL

		sTem.Format(_T("保存文件出错\n\n%s"),m_wndLoadImage.m_pIRImage->GetLastError());
		MessageBox(sTem, _T("存储文件失败"), MB_OK|MB_ICONEXCLAMATION);
	}
}

void CAnalysisView::OnUpdateCreateAnalysisReportView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL);
}

void CAnalysisView::OnUpdateSaveAnalysisResult(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL && m_wndLoadImage.m_bModified);
}

void CAnalysisView::OnUpdateSaveAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndLoadImage.m_pIRImage!=NULL);
}

// 将当前红外图谱编辑结果保存到临时文件夹，以便入库
BOOL CAnalysisView::SaveCurIRImageAsTempFile(CString sFileName, int nFileType)
{
	if(NULL == m_wndLoadImage.m_pIRImage)
		return FALSE;

	if(!m_wndLoadImage.m_bShowPalette && !m_wndLoadImage.m_wndImage.ObjsIsVisible())		// 不显示调色板和测温工具
		return m_wndLoadImage.m_pIRImage->Save(sFileName, THERMAL_IMAGE_FORMAT_JPG);

	// 在内存中重建“复合”图谱并保存

	CRect imgRect;
	m_wndLoadImage.GetClientRect(imgRect);
	imgRect.NormalizeRect();
	CDC* pDC = GetDC();
	CMyMemDC* pMemDC = NULL;
	pDC = pMemDC = new CMyMemDC(pDC, imgRect);

	HBITMAP hBitmap, hBitTemp;

	// 创建HBITMAP
	hBitmap = CreateCompatibleBitmap(pDC->m_hDC, imgRect.Width(), imgRect.Height());   
	hBitTemp = (HBITMAP) SelectObject(pMemDC->m_hDC, hBitmap);

	// 得到位图缓冲区
	m_wndLoadImage.m_pIRImage->Draw(pMemDC->m_hDC, imgRect, NULL, TRUE);		// 绘制图谱
	if(m_wndLoadImage.m_wndImage.ObjsIsVisible())								// 绘制测温工具
	{
		m_wndLoadImage.m_wndImage.SelectObj(NULL);								// 去掉objs编辑handles
		m_wndLoadImage.m_wndImage.DrawObjs(pDC);
	}
	if(m_wndLoadImage.m_bShowPalette)											// 绘制调色板
	{
		// 为了避免因调色板导致最终“复合”图谱的宽度发生改变
		// 重新确定调色板的宽度和位置，将调色板重叠于红外图谱右边
		CRect rectPalette;
		m_wndLoadImage.m_ctrlPalette.GetClientRect(rectPalette);
		int nWidth = rectPalette.Width();
		int nHeight = rectPalette.Height();
		rectPalette.right = imgRect.right;
		rectPalette.left = imgRect.right - nWidth;
		rectPalette.top = imgRect.top + 5;
		rectPalette.bottom = rectPalette.top + nHeight;

		m_wndLoadImage.m_ctrlPalette.DrawPalette(m_wndLoadImage.m_nCurPaletteIndex, &rectPalette, pMemDC);
	}

	int oldMode = SetStretchBltMode(pMemDC->m_hDC, COLORONCOLOR);
	pMemDC->BitBlt(0, 0, imgRect.Width(), imgRect.Height(), pMemDC, 0, 0, SRCCOPY);
	SetStretchBltMode(pMemDC->m_hDC, oldMode);					// Restore mode

	// 得到最终的位图信息并转存为jpg文件
	hBitmap = (HBITMAP) SelectObject(pMemDC->m_hDC, hBitTemp);
	CThermalImage* pImage = new CThermalImage;
	pImage->CreateFromHBITMAP(hBitmap);
	BOOL bRet = FALSE;
	if (!pImage->IsValid())
	{
		CString sInfo(pImage->GetLastError());
		AfxMessageBox(sInfo);
		::delete pImage;
		pImage = NULL;
	}
	else
	{
		if(nFileType == THERMAL_IMAGE_FORMAT_UNKNOWN)
			bRet = pImage->Save(sFileName, THERMAL_IMAGE_FORMAT_JPG);
		else
			bRet = pImage->Save(sFileName, nFileType);

		::delete pImage;
		pImage = NULL;
	}

	//释放内存   
	if(pMemDC != NULL)
		delete pMemDC;

	DeleteDC(pDC->m_hDC);
	DeleteObject(pDC->m_hDC);
	ReleaseDC(pDC);

	DeleteObject(hBitmap);
	DeleteObject(hBitTemp);

	return bRet;
}
