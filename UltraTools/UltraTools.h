// UltraTools.h : main header file for the UltraTools application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "UltraToolsDoc.h"
#include "Database\SysDatabase.h"

#include "Utility/HvUtility.h"


#ifdef ULTRA_TOOLS_LIVE_VIDEO
#undef ULTRA_TOOLS_LIVE_VIDEO
#endif
#define ULTRA_TOOLS_STATIC_IMAGE

// CUltraToolsApp:
// See UltraTools.cpp for the implementation of this class
//

class CUltraToolsApp : public CBCGPWinApp
{
public:
	CUltraToolsApp();

	// 没有利用常规的[doc-view]，在exlorerView中更新，其它view共享
	// 每个AnalysisView各自从中“选”一个图谱，并独立分析与存储
	// 避免执行CDocument::UpdateAllViews()将全部view“统一显示”
	CUltraToolsDoc			m_docIrImage;

	CMultiDocTemplate*		m_pDocTemplateExplorer;					// 资源管理器
	CMultiDocTemplate*		m_pDocTemplateAnalysis;					// 红外图谱分析
	CMultiDocTemplate*		m_pDocTemplateReport;					// 分析报表


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	CString GetAppTemDirectory(){ return m_sTemFilePath; }

	DECLARE_MESSAGE_MAP()

protected:
	HANDLE					m_hMutex;
	CString					m_sTemFilePath;				// 临时文件路径，以"\\"结尾

	BOOL InitSys();

public:
	BOOL					m_bOnTrial;					// TRUE=试用版
	int						m_nDateCntToExpired;		// 距过期剩余天数
	CString					m_sAppPath;					// 以"\\"结尾
	CString					m_sOrganizationForShort;

	//=============================================================================
	// 数据库
	CSysDatabase			m_SysDatabase;				// 数据库

	//-----------------------------------------------------------------------------
	// 在登录和验证时设置，其它地方只读：
	tstring					m_tsLoginID;				// 登录用户ID
	tstring					m_tsLoginName;				// 登录用户名
	tstring					m_tsLoginPassword;			// 登录用户密码
	tstring					m_tsMD5ofLoginID;			// 登录用户ID的MD5
	tstring					m_tsMD5ofLoginPassword;		// 登录用户密码的MD5
	//=============================================================================

	//=============================================================================
	// 加密参数
	tstring					m_tsAppName, m_tsManufacturerID;
	tstring					m_tsOrganizationID, m_tsOrganizationName;
	tstring					m_tsHostID, m_tsHostName;
	//=============================================================================
};

extern CUltraToolsApp theApp;