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

// ��ϵͳ�û��ʺ����ͣ���ͬ�����ͣ�����е�Ȩ�޲�ͬ��
#define	SYS_ACCOUNT_INVALID			-1			// �Ƿ��û�
#define SYS_ACCOUNT_GUEST			0			// �ÿͣ�ֻ���˽�ϵͳ�Ĺ��ܣ����ܽ����κ�ʵ���ԵĲ���
#define SYS_ACCOUNT_NORMAL			1			// ��ͨ�û�
#define SYS_ACCOUNT_QUALITY			2			// ���������ʺ�
#define SYS_ACCOUNT_ADMIN			3			// ����Ա
#define SYS_ACCOUNT_SUPERUSER		99			// �����û�(ϵͳ����)
#define SYS_ACCOUNT_DEVELOPER		100			// �����û�(ϵͳ���ţ�ϵͳ����Ա�ʺ�)

#define HOST_INVALID				-2			// ����UltraTools��������Ϣ��Ч
#define HOST_UNVERIFIED				-1			// �����ݿ�����ʧ�ܵ�ԭ���δ��֤����
#define HOST_AUTHORIZED				0
#define HOST_UNAUTHORIZED			1
#define HOST_EXPIRED				2			// �ѳ����������ڻ����ô���

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

	BOOL InitDatabase();				// ��ʼ����������:   TRUE=���ӳɹ�,FASE=����ʧ��     
	int  UserPermissionValidate();		// ��֤�û��Ϸ���

	int CheckHost(tstring tsHostName, tstring tsHostID);
	int AuthorizeHost(tstring tsHostName, tstring tsHostID,
					tstring tsAuthorizerID, tstring tsAuthorizerPassword);


protected:
	CString			m_sAdoConnectDB;			// ADO�����ַ���

	void CloseDatabase();						// �Ͽ����ݿ�����
};

#endif // !defined(ENGINE_MIS_DB_H_)
