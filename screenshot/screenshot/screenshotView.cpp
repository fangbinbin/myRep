
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


//���Ƴ���ͻ�������

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

//��ͼ����ͼƬ�����ڳ���ͻ�����
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

	//��ʱ�洢��ͼ��CscreenshotDoc������
	CscreenshotDoc* pDoc = GetDocument();
	pDoc->copyScreenShotImage(&memDC, bmpdesktop);

	/*if(pDoc->hasScreenShotImage()){
		CBitmap *screenShotImage = pDoc->GetScreenShotImage();
		DrawImageOnClient(*screenShotImage);
	}*/

	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

//��bitmap�����ڿͻ����ϣ�pDCΪ��srcBitmap����ϵ��CDC
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

	//ʹ��ͼ�ɵȱ�����������Ӧ�ͻ�����С
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
	//���ƽ�ͼ���ͻ���
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


// ��ȡ��Ļ��������
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

	//��ʱ�洢��ͼ��CscreenshotDoc������
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

	//��λͼ��Ϣ��䵽һ����������
	BITMAP mbitmap;
	screenShotImage->GetBitmap(&mbitmap);
	BITMAPINFOHEADER bih = {0};//λͼ��Ϣͷ
	bih.biBitCount = mbitmap.bmBitsPixel;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = mbitmap.bmHeight;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = mbitmap.bmWidthBytes * mbitmap.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = mbitmap.bmWidth;//���
 
	BITMAPFILEHEADER bfh = {0};//λͼ�ļ�ͷ
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + mbitmap.bmWidthBytes * mbitmap.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD)0x4d42;
	byte * p = new byte[mbitmap.bmWidthBytes * mbitmap.bmHeight];//�����ڴ汣��λͼ����
	GetDIBits(bmemDC.m_hDC, (HBITMAP) screenShotImage->m_hObject, 0, mbitmap.bmHeight, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//��ȡλͼ����

	//�ļ�����Ի��򣬱����ļ�
	CString strFileName,strszFilter;
	strszFilter="λͼ�ļ�(*.bmp)|*.bmp|ȫ���ļ�(*.*)|*.*||";
	CFileDialog bmpdlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strszFilter,NULL);
	if(IDOK == bmpdlg.DoModal())
	{
		strFileName = bmpdlg.GetFileName();
		strFileName +=".bmp";

		//ע�⣺����n��len��ֵ��С��ͬ,n�ǰ��ַ�����ģ�len�ǰ��ֽڼ����  
		int n = strFileName.GetLength();
		//��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����  
		int len = WideCharToMultiByte(CP_ACP,0,strFileName,strFileName.GetLength(),NULL,0,NULL,NULL);  
		//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С  
		char filename[MAX_PATH];   //���ֽ�Ϊ��λ  
		//���ֽڱ���ת���ɶ��ֽڱ���  
		WideCharToMultiByte(CP_ACP,0,strFileName,strFileName.GetLength(),filename,len,NULL,NULL);  
		filename[len+1] = '\0';   //���ֽ��ַ���'\0'����

		/*USES_CONVERSION;
		char *filename = T2A(strFileName);*/

		FILE *fp = fopen(filename, "w+b");
		fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ 
		fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ
		fwrite(p, 1, mbitmap.bmWidthBytes * mbitmap.bmHeight, fp);//д��λͼ����
		fclose(fp);
	}
	delete [] p;
}
