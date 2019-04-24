// ExplorerView.cpp : implementation file
//

#include "stdafx.h"
#include "UltraTools.h"
#include "UltraToolsDoc.h"
#include "ExplorerView.h"
#include "MainFrm.h"
#include "ThermalImage.h"
#include "ThermalImageJPG.h"
#include "Utility/MemDC.h"

#include "GlobalDefsForSys.h"

#include "MsWord/WordDocReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//---------------------------------------------------------------------------------------
// ���º�����ڱ��ඨ�����
#define SHELL_TREE_MIN_WIDTH			200			// shellTree��ʼ��/��С���
#define SHELL_TREE_MAX_WIDTH			600
#define BORDER_WIDTH					6
#define IMAGE_PREVIEW_WND_MIN_HEIGHT	100			// ͼ��Ԥ��������С�߶�

#define WARNING_CLR_YELLOW				RGB(255,128,0)
#define WARNING_CLR_RED					RGB(255,0,0)
#define WARNING_CLR_GREEN				RGB(0,255,0)

#define CANVAS_BASE_ID					70

unsigned long _stdcall MakeReportThread(LPVOID lpParam);
unsigned long _stdcall MergeReportThread(LPVOID lpParam);

bool g_bSortListByName = true;			// false: by Datetime

// ����ص�����
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CCheckListCtrl* pListCtrl = (CCheckListCtrl*)lParamSort;

	int nItem1, nItem2, nRet;
	CString sPath, sPath2;
	CString sTs, sTs2, sEquipmentUnit, sEquipmentUnit2, sEquipment, sEquipment2, sPhase, sPhase2;
	CString sDate, sDate2, sTime, sTime2;

	//------------------------------------------------------------------
	// ͨ��ItemData��ȷ������
	LVFINDINFO FindInfo;
	FindInfo.flags = LVFI_PARAM;					// ָ�����ҷ�ʽ
	FindInfo.lParam = lParam1;
	nItem1 = pListCtrl->FindItem(&FindInfo, -1);	// �õ���ӦItem����
	FindInfo.lParam = lParam2;
	nItem2 = pListCtrl->FindItem(&FindInfo, -1);

	if ((nItem1 == -1) || (nItem2 == -1))
	{
		TRACE0("���������޷��ҵ�������Item!\n");
		return 0;
	}

	sTs = pListCtrl->GetItemText(nItem1, 1);
	sEquipmentUnit = pListCtrl->GetItemText(nItem1, 2);
	sEquipment = pListCtrl->GetItemText(nItem1, 3);
	sPhase = pListCtrl->GetItemText(nItem1, 4);
	sDate = pListCtrl->GetItemText(nItem1, 5);
	sTime = pListCtrl->GetItemText(nItem1, 6);
	sPath = pListCtrl->GetItemText(nItem1, 9);

	sTs2 = pListCtrl->GetItemText(nItem2, 1);
	sEquipmentUnit2 = pListCtrl->GetItemText(nItem2, 2);
	sEquipment2 = pListCtrl->GetItemText(nItem2, 3);
	sPhase2 = pListCtrl->GetItemText(nItem2, 4);
	sDate2 = pListCtrl->GetItemText(nItem2, 5);
	sTime2 = pListCtrl->GetItemText(nItem2, 6);
	sPath2 = pListCtrl->GetItemText(nItem2, 9);
	//------------------------------------------------------------------

	nRet = sPath.CompareNoCase(sPath2);
	if (nRet == 0)
	{
		if (g_bSortListByName)		// sort by names
		{
			nRet = sTs.CompareNoCase(sTs2);
			if (nRet == 0)
			{
				nRet = sEquipmentUnit.CompareNoCase(sEquipmentUnit2);
				if (nRet == 0)
				{
					nRet = sEquipment.CompareNoCase(sEquipment2);
					if (nRet == 0)
					{
						nRet = sPhase.CompareNoCase(sPhase2);
						if (nRet == 0)
						{
							nRet = sDate.CompareNoCase(sDate2);
							if (nRet == 0)
								nRet = sTime.CompareNoCase(sTime2);
						}
					}
				}
			}
		}
		else    // sort by datetime
		{
			nRet = sDate.CompareNoCase(sDate2);
			if (nRet == 0)
			{
				nRet = sTime.CompareNoCase(sTime2);
				if (nRet == 0)
				{
					nRet = sTs.CompareNoCase(sTs2);
					if (nRet == 0)
					{
						nRet = sEquipmentUnit.CompareNoCase(sEquipmentUnit2);
						if (nRet == 0)
						{
							nRet = sEquipment.CompareNoCase(sEquipment2);
							if (nRet == 0)
								nRet = sPhase.CompareNoCase(sPhase2);
						}
					}
				}
			}
		}
	}

	return nRet;
}


/////////////////////////////////////////////////////////////////////////////
// CExplorerView

IMPLEMENT_DYNCREATE(CExplorerView, CBCGPFormView)

CExplorerView::CExplorerView()
	: CBCGPFormView(CExplorerView::IDD)
{
	EnableVisualManagerStyle ();

	m_bEndInit = FALSE;
	m_bShowExplorerTree = TRUE;
	m_bShowImagePreviewWnds = FALSE;
	m_bIsMakingReport = FALSE;
	m_bIsMergingReport = FALSE;
	m_hMakeReport = NULL;

	m_sCurTsName = _T("");

	for (int i = 0; i < 3; i++)
		m_pSelectedImages[i] = NULL;
}

CExplorerView::~CExplorerView()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_pSelectedImages[i] != NULL)
		{
			::delete m_pSelectedImages[i];
			m_pSelectedImages[i] = NULL;
		}
	}
}

void CExplorerView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExplorerView)
	DDX_Control(pDX, IDC_TREE, m_wndShellTree);
	DDX_Control(pDX, IDC_LIST_SOURCE, m_wndSourceList);
	DDX_Control(pDX, IDC_STATIC_IMAGE_PREVIEW_LOCATION, m_ctrlPreviewWndLocation);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExplorerView, CBCGPFormView)
	//{{AFX_MSG_MAP(CExplorerView)
	ON_WM_CONTEXTMENU()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_MESSAGE(WM_SPLITTER_MOVED, OnSplitterMovedMessage)
	ON_COMMAND(IDM_EXPLORER_VIEW_TREE, OnViewExplorerTree)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_VIEW_TREE, OnUpdateViewExplorerTree)
	ON_COMMAND(IDM_EXPLORER_VIEW_IMAGE_PREVIEW, OnViewImagePreview)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_VIEW_IMAGE_PREVIEW, OnUpdateViewImagePreview)
	ON_COMMAND(IDM_EXPLORER_LIST_SORT_BY_NAME, OnListSortByName)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_LIST_SORT_BY_NAME, OnUpdateListSortByName)
	ON_COMMAND(IDM_EXPLORER_LIST_SORT_BY_DATETIME, OnListSortByDatetime)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_LIST_SORT_BY_DATETIME, OnUpdateListSortByDatetime)
	ON_COMMAND(IDM_EXPLORER_REPORT_PREVIEW, OnReportPreview)
	ON_COMMAND(IDM_EXPLORER_REPORT_GENERATE, OnReportGeneration)
	ON_COMMAND(IDM_EXPLORER_REPORT_GENERATE_REVERSELY, OnReportGenerationReversely)
	ON_COMMAND(IDM_EXPLORER_REPORT_MERGE, OnReportMerge)
	ON_COMMAND(IDM_EXPLORER_COPY_TO, OnCopyTo)
	ON_COMMAND(IDM_EXPLORER_MOVE_TO, OnMoveTo)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_COPY_TO, OnUpdateCopyOrMoveFile)
	ON_UPDATE_COMMAND_UI(IDM_EXPLORER_MOVE_TO, OnUpdateCopyOrMoveFile)
	//}}AFX_MSG_MAP

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CExplorerView::OnTvnSelchangedTree)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SOURCE, &CExplorerView::OnListItemSelchanged)

	ON_MESSAGE(WM_BCGPM_DBLCLK_ITEM, &CExplorerView::OnDblclkShellList)
	ON_COMMAND(IDM_VIEW_ANALYSIS_FORMVIEW, OnViewAnalysisFormview)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_ANALYSIS_FORMVIEW, OnUpdateViewAnalysisFormview)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerView diagnostics

#ifdef _DEBUG
void CExplorerView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

void CExplorerView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}

CUltraToolsDoc* CExplorerView::GetDocument(BOOL bGlobalDoc) // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUltraToolsDoc)));
	if(bGlobalDoc)
		return &theApp.m_docIrImage;
	return (CUltraToolsDoc*)m_pDocument;
}
#endif //_DEBUG


