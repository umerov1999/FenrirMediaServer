// XTPMarkupTextElement.h: interface for the CXTPMarkupTextElement class.
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
#if !defined(__XTPMARKUPTEXTELEMENT_H__)
#	define __XTPMARKUPTEXTELEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupBrush;
class CXTPMarkupDependencyProperty;

class _XTP_EXT_CLASS CXTPMarkupTextElement : public CXTPMarkupFrameworkContentElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupTextElement)

protected:
	CXTPMarkupTextElement();
	virtual ~CXTPMarkupTextElement();

public:
	void SetBackground(CXTPMarkupBrush* pBrush);
	void SetBackground(COLORREF clr);
	static void AFX_CDECL SetBackground(CXTPMarkupObject* pObject, CXTPMarkupBrush* pBrush);
	CXTPMarkupBrush* GetBackground() const;

	void SetForeground(CXTPMarkupBrush* pBrush);
	void SetForeground(COLORREF clr);
	static void AFX_CDECL SetForeground(CXTPMarkupObject* pObject, CXTPMarkupBrush* pBrush);
	CXTPMarkupBrush* GetForeground() const;

	void SetFontSize(int nFontSize);
	int GetFontSize() const;

	void SetFontWeight(int nFontWeight);
	int GetFontWeight() const;

	void SetFontFamily(LPCWSTR lpszFontFamily);
	LPCWSTR GetFontFamily() const;

	void SetFontStyle(int nFontStyle);
	int GetFontStyle() const;

	void SetTextDecorations(int nTextDecorations);
	int GetTextDecorations() const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupTextElement);
#	endif

	afx_msg long OleGetTextDecorations();
	afx_msg void OleSetTextDecorations(long nValue);
	afx_msg long OleGetFontSize();
	afx_msg void OleSetFontSize(long nValue);
	afx_msg long OleGetFontWeight();
	afx_msg void OleSetFontWeight(long nValue);
	afx_msg BSTR OleGetFontFamily();
	afx_msg void OleSetFontFamily(LPCTSTR lpszValue);
	afx_msg long OleGetFontStyle();
	afx_msg void OleSetFontStyle(long nValue);
	afx_msg LPDISPATCH OleGetBackground();
	afx_msg void OleSetBackground(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetForeground();
	afx_msg void OleSetForeground(LPDISPATCH lpBackground);
	afx_msg VARIANT OleGetTag();
	afx_msg void OleSetTag(const VARIANT& va);

	//}}AFX_CODEJOCK_PRIVATE

public:
	static CXTPMarkupDependencyProperty* m_pBackgroundProperty;
	static CXTPMarkupDependencyProperty* m_pForegroundProperty;

	static CXTPMarkupDependencyProperty* m_pFontSizeProperty;
	static CXTPMarkupDependencyProperty* m_pFontWeightProperty;
	static CXTPMarkupDependencyProperty* m_pFontFamilyProperty;
	static CXTPMarkupDependencyProperty* m_pFontStyleProperty;
	static CXTPMarkupDependencyProperty* m_pTextDecorationsProperty;
	static CXTPMarkupDependencyProperty* m_pFontQualityProperty;
	static CXTPMarkupDependencyProperty* m_pFontCharsetProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTEXTELEMENT_H__)
