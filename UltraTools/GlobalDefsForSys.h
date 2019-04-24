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
//	����ϵͳ�������ݽṹ��ȫ�ֺ��ȫ�ֺ���
//
//------------------------------ Modifications ------------------------------
//	$Log: $  
//===========================================================================


#ifndef _GLOBLE_DEFS_FOR_SYS_H_
#define _GLOBLE_DEFS_FOR_SYS_H_

#include "ThermalImage.h"

//------------------------------------------------------------
// ͳһ�����䡱��Ϣ��
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

#define MEASUREMENT_TOOLS_MAX_CNT	5		// ���¹���Spot/Rectangle������

enum ImageFileType
{
	IMAGE_FILE_TYPE_INVALID = -1,
	IMAGE_FILE_TYPE_NORMAL = 0,				// ����ͼ��
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
	CString sFileName;			// ����·��
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

// �����ǲ����¶�ʱ�Ĳ���
struct tagIRCameraMeasurementParameters 
{
	float	fEmissivity;			// ������
	float	fReflectedTemp;			// �����¶�
	float	fAtmosphericTemp;		// �����¶�
	float	fRelativeHumidity;		// ���ʪ��
	float	fDistance;				// ����

	float	fEstAtmTransm;			// ����������ƣ���Ϊ�㣬FLIR�����ݴ����¶ȡ������ʪ�ȼ���
	float	fExtOptTemp;			// �����ѧ�¶�
	float	fExtOptTransm;			// �����ѧ����

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

// ����ͼ�ײ�ѯ���(�൱�ڼ�¼��������ͼ��)
// ���ڳɶԼ��غ���ͼ�׺���ԵĿɼ���ͼ��
struct tagIRImageQueryResultSingleRecord
{
	CString sEquipmentID, sTsID, sDateTime, sIRCameraID, sVLCameraID;
	CString sIRImageFileName, sVLImageFileName, sOperatorID;

	tagIRImageQueryResultSingleRecord()
	{
		sEquipmentID = _T("");		// �����豸���
		sTsID = _T("");				// �������վ���
		sDateTime = _T("");			// �����¼ʱ��
		sIRCameraID = _T("");		// ����������ID
		sVLCameraID = _T("");		// �չ������ID
		sIRImageFileName = _T("");	// ����ͼƬ�ļ���
		sVLImageFileName = _T("");	// �չ�ͼƬ�ļ���
		sOperatorID = _T("");		// �豸����ԱID
	}
};
typedef CArray<tagIRImageQueryResultSingleRecord, tagIRImageQueryResultSingleRecord> IR_IMAGE_RECORDSET;

// ��ͼ���嵥�л�ȡ����ͼ��Ϣ�������ɱ�����
// ������ͼʱ���������Ķ��࣬����ͼ�ĵ������ֱ����FileNameA or FileNameB
// sPath��"\\"��β
struct tagImageFileBaseInfo
{
	CString sTs, sEquipmentUnit, sEquipment, sPhaseGroup, sType, sGroupMark, sPath;
	CString sFileNameA, sFileNameB, sFileNameC;
	CString sDate, sTime;			// �����ļ�������Listʱ�����ٻ�ȡ���ǡ����һ���޸����ڡ�

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

// ����ͼ�л�ȡ�������ǲ���������������ݣ������ɱ�����
// ��������/�����������𣩺͵��࣬��ֻ��ʾ������¡���Ϊ�򻯳����д���¶����ݾ�����aBody[ ]
struct tagImageFileData
{
	float	fEmissivity;			// ������
	float	fReflectedTemp;			// �����¶�
	float	fAtmosphericTemp;		// �����¶�
	float	fRelativeHumidity;		// ���ʪ��
	float	fDistance;				// ����
	float	fWindSpeed;				// ����

	// A/B/C�������
	float   aPhaseA[MEASUREMENT_TOOLS_MAX_CNT], aPhaseB[MEASUREMENT_TOOLS_MAX_CNT], aPhaseC[MEASUREMENT_TOOLS_MAX_CNT];
	// AB/AC/BC������²�
	float   aPhaseAB[MEASUREMENT_TOOLS_MAX_CNT], aPhaseAC[MEASUREMENT_TOOLS_MAX_CNT], aPhaseBC[MEASUREMENT_TOOLS_MAX_CNT];
	// ���������
	float	aBody[MEASUREMENT_TOOLS_MAX_CNT];

	int		nMeasurementRectangleCnt;	// ʵ��ʹ�õĲ��¿�����
	int		nMeasurementSpotCnt;		// ʵ��ʹ�õĲ��µ�����

	CString sDate, sTime;				// �������ڣ������һ���޸�����

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

// ����CFilleDialog�����ڼ��ػ򱣴汾ϵͳ֧�ֵ�ͼ�׸�ʽ
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
