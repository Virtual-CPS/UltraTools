#if !defined(ADO_DATABASE_H_)
#define ADO_DATABASE_H_

#include <afxdb.h>

// AdoDatabase.h : header file
//
//
// ADO�������ݿ���
// �����������ҪMSADO15.DLL
//
// ˵������װ��ADOʹ�õĲ���������ADO�������ݿ������
//		ʹ��VC�Ϸ����ʹ��ADO�������ݿ�
// ������ADO�������ݿ⡢�򿪼�¼����ִ��SQL���Լ�
//		������ADO��¼������ת����C++�ĳ����������ͣ�CString��char��long��float�ȣ�
//
// ʹ�ã�1.����һ��CBFAdoConnectionʵ����CBFAdoConnection m_adoConn;
//		 2.�������ӣ�m_adoConn.Open(strConnstring);
//					�����Ҫ�������������ӵĿ��ԣ�����bRepen���TRUE
//		 3.�򿪼�¼����
//
//			_RecordsetPtr* prsThis=NULL;
//			prsThis=new _RecordsetPtr;
//			CString strSQL;
//			strSQL="select * from Table_Name";//		
//			//�����¼����ʧ��
//			if(!(m_adoConn.OpenRecordset(strSQL,prsThis)))
//				return FALSE;
//		 4.������¼��ֵ����
//			ע����Ҫ�ò�������
//			CBFRstValues rsv(m_adoConn,prsThis);
//		
//		 5.��õ��ֶε�ֵ
//			rsv.GetValueLong(&m_nDeptID,1L);//��õ�һ���ֶε�ֵ
//			rsv.GetValueStr(m_strName,"ID");//��õ�һ���ֶ���ΪID��ֵ
//			������ͬ��.�����Ҫ���SQL Server��nText���͵��ֶ�ֵ����
//			GetValText(CString &strText, CString &strFieldName)����
//		 6.��¼���ļ�¼�ƶ�	(*prsThis)->MoveNext()�����ƺ���
//		 7.��¼������ʱ����Ҫ�ͷ�����Դ
//			1)�رռ�¼��
//			2)ɾ����¼��ָ��
//			e.g.	(*prsThis)->Close();
//					delete prsThis;
//					prsThis=NULL;
//			���������ڴ�й©
//
//		ע��
//        1.�������Ҫ��ʼ��COM����������Ӧ�����м���AfxInitOle������ʼ������,����ADO�ĵ��ý�ʧ��
//        2.�����Ҫ���ô洢����SQL����Ϊ��Exec��+�洢����������,��ִ��SQLͬ��
//
//
//

// msado15.dll������ڱ��ļ�����Ŀ¼�������Լ�ָ�������msado15.dllȫ·��
#if !defined(__AFXADO_H)
#import "..\..\bin\msado15.dll" no_namespace rename ("EOF", "adoEOF") \
		rename ("LockTypeEnum", "adoLockTypeEnum") \
		rename ("DataTypeEnum", "adoDataTypeEnum") \
		rename ("FieldAttributeEnum", "adoFieldAttributeEnum") \
		rename ("EditModeEnum", "adoEditModeEnum") \
		rename ("RecordStatusEnum", "adoRecordStatusEnum") \
		rename ("ParameterDirectionEnum", "adoParameterDirectionEnum")
#endif //#if !defined(__AFXADO_H)

class CBFAdoConnection
{
public:
	CBFAdoConnection();
	virtual ~CBFAdoConnection();
public:
	int SetConnTimeOut(long lTimeOut);		// �������ӳ�ʱ
	int SetCommTimeOut(long lTimeOut);		// ��������ִ�г�ʱ

	BOOL IsConnectClosed();					// �ж������Ƿ��Ѿ���
	int ExecuteSQL(LPCSTR szSQL);			// ��ִ��SQL��䣬�����ؼ�¼��

