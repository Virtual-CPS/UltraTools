// WordDocOperateBasic.h: interface for the CWordDocOperateBasic class.

#pragma once

#include "msword.h"		// Word2003,要先引入2003的，然后手工将不同的地方的函数加到.h和.cpp里
#include <atlbase.h>	// Com，和word有关

using namespace ms_word;

class CWordDocOperateBasic  
{
public:
	CWordDocOperateBasic();
	virtual ~CWordDocOperateBasic();

// 对Word App操作
public:
	BOOL OpenWord();							// 打开Word
	BOOL CloseWord();							// 关闭Word
	BOOL PrintDocument();						// 打印当前文档
	BOOL SaveWordDoc();							// 保存文件
	CString GetWordVer();						// 得到Word版本

public:
	// Word基本参数结构，以便打印或预览word文档时用
	typedef struct
	{
		CString	sWordVer;
		int		nPrintNum;						// 打印份数
		CString	sPrintPageSize;					// 打印纸张大小：_T("A4")
		int		nPreviewOrPrint;				// 0：预览，1：保存+打印，2：仅后台保存
		CString	sWordTemplateName;				// Word模版名(带路径)
		CString	sWordTemDoc;					// 临时文档（带路径）
	} tagWordOperate;

	tagWordOperate m_structWordOperate;

	_Application	m_App;	// 定义Word提供的应用程序对象;
	Documents		m_Docs;	// 定义Word提供的文档对象;
	_Document		m_Doc;	// 当前被激活的文档对象
	Selection		m_Sel;	// 定义Word提供的选择对象;
};