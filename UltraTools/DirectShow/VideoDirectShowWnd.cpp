// VideoDirectShowWnd.cpp : implementation of the CVideoDirectShowWnd class
//

#include "stdafx.h"
#include "../UltraTools.h"

#ifdef ULTRA_TOOLS_LIVE_VIDEO
	#include "SingleVideoWnd.h"
	#include "VideoMediaRealtimeCapture.h"
	#include "GlobalDefsForSys.h"
#else ifdef ULTRA_TOOLS_STATIC_IMAGE
 	#include "../LoadImageForAnalysisWnd.h"
	#include "../MainFrm.h"
#endif

#include "VideoDirectShowWnd.h"
#include "../MeasurementObj/DrawTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CVideoDirectShowWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
#ifdef ULTRA_TOOLS_LIVE_VIDEO
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_VIDEO_OUTPUT_WND_IMAGE_SAVEAS, OnMenuSaveAs)
	ON_COMMAND(IDM_VIDEO_OUTPUT_WND_ALL_IMAGE_SAVEAS, OnMenuSaveall)
	ON_COMMAND(IDM_VIDEO_OUTPUT_WND_IMAGE_PRINT, OnMenuPrintImage)
	ON_COMMAND(IDM_VIDEO_OUTPUT_WND_CAMERA_SETUP, OnMenuCameraSetup)
#endif
	ON_WM_SIZE()
END_MESSAGE_MAP()

CVideoDirectShowWnd::CVideoDirectShowWnd()
{
#ifdef TSMO_SYS
	m_pCap = NULL;
#endif

	m_pOwner = NULL;
	m_bCameraTypeIsIR = TRUE;
	m_bVideoUseAsNavigation = FALSE;
	m_bShowObjs = TRUE;
	m_nObjTipType = OBJ_TIP_TYPE_DEFAULT;
	m_rectDefaultArea.SetRectEmpty();
}

CVideoDirectShowWnd::~CVideoDirectShowWnd()
{
	// m_objListSelection是m_objListObjects的“子集”，不必逐一delete处理
	m_objListSelection.RemoveAll();

	POSITION pos = m_objListObjects.GetHeadPosition();
	while (pos != NULL)
		delete m_objListObjects.GetNext(pos);

	pos = m_lstSpotTemp.GetHeadPosition();
	while (pos != NULL)
		delete m_lstSpotTemp.GetNext(pos);
	m_lstSpotTemp.RemoveAll();

	pos = m_lstBoxTemp.GetHeadPosition();
	while (pos != NULL)
		delete m_lstBoxTemp.GetNext(pos);
	m_lstBoxTemp.RemoveAll();
}

BOOL CVideoDirectShowWnd::OnEraseBkgnd(CDC* pDC)
{
#ifdef TSMO_SYS
	ASSERT(m_pCap && m_pOwner);

	HRESULT hr;
	TRACE(_T("VideoDirectShowWnd::OnEraseBkgnd\r\n"));
	if (m_pCap && m_pCap->GetPlayState()!=PLAY_STATE_NOT_INIT)
	{
		hr = m_pCap->RepaintVideo(pDC->m_hDC);
		if (FAILED(hr))
		{
			if(m_bShowObjs)
				DrawObjs(pDC);
			return CWnd::OnEraseBkgnd(pDC);
		}
	}
#else ifdef IR_IMAGE_ANALYSI_SYS
	ASSERT(m_pOwner);
	if(m_pOwner && m_pOwner->m_pIRImage)
	{
		CRect rectClient;
		GetClientRect(rectClient);
		m_pOwner->m_pIRImage->Draw(pDC->m_hDC, rectClient, NULL, TRUE);
	}
#endif

	if(m_bShowObjs)
		DrawObjs(pDC);

	return CWnd::OnEraseBkgnd(pDC);
}

void CVideoDirectShowWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bVideoUseAsNavigation || !m_bCameraTypeIsIR)
		return;

	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
	{
		pTool->OnLButtonDown(this, nFlags, point);
	}
	// 	else
	// 		CDialog::OnLButtonDown(nFlags, point);
}

void CVideoDirectShowWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bVideoUseAsNavigation || !m_bCameraTypeIsIR)
		return;

	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonUp(this, nFlags, point);
}

void CVideoDirectShowWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bVideoUseAsNavigation || !m_bCameraTypeIsIR)
		return;

	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnMouseMove(this, nFlags, point);

#ifdef IR_IMAGE_ANALYSI_SYS
	if (NULL!=m_pOwner && NULL!=m_pOwner->m_pIRImage)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame == NULL)
			return;

		long x = point.x;
		long y = point.y;
		m_pOwner->GetImageXY(&x, &y);

		TCHAR s[180];
		CString sTem;
		if (m_pOwner->m_pIRImage->IsInside(x,y))
		{
			long yflip = m_pOwner->m_pIRImage->GetHeight() - y - 1;
			//sprintf_s(s, _T("坐标[x:%d, y:%d]"), x, y);
			sTem.Format(_T("坐标[x:%d, y:%d]"), x, y);
// 			RGBQUAD rgb = m_pOwner->m_pIRImage->GetPixelColor(x, yflip);
// 			if (m_pOwner->m_pIRImage->AlphaIsValid())
// 				rgb.rgbReserved = m_pOwner->m_pIRImage->AlphaGet(x, yflip);
// 			else
// 				rgb.rgbReserved = m_pOwner->m_pIRImage->GetPaletteColor(m_pOwner->m_pIRImage->GetPixelIndex(x,yflip)).rgbReserved;
// 			_stprintf(&s[_tcslen(s)],_T("  RGBA(%d, %d, %d, %d)"), rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue, rgb.rgbReserved);

			float fTemp;
			m_pOwner->GetSpotTemperature(point, fTemp);
			//_stprintf(&s[_tcslen(s)],_T("  温度:%.1f ℃"), fTemp);
			sprintf_s(s,_T("%s  温度:%.1f ℃"), sTem, fTemp);

			pFrame->SetStatusBarMsg(s);
		}
	}
