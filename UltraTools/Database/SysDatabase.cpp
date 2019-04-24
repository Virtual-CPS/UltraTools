// SysDatabase.cpp: implementation of the CSysDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../UltraTools.h"
#include "SysDatabase.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSysDatabase::CSysDatabase()
{
}

CSysDatabase::~CSysDatabase()
{
	CloseDatabase();
}

///////////////////////////////////////////////////////////////////////////////
// 函数功能: 打开数据库, 建立与数据源的连接
// 返回值:   TRUE=连接成功
//           FASE=连接失败 
//
BOOL CSysDatabase::InitDatabase()
{
	// 先关闭全部数据库连接（ADO及ODBC两种数据库连接）
	CloseDatabase();

	CString sConnect, sDatabaseNamePath, sErr;
	tstring tsPassword, tsTem;
/*
	CWinApp* pApp = AfxGetApp();
    const char *pszKey = "Initial";
	int nDatabaseServerType;
	CString sDatabaseServerName, sDatabaseName, sDatabaseAccount, sDatabasePassword;

	//-------------------------------------------------------------------------
	// 获取数据库设置：
	nDatabaseServerType = pApp->GetProfileInt(pszKey,"DatabaseServerType", 0);
	nDatabaseServerType = (nDatabaseServerType==1) ? 1 : 0;
	sDatabaseServerName = pApp->GetProfileString(pszKey,"DatabaseServerName","(local)");
	sDatabaseName = pApp->GetProfileString(pszKey,"DatabaseName", "");
	sDatabaseAccount = pApp->GetProfileString(pszKey,"DatabaseLoginName", "");
	sDatabasePassword = pApp->GetProfileString(pszKey,"DatabasePassword", "");
//	AfxMessageBox(sDatabasePassword);
	sDatabasePassword = Decrypt(sDatabasePassword);				// 口令解密
//	AfxMessageBox(sDatabasePassword);

	if(sDatabaseName.GetLength() == 0)
		sDatabaseName = "UPCS";
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// 连接Microsoft SQL Server数据库(ODBC for modify database; ADO for query)
	if(nDatabaseServerType == 1)	// 连接MS SQL Server
	{
		theApp.m_sAdoConnectDB.Format("Provider=SQLOLEDB;Data Source=%s;Initial Catalog=%s;User Id=%s;Password=%s",
						sDatabaseServerName,sDatabaseName,sDatabaseAccount,sDatabasePassword);
		sConnect.Format("DRIVER={SQL Server}; SERVER=%s;DATABASE=%s;UID=%s;PWD=%s",
						sDatabaseServerName,sDatabaseName,sDatabaseAccount,sDatabasePassword);
		theApp.m_sOdbcConnectDB.Format("ODBC;DRIVER={SQL Server}; SERVER=%s;DATABASE=%s;UID=%s;PWD=%s",
						sDatabaseServerName,sDatabaseName,sDatabaseAccount,sDatabasePassword);// Trusted_Connection=yes;";
	}
	else							// 连接Oracle
	{
		theApp.m_sAdoConnectDB.Format("Provider=OraOLEDB.Oracle;Data Source=%s;User Id=%s;Password=%s",
						sDatabaseName,sDatabaseAccount,sDatabasePassword);
		sConnect.Format("DRIVER={Microsoft ODBC for Oracle}; SERVER=%s;UID=%s;PWD=%s",
						sDatabaseName,sDatabaseAccount,sDatabasePassword);
		theApp.m_sOdbcConnectDB = sConnect;
	}
	//AfxMessageBox(theApp.m_sAdoConnectDB);
	ShowConnectingDlg("正在连接数据库服务器...");
	if(!m_adoDatabaseConn.OpenConnection(theApp.m_sAdoConnectDB))	// ADO连接
	{
		HideConnectingDlg();
		return FALSE;
	}
	try
	{
		m_odbcDatabase.OpenEx(_T(sConnect),CDatabase::useCursorLib|CDatabase::noOdbcDialog);
	}
	catch(CDBException *e)
	{
		HideConnectingDlg();

		sErr.Format("数据库初始化失败[ODBC]:\n\n%s", e->m_strError);
		AfxMessageBox(sErr);
		e->Delete();
		return FALSE;
	}
	//-------------------------------------------------------------------------
*/
	// 连接Microsoft Access数据库
	sDatabaseNamePath = theApp.m_sAppPath + _T("Data\\UltraTools.accdb");
	if (!PathFileExists(sDatabaseNamePath))
	{
		AfxMessageBox(_T("系统数据库文件丢失!"));
		return FALSE;
	}
	//tsPassword = _T("zrb@hfut#2016-07");
	tsTem = _T("mpvxTYi+xxXFDCN4rBFjSqO+1qdaApw/IbE9dqhBzofpIsU=");
	HvUtility::Decrypt(tsTem, tsPassword);
	m_sAdoConnectDB.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;")
				_T("Jet OLEDB:Database Password=%s;")
				_T("Data Source=%s;Persist Security Info=True"),
				tsPassword.c_str(), sDatabaseNamePath);
	//AfxMessageBox(m_sAdoConnectDB);
	if (!OpenAdoConnection())
		return FALSE;							// 连接数据库失败

	return TRUE;
}

