// CAdoDatabase.cpp
// AdoDatabase.cpp : implementation file
//

#include "StdAfx.h"
#include "AdoDatabase.h"

//=============================================================================
//                              CBFAdoConnection 
//=============================================================================

CBFAdoConnection::CBFAdoConnection()
{
	m_pConn=NULL;
}

_ConnectionPtr* CBFAdoConnection::GetConnection()
{
	return m_pConn;
}

CString CBFAdoConnection::GetConnString()
{
	return m_szConnString;
}

CBFAdoConnection::~CBFAdoConnection()
{
	CloseConnect();	// 关闭连接
}

BOOL CBFAdoConnection::OpenConnection(CString strConnString, BOOL bReOpen)
{
	TCHAR c[4096];
	ZeroMemory(c, sizeof(c));
	//_tcscpy(c,strConnString.GetBuffer(0));		// Busy Fish modified for .net2005 2006.06.12
	//strConnString.ReleaseBuffer();
	wsprintf(c, _T("%s"), strConnString);
	return OpenConnection(c,bReOpen);
}

/*
通过Connection对象的Open方法来进行连接数据库，其原型
HRESULT Connection15::Open(_bstr_t ConnectionString, _bstr_t UserID,
							_bstr_t Password, long Options )
ConnectionString为连接字串,UserID是用户名, Password是登陆密码,
Options是连接选项,用于指定Connection对象对数据的更新许可权,可以是如下几个常量:
	adModeUnknown:缺省，当前的许可权未设置
	adModeRead:只读
	adModeWrite:只写
	adModeReadWrite:可以读写
	adModeShareDenyRead:阻止其它Connection对象以读权限打开连接
	adModeShareDenyWrite:阻止其它Connection对象以写权限打开连接
	adModeShareExclusive:阻止其它Connection对象以读写权限打开连接
	adModeShareDenyNone:允许其它Connection对象以任何权限打开连接
*/
BOOL CBFAdoConnection::OpenConnection(TCHAR *sConnString, BOOL bReOpen /*=FALSE*/)
{
	if(!bReOpen)	// 不需重新打开
	{
		if(m_pConn !=NULL && ((*m_pConn)->State!=adStateClosed))
			return TRUE;
	}

	VERIFY(sConnString);
	//_tcscpy(m_szConnString,sConnString);				// Busy Fish modified for .net2005 2006.06.12
	wsprintf(m_szConnString, _T("%s"), sConnString);

	try
	{
		m_pConn = new _ConnectionPtr;
		m_pConn->CreateInstance(__uuidof(Connection));
		if(m_pConn == NULL)
			return FALSE;
		SetConnTimeOut(10);
		HRESULT hr=(*m_pConn)->Open(sConnString, "", "", adModeUnknown);		
		if(FAILED(hr))
		{
			delete m_pConn;
			m_pConn = NULL;
			return FALSE;
		}
		return TRUE;
	}
	catch(_com_error)
	{
		ReportError(ErrFormOpenConnsction);
		delete m_pConn;
		m_pConn = NULL;
		return FALSE;
	}
	catch(...)
	{
#ifdef _DEBUG
		MessageBox(NULL,_T("连接数据库失败，请检查网络和数据库设置是否正确"), _T("连接失败"), MB_OK|MB_ICONINFORMATION);
#endif
		delete m_pConn;
		m_pConn = NULL;

		return FALSE;
	}	
//	SetConnTimeOut(10);
}

BOOL CBFAdoConnection::OpenRecordset(CString strSQL, _RecordsetPtr *rs, CString sConnString)
{
	TCHAR c[4096];
	ZeroMemory(c, sizeof(c));
	//_tcscpy(c,strSQL.GetBuffer(0));			// Busy Fish modified for .net2005 2006.06.12
	//strSQL.ReleaseBuffer();
	_stprintf(c, _T("%s"), strSQL);
	return OpenRecordset(c, rs, sConnString);
}

