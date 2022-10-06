#pragma once
#include <afxwin.h>
#include <iostream>
#include <vector>
#include "ThreadAccessGuard.h"
#include "urgb.h"
#include "Map.h"

struct RenderedLine
{
	RenderedLine(const std::wstring &Line, int Id)
	{
		this->Line = Line;
		this->Id = Id;
	}
	std::wstring Line;
	int Id;
};

struct CallBackSelectTouch
{
	CallBackSelectTouch(LPVOID Func, LPVOID Class)
	{
		this->Func = Func;
		this->Class = Class;
	}
	CallBackSelectTouch(LPVOID)
	{
		Func = NULL;
		Class = NULL;
	}
	CallBackSelectTouch()
	{
		Func = NULL;
		Class = NULL;
	}
	bool Unused() const
	{
		return Func == NULL;
	}
	LPVOID Func;
	LPVOID Class;
};

class UListBox : public CStatic
{
	DECLARE_DYNAMIC(UListBox)

public:
	UListBox();
	virtual ~UListBox();

	afx_msg void AddLine(const std::wstring &Line);
	afx_msg void UpdateLines();
	afx_msg void Init(HBITMAP HBackgr = NULL, CallBackSelectTouch OnSelect = NULL, CallBackSelectTouch OnTouch = NULL, bool resize = true);
	afx_msg void SwitchBackground(HBITMAP HBackgr = NULL, bool resize = true);
	afx_msg void RegisterSpecialPatternOnce(const std::wstring &Pattern, URGB Color);
	afx_msg void UnRegisterSpecialPatternOnce(const std::wstring& Pattern);
	afx_msg void ClearSpecialPatternOnce();
	afx_msg void Select(int Id);
	afx_msg void Clear();
	afx_msg int GetSelected() const { return SelectedId; }
	afx_msg int GetTouch() const { return TouchId; }
	afx_msg void SelectString(int StarIdx, const std::wstring &Line);
	afx_msg int GetCount();
	afx_msg std::wstring GetLine(int Idx);
	afx_msg std::wstring GetSelectedLine();
	afx_msg CRect getRect();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pMsg);
private:
	std::vector<std::wstring> Lines;
	std::vector<RenderedLine> RenderedLines;
	Map::Map<std::wstring, URGB>OnceSpecials;
	CFont m_pFont;
	CScrollBar Scroll;
	CScrollBar HorizScroll;
	bool LinesChenged;
	bool ScrollChenged;
	bool Inited;
	int SelectedId;
	int TouchId;
	int FontSizePX;
	CBitmap Background;
	THREAD_ACCESS_GUARD Async;

	CallBackSelectTouch OnSelect;
	CallBackSelectTouch OnTouch;
};


