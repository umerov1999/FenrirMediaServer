// XTPScrollBarCtrl.h interface for the CXTPScrollBarCtrl class.
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
#if !defined(__XTPSCROLLBARCTRL_H__)
#	define __XTPSCROLLBARCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPScrollBarContainerImpl;

//===========================================================================
// Summary:
//     Scrollbar control.
// Remarks:
//     Scroll bars provide easy navigation through a long list of items or a
//     large amount of information.  They can also provide an analog representation
//     of current position.  You can use a scroll bar as an input device or
//     indicator of speed or quantity.for example, to control the volume of a
//     computer game or to view the time elapsed in a timed process.
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarCtrl
	: public CScrollBar
	, public CXTPScrollBase
{
	DECLARE_DYNCREATE(CXTPScrollBarCtrl);

	friend class CXTPScrollBarContainerImpl;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPScrollBarCtrl object
	//-----------------------------------------------------------------------
	CXTPScrollBarCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPScrollBarCtrl object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPScrollBarCtrl();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//    Sets the scrollbar theme.
	// Parameters:
	//    theme - Scrollbar theme to be set; must be one of the values
	//            defined by the XTPScrollBarTheme enumeration.
	//-----------------------------------------------------------------------
	void SetTheme(XTPScrollBarTheme theme);

	// -----------------------------------------------------------------
	// Summary:
	//     This member is called to update color, text and other visual elements
	//     of the control.
	// -----------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to the associated scrollbar paint manager.
	// Returns:
	//     A pointer to the associated scrollbar paint manager
	//-----------------------------------------------------------------------
	virtual CXTPScrollBarPaintManager* GetScrollBarPaintManager() const;

	// -----------------------------------------------------------------
	// Summary:
	//     The GetScrollInfo function retrieves the parameters of a scroll bar,
	//     including the minimum and maximum scrolling positions, the page size,
	//     and the position of the scroll box (thumb)
	// Parameters:
	//     psi - Pointer to a SCROLLINFO structure. Before calling GetScrollInfo,
	//           set the cbSize member to sizeof(SCROLLINFO), and set the fMask
	//           member to specify the scroll bar parameters to retrieve. Before returning,
	//           the function copies the specified parameters to the appropriate members
	//           of the structure. Refer to MSDN documentation for more details on SCROLLINFO
	//           fields.
	// -----------------------------------------------------------------
	virtual void GetScrollInfo(SCROLLINFO* psi);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called from OnInitDialog or OnInitialUpdate
	//     to initialize the control.
	//-----------------------------------------------------------------------
	void Init();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get XTP_SCROLLBAR_TRACKINFO information
	// Returns:
	//     A pointer to XTP_SCROLLBAR_TRACKINFO information
	// See Also:
	//     GetScrollBarPosInfo
	//-----------------------------------------------------------------------
	XTP_SCROLLBAR_TRACKINFO* GetScrollBarTrackInfo();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get XTP_SCROLLBAR_POSINFO information
	// Returns:
	//     A pointer to XTP_SCROLLBAR_POSINFO information
	// See Also:
	//     GetScrollBarTrackInfo
	//-----------------------------------------------------------------------
	XTP_SCROLLBAR_POSINFO* GetScrollBarPosInfo();

	//{{AFX_CODEJOCK_PRIVATE
	CRect GetScrollBarRect();
	BOOL IsScrollBarEnabled() const;
	CWnd* GetParentWindow() const;
	void DoScroll(int cmd, int pos);
	void RedrawScrollBar();

	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPScrollBarCtrl)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPScrollBarCtrl)
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDraw(CDC* pDC);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetScrollInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetScrollInfo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bPreSubclassInit; // TRUE is sub-classed.
	CXTPScrollBarPaintManager* m_pPaintManager;
};

AFX_INLINE CXTPScrollBarPaintManager* CXTPScrollBarCtrl::GetScrollBarPaintManager() const
{
	return m_pPaintManager;
}

AFX_INLINE XTP_SCROLLBAR_TRACKINFO* CXTPScrollBarCtrl::GetScrollBarTrackInfo()
{
	return m_pSBTrack;
}

AFX_INLINE XTP_SCROLLBAR_POSINFO* CXTPScrollBarCtrl::GetScrollBarPosInfo()
{
	return &m_spi;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSCROLLBARCTRL_H__)
