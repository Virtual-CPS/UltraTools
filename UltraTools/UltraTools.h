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

	// û�����ó����[doc-view]����exlorerView�и��£�����view����
	// ÿ��AnalysisView���Դ��С�ѡ��һ��ͼ�ף�������������洢
	// ����ִ��CDocument::UpdateAllViews()��ȫ��view��ͳһ��ʾ��
	CUltraToolsDoc			m_docIrImage;

	CMultiDocTemplate*		m_pDocTemplateExplorer;					// ��Դ������
	CMultiDocTemplate*		m_pDocTemplateAnalysis;					// ����ͼ�׷���
	CMultiDocTemplate*		m_pDocTemplateReport;					// ��������


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
	CString					m_sTemFilePath;				// ��ʱ�ļ�·������"\\"��β

	BOOL InitSys();

public:
	BOOL					m_bOnTrial;					// TRUE=���ð�
	int						m_nDateCntToExpired;		// �����ʣ������
	CString					m_sAppPath;					// ��"\\"��β
	CString					m_sOrganizationForShort;

	//=============================================================================
	// ���ݿ�
	CSysDatabase			m_SysDatabase;				// ���ݿ�

	//-----------------------------------------------------------------------------
	// �ڵ�¼����֤ʱ���ã������ط�ֻ����
	tstring					m_tsLoginID;				// ��¼�û�ID
	tstring					m_tsLoginName;				// ��¼�û���
	tstring					m_tsLoginPassword;			// ��¼�û�����
	tstring					m_tsMD5ofLoginID;			// ��¼�û�ID��MD5
	tstring					m_tsMD5ofLoginPassword;		// ��¼�û������MD5
	//=============================================================================

	//=============================================================================
	// ���ܲ���
	tstring					m_tsAppName, m_tsManufacturerID;
	tstring					m_tsOrganizationID, m_tsOrganizationName;
	tstring					m_tsHostID, m_tsHostName;
	//=============================================================================
};

extern CUltraToolsApp theApp;