/* 用Recordset对象进行查询取得记录集
Open方法的原型:
HRESULT Recordset15::Open(const _variant_t &Source, const _variant_t &ActiveConnection,
			enum CursorTypeEnum CursorType, enum LockTypeEnum LockType, long Options) 
其中：
①Source是数据查询字符串
②ActiveConnection是已经建立好的连接（我们需要用Connection对象指针来构造一个_variant_t对象) 
③CursorType光标类型，它可以是以下值之一:
	adOpenUnspecified = -1,	// 不作特别指定
	adOpenForwardOnly = 0,	// 前滚静态光标。这种光标只能向前浏览记录集，比如用MoveNext向前滚动,这种方式可以提高浏览速度。但诸如BookMark,RecordCount,AbsolutePosition,AbsolutePage都不能使用
	adOpenKeyset = 1,		// 采用这种光标的记录集看不到其它用户的新增、删除操作，但对于更新原有记录的操作对你是可见的。
	adOpenDynamic = 2,		// 动态光标。所有数据库的操作都会立即在各用户记录集上反应出来。
	adOpenStatic = 3		// 静态光标。它为你的记录集产生一个静态备份，但其它用户的新增、删除、更新操作对你的记录集来说是不可见的。
④LockType锁定类型，它可以是以下值之一：
	adLockUnspecified = -1,	// 未指定
	adLockReadOnly = 1,		// 只读记录集
	adLockPessimistic = 2,	// 悲观锁定方式。数据在更新时锁定其它所有动作，这是最安全的锁定机制
	adLockOptimistic = 3,	// 乐观锁定方式。只有在你调用Update方法时才锁定记录。在此之前仍然可以做数据的更新、插入、删除等动作
	adLockBatchOptimistic = 4，乐观分批更新。编辑时记录不会锁定，更改、插入及删除是在批处理模式下完成。
⑤Options请参考对Connection对象的Execute方法的介绍
*/
BOOL CBFAdoConnection::OpenRecordset(const TCHAR *sSQL, _RecordsetPtr *rs, TCHAR *sConnString)
{
	//rs=new _RecordsetPtr;
	if(NULL == rs)
		return false;
	VERIFY(sSQL);

	try
	{
		if((m_pConn==NULL)||((*m_pConn)->State==adStateClosed))
		{
			//int n=(*m_pConn)->State;
			if(sConnString !=NULL)
			{
				if(!OpenConnection(sConnString))
					return FALSE;
			}
			//尝试连接数据库
			else
			{
				//
				//if(!OpenConnection(m_szConnString))
				//	return FALSE;
				//MessageBox(NULL,"数据库连接已经断开，请重新连接！","连接问题",MB_OK|MB_ICONINFORMATION);		
			}

			return FALSE;
		}
		//rs=new _RecordsetPtr;
		HRESULT hr;
//		AfxMessageBox("Recordset: createInstance..");
		hr=rs->CreateInstance(__uuidof(Recordset));
		if(FAILED(hr))
		{
			return FALSE;						// 建立实例失败
		}
//		AfxMessageBox("Recordset: OpenRecord...");
		hr=(*rs)->Open(sSQL,m_pConn->GetInterfacePtr(),
						adOpenStatic, adLockBatchOptimistic, -1);
//		AfxMessageBox("Recordset: after OpenRecord...");
		if(FAILED(hr))
		{
			return FALSE;						// 打开连接失败
		}
		return TRUE;							// 成功返回
	}
	catch(_com_error)
	{
		//AfxMessageBox(ce->Description());
		ReportError(ErrFromOpenRecordset);
		return FALSE;
	}
	catch(...)
	{
#ifdef _DEBUG
		MessageBox(NULL, _T("数据库记录打开失败！"), _T("记录失败"), MB_OK|MB_ICONINFORMATION);
#endif
		return FALSE;
	}
	return TRUE;
}

