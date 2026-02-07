// XTPMarkupMouseEventArgs.h: interface for the CXTPMarkupMouseEventArgs class.
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
#if !defined(__XTPMMARKUPMOUSEEVENTARGS_H__)
#	define __XTPMMARKUPMOUSEEVENTARGS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupRoutedEvent;
class CXTPMarkupInputElement;

class _XTP_EXT_CLASS CXTPMarkupMouseEventArgs : public CXTPMarkupRoutedEventArgs
{
public:
	CXTPMarkupMouseEventArgs(CXTPMarkupRoutedEvent* pEvent);

public:
	CPoint GetPosition(CXTPMarkupInputElement* relativeTo);

public:
	HWND m_hWnd;
	CPoint m_point;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMMARKUPMOUSEEVENTARGS_H__)
