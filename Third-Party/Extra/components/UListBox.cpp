#include "UListBox.h"
#include <iostream>
#include <vector>
#include <string>
#include "afxdialogex.h"
using namespace std;
#define SCROLL_SIZE 12

IMPLEMENT_DYNAMIC(UListBox, CStatic)

typedef void(*CallBackEvent)(LPVOID pClass, int Idx);

static UINT ON_TOUCH_EVENT = RegisterWindowMessageW(L"UListBox::OnTouchEvent");
static UINT ON_SELECT_EVENT = RegisterWindowMessageW(L"UListBox::OnSelectEvent");

class ColorFragment {
public:
	ColorFragment(DWORD PColor, int PStartOffset, int PEndOffset) {
		Color = PColor;
		StartOffset = PStartOffset;
		EndOffset = PEndOffset;
	}
	DWORD Color;
	int StartOffset;
	int EndOffset;
};

class ColorFragmentLine {
public:
	ColorFragmentLine(int PLength) {
		Length = PLength;
	}
	void AddColorFragment(ColorFragment Fragment) {
		Fragments.push_back(Fragment);
	}
	list<ColorFragment> Flush(DWORD Color) {
		if (Fragments.size() <= 0) {
			Fragments.push_back(ColorFragment(Color, 0, (Length - 1)));
			return Fragments;
		}
		Fragments.sort(sort_compare);
		for (auto i = Fragments.begin(); i != Fragments.end(); i++) {
			if (i == Fragments.begin()) {
				if ((*i).StartOffset > 0) {
					Fragments.insert(i, ColorFragment(Color, 0, (*i).StartOffset - 1));
				}
				continue;
			}
			else {
				auto s = i; s--;
				if ((*s).EndOffset < (*i).StartOffset - 1) {
					Fragments.insert(i, ColorFragment(Color, (*s).EndOffset + 1, (*i).StartOffset - 1));
				}
			}
		}
		if ((*Fragments.rbegin()).EndOffset < (Length - 1)) {
			Fragments.push_back(ColorFragment(Color, (*Fragments.rbegin()).EndOffset + 1, (Length - 1)));
		}
		return Fragments;
	}
private:
	static bool sort_compare(const ColorFragment& first, const ColorFragment& second) {
		return (first.EndOffset < second.EndOffset);
	}
	int Length;
	list<ColorFragment> Fragments;
};

UListBox::UListBox() {
	Inited = false;
	LinesChenged = false;
	ScrollChenged = false;
	SelectedId = -1;
	FontSizePX = -1;
	TouchId = -1;
	THREAD_ACCESS_REGISTER_POINTERS(Async, &Lines, &RenderedLines, &BackgroundPicture);
}

static void SetRange(CScrollBar* pScrollBar, int Min, int Max, int RenderedCount) {
	pScrollBar->SetScrollRange(Min, Max);
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&info);
	info.nPage = RenderedCount;
	info.nMax = Max;
	info.nMin = Min;
	pScrollBar->SetScrollInfo(&info);
}

CRect UListBox::getRect() {
	CRect backgr;
	GetClientRect(&backgr);
	backgr.bottom -= SCROLL_SIZE;
	backgr.right -= SCROLL_SIZE;
	return backgr;
}