void CBFAdoConnection::CloseConnect()
{
	try
	{
		if(m_pConn != NULL)
		{
			if((*m_pConn)->State!=adStateClosed)
				(*m_pConn)->Close();
			delete m_pConn;
			m_pConn = NULL;
		}
	}
	catch(_com_error)
	{
		ReportError(ErrFormCloseConnection);
	}
	catch(...)
	{
		AfxMessageBox(_T("关闭数据库连接未知错误！"));
	}
}

// 开始事务处理,不返回任何记录集，参数为事务SQL数组
BOOL CBFAdoConnection::ExecuteTrans(CStringArray arrStrSQL)
{
	(*m_pConn)->BeginTrans();

	try
	{
		_RecordsetPtr* prsThis;
		for(int i=0;i<arrStrSQL.GetSize();i++)
		{
			prsThis=new _RecordsetPtr;
			OpenRecordset(arrStrSQL.ElementAt(i),prsThis);
			delete prsThis;
		}
		prsThis=NULL;
		(*m_pConn)->CommitTrans();
		return TRUE;
	}
	catch(_com_error)
	{
		(*m_pConn)->RollbackTrans();
		ReportError(ErrFormTanslation);	
		return FALSE;
	}
}

// 执行SQL操作，不返回记录集
int CBFAdoConnection::ExecuteSQL(LPCSTR szSQL)
{
	//VARIANT vEffect;
	//_variant_t RecordsAffected;
	try
	{
		(*m_pConn)->Execute(szSQL,NULL,adCmdText|adExecuteNoRecords);
		//(*m_pConn)->Execute(szSQL,&RecordsAffected,adCmdText);
		return TRUE;
	}
	catch(_com_error)
	{
		ReportError(ErrFormTanslation);				
		return FALSE;
	}	
	//return vEffect.lVal;
}

//返回是否处在连接状态
BOOL CBFAdoConnection::IsConnectClosed()
{
	return (m_pConn==NULL)||((*m_pConn)->State==adStateClosed);
}

//设置连接超时
int CBFAdoConnection::SetConnTimeOut(long lTimeOut)
{
	return (*m_pConn)->put_ConnectionTimeout(lTimeOut);
}

int CBFAdoConnection::SetCommTimeOut(long lTimeOut)
{
	return (*m_pConn)->put_CommandTimeout(lTimeOut);
}

//报告错误
void CBFAdoConnection::ReportError(int nERRORfrom)
{
#ifndef _DEBUG
	return;
#endif

	switch(nERRORfrom)
	{
	case ErrFormOpenConnsction:
		#if  1 //def _DEBUG //调试时显示相应的错误信息
			try
			{
				for(long l=0;l<(*m_pConn)->Errors->Count;l++)
				{
					ErrorPtr pErr;
					pErr=(*m_pConn)->Errors->GetItem(l);
					CString str;
					str=(char*)pErr->Description;					
					MessageBox(NULL, str, _T("连接失败"), MB_OK|MB_ICONINFORMATION);
				}
			}
			catch(...)
			{
				MessageBox(NULL, _T("数据库连接未知错误，无法捕捉具体错误信息!"), _T("错误"), MB_ICONINFORMATION);
			}
		#else
				MessageBox(NULL, _T("连接数据失败，请检查网络和数据库设置是否正确"), _T("连接失败"), MB_OK|MB_ICONINFORMATION);
		#endif	
		break;
	case ErrFromOpenRecordset:
		#if   1 //def _DEBUG
				try
				{
					for(long i=0;i<(*m_pConn)->Errors->Count;i++)
					{
						ErrorPtr pErr;
						pErr=(*m_pConn)->Errors->GetItem(i);
						AfxMessageBox(pErr->Description);
					}
				}
				catch(...)
				{
					MessageBox(NULL, _T("数据库连接未知错误，无法捕捉具体错误信息!"), _T("错误"), MB_ICONINFORMATION);
				}
		#else
				MessageBox(NULL, _T("打开数据库失败,请检查网络，并尝试重新连接数据库！"), _T("记录失败"), MB_OK|MB_ICONINFORMATION);
		#endif
		break;
	case ErrFormCloseConnection:
		#if   1 //def _DEBUG //调试时显示相应的错误信息
			try
			{
				for(long l=0;l<(*m_pConn)->Errors->Count;l++)
				{
					ErrorPtr pErr;
					pErr=(*m_pConn)->Errors->GetItem(l);
					CString str;
					str=(char*)pErr->Description;
					MessageBox(NULL, str, _T("连接失败"), MB_OK|MB_ICONINFORMATION);
				}
			}
			catch(...)
			{
				MessageBox(NULL, _T("数据库连接未知错误，无法捕捉具体错误信息!"), _T("错误"), MB_ICONINFORMATION);
			}

		#else
				;//MessageBox(NULL,"关闭数据库连接异常","关闭异常",MB_OK|MB_ICONINFORMATION);
		#endif
		break;
	case ErrFormTanslation:
		#if 1  ////def _DEBUG
			try
			{
				for(long i=0;i<(*m_pConn)->Errors->Count;i++)
				{
					ErrorPtr pErr;
					pErr=(*m_pConn)->Errors->GetItem(i);
					AfxMessageBox(pErr->Description);
				}
			}
			catch(...)
			{
				MessageBox(NULL, _T("数据库连接未知错误，无法捕捉具体错误信息!"), _T("错误"), MB_ICONINFORMATION);
			}
		#else
				MessageBox(NULL, _T("数据库执行任务失败,请检查数据库。"), _T("任务失败"), MB_OK|MB_ICONINFORMATION);
		#endif
		break;
	default:
		break;
	}
}

