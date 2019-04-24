// WordDocReport.h: interface for the CWordDocReport class.

#pragma once

#include "WordDocOperateBasic.h"

class CWordDocReport : public CWordDocOperateBasic
{
public:
	CWordDocReport();
	virtual ~CWordDocReport();

public:
	//-------------------------------------------------------------------------
	// 利用模板生成报告

	// 入口函数
	int GenerateReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData);
	// 填写报表
	void FillInReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData);

	void SetReportTitle(CString sTitle){ m_sReportTitle = sTitle; };
	void SetReportor(CString sReportor){ m_sReportor = sReportor; };
	void SetReportDate(CString sDate){ m_sReportDate = sDate; };
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// 合并word文档
	int InsertMerge(CStringArray &saFiles);
	int CopyMerge(CStringArray &saFiles);
	//-------------------------------------------------------------------------

protected:
	CString m_sReportTitle;
	CString m_sReportor;
	CString m_sReportDate;
};
