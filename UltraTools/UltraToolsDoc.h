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
	std::vector<CString>	m_vFileNames;			// 生成缩略图时一次遍历list存入此变量，不含路径
	int						m_nCurItem;				// 当前文件在m_vFileNames中的序号
	CString					m_sRootDirectory;		// 因含嵌套子目录，只能保存“搜索入口”目录
	CString					m_sCurrentFile;			// 当前文件（含路径）

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


