// WordDocOperateBasic.cpp: implementation of the CWordDocOperateBasic class.

#include "stdafx.h"
#include "WordDocOperateBasic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CWordDocOperateBasic::CWordDocOperateBasic()
{
	//基本参数初始化
	m_structWordOperate.nPreviewOrPrint = 2;		// 默认为仅保存2，直接打印1，0预览
	m_structWordOperate.nPrintNum = 1;				// 打印份数：1
	m_structWordOperate.sPrintPageSize = _T("A4");	// 打印纸张大小：A4
	m_structWordOperate.sWordVer = _T("Word2003");	// Word默认版本：Word2003
	m_structWordOperate.sWordTemplateName = _T("");
	m_structWordOperate.sWordTemDoc = _T("");
}

CWordDocOperateBasic::~CWordDocOperateBasic()
{
}

// Word Ver
CString CWordDocOperateBasic::GetWordVer()
{
	return m_structWordOperate.sWordVer;
}

// 关闭已打开的Word app
BOOL CWordDocOperateBasic::CloseWord()
{
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	try
	{
		m_Docs.ReleaseDispatch();		// 断开关联；
		m_Sel.ReleaseDispatch();

		// 退出Word
		m_App.Quit(covOptional, covOptional, covOptional); 
		m_App.Quit(covOptional, covOptional, covOptional);
		m_App.ReleaseDispatch();
	}
	catch(CException& e)
	{
		CString sError;
		TCHAR szErr[255];
		e.GetErrorMessage(szErr, 255);
		sError.Format(_T("关闭Word App错误：%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}

// 打开Word
BOOL CWordDocOperateBasic::OpenWord()
{
	BOOL bRet = FALSE;
	
	if (!PathFileExists(m_structWordOperate.sWordTemplateName))
		return FALSE;

	// 由模板拷贝生成空的目标文件
	CString sDocName = m_structWordOperate.sWordTemDoc;
	bRet = CopyFile(m_structWordOperate.sWordTemplateName, sDocName, FALSE);	// FALSE：强制覆盖，避免在同一文档中多次填写
	if (bRet == FALSE)
		return FALSE;

	COleVariant varFilePath(sDocName);
	COleVariant varstrNull(_T(""));
	COleVariant varZero((short)0);
	COleVariant varTrue(short(1),VT_BOOL);
	COleVariant varFalse(short(0),VT_BOOL);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	try
	{
		m_Docs.ReleaseDispatch();
		m_Sel.ReleaseDispatch();
		m_App.m_bAutoRelease = true;
		if(!m_App.CreateDispatch(_T("Word.Application")))
			return FALSE;

		// Word是否可见
		if (m_structWordOperate.nPreviewOrPrint == 0)	// 预览方式
			m_App.SetVisible(TRUE);						// 显示Word
		else											// 直接打印方式
			m_App.SetVisible(TRUE);						// 隐藏Word（不能设置为FALSE，可能是微软出于安全考虑）

		// 打开
		m_Docs.AttachDispatch(m_App.GetDocuments());//将Documents类对象m_Docs和Idispatch接口关联起来
		if (m_structWordOperate.sWordVer.CompareNoCase(_T("Word2000")) ==0)	// office2000
		{
			m_Docs.Open(varFilePath,varFalse,varFalse,varFalse,
						varstrNull,varstrNull,varFalse,varstrNull,
						varstrNull,varTrue,varTrue,varTrue);
		}
		else												// office2003
		{
			m_Docs.Open(varFilePath,
						varFalse,    // Confirm Conversion.
						varFalse,    // ReadOnly.
						varFalse,    // AddToRecentFiles.
						covOptional, // PasswordDocument.
						covOptional, // PasswordTemplate.
						varTrue,     // Revert.
						covOptional, // WritePasswordDocument.
						covOptional, // WritePasswordTemplate.
						covOptional, // Format. // Last argument for Word 97
						covOptional, // Encoding // New for Word 2000/2002
						varTrue,     // Visible
						covOptional, // OpenConflictDocument
						covOptional, // OpenAndRepair
						varZero,     // DocumentDirection wdDocumentDirection LeftToRight
						covOptional);// NoEncodingDialog
		}

		// 得到当前激活的文档
		m_Doc = m_App.GetActiveDocument(); 
	}
	catch(CException& e)
	{
		CString sError;
		TCHAR szErr[255];
		e.GetErrorMessage(szErr, 255);
		sError.Format(_T("打开Word App错误：%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}

// 打印当前文档
BOOL CWordDocOperateBasic::PrintDocument()
{
	CString strPrinterName = m_App.GetActivePrinter();
	LPCTSTR lpszActivePrinter(strPrinterName);
	m_App.SetActivePrinter(lpszActivePrinter);			// 设置当前打印机
	COleVariant covTrue((short)TRUE), covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);	// 定义打印属性

	try
	{
		m_Doc.PrintOut( covFalse, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						COleVariant((long)1), 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional, 
						covOptional); 
	}
	catch(CException& e)
	{
		CString sError;
		TCHAR szErr[255];
		e.GetErrorMessage(szErr, 255);
		sError.Format(_T("打印文档失败：%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}

// 保存Word文件
BOOL CWordDocOperateBasic::SaveWordDoc()
{
	COleVariant varstrNull(_T(""));
	COleVariant varZero((short)0);
	COleVariant varTrue(short(1), VT_BOOL);
	COleVariant varFalse(short(0), VT_BOOL);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	try
	{
		if (m_structWordOperate.sWordVer.CompareNoCase(_T("Word2000")) ==0) // Word 2000
		{
			m_Doc.SaveAs(COleVariant(m_structWordOperate.sWordTemDoc),
			varZero, 
			varFalse, varstrNull, varTrue, varstrNull, 
			varFalse, varFalse, varFalse, varFalse, varFalse);
		}
		else // Word 2003
		{
			m_Doc.SaveAs(COleVariant(m_structWordOperate.sWordTemDoc),
			varZero, 
			varFalse, varstrNull, varTrue, varstrNull, 
			varFalse, varFalse, varFalse, varFalse, varFalse,
			varFalse,varFalse,varFalse,varTrue,varTrue);
		}
	}
	catch(CException& e)
	{
		CString sError;
		TCHAR szErr[255];
		e.GetErrorMessage(szErr, 255);
		sError.Format(_T("保存文档失败：%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}