#endif
}

#ifdef TSMO_SYS

void CVideoDirectShowWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(m_bVideoUseAsNavigation || !m_bCameraTypeIsIR)
		return;

	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonDblClk(this, nFlags, point);

//	CWnd::OnLButtonDblClk(nFlags, point);
}

void CVideoDirectShowWnd ::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(!::IsMenu(m_PopupMenu))							// 快捷菜单尚未加载
		m_PopupMenu.LoadMenu(IDR_VIDEO_OUTPUT_WND_POPUP_MENU);

	char szPrompt1[256];
	char szPrompt2[256];
	char szPrompt3[256];
	CRect rect;

	GetWindowRect(rect);

	int x = rect.TopLeft().x + point.x;
	int y = rect.TopLeft().y + point.y;

	memset(&szPrompt1, 0, sizeof(szPrompt1));
	memset(&szPrompt2, 0, sizeof(szPrompt2));
	memset(&szPrompt3, 0, sizeof(szPrompt3));

	sprintf_s(szPrompt1, "保存 [%s] 图谱...", m_pOwner->m_sCaption);
	sprintf_s(szPrompt2, "打印 [%s] 图谱...", m_pOwner->m_sCaption);
	sprintf_s(szPrompt3, "设置 [%s] 设备", m_pOwner->m_sCaption);

	UINT new_id1 = m_PopupMenu.GetSubMenu(0)->GetMenuItemID(0);
	m_PopupMenu.GetSubMenu(0)->ModifyMenu(IDM_VIDEO_OUTPUT_WND_IMAGE_SAVEAS, MF_BYCOMMAND|MF_STRING, new_id1, (LPCTSTR)(LPSTR)szPrompt1);

	UINT new_id2 = m_PopupMenu.GetSubMenu(0)->GetMenuItemID(3);
	m_PopupMenu.GetSubMenu(0)->ModifyMenu(IDM_VIDEO_OUTPUT_WND_IMAGE_PRINT, MF_BYCOMMAND|MF_STRING, new_id2, (LPCTSTR)(LPSTR)szPrompt2);

	UINT new_id3 = m_PopupMenu.GetSubMenu(0)->GetMenuItemID(5);
	m_PopupMenu.GetSubMenu(0)->ModifyMenu(IDM_VIDEO_OUTPUT_WND_CAMERA_SETUP, MF_BYCOMMAND|MF_STRING, new_id3, (LPCTSTR)(LPSTR)szPrompt3);

	m_PopupMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, this);

	CWnd::OnRButtonDown(nFlags, point);
}

void CVideoDirectShowWnd ::OnMenuPrintImage() 
{
	ASSERT(m_pOwner);

	CPrintImageDlg	dlgPrint;
	dlgPrint.m_nVideoIndex = m_pOwner->m_nWndIndex;
	// 	dlgPrint.m_SrcImage.Copy(*m_pOwner->m_pVideoLogo);
	// 	dlgPrint.m_nChannelIndex = g_VideoPreviewWnds[m_nWndIndex].m_nChannelIndex;
	// 	dlgPrint.m_sDateTime = g_VideoPreviewWnds[m_nWndIndex].m_sDateTime;

	dlgPrint.DoModal();
}

void CVideoDirectShowWnd ::OnMenuCameraSetup()
{
	ASSERT(m_pOwner);
	m_pOwner->OnMenuCameraSetup();
}

void CVideoDirectShowWnd ::OnMenuSaveAs() 
{
	ASSERT(m_pOwner);
	int nWndIndex = m_pOwner->m_nWndIndex;

	char file_name[MAX_PATH];
	static char BASED_CODE OpenFilter[] = "JPG Files (*.jpg)|*.jpg; *.jpg||";

	sprintf_s(file_name, "image_%02d.jpg", nWndIndex+1);

	CFileDialog FileOpenDialog(
		FALSE,
		NULL,
		file_name,
		OFN_FILEMUSTEXIST |	OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		OpenFilter,
		this);

	if(FileOpenDialog.DoModal() == IDOK)
	{
		CString strFilePath = FileOpenDialog.GetPathName();

#if 1
		// Image resize
		CxImage* iDst = new CxImage;

		int		nWidth	= 720;
		int		nHeight = 480;

		if(g_ImageArray[nWndIndex].GetWidth() > 600)
		{
			nWidth	= 720;
			nHeight = g_nVideoFormat == PAL ? 480 : 576;
		}
		else
		{
			nWidth	= 320;
			nHeight = g_nVideoFormat == PAL ? 240 : 288;
		}

		g_ImageArray[nWndIndex].Resample(nWidth, nHeight, 1, iDst);
		iDst->Save(strFilePath, THERMAL_IMAGE_FORMAT_JPG);
		if(iDst != NULL)
			delete iDst;
#else
		m_pOwner->g_ImageArray[nWndIndex].Save(strFilePath, THERMAL_IMAGE_FORMAT_JPG);
#endif
	}
}

