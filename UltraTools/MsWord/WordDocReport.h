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
	// ����ģ�����ɱ���

	// ��ں���
	int GenerateReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData);
	// ��д����
	void FillInReport(struct tagImageFileBaseInfo& baseInfo, struct tagImageFileData& fileData);

	void SetReportTitle(CString sTitle){ m_sReportTitle = sTitle; };
	void SetReportor(CString sReportor){ m_sReportor = sReportor; };
	void SetReportDate(CString sDate){ m_sReportDate = sDate; };
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// �ϲ�word�ĵ�
	int InsertMerge(CStringArray &saFiles);
	int CopyMerge(CStringArray &saFiles);
	//-------------------------------------------------------------------------

protected:
	CString m_sReportTitle;
	CString m_sReportor;
	CString m_sReportDate;
};
