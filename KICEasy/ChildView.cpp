
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
	funcCursor = 0;
	invalid = false;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_INSERT_FUNCTION, &CChildView::OnInsertFunction)
	ON_WM_KEYDOWN()
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

	CFont expFont;
	expFont.CreatePointFont(200, L"Arial");
	memDC.SelectObject(&expFont);
	CString funcStr(("y = " + curFunc).c_str());
	memDC.TextOut(10, 10, funcStr);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
}



BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

double CChildView::CalculateExpression(std::string expr) {
	bool good = true;
	int pcnt = 0;
	for (auto& x : expr) {
		if (!('0' <= x && x <= '9' || x == '.')) good = false;
		if (x == '.') pcnt++;
	}
	if (good && pcnt <= 1) return std::stod('0' + expr);
	int cnt = 0;
	for (int i = (int)expr.size() - 1; i >= 0; i--) {
		if (expr[i] == ')') cnt++;
		else if (expr[i] == '(') cnt--;
		else if (cnt == 0 && (expr[i] == '+' || expr[i] == '-')) {
			std::string left = expr.substr(0, i), right = expr.substr(i + 1);
			if (expr[i] == '+') return CalculateExpression(left) + CalculateExpression(right);
			else return CalculateExpression(left) - CalculateExpression(right);
		}
	}
	if (cnt != 0) {
		invalid = true;
		return 1;
	}
	for (int i = (int)expr.size() - 1; i >= 0; i--) {
		if (expr[i] == ')') cnt++;
		else if (expr[i] == '(') cnt--;
		else if (cnt == 0 && (expr[i] == '*' || expr[i] == '/')) {
			std::string left = expr.substr(0, i), right = expr.substr(i + 1);
			if (expr[i] == '*') return CalculateExpression(left) * CalculateExpression(right);
			else {
				double denom = CalculateExpression(right);
				if (abs(denom) > 0.00001) return CalculateExpression(left) / denom;
				else {
					invalid = true;
					return 1;
				}
			}
		}
	}
	for (int i = (int)expr.size() - 1; i >= 0; i--) {
		if (expr[i] == ')') cnt++;
		else if (expr[i] == '(') cnt--;
		else if (cnt == 0 && expr[i] == '^') {
			std::string left = expr.substr(0, i), right = expr.substr(i + 1);
			double lval = CalculateExpression(left);
			double rval = CalculateExpression(right);
			if (lval < 0 && abs((int)rval - rval) > 0.00001) {
				invalid = true;
				return 1;
			}
			if (lval < 0) return std::pow(lval, (int)rval);
			else return std::pow(lval, rval);
		}
	}
	return CalculateExpression(expr.substr(1, (int)expr.size() - 2));
}

double CChildView::CalculateFunction(double i) {
	std::string expr1, expr2, expr;
	char prev = ' ';
	for (auto &x: curFunc) {
		if (prev >= '0' && prev <= '9' && x == 'x') expr1 += '*';
		expr1 += x;
		prev = x;
	}
	for (auto& x : expr1) {
		if (x == 'x') expr2 += '(' + std::to_string(i) + ')';
		else expr2 += x;
	}
	prev = ' ';
	for (auto& x : expr2) {
		if (prev == ')' && x == '(') expr += '*';
		expr += x;
		prev = x;
	}
	return CalculateExpression(expr);
}

void CChildView::OnInsertFunction()
{
	if (curFunc.empty()) return;
	std::list<POINT> curve;
	for (int i = -1000; i < 1000; i++) {
		invalid = false;
		double tmpy = CalculateFunction((double)i / 100) * 100;
		if (tmpy > 20000) tmpy = 20000;
		if (tmpy < -20000) tmpy = -20000;
		POINT p; p.x = i, p.y = tmpy;
		if (invalid) {
			curves.push_back(curve);
			curve.clear();
		}
		else {
			curve.push_back(p);
		}
	}
	curves.push_back(curve);
	curve.clear();
	Invalidate();
	curFunc = "";
	funcCursor = 0;
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case VK_LEFT:
		funcCursor = max(0, funcCursor - 1);
		break;
	case VK_RIGHT:
		funcCursor = min(curFunc.size(), funcCursor + 1);
		break;
	case VK_BACK:
		if (funcCursor > 0) {
			curFunc.erase(curFunc.begin() + (--funcCursor));
		}
		break;
	case 'X':
		curFunc.insert(curFunc.begin() + (funcCursor++), 'x');
		break;
	default:
		if ('0' <= nChar && nChar <= '9' && (~GetKeyState(VK_SHIFT) & 0x8000)) curFunc.insert(curFunc.begin() + (funcCursor++), nChar);
		else if (nChar == '6') curFunc.insert(curFunc.begin() + (funcCursor++), '^');
		else if (nChar == '8') curFunc.insert(curFunc.begin() + (funcCursor++), '*');
		else if (nChar == '9') curFunc.insert(curFunc.begin() + (funcCursor++), '(');
		else if (nChar == '0') curFunc.insert(curFunc.begin() + (funcCursor++), ')');
		if (nChar == 187 && (GetKeyState(VK_SHIFT) & 0x8000)) curFunc.insert(curFunc.begin() + (funcCursor++), '+');
		if (nChar == 189 && (~GetKeyState(VK_SHIFT) & 0x8000)) curFunc.insert(curFunc.begin() + (funcCursor++), '-');
		if (nChar == 191 && (~GetKeyState(VK_SHIFT) & 0x8000)) curFunc.insert(curFunc.begin() + (funcCursor++), '/');
		break;
	}
	Invalidate();
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