void CVideoDirectShowWnd ::OnMenuSaveall() 
{
	static char		BASED_CODE OpenFilter[] = "JPG Files (*.jpg)|*.jpg; *.jpg||";
	CString			strIndex;

	CFileDialog FileOpenDialog(	FALSE,
		NULL,
		"image",
		OFN_FILEMUSTEXIST |	OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		OpenFilter,
		this );

	if(FileOpenDialog.DoModal() == IDOK)
	{
		for(int i = 0; i < g_iTotalChannelCnt; i++)
		{
			CString strFilePath = FileOpenDialog.GetPathName();
			strIndex.Format("_%02d.jpg", i+1);

#if 1
			// Image resize
			CxImage* iDst = new CxImage;

			int		nWidth	= 720;
			int		nHeight = 480;

			if(g_ImageArray[i].GetWidth() > 600)
			{
				nWidth	= 720;
				nHeight = g_nVideoFormat == PAL ? 480 : 576;
			}
			else
			{
				nWidth	= 320;
				nHeight = g_nVideoFormat == PAL ? 240 : 288;
			}

			g_ImageArray[i].Resample(nWidth, nHeight, 1, iDst);
			iDst->Save(strFilePath + strIndex, THERMAL_IMAGE_FORMAT_JPG);
			if(iDst != NULL)
				delete iDst;
#else
			g_ImageArray[i].Save(strFilePath + strIndex, THERMAL_IMAGE_FORMAT_JPG);
#endif
		}
	}
}

#endif		// #ifdef TSMO_SYS

//#############################################################################
//
//						绘制红外测温spot/area/isotherm
//
//#############################################################################

void CVideoDirectShowWnd::UpdateObjs(int nHint, CObject* pHint)
{
	switch (nHint)
	{
	case HINT_UPDATE_WINDOW:				// redraw entire window
		Invalidate(FALSE);
		break;

	case HINT_UPDATE_DRAWOBJ:				// a single object has changed
		InvalidateObj((CDrawObj*)pHint);
		break;

	case HINT_UPDATE_SELECTION:				// an entire selection has changed
		{
			CDrawObjList* pList = pHint!=NULL ?
				(CDrawObjList*)pHint : &m_objListSelection;
			POSITION pos = pList->GetHeadPosition();
			while (pos != NULL)
			{
				InvalidateObj(pList->GetNext(pos));
			}
		}
		break;

	case HINT_DELETE_SELECTION:				// an entire selection has been removed
		{
			CDrawObjList* pList = pHint!=NULL ?
				(CDrawObjList*)pHint : &m_objListSelection;
			POSITION pos = pList->GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = pList->GetNext(pos);
				InvalidateObj(pObj);
				RemoveObj(pObj);			// remove it from this  selection
				pObj->Remove();
			}
			pList->RemoveAll();
			Invalidate(TRUE);
#ifdef TSMO_SYS
			RemoveAllObjsFromCamera();		// 移除之后重设，只是为了重新“排序”
			SetAllObjsToCamera();
#endif
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}

void CVideoDirectShowWnd::InvalidateObj(CDrawObj* pObj)
{
	ASSERT(m_pOwner);

	CRect rect = pObj->m_rectPosition;

	if (m_pOwner->m_bIsActive && IsObjSelected(pObj))
	{
		rect.left -= 4;
		rect.top -= 5;
		rect.right += 5;
		rect.bottom += 4;
	}
	rect.InflateRect(1, 1); // handles CDrawOleObj objects

	CClientDC dc(this);
	pObj->Draw(&dc);
	if(IsObjSelected(pObj))
		pObj->DrawTracker(&dc, CDrawObj::selected);

	InvalidateRect(rect, FALSE);
}

BOOL CVideoDirectShowWnd::IsObjSelected(const CObject* pDocItem) const
{
	CDrawObj* pDrawObj = (CDrawObj*)pDocItem;

	return m_objListSelection.Find(pDrawObj) != NULL;
}

void CVideoDirectShowWnd::SelectObj(CDrawObj* pObj, BOOL bAdd)
{
	if(!m_bShowObjs)
		return;

	if (!bAdd)
	{
		//UpdateObjs(HINT_UPDATE_SELECTION, NULL);
		m_objListSelection.RemoveAll();
	}

	if (pObj == NULL || IsObjSelected(pObj))
	{
		return;
	}

	m_objListSelection.AddTail(pObj);
	InvalidateObj(pObj);
}

void CVideoDirectShowWnd::SelectObjWithinRect(CRect rect, BOOL bAdd)
{
	if(!m_bShowObjs)
		return;

	if (!bAdd)
		SelectObj(NULL);

	CDrawObjList* pObList = &m_objListObjects;
	POSITION posObj = pObList->GetHeadPosition();
	while (posObj != NULL)
	{
		CDrawObj* pObj = pObList->GetNext(posObj);
		if (pObj->Intersects(rect))
			SelectObj(pObj, TRUE);
	}

	ASSERT(m_pOwner);
	m_pOwner->Invalidate(FALSE);	// BFish add 2010.08.14
}

void CVideoDirectShowWnd::DeselectObj(CDrawObj* pObj)
{
	POSITION pos = m_objListSelection.Find(pObj);
	if (pos != NULL)
	{
		InvalidateObj(pObj);
		m_objListSelection.RemoveAt(pos);
	}
}

void CVideoDirectShowWnd::DrawObjs(CDC* pDC)
{
	ASSERT(m_pOwner);

	POSITION pos = m_objListObjects.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = m_objListObjects.GetNext(pos);

		pObj->Draw(pDC);

		if (m_pOwner->m_bIsActive && !pDC->IsPrinting() && IsObjSelected(pObj))
			pObj->DrawTracker(pDC, CDrawObj::selected);
	}
}

