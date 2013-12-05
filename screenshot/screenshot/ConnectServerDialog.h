#pragma once


// CConnectServerDialog dialog

class CConnectServerDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CConnectServerDialog)

public:
	CConnectServerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectServerDialog();

// Dialog Data
	enum { IDD = IDD_CONNECT_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
