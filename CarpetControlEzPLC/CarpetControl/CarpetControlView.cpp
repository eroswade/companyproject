
// CarpetControlView.cpp : CCarpetControlView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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

// CCarpetControlView 构造/析构

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

// CCarpetControlView 绘制

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


// CCarpetControlView 诊断

#ifdef _DEBUG
void CCarpetControlView::AssertValid() const
{
	CView::AssertValid();
}

void CCarpetControlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCarpetControlDoc* CCarpetControlView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCarpetControlDoc)));
	return (CCarpetControlDoc*)m_pDocument;
}
#endif //_DEBUG


// CCarpetControlView 消息处理程序
