// HvUtility.h

#pragma once

#include <string>
#include <tchar.h>

#ifdef _UNICODE
	#define tstring std::wstring
#else
	#define tstring std::string
#endif



#define MARK_TYPE_APP					1
#define MARK_TYPE_ORGANIZATION			2
#define	MARK_TYPE_HOST					3
#define MARK_TYPE_STAFF					4


class HvUtility
{
private:
	static tstring m_sAppName;
	static tstring m_sOrganizationID;
	static tstring m_sHostID;
	static tstring m_sStaffID;

public:
	static void SetAppName(HINSTANCE hInstance, const tstring& sName, bool bForAll = true);
	static void SetOrganizationID(HINSTANCE hInstance, const tstring& sID, bool bForAll = true);
	static void SetHostID(HINSTANCE hInstance, const tstring& sID, bool bForAll = true);
	static void SetStaffID(HINSTANCE hInstance, const tstring& sID, bool bForAll = true);

	static void CalculateMD5(const tstring& sMsg, tstring& sMD5);

	static int Encrypt(const tstring& sPlaintext, tstring& sCiphertext);
	static int Decrypt(const tstring& sCiphertext, tstring& sPlaintext);
	static bool GenerateSharedSecret(HINSTANCE hInstance, const tstring& sPrivateKey,
						const tstring& sPeerPublicKey, const tstring& sPeerKeyMark, int nMarkType,
						tstring& sSharedSecret, int *pSharedOriginalLen, bool bUsingPeerKey=false);
	static bool HvSign(const tstring& sPrivateKey, tstring& sSignature, int* pSignatureOriginalLen,
						const tstring& sMsg=_T(""));
	static bool HvVerifySignature(const tstring& sPeerPublicKey, const tstring& sSignature,
						const tstring& sMsg=_T(""));

	static tstring Char2String(char* pBuf);
	static void DeleteMe();									// 删除当前程序
	static int AddAppToFW(tstring sAppName);				// 将当前程序添加到防火墙的“例外”

	static bool CreateMultiDirectory(tstring sDir);			// 创建多层目录
	static void DeleteMultiDirectory(const tstring& sDir);	// 删除指定目录及其子目录
#ifndef _THERMAL_IMG_DLL
	static void SetComboBoxWidthEnoughToShowAllItems(CComboBox* pComboBox);
#endif
	static tstring GenerateGUID();
	static bool GetMapFromRes(HINSTANCE hInstance, unsigned int nResID, TCHAR* szFileName, void* pWnd);
	static int Compress(const tstring& sScrFile, const tstring& sDstFile, int nLevel=1);
	static int UnCompress(const tstring& sScrFile, const tstring& sDstFile);

	static int ApplyForAuthorization(HINSTANCE hInstance,
						tstring& sFileName, const tstring& sApplicantName, const tstring& sHostName,
						const tstring& sPrivateKey, const tstring& sPeerPublicKey, bool bForCreation=true);
	static bool Register(HINSTANCE hInstance, const tstring& sRegFile, bool bForPreAuthorization, bool bDelRegFile);
	static bool Init(HINSTANCE hInstance, int& nType, tstring& sDateSpan, tstring& sCurPriKey, tstring& sCurPubKey);
	static bool GetAppName(HINSTANCE hInstance, tstring& sName, int& nType, tstring& sDateSpan,
						tstring& sCurPriKey, tstring& sCurPubKey);
	static bool GetOrganizationID(HINSTANCE hInstance, tstring& sID, int& nType, tstring& sDateSpan,
						tstring& sCurPriKey, tstring& sCurPubKey);
	static bool GetHostID(HINSTANCE hInstance, tstring& sID, int& nType, tstring& sDateSpan,
						tstring& sCurPriKey, tstring& sCurPubKey, bool bOrigin = false);
	static bool GetStaffID(HINSTANCE hInstance, tstring& sID, int& nType, tstring& sDateSpan,
						tstring& sCurPriKey, tstring& sCurPubKey);
};
