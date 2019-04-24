
#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

//#include <winsock.h>

class CShutdown
{
public:
	CShutdown(int flag, int seconds);
	void Usage();
	BOOL ShutdownNow();
	BOOL ShutdownNow(char* RemoteName, int Seconds = 20, BOOL Reboot = FALSE);
	void ShutdownAll(int Seconds = 20, BOOL Reboot = FALSE);
	BOOL ParseCmdLine(char* CmdLine);
	int  GetFlag()		{ return Flag; }
	int  GetSeconds()	{ return Seconds; }

private:
	int	Flag;					// shutdown flag 
	int Seconds;				// shutdown time delay
	const char* GetUsage();
	BOOL CheckNTPrivilege();	// for NT/2000/XP only
};

#endif
