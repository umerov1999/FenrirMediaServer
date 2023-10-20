// XTPWnd.h
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
#if !defined(__XTPWND_H__)
#	define __XTPWND_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPScrollBarCtrl;

class _XTP_EXT_CLASS CXTPWnd : public CWnd
{
	DECLARE_DYNAMIC(CXTPWnd)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPWnd object
	//-----------------------------------------------------------------------
	CXTPWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPWnd object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPWnd();

public:
	//{{AFX_VIRTUAL(CXTPWnd)
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CXTPWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//}}AFX_MSG

protected:
	void SetScrollBarTheme(XTPScrollBarTheme theme);

	BOOL m_bEnableThemedScrollBar;

	CXTPScrollBarCtrl* m_pwndScrollBar[2];
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPWND_H__)
