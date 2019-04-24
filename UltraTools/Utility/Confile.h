#ifndef __CONFILE_H__
#define __CONFILE_H__

#include "Str.h"
#include "Lock.h"

class CConfigFile
{
public:
	CConfigFile() : m_bDebug(true), m_bLogInit(false) {}

	virtual void Lock()		{}
	virtual void Unlock()	{}

	void LogInit(const char *LogFile, const char *Version);
	virtual void LogMsg(const char *Function, int nLine, const char *Fmt, ...);
	void LogEnd();

	virtual ~CConfigFile() { LogEnd(); }

protected:
	// ini file operations
	CStr GetConfString(const CStr& Section, const CStr& Item, const CStr& Default);
	int	 GetConfInt(const CStr& Section, const CStr& Item, int Default);
	bool CheckExist(const CStr& Name);

	CStr m_IniFile;				// ini file
	bool m_bDebug;				// 1 for logging info into the log file, 0 for no

	bool m_bLogInit;			// init log file flag, setting by the LogInit function
	char m_LogText[512];		// string for storing log text
	char m_LogFile[_MAX_PATH];	// string for log file
};

#endif