void CSysDatabase::CloseDatabase()
{
	// 如数据库连接未关闭, 则关闭；关闭已关闭的ADO连接将导致异常
	if(!m_adoDatabaseConn.IsConnectClosed())
		m_adoDatabaseConn.CloseConnect();
}

///////////////////////////////////////////////////////////////////////////////
// 函数功能: 验证用户的合法性及其权限
// 返回值:   系统用户帐号类型
//
int CSysDatabase::UserPermissionValidate()
{
	return SYS_ACCOUNT_SUPERUSER;
/*
	// MD5 of [busy_hfut_fish]/[busy_hfut_fish:10-14]
	if(theApp.m_sMD5ofLoginID == "c466a46293ff9010c36c8be5ba5c4a23" &&
		theApp.m_sMD5ofLoginPassword == "17824199f30a162d84b82c918886ad93")
		return SYS_ACCOUNT_SUPERUSER;					// 超级用户后门

	// MD5 of [busy_developer_fish:10-14]
	if(theApp.m_sMD5ofLoginID == "04a93cf615e4c2903bcf894942bbca82" &&
		theApp.m_sMD5ofLoginPassword == "04a93cf615e4c2903bcf894942bbca82")
		return SYS_ACCOUNT_DEVELOPER;					// 开发者后门

	CString sSQL, sPermission, sPassword, sTem;
	if(!m_adoDatabaseConn.OpenConnection(theApp.m_sAdoConnectDB))
		return SYS_ACCOUNT_INVALID;

	sSQL = "SELECT User_Name,Permission,Password FROM tbl_Operator ";
	sTem.Format("WHERE ID=\'%s\' AND On_Guard=1",
				theApp.m_tsLoginID);
	sSQL += sTem;
	//AfxMessageBox(sSQL);

	// 建立记录集:
	_RecordsetPtr* prsRecSet;
	CBFRstValues rsvRecValue;
	prsRecSet = new _RecordsetPtr;
	if(!(m_adoDatabaseConn.OpenRecordset(sSQL, prsRecSet)))
	{	// 如果记录集打开失败
		if(prsRecSet != NULL)
		{
			delete prsRecSet;
			prsRecSet = NULL;
		}
		return SYS_ACCOUNT_INVALID;
	}
	rsvRecValue.InitConnectAndRst(m_adoDatabaseConn.GetConnection(),prsRecSet);

	int nRet = SYS_ACCOUNT_INVALID;						// 非法用户
	if(!(*prsRecSet)->adoEOF)
	{
		rsvRecValue.GetValueStr(theApp.m_tsLoginName, "User_Name");
		rsvRecValue.GetValueStr(sPermission, "Permission");
		rsvRecValue.GetValueStr(sPassword, "Password");

		sPassword = Decrypt(sPassword);					// 解密
		GetMD5(sPassword, sTem);					    // 计算MD5

		if(sTem == theApp.m_sMD5ofLoginPassword)
		{	// 合法用户
			sPermission = Decrypt(sPermission, theApp.m_tsLoginID);
			sPermission.MakeLower();
			if(sPermission.Find("guest") != -1)			// 访客
				nRet = SYS_ACCOUNT_GUEST;
			else if(sPermission.Find("normal") != -1)	// 普通用户
				nRet = SYS_ACCOUNT_NORMAL;
			else if(sPermission.Find("quality") != -1)	// 品质分析帐号
				nRet = SYS_ACCOUNT_QUALITY;
			else if(sPermission.Find("admin") != -1)	// 管理员
				nRet = SYS_ACCOUNT_ADMIN;
			else if(sPermission.Find("supper") != -1)	// 系统调试员帐号
				nRet = SYS_ACCOUNT_SUPERUSER;
		}
	}

	if(prsRecSet != NULL)
	{
		delete prsRecSet;
		prsRecSet = NULL;
	}

	return nRet;
*/
}

