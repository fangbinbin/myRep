
// screenshotView.h : interface of the CscreenshotView class
//

#pragma once


class CscreenshotView : public CView
{
protected: // create from serialization only
	CscreenshotView();
	DECLARE_DYNCREATE(CscreenshotView)

// Attributes
public:
	CscreenshotDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CscreenshotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	void DrawClientBackground(CDC* pDC);
public:
	afx_msg void OnScreenShot();
protected:
	//在程序客户区中绘制图片
	void DrawImageOnClient(CBitmap& srcBitmap);
	// 截取屏幕部分区域
	void CutScreenArea(CRect& Rect);
public:
	afx_msg void OnAreaShot();
	afx_msg void OnUpdateSaveShot(CCmdUI *pCmdUI);
	afx_msg void OnSaveShot();
};

#ifndef _DEBUG  // debug version in screenshotView.cpp
inline CscreenshotDoc* CscreenshotView::GetDocument() const
   { return reinterpret_cast<CscreenshotDoc*>(m_pDocument); }
#endif

