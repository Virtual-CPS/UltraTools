#ifndef __LOCK_H__
#define __LOCK_H__

class CLock
{
public:
	CLock()			{ InitializeCriticalSection(&m_CS); }
	~CLock()		{ DeleteCriticalSection(&m_CS); }
	void Lock()		{ EnterCriticalSection(&m_CS); }	
	void Unlock()	{ LeaveCriticalSection(&m_CS); }
private:
	CRITICAL_SECTION m_CS;
};

#endif