
// screenshotDoc.cpp : implementation of the CscreenshotDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "screenshot.h"
#endif

#include "screenshotDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CscreenshotDoc

IMPLEMENT_DYNCREATE(CscreenshotDoc, CDocument)

BEGIN_MESSAGE_MAP(CscreenshotDoc, CDocument)
END_MESSAGE_MAP()


// CscreenshotDoc construction/destruction

CscreenshotDoc::CscreenshotDoc()
	: m_screenShotImage(NULL)
{
	// TODO: add one-time construction code here

}

CscreenshotDoc::~CscreenshotDoc()
{
	if(hasScreenShotImage()){
		m_screenShotImage->DeleteObject();
		delete m_screenShotImage;
		m_screenShotImage = NULL;
	}
}

BOOL CscreenshotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CscreenshotDoc serialization

void CscreenshotDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CscreenshotDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CscreenshotDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CscreenshotDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CscreenshotDoc diagnostics

#ifdef _DEBUG
void CscreenshotDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CscreenshotDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// 判断已经存在截图
bool CscreenshotDoc::hasScreenShotImage(void)
{
	return m_screenShotImage != NULL;
}


//保存截图(from bitmap to bitmap)
void CscreenshotDoc::copyScreenShotImage(CDC *pDC, CBitmap& srcBitmap)
{
	if(hasScreenShotImage()){
		m_screenShotImage->DeleteObject();
		delete m_screenShotImage;
		m_screenShotImage = NULL;
	}

	BITMAP bmpInfo;
	srcBitmap.GetBitmap(&bmpInfo);
	m_screenShotImage = new CBitmap;
	m_screenShotImage->CreateBitmapIndirect(&bmpInfo);

	CDC destDC;
	destDC.CreateCompatibleDC(pDC);

	CBitmap *srcOldDC, *destOldDC;
	srcOldDC = pDC->SelectObject(&srcBitmap);
	destOldDC = destDC.SelectObject(m_screenShotImage);

	destDC.BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, pDC, 0, 0, SRCCOPY);

	pDC->SelectObject(srcOldDC);
	destDC.SelectObject(destOldDC);
	destDC.DeleteDC();

	 /*CClientDC MemDC(pParent);
     CDC showDC;
     showDC.CreateCompatibleDC(&MemDC);

     CBitmap *pOldBmp1 = MemDC.SelectObject(pSrcBmp);
     CBitmap *pOldBmp2 = showDC.SelectObject(pDesBmp);
     showDC.BitBlt(0, 0, rc.Width(), rc.Height(),/
                            &MemDC, rc.left, rc.top, SRCCOPY);

     MemDC.SelectObject(pOldBmp1);
     showDC.SelectObject(pOldBmp2);
     showDC.DeleteDC();*/
}


// 返回已保存的截图对象指针
CBitmap* CscreenshotDoc::GetScreenShotImage(void)
{
	if(!hasScreenShotImage())
		return NULL;

	return m_screenShotImage;
}
