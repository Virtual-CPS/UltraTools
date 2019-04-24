// ExplorerView.h : header file
//

#pragma once

#include "UltraTools.h"
#include "UI/SplitterBar.h"
#include "UI/CheckListCtrl.h"


#include <vector>

class CThermalImage;

class CExplorerView : public CBCGPFormView
{
protected:
	CExplorerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExplorerView)

// Form Data
public:
	enum{ IDD = IDD_EXPLOER_FORM_VIEW };

// Attributes
public:
	CBCGPShellTree		m_wndShellTree;
	CCheckListCtrl		m_wndSourceList;
	int					m_nCurItem;
	BOOL				m_bGenerateReportInOrder;
	BOOL				m_bIsMakingReport;
	BOOL				m_bIsMergingReport;
	BOOL				m_bMakeReportTerminated;
	BOOL				m_bMergeReportTerminated;
	HANDLE				m_hMakeReport;
	HANDLE				m_hMergeReport;
	int					m_nPreviewOrPrint;				// 0=预览，1=打印+保存，2=仅保存
	CString				m_sRootDirectory;				// 因含嵌套子目录，只能保存“搜索入口”目录

// Operations
public:
	CUltraToolsDoc* GetDocument(BOOL bGlobalDoc=TRUE);
	void MakeReport(int nItemIndex);
	void MergeReport(CString sPath);

// Overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual ~CExplorerView();
	void SplitFormView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CExplorerView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	virtual void OnInitialUpdate();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnSplitterMovedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewExplorerTree();
	afx_msg void OnUpdateViewExplorerTree(CCmdUI* pCmdUI);
	afx_msg void OnViewImagePreview();
	afx_msg void OnUpdateViewImagePreview(CCmdUI* pCmdUI);
	afx_msg void OnListSortByName();
	afx_msg void OnUpdateListSortByName(CCmdUI* pCmdUI);
	afx_msg void OnListSortByDatetime();
	afx_msg void OnUpdateListSortByDatetime(CCmdUI* pCmdUI);
	afx_msg void OnReportPreview();
	afx_msg void OnReportGeneration();
	afx_msg void OnReportGenerationReversely();
	afx_msg void OnReportMerge();
	afx_msg void OnCopyTo();
	afx_msg void OnMoveTo();
	afx_msg void OnUpdateCopyOrMoveFile(CCmdUI* pCmdUI);
	afx_msg void OnAnalysis();

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);	// tree和list关联后捕获不到该消息
	void SelectFolder(CString sPath);
	afx_msg void OnListItemSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	void SelectItem(int nIndex);

	// 切换到分析诊断
	void CreateAnalysisView();
	afx_msg void OnDblClkImagesList(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnDblclkShellList(WPARAM wParam, LPARAM lParam);			// 双击shellList
	afx_msg void OnViewAnalysisFormview();
	afx_msg void OnUpdateViewAnalysisFormview(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

protected:
	BOOL				m_bEndInit;					// 界面初始化已结束
	CSplitterBar		m_wndVSplitterBar;			// 垂直分割条
	CSplitterBar		m_wndHSplitterBar;			// 水平分割条
	CBCGPToolBar		m_wndToolBar;
	CStatic				m_ctrlPreviewWndLocation;
	BOOL				m_bShowExplorerTree;
	BOOL				m_bShowImagePreviewWnds;

	CStatic				m_wndCanvas[3];
	CThermalImage*		m_pSelectedImages[3];

	CStringArray		m_saFileFilters;
	BOOL				m_bUnusePicOfIrCameraUnknow;	// 不使用红外热像仪未知的图谱
	int					m_nCurPreviewWndCnt;			// 1~3

	// Sys.ini
	BOOL				m_bSavePreviewFileToSysTempPath;
	BOOL				m_bPrintReportWhileGenerated;
	BOOL				m_bMoveImageFileWhileReportSaved;
	BOOL				m_bMoveReportFileWhileReportMerged;
	CString				m_sReportTitlePrefix;

	// 用于切换变电站时重置序号（重新从1开始计数）
	CString				m_sCurTsName;

	DWORD				m_dwMakeReportThreadID;
	DWORD				m_dwMergeReportThreadID;


protected:
	void InitSourceFileList();
	void UpdateSourceFileList();

	void FindSourceFile(CString sPath);
	void MakeFilesGroup();								// 使可见光图谱与红外图谱成对
	void UpdateListIndexCol(CCheckListCtrl &pList, int nIndexColNum);

	void ArrangePreviewWnds();
	void SetCanvasPosition(CRect& rcPosition, int nIndex);

	BOOL MakeFileThumbnail(CString sFileName, int nIndex);

	void GetFileBaseInfo(int nItemIndex, struct tagImageFileBaseInfo& baseInfo);
	int GetFileData(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData);
	int GetFileData(CString sFileName, struct tagImageFileData& fileData, CString sPhaseGroup,int nCurPhaseIndex);

	void StartMakeReportThread();
	BOOL TerminateMakeReportThread();
	BOOL IsMakingReport(){ return m_bIsMakingReport; }

	void StarMergeReportThread();
	BOOL TerminateMergeReportThread();

};

#ifndef _DEBUG  // debug version in ExplorerView.cpp
inline CUltraToolsDoc* CExplorerView::GetDocument(BOOL bGlobalDoc)
{
	if(bGlobalDoc)
		return &theApp.m_docIrImage;
	return (CUltraToolsDoc*) m_pDocument;
}
#endif
