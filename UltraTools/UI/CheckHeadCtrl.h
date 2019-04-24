// CheckHeadCtrl.h : header file
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCheckHeadCtrl window

class CCheckHeadCtrl : public CBCGPHeaderCtrl
{
// Construction
public:
	CCheckHeadCtrl();

// Attributes
public:

// Operations
public:

// Overrides

// Implementation
public:
	virtual ~CCheckHeadCtrl();

	// Generated message map functions
protected:
	afx_msg void OnItemClicked(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};