// 枚举当前连接数据库中的所有表名
// 获取的结果在m_saTableNames中返回
void CBFAdoConnection::GetTableNames()
{
	_RecordsetPtr pSet;
	CString tmp;

	m_saTableNames.RemoveAll();

	if(IsConnectClosed())
		return;

	try
	{
		pSet = (*m_pConn)->OpenSchema(adSchemaTables);
		while(!(pSet->adoEOF))
		{
			// 获取表格
			_bstr_t table_name = pSet->Fields->GetItem("TABLE_NAME")->Value;

			// 获取表格类型
			_bstr_t table_type = pSet->Fields->GetItem("TABLE_TYPE")->Value;

			// 过滤一下，只输出表格名称，其他的省略
			if(strcmp(((LPCSTR)table_type),"TABLE")==0)
			{
				tmp.Format(_T("%s"), (LPCSTR)table_name);
				m_saTableNames.Add(tmp);
			}
			pSet->MoveNext();
		}   
		pSet->Close();
	}
	catch(_com_error e)
	{  
		CString errormessage;  
		errormessage.Format(_T("连接数据库失败!\n错误信息:%s"), e.ErrorMessage());
		AfxMessageBox(errormessage, MB_OK|MB_ICONERROR);
	}
	catch(...)
	{
		AfxMessageBox(_T("提取表名时发生未知错误!"), MB_OK|MB_ICONERROR);
	}

	return;
}

