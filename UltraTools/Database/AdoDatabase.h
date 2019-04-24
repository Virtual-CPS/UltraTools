#if !defined(ADO_DATABASE_H_)
#define ADO_DATABASE_H_

#include <afxdb.h>

// AdoDatabase.h : header file
//
//
// ADO访问数据库类
// 软件环境：需要MSADO15.DLL
//
// 说明：封装了ADO使用的操作函数和ADO访问数据库错误处理
//		使在VC上方便的使用ADO访问数据库
// 包括：ADO连接数据库、打开记录集、执行SQL、以及
//		事务处理、ADO记录集内容转换成C++的常用数据类型（CString、char，long，float等）
//
// 使用：1.定义一个CBFAdoConnection实例：CBFAdoConnection m_adoConn;
//		 2.建立连接：m_adoConn.Open(strConnstring);
//					如果需要无条件重新连接的可以，参数bRepen设成TRUE
//		 3.打开记录集：
//
//			_RecordsetPtr* prsThis=NULL;
//			prsThis=new _RecordsetPtr;
//			CString strSQL;
//			strSQL="select * from Table_Name";//		
//			//如果记录集打开失败
//			if(!(m_adoConn.OpenRecordset(strSQL,prsThis)))
//				return FALSE;
//		 4.建立记录集值对象
//			注意需要用参数构造
//			CBFRstValues rsv(m_adoConn,prsThis);
//		
//		 5.获得的字段的值
//			rsv.GetValueLong(&m_nDeptID,1L);//获得第一个字段的值
//			rsv.GetValueStr(m_strName,"ID");//获得第一个字段名为ID的值
//			其它的同理.如果需要获得SQL Server中nText类型的字段值请用
//			GetValText(CString &strText, CString &strFieldName)函数
//		 6.记录集的记录移动	(*prsThis)->MoveNext()等类似函数
//		 7.记录集不用时候需要释放其资源
//			1)关闭记录集
//			2)删除记录集指针
//			e.g.	(*prsThis)->Close();
//					delete prsThis;
//					prsThis=NULL;
//			否则会造成内存泄漏
//
//		注：
//        1.程序必须要初始化COM环境，请在应用类中加入AfxInitOle函数初始化环境,否则ADO的调用将失败
//        2.如果需要调用存储过程SQL语句改为“Exec”+存储过程名即可,与执行SQL同样
//
//
//

// msado15.dll必须放在本文件所在目录，或者自己指定下面的msado15.dll全路径
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
	int SetConnTimeOut(long lTimeOut);		// 设置连接超时
	int SetCommTimeOut(long lTimeOut);		// 设置命令执行超时

	BOOL IsConnectClosed();					// 判断连接是否已经打开
	int ExecuteSQL(LPCSTR szSQL);			// 简单执行SQL语句，不返回记录集

	// 打开数据库记录集
	// 参数：
	//		strSQL		记录集的SQL语句
	//		rs			返回的记录集_RecordsetPtr对象	
	//		sConnString	数据库的连接字符串
	//					如果使用数据库连接已经打开，参数没用
	//					如果数据库的连接没有打开，当给予一个连接字符串,将先打开数据库连接
	BOOL OpenRecordset(CString strSQL, _RecordsetPtr *rs, CString sConnString=_T(""));//打开数据库记录集	
	BOOL OpenRecordset(const TCHAR *sSQL, _RecordsetPtr* rs, TCHAR* sConnString=NULL);

	// 打开数据库连接
	// 参数：
	//		strConnString	连接字符串
	//		sConnString		连接字符串
	//		bReOpen			是否重新打开，如果为FALSE，，
	//						将先判断数据库是否打开如果没有打开则打开，
	//						如果已经打开，将不执行任何操作
	//						如果为TRUE，则无条件重新打开。
	BOOL OpenConnection(CString strConnString, BOOL bReOpen=FALSE);	
	BOOL OpenConnection(TCHAR* sConnString, BOOL bReOpen=FALSE);
	//static BOOL OpenConnectionThread(CBFAdoConnection* pBfAdoCon);	// 开启连接线程，避免阻塞主线程

	void CloseConnect();					// 关闭数据库连接	

	BOOL ExecuteTrans(CStringArray arrStrSQL);// 开始事务处理,不返回任何记录集，参数为事务SQL数组


	_ConnectionPtr* GetConnection();		// 得到_ConnectionPtr指针
	CString GetConnString();				// 得到连接字符串

	void GetTableNames();					// 枚举当前连接数据库中的所有表名
	void GetTableFields(CString sTableName);// 枚举指定表中的所有字段

	// 统计，参数SqlCommand的形式如下：
	// 1. 统计记录数：SELECT COUNT(*) FROM TableName[ WHERE…]
	// 2. 统计字段总和：SELECT SUM FieldName FROM TableName[ WHERE…]
	// 3. 统计字段平均值：SELECT AVG FieldName FROM TableName[ WHERE…] 
	// 4. 统计字段最大值：SELECT MAX FieldName FROM TableName[ WHERE…]
	// 5. 统计字段最小值：SELECT MIN FieldName FROM TableName[ WHERE…]
	//inline BOOL Statistics(CString SqlCommand, CString& sResult);       // 统计
	BOOL Statistics(CString SqlCommand, TCHAR* szResult);       // 统计

