
// CarpetControlView.cpp : CCarpetControlView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "CarpetControl.h"
#endif

#include "CarpetControlDoc.h"
#include "CarpetControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCarpetControlView

IMPLEMENT_DYNCREATE(CCarpetControlView, CView)

BEGIN_MESSAGE_MAP(CCarpetControlView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CCarpetControlView ����/����

CCarpetControlView::CCarpetControlView()
{

}

CCarpetControlView::~CCarpetControlView()
{
}

BOOL CCarpetControlView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

// CCarpetControlView ����

void CCarpetControlView::OnDraw(CDC* /*pDC*/)
{
	CCarpetControlDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

}

void CCarpetControlView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCarpetControlView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCarpetControlView ���

#ifdef _DEBUG
void CCarpetControlView::AssertValid() const
{
	CView::AssertValid();
}

void CCarpetControlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCarpetControlDoc* CCarpetControlView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCarpetControlDoc)));
	return (CCarpetControlDoc*)m_pDocument;
}
#endif //_DEBUG


// CCarpetControlView ��Ϣ�������
