
#pragma once


// CCheckListCtrl

class CCheckListCtrl : public CBCGPListCtrl
{
	DECLARE_DYNAMIC(CCheckListCtrl)

public:
	CCheckListCtrl();
	virtual ~CCheckListCtrl();
	BOOL Init();
	void SetColCheck();//初始化第1列的复选框图标

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

	void SetOddItemBkColor(COLORREF color, BOOL bDraw = TRUE);		//设置奇数行背景颜色
	void SetEvenItemBkColor(COLORREF color, BOOL bDraw = TRUE);		//设置偶数行背景颜色
	void SetHoverItemBkColor(COLORREF color, BOOL bDraw = TRUE);	//设置热点行背景颜色
	void SetSelectItemBkColor(COLORREF color, BOOL bDraw = TRUE);	//设置选中行背景颜色
	void SetInvalidItemBkColor(COLORREF color, BOOL bDraw = TRUE);	//设置信息无效行背景颜色
	void SetOddItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//设置奇数行文本颜色
	void SetEvenItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//设置偶数行文本颜色
	void SetHoverItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//设置热点行文本颜色
	void SetSelectItemTextColor(COLORREF color, BOOL bDraw = TRUE);	//设置选中行文本颜色
	void SetInvalidItemTextColor(COLORREF color, BOOL bDraw = TRUE);//设置信息无效行文本颜色

protected:
	BOOL m_bTracking;
	int m_nHoverIndex;				//当前热点项目索引
	COLORREF m_OddItemBkColor;		//奇数行背景颜色
	COLORREF m_EvenItemBkColor;		//偶数行背景颜色
	COLORREF m_HoverItemBkColor;	//热点行背景颜色
	COLORREF m_SelectItemBkColor;	//选中行背景颜色
	COLORREF m_InvalidItemBkColor;	//信息无效行背景颜色
	COLORREF m_OddItemTextColor;	//奇数行文本颜色
	COLORREF m_EvenItemTextColor;	//偶数行文本颜色
	COLORREF m_HoverItemTextColor;	//热点行文本颜色
	COLORREF m_SelectItemTextColor;	//选中行文本颜色
	COLORREF m_InvalidItemTextColor;//信息无效行文本颜色

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