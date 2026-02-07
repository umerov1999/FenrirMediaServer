// XTPMarkupInline.h: interface for the CXTPMarkupInline class.
//
// (c)1998-2023 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPINLINE_H__)
#	define __XTPMARKUPINLINE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	define MARKUP_POSITION_EOF (POSITION)(-1)
#	define MARKUP_POSITION_START (POSITION)(0)
#	define MARKUP_POSITION_END (POSITION)(1)

class CXTPMarkupDependencyProperty;
class CXTPMarkupTextBlock;
class CXTPMarkupDrawingContext;
class CXTPMarkupInline;
struct XTPMarkupFontMetrics;

class _XTP_EXT_CLASS CXTPMarkupInline : public CXTPMarkupTextElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupInline)

protected:
	CXTPMarkupInline();
	virtual ~CXTPMarkupInline();

public:
	virtual CXTPMarkupInline* GetFirstInline() const;
	CXTPMarkupInline* GetNextInline() const;

public:
	XTPMarkupBaselineAlignment GetBaselineAlignment() const;
	void SetBaselineAlignment(XTPMarkupBaselineAlignment nBaselineAlignment);

public:
	virtual POSITION GetContentStartPosition() const;
	virtual void GetContentNextPosition(POSITION& pos) const;
	virtual BOOL IsWordBreakPosition(POSITION pos) const;
	virtual BOOL IsLineBreakPosition(POSITION pos) const;
	virtual BOOL IsCaretReturnPosition(POSITION pos) const;
	virtual BOOL IsWhiteSpacePosition(POSITION pos) const;

	virtual void Arrange(CRect rcFinalRect, POSITION posStart, POSITION posEnd);
	virtual CSize Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart, POSITION posEnd);
	virtual void Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart, POSITION posEnd,
						 CXTPSizeF& size);
	virtual void Render(CXTPMarkupDrawingContext* pDC, CRect rc, POSITION posStart,
						POSITION posEnd);

	virtual void PrepareMeasure(CXTPMarkupDrawingContext* pDC);

	virtual int GetBaseline() const;
	virtual const XTPMarkupFontMetrics* GetFontMetrics() const;

	CXTPMarkupTextBlock* GetTextBlock() const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupInline);
#	endif

	afx_msg int OleGetBaselineAlignment();
	afx_msg void OleSetBaselineAlignment(int nValue);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	int m_nIndex;

public:
	static CXTPMarkupDependencyProperty* m_pBaselineAlignmentProperty;

private:
	friend class CXTPMarkupInlineCollection;
};

AFX_INLINE void CXTPMarkupInline::SetBaselineAlignment(XTPMarkupBaselineAlignment nBaselineAlignment)
{
	SetValue(m_pBaselineAlignmentProperty, CXTPMarkupEnum::CreateValue(nBaselineAlignment));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPINLINE_H__)
