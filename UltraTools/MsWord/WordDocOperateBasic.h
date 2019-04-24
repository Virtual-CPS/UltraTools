// WordDocOperateBasic.h: interface for the CWordDocOperateBasic class.

#pragma once

#include "msword.h"		// Word2003,Ҫ������2003�ģ�Ȼ���ֹ�����ͬ�ĵط��ĺ����ӵ�.h��.cpp��
#include <atlbase.h>	// Com����word�й�

using namespace ms_word;

class CWordDocOperateBasic  
{
public:
	CWordDocOperateBasic();
	virtual ~CWordDocOperateBasic();

// ��Word App����
public:
	BOOL OpenWord();							// ��Word
	BOOL CloseWord();							// �ر�Word
	BOOL PrintDocument();						// ��ӡ��ǰ�ĵ�
	BOOL SaveWordDoc();							// �����ļ�
	CString GetWordVer();						// �õ�Word�汾

public:
	// Word���������ṹ���Ա��ӡ��Ԥ��word�ĵ�ʱ��
	typedef struct
	{
		CString	sWordVer;
		int		nPrintNum;						// ��ӡ����
		CString	sPrintPageSize;					// ��ӡֽ�Ŵ�С��_T("A4")
		int		nPreviewOrPrint;				// 0��Ԥ����1������+��ӡ��2������̨����
		CString	sWordTemplateName;				// Wordģ����(��·��)
		CString	sWordTemDoc;					// ��ʱ�ĵ�����·����
	} tagWordOperate;

	tagWordOperate m_structWordOperate;

	_Application	m_App;	// ����Word�ṩ��Ӧ�ó������;
	Documents		m_Docs;	// ����Word�ṩ���ĵ�����;
	_Document		m_Doc;	// ��ǰ��������ĵ�����
	Selection		m_Sel;	// ����Word�ṩ��ѡ�����;
};