void CExplorerView::InitSourceFileList()
{
	int nCnt, i;
	CString sTem;

	//����ListCtrlѡ���У�������
	m_wndSourceList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_CHECKBOXES);
	LVCOLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolumn.fmt = LVCFMT_LEFT;
	nCnt = 10;		// ����10��
	for (i = 0; i<nCnt; i++)
	{
		lvcolumn.iSubItem = i;

		switch (i)
		{
		case 0: lvcolumn.pszText = _T("���");			lvcolumn.cx = 60;	break;
		case 1: lvcolumn.pszText = _T("���վ");		lvcolumn.cx = 120;	break;
		case 2: lvcolumn.pszText = _T("�����Ԫ");		lvcolumn.cx = 120;	break;
		case 3: lvcolumn.pszText = _T("�豸��ż�����");lvcolumn.cx = 180;	break;
		case 4: lvcolumn.pszText = _T("���");			lvcolumn.cx = 50;	break;	// A/B/C/ABC/��
		case 5: lvcolumn.pszText = _T("��������");		lvcolumn.cx = 80;	break;
		case 6: lvcolumn.pszText = _T("����ʱ��");		lvcolumn.cx = 80;	break;
		case 7: lvcolumn.pszText = _T("��ͼ����");		lvcolumn.cx = 80;	break;
		case 8: lvcolumn.pszText = _T("��ͼ��");		lvcolumn.cx = 50;	break;	// Y=�����ͼ��N=������ͼ
		case 9: lvcolumn.pszText = _T("���վ�ļ���·��");	lvcolumn.cx = 400;	break;
		}
		m_wndSourceList.InsertColumn(i, &lvcolumn);
	}
	m_wndSourceList.SetFont(&globalData.fontRegular);
	//m_wndSourceList.EnableToolTips();

	m_wndSourceList.Init();
}

void CExplorerView::UpdateSourceFileList()
{
	m_wndSourceList.DeleteAllItems();
	m_nCurPreviewWndCnt = 0;

	// ����Ŀǰ֧�ֵĸ����ļ�
	FindSourceFile(m_sRootDirectory);

	if (m_wndSourceList.GetItemCount() <= 0)
		return;

	// ����
	g_bSortListByName = true;
	m_wndSourceList.SortItems(ListCompare, (LPARAM)&m_wndSourceList);
	// ���
	MakeFilesGroup();
	// �������
	UpdateListIndexCol(m_wndSourceList, 0);
}

void CExplorerView::FindSourceFile(CString sPath)
{
	CFileFind fileFind;
	CString sFileFullName, sFileName, sSubPath, sExt, sTsName, sTem;
	BOOL bOK = FALSE, bChangeFileName = FALSE;
	int nCnt=0, nItem, nFileFiltersCnt, i, nPos, nPos2;
	
	nFileFiltersCnt = (int)m_saFileFilters.GetCount();

	if (sPath.Right(1) != _T("\\"))
		sPath += _T("\\");
	sTem = sPath + _T("*.*");

	bOK = fileFind.FindFile(sTem);
	while (bOK)
	{
		bOK = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots())		// ����Ŀ¼
		{
			sSubPath = fileFind.GetFilePath();
			if (PathFileExists(sSubPath) && sSubPath.Find(_T("�Ѵ���")) <0)
				FindSourceFile(sSubPath);
		}
		else if (!fileFind.IsDirectory() && !fileFind.IsDots())	// ���ļ�
		{
			sFileFullName = fileFind.GetFilePath();				// �ļ�ȫ·�������ļ�����
			sFileName = fileFind.GetFileName();					// �ļ���������·��

			//-----------------------------------------------------------
			// ���ָ���(Ӣ�Ķ���)��ͳһ�滻�����Ķ��ţ���֧��Ӣ�Ķ��ŷָ�
			bChangeFileName = FALSE;
			sTem = sFileFullName;
			if (sFileFullName.Find(',') > 0)
			{
				bChangeFileName = TRUE;
				sFileFullName.Replace(_T(","), _T("��"));
				sFileName.Replace(_T(","), _T("��"));
			}
			// ��ȫ�������ĸ��ͳһ���ɰ�������ĸ
			if (sFileFullName.Find(_T("��")) > 0)
			{
				bChangeFileName = TRUE;
				sFileFullName.Replace(_T("��"), _T("A"));
				sFileName.Replace(_T("��"), _T("A"));
			}
			if (sFileFullName.Find(_T("��")) > 0)
			{
				bChangeFileName = TRUE;
				sFileFullName.Replace(_T("��"), _T("B"));
				sFileName.Replace(_T("��"), _T("B"));
			}
			if (sFileFullName.Find(_T("��")) > 0)
			{
				bChangeFileName = TRUE;
				sFileFullName.Replace(_T("��"), _T("C"));
				sFileName.Replace(_T("��"), _T("C"));
			}
			if (bChangeFileName)
				CFile::Rename(sTem, sFileFullName);
			//-----------------------------------------------------------

			sTsName = _T("");

			sExt = sFileName.Mid(sFileName.ReverseFind('.') + 1);
			for (i = 0; i<nFileFiltersCnt; i++)
			{
				sTem = m_saFileFilters.GetAt(i);
				if (!sExt.CompareNoCase(sTem))
					break;
			}
			if (i >= nFileFiltersCnt)							// ����Ŀ���ļ�
				continue;

			nCnt = m_wndSourceList.GetItemCount();

			sTem.Format(_T("%d"), nCnt + 1);
			nItem = m_wndSourceList.InsertItem(nCnt, sTem);
			VERIFY(-1 != nItem);

			// ��ȡ���վ���ʹ洢·��
			//sTem = fileFind.GetFilePath();					// �ļ�ȫ·�������ļ�����
			sTem = sFileFullName;								// ���������Ӣ�Ķ�����������
			sTem = sTem.Left(sTem.ReverseFind('\\'));
			nPos = sTem.ReverseFind('\\');
			sTsName = sTem.Mid(nPos +1);
			m_wndSourceList.SetItemText(nItem, 1, sTsName);
			sTem = sTem.Left(nPos+1);							// ʹ·������"\\"��β
			m_wndSourceList.SetItemText(nItem, 9, sTem);

			// ��ȡ�����Ԫ��
			nPos = sFileName.Find(_T("��"));					// �������Ķ��ţ��Ǹ��ַ���
			if (nPos > 1)
			{
				sTem = sFileName.Mid(0, nPos);
				m_wndSourceList.SetItemText(nItem, 2, sTem);
			}
			else
			{
				//m_wndSourceList.SetItemText(nItem, 2, sFileName);
				continue;
			}

			// ��ȡ�豸��ż�����
			nPos = nPos + 1;
			nPos2 = sFileName.Find(_T("��"), nPos);
			if (nPos2 < 0)										// û������豸��ż����������һ��
			{
				nPos2 = sFileName.Find('.', nPos);				// �Ҿ��
				if (nPos2 > nPos)
				{
					sTem = sFileName.Mid(nPos, nPos2 - nPos);
					m_wndSourceList.SetItemText(nItem, 3, sTem);
				}
			}
			else if (nPos2 > nPos)
			{
				sTem = sFileName.Mid(nPos, nPos2 - nPos);
				m_wndSourceList.SetItemText(nItem, 3, sTem);
			}
			else
				continue;

			// ��ȡ���
			nPos = nPos2 + 1;
			nPos2 = sFileName.Find('.', nPos);					// ��������һ�����û�ж��ţ����Ҿ��
			if (nPos2 > nPos)
			{
				sTem = sFileName.Mid(nPos, nPos2 - nPos -1);	// ��2��ȥ�����ࡱ�֣�unicode��һ��������һ���ַ�
				m_wndSourceList.SetItemText(nItem, 4, sTem);
			}
			// ���ܲ�ָ����λ���ʴ˴�����else continue

			// ������ȡ��ͼ���������ڣ���������ͼ�޸�����
			// �����ļ��ġ�����޸����ڡ���Ϊ�ɼ�����(����������GetCreationTime��Ϊ�������ڣ�������)
			if (0 == sExt.CompareNoCase(_T("JPG")))
			{
				FILE* hFile;
				//fopen_s(&hFile, sFileFullName, _T("rb"));
				hFile = _tfopen(sFileFullName, _T("rb"));
				if (hFile)
				{
					CThermalImageJPG::CJpegExifInfo imgExif;
					CxIOFile file(hFile);
					imgExif.DecodeExif(&file, EXIF_READ_BASAL_EXIF);
					fclose(hFile);
					if (NULL != imgExif.m_pExifInfo && imgExif.m_pExifInfo->IsExif)
					{
						sTem = imgExif.m_pExifInfo->DateTimeOriginal;
						nPos = sTem.Find(' ');
						if (nPos > 0)
						{
							m_wndSourceList.SetItemText(nItem, 6, sTem.Mid(nPos + 1));	// time
							sTem = sTem.Mid(0, nPos);
							sTem.Replace(':', '-');
							m_wndSourceList.SetItemText(nItem, 5, sTem);				// date
						}
					}
				}
			}
			else
			{
				CTime dt;
				if (fileFind.GetLastWriteTime(dt))
				{
					m_wndSourceList.SetItemText(nItem, 5, dt.Format(_T("%Y-%m-%d")));
					m_wndSourceList.SetItemText(nItem, 6, dt.Format(_T("%H:%M:%S")));
				}
			}

			m_wndSourceList.SetItemText(nItem, 7, sExt);

			// Ĭ���ǵ��಻����
			m_wndSourceList.SetItemText(nItem, 8, _T("N"));

			// �����������ݡ��кš�
			m_wndSourceList.SetItemData(nItem, nCnt);
		}// ���ļ�
	}// while (bOK)
	fileFind.Close();
}

