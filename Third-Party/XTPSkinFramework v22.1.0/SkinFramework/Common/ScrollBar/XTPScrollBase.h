// XTPScrollBase.h : interface for the CXTPCommandBarScrollBarCtrl class.
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
#if !defined(__XTPSCROLLBASE_H__)
#	define __XTPSCROLLBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPWinThemeWrapper;

#	define XTP_HTSCROLLUP 60		// <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLDOWN 61		// <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLLEFT 60		// <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLRIGHT 61	// <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLUPPAGE 62   // <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLDOWNPAGE 63 // <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLTHUMB 64	// <combine CXTPScrollBase::HitTestScrollBar@POINT@const>
#	define XTP_HTSCROLLPOPUP 65	// <combine CXTPScrollBase::HitTestScrollBar@POINT@const>

#	define IDSYS_SCROLL 23345
#	define IDSYS_ANIMATE 23346

//-------------------------------------------------------------------------
// Summary:
//     Scrollbar style
// See Also:
//     CXTPScrollBase::SetScrollBarStyle
//-------------------------------------------------------------------------
enum XTPScrollBarStyle
{
	xtpScrollStyleDefault,				 // Default style
	xtpScrollStyleStandard,				 // standard window theme
	xtpScrollStyleFlat,					 // Flat scrollbar style
	xtpScrollStyleSystem,				 // WinXP themed style
	xtpScrollStyleOffice,				 // Office scrollbar style
	xtpScrollStyleOffice2007Light,		 // Office 2007 light style
	xtpScrollStyleOffice2007Dark,		 // Office 2007 dark style
	xtpScrollStyleSlider,				 // Makes the scrollbar look like a slider control.
	xtpScrollStyleVisualStudio2012Light, // Visual Studio 2012 Light
	xtpScrollStyleVisualStudio2012Dark,  // Visual Studio 2012 Dark
	xtpScrollStyleVisualStudio2015,		 // Visual Studio 2015
	xtpScrollStyleNativeWindows10,		 // Native Windows 10
	xtpScrollStyleVisualStudio2017,		 // Visual Studio 2017
	xtpScrollStyleVisualStudio2019,		 // Visual Studio 2019
	xtpScrollStyleVisualStudio2022,		 // Visual Studio 2022
};

enum XTPScrollBarTheme
{
	// Microsoft(r) Windows(tm)
	xtpScrollBarThemeWindowsClassic, // Windows classic theme
	xtpScrollBarThemeWindowsDefault, // Windows default theme
	xtpScrollBarThemeFlat,
	xtpScrollBarThemeUltraFlat,
	xtpScrollBarThemeResource,

	// Microsoft(r) Office(tm)
	xtpScrollBarThemeOffice2000,							   // Office 2000 theme
	xtpScrollBarThemeOfficeXP,								   // Office XP theme
	xtpScrollBarThemeOffice2003,							   // Office 2003 theme
	xtpScrollBarThemeOffice2007Light,						   // Office 2007 light theme
	xtpScrollBarThemeOffice2007Dark,						   // Office 2007 dark theme
	xtpScrollBarThemeOffice2010,							   // Office 2010 theme
	xtpScrollBarThemeOffice2013,							   // Office 2013 theme
	xtpScrollBarThemeOffice2016 = xtpScrollBarThemeOffice2013, // Office 2016 theme

	// Microsoft(r) Visual Studio(tm)
	xtpScrollBarThemeVisualStudio2012Light, // Visual Studio 2012 light theme
	xtpScrollBarThemeVisualStudio2012Dark,  // Visual Studio 2012 dark theme
	xtpScrollBarThemeVisualStudio2015,		// Visual Studio 2015 theme
	xtpScrollBarThemeVisualStudio2017,		// Visual Studio 2017 theme
	xtpScrollBarThemeVisualStudio2019,		// Visual Studio 2019 theme
	xtpScrollBarThemeVisualStudio2022,		// Visual Studio 2022 theme

	xtpScrollBarThemeNativeWindows10,

	xtpScrollBarThemeVisualStudio6	= xtpScrollBarThemeWindowsClassic, // Visual Studio 6 theme
	xtpScrollBarThemeVisualStudio2002 = xtpScrollBarThemeWindowsDefault, // Visual Studio 2002 theme
	xtpScrollBarThemeVisualStudio2003 = xtpScrollBarThemeWindowsDefault, // Visual Studio 2003 theme
	xtpScrollBarThemeVisualStudio2005 = xtpScrollBarThemeWindowsDefault, // Visual Studio 2005 theme
	xtpScrollBarThemeVisualStudio2008 = xtpScrollBarThemeWindowsDefault, // Visual Studio 2008 theme
	xtpScrollBarThemeVisualStudio2010 = xtpScrollBarThemeWindowsDefault, // Visual Studio 2010 theme
};

class CXTPScrollBarPaintManager;

const UINT XTP_SBN_SCROLL = 0x1007;

struct NMXTPSCROLL
{
	NMHDR hdr; // NMHDR structure that contains additional information about this notification.

