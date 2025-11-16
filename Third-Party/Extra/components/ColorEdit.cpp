#include "ColorEdit.h"
#include <iostream>
#include <vector>
#include <string>
#include "afxdialogex.h"
#include "WSTRUtils.h"
#include "win_api_utils.h"
#include "base.hpp"
using namespace std;
using namespace WSTRUtils;
#define SCROLL_SIZE 12
IMPLEMENT_DYNAMIC(ColorEdit, CStatic)

#define IDC_POPUP_STOP_AUTOSCROLL       15880
#define IDC_POPUP_SELECT_TEXT           15881
#define IDC_POPUP_STYLE                 15882
#define IDC_POPUP_FULL_TEXT             15883
#define IDC_MESSAGE						15884

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
	void AddColorFragment(const ColorFragment &Fragment) {
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
			else
			{
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

ColorEdit::ColorEdit() {
	Inited = false;
	LinesChenged = false;
	ScrollChenged = false;
	AutoScroll = true;
	FullText = false;
	TextAlign = Align::CENTER_ALIGN;
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

CRect ColorEdit::getRect() {
	CRect backgr;
	GetClientRect(&backgr);
	backgr.bottom -= SCROLL_SIZE;
	backgr.right -= SCROLL_SIZE;
	return backgr;
}

void ColorEdit::Init(Align TextAlign, HBITMAP pBackgroundPicture, bool isFullTextMode) {
	if (Inited) {
		return;
	}
	m_pFont.Attach((HFONT)GetParent()->SendMessageW(WM_GETFONT, 0, 0));
	this->TextAlign = TextAlign;
	this->FullText = isFullTextMode;

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
	Inited = true;
	LinesChenged = true;

	InvalidateRect(getRect());
	UpdateWindow();
}

void ColorEdit::SwitchBackground(HBITMAP pBackgroundPicture) {
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

void ColorEdit::RegisterSpecialPatternOnce(const wstring &Pattern, URGB Color) {
	THREAD_ACCESS_LOCK(Async, &Lines);
	OnceSpecials[Pattern] = Color;
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	LinesChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void ColorEdit::UnRegisterSpecialPatternOnce(const wstring& Pattern) {
	THREAD_ACCESS_LOCK(Async, &Lines);
	OnceSpecials.erase(Pattern);
	LinesChenged = true;
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	InvalidateRect(getRect());
	UpdateWindow();
}

void ColorEdit::ClearSpecialPatternOnce() {
	THREAD_ACCESS_LOCK(Async, &Lines);
	OnceSpecials.clear();
	LinesChenged = true;
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	InvalidateRect(getRect());
	UpdateWindow();
}

ColorEdit::~ColorEdit() {
	Inited = false;
	LinesChenged = false;
	ScrollChenged = false;
	FullText = false;
	AutoScroll = true;
	TextAlign = Align::CENTER_ALIGN;
}


BEGIN_MESSAGE_MAP(ColorEdit, CStatic)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL ColorEdit::OnEraseBkgnd(CDC* pMsg) {
	return TRUE;
}

static TextWrite Base91Decode(const string &str) {
	TextWrite Ret;
	string hh = base91::decode(str.substr(0, 9));
	memcpy(&Ret, hh.data(), sizeof(TextWrite));
	return Ret;
}

void ColorEdit::AddLines(const string &Text, bool stripNewLines) {
	if (!Inited) {
		return;
	}
	string Txt = Text;
	if (stripNewLines) {
		size_t found = string::npos;
		do {
			found = Txt.find('\r');
			if (found == string::npos)
				found = Txt.find('\n');
			if (found != string::npos)
				Txt.erase(found, 1);
		} while (found != string::npos);
	}
	while (Txt.length() > 0) {
		TextWrite info = Base91Decode(Txt);
		string Line = Txt.substr(10, info.Length);
		if (Txt.length() <= (size_t)(10 + info.Length)) {
			Txt.clear();
		}
		else {
			Txt = Txt.substr(10 + info.Length);
		}
		THREAD_ACCESS_LOCK(Async, &Lines);

		if (!stripNewLines) {
			string flushh;
			for (auto& i : Line)
			{
				if (i == '\r') {
					continue;
				}
				if (i == '\n') {
					Lines.push_back(ColoredMessage(UTF8_to_wchar(flushh), info.Color));
					flushh = "";
				}
				else {
					flushh.push_back(i);
				}
			}
			if (flushh.length() > 0) {
				Lines.push_back(ColoredMessage(UTF8_to_wchar(flushh), info.Color));
			}
		}
		else {
			Lines.push_back(ColoredMessage(UTF8_to_wchar(Line), info.Color));
		}

		THREAD_ACCESS_UNLOCK(Async, &Lines);
	}
	LinesChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void ColorEdit::RemoveLastLine() {
	if (!Inited) {
		return;
	}
	THREAD_ACCESS_LOCK(Async, &Lines);
	if (Lines.size() > 0) {
		Lines.pop_back();
	}
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	LinesChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void ColorEdit::RemoveAllLines() {
	if (!Inited) {
		return;
	}
	THREAD_ACCESS_LOCK(Async, &Lines);
	Lines.clear();
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	LinesChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
}

void ColorEdit::OnRButtonDown(UINT nFlags, CPoint point) {
	if (!Inited) {
		return;
	}
	RECT rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	HMENU hPopupMenu = CreatePopupMenu();
	int indexMenu = 0;
	InsertMenuW(hPopupMenu, indexMenu++, MF_BYPOSITION | MF_STRING, IDC_POPUP_SELECT_TEXT, L"Выбрать текст");
	InsertMenuW(hPopupMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
	InsertMenuW(hPopupMenu, indexMenu++, MF_BYPOSITION | (!FullText ? MF_UNCHECKED : MF_CHECKED), IDC_POPUP_FULL_TEXT, L"Показывать весь текст");
	InsertMenuW(hPopupMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
	InsertMenuW(hPopupMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
	InsertMenuW(hPopupMenu, indexMenu++, MF_BYPOSITION | MF_STRING, IDC_POPUP_STYLE, TextAlign == Align::CENTER_ALIGN ? L"Текст влево" : L"Текст по центру");
	InsertMenuW(hPopupMenu, indexMenu++, MF_BYPOSITION | (AutoScroll ? MF_UNCHECKED : MF_CHECKED), IDC_POPUP_STOP_AUTOSCROLL, L"Остановить прокрутку");
	TrackPopupMenu(hPopupMenu, TPM_RIGHTALIGN | TPM_RIGHTBUTTON, rc.left + point.x, rc.top + point.y, 0, m_hWnd, NULL);
	DestroyMenu(hPopupMenu);
	CStatic::OnRButtonDown(nFlags, point);
}

BOOL ColorEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	int yy = 1;
	if (zDelta < 0)
		yy = -1;
	INT MIN, MAX;
	Scroll.GetScrollRange(&MIN, &MAX);
	if (Scroll.GetScrollPos() - yy > MAX || Scroll.GetScrollPos() - yy < MIN) {
		return CStatic::OnMouseWheel(nFlags, zDelta, pt);
	}
	Scroll.SetScrollPos(Scroll.GetScrollPos() - yy);
	ScrollChenged = true;
	InvalidateRect(getRect());
	UpdateWindow();
	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void ColorEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
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

void ColorEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
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

std::vector<ColoredMessage> ColorEdit::GetRenderedLines() {
	std::vector<ColoredMessage> Data;
	if (!Inited) {
		return Data;
	}
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	Data = RenderedLines;
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
	return Data;
}

ColoredMessage ColorEdit::GetLastLine() {
	ColoredMessage ret;
	if (!Inited) {
		return ret;
	}
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	int size = (int)Lines.size();
	if (size > 0) {
		ret = *Lines.rbegin();
	}
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
	return ret;
}

std::wstring ColorEdit::GetRenderedText() {
	std::wstring Dt = L"";
	if (!Inited) {
		return Dt;
	}
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	for (auto& i : RenderedLines) {
		Dt += (i.str + L"\r\n");
	}
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
	return Dt;
}

std::wstring ColorEdit::GetFullText() {
	if (!Inited) {
		return L"";
	}
	std::wstring Dt = L"";
	THREAD_ACCESS_LOCK(Async, &Lines);
	for (auto& i : Lines) {
		Dt += (i.str + L"\r\n");
	}
	THREAD_ACCESS_UNLOCK(Async, &Lines);
	return Dt;
}

static std::string char_to_hex(unsigned char i) {
	string ret;
	ret.resize(10);
	sprintf_s((char*)ret.data(), 10, "%02X", i);
	return ret.data();
}

void ColorEdit::OnPaint() {
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
	THREAD_ACCESS_LOCK(Async, &RenderedLines);
	if (LinesChenged || ScrollChenged) {
		THREAD_ACCESS_LOCK(Async, &Lines);
		RenderedLines.clear();
		if (!ScrollChenged) {
			if (Lines.size() <= 0) {
				SetRange(&Scroll, 0, 0, rect.Height() / tm.tmHeight);
				if (AutoScroll) {
					HorizScroll.SetScrollPos(0);
					Scroll.SetScrollPos(0);
				}
			}
			else {
				SetRange(&Scroll, 0, (int)(Lines.size()), rect.Height() / tm.tmHeight);
				if (AutoScroll) {
					HorizScroll.SetScrollPos(0);
					Scroll.SetScrollPos((int)(Lines.size()));
				}
			}
		}
		if (Lines.size() > 0) {
			for (size_t i = 0; i < min((size_t)(rect.Height() / tm.tmHeight), Lines.size() - Scroll.GetScrollPos()); i++) {
				RenderedLines.push_back(Lines[(int)i + Scroll.GetScrollPos()]);
			}
		}
		ScrollChenged = false;
		LinesChenged = false;
		THREAD_ACCESS_UNLOCK(Async, &Lines);
	}
	CRect recttemp = rect;
	if (RenderedLines.size() <= 0) {
		SetRange(&HorizScroll, 0, 0, rect.Width() / tm.tmHeight);
	}
	for (auto& i : RenderedLines) {
		if ((int)i.str.length() <= HorizScroll.GetScrollPos()) {
			recttemp.top += tm.tmHeight;
			continue;
		}
		wstring rs = i.str;
		INT hMin, hMax;
		HorizScroll.GetScrollRange(&hMin, &hMax);
		if (hMax < (int)rs.length()) {
			SetRange(&HorizScroll, 0, (int)rs.length(), rect.Width() / tm.tmHeight);
		}

		rs = rs.substr(HorizScroll.GetScrollPos());
		ColorFragmentLine Spec((int)rs.length());
		for (auto& s : OnceSpecials) {
			size_t SpecPos = 0;
			while (true) {
				SpecPos = rs.find(s.get_key(), SpecPos);
				if (SpecPos != wstring::npos)
				{
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
		if (TextAlign == Align::CENTER_ALIGN) {
			GetTextExtentPointW(dcMem.m_hDC, rs.c_str(), (int)rs.length(), &textsz);
			Mod.left = (int)(recttemp.Width() / 2) - (int)(textsz.cx / 2);
		}
		list<ColorFragment> Fragments = Spec.Flush(i.Color());
		for (auto& j : Fragments) {
			dcMem.SetTextColor(j.Color);
			wstring TempStr = rs.substr(j.StartOffset, (j.EndOffset - j.StartOffset) + 1);
			dcMem.DrawTextW(TempStr.c_str(), (int)TempStr.length(), &Mod, DT_LEFT);
			GetTextExtentPointW(dcMem.m_hDC, TempStr.c_str(), (int)TempStr.length(), &textsz);
			Mod.left += textsz.cx;
		}
		recttemp.top += tm.tmHeight;
	}
	pDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
	THREAD_ACCESS_UNLOCK(Async, &RenderedLines);
}

#pragma pack(push, 4)                 
static struct {
	DWORD  style;
	DWORD  dwExtendedStyle;
	WORD   ccontrols;
	short  x;
	short  y;
	short  cx;
	short  cy;
	WORD   menu;
	WORD   windowClass;
	WCHAR  wszTitle[1];
	short  pointsize;
	WCHAR  wszFont[29];
	struct {
		DWORD  style;
		DWORD  exStyle;
		short  x;
		short  y;
		short  cx;
		short  cy;
		WORD   id;
		WORD   sysClass;
		WORD   idClass;
		WCHAR  wszTitle[1];
		WORD   cbCreationData;
	} edit;

} g_TextDlgTemplate = {

DS_SETFONT | DS_MODALFRAME | DS_SETFOREGROUND | DS_CENTER | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
0x1,
1,
0, 0, 351, 211,
0,
0,
L"",
9,
L"Montserrat Alternates Light",

   {
   ES_LEFT | ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_TABSTOP,
   WS_EX_NOPARENTNOTIFY,
   0, 0, 351, 211,
   IDC_MESSAGE,
   0xFFFF, 0x0081,
   L"", 0,
   }
};
#pragma pack(pop)

class TXINDLG {
public:
	TXINDLG() {
		Style = 0;
	}
	DWORD Style;
	wstring TextU;
};

static INT_PTR WINAPI DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			break;
		case WM_INITDIALOG: {
			HWND Message = GetDlgItem(hWnd, IDC_MESSAGE);
			SetWindowTextW(hWnd, L"Текст");
			SetWindowLongW(Message, GWL_STYLE, ((TXINDLG*)lParam)->Style | ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_TABSTOP);
			SetWindowTextW(Message, ((TXINDLG*)lParam)->TextU.c_str());
			return FALSE;
		}
	}
	return FALSE;
}

DWORD WINAPI ColorEdit::ShowText(LPVOID param) {
	ColorEdit*FData = (ColorEdit*)param;
	TXINDLG TextU;
	if (FData->FullText) {
		TextU.TextU = FData->GetFullText();
	}
	else {
		TextU.TextU = FData->GetRenderedText();
	}
	if (FData->TextAlign == Align::CENTER_ALIGN) {
		TextU.Style = ES_CENTER;
	}
	else {
		TextU.Style = ES_LEFT;
	}

	HINSTANCE hinst = FData->m_hWnd ? (HINSTANCE)(LONG_PTR)GetWindowLongPtr(FData->m_hWnd, GWLP_HINSTANCE)
		: (HINSTANCE)GetModuleHandle(NULL);

	DialogBoxIndirectParamW(hinst, (LPCDLGTEMPLATEW)&g_TextDlgTemplate, FData->m_hWnd, DlgProc, (LPARAM)&TextU);
	return 0;
}

BOOL ColorEdit::PreTranslateMessage(MSG* pMsg) {
	if (!Inited) {
		return CStatic::PreTranslateMessage(pMsg);
	}
	if (pMsg->wParam == IDC_POPUP_STOP_AUTOSCROLL) {
		AutoScroll = !AutoScroll;
		if (AutoScroll) {
			ScrollChenged = true;
			INT MIN, MAX;
			Scroll.GetScrollRange(&MIN, &MAX);
			Scroll.SetScrollPos(MAX);
			InvalidateRect(getRect());
			UpdateWindow();
		}
	}
	if (pMsg->wParam == IDC_POPUP_SELECT_TEXT) {
		CreateThreadDetachedSimple(&ShowText, this);
	}
	if (pMsg->wParam == IDC_POPUP_STYLE) {
		if (TextAlign == Align::CENTER_ALIGN) {
			TextAlign = Align::LEFT_ALIGN;
		}
		else {
			TextAlign = Align::CENTER_ALIGN;
		}
		InvalidateRect(getRect());
		UpdateWindow();
	}
	if (pMsg->wParam == IDC_POPUP_FULL_TEXT) {
		FullText = !FullText;
	}
	return CStatic::PreTranslateMessage(pMsg);
}