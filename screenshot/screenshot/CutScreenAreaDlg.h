#pragma once
#include "atltypes.h"


// CCutScreenAreaDlg dialog

class CCutScreenAreaDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCutScreenAreaDlg)

public:
	CCutScreenAreaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCutScreenAreaDlg();

// Dialog Data
	enum { IDD = IDD_CUT_SCREEN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	CPoint StartPoint;
	CPoint EndPoint;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
};
