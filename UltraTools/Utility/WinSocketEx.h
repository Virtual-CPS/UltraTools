#ifndef __WINSOCKETEX_H__
#define __WINSOCKETEX_H__

#include "WinSocket.h"

#ifndef MSG_LEN
#define MSG_LEN 8192
#endif

#define	NET_TIMEOUT	60 * 1000	// 60 SEC


struct CWinSocketEx : public CWinSocket
{
	const char* GetStringByErrorCode(int ErrorCode);
	BOOL SetSockOpt(WORD On = 1, WORD Timeout = 60);
	BOOL __stdcall IsValidAddress(const void* lp, UINT nBytes, 
		BOOL bReadWrite = TRUE);
	virtual int Receive(void* pBuf, int nBufLen, int nFlags = 0);
	long GetTimeout(void);
};

#endif