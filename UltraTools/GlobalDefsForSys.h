//===========================================================================
//	(c) Copyright 2015, HFUT, all rights reserved
//===========================================================================
//	File           : GeneralDefsForSys.h
//	$Header: $
//	Author         : hfutZRB@163.com
//	Creation       : 2015.11.06
//	Remake         : 
//------------------------------- Description -------------------------------
//
//	定义系统所用数据结构、全局宏和全局函数
//
//------------------------------ Modifications ------------------------------
//	$Log: $  
//===========================================================================


#ifndef _GLOBLE_DEFS_FOR_SYS_H_
#define _GLOBLE_DEFS_FOR_SYS_H_

#include "ThermalImage.h"

//------------------------------------------------------------
// 统一“分配”消息号
#define IDM_SHOW_DISTRICT_ALL					WM_USER+100
#define IDM_SHOW_DISTRICT_INUSE					WM_USER+101

#define WM_OBJ_MODIFIED							WM_USER+102

#define IDM_EMISSIVITY_MENU_BEGIN				WM_USER+200

#define	WM_UPDATE_IMAGE							WM_USER+800
#define WM_SPLITTER_MOVED						WM_USER+801
//------------------------------------------------------------

#define TIMER_STATUSBAR_PROGRESS_START		100
#define TIMER_SYS_DATETIME					101
// #define TIMER_CONNECT_DATABASE				102

#define	THUMBNAIL_WIDTH				160
#define	THUMBNAIL_HEIGHT			120

#define PALETTE_DEFAULT_WIDTH		40

#define MEASUREMENT_TOOLS_MAX_CNT	5		// 测温工具Spot/Rectangle最大个数

enum ImageFileType
{
	IMAGE_FILE_TYPE_INVALID = -1,
	IMAGE_FILE_TYPE_NORMAL = 0,				// 常规图谱
	IMAGE_FILE_TYPE_FLIR_JPG,
	IMAGE_FILE_TYPE_FLIR_FPF,
	IMAGE_FILE_TYPE_FLIR_IMG,
	IMAGE_FILE_TYPE_FLIR_SEQ,
};

struct tagDocType
{
public:
	int nID;
	BOOL bRead;
	BOOL bWrite;
	const TCHAR* description;
	const TCHAR* ext;
};
extern tagDocType docTypes[CMAX_IMAGE_FORMATS];

struct tagIrFileBaseInfo 
{
	CString sCameraType;
	CString sCameraSN;
	CString sLens;
	CString sFileName;			// 不含路径
	CString sFileDatetime;
	int nFileSize;				// K

	void Reset()
	{
		sCameraType = _T("");
		sCameraSN = _T("");
		sLens = _T("");
		sFileName = _T("");
		sFileDatetime = _T("");
		nFileSize = 0;
	}
};

// 热像仪测量温度时的参数
struct tagIRCameraMeasurementParameters 
{
	float	fEmissivity;			// 发射率
	float	fReflectedTemp;			// 反射温度
	float	fAtmosphericTemp;		// 大气温度
	float	fRelativeHumidity;		// 相对湿度
	float	fDistance;				// 距离

	float	fEstAtmTransm;			// 大气传输估计，若为零，FLIR将根据大气温度、距离和湿度计算
	float	fExtOptTemp;			// 表面光学温度
	float	fExtOptTransm;			// 表面光学传输

	float	fScaleLow;
	float	fScaleHigh;

	void Reset()
	{
		fEmissivity = 0.001f;
		fReflectedTemp = 0.f;
		fAtmosphericTemp = 0.f;
		fRelativeHumidity = 0.f;
		fDistance = 0.f;

		fEstAtmTransm = 0.f;
		fExtOptTemp = 1.f;
		fExtOptTransm = 1.f;

		fScaleLow = 0.f;
		fScaleHigh = 1.f;
	}
};

// 红外图谱查询结果(相当于记录集，但无图谱)
// 便于成对加载红外图谱和配对的可见光图谱
struct tagIRImageQueryResultSingleRecord
{
	CString sEquipmentID, sTsID, sDateTime, sIRCameraID, sVLCameraID;
	CString sIRImageFileName, sVLImageFileName, sOperatorID;

	tagIRImageQueryResultSingleRecord()
	{
		sEquipmentID = _T("");		// 电气设备编号
		sTsID = _T("");				// 所属变电站编号
		sDateTime = _T("");			// 拍摄记录时间
		sIRCameraID = _T("");		// 红外热像仪ID
		sVLCameraID = _T("");		// 普光摄像机ID
		sIRImageFileName = _T("");	// 红外图片文件名
		sVLImageFileName = _T("");	// 普光图片文件名
		sOperatorID = _T("");		// 设备操作员ID
	}
};
typedef CArray<tagIRImageQueryResultSingleRecord, tagIRImageQueryResultSingleRecord> IR_IMAGE_RECORDSET;