// 枚举指定表中的所有字段
// 获取的结果在m_saFieldNames中返回
void CBFAdoConnection::GetTableFields(CString sTableName)
{
	//Field *   field = NULL;		
	Fields *  fields = NULL;
	HRESULT   hr;
	_RecordsetPtr pSet;
	CString sColName;
	BSTR bsColName;
	long ColCount=0, i;

	_variant_t vNULL;
	vNULL.vt = VT_ERROR;
	vNULL.scode = DISP_E_PARAMNOTFOUND;

	m_saFieldNames.RemoveAll();

	if(IsConnectClosed())
		return;
	if(sTableName.IsEmpty())
		return;

	try
	{
		hr = pSet.CreateInstance(__uuidof(Recordset));
		if(SUCCEEDED(hr))
		{
			CString sQuery;
			sQuery.Format(_T("SELECT * FROM %s"), sTableName);
			pSet->PutRefActiveConnection((*m_pConn));
			//hr = pSet->Open(_variant_t(sQuery),"",adOpenDynamic,adLockOptimistic,adCmdText);
			hr = pSet->Open(_variant_t(sQuery),vNULL,adOpenDynamic,adLockOptimistic,adCmdText);
			if(SUCCEEDED(hr))
			{
				hr = pSet->get_Fields(&fields);					// 得到记录集的字段集合
				if(SUCCEEDED(hr))
					hr = fields->get_Count(&ColCount);			//得到记录集的字段集合中的字段的总个数
				for(i=0; i<ColCount; i++)
				{        
					hr = fields->Item[i]->get_Name(&bsColName);	// 得到记录集中的字段名
					sColName = bsColName;
					m_saFieldNames.Add(sColName);
				}
				if(SUCCEEDED(hr))
					fields->Release();							// 释放指针
			}
		}
		pSet->Close();
	}
	catch(_com_error e)
	{
		CString sErr;
		sErr.Format(_T("Code: %08lx\nCode meaning: %s\n"), e.Error(), e.ErrorMessage());
		AfxMessageBox(sErr, MB_OK|MB_ICONERROR);
	}
	catch(...)
	{
		AfxMessageBox(_T("提取字段名时发生未知错误!"), MB_OK|MB_ICONERROR);
	}
}

//inline BOOL CBFAdoConnection::Statistics(CString SqlCommand, CString& sResult)
BOOL CBFAdoConnection::Statistics(CString SqlCommand, TCHAR* szResult)
{
	//sResult.Empty();
	szResult[0] = '\0';

	if(IsConnectClosed())
	{
		return false;
	}

	_variant_t vNULL;
	_variant_t value;
	_RecordsetPtr pRecordset;

	try
	{
		pRecordset = (*m_pConn)->Execute((_bstr_t) SqlCommand, &vNULL, adCmdText);
		value = pRecordset->GetCollect((_variant_t) (long)0);
		pRecordset->Close();
		pRecordset.Release();
	}
	catch(_com_error e)
	{
#ifdef _DEBUG
		CString sErr;
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());
		sErr.Format(_T("ADO访问数据库错误\n\tCode: %08lx\n\tCode meaning: %s\n\tSource: %s\n\tDescription: %s\n"),
			e.Error(), e.ErrorMessage(), (LPCSTR)bstrSource, (LPCSTR)bstrDescription);
		AfxMessageBox(sErr, MB_OK|MB_ICONERROR);
#endif
		return false;
	}
	catch(...)
	{
#ifdef _DEBUG
		AfxMessageBox(_T("统计时发生未知错误!"), MB_OK|MB_ICONERROR);
#endif
		return false;
	}

	// 将统计结果转换成CString类型
	char * c=NULL;
	DATE dt;
	CString str;
	COleDateTime da;

	if(value.vt == VT_NULL)
		return false;
	switch(value.vt)
	{
		case VT_BSTR:
			c = (char *)_com_util::ConvertBSTRToString( V_BSTR(&value) );	
			if(c != NULL)
			{
				//str.Format("%s",c);
				//sResult = str;
				//_tcscpy(szResult, c);		// Busy Fish modified for .net2005 2006.06.11
				wsprintf(szResult, _T("%s"), c);
				delete c;
				c = NULL;
			}
			break;
		case VT_I2:
		case VT_I4:	
		case VT_DECIMAL:
		case VT_UI2:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
		case VT_INT:
		case VT_UINT:
			long lVal;
			lVal = long(value);
			//c = new char[64];
			//memset(c,0,sizeof(char)*64);
			//ltoa(lVal,c,10);
			//sResult.Format(c);
			str.Format(_T("%d"), lVal);
			//sResult = str;
			_ltot(lVal, szResult, 10);
			break;
		case VT_DATE:
			dt = value.date;
			da = COleDateTime(dt);
			//sResult = da.Format("%Y-%m-%d %H:%M:%S");
			str = da.Format(_T("%Y-%m-%d %H:%M:%S"));
			wsprintf(szResult, _T("%s"), str);
			break;
		default:
#ifdef _DEBUG
			AfxMessageBox(_T("未处理的字段数据类型，请处理!"), MB_OK|MB_ICONERROR);
#else
			break;
#endif
	}

	return true;
}