// bRecoverFromDB=TRUE: 不通知parents已更新，避免重复动作
void CVideoDirectShowWnd::AddObj(CDrawObj* pObj, BOOL bRecoverFromDB)
{
	CDrawRect* pRect = (CDrawRect*)pObj;
	CDrawRect::Shape eShape = pRect->GetShapType();
	int nCnt = 0;

	// 分类更新Obj的序号（index based on zero）
	if(eShape == CDrawRect::RECT_SHAPE_SPOT)
		nCnt = GetObjSpotCount();
	else if (eShape == CDrawRect::RECT_SHAPE_LINE)
		nCnt = GetObjLineCount();
	else if(eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
		nCnt = GetObjBoxCount();
	pObj->m_nIndex = nCnt;

	if(m_nObjTipType == OBJ_TIP_TYPE_JUDGE)
	{
		pObj->EnableShowTip(TRUE);

		nCnt = GetObjSpotCount() + GetObjLineCount() + GetObjBoxCount();
		if(nCnt == 0)
			pObj->SetTipString(_T("发热点"));
		else if(nCnt == 1)
			pObj->SetTipString(_T("环境参照体"));
		else if(nCnt == 2)
			pObj->SetTipString(_T("正常"));
	}
	else if(m_nObjTipType == OBJ_TIP_TYPE_TEMPERATURE)
	{
		pObj->EnableShowTip(TRUE);
		pObj->DelTipString();
	}
	else if(m_nObjTipType == OBJ_TIP_TYPE_INDEX)
		pObj->EnableShowTip(FALSE);

	pObj->m_pVideoWnd = this;
	m_objListObjects.AddTail(pObj);

	m_bShowObjs = TRUE;

	if(eShape == CDrawRect::RECT_SHAPE_SPOT)
	{
		SPOT_TEMP_ARRAY* pItem = new(SPOT_TEMP_ARRAY);
		m_lstSpotTemp.AddTail(pItem);
	}
	else if (eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
	{
		BOX_TEMP_ARRAY* pItem = new(BOX_TEMP_ARRAY);
		m_lstBoxTemp.AddTail(pItem);
	}

	SetObjModifiedFlag(OBJ_MODIFIED_FLAG_ADD, pObj, bRecoverFromDB);
}

void CVideoDirectShowWnd::RemoveObj(CDrawObj* pObj)
{
	POSITION pos = m_objListObjects.Find(pObj);
	if (pos == NULL)									// 未找到
		return;

	CDrawRect* pRect = (CDrawRect*)pObj;
	CDrawRect::Shape eShape = pRect->GetShapType();
	int nIndex = pObj->m_nIndex;

	m_objListObjects.RemoveAt(pos);						// 删除

	// 分类更新剩余obj的序号(index based on zero)
	CDrawObj* pObjTem;
	int nCnt=0, nJudgeIndex=0;
	pos = m_objListObjects.GetHeadPosition();
	while (pos != NULL)
	{
		pObjTem = m_objListObjects.GetNext(pos);
		if(((CDrawRect*)pObjTem)->GetShapType() == eShape)
		{
			pObjTem->m_nIndex = nCnt;
			nCnt++;
		}

		if (m_nObjTipType == OBJ_TIP_TYPE_JUDGE)
		{
			if(nJudgeIndex == 0)
				pObjTem->SetTipString(_T("发热点"));
			else if(nJudgeIndex == 1)
				pObjTem->SetTipString(_T("环境参照体"));
			else if(nJudgeIndex == 2)
				pObjTem->SetTipString(_T("正常"));

			pObjTem->UpdateTipPosition(this);		// 根据新的tip内容重新计算
			nJudgeIndex++;
		}
	}

	SetObjModifiedFlag(OBJ_MODIFIED_FLAG_DEL, pObj);

	if(eShape == CDrawRect::RECT_SHAPE_SPOT)
	{
		SPOT_TEMP_ARRAY* pItem;
		pos = m_lstSpotTemp.FindIndex(nIndex);
		if(pos != NULL)
		{
			pItem = m_lstSpotTemp.GetAt(pos);
			m_lstSpotTemp.RemoveAt(pos);
			delete pItem;
		}
	}
	else if (eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
	{
		BOX_TEMP_ARRAY* pItem;
		pos = m_lstBoxTemp.FindIndex(nIndex);
		if(pos != NULL)
		{
			pItem = m_lstBoxTemp.GetAt(pos);
			m_lstBoxTemp.RemoveAt(pos);
			delete pItem;
		}
	}
}

// point in which obj
CDrawObj* CVideoDirectShowWnd::ObjectAt(const CPoint& point)
{
	CRect rect(point, CSize(3, 3));
	POSITION pos = m_objListObjects.GetTailPosition();
	CDrawObj* pObj;
	while (pos != NULL)
	{
		pObj = m_objListObjects.GetPrev(pos);
		if (pObj->Intersects(rect))
			return pObj;
	}

	return NULL;
}

void CVideoDirectShowWnd::OnDrawSelect()
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		CDrawTool::c_drawShape = DRAW_OBJ_SHAPE_SELECTION;
}

void CVideoDirectShowWnd::OnUpdateDrawSelect(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->SetRadio(CDrawTool::c_drawShape == DRAW_OBJ_SHAPE_SELECTION);
	else
		pCmdUI->Enable(FALSE);
}

void CVideoDirectShowWnd::OnDelSelected()
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
	{
		// remove the selected objects
		UpdateObjs(HINT_DELETE_SELECTION, &m_objListSelection);

		//Cleanup Tool members such as CRectTool::m_pDrawObj, that should be NULL at this point.
		CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
		if (pTool != NULL)
		{
			pTool->OnCancel();
		}
	}
}

void CVideoDirectShowWnd::OnUpdateDelSelected(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->Enable(!m_objListSelection.IsEmpty()&&m_bShowObjs);
	else
		pCmdUI->Enable(FALSE);
}

void CVideoDirectShowWnd::OnDelAll()
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
	{
#ifdef TSMO_SYS
		// 清除camera全部测温tools
		RemoveAllObjsFromCamera();
#endif
		// remove all the objects
		// 可能存在未选中的objs，故用m_objListObjects“代替”m_objListSelection
		UpdateObjs(HINT_DELETE_SELECTION, &m_objListObjects);

		m_objListSelection.RemoveAll();

		// Cleanup Tool members
		CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
		if (pTool != NULL)
		{
			pTool->OnCancel();
		}
	}
}

