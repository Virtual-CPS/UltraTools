// VideoDirectShowWnd.h : interface of the CVideoDirectShowWnd class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(VIDEO_DIRECT_SHOW_WND_H_)
#define VIDEO_DIRECT_SHOW_WND_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "SysMark.h"
#include "../MeasurementObj/DrawObj.h"
#include "PrintImageDlg.h"

// Hints for UpdateObjs()
#define HINT_UPDATE_WINDOW      0
#define HINT_UPDATE_DRAWOBJ     1
#define HINT_UPDATE_SELECTION   2
#define HINT_DELETE_SELECTION   3

#define OBJ_MODIFIED_FLAG_ADD		1		// ����
#define OBJ_MODIFIED_FLAG_MODIFY	2		// �޸�
#define OBJ_MODIFIED_FLAG_DEL		3		// ɾ��

#define OBJ_TIP_TYPE_DEFAULT		0		// ����obj��Ĭ����ʾ
#define OBJ_TIP_TYPE_INDEX			1		// ��ʾindex
#define OBJ_TIP_TYPE_TEMPERATURE	2		// ��ʾ�¶�
#define OBJ_TIP_TYPE_JUDGE			3		// ��ʾ�����ȵ㡱�������������塱��������(���������������뷢�ȵ���ͬһ����ͼ�У�obj����˳����ʾ)

struct tagSpotTemp
{
	BOOL bShow;
	float fTemp;

	tagSpotTemp()
	{
		bShow = FALSE;			// Ĭ����ʾ
		fTemp = 0.f;
	}
};
typedef CArray<tagSpotTemp, tagSpotTemp>	SPOT_TEMP_ARRAY;	// ��¼����spot�¶ȵ�����

struct tagBoxTemp
{
	short nShowIndex;			//	�¶���������ʾ��0=����ʾ��1=��ʾfMax��2=��ʾfMin��...
	float fMax;
	float fMin;
	float fAvg;
	float fIso;

	tagBoxTemp()
	{
		nShowIndex = 1;			// Ĭ����ʾ���ֵ
		fMax = 0.f;
		fMin = 0.f;
		fAvg = 0.f;
		fIso = 0.f;
	}
};
typedef CArray<tagBoxTemp, tagBoxTemp>	BOX_TEMP_ARRAY;	// ��¼����box�¶ȵ�����

//#define SHOW_LIVE_VIDEO
#define	SHOW_STATICE_IMAGE

#ifdef SHOW_LIVE_VIDEO
	class CSingleVideoWnd;
	class CVideoMediaRealtimeCapture;
#else ifdef SHOW_STATICE_IMAGE
	class CLoadImageForAnalysisWnd;
#endif

class CVideoDirectShowWnd : public CStatic
{
// Attributes
public:
	CVideoDirectShowWnd();
	virtual ~CVideoDirectShowWnd();

#ifdef SHOW_LIVE_VIDEO
	CVideoMediaRealtimeCapture* m_pCap;
	CSingleVideoWnd*			m_pOwner;
#else ifdef SHOW_STATICE_IMAGE
	CLoadImageForAnalysisWnd*	m_pOwner;
#endif

	BOOL						m_bCameraTypeIsIR;
	BOOL						m_bVideoUseAsNavigation;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

#ifdef TSMO_SYS
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuSaveAs();
	afx_msg void OnMenuSaveall();
	afx_msg void OnMenuPrintImage();
	afx_msg void OnMenuCameraSetup();
#endif

	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ResetAllObjsPosition();	// client�����С�ı���ض�λobjs

public:

	//=========================================================
	//���Ʋ���spot/line/area
	BOOL IsObjSelected(const CObject* pDocItem) const;
	void SelectObj(CDrawObj* pObj, BOOL bAdd = FALSE);
	void SelectObjWithinRect(CRect rect, BOOL bAdd = FALSE);
	void DeselectObj(CDrawObj* pObj);
	void UpdateObjs(int nHint, CObject* pHint);
	void InvalidateObj(CDrawObj* pObj);

	CDrawObj* ObjectAt(const CPoint& point);
	void DrawObjs(CDC* pDC);
	void AddObj(CDrawObj* pObj, BOOL bRecoverFromDB=FALSE);
	void RemoveObj(CDrawObj* pObj);

	afx_msg void OnDrawSelect();
	afx_msg void OnDelSelected();
	afx_msg void OnDelAll();
	afx_msg void OnDrawSpot();
	afx_msg void OnDrawLine();
	afx_msg void OnDrawRect();
	afx_msg void OnSetIRCamerMeasermentTool();			// ����IRCamera����spot/aera/isotherm
	afx_msg void OnUpdateDrawSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelSelected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawSpot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawRect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetIRCamerMeasermentTool(CCmdUI* pCmdUI);

	// ��ȡ��ǰspot/area/isotherm������
	int GetObjSpotCount();
	int GetObjBoxCount();
	int GetObjLineCount();

	CDrawObjList	m_objListSelection;
	CDrawObjList	m_objListObjects;
	void SetObjModifiedFlag(int nModifiedFlag=2, CDrawObj* pObj=NULL, BOOL bRecoverFromDB=FALSE);

	CList<SPOT_TEMP_ARRAY*, SPOT_TEMP_ARRAY*>	m_lstSpotTemp;	// ȫ��spots�¶�
	CList<BOX_TEMP_ARRAY*, BOX_TEMP_ARRAY*>		m_lstBoxTemp;	// ȫ��boxes�¶�
	void RemoveAllTempData();

	void ShowObjs(BOOL bShow=TRUE){SelectObj(NULL);m_bShowObjs=bShow;}
	BOOL ObjsIsVisible(){return m_bShowObjs;}
	void SetObjTipType(int nType)
	{
		if(nType<OBJ_TIP_TYPE_DEFAULT || nType>OBJ_TIP_TYPE_JUDGE)
			m_nObjTipType = OBJ_TIP_TYPE_DEFAULT;
		else
			m_nObjTipType = nType;
	}
	int GetObjTipType()
	{
		return m_nObjTipType;
	}

protected:

	CMenu			m_PopupMenu;
	BOOL			m_bObjModified;
	BOOL			m_bShowObjs;
	int				m_nObjTipType;
	CRect			m_rectDefaultArea;	// videoȱʡ��ʾλ�ã��ݴ���ǰ��rect���뵱ǰ�ıȽϣ���������

#ifdef SHOW_LIVE_VIDEO
	BOOL SetAllObjsToCamera();
	BOOL RemoveAllObjsFromCamera();
	BOOL SetObjToCamera(CDrawObj* pObj, BOOL bAdd=TRUE);
	BOOL RemoveObjFromCamera(CDrawObj* pObj);
	BOOL SetObjSpotToCamera(CDrawRect* pObj, BOOL bAdd=TRUE);
	BOOL ConvertObjSpotToCameraCoordinate(CDrawObj* pObj, CPoint& ptCameraSpot);
	BOOL SetObjRectToCamera(CDrawRect* pObj, BOOL bAdd=TRUE);
	BOOL ConvertObjRectToCameraCoordinate(CDrawObj* pObj, CRect& rectCamera);
	BOOL SetObjLineToCamera(CDrawRect* pObj, BOOL bAdd=TRUE);
	BOOL ConvertObjLineToCameraCoordinate(CDrawObj* pObj, CPoint& ptFrom, CPoint ptTo);
#endif
	//=========================================================
};

#endif // !defined(VIDEO_DIRECT_SHOW_WND_H_)