void UListBox::Init(HBITMAP pBackgroundPicture, CallBackSelectTouch OnSelect, CallBackSelectTouch OnTouch) {
	if (Inited) {
		return;
	}
	m_pFont.Attach((HFONT)GetParent()->SendMessageW(WM_GETFONT, 0, 0));

	RECT scrollpos;
	GetClientRect(&scrollpos);
	scrollpos.left = scrollpos.right - SCROLL_SIZE;
	Scroll.Create(SBS_VERT | WS_VISIBLE | WS_CHILD, scrollpos, this, AFX_IDW_VSCROLL_FIRST);
	GetClientRect(&scrollpos);
	scrollpos.top = scrollpos.bottom - SCROLL_SIZE;
	scrollpos.right -= SCROLL_SIZE;
	HorizScroll.Create(SBS_HORZ | WS_VISIBLE | WS_CHILD, scrollpos, this, AFX_IDW_HSCROLL_FIRST);

	CRect backgr;
	GetClientRect(&backgr);
	if (pBackgroundPicture != NULL) {
		BackgroundPicture.Attach(pBackgroundPicture);
	}
	SetRange(&Scroll, 0, 0, 0);
	SetRange(&HorizScroll, 0, 0, 0);
	this->OnSelect = OnSelect;
	this->OnTouch = OnTouch;

	Inited = true;
	LinesChenged = true;

	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::SwitchBackground(HBITMAP pBackgroundPicture) {
	THREAD_ACCESS_LOCK(Async, &BackgroundPicture);
	BackgroundPicture.DeleteObject();
	CRect backgr;
	GetClientRect(&backgr);
	if (pBackgroundPicture != NULL) {
		BackgroundPicture.Attach(pBackgroundPicture);
	}
	THREAD_ACCESS_UNLOCK(Async, &BackgroundPicture);
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::RegisterSpecialPatternOnce(const wstring &Pattern, URGB Color) {
	THREAD_ACCESS_LOCK(Async, &Lines);
	OnceSpecials[Pattern] = Color;
	LinesChenged = true;
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::UnRegisterSpecialPatternOnce(const wstring& Pattern) {
	THREAD_ACCESS_LOCK(Async, &Lines);
	OnceSpecials.erase(Pattern);
	LinesChenged = true;
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::ClearSpecialPatternOnce() {
	THREAD_ACCESS_LOCK(Async, &Lines);
	OnceSpecials.clear();
	LinesChenged = true;
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	InvalidateRect(getRect());
	UpdateWindow();
}

UListBox::~UListBox() {
	Inited = false;
	LinesChenged = false;
	ScrollChenged = false;
	SelectedId = -1;
	FontSizePX = -1;
	TouchId = -1;
}


BEGIN_MESSAGE_MAP(UListBox, CStatic)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_REGISTERED_MESSAGE(ON_TOUCH_EVENT, OnTouchEventMessage)
	ON_REGISTERED_MESSAGE(ON_SELECT_EVENT, OnSelectEventMessage)
END_MESSAGE_MAP()

BOOL UListBox::OnEraseBkgnd(CDC* pMsg) {
	return TRUE;
}

void UListBox::AddLine(const std::wstring &Line) {
	if (!Inited || Line.length() <= 0) {
		return;
	}
	THREAD_ACCESS_LOCK(Async, &Lines);
	Lines.push_back(Line);
	THREAD_ACCESS_UNLOCK(Async, &Lines);
}

void UListBox::AddLines(const std::list<std::wstring>& listLines) {
	if (!Inited || listLines.size() <= 0) {
		return;
	}
	THREAD_ACCESS_LOCK(Async, &Lines);
	for (auto& i : listLines) {
		if (i.empty()) {
			continue;
		}
		Lines.push_back(i);
	}
	THREAD_ACCESS_UNLOCK(Async, &Lines);
}

void UListBox::UpdateLines() {
	LinesChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::OnLButtonDown(UINT nFlags, CPoint point) {
	if (!Inited || FontSizePX <= 0) {
		return CStatic::OnRButtonDown(nFlags, point);
	}
	int SelectedIdTemp = 0;
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	SelectedIdTemp = Scroll.GetScrollPos() + min((int)((float)point.y / FontSizePX), (int)RenderedLines.size() - 1);
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
	if (SelectedIdTemp != SelectedId) {
		Select(SelectedIdTemp);
	}
	return CStatic::OnRButtonDown(nFlags, point);
}

void UListBox::Clear() {
	SelectedId = -1;
	TouchId = -1;
	THREAD_ACCESS_LOCK(Async, &Lines, &RenderedLines);
	Lines.clear();
	RenderedLines.clear();
	LinesChenged = true;
	THREAD_ACCESS_UNLOCK(Async, &Lines, &RenderedLines);
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::Select(int Id) {
	THREAD_ACCESS_LOCK(Async, &Lines);
	if (Id < 0 || Id >(int)Lines.size()) {
		THREAD_ACCESS_VOID_RETURN(Async, &Lines);
	}
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	SelectedId = Id;
	bool SelHasRendered = false;
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	for (auto &i : RenderedLines) {
		if (i.Id == SelectedId) {
			SelHasRendered = true;
			break;
		}
	}
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
	if (!SelHasRendered) {
		Scroll.SetScrollPos(SelectedId, FALSE);
		ScrollChenged = true;
	}
	InvalidateRect(getRect());
	UpdateWindow();

	SendMessageW(ON_SELECT_EVENT, 0, Id);
}

std::wstring UListBox::GetLine(int Idx) {
	std::wstring Ret;
	THREAD_ACCESS_LOCK(Async, &Lines);
	Ret = Lines[Idx];
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	return Ret;
}

int UListBox::GetCount() {
	THREAD_ACCESS_LOCK(Async, &Lines);
	THREAD_ACCESS_RETURN(Async, (int)Lines.size(), &Lines);
}

void UListBox::SelectString(int StarIdx, const std::wstring &Line) {
	THREAD_ACCESS_LOCK(Async, &Lines);
	for (int i = StarIdx; i < (int)Lines.size(); i++) {
		if (Lines[i] == Line) {
			THREAD_ACCESS_RETURN(Async, Select(i), &Lines);
		}
	}
	THREAD_ACCESS_UNLOCK(Async, &Lines);
}

std::wstring UListBox::GetSelectedLine() {
	if (SelectedId == -1) {
		return L"";
	}
	std::wstring Ret;
	THREAD_ACCESS_LOCK(Async, &Lines);
	Ret = Lines[SelectedId];
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	return Ret;
}

void UListBox::OnMouseMove(UINT nFlags, CPoint point) {
	if (!Inited || FontSizePX <= 0) {
		return CStatic::OnMouseMove(nFlags, point);
	}
	int SelectedIdTemp = 0;
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	SelectedIdTemp = Scroll.GetScrollPos() + min((int)((float)point.y / FontSizePX), (int)RenderedLines.size() - 1);
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
	if (SelectedIdTemp != TouchId) {
		TouchId = SelectedIdTemp;
		InvalidateRect(getRect());
		UpdateWindow();

		SendMessageW(ON_TOUCH_EVENT, 0, TouchId);
	}
	return CStatic::OnMouseMove(nFlags, point);
}

BOOL UListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint point) {
	int yy = 1;
	if (zDelta < 0) {
		yy = -1;
	}
	INT MIN, MAX;
	Scroll.GetScrollRange(&MIN, &MAX);
	if (Scroll.GetScrollPos() - yy > MAX || Scroll.GetScrollPos() - yy < MIN) {
		return CStatic::OnMouseWheel(nFlags, zDelta, point);
	}
	Scroll.SetScrollPos(Scroll.GetScrollPos() - yy);
	ScrollChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
	return CStatic::OnMouseWheel(nFlags, zDelta, point);
}

void UListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (!Inited || pScrollBar != &Scroll) {
		return;
	}

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION) {
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_TRACKPOS;
		pScrollBar->GetScrollInfo(&info);
		nPos = info.nTrackPos;
		pScrollBar->SetScrollPos(nPos, FALSE);
	}
	else if (nSBCode == SB_TOP) {
		INT Minp = 0, Maxp = 0;
		pScrollBar->GetScrollRange(&Minp, &Maxp);
		pScrollBar->SetScrollPos(Minp, FALSE);
	}
	else if (nSBCode == SB_BOTTOM) {
		INT Minp = 0, Maxp = 0;
		pScrollBar->GetScrollRange(&Minp, &Maxp);
		pScrollBar->SetScrollPos(Maxp, FALSE);
	}
	else if (nSBCode == SB_LINEUP) {
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 1, FALSE);
	}
	else if (nSBCode == SB_LINEDOWN) {
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 1, FALSE);
	}
	ScrollChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (!Inited || pScrollBar != &HorizScroll) {
		return;
	}

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION) {
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_TRACKPOS;
		pScrollBar->GetScrollInfo(&info);
		nPos = info.nTrackPos;
		pScrollBar->SetScrollPos(nPos, FALSE);
	}
	else if (nSBCode == SB_LEFT) {
		INT Minp = 0, Maxp = 0;
		pScrollBar->GetScrollRange(&Minp, &Maxp);
		pScrollBar->SetScrollPos(Minp, FALSE);
	}
	else if (nSBCode == SB_RIGHT) {
		INT Minp = 0, Maxp = 0;
		pScrollBar->GetScrollRange(&Minp, &Maxp);
		pScrollBar->SetScrollPos(Maxp, FALSE);
	}
	else if (nSBCode == SB_LINELEFT) {
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 1, FALSE);
	}
	else if (nSBCode == SB_LINERIGHT) {
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 1, FALSE);
	}
	ScrollChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void UListBox::OnPaint() {
	CRect rect;
	GetClientRect(&rect);
	rect.bottom -= SCROLL_SIZE;
	rect.right -= SCROLL_SIZE;

	CPaintDC pDC(this);

	if (!Inited) {
		CBrush brush(GetSysColor(COLOR_3DFACE));
		pDC.FillRect(&rect, &brush);
		return;
	}
	CDC dcMem;
	dcMem.CreateCompatibleDC(&pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&pDC, rect.Width(), rect.Height());
	dcMem.SetMapMode(GetMapMode(pDC));
	dcMem.SetBkMode(TRANSPARENT);

	THREAD_ACCESS_LOCK(Async, &BackgroundPicture);
	if (BackgroundPicture.m_hObject) {
		bitmap.DeleteObject();
		bitmap.Attach((HBITMAP)CopyImage(BackgroundPicture, IMAGE_BITMAP, rect.Width(), rect.Height(), 0));
		dcMem.SelectObject(&bitmap);
	}
	else {
		dcMem.SelectObject(&bitmap);
		CBrush brush(GetSysColor(COLOR_3DFACE));
		dcMem.FillRect(&rect, &brush);
	}
	THREAD_ACCESS_UNLOCK(Async, &BackgroundPicture);
	dcMem.SelectObject(m_pFont);
	TEXTMETRICW tm;
	dcMem.GetTextMetricsW(&tm);
	if (tm.tmHeight > 0) {
		FontSizePX = tm.tmHeight;
	}
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	if (LinesChenged || ScrollChenged) {
		THREAD_ACCESS_LOCK(Async, &Lines);
		RenderedLines.clear();
		if (!ScrollChenged && FontSizePX > 0) {
			if (Lines.size() <= 0) {
				SetRange(&Scroll, 0, 0, rect.Height() / FontSizePX);
			}
			else {
				SetRange(&Scroll, 0, (int)(Lines.size()), rect.Height() / FontSizePX);
			}
		}
		if (Lines.size() > 0) {
			for (size_t i = 0; i < min((size_t)(rect.Height() / std::max(FontSizePX, 1)), Lines.size() - Scroll.GetScrollPos()); i++) {
				RenderedLines.push_back(RenderedLine(Lines[(int)i + Scroll.GetScrollPos()], (int)i + Scroll.GetScrollPos()));
			}
		}
		ScrollChenged = false;
		LinesChenged = false;
		THREAD_ACCESS_UNLOCK(Async, &Lines);
	}
	CRect recttemp = rect;
	if (RenderedLines.size() <= 0 && FontSizePX > 0) {
		SetRange(&HorizScroll, 0, 0, rect.Width() / FontSizePX);
	}
	for (auto &i : RenderedLines) {
		if ((int)i.Line.length() <= HorizScroll.GetScrollPos()) {
			recttemp.top += std::max(FontSizePX, 1);
			continue;
		}
		wstring rs = i.Line;
		DWORD OrignColor = RGB(255, 255, 255);
		if (SelectedId == i.Id) {
			OrignColor = RGB(255, 0, 0);
			CPen Poly(PS_SOLID, 1, RGB(255,0,0));
			dcMem.SelectObject(Poly);
			dcMem.MoveTo(0, recttemp.top);
			dcMem.LineTo(recttemp.Width(), recttemp.top);
			dcMem.MoveTo(0, recttemp.top + std::max(FontSizePX, 1));
			dcMem.LineTo(recttemp.Width(), recttemp.top + std::max(FontSizePX, 1));
		}
		else {
			if (TouchId == i.Id) {
				CPen Poly(PS_SOLID, 1, RGB(100, 100, 100));
				dcMem.SelectObject(Poly);
				dcMem.MoveTo(0, recttemp.top);
				dcMem.LineTo(recttemp.Width(), recttemp.top);
				dcMem.MoveTo(0, recttemp.top + std::max(FontSizePX, 1));
				dcMem.LineTo(recttemp.Width(), recttemp.top + std::max(FontSizePX, 1));
			}
		}
		INT hMin, hMax;
		HorizScroll.GetScrollRange(&hMin, &hMax);
		if (hMax < (int)rs.length() && FontSizePX > 0) {
			SetRange(&HorizScroll, 0, (int)rs.length(), rect.Width() / FontSizePX);
		}

		rs = rs.substr(HorizScroll.GetScrollPos());
		ColorFragmentLine Spec((int)rs.length());
		for (auto& s : OnceSpecials) {
			size_t SpecPos = 0;
			while (true) {
				SpecPos = rs.find(s.get_key(), SpecPos);
				if (SpecPos != wstring::npos) {
					Spec.AddColorFragment(ColorFragment(s.get_value()(), (int)SpecPos, (int)(SpecPos + s.get_key().length() - 1)));
					SpecPos += s.get_key().length();
				}
				else {
					break;
				}
			}
		}

		SIZE textsz;
		CRect Mod = recttemp;
		list<ColorFragment> Fragments = Spec.Flush(OrignColor);
		for (auto& j : Fragments) {
			dcMem.SetTextColor(j.Color);
			wstring TempStr = rs.substr(j.StartOffset, (j.EndOffset - j.StartOffset) + 1);
			dcMem.DrawTextW(TempStr.c_str(), (int)TempStr.length(), &Mod, DT_LEFT);
			GetTextExtentPointW(dcMem.m_hDC, TempStr.c_str(), (int)TempStr.length(), &textsz);
			Mod.left += textsz.cx;
		}
		recttemp.top += std::max(FontSizePX, 1);
	}
	pDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
}

LRESULT UListBox::OnTouchEventMessage(WPARAM wParam, LPARAM lParam) {
	if (!OnTouch.Unused()) {
		((CallBackEvent)OnTouch.Func)(OnTouch.Class, (int)lParam);
	}
	return 0;
}

LRESULT UListBox::OnSelectEventMessage(WPARAM wParam, LPARAM lParam) {
	if (!OnSelect.Unused()) {
		((CallBackEvent)OnSelect.Func)(OnSelect.Class, (int)lParam);
	}
	return 0;
}