void CVideoDirectShowWnd::OnUpdateDelAll(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->Enable(!m_objListObjects.IsEmpty()&&m_bShowObjs);
	else
		pCmdUI->Enable(FALSE);
}

void CVideoDirectShowWnd::OnDrawSpot()
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		CDrawTool::c_drawShape = DRAW_OBJ_SHAPE_SPOT;
}

void CVideoDirectShowWnd::OnUpdateDrawSpot(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->SetRadio(CDrawTool::c_drawShape == DRAW_OBJ_SHAPE_SPOT);
	else
		pCmdUI->Enable(FALSE);
}

void CVideoDirectShowWnd::OnDrawLine()
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		CDrawTool::c_drawShape = DRAW_OBJ_SHAPE_LINE;
}

void CVideoDirectShowWnd::OnUpdateDrawLine(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->SetRadio(CDrawTool::c_drawShape == DRAW_OBJ_SHAPE_LINE);
	else
		pCmdUI->Enable(FALSE);
}

void CVideoDirectShowWnd::OnDrawRect()
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		CDrawTool::c_drawShape = DRAW_OBJ_SHAPE_RECT;
}

void CVideoDirectShowWnd::OnUpdateDrawRect(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->SetRadio(CDrawTool::c_drawShape == DRAW_OBJ_SHAPE_RECT);
	else
		pCmdUI->Enable(FALSE);
}

#ifdef ULTRA_TOOLS_LIVE_VIDEO
// 更新IRCamera测温spot/aera/isotherm
void CVideoDirectShowWnd::OnSetIRCamerMeasermentTool()
{
	ASSERT(m_pOwner);

	if(m_bCameraTypeIsIR && !m_pOwner->IsConnected())
	{
		AfxMessageBox(_T("尚未连接红外热像仪，已放弃更新测温"));
		return;
	}

	SetAllObjsToCamera();
}

void CVideoDirectShowWnd::OnUpdateSetIRCamerMeasermentTool(CCmdUI* pCmdUI)
{
	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
		pCmdUI->Enable(m_objListObjects.GetCount()>0 && m_bShowObjs);
}
#endif
// 
// void CVideoDirectShowWnd::OnSelectAll()
// {
// 	POSITION pos = m_objListObjects.GetHeadPosition();
// 	while (pos != NULL)
// 		SelectObj(m_objListObjects.GetNext(pos), TRUE);
// }
// 
// void CVideoDirectShowWnd::OnUpdateSelectAll(CCmdUI* pCmdUI)
// {
// 	pCmdUI->Enable(m_objListObjects.GetCount() > 0);
// }

// 获取m_objListObjects中spot数量
int CVideoDirectShowWnd::GetObjSpotCount()
{
	int nCnt = 0;
	POSITION pos = m_objListObjects.GetHeadPosition();
	CDrawObj* pObj;

	while (pos != NULL)
	{
		pObj = m_objListObjects.GetNext(pos);
		if(((CDrawRect*)pObj)->GetShapType() == CDrawRect::RECT_SHAPE_SPOT)
			nCnt++;
	}

	return nCnt;
}

// 获取m_objListObjects中rect数量
int CVideoDirectShowWnd::GetObjBoxCount()
{
	int nCnt = 0;
	POSITION pos = m_objListObjects.GetHeadPosition();
	CDrawObj* pObj;

	while (pos != NULL)
	{
		pObj = m_objListObjects.GetNext(pos);
		if(((CDrawRect*)pObj)->GetShapType() == CDrawRect::RECT_SHAPE_RECTANGLE)
			nCnt++;
	}

	return nCnt;
}

// 获取m_objListObjects中line数量
int CVideoDirectShowWnd::GetObjLineCount()
{
	int nCnt = 0;
	POSITION pos = m_objListObjects.GetHeadPosition();
	CDrawObj* pObj;

	while (pos != NULL)
	{
		pObj = m_objListObjects.GetNext(pos);
		if(((CDrawRect*)pObj)->GetShapType() == CDrawRect::RECT_SHAPE_LINE)
			nCnt++;
	}

	return nCnt;
}

///////////////////////////////////////////////////////////////////////////////
// 绘制/修改spot/area/isotherm与IRCamer“衔接”的枢纽：
//
// 更新spot/area/isotherm的位置后设置更新标志
// 同步更新IRCamer测温
//
// nModifiedFlag=1:增加pObj；2:修改；3：删除
// 利用pObj不需遍历链表检索谁被修改，以便快速更新IRCamer
//
// bRecoverFromDB=TRUE: 不通知parents已更新，避免重复动作
void CVideoDirectShowWnd::SetObjModifiedFlag(int nModifiedFlag, CDrawObj* pObj, BOOL bRecoverFromDB)
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);

	m_bObjModified = TRUE;

#ifdef ULTRA_TOOLS_LIVE_VIDEO
	if (NULL != pObj)
	{
		switch (nModifiedFlag)
		{
		case OBJ_MODIFIED_FLAG_ADD:
			SetObjToCamera(pObj, TRUE);
			break;

		case OBJ_MODIFIED_FLAG_MODIFY:
			SetObjToCamera(pObj, FALSE);
			break;

		case OBJ_MODIFIED_FLAG_DEL:
			RemoveObjFromCamera(pObj);
			break;

		default:
			ASSERT(FALSE);		// 尚不支持的类型
		}
	}
#endif

