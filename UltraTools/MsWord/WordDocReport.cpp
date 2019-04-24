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

// 填写报表
void CWordDocReport::FillInReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData)
{
	CString sMark, sFileName, sTem;
	COleVariant varstrNull(_T(""));
	COleVariant varZero((short)0);
	COleVariant varTrue(short(1), VT_BOOL);
	COleVariant varFalse(short(0), VT_BOOL);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	int nPhaseGroupLen = baseInfo.sPhaseGroup.GetLength();

	Bookmarks bookmarks; //书签对象
	Range range;
	bookmarks = m_Doc.GetBookmarks();

	// 报表头基本信息
	sMark = _T("报表标题");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(m_sReportTitle);
	}

	sMark = _T("变电站名");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sTs);
	}

	sMark = _T("间隔单元名");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sEquipmentUnit);
	}

	sMark = _T("设备名称");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sEquipment);
	}

	if (nPhaseGroupLen > 0)
	{
		sMark = _T("相别");
		if (bookmarks.Exists(sMark))
		{
			range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
			range.SetText(baseInfo.sPhaseGroup);
		}

		// 文本框中的书签，用下面这句定位不到
		//range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(_T("文本框相别1")));
		// 需用以下两个语句组合定位书签
		//bookmark = bookmarks.Item(COleVariant(_T("文本框相别1")));
		//range = bookmark.GetRange();
		if (nPhaseGroupLen == 2 && baseInfo.sGroupMark == _T("Y"))	// 二相分开
		{
			//Bookmarks bookmarks; //书签对象
			Bookmark bookmark;
			//bookmarks = m_Doc.GetBookmarks();
			//if bookmarks.Exists(_T("name"));

			sMark = _T("文本框相别1");
			if (bookmarks.Exists(sMark))
			{
				bookmark = bookmarks.Item(COleVariant(sMark));
				range = bookmark.GetRange();
				sTem.Format(_T("%c"), baseInfo.sPhaseGroup[0]);
				range.SetText(sTem);
			}

			sMark = _T("文本框相别2");
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

	sMark = _T("拍摄日期");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sDate);
		//range.SetText(fileData.sDate);
	}

	sMark = _T("拍摄时间");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(baseInfo.sTime);
		//range.SetText(fileData.sTime);
	}

	sMark = _T("报告人");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(m_sReportor);
	}

	sMark = _T("报告日期");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		range.SetText(m_sReportDate);
	}

	// 报表头检测参数
	sMark = _T("辐射率");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.2f"), fileData.fEmissivity);
		range.SetText(sTem);
	}

	sMark = _T("测试距离");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.1f"), fileData.fDistance);
		range.SetText(sTem);
	}

	sMark = _T("环境温度");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.1f"), fileData.fAtmosphericTemp);
		range.SetText(sTem);
	}

	sMark = _T("湿度");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%d"), (int)(fileData.fRelativeHumidity * 100));
		range.SetText(sTem);
	}

	sMark = _T("风速");
	if (bookmarks.Exists(sMark))
	{
		range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
		sTem.Format(_T("%0.1f"), fileData.fWindSpeed);
		range.SetText(sTem);
	}

	//插入图片：嵌入方式显示图片
	InlineShapes inlineshapes;
	LPDISPATCH pInlineShapes;
	CComVariant vtTrue(true), vtFalse(false), vtZero(0l), vtNone, vtNullString(_T(""));
	CComVariant vtOpn(DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (baseInfo.sGroupMark == _T("N"))
	{
		sMark = _T("热图");
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
			sMark.Format(_T("热图%c"), 'A' + i);	// 二相分开，不管此二相是哪二相，热图书签都是“热图A/B”
			if (i == 0)								// 且热图文档名存于FileNameA/B
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

	// 检测结果数据
	int nCnt;
	nCnt = fileData.nMeasurementSpotCnt > 0 ? fileData.nMeasurementSpotCnt : fileData.nMeasurementRectangleCnt;
	if (baseInfo.sGroupMark == _T("N"))			// 单幅热图
	{
		if (nPhaseGroupLen <= 1)				// 0：本体类（点分析/框分析，无相别），1：单相（框分析）
		{
			for (int i = 0; i < nCnt; i++)
			{
				sMark.Format(_T("最高温%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aBody[i]);
					range.SetText(sTem);
				}
			}
		}
		else									// 2：二相一体；3：三相一体(if判断均为真)
		{
			int nIndex;							// 根据序号，决定填写某相别的哪个书签
			nIndex = baseInfo.sPhaseGroup.Find('A');
			if (nIndex >= 0)
			{
				sMark.Format(_T("A相最高温%d"), nIndex + 1);
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
				sMark.Format(_T("B相最高温%d"), nIndex + 1);
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
				sMark.Format(_T("C相最高温%d"), nIndex + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseC[0]);
					range.SetText(sTem);
				}
			}
			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('B') >= 0)	// AB or BA
			{
				sMark = _T("AB相最大温差");
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAB[0]);
					range.SetText(sTem);
				}
			}
			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// AC or CA
			{
				sMark = _T("AC相最大温差");
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAC[0]);
					range.SetText(sTem);
				}
			}
			if (baseInfo.sPhaseGroup.Find('B') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// BC or CB
			{
				sMark = _T("BC相最大温差");
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseBC[0]);
					range.SetText(sTem);
				}
			}
		}
	}
	else										// 三相分开（if均为真）或二相分开
	{
		for (int i = 0; i < nCnt; i++)
		{
			if (baseInfo.sPhaseGroup.Find('A') >= 0)
			{
				sMark.Format(_T("A相最高温%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseA[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('B') >= 0)
			{
				sMark.Format(_T("B相最高温%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseB[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('C') >= 0)
			{
				sMark.Format(_T("C相最高温%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseC[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('B') >= 0)	// AB or BA
			{
				sMark.Format(_T("AB相最大温差%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAB[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('A') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// AC or CA
			{
				sMark.Format(_T("AC相最大温差%d"), i + 1);
				if (bookmarks.Exists(sMark))
				{
					range = m_Doc.GoTo(varTrue, varZero, varZero, COleVariant(sMark));
					sTem.Format(_T("%0.1f"), fileData.aPhaseAC[i]);
					range.SetText(sTem);
				}
			}

			if (baseInfo.sPhaseGroup.Find('B') >= 0 && baseInfo.sPhaseGroup.Find('C') >= 0)	// BC or CB
			{
				sMark.Format(_T("BC相最大温差%d"), i + 1);
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

// 入口函数
int CWordDocReport::GenerateReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData)
{
	try
	{
		if (!OpenWord())
			return -1;							// 打开Word App失败

		FillInReport(baseInfo, fileData);		// 填写报表

		// 预览/打印
		if (m_structWordOperate.nPreviewOrPrint == 0)
		{
			m_Doc.PrintPreview();				// 预览，前提是m_App.SetVisible(TRUE);
			m_Docs.ReleaseDispatch();			// 断开关联
			m_Sel.ReleaseDispatch();
		}
		else
		{
			if (!SaveWordDoc())					// 保存
				return -2;
			if (m_structWordOperate.nPreviewOrPrint == 1)
			{
				if (!PrintDocument())
					return -3;
			}
			CloseWord();						// 关闭
		}
	}
	catch (CException& e)
	{
		CString error;
		TCHAR szErr[255];

		e.GetErrorMessage(szErr, 255);
		error.Format(_T("打开word App失败：%s"), szErr);
		AfxMessageBox(error);

		return -4;
	}

	return 0;
}

// 用插入方式合并文档
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
			return -2;							// 打开Word App失败

		// 合并word文档
		Selection selection(m_App.GetSelection());
		for (int i = 0; i < nCnt; i++)
		{
			sFileName = saFiles.GetAt(i);
			selection.InsertFile(sFileName, covOptional, varFalse, varFalse, varFalse);
			if (i < nCnt - 1)
				selection.InsertBreak(COleVariant((long)7)); // 7 = wdPageBreak
		}

		// 预览/打印
		if (m_structWordOperate.nPreviewOrPrint == 0)
		{
			m_Doc.PrintPreview();				// 预览，前提是m_App.SetVisible(TRUE);
			m_Docs.ReleaseDispatch();			// 断开关联
			m_Sel.ReleaseDispatch();
		}
		else
		{
			if (!SaveWordDoc())					// 保存
				return -3;
			if (m_structWordOperate.nPreviewOrPrint == 1)
			{
				if (!PrintDocument())
					return -4;
			}
			CloseWord();						// 关闭
		}
	}
	catch (CException& e)
	{
		CString error;
		TCHAR szErr[255];

		e.GetErrorMessage(szErr, 255);
		error.Format(_T("合并word文档失败：%s"), szErr);
		AfxMessageBox(error);

		return -5;
	}

	return 0;
}

// 用拷贝方式合并文档
// 涉及被合并文档的格式，建议不用该函数
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
			return -1;							// 打开Word App失败

		// 合并word文档
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

		// 预览/打印
		if (m_structWordOperate.nPreviewOrPrint == 0)
		{
			m_Doc.PrintPreview();				// 预览，前提是m_App.SetVisible(TRUE);
			m_Docs.ReleaseDispatch();			// 断开关联
			m_Sel.ReleaseDispatch();
		}
		else
		{
			if (!SaveWordDoc())					// 保存
				return -3;
			if (m_structWordOperate.nPreviewOrPrint == 1)
			{
				if (!PrintDocument())
					return -4;
			}
			CloseWord();						// 关闭
		}
	}
	catch (CException& e)
	{
		CString error;
		TCHAR szErr[255];

		e.GetErrorMessage(szErr, 255);
		error.Format(_T("合并word文档失败：%s"), szErr);
		AfxMessageBox(error);

		return -5;
	}

	return 0;
}