// ����ͼ�����飺
// �ֿ��������ֿ��Ķ���Ϊһ�飬�������������ĵ��࣬����������飬�Ա���ò�ͬ�ı���ģ��
// ����������ͼΪһ���������ͼΪһ��
// �������ݣ����ڵ�������ͼ�������豸��ż����ơ���ͬ��������ͬһ���վ��ͬһ�����Ԫ�����Ϊͬ��
// ����ǰ����������
void CExplorerView::MakeFilesGroup()
{
	CString sTs, sTs2, sTs3, sEquipmentUnit, sEquipmentUnit2, sEquipmentUnit3;
	CString sEquipment, sEquipment2, sEquipment3, sPhase, sPhase2, sPhase3;
	CString sPhaseGroup;

	for (int i = 0; i<m_wndSourceList.GetItemCount()-2;)
	{
		sPhaseGroup = _T("");

		sTs = m_wndSourceList.GetItemText(i, 1);
		sTs2 = m_wndSourceList.GetItemText(i + 1, 1);
		sEquipmentUnit = m_wndSourceList.GetItemText(i, 2);
		sEquipmentUnit2 = m_wndSourceList.GetItemText(i + 1, 2);
		sEquipment = m_wndSourceList.GetItemText(i, 3);
		sEquipment2 = m_wndSourceList.GetItemText(i + 1, 3);
		if (sTs != sTs2 || sEquipmentUnit != sEquipmentUnit2 || sEquipment != sEquipment2)
		{
			i++;
			continue;
		}

		sPhase = m_wndSourceList.GetItemText(i, 4);
		sPhase2 = m_wndSourceList.GetItemText(i + 1, 4);
		sPhaseGroup = sPhase + sPhase2;

		sTs3 = m_wndSourceList.GetItemText(i + 2, 1);
		sEquipmentUnit3 = m_wndSourceList.GetItemText(i + 2, 2);
		sEquipment3 = m_wndSourceList.GetItemText(i + 2, 3);
		if (sTs != sTs3 || sEquipmentUnit != sEquipmentUnit3 || sEquipment != sEquipment3)
		{
			// ������������ͼ������������
			m_wndSourceList.SetItemText(i, 4, sPhaseGroup);
			m_wndSourceList.SetItemText(i, 8, _T("Y"));
			m_wndSourceList.DeleteItem(i + 1);

			i++;
			continue;
		}

		sPhase3 = m_wndSourceList.GetItemText(i + 2, 4);
		sPhaseGroup = sPhaseGroup + sPhase3;		// ������ǰ�����򣬴�ʱsPhaseGroup����ӦΪ��ABC��

		// ����������ͼ������������
		m_wndSourceList.SetItemText(i, 4, sPhaseGroup);
		m_wndSourceList.SetItemText(i, 8, _T("Y"));
		m_wndSourceList.DeleteItem(i + 2);	// ��ɾ
		m_wndSourceList.DeleteItem(i + 1);	// ��ɾ

		i++;
	}
}

// ����������е����
void CExplorerView::UpdateListIndexCol(CCheckListCtrl &wndList, int nIndexColNum)
{
	CString sTem;
	int nCnt = wndList.GetItemCount();
	if (nIndexColNum<0 || nIndexColNum >= wndList.GetHeaderCtrl().GetItemCount())
		return;

	for (int i = 0; i<nCnt; i++)
	{
		sTem.Format(_T("%d"), i + 1);
		wndList.SetItemText(i, nIndexColNum, sTem);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CExplorerView message handlers

void CExplorerView::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
//	theApp.ShowPopupMenu (IDR_CONTEXT_MENU, point, this);
}

void CExplorerView::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CBCGPFormView::OnWindowPosChanging(lpwndpos);
	
	// Hide horizontal scrollbar:
	ShowScrollBar (SB_HORZ, FALSE);
	ModifyStyle (WS_HSCROLL, 0, SWP_DRAWFRAME);
}