#ifdef ULTRA_TOOLS_STATIC_IMAGE
	if (pObj && pObj->m_bShowTip && nModifiedFlag!=OBJ_MODIFIED_FLAG_DEL)
	{
		CDrawRect* pRect = (CDrawRect*)pObj;
		CDrawRect::Shape eShape = pRect->GetShapType();

		if(eShape == CDrawRect::RECT_SHAPE_SPOT)
		{
			float fTemp;
			CRect rect = pObj->m_rectPosition;
			rect.NormalizeRect();
			CPoint ptSpot = rect.CenterPoint();
			if(m_pOwner->GetSpotTemperature(ptSpot, fTemp))
				pObj->m_faValues[0] = fTemp;
		}
		else if (eShape == CDrawRect::RECT_SHAPE_LINE)
		{
			float fMax, fMin, fAvg;
			CRect rect = pObj->m_rectPosition;
			CPoint ptFrom, ptTo;
			ptFrom = rect.TopLeft();
			ptTo = rect.BottomRight();
			if(m_pOwner->GetLineTemperature(ptFrom, ptTo, fMax, fMin, fAvg))
			{
				pObj->m_faValues[0] = fMax;
				pObj->m_faValues[1] = fMin;
				pObj->m_faValues[2] = fAvg;
			}
		}
		else if (eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
		{
			float fMax, fMin, fAvg;
			CRect rect = pObj->m_rectPosition;
			rect.NormalizeRect();
			if(m_pOwner->GetAreaTemperature(rect, fMax, fMin, fAvg))
			{
				pObj->m_faValues[0] = fMax;
				pObj->m_faValues[1] = fMin;
				pObj->m_faValues[2] = fAvg;
			}
		}
	}
	else if (NULL==pObj && nModifiedFlag==OBJ_MODIFIED_FLAG_MODIFY)
	{	// 更新全部
		CDrawObj* pObjTem;
		int nCnt = 0;
		POSITION pos = m_objListObjects.GetHeadPosition();
		while (pos != NULL)		// 不采用递归，避免反复发送更新消息
		{
			pObjTem = m_objListObjects.GetNext(pos);

			CDrawRect* pRect = (CDrawRect*)pObjTem;
			CDrawRect::Shape eShape = pRect->GetShapType();

			if(eShape == CDrawRect::RECT_SHAPE_SPOT)
			{
				float fTemp;
				CRect rect = pObjTem->m_rectPosition;
				rect.NormalizeRect();
				CPoint ptSpot = rect.CenterPoint();
				if(m_pOwner->GetSpotTemperature(ptSpot, fTemp))
					pObjTem->m_faValues[0] = fTemp;
			}
			else if (eShape == CDrawRect::RECT_SHAPE_LINE)
			{
				float fMax, fMin, fAvg;
				CRect rect = pObjTem->m_rectPosition;
				CPoint ptFrom, ptTo;
				ptFrom = rect.TopLeft();
				ptTo = rect.BottomRight();
				if(m_pOwner->GetLineTemperature(ptFrom, ptTo, fMax, fMin, fAvg))
				{
					pObjTem->m_faValues[0] = fMax;
					pObjTem->m_faValues[1] = fMin;
					pObjTem->m_faValues[2] = fAvg;
				}
			}
			else if (eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
			{
				float fMax, fMin, fAvg;
				CRect rect = pObjTem->m_rectPosition;
				rect.NormalizeRect();
				if(m_pOwner->GetAreaTemperature(rect, fMax, fMin, fAvg))
				{
					pObjTem->m_faValues[0] = fMax;
					pObjTem->m_faValues[1] = fMin;
					pObjTem->m_faValues[2] = fAvg;
				}
			}

			pRect->UpdateTipPosition(this);
		}

		Invalidate(TRUE);
	}
#endif

	if(bRecoverFromDB)
		return;

	if(m_pOwner && m_pOwner->GetSafeHwnd())
	{
		m_pOwner->SendMessage(WM_OBJ_MODIFIED, (WPARAM)pObj, (int)nModifiedFlag);

		CWnd* pParent = m_pOwner->GetParent();
		if(pParent && pParent->GetSafeHwnd())
			pParent->SendMessage(WM_OBJ_MODIFIED, (WPARAM)pObj, (int)nModifiedFlag);
	}
}

//#############################################################################
#ifdef ULTRA_TOOLS_LIVE_VIDEO

// 以非单纯增加方式将obj转换成camera的spot/area/isotherm
// 若camera中已有拟设置的测温tools，则自动覆盖原设置
BOOL CVideoDirectShowWnd::SetAllObjsToCamera()
{
	int nCnt = 0;
	POSITION pos = m_objListObjects.GetHeadPosition();
	CDrawObj* pObj;

	CWaitCursor cursorWait;

	while (pos != NULL)
	{
		pObj = m_objListObjects.GetNext(pos);
		if(SetObjToCamera(pObj, FALSE))			// 非单纯增加方式
			nCnt++;		// 允许某些失败，若退出，则可能放弃尚未遍历到的objs
	}

	return nCnt==m_objListObjects.GetCount();
}

// 清除camera全部spot/area/isotherm
// 不利用obj链表，以避免camera中已有不一致的“初始”设置
BOOL CVideoDirectShowWnd::RemoveAllObjsFromCamera()
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);
	ASSERT(m_pOwner);
	return m_pOwner->RemoveAllObjsFromCamera();
}

// 根据pObj的具体类型，将pObj设置为camera的spot/area/isotherm
// bAdd=TRUE:新增；bAdd=FALSE:更新
BOOL CVideoDirectShowWnd::SetObjToCamera(CDrawObj* pObj, BOOL bAdd)
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);

	if(NULL == pObj)
		return FALSE;

	CPoint ptCameraSpot;
	CRect ptCameraArea;

	CDrawRect* pRect = (CDrawRect*)pObj;
	CDrawRect::Shape eShape = pRect->GetShapType();
	if(eShape == CDrawRect::RECT_SHAPE_SPOT)
		return SetObjSpotToCamera(pRect, bAdd);
	else if(eShape == CDrawRect::RECT_SHAPE_LINE)
		return SetObjLineToCamera(pRect, bAdd);
	else if(eShape == CDrawRect::RECT_SHAPE_RECTANGLE)
		return SetObjRectToCamera(pRect, bAdd);

	return TRUE;
}

