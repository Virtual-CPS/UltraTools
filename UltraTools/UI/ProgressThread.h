// ProgressThread.h

#ifndef __PROGRESS_THREAD_H__
#define __PROGRESS_THREAD_H__


class CProgressThread : public CWinThread
{
public:
	void CloseProgressDlg();
	void SetMessage(const CString& sMessage);
	void SetPos(UINT nPos);

protected:
	CProgressThread();
	virtual ~CProgressThread();

	DECLARE_DYNCREATE(CProgressThread)

	//{{AFX_VIRTUAL(CProgressThread)
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CProgressThread)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CBCGPProgressDlg*		m_pProgressDlg;
public:
	virtual BOOL PumpMessage();
};


#endif //__PROGRESS_THREAD_H__