void CExplorerView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();

	// Insert a splitter bars
	CRect rect(0,0,0,0);
	m_wndVSplitterBar.Create(WS_CHILD|WS_VISIBLE, rect, this, 997, FALSE);	// ��ֱ���õ�splitter
	m_wndVSplitterBar.SetPanes(&m_wndShellTree, &m_wndSourceList);
	m_wndHSplitterBar.Create(WS_CHILD | WS_VISIBLE, rect, this, 998, TRUE);	// ˮƽ���õ�splitter
	m_wndHSplitterBar.SetPanes(&m_wndSourceList, &m_ctrlPreviewWndLocation);

	// ���������ļ�����
	m_saFileFilters.Add(_T("JPG"));
	m_saFileFilters.Add(_T("JPEG"));
	m_saFileFilters.Add(_T("JPE"));
	m_saFileFilters.Add(_T("FPF"));		// FLIRר��:FPF/IMG/SEQ
	m_saFileFilters.Add(_T("IMG"));
	m_saFileFilters.Add(_T("SEQ"));
	m_saFileFilters.Add(_T("DLV"));		// ���ݴ���ר��:DLV/DLI
	m_saFileFilters.Add(_T("DLI"));

	InitSourceFileList();

	for (int i = 0; i < 3; i++)
	{
		m_wndCanvas[i].Create(_T("Image Preview Canvas"),
				WS_VISIBLE | WS_CHILD | SS_OWNERDRAW,
				rect, this, CANVAS_BASE_ID + i);
	}

	SplitFormView();

	CUltraToolsDoc* pDoc = GetDocument(FALSE);
	ASSERT_VALID (pDoc);
	pDoc->SetTitle (_T("ͼ����������  "));

	m_bEndInit = TRUE;
	
	// ��׼�����List��չ��ͼ����������ʼ��List
	//CString sPath = _T("D:\\Projects\\�Ⱥ���ϵ�в�Ʒ\\UltraTools����ͨ\\UltraTools����ͼ��");
	TCHAR szTem[256]; 
	CString sSysIniFile = theApp.m_sAppPath + _T("Data\\Sys.ini");
	m_bSavePreviewFileToSysTempPath = 0 == GetPrivateProfileInt(_T("Report"), _T("Ԥ��������ʱ�ĵ��洢����ͼ�����ļ���"), 0, sSysIniFile);
	m_bPrintReportWhileGenerated = 1 == GetPrivateProfileInt(_T("Report"), _T("���ɱ����ͬʱ��ӡ����"), 0, sSysIniFile);
	m_bMoveImageFileWhileReportSaved = 1 == GetPrivateProfileInt(_T("Report"), _T("���������ù�����ͼ���롾�Ѵ����ļ���"), 1, sSysIniFile);
	m_bMoveReportFileWhileReportMerged = 1 == GetPrivateProfileInt(_T("Report"), _T("���Ѻϲ��ı������롾�Ѵ����ļ���"), 1, sSysIniFile);
	GetPrivateProfileString(_T("Report"), _T("�������ǰ׺"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	m_sReportTitlePrefix = CString(szTem);

	GetPrivateProfileString(_T("Report"), _T("��ʼ��ͼ�ļ���"), _T(""), szTem, sizeof(szTem), sSysIniFile);
	CString sPath(szTem);
	//m_wndShellTree.Expand(m_wndShellTree.GetRootItem(), TVE_EXPAND);
	m_wndShellTree.SelectPath(sPath);

	OnViewImagePreview();
}

void CExplorerView::OnSize(UINT nType, int cx, int cy)
{
	CBCGPFormView::OnSize(nType, cx, cy);

	if (GetSafeHwnd()==NULL || !m_bEndInit)
		return;

	SplitFormView();
	ArrangePreviewWnds();
}

LRESULT CExplorerView::OnSplitterMovedMessage(WPARAM wParam, LPARAM lParam)
{
	SplitFormView();
	ArrangePreviewWnds();
	return 0;
}

void CExplorerView::SplitFormView()
{
	CRect rectClient, rectToolBar, rectSpliterV, rectSpliterH;
	CRect rectTree, rectList, rectPreview;
	int nWidth, nHeight;

	GetClientRect(&rectClient);

	//int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	if (m_bShowExplorerTree)
	{
		m_wndShellTree.GetClientRect(rectTree);
		nWidth = rectTree.Width();
		rectTree = rectClient;
		nWidth = max(SHELL_TREE_MIN_WIDTH, nWidth);		// С��ȡ��
		nWidth = min(SHELL_TREE_MAX_WIDTH, nWidth);		// ����ȡС
		rectTree.right = rectTree.left + nWidth;
		m_wndShellTree.MoveWindow(&rectTree, TRUE);
		m_wndShellTree.ShowWindow(SW_SHOW);

		// ��ֱ���õ�spliter
		rectSpliterV.top = rectTree.top;
		rectSpliterV.left = rectTree.right;
		rectSpliterV.right = rectSpliterV.left + GetSystemMetrics(SM_CXFRAME);
		rectSpliterV.bottom = rectClient.bottom - BORDER_WIDTH;
		m_wndVSplitterBar.MoveWindow(rectSpliterV, TRUE);
		m_wndVSplitterBar.ShowWindow(SW_SHOW);

		if (m_bShowImagePreviewWnds)
		{
			m_ctrlPreviewWndLocation.GetClientRect(rectPreview);
			rectPreview.top = rectClient.bottom - max(IMAGE_PREVIEW_WND_MIN_HEIGHT, rectPreview.Height());
			rectPreview.left = rectSpliterV.right;
			rectPreview.right = rectClient.right;
			rectPreview.bottom = rectClient.bottom;
			m_ctrlPreviewWndLocation.MoveWindow(&rectPreview, TRUE);
			m_ctrlPreviewWndLocation.ShowWindow(SW_SHOW);

			// ˮƽ���õ�spliter
			rectSpliterH.bottom = rectPreview.top;
			rectSpliterH.top = rectSpliterH.bottom - GetSystemMetrics(SM_CYFRAME);
			rectSpliterH.left = rectSpliterV.right;
			rectSpliterH.right = rectClient.right;
			m_wndHSplitterBar.MoveWindow(rectSpliterH, TRUE);
			m_wndHSplitterBar.ShowWindow(SW_SHOW);
		}
		else
		{
			m_ctrlPreviewWndLocation.ShowWindow(SW_HIDE);
			m_wndHSplitterBar.ShowWindow(SW_HIDE);
		}

		rectList.top = 0;
		rectList.left = rectSpliterV.right;
		rectList.right = rectClient.right;
		if (m_bShowImagePreviewWnds)
			rectList.bottom = rectSpliterH.top;
		else
			rectList.bottom = rectClient.bottom;
		m_wndSourceList.MoveWindow(&rectList, TRUE);
	}
	else
	{
		m_wndShellTree.ShowWindow(SW_HIDE);
		m_wndVSplitterBar.ShowWindow(SW_HIDE);

		if (m_bShowImagePreviewWnds)
		{
			m_ctrlPreviewWndLocation.GetClientRect(rectPreview);
			nHeight = rectPreview.Height();
			nHeight = rectClient.bottom - max(IMAGE_PREVIEW_WND_MIN_HEIGHT, nHeight);
			rectPreview = rectClient;
			rectPreview.top = nHeight;
			m_ctrlPreviewWndLocation.MoveWindow(&rectPreview, TRUE);
			m_ctrlPreviewWndLocation.ShowWindow(SW_SHOW);

			// ˮƽ���õ�spliter
			rectSpliterH = rectClient;
			rectSpliterH.bottom = rectPreview.top;
			rectSpliterH.top = rectSpliterH.bottom - GetSystemMetrics(SM_CYFRAME);
			m_wndHSplitterBar.MoveWindow(rectSpliterH, TRUE);
			m_wndHSplitterBar.ShowWindow(SW_SHOW);
		}
		else
		{
			m_ctrlPreviewWndLocation.ShowWindow(SW_HIDE);
			m_wndHSplitterBar.ShowWindow(SW_HIDE);
		}

		rectList = rectClient;
		if (m_bShowImagePreviewWnds)
			rectList.bottom = rectSpliterH.top;
		else
			rectList.bottom = rectClient.bottom;
		m_wndSourceList.MoveWindow(&rectList, TRUE);
	}
}

void CExplorerView::ArrangePreviewWnds()
{
	// ȫ����
	for (int i = 0; i < 3; i++)
		m_wndCanvas[i].ShowWindow(SW_HIDE);
	m_ctrlPreviewWndLocation.Invalidate(FALSE);

	if (m_nCurPreviewWndCnt <= 0)
		return;

	CRect rcClient, rect;
	int nWidth;
	m_ctrlPreviewWndLocation.GetWindowRect(rcClient);
	ScreenToClient(rcClient);

	if (m_nCurPreviewWndCnt == 1)
	{
		//m_wndCanvas[0].MoveWindow(rcClient);
		SetCanvasPosition(rcClient, 0);
		m_wndCanvas[0].ShowWindow(SW_SHOW);
		return;
	}

	nWidth = rcClient.Width() / m_nCurPreviewWndCnt;
	rect = rcClient;

	rect.right = rect.left + nWidth;
	SetCanvasPosition(rect, 0);

	rect.left = rect.right;
	rect.right = rect.left + nWidth;
	SetCanvasPosition(rect, 1);

	if (3 == m_nCurPreviewWndCnt)
	{
		rect.left = rect.right;
		rect.right = rect.left + nWidth;
		SetCanvasPosition(rect, 2);
	}

	for (int i = 0; i < m_nCurPreviewWndCnt; i++)
		m_wndCanvas[i].ShowWindow(SW_SHOW);
}

// ֱ����Rect�л�����ͼ����������ͼ��߱Ȼ���
// �����������û���λ�õ�ͬʱ��������ͼ�����������
void CExplorerView::SetCanvasPosition(CRect& rcPosition, int nIndex)
{
	if (nIndex < 0 || nIndex >= 3)
		return;

	CRect rectImage = rcPosition;

	if (m_pSelectedImages[nIndex] == NULL)
	{
		rectImage.SetRectEmpty();
		m_wndCanvas[nIndex].MoveWindow(rectImage);
	}
	else
	{
		const int   nWidth = rectImage.Width();
		const int   nHeight = rectImage.Height();
		const float fRatio = (float)nHeight / nWidth;
		const float fImgRatio = (float)m_pSelectedImages[nIndex]->GetHeight() /
										m_pSelectedImages[nIndex]->GetWidth();

		int XDest, YDest, nDestWidth, nDestHeight;

		if (fImgRatio > fRatio)
		{
			nDestWidth = (int)(nHeight / fImgRatio);
			XDest = rectImage.left + (nWidth - nDestWidth) / 2;
			YDest = rectImage.top;
			nDestHeight = nHeight;
		}
		else
		{
			XDest = rectImage.left;
			nDestWidth = nWidth;
			nDestHeight = (int)(nWidth*fImgRatio);
			YDest = rectImage.top + (nHeight - nDestHeight) / 2;
		}

		CRect rect(XDest, YDest, XDest + nDestWidth, YDest + nDestHeight);
		m_wndCanvas[nIndex].MoveWindow(rect);
		//m_wndCanvas[nIndex].Invalidate();
	}
}

void CExplorerView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (!(lpDrawItemStruct->itemAction & ODA_DRAWENTIRE))
		return;
	if (m_nCurPreviewWndCnt <= 0 || !m_bShowImagePreviewWnds)
		return;

	CRect rect;
	CMyMemDC *pMemDC = NULL;
	pMemDC = new CMyMemDC(lpDrawItemStruct->hDC);

//	GetClientRect(rect);
//	HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 255, 255));
//
//	::FillRect(pMemDC->m_hDC, rect, hBrush);
//	DeleteObject(hBrush);

	for (int i = 0; i < m_nCurPreviewWndCnt; i++)
	{
		if (m_pSelectedImages[i]!=NULL && nIDCtl-CANVAS_BASE_ID==i)
		{
			m_wndCanvas[i].GetClientRect(rect);
			m_pSelectedImages[i]->Draw(pMemDC->m_hDC, rect, NULL, TRUE);
		}
	}

	delete pMemDC;
}

void CExplorerView::OnViewExplorerTree() 
{
	m_bShowExplorerTree = !m_bShowExplorerTree;
	SplitFormView();
	ArrangePreviewWnds();
}