	CObject* pSender;
	int nSBCode;
	int nPos;
};

//===========================================================================
// Summary:
//     The XTP_SCROLLBAR_POSINFO structure contains scroll bar information.
//===========================================================================
struct _XTP_EXT_CLASS XTP_SCROLLBAR_POSINFO
{
	int posMin; // Minimum position
	int posMax; // Maximum position
	int page;   // Page size
	int pos;	// Position of thumb

	int pxTop;		   // Top bounding rectangle
	int pxBottom;	  // Bottom bounding rectangle
	int pxLeft;		   // Left bounding rectangle
	int pxRight;	   // Right bounding rectangle
	int cpxThumb;	  // Size of thumb button
	int pxUpArrow;	 // Position of Up arrow
	int pxDownArrow;   // Position of Down arrow
	int pxStart;	   // Previous position of thumb button
	int pxThumbBottom; // Thumb bottom bounding rectangle
	int pxThumbTop;	// Thumb top bounding rectangle
	int pxPopup;	   // In-place scroll position
	int cpx;		   // position in pixels
	int pxMin;		   // Minimum position in pixels

	RECT rc;	   // Bounding rectangle
	int ht;		   // Hit test
	BOOL fVert;	// TRUE if vertical
	BOOL fSizebox; // TRUE if is of size-box type, fVert is ignored.
};

//===========================================================================
// Summary:
//     The XTP_SCROLLBAR_TRACKINFO structure contains tracking information.
//===========================================================================
struct _XTP_EXT_CLASS XTP_SCROLLBAR_TRACKINFO
{
	BOOL fHitOld;	  // Previous hittest
	RECT rcTrack;	  // Bounding rectangle of available thumb position
	UINT cmdSB;		   // Scroll command
	UINT_PTR hTimerSB; // Timer identifier
	int dpxThumb;	  // Delta of thumb button
	int pxOld;		   // Previous position in pixels
	int posOld;		   // Previous position
	int posNew;		   // new position
	HWND hWndTrack;	// Parent tracking window
	BOOL bTrackThumb;  // TRUE if thumb is tracking

	XTP_SCROLLBAR_POSINFO* pSBInfo; // SCROLLBARPOSINFO pointer
};

