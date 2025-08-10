#pragma once
#include <afxwin.h>
#include <iostream>
#include <vector>
#include "win_api_utils.h"
#include "urgb.h"
#include "Map.h"

class ColoredMessage {
public:
	ColoredMessage() {
		Color = URGB(0, 0, 0);
	}
	ColoredMessage(const std::wstring &Tstring, URGB TColor) {
		str = Tstring;
		Color = TColor;
	}
	std::wstring str;
	URGB Color;
};

enum class Align {
	LEFT_ALIGN,
	CENTER_ALIGN
};

#pragma pack(push,1)
class TextWrite {
public:
	TextWrite() {
		Color = URGB(0, 0, 0);
		Length = -1;
	}
	TextWrite(URGB PColor, unsigned int PLength) {
		Color = PColor;
		Length = PLength;
	}
	URGB Color;
	unsigned int Length;
};
#pragma pack(pop)

class ColorEdit : public CStatic {
	DECLARE_DYNAMIC(ColorEdit)

public:
	ColorEdit();
	virtual ~ColorEdit();

	afx_msg void AddLines(std::string Text);
	afx_msg void RemoveLastLine();
	afx_msg void RemoveAllLines();
	afx_msg std::vector<ColoredMessage> GetRenderedLines();
	afx_msg std::wstring GetRenderedText();
	afx_msg std::wstring GetFullText();
	afx_msg void Init(Align TextAlign, HBITMAP pBackgroundPicture = NULL, bool isFullTextMode = false);
	afx_msg void SwitchBackground(HBITMAP pBackgroundPicture = NULL);
	afx_msg void RegisterSpecialPatternOnce(const std::wstring &Pattern, URGB Color);
	afx_msg void UnRegisterSpecialPatternOnce(const std::wstring& Pattern);
	afx_msg void ClearSpecialPatternOnce();
	afx_msg Align GetTextAlign() const { return TextAlign; }
	afx_msg ColoredMessage GetLastLine();
	afx_msg CRect getRect();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pMsg);
private:
	static DWORD WINAPI ShowText(LPVOID param);
	std::vector<ColoredMessage> Lines;
	std::vector<ColoredMessage> RenderedLines;
	Map::Map<std::wstring, URGB>OnceSpecials;
	CFont m_pFont;
	CScrollBar Scroll;
	CScrollBar HorizScroll;
	bool LinesChenged;
	bool ScrollChenged;
	bool AutoScroll;
	bool FullText;
	CBitmap BackgroundPicture;
	bool Inited;
	Align TextAlign;
	THREAD_ACCESS_GUARD Async;
};