void CExplorerView::OnUpdateViewExplorerTree(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_bShowExplorerTree);
}

void CExplorerView::OnViewImagePreview() 
{
	m_bShowImagePreviewWnds = !m_bShowImagePreviewWnds;
	SplitFormView();
	ArrangePreviewWnds();
}

void CExplorerView::OnUpdateViewImagePreview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_bShowImagePreviewWnds);
}

void CExplorerView::OnListSortByName()
{
	if (m_wndSourceList.GetItemCount() <= 0 || g_bSortListByName)
		return;

	g_bSortListByName = true;
	m_wndSourceList.SortItems(ListCompare, (LPARAM)&m_wndSourceList);
	// �������
	UpdateListIndexCol(m_wndSourceList, 0);
}

void CExplorerView::OnUpdateListSortByName(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_bSortListByName);
}

void CExplorerView::OnListSortByDatetime()
{
	if (m_wndSourceList.GetItemCount() <= 0 || !g_bSortListByName)
		return;

	g_bSortListByName = false;
	m_wndSourceList.SortItems(ListCompare, (LPARAM)&m_wndSourceList);
	// �������
	UpdateListIndexCol(m_wndSourceList, 0);
}

void CExplorerView::OnUpdateListSortByDatetime(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!g_bSortListByName);
}

void CExplorerView::OnReportPreview()
{
	UINT uSelectedCount = m_wndSourceList.GetSelectedCount();
	if (uSelectedCount == 0)
	{
		AfxMessageBox(_T("���ȵ��ѡ��һ����¼!"));
		return;
	}
	int  nItem = m_wndSourceList.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == LB_ERR)
		return;

	m_nPreviewOrPrint = 0;
	m_nCurItem = nItem;
	StartMakeReportThread();
}

// �����̣߳���Թ�ѡ����ͼ����˳�����ɱ���
void CExplorerView::OnReportGeneration()
{
	int uCount = m_wndSourceList.GetCheckedItemCount();
	if (uCount == 0)
	{
		AfxMessageBox(_T("���ȹ�ѡ��¼!"));
		return;
	}

	if (m_bPrintReportWhileGenerated)
		m_nPreviewOrPrint = 1;
	else
		m_nPreviewOrPrint = 2;

	m_bGenerateReportInOrder = TRUE;
	StartMakeReportThread();
}

// �����̣߳���Թ�ѡ����ͼ�����������ɱ���
void CExplorerView::OnReportGenerationReversely()
{
	int uCount = m_wndSourceList.GetCheckedItemCount();
	if (uCount == 0)
	{
		AfxMessageBox(_T("���ȹ�ѡ��¼!"));
		return;
	}

	if (m_bPrintReportWhileGenerated)
		m_nPreviewOrPrint = 1;
	else
		m_nPreviewOrPrint = 2;

	m_bGenerateReportInOrder = FALSE;
	StartMakeReportThread();
}

void CExplorerView::OnReportMerge()
{
	if (m_bPrintReportWhileGenerated)
		m_nPreviewOrPrint = 1;
	else
		m_nPreviewOrPrint = 2;

	StarMergeReportThread();
}

void CExplorerView::OnCopyTo()
{
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CString sPath = pDoc->m_sRootDirectory;
	if (PathFileExists(sPath) &&
		theApp.GetShellManager()->BrowseForFolder (sPath, 
		this, sPath, _T("��ѡ�е��ļ��������ļ���:")))
	{
		CString sSrcFile, sDstFile;
		sSrcFile = pDoc->m_sCurrentFile;
		int nPos = sSrcFile.ReverseFind('\\');
		if(nPos > 0)
			sSrcFile = sSrcFile.Mid(nPos+1);
		sDstFile = sPath + sSrcFile;
		CopyFile(pDoc->m_sCurrentFile, sDstFile, FALSE);	// ����
	}
}

void CExplorerView::OnMoveTo() 
{
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CString sPath = pDoc->m_sRootDirectory;
	if (PathFileExists(sPath) &&
		theApp.GetShellManager()->BrowseForFolder (sPath, 
		this, sPath, _T("��ѡ�е��ļ��ƶ����ļ���:")))
	{
		CString sSrcFile, sDstFile;
		sSrcFile = pDoc->m_sCurrentFile;
		int nPos = sSrcFile.ReverseFind('\\');
		if(nPos > 0)
			sSrcFile = sSrcFile.Mid(nPos+1);
		sDstFile = sPath + sSrcFile;
		MoveFile(pDoc->m_sCurrentFile, sDstFile);

		// ˢ��List��Thumbnail
		// TODO��remove item(s) from lists
		SelectFolder(sPath);
	}
}

void CExplorerView::OnAnalysis()
{
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	if(pFrame != NULL)
//		pFrame->CreateAnalysisView();
}

void CExplorerView::OnUpdateCopyOrMoveFile(CCmdUI* pCmdUI)
{
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_sCurrentFile.GetLength()>0);
}

// ��ͼ������ѡ���˲�ͬ�ļ���
void CExplorerView::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!m_bEndInit)
	{
		*pResult = 0;
		return;
	}

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CString sPath;
	if( m_wndShellTree.GetItemPath(sPath, NULL) )	// NULL=ѡ�е�item
		SelectFolder(sPath);

	*pResult = 0;
}

// ����˵�ǰĿ¼�����»�ȡͼ���嵥
void CExplorerView::SelectFolder(CString sPath)
{
	if(!m_bEndInit)
		return;
	if (!PathFileExists(sPath))
		return;

	m_sRootDirectory = sPath;

	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_vFileNames.clear();
	pDoc->m_sRootDirectory = sPath;
	pDoc->m_sCurrentFile = _T("");
	pDoc->m_nCurItem = -1;
//	pDoc->RemoveAllIrImageRecords();

	UpdateSourceFileList();
}

void CExplorerView::OnListItemSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (-1 != pNMLV->iItem && LVIF_STATE == pNMLV->uChanged && (pNMLV->uNewState & LVIS_SELECTED))
		SelectItem( pNMLV->iItem );
	*pResult = 0;
}

void CExplorerView::SelectItem( int nIndex )
{
	struct tagImageFileBaseInfo baseInfo;

	for (int i = 0; i < 3; i++)
	{
		if (m_pSelectedImages[i] != NULL)
		{
			::delete m_pSelectedImages[i];
			m_pSelectedImages[i] = NULL;
		}
	}

	GetFileBaseInfo(nIndex, baseInfo);

	if (baseInfo.sGroupMark == _T("N"))		// ������ͼ
	{
		m_nCurPreviewWndCnt = 1;
		MakeFileThumbnail(baseInfo.sFileNameA, 0);
		ArrangePreviewWnds();
		return;
	}

	// ������ͼ����������ͼ
	m_nCurPreviewWndCnt = baseInfo.sPhaseGroup.GetLength();
	MakeFileThumbnail(baseInfo.sFileNameA, 0);
	MakeFileThumbnail(baseInfo.sFileNameB, 1);
	if (3 == m_nCurPreviewWndCnt)
		MakeFileThumbnail(baseInfo.sFileNameC, 2);
	ArrangePreviewWnds();
}

BOOL CExplorerView::MakeFileThumbnail(CString sFileName, int nIndex)
{
	int nType;
	CString sExt, sInfo;

	if (!PathFileExists(sFileName) || nIndex < 0 || nIndex >= 3)
		return FALSE;

	nType = GetTypeFromFileName(sFileName, sExt);

	if (THERMAL_IMAGE_FORMAT_UNKNOWN == nType)
	{
		BOOL bRet = FALSE;
		m_pSelectedImages[nIndex] = new CThermalImage();
		if (sExt == _T("fpf"))
			bRet = MakeFpfFileThumbnail(sFileName, m_pSelectedImages[nIndex]);

		if (!bRet)
		{
			::delete m_pSelectedImages[nIndex];
			m_pSelectedImages[nIndex] = NULL;
		}
	}
	else
		m_pSelectedImages[nIndex] = new CThermalImage(sFileName, nType, false, false);

	if (!m_pSelectedImages[nIndex]->IsValid())
	{
		sInfo.Format(_T("���ļ�[%s]ʧ�ܣ�\n\t%s"),
			sFileName, m_pSelectedImages[nIndex]->GetLastError());
		AfxMessageBox(sInfo);
		::delete m_pSelectedImages[nIndex];
		m_pSelectedImages[nIndex] = NULL;
		return FALSE;
	}

	return TRUE;
}

void CExplorerView::CreateAnalysisView()
{
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->m_sCurrentFile.GetLength() >0)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame != NULL)
			pFrame->CreateAnalysisView(pDoc->m_sCurrentFile);
	}
}

