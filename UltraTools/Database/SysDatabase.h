// SysDatabase.h: interface for the CSysDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SYS_DATABASE_H_)
#define SYS_DATABASE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AdoDatabase.h"
#include "../Utility/HvUtility.h"

// 本系统用户帐号类型，不同的类型，其具有的权限不同：
#define	SYS_ACCOUNT_INVALID			-1			// 非法用户
#define SYS_ACCOUNT_GUEST			0			// 访客，只能了解系统的功能，不能进行任何实质性的操作
#define SYS_ACCOUNT_NORMAL			1			// 普通用户
#define SYS_ACCOUNT_QUALITY			2			// 质量分析帐号
#define SYS_ACCOUNT_ADMIN			3			// 管理员
#define SYS_ACCOUNT_SUPERUSER		99			// 超级用户(系统后门)
#define SYS_ACCOUNT_DEVELOPER		100			// 超级用户(系统后门，系统调试员帐号)

#define HOST_INVALID				-2			// 运行UltraTools的主机信息无效
#define HOST_UNVERIFIED				-1			// 因数据库连接失败等原因而未验证主机
#define HOST_AUTHORIZED				0
#define HOST_UNAUTHORIZED			1
#define HOST_EXPIRED				2			// 已超过试用日期或试用次数

class CSysDatabase
{
public:
	CSysDatabase();
	virtual ~CSysDatabase();

	CBFAdoConnection m_adoDatabaseConn;
 
	CString GetAdoConnectStr(){ return m_sAdoConnectDB; }
	BOOL OpenAdoConnection(BOOL bReOpen = FALSE)
	{
		return m_adoDatabaseConn.OpenConnection(m_sAdoConnectDB, bReOpen);
	}

	BOOL InitDatabase();				// 初始化数据连接:   TRUE=连接成功,FASE=连接失败     
	int  UserPermissionValidate();		// 验证用户合法性

	int CheckHost(tstring tsHostName, tstring tsHostID);
	int AuthorizeHost(tstring tsHostName, tstring tsHostID,
					tstring tsAuthorizerID, tstring tsAuthorizerPassword);


protected:
	CString			m_sAdoConnectDB;			// ADO连接字符串

	void CloseDatabase();						// 断开数据库连接
};

#endif // !defined(ENGINE_MIS_DB_H_)