// 根据pObj的类型和序号清除camera中的相应类型tool
BOOL CVideoDirectShowWnd::RemoveObjFromCamera(CDrawObj* pObj)
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);
	ASSERT(m_pOwner);

	if(NULL == pObj)
		return FALSE;


	CDrawRect* pRect = (CDrawRect*)pObj;
	CDrawRect::Shape eShape = pRect->GetShapType();
	int nIndex = pRect->m_nIndex;

	// 判断nIndex的有效性
	if(nIndex < 0)
		return FALSE;

	// 设置摄像机
	return m_pOwner->RemoveObjFromCamera(nIndex, eShape);
}

BOOL CVideoDirectShowWnd::SetObjSpotToCamera(CDrawRect* pObj, BOOL bAdd)
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);

	if(NULL == pObj)
		return FALSE;

	CPoint ptCameraSpot;
	int nIndex = pObj->m_nIndex;

	CDrawRect::Shape eShape = pObj->GetShapType();
	if(eShape != CDrawRect::RECT_SHAPE_SPOT)
		return FALSE;

	// 判断nIndex的有效性
	if(nIndex < 0)
		return FALSE;

	// 坐标转换
	if(!ConvertObjSpotToCameraCoordinate(pObj, ptCameraSpot))
		return FALSE;

	// 设置摄像机
	return m_pOwner->SetObjSpotToCamera(nIndex, ptCameraSpot);
}

// 将绘制的spot对象的坐标转换成camera坐标，以便提取温度
// Image控件的宽高比应与camera相等，但图幅可能不相等
BOOL CVideoDirectShowWnd::ConvertObjSpotToCameraCoordinate(CDrawObj* pObj, CPoint& ptCameraSpot)
{
	ptCameraSpot = CPoint(0, 0);

	CDrawRect* pSpot = (CDrawRect*)pObj;

	if(pSpot==NULL || pSpot->GetShapType()!=CDrawRect::RECT_SHAPE_SPOT)
		return FALSE;

	CRect rectTarget;
	rectTarget.SetRectEmpty();
	rectTarget.right = 320;			// 根据camera替换常量
	rectTarget.bottom = 240;

	int nWidth;
	float fScale, fScale2;
	CRect rectSpot = pSpot->m_rectPosition;
	CRect rectClient;
	CPoint point;
	GetClientRect(rectClient);
	rectClient.NormalizeRect();

	// 检查宽高比一致
	fScale = 100.0f * rectTarget.Width()/rectTarget.Height();
	fScale2 = 100.0f * rectClient.Width()/rectClient.Height();
	ASSERT((int)fScale == (int)fScale2);

	// 计算spot的原始位置
	rectSpot.NormalizeRect();
	point = rectSpot.CenterPoint();
	if(!rectClient.PtInRect(point))		// spot不在“视野”范围内
		return FALSE;

	// 转换spot在camera中的位置
	nWidth = rectClient.Width();
	if(nWidth <= 0)
		return FALSE;
	fScale = 1.0f * rectTarget.Width() / nWidth;	// 非zero
	ptCameraSpot.x = (long)(fScale * point.x);
	ptCameraSpot.y = (long)(fScale * point.y);
	return TRUE;
}

BOOL CVideoDirectShowWnd::SetObjRectToCamera(CDrawRect* pObj, BOOL bAdd)
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);

	if(NULL == pObj)
		return FALSE;

	CRect rectCamera, rectTarget;
	int nIndex = pObj->m_nIndex;

	CDrawRect::Shape eShape = pObj->GetShapType();
	if(eShape != CDrawRect::RECT_SHAPE_RECTANGLE)
		return FALSE;

	// 判断nIndex的有效性
	if(nIndex < 0)
		return FALSE;

	// 坐标转换
	if(!ConvertObjRectToCameraCoordinate(pObj, rectCamera))
		return FALSE;
	if(rectCamera.IsRectEmpty())
		return FALSE;

	// 设置摄像机
	return m_pOwner->SetObjRectToCamera(nIndex, rectCamera);
}

// 将绘制的rect对象的坐标转换成camera坐标，以便提取温度
// Image控件的宽高比应与camera相等，但图幅可能不相等
BOOL CVideoDirectShowWnd::ConvertObjRectToCameraCoordinate(CDrawObj* pObj, CRect& rectCamera)
{
	rectCamera.SetRectEmpty();

	CDrawRect* pRect = (CDrawRect*)pObj;

	if(pRect==NULL || pRect->GetShapType()!=CDrawRect::RECT_SHAPE_RECTANGLE)
		return FALSE;

	CRect rectTarget;
	rectTarget.SetRectEmpty();
	rectTarget.right = 320;			// 根据camera替换常量
	rectTarget.bottom = 240;

	int nWidth, nHeight;
	float fScale, fScale2;
	CRect rectObj, rectClient, rectInter;

	GetClientRect(rectClient);
	rectClient.NormalizeRect();

	// 检查宽高比一致
	fScale = 100.0f * rectTarget.Width()/rectTarget.Height();
	fScale2 = 100.0f * rectClient.Width()/rectClient.Height();
	ASSERT((int)fScale == (int)fScale2);

	rectObj = pRect->m_rectPosition;
	rectObj.NormalizeRect();

	rectInter.IntersectRect(rectClient, rectObj);	// rectObj在rectClient中那部分
	if(rectInter.IsRectEmpty())						// rectObj不在“视野”范围内
		return FALSE;

	// 转换rect在camera中的位置
	fScale = 1.0f * rectTarget.Width() / rectClient.Width();// 非zero
	nWidth = rectInter.Width();
	nHeight = rectInter.Height();
	rectCamera.left = (long)(fScale * rectInter.left);
	rectCamera.top = (long)(fScale * rectInter.top);
	rectCamera.right = rectCamera.left + (long)(fScale * nWidth);
	rectCamera.bottom = rectCamera.top + (long)(fScale * nHeight);

	return TRUE;
}