//=============================================================================
//                               CRecordsetValus
//=============================================================================

CBFRstValues::CBFRstValues()
{
}

CBFRstValues::CBFRstValues(_ConnectionPtr* pConn,_RecordsetPtr* pRs)
{
	ASSERT(pConn);
	ASSERT(pRs);
	m_prsThis=pRs;
	m_pConn=pConn;
}

CBFRstValues::CBFRstValues(CBFAdoConnection* pBFadoConn,_RecordsetPtr* pRs)
{
	CBFRstValues(pBFadoConn->GetConnection(),pRs);
}

CBFRstValues::~CBFRstValues()
{
}

void CBFRstValues::InitConnectAndRst(_ConnectionPtr *pConn, _RecordsetPtr *pRs)
{
	ASSERT(pConn);
	ASSERT(pRs);
	m_prsThis = pRs;
	m_pConn = pConn;
}

void CBFRstValues::InitConnectAndRst(CBFAdoConnection *pBFAdoConn, _RecordsetPtr *pRs)
{
	InitConnectAndRst(pBFAdoConn->GetConnection(),pRs);
}

BOOL CBFRstValues::GetValueLong(long *lVal, _variant_t &vIndex)
{
	_variant_t value;
	try
	{
		if((*m_prsThis)==NULL||(*m_prsThis)->State==adStateClosed)		
		{
#ifdef _DEBUG
			MessageBox(NULL,_T("无法读取数据库资料，可能数据库连接已经断开，请重新连接、然后重试。"),_T("连接问题"), MB_ICONINFORMATION);
#endif
			return FALSE;
		}

		if((*m_prsThis)->adoEOF)
		{
			lVal=0;
			return FALSE;
		}

		value=(*m_prsThis)->GetCollect(vIndex);		
	}
	catch(_com_error)
	{
		ReportError();
		return FALSE;
	}
	catch(...)
	{
#ifdef _DEBUG
		AfxMessageBox(_T("数据库字段访问未知错误！请检查数据库是否改动。"), MB_ICONINFORMATION);
#endif
		return FALSE;
	}	

	if(VerifyVTData(value))		
	{
		*lVal = long(value);
	}
	else
	{
		*lVal=0;
	}

	return TRUE;
}

