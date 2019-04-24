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
	//����������ʼ��
	m_structWordOperate.nPreviewOrPrint = 2;		// Ĭ��Ϊ������2��ֱ�Ӵ�ӡ1��0Ԥ��
	m_structWordOperate.nPrintNum = 1;				// ��ӡ������1
	m_structWordOperate.sPrintPageSize = _T("A4");	// ��ӡֽ�Ŵ�С��A4
	m_structWordOperate.sWordVer = _T("Word2003");	// WordĬ�ϰ汾��Word2003
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

// �ر��Ѵ򿪵�Word app
BOOL CWordDocOperateBasic::CloseWord()
{
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	try
	{
		m_Docs.ReleaseDispatch();		// �Ͽ�������
		m_Sel.ReleaseDispatch();

		// �˳�Word
		m_App.Quit(covOptional, covOptional, covOptional); 
		m_App.Quit(covOptional, covOptional, covOptional);
		m_App.ReleaseDispatch();
	}
	catch(CException& e)
	{
		CString sError;
		TCHAR szErr[255];
		e.GetErrorMessage(szErr, 255);
		sError.Format(_T("�ر�Word App����%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}

// ��Word
BOOL CWordDocOperateBasic::OpenWord()
{
	BOOL bRet = FALSE;
	
	if (!PathFileExists(m_structWordOperate.sWordTemplateName))
		return FALSE;

	// ��ģ�忽�����ɿյ�Ŀ���ļ�
	CString sDocName = m_structWordOperate.sWordTemDoc;
	bRet = CopyFile(m_structWordOperate.sWordTemplateName, sDocName, FALSE);	// FALSE��ǿ�Ƹ��ǣ�������ͬһ�ĵ��ж����д
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

		// Word�Ƿ�ɼ�
		if (m_structWordOperate.nPreviewOrPrint == 0)	// Ԥ����ʽ
			m_App.SetVisible(TRUE);						// ��ʾWord
		else											// ֱ�Ӵ�ӡ��ʽ
			m_App.SetVisible(TRUE);						// ����Word����������ΪFALSE��������΢����ڰ�ȫ���ǣ�

		// ��
		m_Docs.AttachDispatch(m_App.GetDocuments());//��Documents�����m_Docs��Idispatch�ӿڹ�������
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

		// �õ���ǰ������ĵ�
		m_Doc = m_App.GetActiveDocument(); 
	}
	catch(CException& e)
	{
		CString sError;
		TCHAR szErr[255];
		e.GetErrorMessage(szErr, 255);
		sError.Format(_T("��Word App����%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}

// ��ӡ��ǰ�ĵ�
BOOL CWordDocOperateBasic::PrintDocument()
{
	CString strPrinterName = m_App.GetActivePrinter();
	LPCTSTR lpszActivePrinter(strPrinterName);
	m_App.SetActivePrinter(lpszActivePrinter);			// ���õ�ǰ��ӡ��
	COleVariant covTrue((short)TRUE), covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);	// �����ӡ����

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
		sError.Format(_T("��ӡ�ĵ�ʧ�ܣ�%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}

// ����Word�ļ�
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
		sError.Format(_T("�����ĵ�ʧ�ܣ�%s"), szErr);
		AfxMessageBox(sError);
		return FALSE;
	}

	return TRUE;
}