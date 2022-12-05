
// ProjectARBREView.h : interface of the CProjectARBREView class
//

#pragma once


class CProjectARBREView : public CView
{
protected: // create from serialization only
	CProjectARBREView() noexcept;
	DECLARE_DYNCREATE(CProjectARBREView)

// Attributes
public:
	CProjectARBREDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CProjectARBREView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in ProjectARBREView.cpp
inline CProjectARBREDoc* CProjectARBREView::GetDocument() const
   { return reinterpret_cast<CProjectARBREDoc*>(m_pDocument); }
#endif

