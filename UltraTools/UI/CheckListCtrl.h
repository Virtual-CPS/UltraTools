
#pragma once


// CCheckListCtrl

class CCheckListCtrl : public CBCGPListCtrl
{
	DECLARE_DYNAMIC(CCheckListCtrl)

public:
	CCheckListCtrl();
	virtual ~CCheckListCtrl();
	BOOL Init();
	void SetColCheck();//��ʼ����1�еĸ�ѡ��ͼ��

public:
	int GetCheckedItemCount()
	{
		int nCheckedItemCount = 0;
		int nCount = GetItemCount();
		HWND hWnd = GetSafeHwnd();
		for (int nItem = 0; nItem<nCount; nItem++)
		{
			if (ListView_GetCheckState(hWnd, nItem))//GetCheck(nItem)
				nCheckedItemCount++;
		}
		return nCheckedItemCount;
	}

	void SetOddItemBkColor(COLORREF color, BOOL bDraw = TRUE);		//���������б�����ɫ
	void SetEvenItemBkColor(COLORREF color, BOOL bDraw = TRUE);		//����ż���б�����ɫ
	void SetHoverItemBkColor(COLORREF color, BOOL bDraw = TRUE);	//�����ȵ��б�����ɫ
	void SetSelectItemBkColor(COLORREF color, BOOL bDraw = TRUE);	//����ѡ���б�����ɫ
	void SetInvalidItemBkColor(COLORREF color, BOOL bDraw = TRUE);	//������Ϣ��Ч�б�����ɫ
	void SetOddItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//�����������ı���ɫ
	void SetEvenItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//����ż�����ı���ɫ
	void SetHoverItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//�����ȵ����ı���ɫ
	void SetSelectItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//����ѡ�����ı���ɫ
	void SetInvalidItemTextColor(COLORREF color, BOOL bDraw = TRUE);//������Ϣ��Ч���ı���ɫ

protected:
	BOOL m_bTracking;
	int m_nHoverIndex;				//��ǰ�ȵ���Ŀ����
	COLORREF m_OddItemBkColor;		//�����б�����ɫ
	COLORREF m_EvenItemBkColor;		//ż���б�����ɫ
	COLORREF m_HoverItemBkColor;	//�ȵ��б�����ɫ
	COLORREF m_SelectItemBkColor;	//ѡ���б�����ɫ
	COLORREF m_InvalidItemBkColor;	//��Ϣ��Ч�б�����ɫ
	COLORREF m_OddItemTextColor;	//�������ı���ɫ
	COLORREF m_EvenItemTextColor;	//ż�����ı���ɫ
	COLORREF m_HoverItemTextColor;	//�ȵ����ı���ɫ
	COLORREF m_SelectItemTextColor;	//ѡ�����ı���ɫ
	COLORREF m_InvalidItemTextColor;//��Ϣ��Ч���ı���ɫ

protected:
	DECLARE_MESSAGE_MAP()

private:
	BOOL		m_bInited;
	CImageList	m_checkImgList;
	BOOL		m_bChecked;
	void ScanItemsSetHeadSate(int curItem);

public:
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
};