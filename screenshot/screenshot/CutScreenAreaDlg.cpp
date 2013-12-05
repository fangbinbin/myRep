// CutScreenAreaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "screenshot.h"
#include "CutScreenAreaDlg.h"
#include "afxdialogex.h"


// CCutScreenAreaDlg dialog

IMPLEMENT_DYNAMIC(CCutScreenAreaDlg, CDialogEx)

CCutScreenAreaDlg::CCutScreenAreaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCutScreenAreaDlg::IDD, pParent)
	, StartPoint(0)
	, EndPoint(0)
{
}

CCutScreenAreaDlg::~CCutScreenAreaDlg()
{
}

void CCutScreenAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCutScreenAreaDlg, CDialogEx)
//	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CCutScreenAreaDlg message handlers


//BOOL CCutScreenAreaDlg::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: Add your message handler code here and/or call default
//	CDC *pDesktopDC=GetDesktopWindow()->GetDC();
//	CDC memDC;
//	memDC.CreateCompatibleDC(pDesktopDC);
//	CRect RectDesktop;
//	GetDesktopWindow()->GetClientRect(&RectDesktop);
//	CBitmap bmpdesktop;
//	bmpdesktop.CreateCompatibleBitmap(pDesktopDC,RectDesktop.Width(),RectDesktop.Height());
//	CRect rect;
//	GetClientRect(&rect);
//	pDC->StretchBlt(0,0,rect.Width(),rect.Height(),pDesktopDC,0,0,RectDesktop.Width(),RectDesktop.Height(),SRCCOPY);
//
//	return CDialogEx::OnEraseBkgnd(pDC);
//}


void CCutScreenAreaDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	StartPoint = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CCutScreenAreaDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	EndPoint = point;
	CDialogEx::OnLButtonUp(nFlags, point);
	//鼠标左键弹上之后自动关闭对话框
	SendMessage(WM_CLOSE);
}


BOOL CCutScreenAreaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	/*int cxScreen,cyScreen;
	cxScreen=GetSystemMetrics(SM_CXSCREEN);
	cyScreen=GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&wndTopMost,0,0,cxScreen,cyScreen,SWP_SHOWWINDOW);

	SetLayeredWindowAttributes(m_hWnd, RGB(0,0,0),0, LWA_COLORKEY);*/

	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000);
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
  
	if(hInst)  
	{  
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD);
		MYFUNC fun = NULL;  
  
		//取得SetLayeredWindowAttributes函数指针  
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)  
		{  
			SetBackgroundColor(RGB(0,0,0));
			fun(this->GetSafeHwnd(), 0, 120, 2); 
		}  
		FreeLibrary(hInst);  
	}
  
	//窗口最大化  
	ShowWindow(SW_SHOWMAXIMIZED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
