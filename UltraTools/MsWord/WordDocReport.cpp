// WordDocReport.cpp: implementation of the CWordDocReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WordDocReport.h"
#include "../GlobalDefsForSys.h"

#include <afxdisp.h>			// COleVariant

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWordDocReport::CWordDocReport()
{
}

CWordDocReport::~CWordDocReport()
{
}

// ��д����
void CWordDocReport::FillInReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData)
{
	CString sMark, sFileName, sTem;
	COleVariant varstrNull(_T(""));
	COleVariant varZero((short)0);
	COleVariant varTrue(short(1), VT_BOOL);
	COleVariant varFalse(short(0), VT_BOOL);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	int nPhaseGroupLen = baseInfo.sPhaseGroup.GetLength();

	Bookmarks bookmarks; //��ǩ����
	Range range;
	bookmarks = m_Doc.GetBookmarks();

	// ����ͷ������Ϣ
	sMark = _T("�������");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(m_sReportTitle);
	}

	sMark = _T("���վ��");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sTs);
	}

	sMark = _T("�����Ԫ��");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sEquipmentUnit);
	}

	sMark = _T("�豸����");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sEquipment);
	}

	if (nPhaseGroupLen > 0)
	{
		sMark = _T("���");
		if (bookmarks.Exists(sMark))
		{
			range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
			range.SetText(baseInfo.sPhaseGroup);
		}

		// �ı����е���ǩ����������䶨λ����
		//range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(_T("�ı������1")));
		// �����������������϶�λ��ǩ
		//bookmark = bookmarks.Item(COleVariant(_T("�ı������1")));
		//range = bookmark.GetRange();
		if (nPhaseGroupLen == 2 && baseInfo.sGroupMark == _T("Y"))	// ����ֿ�
		{
			//Bookmarks bookmarks; //��ǩ����
			Bookmark bookmark;
			//bookmarks = m_Doc.GetBookmarks();
			//if bookmarks.Exists(_T("name"));

			sMark = _T("�ı������1");
			if (bookmarks.Exists(sMark))
			{
				bookmark = bookmarks.Item(COleVariant(sMark));
				range = bookmark.GetRange();
				sTem.Format(_T("%c"), baseInfo.sPhaseGroup[0]);
				range.SetText(sTem);
			}

			sMark = _T("�ı������2");
			if (bookmarks.Exists(sMark))
			{
				bookmark = bookmarks.Item(COleVariant(sMark));
				range = bookmark.GetRange();
				sTem.Format(_T("%c"), baseInfo.sPhaseGroup[1]);
				range.SetText(sTem);
			}

			bookmark.ReleaseDispatch();
			//bookmarks.ReleaseDispatch();
		}
	}

	sMark = _T("��������");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sDate);
		//range.SetText(fileData.sDate);
	}

	sMark = _T("����ʱ��");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sTime);
		//range.SetText(fileData.sTime);
	}

	sMark = _T("������");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(m_sReportor);
	}

	sMark = _T("��������");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(m_sReportDate);
	}

	// ����ͷ������
	sMark = _T("������");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.2f"), fileData.fEmissivity);
		range.SetText(sTem);
	}

	sMark = _T("���Ծ���");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.1f"), fileData.fDistance);
		range.SetText(sTem);
	}

	sMark = _T("�����¶�");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.1f"), fileData.fAtmosphericTemp);
		range.SetText(sTem);
	}

	sMark = _T("ʪ��");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%d"), (int)(fileData.fRelativeHumidity * 100));
		range.SetText(sTem);
	}

	sMark = _T("����");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.1f"), fileData.fWindSpeed);
		range.SetText(sTem);
	}

	//����ͼƬ��Ƕ�뷽ʽ��ʾͼƬ
	InlineShapes inlineshapes;
	LPDISPATCH pInlineShapes;
	CComVariant vtTrue(true), vtFalse(false), vtZero(0l), vtNone, vtNullString(_T(""));
	CComVariant vtOpn(DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (baseInfo.sGroupMark == _T("N"))
	{
		sMark = _T("��ͼ");
		if (bookmarks.Exists(sMark))
		{
			range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
			pInlineShapes = range.GetInlineShapes();
			inlineshapes.AttachDispatch(pInlineShapes);
			inlineshapes.AddPicture(baseInfo.sFileNameA, &vtFalse, &vtTrue, &vtOpn);
			inlineshapes.ReleaseDispatch();
		}
	}
	else
	{
		for (int i = 0; i < nPhaseGroupLen; i++)
		{
			sMark.Format(_T("��ͼ%c"), 'A' + i);	// ����ֿ������ܴ˶������Ķ��࣬��ͼ��ǩ���ǡ���ͼA/B��
			if (i == 0)								// ����ͼ�ĵ�������FileNameA/B
				sFileName = baseInfo.sFileNameA;
			else if (i == 1)
				sFileName = baseInfo.sFileNameB;
			else
				sFileName = baseInfo.sFileNameC;

			if (bookmarks.Exists(sMark))
			{
				range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
				pInlineShapes = range.GetInlineShapes();
				inlineshapes.AttachDispatch(pInlineShapes);
				inlineshapes.AddPicture(sFileName, &vtFalse, &vtTrue, &vtOpn);
				inlineshapes.ReleaseDispatch();
			}
		}
	}

	// ���������
	int nCnt;
	nCnt = fileData.nMeasurementSpotCnt > 0 ? fileData.nMeasurementSpotCnt : fileData.nMeasurementRectangleCnt;
	if (baseInfo.sGroupMark == _T("N"))			// ������ͼ
	{
		if (nPhaseGroupLen <= 1)				// 0�������ࣨ�����/�����������𣩣�1�����ࣨ�������
		{
			for (int i = 0; i < nCnt; i++)
			{
				sMark.Format(_T("�����%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aBody[i]);
					range.SetText(sTem);
				}
			}
		}
		else									// 2������һ�壻3������һ��(if�жϾ�Ϊ��)
		{
			int nIndex;							// ������ţ�������дĳ�����ĸ���ǩ
			nIndex = baseInfo.sPhaseGroup.Find('A');
			if (nIndex >= 0)
			{
				sMark.Format(_T("A�������%d"), nIndex + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseA[0]);
					range.SetText(sTem);
				}
			}
			nIndex = baseInfo.sPhaseGroup.Find('B');
			if (nIndex >= 0)
			{
				sMark.Format(_T("B�������%d"), nIndex + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseB[0]);
					range.SetText(sTem);
				}
			}
			nIndex = baseInfo.sPhaseGroup.Find('C');
			if (nIndex >= 0)
			{
				sMark.Format(_T("C�������%d"), nIndex + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseC[0]);
					range.SetText(sTem);
				}
			}
			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('B') >= 0)	// AB or BA
			{
				sMark = _T("AB������²�");
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAB[0]);
					range.SetText(sTem);
				}
			}
			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// AC or CA
			{
				sMark = _T("AC������²�");
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAC[0]);
					range.SetText(sTem);
				}
			}
			if (baseInfo.sPhaseGroup.Find('B') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// BC or CB
			{
				sMark = _T("BC������²�");
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseBC[0]);
					range.SetText(sTem);
				}
			}
		}
	}
	else										// ����ֿ���if��Ϊ�棩�����ֿ�
	{
		for (int i = 0; i < nCnt; i++)
		{
			if (baseInfo.sPhaseGroup.Find('A') >= 0)
			{
				sMark.Format(_T("A�������%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseA[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('B') >= 0)
			{
				sMark.Format(_T("B�������%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseB[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('C') >= 0)
			{
				sMark.Format(_T("C�������%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseC[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('B') >= 0)	// AB or BA
			{
				sMark.Format(_T("AB������²�%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAB[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// AC or CA
			{
				sMark.Format(_T("AC������²�%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAC[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('B') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// BC or CB
			{
				sMark.Format(_T("BC������²�%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseBC[i]);
					range.SetText(sTem);
				}
			}
		}
	}

	range.ReleaseDispatch();
	bookmarks.ReleaseDispatch();
}

// ��ں���
int CWordDocReport::GenerateReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData)
{
	try
	{
		if (!OpenWord())
			return -1;							// ��Word Appʧ��

		FillInReport(baseInfo, fileData);		// ��д����

		// Ԥ��/��ӡ
		if (m_structWordOperate.nPreviewOrPrint == 0)
		{
			m_Doc.PrintPreview();				// Ԥ����ǰ����m_App.SetVisible(TRUE);
			m_Docs.ReleaseDispatch();			// �Ͽ�����
			m_Sel.ReleaseDispatch();
		}
		else
		{
			if (!SaveWordDoc())					// ����
				return -2;
			if (m_structWordOperate.nPreviewOrPrint == 1)
			{
				if (!PrintDocument())
					return -3;
			}
			CloseWord();						// �ر�
		}
	}
	catch (CException& e)
	{
		CString error;
		TCHAR szErr[255];

		e.GetErrorMessage(szErr, 255);
		error.Format(_T("��word Appʧ�ܣ�%s"), szErr);
		AfxMessageBox(error);

		return -4;
	}

	return 0;
}

// �ò��뷽ʽ�ϲ��ĵ�
int CWordDocReport::InsertMerge(CStringArray &saFiles)
{
	COleVariant varFalse(short(0), VT_BOOL);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString sFileName;
	int nCnt = saFiles.GetCount();
	if (nCnt <= 1)
		return 0;

	try
	{
		if (!OpenWord())
			return -2;							// ��Word Appʧ��

		// �ϲ�word�ĵ�
		Selection selection(m_App.GetSelection());
		for (int i = 0; i < nCnt; i++)
		{
			sFileName = saFiles.GetAt(i);
			selection.InsertFile(sFileName, covOptional, varFalse, varFalse, varFalse);
			if (i < nCnt - 1)
				selection.InsertBreak(COleVariant((long)7)); // 7 = wdPageBreak
		}

		// Ԥ��/��ӡ
		if (m_structWordOperate.nPreviewOrPrint == 0)
		{
			m_Doc.PrintPreview();				// Ԥ����ǰ����m_App.SetVisible(TRUE);
			m_Docs.ReleaseDispatch();			// �Ͽ�����
			m_Sel.ReleaseDispatch();
		}
		else
		{
			if (!SaveWordDoc())					// ����
				return -3;
			if (m_structWordOperate.nPreviewOrPrint == 1)
			{
				if (!PrintDocument())
					return -4;
			}
			CloseWord();						// �ر�
		}
	}
	catch (CException& e)
	{
		CString error;
		TCHAR szErr[255];

		e.GetErrorMessage(szErr, 255);
		error.Format(_T("�ϲ�word�ĵ�ʧ�ܣ�%s"), szErr);
		AfxMessageBox(error);

		return -5;
	}

	return 0;
}

// �ÿ�����ʽ�ϲ��ĵ�
// �漰���ϲ��ĵ��ĸ�ʽ�����鲻�øú���
int CWordDocReport::CopyMerge(CStringArray &saFiles)
{
	COleVariant varFalse(short(0), VT_BOOL);
	COleVariant varZero((short)0);
	COleVariant varOne((short)1);
	COleVariant varTwo((short)2);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString sFileName;
	int nCnt = saFiles.GetCount();
	if (nCnt <= 1)
		return -1;

	try
	{
		if (!OpenWord())
			return -1;							// ��Word Appʧ��

		// �ϲ�word�ĵ�
		Selection selection(m_App.GetSelection());
		for (int i = 0; i < nCnt; i++)
		{
			sFileName = saFiles.GetAt(i);
			// wdMergeTargetCurrent		1	Merge into current document.
			// wdMergeTargetNew			2	Merge into new document.
			// wdMergeTargetSelected	0	Merge into selected document.
			//
			// wdFormattingFromCurrent	1	Copy source formatting from the current item.
			// wdFormattingFromPrompt	2	Prompt the user for formatting to use.
			// wdFormattingFromSelected	0	Copy source formatting from the current selection.
			m_Doc.Merge(sFileName, varZero, varFalse, varOne, varFalse);
			if (i < nCnt - 1)
				selection.InsertBreak(COleVariant((long)7)); // 7 = wdPageBreak
		}

		// Ԥ��/��ӡ
		if (m_structWordOperate.nPreviewOrPrint == 0)
		{
			m_Doc.PrintPreview();				// Ԥ����ǰ����m_App.SetVisible(TRUE);
			m_Docs.ReleaseDispatch();			// �Ͽ�����
			m_Sel.ReleaseDispatch();
		}
		else
		{
			if (!SaveWordDoc())					// ����
				return -3;
			if (m_structWordOperate.nPreviewOrPrint == 1)
			{
				if (!PrintDocument())
					return -4;
			}
			CloseWord();						// �ر�
		}
	}
	catch (CException& e)
	{
		CString error;
		TCHAR szErr[255];

		e.GetErrorMessage(szErr, 255);
		error.Format(_T("�ϲ�word�ĵ�ʧ�ܣ�%s"), szErr);
		AfxMessageBox(error);

		return -5;
	}

	return 0;
}