	// �����ݿ��¼��
	// ������
	//		strSQL		��¼����SQL���
	//		rs			���صļ�¼��_RecordsetPtr����	
	//		sConnString	���ݿ�������ַ���
	//					���ʹ�����ݿ������Ѿ��򿪣�����û��
	//					������ݿ������û�д򿪣�������һ�������ַ���,���ȴ����ݿ�����
	BOOL OpenRecordset(CString strSQL, _RecordsetPtr *rs, CString sConnString=_T(""));//�����ݿ��¼��	
	BOOL OpenRecordset(const TCHAR *sSQL, _RecordsetPtr* rs, TCHAR* sConnString=NULL);

	// �����ݿ�����
	// ������
	//		strConnString	�����ַ���
	//		sConnString		�����ַ���
	//		bReOpen			�Ƿ����´򿪣����ΪFALSE����
	//						�����ж����ݿ��Ƿ�����û�д���򿪣�
	//						����Ѿ��򿪣�����ִ���κβ���
	//						���ΪTRUE�������������´򿪡�
	BOOL OpenConnection(CString strConnString, BOOL bReOpen=FALSE);	
	BOOL OpenConnection(TCHAR* sConnString, BOOL bReOpen=FALSE);
	//static BOOL OpenConnectionThread(CBFAdoConnection* pBfAdoCon);	// ���������̣߳������������߳�

	void CloseConnect();					// �ر����ݿ�����	

	BOOL ExecuteTrans(CStringArray arrStrSQL);// ��ʼ������,�������κμ�¼��������Ϊ����SQL����


	_ConnectionPtr* GetConnection();		// �õ�_ConnectionPtrָ��
	CString GetConnString();				// �õ������ַ���

	void GetTableNames();					// ö�ٵ�ǰ�������ݿ��е����б���
	void GetTableFields(CString sTableName);// ö��ָ�����е������ֶ�

	// ͳ�ƣ�����SqlCommand����ʽ���£�
	// 1. ͳ�Ƽ�¼����SELECT COUNT(*) FROM TableName[ WHERE��]
	// 2. ͳ���ֶ��ܺͣ�SELECT SUM FieldName FROM TableName[ WHERE��]
	// 3. ͳ���ֶ�ƽ��ֵ��SELECT AVG FieldName FROM TableName[ WHERE��] 
	// 4. ͳ���ֶ����ֵ��SELECT MAX FieldName FROM TableName[ WHERE��]
	// 5. ͳ���ֶ���Сֵ��SELECT MIN FieldName FROM TableName[ WHERE��]
	//inline BOOL Statistics(CString SqlCommand, CString& sResult);       // ͳ��
	BOOL Statistics(CString SqlCommand, TCHAR* szResult);       // ͳ��

// Attribute:
public:
	CStringArray m_saTableNames;			// ����GetTableNames()��ȡ�Ľ��
	CStringArray m_saFieldNames;			// ����GetTableFields()��ȡ�Ľ��
private:
	enum ERRORFrom	{	
					ErrFormOpenConnsction,
					ErrFromOpenRecordset,
					ErrFormCloseConnection,
					ErrFormTanslation
					};
	_ConnectionPtr* m_pConn;
	TCHAR m_szConnString[512];

protected:
	void ReportError(int nERRORfrom);
};


class CBFRstValues  
{	
public:
	// ���ֹ�����ķ���
	// ����޲������죬�빹������InitConnectAndRst����
	// ���������ֹ����������InitConnectAndRst����	
	CBFRstValues();
	CBFRstValues(_ConnectionPtr* pConn, _RecordsetPtr* pRs);
	CBFRstValues(CBFAdoConnection* pBFadoConn, _RecordsetPtr* pRs);

	virtual ~CBFRstValues();
public:
	// ��ʼ�����Ӷ���ͼ�¼������
	void InitConnectAndRst(_ConnectionPtr* pConn, _RecordsetPtr* pRs);
	void InitConnectAndRst(CBFAdoConnection* pBFAdoConn, _RecordsetPtr * pRs);

