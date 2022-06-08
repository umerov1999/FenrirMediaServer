// XTPMarkupRoutedEventArgs.h: interface for the CXTPMarkupRoutedEventArgs class.
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
#if !defined(__XTPMARKUPROUTEDEVENTARGS_H__)
#	define __XTPMARKUPROUTEDEVENTARGS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupRoutedEvent;
class CXTPMarkupObject;

class _XTP_EXT_CLASS CXTPMarkupRoutedEventArgs : public CXTPMarkupObject
{
public:
	CXTPMarkupRoutedEventArgs();
	CXTPMarkupRoutedEventArgs(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupObject* pSource);

public:
	CXTPMarkupRoutedEvent* GetEvent() const;

	BOOL IsHandled() const;
	void SetHandled();

	CXTPMarkupObject* GetSource() const;
	void SetSource(CXTPMarkupObject* pSource);

protected:
	CXTPMarkupRoutedEvent* m_pEvent;
	CXTPMarkupObject* m_pSource;
	BOOL m_bHandled;

	friend class CXTPMarkupInputElement;

	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupRoutedEventArgs);
#	endif

	afx_msg BOOL OleGetHandled();
	afx_msg void OleSetHandled(BOOL bHandled);
	afx_msg LPDISPATCH OleGetEvent();
	afx_msg LPDISPATCH OleGetSource();
};

AFX_INLINE CXTPMarkupRoutedEvent* CXTPMarkupRoutedEventArgs::GetEvent() const
{
	return m_pEvent;
}

AFX_INLINE BOOL CXTPMarkupRoutedEventArgs::IsHandled() const
{
	return m_bHandled;
}

AFX_INLINE void CXTPMarkupRoutedEventArgs::SetHandled()
{
	m_bHandled = TRUE;
}

AFX_INLINE CXTPMarkupObject* CXTPMarkupRoutedEventArgs::GetSource() const
{
	return m_pSource;
}

AFX_INLINE void CXTPMarkupRoutedEventArgs::SetSource(CXTPMarkupObject* pSource)
{
	m_pSource = pSource;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPROUTEDEVENTARGS_H__)
