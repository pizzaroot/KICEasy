
// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "KICEasy.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_INSERT_FUNCTION, &CChildView::OnInsertFunction)
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memDC.SelectObject(&bitmap);
	
	memDC.SelectStockObject(WHITE_PEN);
	memDC.Rectangle(0, 0, rect.Width(), rect.Height());

	memDC.SelectStockObject(BLACK_PEN);
	
	memDC.MoveTo(0, rect.Height() / 2);
	memDC.LineTo(rect.Width(), rect.Height() / 2);
	memDC.MoveTo(rect.Width() / 2, 0);
	memDC.LineTo(rect.Width() / 2, rect.Height());

	CPen graphPen(PS_SOLID, 2, RGB(0, 0, 0));
	memDC.SelectObject(&graphPen);

	for (auto &curve : curves) {
		bool skip = true;
		for (auto &point : curve) {
			if (!skip) memDC.LineTo(point.x + rect.Width() / 2, rect.Height() / 2 - point.y);
			else skip = false;
			memDC.MoveTo(point.x + rect.Width() / 2, rect.Height() / 2 - point.y);
		}
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
}



BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}


void CChildView::OnInsertFunction()
{
	std::list<POINT> curve;
	for (int i = 0; i < 10000; i++) {
		POINT p; p.x = i, p.y = sqrt((float)i / 100) * 100;
		curve.push_back(p);
	}
	curves.push_back(curve);
	Invalidate(0);
}