	// GetValText����
	// �õ����ݿ�nText�ֶε�ֵ
	// ������
	//		strText			�������շ���ֵ���ֶ�ֵ��
	//		strFieldName	�ֶ��������ֶ��������ͱ�����nText����
	BOOL GetValText(CString& strText,CString& strFieldName);//�õ����ݿ�nText�ֶε�ֵ
	
	// GetValueStr����
	// �õ������ͣ������ͺ��ַ����ֶ�ֵ����
	// ������
	//		cVal			�������շ���ֵ���ֶ�ֵ�����ַ���ָ�룬Ҫ��Ҫ�����㹻���ڴ浥Ԫ
	//						�����㹻�����ֶ����ݵ��ַ����顣
	//		vIndex			���ݿ��ֶε����ֻ���������������,һ�㲻ֱ�������������
	//						Ӧ����ͬ��ʽ�Ķ�̬�����Ĳ�������
	//						���ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���
	//		nFieldLen		��Ҫ���ص����ݵ��ַ����ĳ��ȣ����Ϊ��1,�򷵻������ֶ�ֵ
	//		lpszFieldName	�ֶ���,���ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���
	//		nFieldIndex		��SQL������ֶε�����������ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���

	// GetValueLong����
	// �õ������ͣ������ͺ��ַ����ֶ�ֵ����
	// ������
	//		lVal			�������շ���ֵ���ֶ�ֵ��
	//		vIndex			���ݿ��ֶε����ֻ���������������,һ�㲻ֱ�������������
	//						Ӧ����ͬ��ʽ�Ķ�̬�����Ĳ�������
	//						���ݿ��ֶε���������Ҫ���������ͣ�int,long)	
	//		lpszFieldName	�ֶ���,���ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���
	//		nFieldIndex		��SQL������ֶε�����������ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���

	// GetValueFloat����
	// �õ������ͣ������ͺ��ַ����ֶ�ֵ����
	// ������
	//		fVal			�������շ���ֵ���ֶ�ֵ��
	//		vIndex			���ݿ��ֶε����ֻ���������������,һ�㲻ֱ�������������
	//						Ӧ����ͬ��ʽ�Ķ�̬�����Ĳ�������
	//						���ݿ��ֶε���������Ҫ�������ͣ�int,long,float,�����͵�)	
	//		lpszFieldName	�ֶ���,���ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���
	//		nFieldIndex		��SQL������ֶε�����������ݿ��ֶε��������Ϳ����������ͣ������ͺ��ַ���

	BOOL GetValueStr(TCHAR* cVal,_variant_t &vIndex,int nFieldLen=-1);
	BOOL GetValueLong(long* lVal,_variant_t &vIndex);
	BOOL GetValueFloat(float* fVal,_variant_t &vIndex);

	BOOL GetValueStr(TCHAR* cVal,long lIndex,int nFieldLen=-1);
	BOOL GetValueLong(long* lVal,long lIndex);
	BOOL GetValueFloat(float* fVal,long lIndex);

	BOOL GetValueStr(TCHAR* cVal,CString strIndex,int nFieldLen=-1);
	BOOL GetValueLong(long *lVal, LPCSTR lpszIndex);
	BOOL GetValueFloat(float* fVal,CString strIndex);

	BOOL GetValueStr(CString& str,LPCSTR lpszFieldName,int nFieldLen=-1);
	BOOL GetValueStr(CString& str,UINT nFieldIndex,int nFieldLen=-1);
	BOOL GetValueStr(CString& str,_variant_t &vIndex,int nFieldLen=-1);

	// �ж�ֵ�Ƿ���Ч,�Ƿ�ΪNULL
	BOOL VerifyVTData(_variant_t& value);
	BOOL VerifyVTData(TCHAR* pData);

protected:	
	_RecordsetPtr*	m_prsThis;
	_ConnectionPtr* m_pConn;
	void ReportError();
};

#endif // #if !defined(ADO_DATABASE_H_)