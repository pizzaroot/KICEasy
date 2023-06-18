﻿
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once
#include <algorithm>
#include <list>
#include <string>
#include <cmath>

// CChildView 창

class CChildView : public CWnd
{
// 생성입니다.
public:
	CChildView();

// 특성입니다.
private:
	std::list<std::list<POINT>> curves;
	std::string curFunc;
	int funcCursor;
	bool invalid;

// 작업입니다.
public:

// 재정의입니다.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnInsertFunction();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	double CalculateExpression(std::string);
	double CalculateFunction(double);
};