int CSysDatabase::CheckHost(tstring tsHostName, tstring tsHostID)
{
	if (tsHostName.length() <= 0 || tsHostID.length() <= 0)
		return HOST_INVALID;

	CString sSQL, sAuthorizerID, sTem;
	long nLoginTimes, nLimitTimes;
	COleDateTime otExpiration;
	CTime tExpiration, tCurrent, tLastLogin;
	BOOL bUpdateRecordset = FALSE;
	_ConnectionPtr pConnection;
	_RecordsetPtr pRecordset;
	_variant_t vValue;
	int nRet = HOST_UNAUTHORIZED;

	tLastLogin = tCurrent = CTime::GetCurrentTime();
	nLoginTimes = 1;
	sAuthorizerID = _T("");

	if (!OpenAdoConnection())
		return HOST_UNVERIFIED;

	sSQL.Format(_T("SELECT * FROM tbl_Computer WHERE Host_ID=\'%s\'"), tsHostID.c_str());
	//AfxMessageBox(sSQL);

	try
	{
		// 建立记录集
		pRecordset.CreateInstance("ADODB.Recordset");
		pConnection = m_adoDatabaseConn.GetConnection();
		_variant_t activeCon = _variant_t((IDispatch*)pConnection, true);
		pRecordset->Open(_bstr_t(sSQL), &activeCon, adOpenStatic, adLockOptimistic, adCmdText);

		// 处理记录集
		if (!(pRecordset->adoEOF || pRecordset->BOF))
		{
			//sHostName = pRecordset->GetCollect("Host_Name").bstrVal;
			//sMac = pRecordset->GetCollect("MAC").bstrVal;
			nLoginTimes = pRecordset->GetCollect("Login_Times").intVal;
			nLimitTimes = pRecordset->GetCollect("Limit_Times").intVal;
			otExpiration = pRecordset->GetCollect("Expiration_Date").date;
			vValue = pRecordset->GetCollect("Authorizer_ID");
			if (vValue.vt != VT_NULL)
				sAuthorizerID = vValue.bstrVal;
			vValue = pRecordset->GetCollect("Last_Login_Date");

			nLoginTimes++;

			SYSTEMTIME sysTime;
			VariantTimeToSystemTime(otExpiration, &sysTime);
			tExpiration = CTime(sysTime);
			if (vValue.vt != VT_NULL)
			{
				VariantTimeToSystemTime(vValue.date, &sysTime);
				tLastLogin = CTime(sysTime);
			}
			else
				tLastLogin = tCurrent;

			if (sAuthorizerID.GetLength() > 0)
				nRet = HOST_AUTHORIZED;
			else
			{
				if (nLoginTimes >= nLimitTimes || tCurrent > tExpiration ||	// 过了试用次数或日期
					tCurrent < tLastLogin)									// 存在非法改系统日期
				{
					nLoginTimes = nLimitTimes;
					nRet = HOST_EXPIRED;
				}
				else
					nRet = HOST_UNAUTHORIZED;
			}

			bUpdateRecordset = TRUE;				// 更新主机记录
		}
		else	// 添加主机记录
		{
			bUpdateRecordset = FALSE;				// 新增主机记录
			pRecordset->AddNew();
			nRet = HOST_UNAUTHORIZED;
		}

		CString sLastLoginDate = tCurrent.Format(_T("%Y-%m-%d %H:%M:%S"));

		if (bUpdateRecordset)
		{
			pRecordset->Update(_T("Login_Times"), nLoginTimes);
			pRecordset->Update(_T("Last_Login_Date"), _bstr_t(sLastLoginDate));
		}
		else
		{
			pRecordset->PutCollect("Host_Name", _bstr_t(tsHostName.c_str()));
			pRecordset->PutCollect("Host_ID", _bstr_t(tsHostID.c_str()));
			pRecordset->PutCollect(_T("Login_Times"), nLoginTimes);
			pRecordset->PutCollect(_T("Last_Login_Date"), _bstr_t(sLastLoginDate));
			// 限定的试用次数、试用期限，用数据库中设置的默认值（设置默认值，是为了便于统一维护）
		}

		// 关闭记录集
		pRecordset->Update();			// 更新数据库
		pRecordset->Close();			// 关闭记录集
	}
	catch (_com_error &e)
	{
		CString sError, sSource((LPCSTR)e.Source()), sDescription((LPCSTR)e.Description());
		sError.Format(_T("处理记录集失败[err:%s]!\n\n数据源：%s\n出错原因：%s\n"),
			e.ErrorMessage(), sSource, sDescription);
		//AfxMessageBox(sError);
		TRACE(sError);
		return HOST_UNVERIFIED;			// 发生异常时，记录集已自动关闭，再次关闭则出错
	}

	return nRet;
}

