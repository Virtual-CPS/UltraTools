#ifndef __WINSOCKET_H__
#define __WINSOCKET_H__

#include <winsock.h>

#ifndef SD_RECEIVE
//
// WinSock 2 extension -- manifest constants for shutdown()
// but try to use WinSock 1.1 here
//
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#endif

class CWinSocket
{
// Construction
public:
	CWinSocket();

	virtual BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM, 
		char* pSocketAddress = NULL);

// inline
	BOOL GetPeerName(sockaddr* pSockAddr, int* pSockAddrLen)
	{
		return (SOCKET_ERROR != getpeername(m_hSocket, pSockAddr, pSockAddrLen)); 
	}

	BOOL GetSockName(sockaddr* pSockAddr, int* pSockAddrLen)
	{ 
		return (SOCKET_ERROR != getsockname(m_hSocket, pSockAddr, pSockAddrLen)); 
	}

	BOOL Bind (const sockaddr* pSockAddr, int nSockAddrLen)
	{
		return (SOCKET_ERROR != bind(m_hSocket, pSockAddr, nSockAddrLen)); 
	}

	BOOL Connect(const sockaddr* pSockAddr, int nSockAddrLen)
	{ 
		return ConnectHelper(pSockAddr, nSockAddrLen); 
	}

	BOOL Listen(int nConnectionBacklog=5)
	{ 
	    return (SOCKET_ERROR != listen(m_hSocket, nConnectionBacklog)); 
	}

	virtual int Receive(void* pBuf, int nBufLen, int nFlags = 0)
	{
		return recv(m_hSocket, (char*)pBuf, nBufLen, nFlags);
	}
	virtual int Send(const void* pBuf, int nBufLen, int nFlags = 0)
	{
		return send(m_hSocket, (char*)pBuf, nBufLen, nFlags);
	}
	
	DWORD write(const void* pBuf, int nBufLen);
	DWORD read(const void* pBuf, int nBufLen);

	int ReceiveFrom(void* pBuf, int nBufLen, 
		SOCKADDR* pSockAddr, int* pSockAddrLen, int nFlags = 0)
	{
		return ReceiveFromHelper(pBuf, nBufLen, pSockAddr, pSockAddrLen, nFlags); 
	}

	int SendTo(const void* pBuf, int nBufLen,
		const sockaddr* pSockAddr, int nSockAddrLen, int nFlags = 0)
	{ 
		return SendToHelper(pBuf, nBufLen, pSockAddr, nSockAddrLen, nFlags); 
	}

	BOOL ShutDown(int nHow = SD_BOTH)
	{ 
		return (SOCKET_ERROR != shutdown(m_hSocket,nHow)); 
	}

// Implementation
public:
	virtual ~CWinSocket()
	{
		if (m_hSocket != INVALID_SOCKET)
		{
			ShutDown(SD_BOTH);
			Close();
		}
	}

  	virtual BOOL ConnectHelper(const sockaddr* pSockAddr, int nSockAddrLen)
	{
		return connect(m_hSocket, pSockAddr, nSockAddrLen) != SOCKET_ERROR;
	}
	
	virtual int ReceiveFromHelper(void* pBuf, int nBufLen,
		sockaddr * pSockAddr, int* pSockAddrLen, int nFlags)
	{
		return recvfrom(m_hSocket, (char*)pBuf, nBufLen, nFlags, 
			pSockAddr, pSockAddrLen);
	}

	virtual int SendToHelper(const void* pBuf, int nBufLen,
		const sockaddr * pSockAddr, int nSockAddrLen, int nFlags)
	{
		return sendto(m_hSocket, (char*)pBuf, nBufLen, nFlags, pSockAddr, 
			nSockAddrLen);
	}

// Attributes
public:
	SOCKET m_hSocket;

	BOOL GetPeerName(char *pPeerAddress, int MaxAddrLen, UINT& rPeerPort);
	BOOL GetSockName(char* pSocketAddress, int MaxAddrLen, UINT& rSocketPort);
	static int GetLastError() { return WSAGetLastError(); }

// Operations
public:
	static BOOL SocketInit();
	virtual BOOL Accept(CWinSocket* pClientSock, 
		sockaddr* pSockAddr = NULL, int* pSockAddrLen = NULL);
	BOOL Bind(UINT nSocketPort, char* pSocketAddress = NULL);
	virtual void Close();
	BOOL Connect(const char *pHostAddress, UINT nHostPort);
	int ReceiveFrom(void* pBuf, int nBufLen, 
		char* SocketAddress, int MaxAddrLen, UINT& rSocketPort, int nFlags = 0);
	int SendTo(const void* pBuf, int nBufLen, 
		UINT nHostPort, char* pHostAddress = NULL, int nFlags = 0);
};

#endif
