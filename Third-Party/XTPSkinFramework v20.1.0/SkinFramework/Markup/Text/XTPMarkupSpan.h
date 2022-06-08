// XTPMarkupSpan.h: interface for the CXTPMarkupSpan class.
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
#if !defined(__XTPMARKUPSPAN_H__)
#	define __XTPMARKUPSPAN_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupInlineCollection;
class CXTPMarkupInline;
class CXTPMarkupBuilder;
class CXTPMarkupObject;
class CXTPMarkupInlineCollection;

class _XTP_EXT_CLASS CXTPMarkupSpan : public CXTPMarkupInline
{
	DECLARE_MARKUPCLASS(CXTPMarkupSpan)

protected:
	CXTPMarkupSpan();
	virtual ~CXTPMarkupSpan();

public:
	CXTPMarkupInlineCollection* GetInlines() const;

public:
	virtual CXTPMarkupInline* GetFirstInline() const;

protected:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;
	virtual BOOL AllowWhiteSpaceContent() const;
	virtual int GetLogicalChildrenCount() const;
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupSpan);
#	endif

	afx_msg LPDISPATCH OleGetInlines();

	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPMarkupInlineCollection* m_pInlines;
};

AFX_INLINE int CXTPMarkupSpan::GetLogicalChildrenCount() const
{
	return m_pInlines ? 1 : 0;
}

AFX_INLINE CXTPMarkupObject* CXTPMarkupSpan::GetLogicalChild(int nIndex) const
{
	return nIndex == 0 ? (CXTPMarkupObject*)m_pInlines : NULL;
}

AFX_INLINE CXTPMarkupInlineCollection* CXTPMarkupSpan::GetInlines() const
{
	return m_pInlines;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSPAN_H__)
