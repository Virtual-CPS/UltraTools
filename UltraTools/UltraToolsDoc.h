// UltraToolsDoc.h : interface of the CUltraToolsDoc class
//


#pragma once

#include <vector>


class CUltraToolsDoc : public CDocument
{
public:
	CUltraToolsDoc();
	virtual ~CUltraToolsDoc();
	DECLARE_DYNCREATE(CUltraToolsDoc)

// Attributes
public:
	std::vector<CString>	m_vFileNames;			// ��������ͼʱһ�α���list����˱���������·��
	int						m_nCurItem;				// ��ǰ�ļ���m_vFileNames�е����
	CString					m_sRootDirectory;		// ��Ƕ����Ŀ¼��ֻ�ܱ��桰������ڡ�Ŀ¼
	CString					m_sCurrentFile;			// ��ǰ�ļ�����·����

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);

// Implementation
public:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL SaveModified();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