// ˫������ͼ
void CExplorerView::OnDblClkImagesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	int nIndex = pNMLV->iItem;
	if(nIndex < 0)
		return;

	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_sCurrentFile = _T("");
	pDoc->m_nCurItem = nIndex;

	CreateAnalysisView();
}

// ˫��shellList
LRESULT CExplorerView::OnDblclkShellList(WPARAM wParam, LPARAM lParam)
{
	int nIndex = (int)wParam;
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_sCurrentFile = _T("");
//	pDoc->m_nCurItem = nIndex;
	nIndex = pDoc->m_nCurItem;
//	m_wndSourceList.GetItemPath(pDoc->m_sCurrentFile, nIndex);

	CreateAnalysisView();

	return -1;
}

void CExplorerView::OnViewAnalysisFormview() 
{
	CreateAnalysisView();
}

void CExplorerView::OnUpdateViewAnalysisFormview(CCmdUI* pCmdUI) 
{
	CUltraToolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_sCurrentFile.GetLength()>0);
}

void CExplorerView::GetFileBaseInfo(int nItemIndex, struct tagImageFileBaseInfo& baseInfo)
{
	CString sFileName, sTem;

	baseInfo.sTs = m_wndSourceList.GetItemText(nItemIndex, 1);
	baseInfo.sEquipmentUnit = m_wndSourceList.GetItemText(nItemIndex, 2);
	baseInfo.sEquipment = m_wndSourceList.GetItemText(nItemIndex, 3);
	baseInfo.sPhaseGroup = m_wndSourceList.GetItemText(nItemIndex, 4);
	baseInfo.sDate = m_wndSourceList.GetItemText(nItemIndex, 5);
	baseInfo.sTime = m_wndSourceList.GetItemText(nItemIndex, 6);
	baseInfo.sType = m_wndSourceList.GetItemText(nItemIndex, 7);
	baseInfo.sGroupMark = m_wndSourceList.GetItemText(nItemIndex, 8);
	baseInfo.sPath = m_wndSourceList.GetItemText(nItemIndex, 9);

	int nPhaseGroupLen = baseInfo.sPhaseGroup.GetLength();

	if (baseInfo.sGroupMark == _T("N"))		// ������ͼ
	{
		m_nCurPreviewWndCnt = 1;

		sFileName = baseInfo.sPath + baseInfo.sTs + _T("\\") +
					baseInfo.sEquipmentUnit + _T("��") +
					baseInfo.sEquipment;
		if (nPhaseGroupLen > 0)
			sFileName = sFileName + _T("��") + baseInfo.sPhaseGroup + _T("��");
		baseInfo.sFileNameA = sFileName + _T(".") + baseInfo.sType;
		return;
	}

	// ������ͼ��������ͼ
	// ������ͼʱ���������Ķ��࣬����ͼ�ĵ������ֱ����FileNameA or FileNameB
	sFileName = baseInfo.sPath + baseInfo.sTs + _T("\\") +
				baseInfo.sEquipmentUnit + _T("��") +
				baseInfo.sEquipment + _T("��");
	for (int i = 0; i < nPhaseGroupLen; i++)
	{
		sTem.Format(_T("%c��."), baseInfo.sPhaseGroup[i]);
		if (0 == i)
			baseInfo.sFileNameA = sFileName + sTem + baseInfo.sType;
		else if (1 == i)
			baseInfo.sFileNameB = sFileName + sTem + baseInfo.sType;
		else
			baseInfo.sFileNameC = sFileName + sTem + baseInfo.sType;
	}
}

// ��ȡbaseInfo��ָ����ͼ�Ĳ��²�����������ݣ����浽fileData
int CExplorerView::GetFileData(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData)
{
	CString sFileName;
	int nRet = -1;
	int nPhaseGroupLen = baseInfo.sPhaseGroup.GetLength();

	if (baseInfo.sGroupMark == _T("N"))		// ������ͼ
	{
		sFileName = baseInfo.sFileNameA;
		nRet = GetFileData(sFileName, fileData, baseInfo.sPhaseGroup, -1);
	}
	else									// ������������ͼ
	{
		for (int i = 0; i < nPhaseGroupLen; i++)
		{
			if (i == 0)
				sFileName = baseInfo.sFileNameA;
			else if (i == 1)
				sFileName = baseInfo.sFileNameB;
			else
				sFileName = baseInfo.sFileNameC;

			nRet = GetFileData(sFileName, fileData, baseInfo.sPhaseGroup, i);
		}
	}

	return nRet;
}

