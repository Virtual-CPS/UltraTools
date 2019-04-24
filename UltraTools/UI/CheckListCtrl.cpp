// CheckListCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CheckListCtrl.h"
#include "../Resource.h"


// CCheckListCtrl

IMPLEMENT_DYNAMIC(CCheckListCtrl, CBCGPListCtrl)

CCheckListCtrl::CCheckListCtrl()
: m_bInited(FALSE), m_bChecked(FALSE)
{
	m_OddItemBkColor = 0xF7F7F7;								//�����б�����ɫ
	m_EvenItemBkColor = 0xFFFFFF;								//ż���б�����ɫ 0xFCFCFC
	m_HoverItemBkColor = GetSysColor(COLOR_HIGHLIGHT);			//�ȵ��б�����ɫ 0xFF
	m_SelectItemBkColor = GetSysColor(COLOR_HIGHLIGHT);			//ѡ���б�����ɫ
	m_InvalidItemBkColor = 0xFF;								//��Ϣ��Ч�б�����ɫ
	m_OddItemTextColor = GetSysColor(COLOR_BTNTEXT);			//�������ı���ɫ
	m_EvenItemTextColor = GetSysColor(COLOR_BTNTEXT);			//ż�����ı���ɫ
	m_HoverItemTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);	//�ȵ����ı���ɫ
	m_SelectItemTextColor = GetSysColor(COLOR_BTNTEXT);			//ѡ�����ı���ɫ
	m_InvalidItemTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);	//��Ϣ��Ч���ı���ɫ
	m_nHoverIndex = -1;
	m_bTracking = FALSE;
}

CCheckListCtrl::~CCheckListCtrl()
{
}

