// XTPMarkupScrollViewer.h: interface for the CXTPMarkupScrollViewer class.
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
#if !defined(__XTPMARKUPSCROLLVIEWER_H__)
#	define __XTPMARKUPSCROLLVIEWER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupScrollBarGripper;
class CXTPMarkupScrollBar;
class CXTPMarkupVisual;
class CXTPMarkupDrawingContext;
class CXTPMarkupMouseWheelEventArgs;
class CXTPMarkupDependencyProperty;

enum XTPMarkupScrollBarVisibility
{
	xtpMarkupScrollBarDisabled,
	xtpMarkupScrollBarAuto,
	xtpMarkupScrollBarVisible
};

class _XTP_EXT_CLASS CXTPMarkupScrollViewer : public CXTPMarkupContentControl
{
	DECLARE_MARKUPCLASS(CXTPMarkupScrollViewer);

protected:
	CXTPMarkupScrollViewer();
	~CXTPMarkupScrollViewer();

public:
	int GetVisualChildrenCount() const;
	CXTPMarkupVisual* GetVisualChild(int nIndex) const;

	void Scroll(int nBar, int cmd, int pos);
	void SetScrollPos(int nBar, int pos);
	int GetScrollPos(int nBar);
	int GetScrollLimit(int nBar);

	XTPMarkupScrollBarVisibility GetVerticalScrollBarVisibility() const;
	XTPMarkupScrollBarVisibility GetHorizontalScrollBarVisibility() const;

protected:
	CSize ArrangeOverride(CSize szFinalSize);
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);

	void OnMouseWheel(CXTPMarkupMouseWheelEventArgs* e);

protected:
	void CreateScrollBar(int nBar);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupScrollViewer);
#	endif

	afx_msg long OleGetVerticalOffset();
	afx_msg long OleGetHorizontalOffset();
	afx_msg void OleSetVerticalOffset(int nOffset);
	afx_msg void OleSetHorizontalOffset(int nOffset);

	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPMarkupScrollBar* m_pVerticalScrollBar;
	CXTPMarkupScrollBar* m_pHorizontalScrollBar;
	CXTPMarkupScrollBarGripper* m_pScrollBarGripper;

	int m_xOffset;
	int m_yOffset;

public:
	static CXTPMarkupDependencyProperty* m_pVerticalScrollBarVisibilityProperty;
	static CXTPMarkupDependencyProperty* m_pHorizontalScrollBarVisibilityProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSCROLLVIEWER_H__)
