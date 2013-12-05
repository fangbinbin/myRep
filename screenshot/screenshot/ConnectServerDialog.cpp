// ConnectServerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "screenshot.h"
#include "ConnectServerDialog.h"
#include "afxdialogex.h"


// CConnectServerDialog dialog

IMPLEMENT_DYNAMIC(CConnectServerDialog, CDialogEx)

CConnectServerDialog::CConnectServerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConnectServerDialog::IDD, pParent)
{

}

CConnectServerDialog::~CConnectServerDialog()
{
}

void CConnectServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConnectServerDialog, CDialogEx)
END_MESSAGE_MAP()


// CConnectServerDialog message handlers
