// XTPMarkupHyperlink.h: interface for the CXTPMarkupHyperlink class.
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
#if !defined(__XTPMARKUPHYPERLINK_H__)
#	define __XTPMARKUPHYPERLINK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupRoutedEvent;
class CXTPMarkupMouseButtonEventArgs;

class _XTP_EXT_CLASS CXTPMarkupHyperlink : public CXTPMarkupSpan
{
	DECLARE_MARKUPCLASS(CXTPMarkupHyperlink)

protected:
	CXTPMarkupHyperlink();

protected:
	virtual void OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e);

protected:
	virtual void OnClick();

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupHyperlink);
#	endif

	afx_msg LPDISPATCH OleGetClickEvent();

	//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bPressed;

public:
	static CXTPMarkupRoutedEvent* m_pClickEvent;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPHYPERLINK_H__)
