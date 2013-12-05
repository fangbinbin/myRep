
// screenshotView.cpp : implementation of the CscreenshotView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "screenshot.h"
#endif

#include "screenshotDoc.h"
#include "screenshotView.h"
#include "CutScreenAreaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <cmath>
#include <cstdio>
#include <cstring>
#include <afxpriv.h>

// CscreenshotView

IMPLEMENT_DYNCREATE(CscreenshotView, CView)

BEGIN_MESSAGE_MAP(CscreenshotView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_SCREEN_SHOT, &CscreenshotView::OnScreenShot)
	ON_COMMAND(ID_AREA_SHOT, &CscreenshotView::OnAreaShot)
	ON_UPDATE_COMMAND_UI(ID_SAVE_SHOT, &CscreenshotView::OnUpdateSaveShot)
	ON_COMMAND(ID_SAVE_SHOT, &CscreenshotView::OnSaveShot)
END_MESSAGE_MAP()

// CscreenshotView construction/destruction

CscreenshotView::CscreenshotView()
{
	// TODO: add construction code here

}

CscreenshotView::~CscreenshotView()
{
}

BOOL CscreenshotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CscreenshotView drawing

void CscreenshotView::OnDraw(CDC* pDC)
{
	CscreenshotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	DrawClientBackground(pDC);

	if(pDoc->hasScreenShotImage()){
		CBitmap *screenShotImage = pDoc->GetScreenShotImage();
		/*CDC dcMem;
		dcMem.CreateCompatibleDC(pDC);
		BITMAP bitmap;
		screenShotImage->GetBitmap(&bitmap);
		CBitmap *pbmpOld = dcMem.SelectObject(screenShotImage);*/
		DrawImageOnClient(*screenShotImage);
	}
}

void CscreenshotView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CscreenshotView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CscreenshotView diagnostics

#ifdef _DEBUG
void CscreenshotView::AssertValid() const
{
	CView::AssertValid();
}

void CscreenshotView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CscreenshotDoc* CscreenshotView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CscreenshotDoc)));
	return (CscreenshotDoc*)m_pDocument;
}
#endif //_DEBUG


//绘制程序客户区背景

void CscreenshotView::DrawClientBackground(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap bmpBackground;
	bmpBackground.LoadBitmapW(IDB_BACKGROUND);
	BITMAP bitmap;
	bmpBackground.GetBitmap(&bitmap);
	CBitmap *pbmpOld = dcMem.SelectObject(&bmpBackground);
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, (bitmap.bmWidth - rect.Width()) / 2, (bitmap.bmHeight - rect.Height()) / 2, rect.Width(), rect.Height(), SRCCOPY);
	dcMem.SelectObject(pbmpOld);
}

//截图并把图片绘制在程序客户区上
void CscreenshotView::OnScreenShot()
{
	// TODO: Add your command handler code here
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	Sleep(50);
	CDC *pDesktopDC=GetDesktopWindow()->GetDC();

	CRect RectDesktop;
	GetDesktopWindow()->GetClientRect(&RectDesktop);
	CBitmap bmpdesktop;
	bmpdesktop.CreateCompatibleBitmap(pDesktopDC, RectDesktop.Width(), RectDesktop.Height());

	DrawClientBackground(GetDC());

	CDC memDC;
	memDC.CreateCompatibleDC(pDesktopDC);
	memDC.SelectObject(&bmpdesktop);
	memDC.BitBlt(0, 0, RectDesktop.Width(), RectDesktop.Height(), pDesktopDC, 0, 0, SRCCOPY);

	//临时存储截图到CscreenshotDoc对象中
	CscreenshotDoc* pDoc = GetDocument();
	pDoc->copyScreenShotImage(&memDC, bmpdesktop);

	/*if(pDoc->hasScreenShotImage()){
		CBitmap *screenShotImage = pDoc->GetScreenShotImage();
		DrawImageOnClient(*screenShotImage);
	}*/

	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

//把bitmap绘制在客户区上，pDC为与srcBitmap相联系的CDC
void CscreenshotView::DrawImageOnClient(CBitmap& srcBitmap)
{
	CDC *pDC=this->GetDC();
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	BITMAP bmpInfo;
	srcBitmap.GetBitmap(&bmpInfo);
	CBitmap *pbmpOld = dcMem.SelectObject(&srcBitmap);

	CRect rect;
	GetClientRect(&rect);

	//使截图成等比例缩放来适应客户区大小
	double zoomFactor, widthFactor, heightFactor;
	widthFactor = rect.Width() / 1.0 / bmpInfo.bmWidth;
	heightFactor = rect.Height() / 1.0 / bmpInfo.bmHeight;
	zoomFactor = widthFactor < heightFactor ? widthFactor : heightFactor;

	CRect srcRect, destRect;
	if(zoomFactor >= 1){
		destRect.SetRect((rect.Width() - bmpInfo.bmWidth) / 2,
			(rect.Height() - bmpInfo.bmHeight) / 2,
			(rect.Width() + bmpInfo.bmWidth) / 2,
			(rect.Height() + bmpInfo.bmHeight) / 2);

		srcRect.SetRect(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight);
	}else{
		if(widthFactor < heightFactor){
			destRect.SetRect(
				0,
				(rect.Height() - std::floor(bmpInfo.bmHeight * zoomFactor)) / 2,
				rect.Width(),
				(rect.Height() + std::floor(bmpInfo.bmHeight * zoomFactor)) / 2);
			srcRect.SetRect(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight);
		}else{
			destRect.SetRect(
				(rect.Width() - std::floor(bmpInfo.bmWidth * zoomFactor)) / 2,
				0,
				(rect.Width() + std::floor(bmpInfo.bmWidth * zoomFactor)) / 2,
				rect.Height());
			srcRect.SetRect(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight);
		}
	}
	//绘制截图到客户区
	pDC->StretchBlt(
					destRect.left,
					destRect.top,
					destRect.Width(),
					destRect.Height(),
					&dcMem,
					srcRect.left,
					srcRect.top,
					srcRect.Width(),
					srcRect.Height(),
					SRCCOPY);
}


// 截取屏幕部分区域
void CscreenshotView::CutScreenArea(CRect& Rect)
{	
	CDC *pDesktopDC=GetDesktopWindow()->GetDC();

	CRect RectDesktop;
	GetDesktopWindow()->GetClientRect(&RectDesktop);
	CBitmap bmpdesktop;
	bmpdesktop.CreateCompatibleBitmap(pDesktopDC, Rect.Width(), Rect.Height());

	DrawClientBackground(GetDC());

	CDC memDC;
	memDC.CreateCompatibleDC(pDesktopDC);
	memDC.SelectObject(&bmpdesktop);
	memDC.StretchBlt(
					0,
					0,
					Rect.Width(),
					Rect.Height(),
					pDesktopDC,
					Rect.left,
					Rect.top,
					Rect.Width(),
					Rect.Height(),
					SRCCOPY);

	//临时存储截图到CscreenshotDoc对象中
	CscreenshotDoc* pDoc = GetDocument();
	pDoc->copyScreenShotImage(&memDC, bmpdesktop);

	/*if(pDoc->hasScreenShotImage()){
		CBitmap *screenShotImage = pDoc->GetScreenShotImage();
		DrawImageOnClient(*screenShotImage);
	}*/
}

void CscreenshotView::OnAreaShot()
{
	// TODO: Add your command handler code here
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	CCutScreenAreaDlg dlg;
	dlg.DoModal();
	CRect rect;
	rect.left = dlg.StartPoint.x < dlg.EndPoint.x ? dlg.StartPoint.x : dlg.EndPoint.x;
	rect.right = dlg.StartPoint.x > dlg.EndPoint.x ? dlg.StartPoint.x : dlg.EndPoint.x;
	rect.top = dlg.StartPoint.y < dlg.EndPoint.y ? dlg.StartPoint.y : dlg.EndPoint.y;
	rect.bottom = dlg.StartPoint.y > dlg.EndPoint.y ? dlg.StartPoint.y : dlg.EndPoint.y;
	CutScreenArea(rect);

	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}


void CscreenshotView::OnUpdateSaveShot(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CscreenshotDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->hasScreenShotImage());
}


