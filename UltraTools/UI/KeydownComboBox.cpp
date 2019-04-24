// KeydownComboBox.cpp : implementation file
//
// 在组合框中捕获键盘消息，对组合框中的列表项
// 按汉字拼音首字母进行搜索，如果有多个选项符合条件用'+'键向下选择
// 对于英文中的特殊字符不必输入，系统自动过滤

#include "stdafx.h"
#include "KeydownComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeydownComboBox

CKeydownComboBox::CKeydownComboBox()
{
	m_bAutoSearchChinese = TRUE;
	m_sKeyboardEntry = _T("");
	m_nNextIndex = -1;
}

CKeydownComboBox::~CKeydownComboBox()
{
}


BEGIN_MESSAGE_MAP(CKeydownComboBox, CBCGPComboBox)
	//{{AFX_MSG_MAP(CKeydownComboBox)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(CBN_SETFOCUS, &CKeydownComboBox::OnCbnSetfocus)
	ON_CONTROL_REFLECT(CBN_SELENDOK, &CKeydownComboBox::OnCbnSelendok)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeydownComboBox message handlers

//********************************************************************************************
//* Syntax
//*        GetFirstLetter(CString sName, CString& sFirstLetter)
//* Remarks:
//*        Get the first letter of pinyin according to specified Chinese character.
//* Parameters:
//*        sChineseStr      - a CString object that is to be parsed.
//*        sFirstLetter     - a CString object that is to receive the string of the first letter.
//* Return Values:
//*        None.
//* Author:
//* Create Date:
//********************************************************************************************
void CKeydownComboBox::GetFirstLetter(CString sChineseStr, CString &sFirstLetter, int nFirstLetterStrLen)
{
	TBYTE ucHigh, ucLow;
    int nCode;
    CString sRet;
	
    sFirstLetter.Empty();
	int nLen = sChineseStr.GetLength();
	int nCnt = 1;
	
    for (int i=0; i<nLen && nCnt<=nFirstLetterStrLen; i++)
    {
        if ( (TBYTE)sChineseStr[i] < 0x80 )		// 不是汉字
            continue;
		
        ucHigh = (TBYTE)sChineseStr[i];
        ucLow  = (TBYTE)sChineseStr[i+1];
        if ( ucHigh < 0xa1 || ucLow < 0xa1)
            continue;
        else
            // Treat code by section-position as an int type parameter,
            // so make following change to nCode.
            nCode = (ucHigh - 0xa0) * 100 + ucLow - 0xa0;
		
        FirstLetter(nCode, sRet);
        sFirstLetter += sRet;
		nCnt++;
        i++;
    }
}

//********************************************************************************************
//* Syntax
//*        void FirstLetter(int nCode, CString& sLetter)
//* Remarks:
//*        Get the first letter of pinyin according to specified Chinese character code.
//* Parameters:
//*        nCode        - the code of the Chinese character.
//*        sLetter      - a CString object that is to receive the string of the first letter.
//* Return Values:
//*        None.
//* Author:
//* Create Date:
//********************************************************************************************
void CKeydownComboBox::FirstLetter(int nCode, CString &sLetter)
{
	if(nCode >= 1601 && nCode < 1637) { sLetter = _T("A"); return;}
	if(nCode >= 1637 && nCode < 1833) { sLetter = _T("B"); return;}
	if(nCode >= 1833 && nCode < 2078) { sLetter = _T("C"); return;}
	if(nCode >= 2078 && nCode < 2274) { sLetter = _T("D"); return;}
	if(nCode >= 2274 && nCode < 2302) { sLetter = _T("E"); return;}
	if(nCode >= 2302 && nCode < 2433) { sLetter = _T("F"); return;}
	if(nCode >= 2433 && nCode < 2594) { sLetter = _T("G"); return;}
	if(nCode >= 2594 && nCode < 2787) { sLetter = _T("H"); return;}
	if(nCode >= 2787 && nCode < 3106) { sLetter = _T("J"); return;}
	if(nCode >= 3106 && nCode < 3212) { sLetter = _T("K"); return;}
	if(nCode >= 3212 && nCode < 3472) { sLetter = _T("L"); return;}
	if(nCode >= 3472 && nCode < 3635) { sLetter = _T("M"); return;}
	if(nCode >= 3635 && nCode < 3722) { sLetter = _T("N"); return;}
	if(nCode >= 3722 && nCode < 3730) { sLetter = _T("O"); return;}
	if(nCode >= 3730 && nCode < 3858) { sLetter = _T("P"); return;}
	if(nCode >= 3858 && nCode < 4027) { sLetter = _T("Q"); return;}
	if(nCode >= 4027 && nCode < 4086) { sLetter = _T("R"); return;}
	if(nCode >= 4086 && nCode < 4390) { sLetter = _T("S"); return;}
	if(nCode >= 4390 && nCode < 4558) { sLetter = _T("T"); return;}
	if(nCode >= 4558 && nCode < 4684) { sLetter = _T("W"); return;}
	if(nCode >= 4684 && nCode < 4925) { sLetter = _T("X"); return;}
	if(nCode >= 4925 && nCode < 5249) { sLetter = _T("Y"); return;}
    if(nCode >= 5249 && nCode < 5590) { sLetter = _T("Z"); return;}
}

void CKeydownComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE)
	{
		m_sKeyboardEntry = _T("");
		//SetCurSel(-1);
	}
// 	else if (nChar == VK_RETURN)
// 	{
// 		CString str;
// 		GetLBText(GetCurSel(),str);
// 		MessageBox(str);
// 	}
	else if (nChar == '+')
	{
		SearchItem(m_nNextIndex+1, TRUE);
	}
	else if(isalnum(nChar))
	{
		m_sKeyboardEntry = m_sKeyboardEntry + char(nChar);
		SearchItem(0, TRUE);
	}
	else if (nChar == '-')
	{
		SearchItem(m_nNextIndex-1, FALSE);
	}
}

void CKeydownComboBox::SearchItem(int nStart,  BOOL bForward)
{
	int nLen = m_sKeyboardEntry.GetLength();
	CString sItem = "";
	CString strname="";
	CString str_e_name="";
	char szHZ[256];
	int nCnt = GetCount();
	for (int n=nStart; bForward ? n<nCnt : n>0; bForward ? n++ : n--)	//遍历列表
	{
		GetLBText(n, sItem);
		if(m_bAutoSearchChinese)
			RetainOnlyChinese(sItem);
		if(sItem.GetLength() <= 0)
			continue;

		sprintf_s(szHZ, "%s", sItem);  
		if(szHZ[0] < 0)									//判断是否为汉字
		{
			if (2*nLen <= GetLBTextLen(n))
			{
				GetFirstLetter(sItem, strname, nLen);
			}
			if(m_sKeyboardEntry.CompareNoCase(strname) == 0)
			{
				m_nNextIndex=n;
				SetCurSel(n);
				ShowDropDown();
				UpdateData();
				break;
			}      
		}
		else 
		{
			RetainOnlyAlnum(sItem);
			if (nLen <= sItem.GetLength())
			{
				str_e_name = sItem.Mid(0, nLen);
				if (m_sKeyboardEntry.CompareNoCase(str_e_name) == 0)
				{
					m_nNextIndex = n;
					SetCurSel(n);
					UpdateData();
					ShowDropDown();
					break;
				}
			}
		}
	}
}

// 只保留字母数字字符
void CKeydownComboBox::RetainOnlyAlnum(CString &sStr)
{
	for (int i=0; i<sStr.GetLength(); i++)
	{
		char ch0 = sStr.GetAt(i);
		if (!isalnum(ch0))
		{
			if (ch0)
			{
				sStr.Remove(ch0);
				i--;
			}
		}
	}
}

// 删除全部字母数字字符
void CKeydownComboBox::RetainOnlyChinese(CString &sStr)
{
	for (int i=0; i<sStr.GetLength(); i++)
	{
		char ch0 = sStr.GetAt(i);
		if(ch0 & 0x80)
		{
			i++;
			continue;
		}
		if (ch0)
		{
			sStr.Remove(ch0);
			i--;
		}
	}
}

void CKeydownComboBox::OnCbnSetfocus()
{
	m_sKeyboardEntry = _T("");
}

void CKeydownComboBox::OnCbnSelendok()
{
	m_sKeyboardEntry = _T("");
}