// Attribute:
public:
	CStringArray m_saTableNames;			// 返回GetTableNames()获取的结果
	CStringArray m_saFieldNames;			// 返回GetTableFields()获取的结果
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
	// 三种构造类的方法
	// 如果无参数构造，请构造后调用InitConnectAndRst方法
	// 其他的两种构造则不需调用InitConnectAndRst方法	
	CBFRstValues();
	CBFRstValues(_ConnectionPtr* pConn, _RecordsetPtr* pRs);
	CBFRstValues(CBFAdoConnection* pBFadoConn, _RecordsetPtr* pRs);

	virtual ~CBFRstValues();
public:
	// 初始化连接队象和记录集对象
	void InitConnectAndRst(_ConnectionPtr* pConn, _RecordsetPtr* pRs);
	void InitConnectAndRst(CBFAdoConnection* pBFAdoConn, _RecordsetPtr * pRs);

	// GetValText函数
	// 得到数据库nText字段的值
	// 参数：
	//		strText			用来接收返回值（字段值）
	//		strFieldName	字段名，该字段数据类型必须是nText类型
	BOOL GetValText(CString& strText,CString& strFieldName);//得到数据库nText字段的值
	
	// GetValueStr函数
	// 得到数字型，日期型和字符型字段值函数
	// 参数：
	//		cVal			用来接收返回值（字段值）的字符串指针，要求要开辟足够的内存单元
	//						或者足够容纳字段内容的字符数组。
	//		vIndex			数据库字段的名字或者索引，变体型,一般不直接用这个参数，
	//						应该用同形式的多态函数的参数调用
	//						数据库字段的数据类型可以是数字型，日期型和字符型
	//		nFieldLen		需要返回的数据的字符串的长度，如果为－1,则返回整个字段值
	//		lpszFieldName	字段名,数据库字段的数据类型可以是数字型，日期型和字符型
	//		nFieldIndex		在SQL语句中字段的索引序号数据库字段的数据类型可以是数字型，日期型和字符型

	// GetValueLong函数
	// 得到数字型，日期型和字符型字段值函数
	// 参数：
	//		lVal			用来接收返回值（字段值）
	//		vIndex			数据库字段的名字或者索引，变体型,一般不直接用这个参数，
	//						应该用同形式的多态函数的参数调用
	//						数据库字段的数据类型要求是数字型（int,long)	
	//		lpszFieldName	字段名,数据库字段的数据类型可以是数字型，日期型和字符型
	//		nFieldIndex		在SQL语句中字段的索引序号数据库字段的数据类型可以是数字型，日期型和字符型

	// GetValueFloat函数
	// 得到数字型，日期型和字符型字段值函数
	// 参数：
	//		fVal			用来接收返回值（字段值）
	//		vIndex			数据库字段的名字或者索引，变体型,一般不直接用这个参数，
	//						应该用同形式的多态函数的参数调用
	//						数据库字段的数据类型要求是字型（int,long,float,货币型等)	
	//		lpszFieldName	字段名,数据库字段的数据类型可以是数字型，日期型和字符型
	//		nFieldIndex		在SQL语句中字段的索引序号数据库字段的数据类型可以是数字型，日期型和字符型

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

	// 判断值是否有效,是否为NULL
	BOOL VerifyVTData(_variant_t& value);
	BOOL VerifyVTData(TCHAR* pData);

protected:	
	_RecordsetPtr*	m_prsThis;
	_ConnectionPtr* m_pConn;
	void ReportError();
};

#endif // #if !defined(ADO_DATABASE_H_)