BOOL CBFRstValues::GetValueStr(TCHAR *cVal, _variant_t &vIndex, int nFieldLen)
{
	TCHAR * c=NULL;
	_variant_t value;
	DATE dt;
	CString str;
	COleDateTime da;
	
	//检查是数组
	//VERIFY(sizeof(cVal)<1);
	memset(cVal,0,sizeof(cVal));
	try
	{
		if((*m_prsThis)==NULL||(*m_prsThis)->State==adStateClosed)
		{
#ifdef _DEBUG
			MessageBox(NULL, _T("无法读取数据库资料，可能数据库连接已经断开，请重新连接、然后重试。"),_T("连接问题"), MB_ICONINFORMATION);
#endif
			return FALSE;
		}	

		if((*m_prsThis)->adoEOF)
		{
			cVal[0]='\0';
			return FALSE;
		}
		value = (*m_prsThis)->GetCollect(vIndex);
	}
	catch(_com_error)
	{
		ReportError();
		//AfxMessageBox("数据库字段访问错误！");
		return FALSE;
	}
	catch(...)
	{
#ifdef _DEBUG
		AfxMessageBox(_T("数据库字段访问未知错误！请检查数据库是否改动。"), MB_ICONINFORMATION);
#endif
		return FALSE;
	}	
	
	if(VerifyVTData(value))
	{
		switch(value.vt)
		{
			case VT_BSTR:
				c = (TCHAR *)_com_util::ConvertBSTRToString( V_BSTR(&value) );	
				if(VerifyVTData(c))
				{											
					if(nFieldLen<=0)
					{
						_tcscpy(cVal, c);
					}
					else
					{
						_tcsnccpy(cVal, c, nFieldLen);
					}				
				}	
				delete c;
				c=NULL;
				break;
			case VT_I2:
			case VT_I4:	
			case VT_UI2:
			case VT_UI4:
			case VT_I8:
			case VT_UI8:
			case VT_INT:
			case VT_UINT:
				long lVal;
				lVal = long(value);
				_ltot(lVal,cVal,10);
				break;
			case VT_DATE:
				dt=value.date;
				da=COleDateTime(dt);
				str=da.Format(_T("%Y-%m-%d %H:%M:%S"));
				//str=da.Format("%Y-%m-%d ");
				c = new TCHAR[64];
				memset(c, 0, sizeof(TCHAR)*64);
				_tcscpy(c, str.GetBuffer(0));
				str.ReleaseBuffer();
				if(VerifyVTData(c))
				{
					if(nFieldLen<=0)
					{
						_tcscpy(cVal, c);
					}
					else
					{
						_tcsnccpy(cVal, c, nFieldLen);
						cVal[nFieldLen]='\0';
					}
				}
				delete c;
				c = NULL;
				break;
			case VT_DECIMAL:
				float fVal;
				fVal = float(value);
				str.Format(_T("%f"), fVal);
				int nLen;
				nLen = str.GetLength();
				while(nLen >0)
				{	// 去掉后续零
					if(str.Mid(nLen-1) == _T("0"))
					{
						str = str.Mid(0, nLen-1);
						nLen = str.GetLength();
					}
					else if(str.Mid(nLen-1) == _T("."))
					{
						str = str.Mid(0, nLen-1);
						break;
					}
					else
						break;
				}
				c = new TCHAR[14];
				memset(c, 0, sizeof(TCHAR)*14);
				_tcscpy(c,str.GetBuffer(0));
				str.ReleaseBuffer();
				if(VerifyVTData(c))
				{
					if(nFieldLen<=0)
					{
						_tcscpy(cVal, c);
					}
					else
					{
						_tcsnccpy(cVal, c, nFieldLen);
						cVal[nFieldLen]='\0';
					}
				}
				delete c;
				c = NULL;
				break;
			default:
#ifdef _DEBUG
				AfxMessageBox(_T("未处理的字段数据类型，请处理！"));
#endif
				break;
		}
	}

	else
	{
		cVal[0]='\0';
	}
	return TRUE;
}

BOOL CBFRstValues::GetValueLong(long *lVal, long lIndex)
{
	_variant_t vIndex(lIndex);
	return GetValueLong(lVal,vIndex);
}

BOOL CBFRstValues::GetValueLong(long *lVal, LPCSTR lpszIndex)
{
	_variant_t vIndex(lpszIndex);
	return GetValueLong(lVal,vIndex);
}

