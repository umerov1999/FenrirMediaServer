// XTPMarkupRun.h: interface for the CXTPMarkupRun class.
//
// (c)1998-2021 Codejock Software, All Rights Reserved.
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
#if !defined(__XTPMARKUPRUN_H__)
#	define __XTPMARKUPRUN_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupFont;
class CXTPMarkupDrawingContext;
class CXTPMarkupBuilder;
class CXTPMarkupObject;
class CXTPMarkupString;
struct XTPMarkupFontMetrics;

class _XTP_EXT_CLASS CXTPMarkupRun : public CXTPMarkupInline
{
	DECLARE_MARKUPCLASS(CXTPMarkupRun)

protected:
	CXTPMarkupRun();
	virtual ~CXTPMarkupRun();

protected:
	virtual void OnFinalRelease();

public:
	CString GetText() const;
	LPCWSTR GetTextW() const;
	void SetText(LPCWSTR lpszText);
	void SetText(CXTPMarkupString* pText);

public:
	virtual POSITION GetContentStartPosition() const;
	virtual void GetContentNextPosition(POSITION& pos) const;
	virtual BOOL IsWordBreakPosition(POSITION pos) const;
	virtual BOOL IsLineBreakPosition(POSITION pos) const;
	virtual BOOL IsCaretReturnPosition(POSITION pos) const;
	virtual BOOL IsWhiteSpacePosition(POSITION pos) const;
	virtual CSize Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart, POSITION posEnd);
	virtual void Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart, POSITION posEnd,
						 CXTPSizeF& size);
	virtual void Render(CXTPMarkupDrawingContext* pDC, CRect rc, POSITION posStart,
						POSITION posEnd);

protected:
	virtual void GetLogFont(LOGFONT* lf) const;

protected:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;
	virtual void PrepareMeasure(CXTPMarkupDrawingContext* pDC);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupRun);
#	endif

	afx_msg VARIANT OleGetText();
	afx_msg void OleSetText(const VARIANT& lpCaption);

	//}}AFX_CODEJOCK_PRIVATE

protected:
	int m_nBaseline;
	virtual int GetBaseline() const;
	virtual const XTPMarkupFontMetrics* GetFontMetrics() const;

	CXTPMarkupFont* m_pFont;

private:
	XTPMarkupFontMetrics* m_pFontMetrics;

public:
	static CXTPMarkupDependencyProperty* m_pTextProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPRUN_H__)