BEGIN_MESSAGE_MAP(CCheckListCtrl, CBCGPListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CCheckListCtrl::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_CLICK, &CCheckListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CCheckListCtrl::OnNMCustomdraw)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CCheckListCtrl::SetOddItemBkColor(COLORREF color, BOOL bDraw)//���������б�����ɫ
{
	m_OddItemBkColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetEvenItemBkColor(COLORREF color, BOOL bDraw)//����ż���б�����ɫ
{
	m_EvenItemBkColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetHoverItemBkColor(COLORREF color, BOOL bDraw)//�����ȵ��б�����ɫ
{
	m_HoverItemBkColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetSelectItemBkColor(COLORREF color, BOOL bDraw)//����ѡ���б�����ɫ
{
	m_SelectItemBkColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetInvalidItemBkColor(COLORREF color, BOOL bDraw)//������Ϣ��Ч�б�����ɫ
{
	m_InvalidItemBkColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetOddItemTextColor(COLORREF color, BOOL bDraw)//�����������ı���ɫ
{
	m_OddItemTextColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetEvenItemTextColor(COLORREF color, BOOL bDraw)//����ż�����ı���ɫ
{
	m_EvenItemTextColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetHoverItemTextColor(COLORREF color, BOOL bDraw)//�����ȵ����ı���ɫ
{
	m_HoverItemTextColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetSelectItemTextColor(COLORREF color, BOOL bDraw)//����ѡ�����ı���ɫ
{
	m_SelectItemTextColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}
void CCheckListCtrl::SetInvalidItemTextColor(COLORREF color, BOOL bDraw)//������Ϣ��Ч���ı���ɫ
{
	m_InvalidItemTextColor = color;
	if (bDraw)
		InvalidateRect(NULL);
}

BOOL CCheckListCtrl::Init()
{
	if (m_bInited)
		return TRUE;

	VERIFY(m_checkImgList.Create(IDB_CHECKBOXES, 16, 4, RGB(255, 0, 255)));
	SetColCheck();

	m_bInited = TRUE;

	return TRUE;
}

void CCheckListCtrl::SetColCheck()
{
	this->GetHeaderCtrl().SetImageList(&m_checkImgList);
	HDITEM hdItem;
	hdItem.mask = HDI_IMAGE | HDI_FORMAT;
	VERIFY(this->GetHeaderCtrl().GetItem(0, &hdItem));
	hdItem.iImage = 1;
	hdItem.fmt |= HDF_IMAGE;
	VERIFY(this->GetHeaderCtrl().SetItem(0, &hdItem));
}

// CCheckListCtrl ��Ϣ�������

void CCheckListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iSubItem == 0)
	{
		int nCount = GetItemCount();
		if (nCount > 0)
		{
			m_bChecked = !m_bChecked;
			for (int i=0; i<nCount; i++)
				SetCheck(i, m_bChecked);
			HDITEM hdItem;
			hdItem.mask = HDI_IMAGE | HDI_FORMAT;
			VERIFY(this->GetHeaderCtrl().GetItem(0, &hdItem));
			if (m_bChecked)
				hdItem.iImage = 2;
			else
				hdItem.iImage = 1;
			hdItem.fmt |= HDF_IMAGE;
			VERIFY(this->GetHeaderCtrl().SetItem(0, &hdItem));
		}
	}
	*pResult = 0;
}

void CCheckListCtrl::ScanItemsSetHeadSate(int curItem)
{
	int nCheckedItemCount = 0;
	int nCount = GetItemCount();
	HWND hWnd = GetSafeHwnd();
	for (int nItem=0; nItem<nCount; nItem++)
	{
		if (ListView_GetCheckState(hWnd, nItem))//GetCheck(nItem)
			nCheckedItemCount++;
	}

	//��Ϊ��ʱ�����л�δ���״̬���£����������⴦��
	if (!ListView_GetCheckState(hWnd, curItem))
		nCheckedItemCount++;
	else
		nCheckedItemCount--;

	HDITEM hdItem;
	hdItem.mask = HDI_IMAGE | HDI_FORMAT;
	VERIFY(this->GetHeaderCtrl().GetItem(0, &hdItem));
	if (nCheckedItemCount == 0)
	{
		m_bChecked = FALSE;
		hdItem.iImage = 1;
	}
	else
	{
		if (nCheckedItemCount == nCount)
		{
			m_bChecked = TRUE;
			hdItem.iImage = 2;
		}
		else
			if (nCheckedItemCount > 0 && nCheckedItemCount < nCount)
				hdItem.iImage = 3;
	}
	hdItem.fmt |= HDF_IMAGE;
	VERIFY(this->GetHeaderCtrl().SetItem(0, &hdItem));
}

void CCheckListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	ScreenToClient(&point);
	UINT nFlag;
	int nItem = HitTest(point, &nFlag);
	if (nFlag == LVHT_ONITEMSTATEICON)
		ScanItemsSetHeadSate(pNMItemActivate->iItem);

	*pResult = 0;
}

void CCheckListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = 0;

	int nItemIndex = pNMCD->nmcd.dwItemSpec;
	if (pNMCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else
	{
		if (nItemIndex == m_nHoverIndex)
		{	//�ȵ���
			pNMCD->clrTextBk = m_HoverItemBkColor;
			pNMCD->clrText = m_HoverItemTextColor;
		}
		else
		{
			if (GetItemState(nItemIndex, LVIS_SELECTED) == LVIS_SELECTED)
			{	//ѡ����
				pNMCD->clrTextBk = m_SelectItemBkColor;
				pNMCD->clrText = pNMCD->clrFace = m_SelectItemTextColor;
				::SetTextColor(pNMCD->nmcd.hdc, m_SelectItemTextColor);
			}
			else
			{
				if (nItemIndex % 2 == 0)
				{	//ż����
					pNMCD->clrTextBk = m_EvenItemBkColor;
					pNMCD->clrText = m_EvenItemTextColor;
				}
				else
				{	//������
					pNMCD->clrTextBk = m_OddItemBkColor;
					pNMCD->clrText = m_OddItemTextColor;
				}
			}
		}

		*pResult = CDRF_NEWFONT;
	}
}


void CCheckListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	int nIndex = HitTest(point);
	if (nIndex != m_nHoverIndex)
	{
		int nOldIndex = m_nHoverIndex;
		m_nHoverIndex = nIndex;
		CRect rc;
		if (nOldIndex != -1)
		{
			GetItemRect(nOldIndex, &rc, LVIR_BOUNDS);
			InvalidateRect(&rc);
		}
		if (m_nHoverIndex != -1)
		{
			GetItemRect(m_nHoverIndex, &rc, LVIR_BOUNDS);
			InvalidateRect(&rc);
		}
	}

	if (!m_bTracking)
	{
		TRACKMOUSEEVENT   tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;//   |   TME_HOVER; 
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CBCGPListCtrl::OnMouseMove(nFlags, point);
}

LRESULT CCheckListCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;

	if (m_nHoverIndex != -1)
	{
		CRect rc;
		GetItemRect(m_nHoverIndex, &rc, LVIR_BOUNDS);
		m_nHoverIndex = -1;
		InvalidateRect(&rc);
	}
	return 0;
}