void CscreenshotView::OnSaveShot()
{
	// TODO: Add your command handler code here
	CscreenshotDoc* pDoc = GetDocument();
	CBitmap *screenShotImage = pDoc->GetScreenShotImage();

	CDC bmemDC;
	bmemDC.CreateCompatibleDC(NULL);
	bmemDC.SelectObject(screenShotImage);

	//将位图信息填充到一个缓冲区内
	BITMAP mbitmap;
	screenShotImage->GetBitmap(&mbitmap);
	BITMAPINFOHEADER bih = {0};//位图信息头
	bih.biBitCount = mbitmap.bmBitsPixel;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = mbitmap.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = mbitmap.bmWidthBytes * mbitmap.bmHeight;//图像数据大小
	bih.biWidth = mbitmap.bmWidth;//宽度
 
	BITMAPFILEHEADER bfh = {0};//位图文件头
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
	bfh.bfSize = bfh.bfOffBits + mbitmap.bmWidthBytes * mbitmap.bmHeight;//文件总的大小
	bfh.bfType = (WORD)0x4d42;
	byte * p = new byte[mbitmap.bmWidthBytes * mbitmap.bmHeight];//申请内存保存位图数据
	GetDIBits(bmemDC.m_hDC, (HBITMAP) screenShotImage->m_hObject, 0, mbitmap.bmHeight, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//获取位图数据

	//文件保存对话框，保存文件
	CString strFileName,strszFilter;
	strszFilter="位图文件(*.bmp)|*.bmp|全部文件(*.*)|*.*||";
	CFileDialog bmpdlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strszFilter,NULL);
	if(IDOK == bmpdlg.DoModal())
	{
		strFileName = bmpdlg.GetFileName();
		strFileName +=".bmp";

		//注意：以下n和len的值大小不同,n是按字符计算的，len是按字节计算的  
		int n = strFileName.GetLength();
		//获取宽字节字符的大小，大小是按字节计算的  
		int len = WideCharToMultiByte(CP_ACP,0,strFileName,strFileName.GetLength(),NULL,0,NULL,NULL);  
		//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小  
		char filename[MAX_PATH];   //以字节为单位  
		//宽字节编码转换成多字节编码  
		WideCharToMultiByte(CP_ACP,0,strFileName,strFileName.GetLength(),filename,len,NULL,NULL);  
		filename[len+1] = '\0';   //多字节字符以'\0'结束

		/*USES_CONVERSION;
		char *filename = T2A(strFileName);*/

		FILE *fp = fopen(filename, "w+b");
		fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//写入位图文件头 
		fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//写入位图信息头
		fwrite(p, 1, mbitmap.bmWidthBytes * mbitmap.bmHeight, fp);//写入位图数据
		fclose(fp);
	}
	delete [] p;
}
