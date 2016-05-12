
// CarpetControlView.h : CCarpetControlView ��Ľӿ�
//

#pragma once


class CCarpetControlView : public CView
{
protected: // �������л�����
	CCarpetControlView();
	DECLARE_DYNCREATE(CCarpetControlView)

// ����
public:
	CCarpetControlDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CCarpetControlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // CarpetControlView.cpp �еĵ��԰汾
inline CCarpetControlDoc* CCarpetControlView::GetDocument() const
   { return reinterpret_cast<CCarpetControlDoc*>(m_pDocument); }
#endif