// 只更新记录中的授权信息，该记录须此前已用CheckHost( )添加
int CSysDatabase::AuthorizeHost(tstring tsHostName, tstring tsHostID,
								tstring tsAuthorizerID, tstring tsAuthorizerPassword)
{
	if (tsHostName.length() <= 0 || tsHostID.length() <= 0 || tsAuthorizerID.length() <= 0)
		return HOST_INVALID;

	CString sSQL, sTem;
	CTime tCurrent;
	_ConnectionPtr pConnection;
	_RecordsetPtr pRecordset;
	int nRet;

	tCurrent = CTime::GetCurrentTime();

	if (!OpenAdoConnection())
		return HOST_UNVERIFIED;

	// ---------------------------------------------------------------------------
	// 验证Authorizer的身份
	sSQL.Format(_T("SELECT * FROM tbl_Admin WHERE ID=\'%s\' AND Password=\'%s\' AND On_Guard=true"),
					tsAuthorizerID.c_str(), tsAuthorizerPassword.c_str());
	//AfxMessageBox(sSQL);

	try
	{
		// 建立记录集
		pRecordset.CreateInstance("ADODB.Recordset");
		pConnection = m_adoDatabaseConn.GetConnection();
		_variant_t activeCon = _variant_t((IDispatch*)pConnection, true);
		pRecordset->Open(_bstr_t(sSQL), &activeCon, adOpenStatic, adLockOptimistic, adCmdText);

		// 处理记录集
		if (pRecordset->adoEOF || pRecordset->BOF)	// 不存在该Authorizer
		{
			pRecordset->Close();
			return HOST_UNAUTHORIZED;
		}
		pRecordset->Close();
	}
	catch (_com_error &e)
	{
		CString sError, sSource((LPCSTR)e.Source()), sDescription((LPCSTR)e.Description());
		sError.Format(_T("处理记录集失败[err:%s]!\n\n数据源：%s\n出错原因：%s\n"),
			e.ErrorMessage(), sSource, sDescription);
		//AfxMessageBox(sError);
		TRACE(sError);
		return HOST_UNVERIFIED;			// 发生异常时，记录集已自动关闭，再次关闭则出错
	}

	// ---------------------------------------------------------------------------
	// 给Host授权
	sSQL.Format(_T("SELECT * FROM tbl_Computer WHERE Host_ID=\'%s\'"), tsHostID.c_str());
	//AfxMessageBox(sSQL);

	try
	{
		// 建立记录集
		pRecordset.CreateInstance("ADODB.Recordset");
		pConnection = m_adoDatabaseConn.GetConnection();
		_variant_t activeCon = _variant_t((IDispatch*)pConnection, true);
		pRecordset->Open(_bstr_t(sSQL), &activeCon, adOpenStatic, adLockOptimistic, adCmdText);

		// 处理记录集
		if (!(pRecordset->adoEOF || pRecordset->BOF))
		{
			nRet = HOST_AUTHORIZED;
			sTem = tCurrent.Format(_T("%Y-%m-%d %H:%M:%S"));

			pRecordset->Update(_T("Authorizer_ID"), _bstr_t(tsAuthorizerID.c_str()));
			pRecordset->Update(_T("Authorization_Date"), _bstr_t(sTem));
		}
		else
			nRet = HOST_UNAUTHORIZED;

		// 关闭记录集
		pRecordset->Update();			// 更新数据库
		pRecordset->Close();			// 关闭记录集
	}
	catch (_com_error &e)
	{
		CString sError, sSource((LPCSTR)e.Source()), sDescription((LPCSTR)e.Description());
		sError.Format(_T("处理记录集失败[err:%s]!\n\n数据源：%s\n出错原因：%s\n"),
			e.ErrorMessage(), sSource, sDescription);
		//AfxMessageBox(sError);
		TRACE(sError);
		return HOST_UNVERIFIED;			// 发生异常时，记录集已自动关闭，再次关闭则出错
	}

	return nRet;
}
