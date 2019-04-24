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
	HANDLE				m_hImageEditThread;		// ͼ�ױ༭�߳�
	long				m_MenuCommand;			// ���ݸ�ͼ�ױ༭�̵߳ĵ�����ID
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
		CDocument* pDoc = CView::GetDocument();			// ��MainFrame�ر�ʱ��Ҳ�ܼ����Ƿ񱣴�
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

	CBrush				m_brHatch;						// FormView����ˢ

	int					m_nCurReportTemplateIndex;		// ����ģ���ļ����
	CString				m_sCurReportTemplateName;		// ѡ�е�ģ������������׺��·�����ļ�����
	CString				m_sCurAnalyst;					// ��ǰ�����ߣ���һ���ǵ�ǰ��¼�û����ڷ��������û������������(���汨��ʱ����)

	void RepositionCtrls();								// �������н����еĿؼ�
	void CenterImage();									// ��δʹ�ã���ͼ�׷�����塰Ƕ�롱FormView��������

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate(); // called first time after construct
 	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	LRESULT OnUpdateImage(WPARAM wParam, LPARAM lParam);

	//--------------
	// �л���ǰͼ��
	afx_msg void OnFirstImage();
	afx_msg void OnPreImage();
	afx_msg void OnNextImage();
	afx_msg void OnLastImage();
	afx_msg void OnUpdateFirstImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePreImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLastImage(CCmdUI* pCmdUI);

	//---------------------------------------------------------------
	// ���Ʋ���objs(����obj����ͼ�ױ༭���Ա��л�ͼ��ʱ������objs)
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
	// ͼ�׼���
	afx_msg void OnSelectAndLoadVLImage();
	BOOL LoadCoupleImage(CString sImageFilePathAndName);		// ���ز���ָ�����ļ�֮�󣬳��Լ���������ĵ�
	afx_msg void OnSelectReportTemplate();

	//-----------
	// ͼ�ױ༭
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

	afx_msg void OnCreateAnalysisReportView();			// ��ʱ�����������������ɱ���
	afx_msg void OnSaveAnalysisResult();				// �����Ա����������������ɱ���
	afx_msg void OnSaveAs();							// �����������ĺ���ͼ�����Ϊ
	afx_msg void OnUpdateCreateAnalysisReportView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveAnalysisResult(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveAs(CCmdUI* pCmdUI);

	BOOL SaveCurIRImageAsTempFile(CString sFileName, int nFileType=THERMAL_IMAGE_FORMAT_UNKNOWN);	// ����ǰ����ͼ�ױ༭������浽��ʱ�ļ��У��Ա����
};

#ifndef _DEBUG  // debug version in CAnalysisView.cpp
inline CUltraToolsDoc* CAnalysisView::GetDocument(BOOL bGlobalDoc) const
{
	if(bGlobalDoc)
		return &theApp.m_docIrImage;
	return (CUltraToolsDoc*)m_pDocument;
}
#endif

