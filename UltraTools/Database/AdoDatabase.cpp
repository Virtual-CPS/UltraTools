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
	CloseConnect();	// �ر�����
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
ͨ��Connection�����Open�����������������ݿ⣬��ԭ��
HRESULT Connection15::Open(_bstr_t ConnectionString, _bstr_t UserID,
							_bstr_t Password, long Options )
ConnectionStringΪ�����ִ�,UserID���û���, Password�ǵ�½����,
Options������ѡ��,����ָ��Connection��������ݵĸ������Ȩ,���������¼�������:
	adModeUnknown:ȱʡ����ǰ�����Ȩδ����
	adModeRead:ֻ��
	adModeWrite:ֻд
	adModeReadWrite:���Զ�д
	adModeShareDenyRead:��ֹ����Connection�����Զ�Ȩ�޴�����
	adModeShareDenyWrite:��ֹ����Connection������дȨ�޴�����
	adModeShareExclusive:��ֹ����Connection�����Զ�дȨ�޴�����
	adModeShareDenyNone:��������Connection�������κ�Ȩ�޴�����
*/
BOOL CBFAdoConnection::OpenConnection(TCHAR *sConnString, BOOL bReOpen /*=FALSE*/)
{
	if(!bReOpen)	// �������´�
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
		MessageBox(NULL,_T("�������ݿ�ʧ�ܣ�������������ݿ������Ƿ���ȷ"), _T("����ʧ��"), MB_OK|MB_ICONINFORMATION);
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

/* ��Recordset������в�ѯȡ�ü�¼��
Open������ԭ��:
HRESULT Recordset15::Open(const _variant_t &Source, const _variant_t &ActiveConnection,
			enum CursorTypeEnum CursorType, enum LockTypeEnum LockType, long Options) 
���У�
��Source�����ݲ�ѯ�ַ���
��ActiveConnection���Ѿ������õ����ӣ�������Ҫ��Connection����ָ��������һ��_variant_t����) 
��CursorType������ͣ�������������ֵ֮һ:
	adOpenUnspecified = -1,	// �����ر�ָ��
	adOpenForwardOnly = 0,	// ǰ����̬��ꡣ���ֹ��ֻ����ǰ�����¼����������MoveNext��ǰ����,���ַ�ʽ�����������ٶȡ�������BookMark,RecordCount,AbsolutePosition,AbsolutePage������ʹ��
	adOpenKeyset = 1,		// �������ֹ��ļ�¼�������������û���������ɾ�������������ڸ���ԭ�м�¼�Ĳ��������ǿɼ��ġ�
	adOpenDynamic = 2,		// ��̬��ꡣ�������ݿ�Ĳ������������ڸ��û���¼���Ϸ�Ӧ������
	adOpenStatic = 3		// ��̬��ꡣ��Ϊ��ļ�¼������һ����̬���ݣ��������û���������ɾ�������²�������ļ�¼����˵�ǲ��ɼ��ġ�
��LockType�������ͣ�������������ֵ֮һ��
	adLockUnspecified = -1,	// δָ��
	adLockReadOnly = 1,		// ֻ����¼��
	adLockPessimistic = 2,	// ����������ʽ�������ڸ���ʱ�����������ж����������ȫ����������
	adLockOptimistic = 3,	// �ֹ�������ʽ��ֻ���������Update����ʱ��������¼���ڴ�֮ǰ��Ȼ���������ݵĸ��¡����롢ɾ���ȶ���
	adLockBatchOptimistic = 4���ֹ۷������¡��༭ʱ��¼�������������ġ����뼰ɾ������������ģʽ����ɡ�
��Options��ο���Connection�����Execute�����Ľ���
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
			//�����������ݿ�
			else
			{
				//
				//if(!OpenConnection(m_szConnString))
				//	return FALSE;
				//MessageBox(NULL,"���ݿ������Ѿ��Ͽ������������ӣ�","��������",MB_OK|MB_ICONINFORMATION);		
			}

			return FALSE;
		}
		//rs=new _RecordsetPtr;
		HRESULT hr;
//		AfxMessageBox("Recordset: createInstance..");
		hr=rs->CreateInstance(__uuidof(Recordset));
		if(FAILED(hr))
		{
			return FALSE;						// ����ʵ��ʧ��
		}
//		AfxMessageBox("Recordset: OpenRecord...");
		hr=(*rs)->Open(sSQL,m_pConn->GetInterfacePtr(),
						adOpenStatic, adLockBatchOptimistic, -1);
//		AfxMessageBox("Recordset: after OpenRecord...");
		if(FAILED(hr))
		{
			return FALSE;						// ������ʧ��
		}
		return TRUE;							// �ɹ�����
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
		MessageBox(NULL, _T("���ݿ��¼��ʧ�ܣ�"), _T("��¼ʧ��"), MB_OK|MB_ICONINFORMATION);
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
		AfxMessageBox(_T("�ر����ݿ�����δ֪����"));
	}
}

// ��ʼ������,�������κμ�¼��������Ϊ����SQL����
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

// ִ��SQL�����������ؼ�¼��
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

//�����Ƿ�������״̬
BOOL CBFAdoConnection::IsConnectClosed()
{
	return (m_pConn==NULL)||((*m_pConn)->State==adStateClosed);
}

//�������ӳ�ʱ
int CBFAdoConnection::SetConnTimeOut(long lTimeOut)
{
	return (*m_pConn)->put_ConnectionTimeout(lTimeOut);
}

int CBFAdoConnection::SetCommTimeOut(long lTimeOut)
{
	return (*m_pConn)->put_CommandTimeout(lTimeOut);
}

//�������
void CBFAdoConnection::ReportError(int nERRORfrom)
{
#ifndef _DEBUG
	return;
#endif

	switch(nERRORfrom)
	{
	case ErrFormOpenConnsction:
		#if  1 //def _DEBUG //����ʱ��ʾ��Ӧ�Ĵ�����Ϣ
			try
			{
				for(long l=0;l<(*m_pConn)->Errors->Count;l++)
				{
					ErrorPtr pErr;
					pErr=(*m_pConn)->Errors->GetItem(l);
					CString str;
					str=(char*)pErr->Description;					
					MessageBox(NULL, str, _T("����ʧ��"), MB_OK|MB_ICONINFORMATION);
				}
			}
			catch(...)
			{
				MessageBox(NULL, _T("���ݿ�����δ֪�����޷���׽���������Ϣ!"), _T("����"), MB_ICONINFORMATION);
			}
		#else
				MessageBox(NULL, _T("��������ʧ�ܣ�������������ݿ������Ƿ���ȷ"), _T("����ʧ��"), MB_OK|MB_ICONINFORMATION);
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
					MessageBox(NULL, _T("���ݿ�����δ֪�����޷���׽���������Ϣ!"), _T("����"), MB_ICONINFORMATION);
				}
		#else
				MessageBox(NULL, _T("�����ݿ�ʧ��,�������磬�����������������ݿ⣡"), _T("��¼ʧ��"), MB_OK|MB_ICONINFORMATION);
		#endif
		break;
	case ErrFormCloseConnection:
		#if   1 //def _DEBUG //����ʱ��ʾ��Ӧ�Ĵ�����Ϣ
			try
			{
				for(long l=0;l<(*m_pConn)->Errors->Count;l++)
				{
					ErrorPtr pErr;
					pErr=(*m_pConn)->Errors->GetItem(l);
					CString str;
					str=(char*)pErr->Description;
					MessageBox(NULL, str, _T("����ʧ��"), MB_OK|MB_ICONINFORMATION);
				}
			}
			catch(...)
			{
				MessageBox(NULL, _T("���ݿ�����δ֪�����޷���׽���������Ϣ!"), _T("����"), MB_ICONINFORMATION);
			}

		#else
				;//MessageBox(NULL,"�ر����ݿ������쳣","�ر��쳣",MB_OK|MB_ICONINFORMATION);
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
				MessageBox(NULL, _T("���ݿ�����δ֪�����޷���׽���������Ϣ!"), _T("����"), MB_ICONINFORMATION);
			}
		#else
				MessageBox(NULL, _T("���ݿ�ִ������ʧ��,�������ݿ⡣"), _T("����ʧ��"), MB_OK|MB_ICONINFORMATION);
		#endif
		break;
	default:
		break;
	}
}

// ö�ٵ�ǰ�������ݿ��е����б���
// ��ȡ�Ľ����m_saTableNames�з���
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
			// ��ȡ���
			_bstr_t table_name = pSet->Fields->GetItem("TABLE_NAME")->Value;

			// ��ȡ�������
			_bstr_t table_type = pSet->Fields->GetItem("TABLE_TYPE")->Value;

			// ����һ�£�ֻ���������ƣ�������ʡ��
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
		errormessage.Format(_T("�������ݿ�ʧ��!\n������Ϣ:%s"), e.ErrorMessage());
		AfxMessageBox(errormessage, MB_OK|MB_ICONERROR);
	}
	catch(...)
	{
		AfxMessageBox(_T("��ȡ����ʱ����δ֪����!"), MB_OK|MB_ICONERROR);
	}

	return;
}

// ö��ָ�����е������ֶ�
// ��ȡ�Ľ����m_saFieldNames�з���
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
				hr = pSet->get_Fields(&fields);					// �õ���¼�����ֶμ���
				if(SUCCEEDED(hr))
					hr = fields->get_Count(&ColCount);			//�õ���¼�����ֶμ����е��ֶε��ܸ���
				for(i=0; i<ColCount; i++)
				{        
					hr = fields->Item[i]->get_Name(&bsColName);	// �õ���¼���е��ֶ���
					sColName = bsColName;
					m_saFieldNames.Add(sColName);
				}
				if(SUCCEEDED(hr))
					fields->Release();							// �ͷ�ָ��
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
		AfxMessageBox(_T("��ȡ�ֶ���ʱ����δ֪����!"), MB_OK|MB_ICONERROR);
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
		sErr.Format(_T("ADO�������ݿ����\n\tCode: %08lx\n\tCode meaning: %s\n\tSource: %s\n\tDescription: %s\n"),
			e.Error(), e.ErrorMessage(), (LPCSTR)bstrSource, (LPCSTR)bstrDescription);
		AfxMessageBox(sErr, MB_OK|MB_ICONERROR);
#endif
		return false;
	}
	catch(...)
	{
#ifdef _DEBUG
		AfxMessageBox(_T("ͳ��ʱ����δ֪����!"), MB_OK|MB_ICONERROR);
#endif
		return false;
	}

	// ��ͳ�ƽ��ת����CString����
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
			AfxMessageBox(_T("δ������ֶ��������ͣ��봦��!"), MB_OK|MB_ICONERROR);
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
			MessageBox(NULL,_T("�޷���ȡ���ݿ����ϣ��������ݿ������Ѿ��Ͽ������������ӡ�Ȼ�����ԡ�"),_T("��������"), MB_ICONINFORMATION);
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
		AfxMessageBox(_T("���ݿ��ֶη���δ֪�����������ݿ��Ƿ�Ķ���"), MB_ICONINFORMATION);
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
	
	//���������
	//VERIFY(sizeof(cVal)<1);
	memset(cVal,0,sizeof(cVal));
	try
	{
		if((*m_prsThis)==NULL||(*m_prsThis)->State==adStateClosed)
		{
#ifdef _DEBUG
			MessageBox(NULL, _T("�޷���ȡ���ݿ����ϣ��������ݿ������Ѿ��Ͽ������������ӡ�Ȼ�����ԡ�"),_T("��������"), MB_ICONINFORMATION);
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
		//AfxMessageBox("���ݿ��ֶη��ʴ���");
		return FALSE;
	}
	catch(...)
	{
#ifdef _DEBUG
		AfxMessageBox(_T("���ݿ��ֶη���δ֪�����������ݿ��Ƿ�Ķ���"), MB_ICONINFORMATION);
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
				{	// ȥ��������
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
				AfxMessageBox(_T("δ������ֶ��������ͣ��봦��"));
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
			MessageBox(NULL, _T("�޷���ȡ���ݿ����ϣ��������ݿ������Ѿ��Ͽ������������ӡ�Ȼ�����ԡ�"), _T("��������"), MB_ICONINFORMATION);
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
		AfxMessageBox(_T("���ݿ��ֶη���δ֪���������������ݿ�ṹ�Ƿ��Ѿ����ġ�"), MB_ICONINFORMATION);
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
		AfxMessageBox(_T("�ֶδ��������ʴ���������д�Ƿ���ȷ��"));
	}
#endif
	//MessageBox(NULL,"�ֶη��ʷ���������ȷ�����ݿ�ṹû�иĶ���","�ֶη���",MB_ICONINFORMATION);
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
		AfxMessageBox(_T("��ȡText�ֶ�δ֪����"));
#endif
		return FALSE;
	}

	return TRUE;
}