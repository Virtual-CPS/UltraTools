// AnalysisView.h : interface of the CAnalysisView class
//

#pragma once

#include "UltraTools.h"
#include "LoadImageForAnalysisWnd.h"
#include "IRCameraMeasurementDockingBar.h"
#include "GlobalDefsForSys.h"

class CUltraToolsDoc;


class CAnalysisView : public CScrollView
{
protected: // create from serialization only
	CAnalysisView();
	DECLARE_DYNCREATE(CAnalysisView)
	virtual ~CAnalysisView();

// Attributes
public:
	HANDLE				m_hImageEditThread;		// 图谱编辑线程
	long				m_MenuCommand;			// 传递给图谱编辑线程的的命令ID
	CIRCameraMeasurementDockingBar*	m_pIRCameraMeasurementDockingBar;
	CLoadImageForAnalysisWnd		m_wndLoadImage;

	CUltraToolsDoc* GetDocument(BOOL bGlobalDoc=TRUE) const;

// Operations
public:
	BOOL GetImageXY(long *x, long *y);
	void SetModified(BOOL bModified=TRUE)
	{
		m_wndLoadImage.SetModified(bModified);
		//CUltraToolsDoc* pDoc = GetDocument(FALSE);
		CDocument* pDoc = CView::GetDocument();			// 在MainFrame关闭时，也能激活是否保存
		pDoc->SetModifiedFlag(bModified);
	}

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
// public:
// 	virtual ~CAnalysisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CTabCtrl			m_tabAnalysis;
	BOOL				m_bEndInit;

	CBrush				m_brHatch;						// FormView背景刷

	int					m_nCurReportTemplateIndex;		// 报表模板文件序号
	CString				m_sCurReportTemplateName;		// 选中的模板名（不含后缀和路径的文件名）
	CString				m_sCurAnalyst;					// 当前分析者，不一定是当前登录用户，在分析界面用户可能输入更改(保存报表时更新)

	void RepositionCtrls();								// 重新排列界面中的控件
	void CenterImage();									// 暂未使用，若图谱分析面板“嵌入”FormView，则启用

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate(); // called first time after construct
 	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	LRESULT OnUpdateImage(WPARAM wParam, LPARAM lParam);

	//--------------
	// 切换当前图谱
	afx_msg void OnFirstImage();
	afx_msg void OnPreImage();
	afx_msg void OnNextImage();
	afx_msg void OnLastImage();
	afx_msg void OnUpdateFirstImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePreImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLastImage(CCmdUI* pCmdUI);

	//---------------------------------------------------------------
	// 绘制测温objs(绘制obj不算图谱编辑，以便切换图谱时“共享”objs)
	afx_msg void OnViewIRCameraMeasurementDockingBar();
	afx_msg void OnUpdateViewIRCameraMeasurementDockingBar(CCmdUI* pCmdUI);
	afx_msg void OnDrawSelect();
	afx_msg void OnDelSelected();
	afx_msg void OnDelAllObjs();
	afx_msg void OnDrawSpot();
	afx_msg void OnDrawLine();
	afx_msg void OnDrawRect();
	afx_msg void OnShowObjs();
	afx_msg void OnSetObjsTipType(UINT id);
	afx_msg void OnShowPaletteScale();
	afx_msg void OnShowPalette();
	afx_msg void OnUpdateDrawSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelSelected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelAllObjs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawSpot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawRect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowObjs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetObjsTipType(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowPaletteScale(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowPalette(CCmdUI* pCmdUI);
	LRESULT OnObjModified(WPARAM wParam, LPARAM lParam);

	//----------
	// 图谱加载
	afx_msg void OnSelectAndLoadVLImage();
	BOOL LoadCoupleImage(CString sImageFilePathAndName);		// 加载参数指定的文件之后，尝试加载其配对文档
	afx_msg void OnSelectReportTemplate();

	//-----------
	// 图谱编辑
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnUpdateViewZoomin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);
	afx_msg void OnViewActualSize();
	afx_msg void OnUpdateViewActualSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnableModify(CCmdUI* pCmdUI);
	afx_msg void OnViewRotateAntiClockwise();
	afx_msg void OnViewRotateClockwise();
	afx_msg void OnUpdateEnableRotateImage(CCmdUI* pCmdUI);

	afx_msg void OnCreateAnalysisReportView();			// 临时保存分析结果，并生成报表
	afx_msg void OnSaveAnalysisResult();				// 永久性保存分析结果，并生成报表
	afx_msg void OnSaveAs();							// 将分析处理后的红外图谱另存为
	afx_msg void OnUpdateCreateAnalysisReportView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveAnalysisResult(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveAs(CCmdUI* pCmdUI);

	BOOL SaveCurIRImageAsTempFile(CString sFileName, int nFileType=THERMAL_IMAGE_FORMAT_UNKNOWN);	// 将当前红外图谱编辑结果保存到临时文件夹，以便入库
};

#ifndef _DEBUG  // debug version in CAnalysisView.cpp
inline CUltraToolsDoc* CAnalysisView::GetDocument(BOOL bGlobalDoc) const
{
	if(bGlobalDoc)
		return &theApp.m_docIrImage;
	return (CUltraToolsDoc*)m_pDocument;
}
#endif