// 从图谱清单中获取的热图信息，供生成报表用
// 两幅热图时，不管是哪二相，其热图文档名均分别存于FileNameA or FileNameB
// sPath以"\\"结尾
struct tagImageFileBaseInfo
{
	CString sTs, sEquipmentUnit, sEquipment, sPhaseGroup, sType, sGroupMark, sPath;
	CString sFileNameA, sFileNameB, sFileNameC;
	CString sDate, sTime;			// 遍历文件夹生成List时，快速获取的是“最后一次修改日期”

	tagImageFileBaseInfo()
	{
		sTs = _T("");
		sEquipmentUnit = _T("");
		sEquipment = _T("");
		sPhaseGroup = _T("");
		sType = _T("");
		sGroupMark = _T("");
		sPath = _T("");
		sFileNameA = sFileNameB = sFileNameC = _T("");
		sDate = sTime = _T("");
	}
};

// 从热图中获取的热像仪测量参数及相关数据，供生成报表用
// 本体点分析/框分析（无相别）和单相，都只显示“最高温”，为简化程序编写，温度数据均存于aBody[ ]
struct tagImageFileData
{
	float	fEmissivity;			// 发射率
	float	fReflectedTemp;			// 反射温度
	float	fAtmosphericTemp;		// 大气温度
	float	fRelativeHumidity;		// 相对湿度
	float	fDistance;				// 距离
	float	fWindSpeed;				// 风速

	// A/B/C相最高温
	float   aPhaseA[MEASUREMENT_TOOLS_MAX_CNT], aPhaseB[MEASUREMENT_TOOLS_MAX_CNT], aPhaseC[MEASUREMENT_TOOLS_MAX_CNT];
	// AB/AC/BC相最高温差
	float   aPhaseAB[MEASUREMENT_TOOLS_MAX_CNT], aPhaseAC[MEASUREMENT_TOOLS_MAX_CNT], aPhaseBC[MEASUREMENT_TOOLS_MAX_CNT];
	// 本体最高温
	float	aBody[MEASUREMENT_TOOLS_MAX_CNT];

	int		nMeasurementRectangleCnt;	// 实际使用的测温框数量
	int		nMeasurementSpotCnt;		// 实际使用的测温点数量

	CString sDate, sTime;				// 拍摄日期，非最后一次修改日期

	tagImageFileData()
	{
		fEmissivity = 0.f;
		fReflectedTemp = 0.f;
		fAtmosphericTemp = 0.f;
		fRelativeHumidity = 0.f;
		fDistance = 0.f;
		fWindSpeed = 0.f;

		memset(aPhaseA, 0, sizeof(aPhaseA));
		memset(aPhaseB, 0, sizeof(aPhaseB));
		memset(aPhaseC, 0, sizeof(aPhaseC));
		memset(aPhaseAB, 0, sizeof(aPhaseAB));
		memset(aPhaseAC, 0, sizeof(aPhaseAC));
		memset(aPhaseBC, 0, sizeof(aPhaseBC));
		memset(aBody, 0, sizeof(aBody));

		nMeasurementRectangleCnt = nMeasurementSpotCnt = 0;
		sDate = sTime = _T("");
	}
};

// 定制CFilleDialog，用于加载或保存本系统支持的图谱格式
extern BOOL PromptForFileName(CString& sFileName, CString& sDefaultPath, CString sTitle,
					   DWORD dwFlags, BOOL bOpenFileDialog, int* pType=NULL);
extern int GetIndexFromType(int nDocType, BOOL bOpenFileDialog);
extern int GetTypeFromIndex(int nIndex, BOOL bOpenFileDialog);
extern CString GetExtFromType(int nDocType);
extern CString GetDescFromType(int nDocType);
extern CString GetFileTypes(BOOL bOpenFileDialog);
extern BOOL GetWritableType(int nDocType);


extern void MakePalette(void *ptr);

extern int GetTypeFromFileName(const CString& sFileName, CString& sExt);
extern BOOL MakeFpfFileThumbnail(CString sFileName, CThermalImage* pImage);
extern unsigned long _stdcall RunLoadThumbnailThread( LPVOID lpParam );

extern void /*unsigned long _stdcall*/ RunImageEditThread(void *lpParam);

extern void SetComboBoxWidthEnoughToShowAllItems(CComboBox* pComboBox);

extern CString CheckMapFile(CString sFilePathName);

#endif