// nCurPhaseIndex<0��������ͼ��0~1��������������ͼ����ǰ�������sPhaseGroup�еĵ�x�����ͼ
int CExplorerView::GetFileData(CString sFileName, struct tagImageFileData& fileData,
								CString sPhaseGroup, int nCurPhaseIndex)
{
	CThermalImage img;
	EXIFINFO * pExif;
	if (!img.Load(sFileName, THERMAL_IMAGE_FORMAT_JPG))
		return -1;
	pExif = img.GetExifInfo();
	if (NULL == pExif || !pExif->IsThermal)
	{
		TRACE1("Error parsing EXIF: code %s\n", img.GetLastError());
		return -2;
	}

	// ��ȡ�¶�����
	afxCurrentInstanceHandle = _AtlBaseModule.GetModuleInstance();
	afxCurrentResourceHandle = _AtlBaseModule.GetResourceInstance();
	HINSTANCE hInstance = AfxGetInstanceHandle();
	if (!img.CalculateTemperatureOfMeasurementTools(hInstance, theApp.m_tsAppName, theApp.m_tsOrganizationID, theApp.m_tsHostID))
	{
		TRACE0("Failure in calculating temperature: NO measure tools or other reason.\n");
		return -3;
	}

	fileData.nMeasurementRectangleCnt = min(pExif->ThermalParameters.MeasurementRectangleCnt, MEASUREMENT_TOOLS_MAX_CNT);
	fileData.nMeasurementSpotCnt = min(pExif->ThermalParameters.MeasurementSpotCnt, MEASUREMENT_TOOLS_MAX_CNT);

	int nCnt, nIndex;
	BOOL bUseSpot;						// TRUE:Spot��FALSE:Area
	if (fileData.nMeasurementSpotCnt > 0)
	{
		bUseSpot = TRUE;
		nCnt = fileData.nMeasurementSpotCnt;
	}
	else
	{
		bUseSpot = FALSE;
		nCnt = fileData.nMeasurementRectangleCnt;
	}

	// ������²������������һ����ͼ�ģ�
	if (nCurPhaseIndex <= 0)
	{
		fileData.fEmissivity = pExif->ThermalParameters.Emissivity;
		fileData.fReflectedTemp = pExif->ThermalParameters.ReflectedTemperature;
		fileData.fAtmosphericTemp = pExif->ThermalParameters.AtmosphericTemperature;
		fileData.fRelativeHumidity = pExif->ThermalParameters.RelativeHumidity;
		fileData.fDistance = pExif->ThermalParameters.ObjectDistance;
		//fileData.fWindSpeed;					// ����

		fileData.sDate = pExif->DateTimeOriginal;
		nIndex = fileData.sDate.Find(' ');
		if (nIndex > 0)
		{
			fileData.sTime = fileData.sDate.Mid(nIndex + 1);
			fileData.sDate = fileData.sDate.Mid(0, nIndex);
			fileData.sDate.Replace(':', '-');
		}
	}

	// �¶�ֵ���²�
	// �¶�ֵ�����������봦�����ⱨ������ʾ���¶ȼ�������ʾ���¶����²���֡�����
	// A=1.36����������ʾΪ1.4
	// B=1.33����������ʾΪ1.3
	// A-B=1.36-1.33=0.03-->0.0����������ʾΪ0.0����������ʾ���¶�ֵ1.4-1.3=0.1
	// �¶�ֵ�������봦���
	// A=1.36-->1.4����������ʾΪ1.4
	// B=1.33-->1.3����������ʾΪ1.3
	// A-B=1.4-1.3=0.1����������ʾΪ0.1
	int nPhaseGroupLen = sPhaseGroup.GetLength();
	float fDelta = 0.f;
	if (nCurPhaseIndex < 0)						// ������ͼ����
	{
		if (nPhaseGroupLen <= 1)				// 0�������ࣨ�����/�����������𣩣�1�����ࣨ�������
		{
			for (int i = 0; i < nCnt; i++)
			{
				fileData.aBody[i] = bUseSpot ? pExif->ThermalParameters.MeasurementSpot[i].temperature
									: pExif->ThermalParameters.MeasurementRectangle[i].temperatureMax;
				fDelta = fileData.aBody[i] >= 0 ? 0.5f : -0.5f;
				fileData.aBody[i] = ((int)(fileData.aBody[i] * 10 + fDelta)) / 10.f;
			}
		}
		else									// 2������һ�壻3������һ��(if�жϾ�Ϊ��)
		{
			nIndex = sPhaseGroup.Find('A');
			if (nIndex >= 0)
			{
				fileData.aPhaseA[0] = pExif->ThermalParameters.MeasurementRectangle[nIndex].temperatureMax;
				fDelta = fileData.aPhaseA[0] >= 0 ? 0.5f : -0.5f;
				fileData.aPhaseA[0] = ((int)(fileData.aPhaseA[0] * 10 + fDelta)) / 10.f;
			}
			nIndex = sPhaseGroup.Find('B');
			if (nIndex >= 0)
			{
				fileData.aPhaseB[0] = pExif->ThermalParameters.MeasurementRectangle[nIndex].temperatureMax;
				fDelta = fileData.aPhaseB[0] >= 0 ? 0.5f : -0.5f;
				fileData.aPhaseB[0] = ((int)(fileData.aPhaseB[0] * 10 + fDelta)) / 10.f;
			}
			nIndex = sPhaseGroup.Find('C');
			if (nIndex >= 0)
			{
				fileData.aPhaseC[0] = pExif->ThermalParameters.MeasurementRectangle[nIndex].temperatureMax;
				fDelta = fileData.aPhaseC[0] >= 0 ? 0.5f : -0.5f;
				fileData.aPhaseC[0] = ((int)(fileData.aPhaseC[0] * 10 + fDelta)) / 10.f;
			}

			// �����²�
			if (sPhaseGroup.Find('A') >= 0 && sPhaseGroup.Find('B') >= 0)	// AB or BA
				fileData.aPhaseAB[0] = abs(fileData.aPhaseA[0] - fileData.aPhaseB[0]);
			if (sPhaseGroup.Find('A') >= 0 && sPhaseGroup.Find('C') >= 0)	// AC or CA
				fileData.aPhaseAC[0] = abs(fileData.aPhaseA[0] - fileData.aPhaseC[0]);
			if (sPhaseGroup.Find('B') >= 0 && sPhaseGroup.Find('C') >= 0)	// BC or CB
				fileData.aPhaseBC[0] = abs(fileData.aPhaseB[0] - fileData.aPhaseC[0]);
		}
	}
	else										// ������������ͼ��ĳһ�����ͼ
	{
		nIndex = sPhaseGroup.Find('A');
		if (nIndex == nCurPhaseIndex)			// A��
		{
			for (int i = 0; i < nCnt; i++)
			{
				fileData.aPhaseA[i] = pExif->ThermalParameters.MeasurementRectangle[i].temperatureMax;
				fDelta = fileData.aPhaseA[i] >= 0 ? 0.5f : -0.5f;
				fileData.aPhaseA[i] = ((int)(fileData.aPhaseA[i] * 10 + fDelta)) / 10.f;
			}
		}
		else
		{
			nIndex = sPhaseGroup.Find('B');
			if (nIndex == nCurPhaseIndex)		// B��
			{
				for (int i = 0; i < nCnt; i++)
				{
					fileData.aPhaseB[i] = pExif->ThermalParameters.MeasurementRectangle[i].temperatureMax;
					fDelta = fileData.aPhaseB[i] >= 0 ? 0.5f : -0.5f;
					fileData.aPhaseB[i] = ((int)(fileData.aPhaseB[i] * 10 + fDelta)) / 10.f;
				}
			}
			else
			{
				nIndex = sPhaseGroup.Find('C');
				if (nIndex == nCurPhaseIndex)	// C��
				{
					for (int i = 0; i < nCnt; i++)
					{
						fileData.aPhaseC[i] = pExif->ThermalParameters.MeasurementRectangle[i].temperatureMax;
						fDelta = fileData.aPhaseC[i] >= 0 ? 0.5f : -0.5f;
						fileData.aPhaseC[i] = ((int)(fileData.aPhaseC[i] * 10 + fDelta)) / 10.f;
					}
				}
			}
		}

		if (nCurPhaseIndex+1 == nPhaseGroupLen)	// ���һ�࣬���Լ����²�
		{
			for (int i = 0; i < nCnt; i++)		// ��ֻ��������ͼʱ���������Ӧ�Ķ���ȽϷ�������ȫ����
			{
				fileData.aPhaseAB[i] = abs(fileData.aPhaseA[i] - fileData.aPhaseB[i]);
				fileData.aPhaseAC[i] = abs(fileData.aPhaseA[i] - fileData.aPhaseC[i]);
				fileData.aPhaseBC[i] = abs(fileData.aPhaseB[i] - fileData.aPhaseC[i]);
			}
		}
	}

	return 0;
}

// ������ͼ�嵥��nItemIndexָ���ļ�¼�����ɱ���
void CExplorerView::MakeReport(int nItemIndex)
{
	// ���߳��е���Word���������ʼ������
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CoInitialize(NULL);
	AfxEnableControlContainer();

	struct tagImageFileBaseInfo baseInfo;
	struct tagImageFileData fileData;
	CString sReportTemplate, sReportFileName, sTem;
	CString sReportTitle, sReportDate;
	SYSTEMTIME stLocal;

	::GetLocalTime(&stLocal);
	sReportDate.Format(_T("%u-%02u-%02u"), stLocal.wYear, stLocal.wMonth, stLocal.wDay);

	// ��SourceFileList��ȡnItemIndexָ����ͼ�Ļ�����Ϣ
	GetFileBaseInfo(nItemIndex, baseInfo);
	// ��ȡ������ͼ�Ĳ����������¶�����
	if (GetFileData(baseInfo, fileData))
	{
		// TODO:������־���Ա��º����
		return;
	}

	// ����ı��⼰�ĵ���
	sTem = _T("");
	if (m_sReportTitlePrefix.GetLength() > 0)
	{
		static int nReportIndex = 1;
		if (baseInfo.sTs != m_sCurTsName)
		{
			m_sCurTsName = baseInfo.sTs;
			nReportIndex = 1;
		}
		sTem.Format(_T("%s%d  "), m_sReportTitlePrefix, nReportIndex++);
	}
	sReportTitle = sTem + baseInfo.sEquipmentUnit + baseInfo.sEquipment;
	if (0 == m_nPreviewOrPrint && m_bSavePreviewFileToSysTempPath)
		sReportFileName = theApp.GetAppTemDirectory() + sReportTitle + _T(" ���⾫ȷ��ⱨ��.doc");
	else
		sReportFileName = baseInfo.sPath + baseInfo.sTs + _T("\\") + sReportTitle + _T(" ���⾫ȷ��ⱨ��.doc");

	// ��������ģ��
	int nPhaseGroupLen = baseInfo.sPhaseGroup.GetLength();
	if (baseInfo.sGroupMark == _T("Y"))
	{
		if (3 == nPhaseGroupLen)
			sReportTemplate = theApp.m_sAppPath + _T("Templates\\����ֿ�����ģ��.doc");
		else	// 2 == nPhaseGroupLen
			sReportTemplate = theApp.m_sAppPath + _T("Templates\\����ֿ�����ģ��.doc");
	}
	else
	{
		if (nPhaseGroupLen >= 2)
			sReportTemplate = theApp.m_sAppPath + _T("Templates\\��������һ�屨��ģ��.doc");
		else if (nPhaseGroupLen == 1)
			sReportTemplate = theApp.m_sAppPath + _T("Templates\\���౨��ģ��.doc");
		else
		{
			if (fileData.nMeasurementSpotCnt > 0)
				sReportTemplate = theApp.m_sAppPath + _T("Templates\\��������������ģ��.doc");
			else
				sReportTemplate = theApp.m_sAppPath + _T("Templates\\��������������ģ��.doc");
		}
	}

	// ���ɱ���
	CWordDocReport wordReport;

	wordReport.m_structWordOperate.nPreviewOrPrint = m_nPreviewOrPrint;
	wordReport.m_structWordOperate.sWordTemplateName = sReportTemplate;
	wordReport.m_structWordOperate.sWordTemDoc = sReportFileName;

	wordReport.SetReportTitle(sReportTitle);
	sTem = theApp.m_tsLoginName.c_str();
	wordReport.SetReportor(sTem);
	wordReport.SetReportDate(sReportDate);

	// �߳��е���WaitCursor���ƻ�����Word�������Ļ���
	// BeginWaitCursor();
	int nRet = wordReport.GenerateReport(baseInfo, fileData);
	// EndWaitCursor();

	if (wordReport.m_structWordOperate.nPreviewOrPrint != 0)
	{
		if (0 == nRet && m_bMoveImageFileWhileReportSaved)
		{
			CString sDstPath;
			sDstPath = baseInfo.sPath + baseInfo.sTs + _T("\\�Ѵ��� ") + sReportDate + _T("\\");
			if (!PathFileExists(sDstPath))
				CreateDirectory(sDstPath, NULL);

			CString sDstFile, sFileName, sTem;
			int nPos;

			if (baseInfo.sGroupMark == _T("N"))
			{
				nPos = baseInfo.sFileNameA.ReverseFind('\\');
				if (nPos > 0)
				{
					sTem = baseInfo.sFileNameA.Mid(nPos + 1);
					sDstFile = sDstPath + sTem;
					MoveFile(baseInfo.sFileNameA, sDstFile);
				}
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					if (i == 0)
						sFileName = baseInfo.sFileNameA;
					else if (i == 1)
						sFileName = baseInfo.sFileNameB;
					else
						sFileName = baseInfo.sFileNameC;

					nPos = sFileName.ReverseFind('\\');
					if (nPos > 0)
					{
						sTem = sFileName.Mid(nPos + 1);
						sDstFile = sDstPath + sTem;
						MoveFile(sFileName, sDstFile);
					}
				}
			}

			// ɾ���б��ж�Ӧ��item
			m_wndSourceList.DeleteItem(nItemIndex);
		}

		if (0 != nRet)
			AfxMessageBox(_T("��ӡ�򱣴汨����ֹ���!"));
	}
}

