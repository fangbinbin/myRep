
// screenshotDoc.h : interface of the CscreenshotDoc class
//


#pragma once


class CscreenshotDoc : public CDocument
{
protected: // create from serialization only
	CscreenshotDoc();
	DECLARE_DYNCREATE(CscreenshotDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CscreenshotDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
private:
	CBitmap* m_screenShotImage;//�����ͼ
public:
	bool hasScreenShotImage(void);
	// //�����ͼ(from bitmap to bitmap)
	void copyScreenShotImage(CDC *pDC, CBitmap& srcBitmap);
	// �����ѱ���Ľ�ͼ����ָ��
	CBitmap* GetScreenShotImage(void);
};