//-------------------------------------------------------------------------
// Summary:
//     The XTP_SLIDERTICKS structure contains slider ticks.
//-------------------------------------------------------------------------
struct _XTP_EXT_CLASS XTP_SLIDERTICKS
{
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs an XTP_SLIDERTICKS object.
	// Parameters:
	//     pTicks - Point to double array
	//     nCount - Count of double values
	//-----------------------------------------------------------------------
	XTP_SLIDERTICKS(double* pTicks, int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys an XTP_SLIDERTICKS object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~XTP_SLIDERTICKS();

	double* pTicks; // Ticks array
	int nCount;		// Count of ticks
};

_XTP_EXT_CLASS int AFX_CDECL SBPosFromPx(XTP_SCROLLBAR_POSINFO* pSBInfo, int px);

_XTP_EXT_CLASS CRect AFX_CDECL OffsetSourceRect(CRect rc, int nState);

//===========================================================================
// Summary:
//     CXTPScrollBase is the base class for all scrollable objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBase
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPScrollBase object.
	//-----------------------------------------------------------------------
	CXTPScrollBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPScrollBase object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPScrollBase();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the style of the scrollbar
	// Parameters:
	//     scrollStyle - Style to be set; must be one of the values
	//                   defined by the XTPScrollBarStyle enumeration.
	// See Also:
	//     GetScrollBarStyle
	//-----------------------------------------------------------------------
	void SetScrollBarStyle(XTPScrollBarStyle scrollStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the style of the scrollbar
	// Returns:
	//     Style of the scrollbar; one of the values
	//     defined by the XTPScrollBarStyle enumeration.
	// See Also:
	//     SetScrollBarStyle
	//-----------------------------------------------------------------------
	XTPScrollBarStyle GetScrollBarStyle() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get XTP_SCROLLBAR_TRACKINFO information
	// Returns:
	//     A pointer to XTP_SCROLLBAR_TRACKINFO information
	// See Also:
	//     GetScrollBarPosInfo
	//-----------------------------------------------------------------------
	XTP_SCROLLBAR_TRACKINFO* GetScrollBarTrackInfo() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get XTP_SCROLLBAR_POSINFO information
	// Returns:
	//     A pointer to XTP_SCROLLBAR_POSINFO information
	// See Also:
	//     GetScrollBarTrackInfo
	//-----------------------------------------------------------------------
	XTP_SCROLLBAR_POSINFO* GetScrollBarPosInfo();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent window of scrollbar
	// Returns:
	//     Pointer to parent window
	//-----------------------------------------------------------------------
	virtual CWnd* GetParentWindow() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to check ScrollBar part under pointer.
	// Parameters:
	//     pt - Mouse pointer to test
	//-----------------------------------------------------------------------
	int HitTestScrollBar(POINT pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bounding rectangle of a specified part of the scrollbar.
	// Parameters:
	//     nPart - Part of the scrollbar to get bounding rectangle of.
	// Returns:
	//     The bounding rectangle of the specified part of the scroll bar.
	//-----------------------------------------------------------------------
	CRect GetScrollBarPartRect(int nPart) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to start scrollbar tracking
	// Parameters:
	//     hWnd - Parent Window handle
	//     point - Starting cursor position
	//     pSBInfo - Scrollbar information
	//     bDirect - TRUE to scroll to specified position
	//-----------------------------------------------------------------------
	void PerformTrackInit(HWND hWnd, CPoint point, XTP_SCROLLBAR_POSINFO* pSBInfo, BOOL bDirect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the ticks in the slider.
	// Returns:
	//     An XTP_SLIDERTICKS object.
	//-----------------------------------------------------------------------
	XTP_SLIDERTICKS* GetTicks() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set ticks for the slider.
	// Parameters:
	//     pTicks - Pointer to double array of ticks.
	//     nCount - Count of elements in array.
	//-----------------------------------------------------------------------
	void SetTicks(double* pTicks, int nCount);

	HBRUSH GetCltColor(CDC* pDC) const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the scrollbar is a slider control
	// Returns:
	//     TRUE if the scrollbar is a slider control
	//-----------------------------------------------------------------------
	BOOL IsSlider() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the scrollbar is enabled.
	// Returns:
	//     TRUE if the scrollbar is enabled.
	//-----------------------------------------------------------------------
	virtual BOOL IsScrollBarEnabled() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to the associated scrollbar paint manager.
	// Returns:
	//     A pointer to the associated scrollbar paint manager.
	//-----------------------------------------------------------------------
	virtual CXTPScrollBarPaintManager* GetScrollBarPaintManager() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the rectangle occupied by the scrollbar in client coordinates.
	// Returns:
	//     The rectangle occupied by the scrollbar in client coordinates.
	//-----------------------------------------------------------------------
	virtual CRect GetScrollBarRect() = 0;

	// -----------------------------------------------------------------
	// Summary:
	//     The GetScrollInfo function retrieves the parameters of a scroll bar,
	//     including the minimum and maximum scrolling positions, the page size,
	//     and the position of the scroll box(thumb)
	// Parameters:
	//     psi - Pointer to a SCROLLINFO structure. Before calling GetScrollInfo,
	//           set the cbSize member to sizeof(SCROLLINFO), and set the fMask
	//           member to specify the scroll bar parameters to retrieve. Before returning,
	//           the function copies the specified parameters to the appropriate members
	//           of the structure. Refer to MSDN documentation for more details on SCROLLINFO
	//           fields.
	// -----------------------------------------------------------------
	virtual void GetScrollInfo(SCROLLINFO* psi) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Forces the scrollbar to redraw itself.
	//-----------------------------------------------------------------------
	virtual void RedrawScrollBar() = 0;

	//{{AFX_CODEJOCK_PRIVATE
	void EndScroll(BOOL fCancel);
	void MoveThumb(int px);
	void TrackThumb(UINT message, CPoint pt);
	void TrackBox(UINT message, CPoint point);
	void ContScroll();
	virtual void DoAnimate();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	void SetupScrollInfo();
	virtual void DoScroll(int cmd, int pos) = 0;
	virtual void CalcScrollBarInfo(LPRECT lprc, XTP_SCROLLBAR_POSINFO* pSBInfo, SCROLLINFO* pSI);
	void CalcTrackDragRect(XTP_SCROLLBAR_TRACKINFO* pSBTrack) const;
	//}}AFX_CODEJOCK_PRIVATE

	XTP_SCROLLBAR_POSINFO m_spi;		 // ScrollBar position.
	XTP_SCROLLBAR_TRACKINFO* m_pSBTrack; // ScrollBar tracking
	XTP_SLIDERTICKS* m_pTicks;			 // Slider Ticks

	XTPScrollBarStyle m_scrollBarStyle; // ScrollBar style
	BOOL m_bAnimation;					// TRUE if animation enabled.
	int m_nTimerElapse;					// Scroll delay.
};

AFX_INLINE XTP_SCROLLBAR_TRACKINFO* CXTPScrollBase::GetScrollBarTrackInfo() const
{
	return m_pSBTrack;
}

AFX_INLINE XTP_SLIDERTICKS* CXTPScrollBase::GetTicks() const
{
	return m_pTicks;
}

AFX_INLINE XTP_SCROLLBAR_POSINFO* CXTPScrollBase::GetScrollBarPosInfo()
{
	return &m_spi;
}

AFX_INLINE void CXTPScrollBase::SetScrollBarStyle(XTPScrollBarStyle scrollStyle)
{
	m_scrollBarStyle = scrollStyle;
}

AFX_INLINE BOOL CXTPScrollBase::IsSlider() const
{
	return (m_scrollBarStyle == xtpScrollStyleSlider);
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPSCROLLBASE_H__)