// �ϲ���ǰ�ļ��м������ļ�����word�ĵ�
void CExplorerView::MergeReport(CString sPath)
{
	// ���߳��е���Word���������ʼ������
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CoInitialize(NULL);
	AfxEnableControlContainer();

	CStringArray saFiles;

	CFileFind fileFind;
	CString sFileFullName, sSubPath, sTsName(_T("")), sCurDate, sTem;
	BOOL bOK = FALSE;
	int nCnt = 0, nPos;

	CString sReportTemplate, sReportFileName;
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal);
	sCurDate.Format(_T("%u-%02u-%02u"), stLocal.wYear, stLocal.wMonth, stLocal.wDay);

	if (sPath.Right(1) != _T("\\"))
		sPath += _T("\\");
	sTem = sPath + _T("*.*");	// ���������ļ��У�����*.doc

	bOK = fileFind.FindFile(sTem);
	while (bOK)
	{
		bOK = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots())		// ����Ŀ¼
		{
			sSubPath = fileFind.GetFilePath();
			if (PathFileExists(sSubPath) && sSubPath.Find(_T("�Ѵ���")) <0)
				MergeReport(sSubPath);
		}
		else if (!fileFind.IsDirectory() && !fileFind.IsDots())	// ���ļ�
		{
			sFileFullName = fileFind.GetFilePath();				// �ļ�ȫ·�������ļ�����
			sTem = sFileFullName.Mid(sFileFullName.ReverseFind('.') + 1);
			if (sTem.CompareNoCase(_T("doc")))
				continue;

			saFiles.Add(sFileFullName);

			// ��ȡ���վ��
			sTem = sFileFullName.Left(sFileFullName.ReverseFind('\\'));
			nPos = sTem.ReverseFind('\\');
			sTsName = sTem.Mid(nPos + 1);
		}
	}
	fileFind.Close();

	nCnt = saFiles.GetCount();
	if (nCnt <= 1)
	{
		saFiles.RemoveAll();
		return;
	}

	// �����ĵ���
	sTem.Format(_T("%s  ���⾫ȷ��ⱨ��(�϶�) %s.doc"), sTsName, sCurDate);
	if (0 == m_nPreviewOrPrint && m_bSavePreviewFileToSysTempPath)
		sReportFileName = theApp.GetAppTemDirectory() + sTem;
	else
		sReportFileName = sPath + sTem;

	// ��������ģ��
	sReportTemplate = theApp.m_sAppPath + _T("Templates\\�ϲ�����ģ��.doc");

	// ���ɱ���
	CWordDocReport wordReport;

	wordReport.m_structWordOperate.nPreviewOrPrint = m_nPreviewOrPrint;
	wordReport.m_structWordOperate.sWordTemplateName = sReportTemplate;
	wordReport.m_structWordOperate.sWordTemDoc = sReportFileName;

	// �߳��е���WaitCursor���ƻ�����Word�������Ļ���
	// BeginWaitCursor();
	int nRet = wordReport.InsertMerge(saFiles);
	// EndWaitCursor();

	if (0 != nRet)
	{
		AfxMessageBox(_T("�ϲ�����ʱ���ֹ���!"));
		return;
	}

	if (m_nPreviewOrPrint != 0 && m_bMoveReportFileWhileReportMerged)
	{
		CString sDstPath;
		sDstPath = sPath + _T("\\�Ѵ��� ") + sCurDate + _T("\\");
		if (!PathFileExists(sDstPath))
			CreateDirectory(sDstPath, NULL);

		for (int i = 0; i < nCnt; i++)
		{
			sFileFullName = saFiles.GetAt(i);

			nPos = sFileFullName.ReverseFind('\\');
			if (nPos > 0)
			{
				sTem = sFileFullName.Mid(nPos + 1);
				sTem = sDstPath + sTem;
				MoveFile(sFileFullName, sTem);
			}
		}
	}

	saFiles.RemoveAll();
}

void CExplorerView::StartMakeReportThread()
{
	if (m_bIsMakingReport)
		return;

	m_hMakeReport = CreateThread(NULL,
								0,
								MakeReportThread,
								this,
								0,
								&m_dwMakeReportThreadID);
	m_bIsMakingReport = TRUE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != NULL)
		pFrame->ShowProgress(TRUE);
}

BOOL CExplorerView::TerminateMakeReportThread()
{
	if (m_bIsMakingReport == FALSE)
		return TRUE;

	while (m_bIsMakingReport)
	{
		MSG message;
		while (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	::CloseHandle(m_hMakeReport);
	m_bMakeReportTerminated = TRUE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ���ɱ�����߳�
unsigned long _stdcall MakeReportThread(LPVOID lpParam)
{
	CExplorerView *pView = (CExplorerView*)lpParam;
	ASSERT(pView);
	pView->m_wndShellTree.EnableWindow(FALSE);		// ���ã��ž����

	CCheckListCtrl& ListCtrl = pView->m_wndSourceList;

	if (0 == pView->m_nPreviewOrPrint)
	{
		pView->MakeReport(pView->m_nCurItem);
	}
	else
	{
		if (pView->m_bGenerateReportInOrder)	// ����ͷβ��ʼ��ע��MakeReport( )����ɾ��item����
		{
			for (int i = 0; i <ListCtrl.GetItemCount();)
			{
				if (ListView_GetCheckState(ListCtrl.GetSafeHwnd(), i))//GetCheck(i)
					pView->MakeReport(i);
				else
					i++;
			}
		}
		else									// ���б�β��ʼ
		{
			int nCount = ListCtrl.GetItemCount();
			for (int i = nCount - 1; i >= 0; i--)
			{
				if (ListView_GetCheckState(ListCtrl.GetSafeHwnd(), i))//GetCheck(i)
					pView->MakeReport(i);
			}
		}
	}

	pView->m_bIsMakingReport = FALSE;
	pView->m_bMakeReportTerminated = FALSE;
	pView->m_wndShellTree.EnableWindow(TRUE);		// ���ã�������

	::CloseHandle(pView->m_hMakeReport);
	pView->m_hMakeReport = NULL;

	return 0;
}

void CExplorerView::StarMergeReportThread()
{
	if (m_bIsMergingReport)
		return;

	m_hMergeReport = CreateThread(NULL,
					0,
					MergeReportThread,
					this,
					0,
					&m_dwMergeReportThreadID);
	m_bIsMergingReport = TRUE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != NULL)
		pFrame->ShowProgress(TRUE);
}

BOOL CExplorerView::TerminateMergeReportThread()
{
	if (m_bIsMergingReport == FALSE)
		return TRUE;

	while (m_bIsMergingReport)
	{
		MSG message;
		while (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	::CloseHandle(m_hMergeReport);
	m_bMergeReportTerminated = TRUE;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
// �ϲ�������߳�
unsigned long _stdcall MergeReportThread(LPVOID lpParam)
{
	CExplorerView *pView = (CExplorerView*)lpParam;
	ASSERT(pView);
	pView->m_wndShellTree.EnableWindow(FALSE);		// ���ã��ž����

	pView->MergeReport(pView->m_sRootDirectory);

	pView->m_bIsMergingReport = FALSE;
	pView->m_bMergeReportTerminated = FALSE;
	pView->m_wndShellTree.EnableWindow(TRUE);		// ���ã�������

	::CloseHandle(pView->m_hMergeReport);
	pView->m_hMergeReport = NULL;

	return 0;
}