BOOL CVideoDirectShowWnd::SetObjLineToCamera(CDrawRect* pObj, BOOL bAdd)
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);

	if(NULL == pObj)
		return FALSE;

	CPoint ptFrom, ptTo;
	int nIndex = pObj->m_nIndex;

	CDrawRect::Shape eShape = pObj->GetShapType();
	if(eShape != CDrawRect::RECT_SHAPE_LINE)
		return FALSE;

	// 判断nIndex的有效性
	if(nIndex < 0)
		return FALSE;

	// 坐标转换
	if(!ConvertObjLineToCameraCoordinate(pObj, ptFrom, ptTo))
		return FALSE;

	// 设置摄像机
	CPoint ptMedian;
	ptMedian.x = (ptFrom.x + ptTo.x)/2;
	ptMedian.y = (ptFrom.y + ptTo.y)/2;
	return m_pOwner->SetObjLineToCamera(nIndex, ptFrom, ptTo, ptMedian);
}

// 将绘制的line对象的坐标转换成camera坐标，以便提取温度
// Image控件的宽高比应与camera相等，但图幅可能不相等
BOOL CVideoDirectShowWnd::ConvertObjLineToCameraCoordinate(CDrawObj* pObj, CPoint& ptFrom, CPoint ptTo)
{
	ptFrom = ptTo = CPoint(0, 0);

	CDrawRect* pLine = (CDrawRect*)pObj;

	if(pLine==NULL || pLine->GetShapType()!=CDrawRect::RECT_SHAPE_LINE)
		return FALSE;
/*
	int nWidth, nHeight;
	float fScale;
	CPoint ptObjFrom, ptObjTo;
//	CRect rectObj = pLine->m_rectObjPosition;
	CRect rectObj = pLine->GetScaledObjPosition();
	ptObjFrom = rectObj.TopLeft();
	ptObjTo = rectObj.BottomRight();
	
	CRect rectClient;
	GetClientRect(rectClient);
	rectClient.NormalizeRect();

	rectObj.NormalizeRect();
	rectObj.IntersectRect(rectClient, rectObj);	// rectObj在rectClient中那部分
	if(!rectObj.IsRectEmpty())			// rectObj不在“视野”范围内
		return FALSE;

	// 转换rect在camera中的位置
	nWidth = rectClient.Width();
	if(nWidth <= 0)
		return FALSE;
	fScale = 320.0f / nWidth;			// 应根据摄像机类型确定常量
	nWidth = rectObj.Width();
	nHeight = rectObj.Height();
	rectCamera.left = (long)(fScale * rectCamera.left);
	rectCamera.top = (long)(fScale * rectCamera.top);
	rectCamera.right = rectCamera.left + (long)(fScale * nWidth);
	rectCamera.bottom = rectCamera.top + (long)(fScale * nHeight);
*/
	return TRUE;
}

#endif
//#############################################################################

void CVideoDirectShowWnd::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	if(GetSafeHwnd() == NULL)
		return;

	if(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR)
	{
		if(!m_rectDefaultArea.IsRectEmpty())
			ResetAllObjsPosition();
	}

	GetClientRect(m_rectDefaultArea);
}

// client区域大小改变后，重定位objs
void CVideoDirectShowWnd::ResetAllObjsPosition()
{
	ASSERT(!m_bVideoUseAsNavigation && m_bCameraTypeIsIR);

	CDrawObj* pObj;
	CRect rectClient, rectTarge;
	CPoint ptLeftTop, ptRightBottom;
	float fScaleWidth, fScaleHight;

	GetClientRect(rectClient);
	rectClient.NormalizeRect();
	m_rectDefaultArea.NormalizeRect();

	if(rectClient.IsRectEmpty() || m_rectDefaultArea.IsRectEmpty())
		return;

	fScaleWidth = 1.0f * rectClient.Width() / m_rectDefaultArea.Width();
	fScaleHight = 1.0f * rectClient.Height() / m_rectDefaultArea.Height();
//	ASSERT((int)(fScaleWidth*1000) == (int)(fScaleHight*1000));

	POSITION pos = m_objListObjects.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = m_objListObjects.GetNext(pos);

		ptLeftTop = pObj->m_rectPosition.TopLeft();
		ptLeftTop.x = (long)(fScaleWidth * ptLeftTop.x);
		ptLeftTop.y = (long)(fScaleHight * ptLeftTop.y);

		ptRightBottom = pObj->m_rectPosition.BottomRight();
		ptRightBottom.x = (long)(fScaleWidth * ptRightBottom.x);
		ptRightBottom.y = (long)(fScaleHight * ptRightBottom.y);

		rectTarge.SetRect(ptLeftTop, ptRightBottom);
		pObj->m_rectPosition = rectTarge;
		pObj->UpdateTipPosition(this);
	}
}

// 只清除数据，不删除表头
void CVideoDirectShowWnd::RemoveAllTempData()
{
	SPOT_TEMP_ARRAY* pSpotArray;
	BOX_TEMP_ARRAY* pBoxArray;

	POSITION pos = m_lstSpotTemp.GetHeadPosition();
	while (pos != NULL)
	{
		pSpotArray = m_lstSpotTemp.GetNext(pos);
		pSpotArray->RemoveAll();
	}

	pos = m_lstBoxTemp.GetHeadPosition();
	while (pos != NULL)
	{
		pBoxArray = m_lstBoxTemp.GetNext(pos);
		pBoxArray->RemoveAll();
	}
}