BOOL CBFRstValues::VerifyVTData(TCHAR *pData)
{
	if(pData == NULL)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CBFRstValues::VerifyVTData(_variant_t &value)
{
	if(value.vt == VT_NULL)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CBFRstValues::GetValueStr(CString& str,LPCSTR lpszFieldName,int nFieldLen/*=-1*/)
{
	_variant_t vIndex(lpszFieldName);
	return GetValueStr(str,vIndex,nFieldLen);
}

BOOL CBFRstValues::GetValueStr(CString& str,UINT nFieldIndex,int nFieldLen/*=-1*/)
{
	_variant_t vIndex((long)nFieldIndex);		
	return GetValueStr(str,vIndex,nFieldLen);
	
}

BOOL CBFRstValues::GetValueStr(CString& str,_variant_t &vIndex,int nFieldLen/*=-1*/)
{
	TCHAR buffer[1024];
	if(nFieldLen > 1023) 
		nFieldLen = 1023;
	
	BOOL bResult = GetValueStr(buffer, vIndex, nFieldLen);
	str.Format(buffer);
	str.TrimRight();
	return bResult;
}

BOOL CBFRstValues::GetValueFloat(float* fVal,_variant_t &vIndex)
{
	_variant_t value;	
	try
	{
		if((*m_prsThis)==NULL||(*m_prsThis)->State==adStateClosed)
		{
#ifdef _DEBUG
			MessageBox(NULL, _T("无法读取数据库资料，可能数据库连接已经断开，请重新连接、然后重试。"), _T("连接问题"), MB_ICONINFORMATION);
#endif
			return FALSE;
		}
		
		if((*m_prsThis)->adoEOF)
		{
			fVal=0;
			return FALSE;
		}

		value=(*m_prsThis)->GetCollect(vIndex);			
	}
	catch(_com_error)
	{		
		ReportError();
		return FALSE;
	}
	catch(...)
	{
#ifdef _DEBUG
		AfxMessageBox(_T("数据库字段访问未知错误！请检查连接数据库结构是否已经更改。"), MB_ICONINFORMATION);
#endif
		return FALSE;
	}

	if(VerifyVTData(value))
	{
		*fVal = float(value);
	}
	else
	{
		*fVal = 0;
	}
	return TRUE;
}


BOOL CBFRstValues::GetValueFloat(float* fVal,long lIndex)
{
	_variant_t vIndex(lIndex);
	return GetValueFloat(fVal,vIndex);
}

BOOL CBFRstValues::GetValueFloat(float* fVal,CString  strIndex)
{
	_variant_t vIndex(strIndex);
	return GetValueFloat(fVal,vIndex);
}

BOOL CBFRstValues::GetValueStr(TCHAR *cVal, long lIndex, int nFieldLen)
{
	UNREFERENCED_PARAMETER(nFieldLen);
	_variant_t vIndex;
	vIndex=_variant_t(lIndex);
	return GetValueStr(cVal, vIndex);	
}


BOOL CBFRstValues::GetValueStr(TCHAR *cVal, CString strIndex, int nFieldLen)
{	
	UNREFERENCED_PARAMETER(nFieldLen);
	_variant_t vIndex;
	vIndex=_variant_t(strIndex);	
	return GetValueStr(cVal,vIndex);	
}


void CBFRstValues::ReportError()
{
#ifdef _DEBUG
	try
	{
		for(long l=0;l<(*m_pConn)->Errors->Count;l++)
		{
			ErrorPtr pErr;
			pErr=(*m_pConn)->Errors->GetItem(l);
			AfxMessageBox(pErr->Description);
		}		
	}
	catch(...)
	{
		AfxMessageBox(_T("字段错误对象访问错误，请检查书写是否正确。"));
	}
#endif
	//MessageBox(NULL,"字段访问发生错误，请确认数据库结构没有改动。","字段访问",MB_ICONINFORMATION);
}

BOOL CBFRstValues::GetValText(CString &strText, CString &strFieldName)
{
	_bstr_t varChunk;
	_bstr_t varNotes;
	long lngTotalsize,
       lngOffSet = 0,
       lngChunkSize = 100; 
	_variant_t vFieldName = _variant_t(strFieldName);
	lngTotalsize = (*m_prsThis)->Fields->Item[vFieldName]->ActualSize/2; 
	if (lngTotalsize <= 0)
		return FALSE;

	try
	{
		while (lngOffSet < lngTotalsize)
		{
			varChunk = (*m_prsThis)->Fields->Item[vFieldName]->GetChunk(lngChunkSize);
			varNotes = varNotes + varChunk;
			lngOffSet = lngOffSet + lngChunkSize;
		}
		strText=(char*)varNotes;
	}
	catch(_com_error)
	{
		ReportError();
		return FALSE;
	}
	catch(...)
	{

#ifdef _DEBUG
		AfxMessageBox(_T("获取Text字段未知错误"));
#endif
		return FALSE;
	}

	return TRUE;
}