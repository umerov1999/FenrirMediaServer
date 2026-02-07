// XTPDrawHelpers.h: interface for the CXTPDrawHelpers class.
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
#if !defined(__XTPDRAWHELPERS_H__)
#	define __XTPDRAWHELPERS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPPaintManagerColorGradient;

#	ifndef DT_HIDEPREFIX
#		define DT_HIDEPREFIX 0x00100000
#	endif

#	ifndef LAYOUT_BITMAPORIENTATIONPRESERVED
#		define LAYOUT_BITMAPORIENTATIONPRESERVED 0x00000008
#	endif

#	ifndef LAYOUT_RTL
#		define LAYOUT_RTL 0x00000001
#	endif

//===========================================================================
// Summary:
//     CXTPTransparentBitmap is a helper class used to extract the
//     transparent color from a transparent BitMap.  Also, this class
//     is used to convert a transparent BitMap into a transparent icon.
//===========================================================================
class _XTP_EXT_CLASS CXTPTransparentBitmap
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructor.  Construct a new CXTPTransparentBitmap from a
	//     handle to an existing BitMap.
	// Parameters:
	//     hBitmap - Handle to an existing BitMap.
	//-----------------------------------------------------------------------
	CXTPTransparentBitmap(HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the transparent color of the BitMap.
	// Returns:
	//     -1 if the BitMap is NULL.
	//     Otherwise, a COLORREF that contains the transparent color of the BitMap.
	//-----------------------------------------------------------------------
	COLORREF GetTransparentColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to create an icon based on the BitMap.
	// Returns:
	//     NULL if the BitMap is NULL.
	//     NULL if the width or height of the BitMap is 0.
	//     Otherwise, a handle to the icon created from the BitMap.
	//-----------------------------------------------------------------------
	HICON ConvertToIcon() const;

protected:
	HBITMAP m_hBitmap; // A handle to a BtiMap.
};

//===========================================================================
// Summary:
//     CXTPClientCursorPos is a helper class used to retrieve the cursor
//     position in client coordinates.
//===========================================================================
class _XTP_EXT_CLASS CXTPClientCursorPos : public CPoint
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPClientCursorPos object used to retrieve the
	//     cursor position in client coordinates.
	// Parameters:
	//     pWnd - Pointer to the window that contains the client area to
	//            get the cursor position for.
	//-----------------------------------------------------------------------
	CXTPClientCursorPos(CWnd* pWnd);
};

//===========================================================================
// Summary:
//     CXTPEmptyRect is a helper class used to instantiate an empty
//     CRect object.
//===========================================================================
class _XTP_EXT_CLASS CXTPEmptyRect : public CRect
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPEmptyRect object used to instantiate an
	//     empty CRect object.
	//-----------------------------------------------------------------------
	CXTPEmptyRect();
};

//===========================================================================
// Summary:
//     CXTPEmptySize is a helper class used to instantiate an empty
//     CSize object.
//===========================================================================
class _XTP_EXT_CLASS CXTPEmptySize : public CSize
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPEmptySize object used to instantiate an
	//     empty CSize object.
	//-----------------------------------------------------------------------
	CXTPEmptySize();

	//-----------------------------------------------------------------------
	// Summary:
	//     Makes CXTPEmptySize a null size by setting all coordinates to zero.
	//-----------------------------------------------------------------------
	void SetSizeEmpty();

	//-----------------------------------------------------------------------
	// Summary:
	//      Assigns srcSize to CSize.
	// Parameters:
	//      srcSize - Refers to a source size. Can be a SIZE or CSize.
	//-----------------------------------------------------------------------
	const SIZE& operator=(const SIZE& srcSize);
};

//===========================================================================
// Summary:
//     CXTPWindowRect is a helper class used to retrieve the screen
//     size for a specified window.
//===========================================================================
class _XTP_EXT_CLASS CXTPWindowRect : public CRect
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPWindowRect object used to retrieve the
	//     screen size for the specified window.
	// Parameters:
	//     hWnd - Handle to the window to retrieve the screen size for.
	//     pWnd - Points to the window to retrieve the screen size for.
	//-----------------------------------------------------------------------
	CXTPWindowRect(HWND hWnd);
	CXTPWindowRect(const CWnd* pWnd); // <combine CXTPWindowRect::CXTPWindowRect@HWND>
};

//===========================================================================
// Summary:
//     CXTPClientRect is a helper class used to retrieve the client
//     area for a specified window.
//===========================================================================
class _XTP_EXT_CLASS CXTPClientRect : public CRect
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPClientRect object used to retrieve the
	//     client area for the specified window.
	// Parameters:
	//     hWnd - Handle to the window to retrieve the client area for.
	//     pWnd - Points to the window to retrieve the client area for.
	//-----------------------------------------------------------------------
	CXTPClientRect(HWND hWnd);
	CXTPClientRect(const CWnd* pWnd); //<combine CXTPClientRect::CXTPClientRect@HWND>
};

//===========================================================================
// Summary:
//     CXTPBufferDC is a helper class used to create a memory device
//     context used to draw to an off-screen bitmap.  When destroyed, the
//     class selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBufferDC : public CDC
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDC object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     hDestDC - Handle to the destination device context the memory
	//               device is BitBlt to.
	//     rcPaint - Size of the area to paint.
	//     clrBack - Represents background color for fill, can be COLORREF or gradient values using
	//               CXTPPaintManagerColorGradient.
	//     bHorz   - Used when drawing gradient background, TRUE to
	//               draw gradient from left to right, otherwise drawn top to bottom.
	//-----------------------------------------------------------------------
	CXTPBufferDC(HDC hDestDC, const CRect& rcPaint);
	CXTPBufferDC(HDC hDestDC, const CRect& rcPaint, const CXTPPaintManagerColorGradient& clrBack,
				 const BOOL bHorz = FALSE); // <combine CXTPBufferDC::CXTPBufferDC@HDC@CRect&>

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDC object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     paintDC - Handle to the destination device context the memory
	//-----------------------------------------------------------------------
	CXTPBufferDC(CPaintDC& paintDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBufferDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBufferDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the valid flag to false
	//     so that the off screen device context will not be drawn.
	//-----------------------------------------------------------------------
	void Discard();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve a CDC pointer to the destination
	//     device context.
	//-----------------------------------------------------------------------
	CDC* GetDestDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to take a snapshot of the screen underneath
	//     the area where the off screen bitmap is to be drawn.
	//-----------------------------------------------------------------------
	void TakeSnapshot();

protected:
	HDC m_hDestDC;		  // Handle to the destination device context.
	HBITMAP m_hBitmap;	// Bitmap in memory device context
	CRect m_rect;		  // Size of the area to paint.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPBufferDCEx is a helper class used to create a memory device
//     context used to draw to an off-screen bitmap.  When destroyed, the
//     class selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBufferDCEx : public CDC
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDCEx object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     hDestDC - Handle to the destination device context the memory
	//               device is BitBlt to.
	//     rcPaint - Size of the area to paint.
	//-----------------------------------------------------------------------
	CXTPBufferDCEx(HDC hDestDC, const CRect rcPaint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBufferDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBufferDCEx();

protected:
	HDC m_hDestDC;		  // Handle to the destination device context.
	HBITMAP m_hBitmap;	// Bitmap in memory device context
	CRect m_rect;		  // Size of the area to paint.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPBitmapDC is a helper class used to select a bitmap into the
//     device context specified by hDC.  When destroyed, the class
//     selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBitmapDC : public CDC
{
public:
	//---------------------------------------------------------------
	// Summary:
	//      Constructs CXTPBitmapDC
	//---------------------------------------------------------------
	CXTPBitmapDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBitmapDC object and selects the specified bitmap
	//     into the device context specified by pDC.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     pBitmap - Pointer to a CBitmap object to select into the device
	//               context.
	//     hBitmap - Bitmap handle
	//-----------------------------------------------------------------------
	CXTPBitmapDC(CDC* pDC, CBitmap* pBitmap);
	CXTPBitmapDC(CDC* pDC, HBITMAP hBitmap); // <combine CXTPBitmapDC::CXTPBitmapDC@CDC*@CBitmap*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBitmapDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBitmapDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the bitmap selected by the
	//     device context.
	// Parameters:
	//     pBitmap - Pointer to a CBitmap object to select into the device
	//               context.
	//-----------------------------------------------------------------------
	void SetBitmap(HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Releases the currently selected bitmap and selects the initial bitmap into device
	//     context.
	//-----------------------------------------------------------------------
	void SelectOld();

protected:
	HBITMAP m_hOldBitmap;	 // Handle to the previously selected bitmap.
	HBITMAP m_hCurrentBitmap; // Currently selected bitmap handle
};

//===========================================================================
// Summary:
//     CXTPFontDC is a helper class used to select a font into the
//     device context specified by pDC.  When destroyed, the class
//     selects the previous font back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPFontDC
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPFontDC object and selects the specified font
	//     into the device context specified by pDC.
	// Parameters:
	//     pDC   - Pointer to a valid device context.
	//     pFont - Pointer to a CFont object to select into the device
	//             context.
	//     clrTextColor - Text color to set in the DC.
	//-----------------------------------------------------------------------
	CXTPFontDC(CDC* pDC, CFont* pFont = NULL);
	CXTPFontDC(CDC* pDC, CFont* pFont,
			   COLORREF clrTextColor); // <combine CXTPFontDC::CXTPFontDC@CDC*@CFont*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPFontDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPFontDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the font selected by the
	//     device context.
	// Parameters:
	//     pFont - Pointer to a CFont object to select into the device
	//             context.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the text color set in the
	//     device context.
	// Parameters:
	//     clrTextColor - Text color to set in the DC.
	//-----------------------------------------------------------------------
	void SetColor(COLORREF clrTextColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the font and color selected
	//     by the device context.
	// Parameters:
	//     pFont - Pointer to a CFont object to select into the device
	//             context.
	//     clrTextColor - Text color to set in the DC.
	//-----------------------------------------------------------------------
	void SetFontColor(CFont* pFont, COLORREF clrTextColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to select the initial font back into the
	//     device context.
	// Remarks:
	//     This method is also called from the destructor.
	//-----------------------------------------------------------------------
	void ReleaseFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the initial text color back into the
	//     device context.
	// Remarks:
	//     This method is also called from the destructor.
	//-----------------------------------------------------------------------
	void ReleaseColor();

	//-----------------------------------------------------------------------
	// Summary:
	//     Disables handle leak detection in debug mode. Must be used cautiously
	//     and only when there is no control over the code being executed.
	//-----------------------------------------------------------------------
	void DisableHandleLeakDetection();

protected:
	CDC* m_pDC;					// Stored pointer to a device context.
	CFont* m_pOldFont;			// Stored pointer to an initial font from the device context.
	CFont* m_pCurrentFont;		// Stored pointer to the current font from the device context.
	COLORREF m_clrOldTextColor; // Stored an initial text color from the device context.
};

//===========================================================================
// Summary:
//     CXTPPenDC is a helper class used to create a pen using
//     the color specified by crColor.  The pen is then selected
//     into the device context specified by hDC.  When destroyed, the
//     class selects the previous pen back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPPenDC
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPenDC object using an existing CPen object.  The
	//     pen is then selected into the device context specified by hDC.
	// Parameters:
	//     pDC  - Pointer to a valid device context.
	//     pPen - Pointer to a CPen object to select into the device
	//             context.
	//     hDC     - Handle to a valid device context.
	//     nStyle  - Pen style, one of PS_* constants.
	//     nWidth  - Pen width in pixels
	//     crColor - RGB value used to create pen.
	// Remarks:
	//     The crColor version constructs a CXTPPenDC object and creates a pen using
	//     the color specified by crColor.  The pen is then selected
	//     into the device context specified by hDC.
	//-----------------------------------------------------------------------
	CXTPPenDC(CDC* pDC, CPen* pPen);
	CXTPPenDC(HDC hDC, COLORREF crColor);			  // <combine CXTPPenDC::CXTPPenDC@CDC*@CPen*>
	CXTPPenDC(HDC hDC, int nStyle, COLORREF crColor); // <combine CXTPPenDC::CXTPPenDC@CDC*@CPen*>
	CXTPPenDC(HDC hDC, int nStyle, int nWidth,
			  COLORREF crColor); // <combine CXTPPenDC::CXTPPenDC@CDC*@CPen*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPenDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPenDC();

	void Create(HDC hDC, int nStyle, int nWidth, COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve or change the color used by the
	//     device context pen.
	// Parameters:
	//     crColor - RGB value to change the pen color to.
	// Remarks:
	//     The No argument version is used to retrieve the color used by the
	//     device context pen.
	// Returns:
	//     The no argument version returns an RGB value that represents the selected pen color.
	//-----------------------------------------------------------------------
	COLORREF Color();
	void Color(COLORREF crColor); // <combine CXTPPenDC::Color>

private:
	void ReleasePen();

protected:
	CPen m_pen;			// Pen selected into device context.
	HDC m_hDC;			// Device context handle.
	HPEN m_hOldPen;		// Handle to the previously selected pen.
	HPEN m_hCurrentPen; // Handle to the currently selected pen.
	int m_nStyle;		// Pen style, one of PS_* constants.
	int m_nWidth;		// Pen width in pixels
	COLORREF m_crColor; // Pen color
};

//===========================================================================
// Summary:
//     CXTPBrushDC is a helper class used to create a  brush using
//     the color specified by crColor.  The brush is then selected
//     into the device context specified by hDC.  When destroyed, the
//     class selects the previous brush back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBrushDC
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBrushDC object and creates a brush using
	//     the color specified by crColor.  The brush is then selected
	//     into the device context specified by hDC.
	// Parameters:
	//     hDC     - Handle to a valid device context.
	//     crColor - RGB value used to create brush.
	//-----------------------------------------------------------------------
	CXTPBrushDC(HDC hDC, COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBrushDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBrushDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the color used by the
	//     device context brush.
	// Parameters:
	//     crColor - RGB value to change the brush color to.
	//-----------------------------------------------------------------------
	void Color(COLORREF crColor);

private:
	void ReleaseBrush();

protected:
	CBrush m_brush;		// Brush selected into device context.
	HDC m_hDC;			// Device context handle.
	HBRUSH m_hOldBrush; // Handle to the previously selected brush.
};

//{{AFX_CODEJOCK_PRIVATE
#	ifndef COLORREF_NULL
#		define COLORREF_NULL (COLORREF) - 1
#	endif
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPDCSaveState is a helper class used to save the current state of the
//     specified device context by copying state information (such as clipping
//     region, selected objects, and mapping mode) to a context stack maintained
//     by Windows. The saved device context will be restored once the object is
//     destroyed.
//===========================================================================
class _XTP_EXT_CLASS CXTPDCSaveState
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDCSaveState object and saves the state of the
	//     specified device context.
	// Parameters:
	//     pDC - Points to to a valid device context.
	//-----------------------------------------------------------------------
	CXTPDCSaveState(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDCSaveState object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDCSaveState();

private:
	CDC* m_pDC;   // Pointer to a valid device context.
	int m_nState; // Integer identifying the saved device context
};

//===========================================================================
// Summary:
//     CXTPCompatibleDC is a helper class used to create a memory device
//     context (DC) compatible with the device specified by pDC.  When
//     destroyed, the class selects the previous bitmap back into the
//     device context to handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPCompatibleDC : public CDC
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCompatibleDC object and creates a memory
	//     device context (DC) compatible with the device specified by
	//     pDC.  The bitmap specified by pBitmap is then selected into
	//     the device context.
	// Parameters:
	//     pDC     - Points to a valid device context.
	//     pBitmap - Points to the previously selected bitmap.
	//     hBitmap - Points to the previously selected bitmap.
	//-----------------------------------------------------------------------
	CXTPCompatibleDC(CDC* pDC, CBitmap* pBitmap);
	CXTPCompatibleDC(CDC* pDC,
					 HBITMAP hBitmap); // <combine CXTPCompatibleDC::CXTPCompatibleDC@CDC*@CBitmap*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCompatibleDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCompatibleDC();

protected:
	HBITMAP m_hBitmap;	// Currently selected bitmap.
	HBITMAP m_hOldBitmap; // Pointer to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPSplitterTracker is a standalone class. It is used
//     to track a splitter rectangle.
//===========================================================================
class _XTP_EXT_CLASS CXTPSplitterTracker
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSplitterTracker object.
	// Parameters:
	//     bSolid - TRUE to use solid tracker; FALSE for HalftoneBrush.
	//-----------------------------------------------------------------------
	CXTPSplitterTracker(BOOL bSolid = FALSE, BOOL bDesktopDC = TRUE);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to track size of the splitter.
	// Parameters:
	//     pTrackWnd - Pointer to a CWnd object.
	//     rcAvail - Available rectangle of tracking.
	//     rectTracker - Current rectangle of tracking.
	//     point - Starting point.
	//     bHoriz - TRUE to track horizontally.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL Track(CWnd* pTrackWnd, CRect rcAvail, CRect& rectTracker, CPoint point, BOOL bHoriz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Controls the accumulation of bounding-rectangle information for the specified device
	//     context.
	// Parameters:
	//     rcBoundRect - A CRect object that specifies the boundaries of the bounding rectangle.
	//-----------------------------------------------------------------------
	void SetBoundRect(CRect rcBoundRect);

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     This function is called by the framework during the resizing of splitter windows.
	//     This function inverts the contents of the Splitter Tracker rectangle.
	//     Inversion is a logical NOT operation and flips the bits of each pixel.
	// Parameters:
	//     rc - A CRect object that specifies the XY position of the Splitter Tracker.
	//-----------------------------------------------------------------------
	void OnInvertTracker(CRect rc);

private:
	CDC* m_pDC;
	BOOL m_bSolid;
	CRect m_rcBoundRect;
	CWnd* m_pWnd;
	BOOL m_bDesktopDC;
	CWnd* m_pSplitterWnd;
	PVOID m_pfnSetLayeredWindowAttributes; // Pointer to SetLayeredWindowAttributes method.
};

//===========================================================================
// Summary:
//     The class CXTPMouseMonitor is a helper class that is used to
//     monitor mouse messages for in-place controls.  This class is used
//     in XTPPropertyGridInPlaceList and XTPGridInplaceControls.
//===========================================================================
class _XTP_EXT_CLASS CXTPMouseMonitor
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set up a hook to monitor mouse messages.
	// Parameters:
	//     pWndMonitor - A pointer to a CWnd object.  Used to determine
	//                   which monitor currently contains the mouse cursor.
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetupHook(CWnd* pWndMonitor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to determine if mouse messages was hooked.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsMouseHooked();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     The MouseProc hook procedure is an application-defined or
	//     library-defined callback function used with the SetWindowsHookEx
	//     function. The system calls this function whenever an application
	//     calls the GetMessage or PeekMessage function and there is a mouse
	//     message to be processed.
	// Parameters:
	//     nCode  - Specifies a code the hook procedure uses to determine
	//          how to process the message. This parameter can be one of the
	//          following values: HC_ACTION and HC_NOREMOVE.
	//          HC_ACTION   - The wParam and lParam parameters contain information about a mouse
	//          message. HC_NOREMOVE - The wParam and lParam parameters contain information about a
	//          mouse message,
	//                        and the mouse message has not been removed from the message queue. (An
	//                        application called the PeekMessage function, specifying the
	//                        PM_NOREMOVE flag.)
	//     wParam - Specifies the identifier of the mouse message.
	//     lParam - Pointer to a MOUSEHOOKSTRUCT structure.
	// Returns:
	//     If nCode is less than zero, the hook procedure must return the value returned by
	//     CallNextHookEx. If nCode is greater than or equal to zero, and the hook procedure did not
	//     process the message, it is highly recommended that you call CallNextHookEx and return the
	//     value it returns; otherwise, other applications that have installed WH_MOUSE hooks will
	//     not receive hook notifications and may behave incorrectly as a result. If the hook
	//     procedure processed the message, it may return a nonzero value to prevent the system from
	//     passing the message to the target window procedure.
	//-----------------------------------------------------------------------
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
	static HHOOK m_hHookMouse;  // A handle to a hook.
	static CWnd* m_pWndMonitor; // A pointer to the in-place control.
};

//-----------------------------------------------------------------------
// Summary:
//     The XTPTrackMouseEvent() function Posts messages when the mouse pointer
//     leaves a window or hovers over a window for a specified amount of time.
// Parameters:
//     hWndTrack   - A handle to the window to track.
//     dwFlags     - The services requested. This member can be a combination
//                   of the following values.
//                   * TME_CANCEL    - The caller wants to cancel a prior tracking
//                                     request. The caller should also specify
//                                     the type of tracking that it wants to
//                                     cancel. For example, to cancel hover tracking,
//                                     the caller must pass the TME_CANCEL and
//                                     TME_HOVER flags.
//                   * TME_HOVER     - The caller wants hover notification.
//                                     Notification is delivered as a WM_MOUSEHOVER
//                                     message.  If the caller requests hover tracking
//                                     while hover tracking is already active,
//                                     the hover timer will be reset.  This flag is
//                                     ignored if the mouse pointer is not over the
//                                     specified window or area.
//                   * TME_LEAVE     - The caller wants leave notification. Notification
//                                     is delivered as a WM_MOUSELEAVE message. If the
//                                     mouse is not over the specified window or area,
//                                     a leave notification is generated immediately
//                                     and no further tracking is performed.
//                   * TME_NONCLIENT - The caller wants hover and leave notification
//                                     for the nonclient areas. Notification is delivered
//                                     as WM_NCMOUSEHOVER and WM_NCMOUSELEAVE messages.
//                   * TME_QUERY     - The function fills in the structure instead of
//                                     treating it as a tracking request. The structure
//                                     is filled such that had that structure been passed
//                                     to TrackMouseEvent, it would generate the current
//                                     tracking. The only anomaly is that the hover time-out
//                                     returned is always the actual time-out and not
//                                     HOVER_DEFAULT, if HOVER_DEFAULT was specified during
//                                     the original TrackMouseEvent request.
//     dwHoverTime - The hover time-out (if TME_HOVER was specified in dwFlags), in
//                   milliseconds. Can be HOVER_DEFAULT, which means to use the system default
//                   hover time-out.
// Remarks:
//     The mouse pointer is considered to be hovering when it stays within a
//     specified rectangle for a specified period of time. Call SystemParametersInfo.
//     and use the values SPI_GETMOUSEHOVERWIDTH, SPI_GETMOUSEHOVERHEIGHT, and
//     SPI_GETMOUSEHOVERTIME to retrieve the size of the rectangle and the time.
//
//     The function can post the following messages.
//
//     * WM_NCMOUSEHOVER - The same meaning as WM_MOUSEHOVER except this is for
//                         the nonclient area of the window.
//     * WM_NCMOUSELEAVE - The same meaning as WM_MOUSELEAVE except this is for
//                         the nonclient area of the window.
//     * WM_MOUSEHOVER   - The mouse hovered over the client area of the window
//                         for the period of time specified in a prior call to
//                         XTPTrackMouseEvent. Hover tracking stops when this message
//                         is generated. The application must call XTPTrackMouseEvent
//                         again if it requires further tracking of mouse hover behavior.
//     * WM_MOUSELEAVE   - The mouse left the client area of the window specified
//                         in a prior call to XTPTrackMouseEvent. All tracking requested
//                         by XTPTrackMouseEvent is canceled when this message is generated.
//                         The application must call XTPTrackMouseEvent when the mouse
//                         reenters its window if it requires further tracking of mouse
//                         hover behavior.
// Returns:
//     If the function succeeds, the return value is nonzero.  If the function fails,
//     return value is zero. To get extended error information, call GetLastError.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPTrackMouseEvent(HWND hWndTrack, DWORD dwFlags = TME_LEAVE,
												 DWORD dwHoverTime = HOVER_DEFAULT);

//===========================================================================
// Summary:
//     CXTPDrawHelpers is a helper class used to perform specialized
//     drawing tasks.  You can use this class to perform such tasks as
//     gradient fills and blending colors.
//===========================================================================
class _XTP_EXT_CLASS CXTPDrawHelpers
{
	friend class CXTPSingleton<CXTPDrawHelpers>;

public:
	typedef BOOL(__stdcall* PFNGRADIENTFILL)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG,
											 ULONG); // Function pointer used to access the Windows
													 // API GradientFill function.
	typedef BOOL(__stdcall* PFNALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int,
										   BLENDFUNCTION); // AlphaBlend function declaration
	typedef BOOL(WINAPI* PFNTRANSPARENTBLT)(HDC, int, int, int, int, HDC, int, int, int, int, UINT);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDrawHelpers object.
	//-----------------------------------------------------------------------
	CXTPDrawHelpers();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXCSplitPath object, handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDrawHelpers();

public:
	void SolidRectangle(CDC* pDC, CRect rc, COLORREF clrRect, COLORREF clrFill);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to perform a gradient fill using
	//     the specified COLORREF values crFrom and crTo.
	// Parameters:
	//     pDC         - Points to the device context
	//     lpRect      - Size of area to fill
	//     crFrom      - RGB value to start from
	//     crTo        - RGB value to fill to
	//     grc         - CXTPPaintManagerColorGradient struct containing start from and
	//                   fill to RGB color values.
	//     bHorz       - TRUE if fill is horizontal.
	//     lpRectClip  - Pointer to a RECT structure that contains a clipping rectangle.
	//                   This parameter is optional and may be set to NULL.
	//     hdc         - Handle to a device context.
	//     pVertex     - Pointer to an array of TRIVERTEX structures that
	//                   each define a triangle vertex.
	//     dwNumVertex - The number of vertices.
	//     pMesh       - Array of GRADIENT_TRIANGLE structures in triangle
	//                   mode, or an array of GRADIENT_RECT structures in
	//                   rectangle mode.
	//     dwNumMesh   - The number of elements (triangles or rectangles)
	//                   in pMesh.
	//     dwMode      - Specifies gradient fill mode. For a list of possible
	//                   values, see GradientFill in the Platform SDK.
	// Remarks:
	//     Call this member function to fill rectangle and triangle structures
	//     with color that smoothly fades from one side to the other.
	// Returns:
	//     The HDC version TRUE if successful; otherwise FALSE.
	//-----------------------------------------------------------------------
	void GradientFill(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	void GradientFill(
		CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz,
		LPCRECT lpRectClip); // <combine
							 // CXTPDrawHelpers::GradientFill@CDC*@LPCRECT@COLORREF@COLORREF@BOOL>
	void GradientFill(
		CDC* pDC, LPCRECT lpRect, const CXTPPaintManagerColorGradient& grc, BOOL bHorz,
		LPCRECT lpRectClip =
			NULL); // <combine CXTPDrawHelpers::GradientFill@CDC*@LPCRECT@COLORREF@COLORREF@BOOL>
	BOOL GradientFill(
		HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh,
		ULONG dwMode); // <combine
					   // CXTPDrawHelpers::GradientFill@CDC*@LPCRECT@COLORREF@COLORREF@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a 1 or 2 pixel wide border
	//     using the colors specified by pOutsideClr and pInsideClr. If
	//     pInsideClr is NULL, then a 1 pixel border is drawn.
	// Parameters:
	//     pDC        - Pointer to a valid device context.
	//     rBorder    - Size of the border to draw.
	//     clrOutside - An array of 4 colors that represent the left,
	//                  top, right and bottom outside border color.
	//     clrInside  - An array of 4 colors that represent the left,
	//                  top, right and bottom inside border color.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawBorder(CDC* pDC, CRect rBorder, COLORREF* pOutsideClr,
									 COLORREF* pInsideClr = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to exclude the pixels for each corner of the
	//     area specified by rc.
	// Parameters:
	//     pDC - Points to the device context
	//     rc  - Area to exclude corners from
	//-----------------------------------------------------------------------
	void ExcludeCorners(CDC* pDC, CRect rc, BOOL bTopCornersOnly = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will search a string, strip off the mnemonic
	//     '&', and reformat the string.
	// Parameters:
	//     strClear - Text needed to strip.
	//     lpszClear - Text needed to strip.
	//-----------------------------------------------------------------------
	static void AFX_CDECL StripMnemonics(CString& strClear);
	static void AFX_CDECL
		StripMnemonics(LPTSTR lpszClear); // <combine CXTPDrawHelpers::StripMnemonics@CString&>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to blur the color value for the
	//     points specified by pts
	// Parameters:
	//     pDC    - Points to the device context
	//     pts    - Array of points to blur
	//     nCount - Number of points in array.
	//-----------------------------------------------------------------------
	void BlurPoints(CDC* pDC, LPPOINT pts, int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to blend the colors specified by
	//     crA and crB.
	// Parameters:
	//     crA      - RGB value to blend
	//     crB      - RGB value to blend
	//     fAmountA - The amount that crA saturates crB.
	// Returns:
	//     An RGB value that represents the blended color.
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL BlendColors(COLORREF crA, COLORREF crB, double fAmountA);

	static BOOL AFX_CDECL GetIconLogFont(LOGFONT* pLogFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks for the system for non-standard character set and
	//     retrieves DEFAULT_GUI_FONT object. If a standard character
	//     set is used, then SPI_GETICONTITLELOGFONT is used to retrieve
	//     system font information. If bModernFont is specified, then the
	//     system is checked for the existence of "Segoe UI" or "Tahoma"
	//     fonts and sets the lfFaceName member to the first value found.
	//     If the modern fonts do not exist, then the default value for
	//     SPI_GETICONTITLELOGFONT will be used.
	// Parameters:
	//     pLogFont    - Destination LOGFONT structure to recive font
	//                   information.
	//     bModernFont - TRUE to search for "Segoe UI" and "Tahoma"
	//                   fonts and use those values for lfFaceName if
	//                   found.
	//     bVertFont   - TRUE if the font is to be used for vertical
	//                   display.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL GetDefaultLogFont(LOGFONT* pLogFont, BOOL bModernFont = TRUE,
											BOOL bVertFont = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to search for and replace a single
	//     color using the specified device context.
	// Parameters:
	//     pDC       - Points to a valid device context.
	//     rArea     - Area on device context to replace colors.
	//     crFind    - RGB value specifying which color to locate.
	//     crReplace - RGB value specifying which color to replace crFind with.
	//-----------------------------------------------------------------------
	static void AFX_CDECL ReplaceColor(CDC* pDC, CRect rArea, COLORREF crFind, COLORREF crReplace);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get dark color from specified by lColor
	// Parameters:
	//     lScale - Scale value
	//     lColor - RGB value to get
	// See Also:
	//     LightenColor
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL DarkenColor(long lScale, COLORREF lColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get light color from specified by lColor
	// Parameters:
	//     lScale - Scale value
	//     lColor - RGB value to get
	// See Also:
	//     DarkenColor
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL LightenColor(long lScale, COLORREF lColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to convert a color from RGB to HSL color model.
	// Parameters:
	//     rgb - RGB color to convert
	// Returns:
	//     An HSL representation of the specified color.
	// See Also:
	//     HSLtoRGB
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL RGBtoHSL(COLORREF rgb);
	static void AFX_CDECL RGBtoHSL(COLORREF clr, double& h, double& s,
								   double& l); // <combine CXTPDrawHelpers::RGBtoHSL@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to convert a color from HSL to RGB color model.
	// Parameters:
	//     hsl - HSL color to convert
	// Returns:
	//     An RGB representation of the specified color.
	// See Also:
	//     HueToRGB, RGBtoHSL
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL HSLtoRGB(COLORREF hsl);
	static COLORREF AFX_CDECL HSLtoRGB(double h, double s,
									   double l); // <combine CXTPDrawHelpers::HSLtoRGB@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to darken or lighten the color specified
	//     by clr.  Unlike DarkenColor() which modifies the red, green and blue
	//     color values, DarkenColorLum() adjusts the luminosity (brightness)
	//     for the color.
	// Parameters:
	//      clr      - RGB representing the color to adjust.
	//      dPercent - A double value representing the percentage in decimal
	//                 format to darken the color by.  For example if you
	//                 wanted to darken the color by 75% you would use 0.25.
	// Returns:
	//     A COLORREF value representing the adjusted color value.
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL DarkenColorLum(COLORREF clr, double dPercent);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will either lighten or darken the color
	//     specified by clr using the value specified by nPercent. The
	//     value must be a value between -100 and +100.  Negative values
	//     will darken the color, positive values will lighten it.
	// Parameters:
	//     clr      - RGB value representing the color to adjust.
	//     nPercent - Amount to adjust the color by, must be a value
	//                        between -100 and +100 percent.
	// Returns:
	//     An RGB value representing the adjusted color value. If the
	//     value specified by nPercent is 0, then the return value will
	//     be the same value as clr.  If nPercent is not a valid value,
	//     the return value will be COLORREF_NULL (-1).
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL AdjustBrightness(COLORREF clr, int nPercent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to convert the specified color to
	//     greyscale values.
	// Parameters:
	//     clr - RGB value of the color to convert to greyscale.
	// Returns:
	//     An RGB value representing a greyscale version of the
	//     color specified by clr.
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL GreyScaleColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the specified font
	//     exists.
	// Parameters:
	//     lpszFaceName - A NULL-terminated string that represents the
	//                   font face name.
	// Returns:
	//     TRUE if the font was found, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL FontExists(LPCTSTR lpszFaceName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves DEFAULT_GUI_FONT font name.
	// Returns:
	//     Face name of DEFAULT_GUI_FONT font.
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetDefaultFontName();

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws text using the font name and height provided.
	// Parameters:
	//     hDC - Device context handle
	//     lpszFontFaceName - Pointer to font name string
	//     nFontHeight - Font height. If 0, the text will be fit into the rectangle.
	//     nCount - The length, in characters, of the string. If nCount is -1,
	//              then string length will be computed.
	//     lpRect - A pointer to a RECT structure that contains the rectangle
	//              (in logical coordinates) in which the text is to be formatted.
	//     uFormat - The method of formatting the text. Refer to DrawText SDK
	//               function documentation for more details.
	//     crForeColor - Foreground text color. When equals to COLORREF_NULL, no
	//                   foreground text color is used.
	//     crBackColor - Background text color. When equals to COLORREF_NULL, no
	//                   background text color is used.
	// Returns:
	//     If the function succeeds, the return value is the height of the text
	//     in logical units. If DT_VCENTER or DT_BOTTOM is specified, the return
	//     value is the offset from lpRect->top to the bottom of the drawn text
	//     If the function fails, the return value is zero.
	//-----------------------------------------------------------------------
	static int AFX_CDECL DrawFontText(CDC* pDC, LPCTSTR lpszFontFaceName, int nFontHeight,
									  LPCTSTR lpchText, int nCount, LPRECT lpRect, UINT uFormat,
									  COLORREF crForeColor = COLORREF_NULL,
									  COLORREF crBackColor = COLORREF_NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws text using the font name and height provided.
	// Parameters:
	//     hDC - Device context handle
	//     pFont - Pointer to font
	//     nCount - The length, in characters, of the string. If nCount is -1,
	//              then string length will be computed.
	//     lpRect - A pointer to a RECT structure that contains the rectangle
	//              (in logical coordinates) in which the text is to be formatted.
	//     uFormat - The method of formatting the text. Refer to DrawText SDK
	//               function documentation for more details.
	//     crForeColor - Foreground text color. When equals to COLORREF_NULL, no
	//                   foreground text color is used.
	//     crBackColor - Background text color. When equals to COLORREF_NULL, no
	//                   background text color is used.
	// Returns:
	//     If the function succeeds, the return value is the height of the text
	//     in logical units. If DT_VCENTER or DT_BOTTOM is specified, the return
	//     value is the offset from lpRect->top to the bottom of the drawn text
	//     If the function fails, the return value is zero.
	//-----------------------------------------------------------------------
	static int AFX_CDECL DrawFontText(CDC* pDC, CFont* pFont, LPCTSTR lpchText, int nCount,
									  LPRECT lpRect, UINT uFormat,
									  COLORREF crForeColor = COLORREF_NULL,
									  COLORREF crBackColor = COLORREF_NULL);

	static CPoint AFX_CDECL Dlu2Pix(int dluX, int dluY);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves vertical font name
	// Parameters:
	//     bUseOfficeFont - TRUE to use "Tahoma" font if exists.
	// Returns:
	//     Face name of vertical font.
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetVerticalFontName(BOOL bUseOfficeFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a triangle onto the
	//     specified device context.
	// Parameters:
	//     pDC      - Points to the device context
	//     pt0      - First point of the triangle
	//     pt1      - Second point of the triangle
	//     pt2      - Third point of the triangle
	//     clrPen   - An RGB value that represents the border color.
	//     clrBrush - An RGB value that represents the fill color, if COLORREF_NULL
	//                the value specified by clrPen is used.
	//-----------------------------------------------------------------------
	static void AFX_CDECL Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clrPen,
								   COLORREF clrBrush = COLORREF_NULL);
	static void AFX_CDECL Triangle(
		CDC* pDC, CPoint pt0, CPoint pt1,
		CPoint pt2); // <combine CXTPDrawHelpers::Triangle@CDC*@CPoint@CPoint@CPoint@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw an expand triangle commonly
	//     used on tree controls or other controls that need to show the
	//     expanded state of a list or group of items.
	// Parameters:
	//     pDC       - Points to the device context
	//     pt        - Top left anchor point for the triangle.
	//     bExpanded - TRUE to draw the expanded triangle.
	//     clrPen    - An RGB value that represents the border color.
	//     clrBrush  - An RGB value that represents the fill color, if
	//                 COLORREF_NULL the value specified by clrPen is used.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawExpandTriangle(CDC* pDC, CPoint pt, BOOL bExpanded, COLORREF clrPen,
											 COLORREF clrBrush = COLORREF_NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw an expand arrow commonly
	//     used on pop-up windows or other controls that need to show the
	//     expanded state of a docked window.
	// Parameters:
	//     pDC       - Points to the device context
	//     pt        - Top left anchor point for the triangle.
	//     bExpanded - TRUE to draw the expanded triangle.
	//     clrArrow  - An RGB value that represents the arrow color.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawExpandArrow(CDC* pDC, CRect rc, BOOL bExpanded, COLORREF clrArrow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a vertical or horizontal line using
	//     the specified device context.
	// Parameters:
	//     pDC    - Points to the device context
	//     x1     - Specifies the logical x-coordinate of the start position.
	//     y1     - Specifies the logical y-coordinate of the start position.
	//     x2     - Specifies the logical x-coordinate of the end position.
	//     y2     - Specifies the logical y-coordinate of the end position.
	//     crLine - Specifies the RGB color value used to draw the line.
	// Returns:
	//     TRUE if the specified device context is valid, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL DrawLine(CDC* pDC, int x1, int y1, int x2, int y2, COLORREF crLine);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the system display
	//     is set to low resolution.
	// Parameters:
	//     hDC - Handle to a device context, if NULL the desktop window
	//           device context is used.
	// Returns:
	//     TRUE if the system display is set to low resolution,
	//     otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsLowResolution(HDC hDC = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if GradientFill from msimg32.dll available.
	// Returns:
	//     TRUE if GradientFill can be used
	//-----------------------------------------------------------------------
	BOOL IsFastFillAvailable() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called by the Theme Manager to capture the
	//      area on the parent window underneath the specified owner window.
	//      This is then used for rendering the owner transparent.
	// Parameters:
	//      pWndOwner   - Pointer the child of the parent window to take a
	//                    snapshot for.
	//      bmpSnapshot - Bitmap used to store transparent background.
	// Returns:
	//      TRUE if the parent area was successfully captured, otherwise
	//      returns FALSE.
	// See Also:
	//      DrawTransparentBack
	//-----------------------------------------------------------------------
	BOOL TakeSnapShot(CWnd* pWndOwner, CBitmap& bmpSnapshot);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to render the window specified
	//      by pWndOwner transparent displaying the parent windows background
	//      instead.
	// Parameters:
	//      pDC         - Pointer to pWndOwner device context.
	//      pWndOwner   - Pointer the child of the parent window to draw
	//                    transparent.
	//      bmpSnapshot - Bitmap used to store transparent background.
	// Returns:
	//      TRUE if the area was dran transparently, otherwise returns FALSE.
	// See Also:
	//      TakeSnapShot
	//-----------------------------------------------------------------------
	BOOL DrawTransparentBack(CDC* pDC, CWnd* pWndOwner, CBitmap& bmpSnapshot);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if top level parent is active for window handle
	// Parameters:
	//     hWnd        - Window handle to test
	// Returns:
	//     TRUE if top level parent is active.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsTopParentActive(HWND hWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to switch Left and Right keys if window
	//     in Right To Left layout
	// Parameters:
	//     pWnd        - Window to test
	//     nChar       - Virtual key, passed to OnKeyDown member.
	//-----------------------------------------------------------------------
	static void AFX_CDECL KeyToLayout(CWnd* pWnd, UINT& nChar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts the screen coordinates of a given point on the display to window coordinates.
	// Parameters:
	//     pWnd - window whose area is used for the conversion
	//     lpPoint - Points to a CPoint object or POINT structure that contains the screen
	//     coordinates to be converted.
	//-----------------------------------------------------------------------
	static void AFX_CDECL ScreenToWindow(CWnd* pWnd, LPPOINT lpPoint);

	static DWORD AFX_CDECL GetLayout(CDC* pDC);
	static DWORD AFX_CDECL GetLayout(HDC hDC);

	static DWORD AFX_CDECL SetLayout(CDC* pDC, DWORD dwLayout);
	static DWORD AFX_CDECL SetLayout(HDC hDC, DWORD dwLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if layout of a device context right to left.
	// Parameters:
	//     pDC - Device context to test
	//     hDC - Device context to test
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsContextRTL(CDC* pDC);
	static BOOL AFX_CDECL IsContextRTL(HDC hDC); // <combine CXTPDrawHelpers::IsContextRTL@CDC*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set right to left layout for device context
	// Parameters:
	//     pDC - Device context to set layout
	//     hDC - Device context to set layout
	//     bLayoutRTL - TRUE to set right to left layout
	//     dwLayoutRTL - Layout flags to be passed to SetLayout GDI function.
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetContextRTL(CDC* pDC, DWORD dwLayoutRTL);
	static void AFX_CDECL SetContextRTL(
		HDC hDC, DWORD dwLayoutRTL); // <combine CXTPDrawHelpers::SetContextRTL@CDC*@DWORD>

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if window has right-to-left layout.
	// Parameters:
	//     pWnd - Window handle to test
	//     hWnd - Window handle to test
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsWindowRTL(CWnd* pWnd);
	static BOOL AFX_CDECL IsWindowRTL(HWND hWnd); // <combine CXTPDrawHelpers::IsWindowRTL@CWnd*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if window has vertical scroll bar on the left side.
	// Parameters:
	//     pWnd - Window handle to test
	//     hWnd - Window handle to test
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsVScrollOnTheLeft(CWnd* pWnd);
	static BOOL AFX_CDECL
		IsVScrollOnTheLeft(HWND hWnd); // <combine CXTPDrawHelpers::IsVScrollOnTheLeft@CWnd*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Register class helper.
	// Parameters:
	//     hInstance - Handle to the instance that contains the window procedure for the class.
	//     lpszClassName - Pointer to a null-terminated string or is an atom
	//     style - Specifies the class style(s).
	//     hIcon - Handle to the class icon
	//     hbrBackground - Handle to the class background brush.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL RegisterWndClass(HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style,
										   HICON hIcon = 0, HBRUSH hbrBackground = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies the CWnd caption title (if it has one) into the destination
	//     string strWindowText.
	// Parameters:
	//     hWnd          - Handle to the window or control containing the text.
	//     strWindowText - A CString object that is to receive the copied string
	//                     of the window's title.
	//-----------------------------------------------------------------------
	static void AFX_CDECL GetWindowCaption(HWND hWnd, CString& strWindowText);

	//-----------------------------------------------------------------------
	// Summary:
	//	   Call this function to render a color rectangle with a border.
	// Parameters:
	//	   pDC       - Pointer to a valid device context.
	//	   rColor    - Size of the color rectangle to draw.
	//     l         - Left edge of the rectangle.
	//     t         - Top edge of the rectangle.
	//     cx        - Width of the rectangle.
	//     cy        - Height of the rectangle.
	//	   clrFill   - Background color for the rectangle.
	//	   clrBorder - Border color for the rectangle.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawColorRect(CDC* pDC, CRect rColor, COLORREF clrFill,
										COLORREF clrBorder);
	static void AFX_CDECL DrawColorRect(
		CDC* pDC, int l, int t, int cx, int cy, COLORREF clrFill,
		COLORREF clrBorder); // <combine
							 // CXTPDrawHelpers::DrawColorRect@CDC*@CRect@COLORREF@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//	   Call this function to render a hatch pattern rectangle with a border.
	// Parameters:
	//	   pDC       - Pointer to a valid device context.
	//	   rColor    - Size of the hatch rectangle to draw.
	//	   clrFill   - Background color for the hatch rectangle.
	//	   clrBorder - Border color for the hatch rectangle.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawHatchRect(CDC* pDC, CRect rColor, COLORREF clrFill,
										COLORREF clrBorder);

	//-----------------------------------------------------------------------
	// Summary:
	//	   Call this function to create a pattern brush used for fill.
	// Parameters:
	//	   bmp        - Reference to a bitmap object.
	//	   brush      - Reference to a brush object.
	//	   clrFill    - Background color for the hatch rectangle.
	//     pBitsBrush - Pattern defined as a WORD array containing 8 hex
	//                  values that represent an 8 x 8 pixel pattern.
	//-----------------------------------------------------------------------
	static void AFX_CDECL CreatePatternBrush(CBitmap& bmp, CBrush& brush, COLORREF clrFill,
											 const void* pBitsBrush);

	//-----------------------------------------------------------------------
	// Summary:
	//	   Call this function to render a pattern rectangle with a border.
	// Parameters:
	//	   pDC        - Pointer to a valid device context.
	//	   rColor     - Size of the hatch rectangle to draw.
	//	   clrFill    - Background color for the hatch rectangle.
	//	   clrBorder  - Border color for the hatch rectangle.
	//     pBitsBrush - Pattern defined as a WORD array containing 8 hex
	//                  values that represent an 8 x 8 pixel pattern.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawBrushRect(CDC* pDC, CRect rColor, COLORREF clrFill,
										COLORREF clrBorder, const void* pBitsBrush);

	//-----------------------------------------------------------------------
	// Summary:
	//	   Computes font height such that the string provided fits the specified
	//     rectangle as tight as possible.
	// Parameters:
	//	   pDC       - Pointer to a valid device context.
	//	   lpStr     - Pointer to a string which is to be fitted into the specified
	//                 rectangle.
	//	   cch       - Number of characters to use from the string provided, or
	//                 -1 to compute the length of the string.
	//	   rc        - Rectangle to contain the string specified.
	//	   fntTest   - Font used to draw the text specified. Font height is ignored.
	//                 Font width has to be 0.
	// Returns:
	//     If successful, the return value is the height of the font necessary to fit the
	//     specified rectangle as tight as possible. Otherwise, the return value is 0.
	//-----------------------------------------------------------------------
	static int ComputeFittingFontHeight(CDC* pDC, LPCTSTR lpStr, int cch, const CRect& rc,
										CFont& fntTest);

	//-----------------------------------------------------------------------
	// Summary:
	//	   Draws a stretched image from an image list.
	// Parameters:
	//	   pDC        - Pointer to a valid device context where the image is
	//                  to be drawn to.
	//	   rcDest     - Destination rectangle.
	//	   pImageList - A valid image list pointer.
	//     nImage     - Image index to draw.
	// Returns:
	//		TRUE is the image is drawn successfully, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL StretchImageListImage(CDC* pDC, CRect rcDest, CImageList* pImageList,
												int nImage);

	//-----------------------------------------------------------------------
	// Summary:
	//	   Processes all pending WM_PAINT message in the message queue of the calling
	//     thread. Ensures the painting never causes deadlock.
	// Parameters:
	//	   hWnd - Window handle for which the painting messages have to be processed,
	//		      or NULL to process painting message for all thread windows.
	// Returns:
	//	   FALSE if message loop is broken due to either WM_QUIT or error in ::GetMessage.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL ProcessPendingPaintMessages(HWND hWnd = NULL);

	//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATED_IN_FAVOR2(XTPSystemVersion, GetComCtlVersion)
	static DWORD AFX_CDECL GetComCtlVersion();

	_XTP_DEPRECATED_IN_FAVOR2(XTPMultiMonitor, GetWorkArea)
	static CRect AFX_CDECL GetWorkArea();

	_XTP_DEPRECATED_IN_FAVOR2(XTPMultiMonitor, GetWorkArea)
	static CRect AFX_CDECL GetWorkArea(const POINT& point);

	_XTP_DEPRECATED_IN_FAVOR2(XTPMultiMonitor, GetWorkArea)
	static CRect AFX_CDECL GetWorkArea(LPCRECT rect);

	_XTP_DEPRECATED_IN_FAVOR2(XTPMultiMonitor, GetWorkArea)
	static CRect AFX_CDECL GetWorkArea(const CWnd* pWnd);

	_XTP_DEPRECATED_IN_FAVOR2(XTPMultiMonitor, GetScreenArea)
	static CRect AFX_CDECL GetScreenArea(const CWnd* pWnd);
	//}}AFX_CODEJOCK_PRIVATE

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Auxiliary function for color convertion from HSL to RGB color model.
	// Parameters:
	//     m1 - Input value.
	//     m2 - Input value.
	//     h - Input value.
	// Returns:
	//     A converted value
	// See Also:
	//     RGBtoHSL
	//-----------------------------------------------------------------------
	static int AFX_CDECL HueToRGB(int m1, int m2, int h);
	static double AFX_CDECL HueToRGB(double temp1, double temp2, double temp3);

public:
	PFNALPHABLEND m_pfnAlphaBlend;
	PFNTRANSPARENTBLT m_pfnTransparentBlt;
	PFNGRADIENTFILL m_pfnFastGradientFill;

private:
	HMODULE m_hMsImgDll; // Handle to MsImg32.dll.

	// private members used for draw routines.
	void GradientFillFast(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	void GradientFillSlow(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

	// singleton
	friend _XTP_EXT_CLASS CXTPDrawHelpers* AFX_CDECL XTPDrawHelpers(); // Used to access the
																	   // CXTPDrawHelpers object.
};

//===========================================================================
// Summary:
//     CXTPNoFlickerWnd is a TBase derived general purpose template
//     helper class. The CXTPNoFlickerWnd class is used for drawing
//     flicker free controls. To use, instantiate the template using
//     any CWnd derived class. For example, to create a tab control
//     that is flicker free you would use the following declaration:
// Example:
//     The following example demonstrates how to use CXTPNoFlickerWnd
//     to create a flicker free object.
// <code>
// CXTPNoFlickerWnd <CTabCtrl> m_tabCtrl;
// </code>
//===========================================================================
template<class TBase>
class CXTPNoFlickerWnd : public TBase
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPNoFlickerWnd object.
	//-----------------------------------------------------------------------
	CXTPNoFlickerWnd()
	{
		m_crBack = ::GetSysColor(COLOR_3DFACE);
	}

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the background fill
	//     color for the flicker free control.
	// Parameters:
	//     crBack - An RGB value.
	//-----------------------------------------------------------------------
	void SetBackColor(COLORREF crBack)
	{
		m_crBack = crBack;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to retrieve the background
	//     fill color for the flicker free control.
	// Returns:
	//     An RGB value.
	//-----------------------------------------------------------------------
	COLORREF GetBackColor()
	{
		return m_crBack;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method provides a CE procedure (WindowProc) for a CWnd
	//     object. It dispatches messages through the window message
	//     map. The return value depends on the message.
	// Parameters:
	//     message - Specifies the Windows message to be processed.
	//     wParam  - Provides additional information used in processing
	//               the message. The parameter value depends on the
	//               message.
	//     lParam  - Provides additional information used in processing
	//               the message. The parameter value depends on the
	//               message.
	// Returns:
	//     An LRESULT object.
	//-----------------------------------------------------------------------
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_PAINT:
			{
				CPaintDC dc(this);

				// Get the client rect and paint to a memory device context.
				// This will help reduce screen flicker. Pass the memory
				// device context to the default window procedure to do
				// default painting.

				CRect rc;
				TBase::GetClientRect(&rc);

				CXTPBufferDC memDC(dc.GetSafeHdc(), rc);
				memDC.FillSolidRect(rc, m_crBack);

				return TBase::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
			}

			case WM_ERASEBKGND: { return TRUE;
			}
		}

		return TBase::WindowProc(message, wParam, lParam);
	}

protected:
	COLORREF m_crBack; // An RGB value.
};

//{{AFX_CODEJOCK_PRIVATE
_XTP_EXT_CLASS COLORREF AFX_CDECL XTPOfficeBorder_GetBorderColor(bool bTopLeft);
_XTP_EXT_CLASS void AFX_CDECL XTPOfficeBorder_OnPaint(CWnd* pWnd, COLORREF clr1, COLORREF clr2);
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPOfficeBorder is a template class used for windows that requires
//     a thin border. This class can be used as the base class for any
//     CWnd derived class to display an Office style, non-client border.
// Parameters:
//     TBase     - Object class name.
//     bCalcSize - true to remove non-client edge and adjust borders to 1 pixel,
//                 false to only draw 1 pixel border.
// Example:
//     Sample code demonstrates how to use the CXTPOfficeBorder template
//     class with your base class.
// <code>
// class CTreeCtrlEx : public CXTPOfficeBorder<CXTPTreeCtrl>
// {
//     ...
// };
// </code>
//     Sample code demonstrates how to use the CXTPOfficeBorder template
//     class with a member variable.
// <code>
// class CMyView : public CView
// {
//     ...
// protected:
//     CXTPOfficeBorder<CEdit,false> m_edit;
// };
// </code>
//========================================================================
template<class TBase, bool bCalcSize = true>
class CXTPOfficeBorder : public TBase
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to return the color used for drawing non-client
	//     Office style borders for the active current theme.
	// Parameters:
	//     bTopLeft - true to return the top-left border color,
	//                false to return the bottom-right border color.
	// Returns:
	//     An RGB color value that represents the non-client border color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetBorderColor(bool bTopLeft) const
	{
		return XTPOfficeBorder_GetBorderColor(bTopLeft);
	}

	//{{AFX_CODEJOCK_PRIVATE
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_NCPAINT:
			{
				TBase::WindowProc(message, wParam, lParam);
				XTPOfficeBorder_OnPaint(this, GetBorderColor(true), GetBorderColor(false));
				return 0; // Handled.
			}

			case WM_CREATE:
			{
				if (TBase::WindowProc(message, wParam, lParam) == -1)
					return -1;

#	if (_MSC_VER > 1300)
#		pragma warning(push)
#		pragma warning(disable : 4127) // conditional expression is constant
#	endif
				if (bCalcSize)
#	if (_MSC_VER > 1300)
#		pragma warning(pop)
#	endif
				{
					// remove 3D borders.
					long lStyle = ::GetWindowLong(TBase::m_hWnd, GWL_STYLE) & ~WS_BORDER;
					::SetWindowLong(TBase::m_hWnd, GWL_STYLE, lStyle);

					long lExStyle = ::GetWindowLong(TBase::m_hWnd, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE;
					::SetWindowLong(TBase::m_hWnd, GWL_EXSTYLE, lExStyle);

					// force non-client area to be recalculated.
					::SetWindowPos(TBase::m_hWnd, NULL, NULL, NULL, NULL, NULL,
								   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
									   | SWP_FRAMECHANGED);
				}

				return 0; // Handled.
			}
			break;

			case WM_NCCALCSIZE: { LRESULT lResult = TBase::WindowProc(message, wParam, lParam);

#	if (_MSC_VER > 1300)
#		pragma warning(push)
#		pragma warning(disable : 4127) // conditional expression is constant
#	endif
				if (bCalcSize)
#	if (_MSC_VER > 1300)
#		pragma warning(pop)
#	endif
				{
					NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS FAR*)lParam;

					// adjust non-client area for border space
					lpncsp->rgrc[0].left += 1;
					lpncsp->rgrc[0].top += 1;
					lpncsp->rgrc[0].right -= 1;
					lpncsp->rgrc[0].bottom -= 1;
				}

				return lResult;
			}
		}

		return TBase::WindowProc(message, wParam, lParam);
	}
	//}}AFX_CODEJOCK_PRIVATE
};

//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPDrawHelpers members.
//     Since this class is designed as a single instance object, you can
//     only access its members through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPDrawHelpers.
// Example:
//     <code>BOOL bLowRes = XTPDrawHelpers()->IsLowResolution();</code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPDrawHelpers* AFX_CDECL XTPDrawHelpers();

//---------------------------------------------------------------------------
// Summary:
//    Prepares a printer device context for printing and calculates
//    DEVMODE and DEVNAMES values.
// Parameters:
//    ref_hDevMode  - Reference to a handle of a movable global memory object
//                    that contains a DEVMODE structure.
//    ref_hDevNames - Reference to a handle of a movable global memory object
//                    that contains a DEVNAMES structure.
// Returns:
//    Nonzero if successful, otherwise 0.
// See Also:
//    CWinApp::GetPrinterDeviceDefaults, DEVMODE, DEVNAMES
//---------------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPGetPrinterDeviceDefaults(HGLOBAL& ref_hDevMode,
														  HGLOBAL& ref_hDevNames);

//===========================================================================
// Summary:
//      Helper class which provides currently selected printer information.
//===========================================================================
class _XTP_EXT_CLASS CXTPPrinterInfo : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPPrinterInfo)
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also:
	//     ~CXTPPrinterInfo
	//-------------------------------------------------------------------------
	CXTPPrinterInfo();

	//-------------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also:
	//     CXTPPrinterInfo
	//-------------------------------------------------------------------------
	virtual ~CXTPPrinterInfo();

	//-------------------------------------------------------------------------
	// Summary:
	//     This enum defines some printer properties IDs.
	// See Also:
	//     GetName
	//-------------------------------------------------------------------------
	enum XTPEnumDeviceName
	{
		xtpDevName_Driver = 0, // Define Driver name property.
		xtpDevName_Device = 1, // Define Device name property.
		xtpDevName_Port   = 2  // Define Port name property.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the currently selected printer
	//     Driver name, Device name, or Port name.
	// Parameters:
	//     eNameID - [in] A value from XTPEnumDeviceName enum which specifies
	//               the property.
	// Returns:
	//     A string with the requested name.
	// See Also:
	//     XTPEnumDeviceName
	//-----------------------------------------------------------------------
	CString GetName(XTPEnumDeviceName eNameID);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves name offset by name ID.
	// Parameters:
	//     pDevNames - [in] A pointer to a DEVNAMES structure.
	//     eNameID   - [in] Name ID.
	// Returns:
	//     Name offset in chars.
	// See Also:
	//     XTPEnumDeviceName, DEVNAMES
	//-----------------------------------------------------------------------
	WORD _GetNameOffset(LPDEVNAMES pDevNames, XTPEnumDeviceName eNameID);

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPPrinterInfo);

	afx_msg BSTR OleGetDeviceName();
	afx_msg BSTR OleGetDriverName();
	afx_msg BSTR OleGetPortName();
	//}}AFX_CODEJOCK_PRIVATE
#	endif
};

//===========================================================================
// Summary:
//     This class is used to define a printed page header or footer.
//     Also some calculation and drawing functionalities are provided.
//     Used as a member of printing options.
// See Also:
//     CXTPPrintOptions
//===========================================================================
class _XTP_EXT_CLASS CXTPPrintPageHeaderFooter : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPPrintOptions;
	DECLARE_DYNAMIC(CXTPPrintPageHeaderFooter)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Object constructor.
	// Parameters:
	//     pOwner  - A pointer to the owner object.
	//     bHeader - TRUE if the object represents a header,
	//               FALSE if the object represents a footer.
	// See Also:
	//     CXTPPrintOptions
	//-----------------------------------------------------------------------
	CXTPPrintPageHeaderFooter(CXTPPrintOptions* pOwner, BOOL bHeader);

	//-------------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also:
	//     CXTPPrintPageHeaderFooter
	//-------------------------------------------------------------------------
	virtual ~CXTPPrintPageHeaderFooter();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to format a header or footer using
	//     the format string specified in the m_strFormatString member.
	//     The formatted text is copied into the following members:
	//     m_strLeft, m_strCenter, m_strRight.
	//     If m_strFormatString is empty, then this function does nothing.
	// Parameters:
	//     pInfo        - [in] Printing context.
	//     pcszWndTitle - [in] Pointer to window title (for &w format string specifier).
	//     nVirtualPage - int number of page (e.g. in horizontal direction for wide data)
	// Remarks:
	//     FormatTexts should be called before Draw call. Format specifiers
	//     are the same as in Internet Explorer:
	//
	//     * <b>&d</b>: Date in short format (as specified by Regional Settings
	//                  in Control Panel).
	//     * <b>&D</b>: Date in long format (as specified by Regional Settings
	//                  in Control Panel).
	//
	//     * <b>&t</b>: Time in the format specified by Regional Settings in Control Panel.
	//     * <b>&T</b>: Time in 24-hour format.
	//
	//     * <b>&p</b>: Current page number.
	//     * <b>&P</b>: Total number of pages.
	//
	//     * <b>&b</b>: The text immediately following these characters as centered.
	//     * <b>&b&b</b>: The text immediately following the first "&b" as centered, and
	//                    the text following the second "&b" as right-justified.
	//
	//     * <b>&w</b>: Window title.
	//
	//     * <b>&&</b>: A single ampersand (&).
	//     * <b>\\n</b>: New line marker.
	//
	// See Also:
	//     m_strFormatString, m_strLeft, m_strCenter, m_strRight
	//-----------------------------------------------------------------------
	virtual void FormatTexts(CPrintInfo* pInfo, LPCTSTR pcszWndTitle, int nVirtualPage = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a header or footer on the device context.
	// Parameters:
	//     pDC             - [in] A pointer to a device context.
	//     rcRect          - [in, out] A reference to a bounding rectangle.
	//                       After the function call, this member contains a rect
	//                       necessary to draw a header or footer. A header
	//                       is aligned to the top of the passed bounding rectangle,
	//                       while a footer is aligned to the bottom.
	//     bCalculateOnly  - TRUE to calculate the rectangle without drawing.
	// See Also:
	//     FormatTexts, m_strLeft, m_strCenter, m_strRight
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CRect& rcRect, BOOL bCalculateOnly = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to copy members from a specified source object.
	// Parameters:
	//     pSrc - A pointer to the source object to copy data from.
	// See Also:
	//     Clear
	//-----------------------------------------------------------------------
	virtual void Set(const CXTPPrintPageHeaderFooter* pSrc);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member function to clear all members' values.
	// See Also:
	//     Set
	//-------------------------------------------------------------------------
	virtual void Clear();

	//-------------------------------------------------------------------------
	// Summary:
	//     Checks if all strings are empty.
	// Returns:
	//     TRUE if all strings are empty.
	//-------------------------------------------------------------------------
	virtual BOOL IsEmpty();

	LOGFONT m_lfFont;		   // Font details.
	COLORREF m_clrColor;	   // Text color.
	CString m_strFormatString; // Text as Format string.

	CString m_strLeft;   // A string to show with left align.
	CString m_strCenter; // A string to show with center align.
	CString m_strRight;  // A string to show with right align.

	//{{AFX_CODEJOCK_PRIVATE
	static CString AFX_CDECL GetParentFrameTitle(CWnd* pWnd);
	static void AFX_CDECL DoInsertHFFormatSpecifierViaMenu(CWnd* pParent, CEdit* pEdit,
														   CButton* pButton);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bHeader;				// If TRUE, the object represents a Header. Otherwise footer.
	CXTPPrintOptions* m_pOwner; // Store pointer to owner object.

	//{{AFX_CODEJOCK_PRIVATE
	virtual int Calc3ColSizes(CDC* pDC, int nW, const CString& str1, const CString& str2,
							  const CString& str3, CSize& rsz1, CSize& rsz2, CSize& rsz3);
	virtual int _Calc3ColSizesIfSingleCol(CDC* pDC, int nW, const CString& str1,
										  const CString& str2, const CString& str3, CSize& rsz1,
										  CSize& rsz2, CSize& rsz3);

	virtual void _SplitFormatLCR(CString strFormat);
	virtual void _FormatDateTime(CString& strFormat, LCID lcidLocale);
	//}}AFX_CODEJOCK_PRIVATE

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE
	afx_msg LPFONTDISP OleGetFont();
	afx_msg void OleSetFont(LPFONTDISP pFontDisp);
	afx_msg void OleClear();

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPPrintPageHeaderFooter);
	//}}AFX_CODEJOCK_PRIVATE
#	endif

private:
	int fnYi(int Xi, int Wi);
};

//-------------------------------------------------------------------------
// Summary:
//     Obtains a native handle of a specified object.
// Returns:
//     A native handle value.
//-------------------------------------------------------------------------
AFX_INLINE HDC XTPGetHandle(CDC* pDC)
{
	ASSERT(NULL != pDC);
	return pDC->m_hDC;
}

AFX_INLINE HDC XTPGetHandle(CDC& dc) // <combine XTPGetHandle@CDC*>
{
	return dc.m_hDC;
}

AFX_INLINE HDC XTPGetHandle(HDC hDC) // <combine XTPGetHandle@CDC*>
{
	return hDC;
}

//-------------------------------------------------------------------------
// Summary:
//     Specifies process DPI awareness level. Refer to platform SDK
//     documentation for its analogue called PROCESS_DPI_AWARENESS.
//-------------------------------------------------------------------------
enum XTPDpiAwareness
{
	xtpDpiUnaware		  = 0, // Same as PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE.
	xtpDpiSystemAware	 = 1, // Same as PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE.
	xtpDpiPerMonitorAware = 2  // Same as PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE.
};

//-------------------------------------------------------------------------
// Summary:
//     This helper class is used in drawing when DPI is different than 96.
// Remarks:
//     relative pixel = 1 pixel at 96 DPI and scaled based on actual DPI.
//
//     Original code comes from:
//     http://msdn.microsoft.com/en-us/library/windows/desktop/dd464660(v=vs.85).aspx
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDpi
{
	friend class CXTPSingleton<CXTPDpi>;

public:
	enum
	{
		DefaultDpi = 96 // Normal DPI.
	};

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a DPI helper object that works with global DPI settings.
	//-------------------------------------------------------------------------
	CXTPDpi();

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a DPI helper object that works with the
	//     provided device context's settings.
	// Parameters:
	//	   hDC - Device context with DPI settings to be used.
	//-------------------------------------------------------------------------
	CXTPDpi(HDC hDC);

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a DPI helper object that works with the
	//     provided window's device context's settings.
	// Parameters:
	//	   hWnd - Window handle with DPI settings to be used.
	//-------------------------------------------------------------------------
	CXTPDpi(HWND hWnd);

	//-------------------------------------------------------------------------
	// Summary:
	//     Handles object destruction.
	//-------------------------------------------------------------------------
	~CXTPDpi();

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets process DPI awareness level. Whenever possible, prefer setting
	//     DPI awareness level in the application's manifest file. Refer to
	//     platform SDK's documentation for SetProcessDpiAwareness and
	//     SetProcessDpiAware details and supported OS versions.
	// Parameters:
	//     dpiAwareness - DPI awareness level that is to be set only if
	//                    it is supported by the OS.
	// Returns:
	//     TRUE if successful.
	//-------------------------------------------------------------------------
	BOOL SetDpiAwareness(XTPDpiAwareness dpiAwareness);

	//-------------------------------------------------------------------------
	// Summary:
	//     Obtains process DPI awareness level if possible. Refer to platform
	//     SDK's documentation for GetProcessDpiAwareness details
	//     and supported OS versions.
	// Returns:
	//     DPI awareness level enabled for the calling process.
	//-------------------------------------------------------------------------
	XTPDpiAwareness GetDpiAwareness() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Processes a DPI related message. Must not be called for DPI helper
	//     objects specific to a device context or window handle.
	// Parameters:
	//     hWnd - Window handle.
	//     message - Message code.
	//     wParam - Message argument.
	//     lParam - Message argument.
	//     pResult - Pointer to the result of message processing.
	// Returns:
	//     TRUE if the message has been processed.
	//-------------------------------------------------------------------------
	BOOL OnWndMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	//-------------------------------------------------------------------------
	// Summary:
	//     Obtains horizontal and vertical DPI values for the system, device
	//     context, or window handle.
	// Returns:
	//     DPI values.
	//-------------------------------------------------------------------------
	int GetDPIX() const;
	int GetDPIY() const; // <combine CXTPDpi::GetDPIX>

	//-------------------------------------------------------------------------
	// Summary:
	//     Determines if system, device context, or window handle DPI settings are default.
	// Returns:
	//     TRUE if DPI setting are default for the target context.
	//-------------------------------------------------------------------------
	BOOL IsDefaultDpiX() const;
	BOOL IsDefaultDpiY() const; // <combine CXTPDpi::IsDefaultDpiX>
	BOOL IsDefaultDpi() const;  // <combine CXTPDpi::IsDefaultDpiX>

	//-----------------------------------------------------------------------
	// Summary:
	//	   Gets/sets global global DPI bitmap scaling flag for the current process.
	//	   The controls can use this value as a default value for a control specific
	// 	   DPI bitmap flag flag.
	// Parameters:
	//     bEnable - TRUE to enable bitmaps scaling, FALSE to disable.
	// Returns:
	//     The current value of the bitmap scaling flag.
	//-----------------------------------------------------------------------
	BOOL IsDpiBitmapScalingEnabled() const;
	void EnableDpiBitmapScaling(BOOL bEnable = TRUE); // <combine
													  // CXTPDpi::IsDpiBitmapScalingEnabled@const>

	//-------------------------------------------------------------------------
	// Summary:
	//     Perform scalling/unscalling of X, Y coordinates, rectangle,
	//     size, and point objects according to active DPI settings.
	// Parameters:
	//     x       - X-coordinate to scale/unscale using current horizontal DPI settings.
	//     y       - Y-coordinate to scale/unscale using current vertical DPI settings.
	//     pRect   - A pointer to a rectangle object to scale/unscale using current DPI settings.
	//     rc      - A reference to a rectangle object to scale/unscale using current DPI settings.
	//     pPoint  - A pointer to a point object to scale/unscale using current DPI settings.
	//     pt      - A reference to a point object to scale/unscale using current DPI settings.
	//     pSize   - A pointer to a size object to scale/unscale using current DPI settings.
	//     sz      - A reference to a size object to scale/unscale using current DPI settings.
	//     iDpiScaleAbove - Scaled only if current DPI is larger than specified.
	// Returns:
	//     Scaled/unscaled value.
	//-------------------------------------------------------------------------
	int ScaleX(int x, int iDpiScaleAbove = CXTPDpi::DefaultDpi) const;
	int ScaleY(int y,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	long ScaleX(long x, int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	long ScaleY(long y,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	float ScaleX(float x,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	float ScaleY(float y,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	double ScaleX(double x,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	double ScaleY(double y,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	unsigned int ScaleX(unsigned int x,
						int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	unsigned int ScaleY(unsigned int y,
						int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	int UnscaleX(int x, int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	int UnscaleY(int y, int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	long UnscaleX(long x,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	long UnscaleY(long y,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	float UnscaleX(float x,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	float UnscaleY(float y,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	double UnscaleX(double x,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	double UnscaleY(double y,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	unsigned int UnscaleX(unsigned int x, int iDpiScaleAbove = CXTPDpi::DefaultDpi)
		const; // <combine CXTPDpi::ScaleX@int@int>
	unsigned int UnscaleY(unsigned int y, int iDpiScaleAbove = CXTPDpi::DefaultDpi)
		const; // <combine CXTPDpi::ScaleX@int@int>

	int ScaleX(HDC hDC, int x,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	int ScaleY(HDC hDC, int y,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	long ScaleX(HDC hDC, long x,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	long ScaleY(HDC hDC, long y,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	float ScaleX(HDC hDC, float x,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	float ScaleY(HDC hDC, float y,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	double ScaleX(HDC hDC, double x,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	double ScaleY(HDC hDC, double y,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	unsigned int ScaleX(HDC hDC, unsigned int x,
						int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	unsigned int ScaleY(HDC hDC, unsigned int y,
						int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	int UnscaleX(HDC hDC, int x,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	int UnscaleY(HDC hDC, int y,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	long UnscaleX(HDC hDC, long x,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	long UnscaleY(HDC hDC, long y,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	float UnscaleX(HDC hDC, float x,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	float UnscaleY(HDC hDC, float y,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	double UnscaleX(HDC hDC, double x,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	double UnscaleY(HDC hDC, double y,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	unsigned int UnscaleX(HDC hDC, unsigned int x, int iDpiScaleAbove = CXTPDpi::DefaultDpi)
		const; // <combine CXTPDpi::ScaleX@int@int>
	unsigned int UnscaleY(HDC hDC, unsigned int y, int iDpiScaleAbove = CXTPDpi::DefaultDpi)
		const; // <combine CXTPDpi::ScaleX@int@int>

	void ScaleRect(RECT* pRect,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	RECT ScaleRect(const RECT& rc,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	void UnscaleRect(RECT* pRect,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	RECT UnscaleRect(const RECT& rc,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	void Scale(RECT* pRect,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	RECT Scale(const RECT& rc,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	void Unscale(RECT* pRect,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	RECT Unscale(const RECT& rc,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	void ScaleRect(CRect* pRect,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	CRect ScaleRect(const CRect& rc,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	void UnscaleRect(CRect* pRect,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	CRect UnscaleRect(const CRect& rc,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	void Scale(CRect* pRect,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	CRect Scale(const CRect& rc,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	void Unscale(CRect* pRect,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	CRect Unscale(const CRect& rc,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>

	void ScaleRect(HDC hDC, RECT* pRect,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	RECT ScaleRect(HDC hDC, const RECT& rc,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	void UnscaleRect(HDC hDC, RECT* pRect,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	RECT UnscaleRect(HDC hDC, const RECT& rc,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	void Scale(HDC hDC, RECT* pRect,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	RECT Scale(HDC hDC, const RECT& rc,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	void Unscale(HDC hDC, RECT* pRect,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	RECT Unscale(HDC hDC, const RECT& rc,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	void ScaleRect(HDC hDC, CRect* pRect,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	CRect ScaleRect(HDC hDC, const CRect& rc,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	void UnscaleRect(HDC hDC, CRect* pRect,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	CRect UnscaleRect(HDC hDC, const CRect& rc,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	void Scale(HDC hDC, CRect* pRect,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	CRect Scale(HDC hDC, const CRect& rc,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	void Unscale(HDC hDC, CRect* pRect,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	CRect Unscale(HDC hDC, const CRect& rc,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>

	void ScalePoint(POINT* pPoint,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	POINT ScalePoint(const POINT& pt,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	void UnscalePoint(POINT* pPoint,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	POINT UnscalePoint(const POINT& pt,
					   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		// CXTPDpi::ScaleX@int@int>
	void Scale(POINT* pPoint,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	POINT Scale(const POINT& pt,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	void Unscale(POINT* pPoint,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	POINT Unscale(const POINT& pt,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	void ScalePoint(CPoint* pPoint,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	CPoint ScalePoint(const CPoint& pt,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	void UnscalePoint(CPoint* pPoint,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	CPoint UnscalePoint(const CPoint& pt,
						int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	void Scale(CPoint* pPoint,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	CPoint Scale(const CPoint& pt,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	void Unscale(CPoint* pPoint,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	CPoint Unscale(const CPoint& pt,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>

	void ScalePoint(HDC hDC, POINT* pPoint,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	POINT ScalePoint(HDC hDC, const POINT& pt,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	void UnscalePoint(HDC hDC, POINT* pPoint,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	POINT UnscalePoint(HDC hDC, const POINT& pt,
					   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		// CXTPDpi::ScaleX@int@int>
	void Scale(HDC hDC, POINT* pPoint,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	POINT Scale(HDC hDC, const POINT& pt,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	void Unscale(HDC hDC, POINT* pPoint,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	POINT Unscale(HDC hDC, const POINT& pt,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>
	void ScalePoint(HDC hDC, CPoint* pPoint,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	CPoint ScalePoint(HDC hDC, const CPoint& pt,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	void UnscalePoint(HDC hDC, CPoint* pPoint,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	CPoint UnscalePoint(HDC hDC, const CPoint& pt,
						int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																		 // CXTPDpi::ScaleX@int@int>
	void Scale(HDC hDC, CPoint* pPoint,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	CPoint Scale(HDC hDC, const CPoint& pt,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	void Unscale(HDC hDC, CPoint* pPoint,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	CPoint Unscale(HDC hDC, const CPoint& pt,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>

	void ScaleSize(SIZE* pSize,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	SIZE ScaleSize(const SIZE& sz,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	void UnscaleSize(SIZE* pSize,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	SIZE UnscaleSize(const SIZE& sz,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	void Scale(SIZE* pSize,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	SIZE Scale(const SIZE& sz,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	void Unscale(SIZE* pSize,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	SIZE Unscale(const SIZE& sz,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	void ScaleSize(CSize* pSize,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	CSize ScaleSize(const CSize& sz,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	void UnscaleSize(CSize* pSize,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	CSize UnscaleSize(const CSize& sz,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	void Scale(CSize* pSize,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	CSize Scale(const CSize& sz,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	void Unscale(CSize* pSize,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	CSize Unscale(const CSize& sz,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>

	void ScaleSize(HDC hDC, SIZE* pSize,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	SIZE ScaleSize(HDC hDC, const SIZE& sz,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	void UnscaleSize(HDC hDC, SIZE* pSize,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	SIZE UnscaleSize(HDC hDC, const SIZE& sz,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	void Scale(HDC hDC, SIZE* pSize,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	SIZE Scale(HDC hDC, const SIZE& sz,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	void Unscale(HDC hDC, SIZE* pSize,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	SIZE Unscale(HDC hDC, const SIZE& sz,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	void ScaleSize(HDC hDC, CSize* pSize,
				   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	// CXTPDpi::ScaleX@int@int>
	CSize ScaleSize(HDC hDC, const CSize& sz,
					int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	 // CXTPDpi::ScaleX@int@int>
	void UnscaleSize(HDC hDC, CSize* pSize,
					 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	  // CXTPDpi::ScaleX@int@int>
	CSize UnscaleSize(HDC hDC, const CSize& sz,
					  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																	   // CXTPDpi::ScaleX@int@int>
	void Scale(HDC hDC, CSize* pSize,
			   int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine CXTPDpi::ScaleX@int@int>
	CSize Scale(HDC hDC, const CSize& sz,
				int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																 // CXTPDpi::ScaleX@int@int>
	void Unscale(HDC hDC, CSize* pSize,
				 int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																  // CXTPDpi::ScaleX@int@int>
	CSize Unscale(HDC hDC, const CSize& sz,
				  int iDpiScaleAbove = CXTPDpi::DefaultDpi) const; // <combine
																   // CXTPDpi::ScaleX@int@int>

	//-------------------------------------------------------------------------
	// Summary:
	//     Selects one of the provided values that is specific to the current screen DPI.
	// Parameters:
	//     upToDpi120 - The value that is selected if the current DPI is in range [96-120).
	//     upToDpi144 - The value that is selected if the current DPI is in range [120-144).
	//     upToDpi192 - The value that is selected if the current DPI is in range [144-192).
	//     theLargest - The value that is selected if the current DPI is in range [192-*).
	// Returns:
	//     Scaled screen size according to active DPI settings.
	//-------------------------------------------------------------------------
	int SelectDpiSpecific(int upToDpi120, int upToDpi144, int upToDpi192, int theLargest) const;
	LPCTSTR SelectDpiSpecific(
		LPCTSTR upToDpi120, LPCTSTR upToDpi144, LPCTSTR upToDpi192,
		LPCTSTR theLargest) const; // <combine CXTPDpi::SelectDpiSpecific@int@int@int@int>
	void* SelectDpiSpecific(void* upToDpi120, void* upToDpi144, void* upToDpi192,
							void* theLargest) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Scale screen width and height according to active DPI settings.
	// Returns:
	//     Scaled screen size according to active DPI settings.
	//-------------------------------------------------------------------------
	int ScaledScreenWidth() const;
	int ScaledScreenHeight() const; // <combine CXTPDpi::ScaledScreenWidth@const>

	//-------------------------------------------------------------------------
	// Summary:
	//     Determines if screen resolution meets minimum requirements in relative pixels.
	// Returns:
	//     TRUE if screen resolution meets minimum requirements in relative pixels.
	//-------------------------------------------------------------------------
	bool IsResolutionAtLeast(int cxMin, int cyMin) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Convert a point size (1/72 of an inch) to raw pixels.
	// Returns:
	//     Converted point size.
	//-------------------------------------------------------------------------
	int PointsToPixels(int pt) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Invalidates any cached metrics.
	//-------------------------------------------------------------------------
	void Invalidate();

	//-------------------------------------------------------------------------
	// Summary:
	//     Tries to obtain physical scaling factors for a window regardless of window
	//     or process DPI awareness.
	// Parameters:
	//     hWnd - Windows handle for which physical scaling factors are to be obtained.
	//     xScale - Gets set to the horizontal physical scaling factor
	//              upon successful completion.
	//     yScale - Gets set to the vertical physical scaling factor
	//              upon successful completion.
	// Returns:
	//     TRUE if the physical scaling factors are obtained successfully.
	//-------------------------------------------------------------------------
	BOOL GetPhysicalWindowScaleFactors(HWND hWnd, double& xScale, double& yScale) const;

private:
	void Construct(HDC hDC = NULL, HWND hWnd = NULL);
	void Init(int dpiX = 0, int dpiY = 0, BOOL bUpdate = FALSE);
	void InitDpiAwareness();
	int ScaleSystemMetricX(int nIndex) const;
	int ScaleSystemMetricY(int nIndex) const;

	LRESULT OnWmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmDpiChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	enum
	{
		DpiValueCount = 0x100
	};

	BOOL m_bInitialized;
	BOOL m_bSupportsDpiChange;

	HDC m_hDC;
	HWND m_hWnd;
	BOOL m_bReleaseDC;
	BOOL m_bDpiBitmapScaling;

	int m_dpiX;
	int m_dpiY;

	POINT m_nPositiveDpiValues[DpiValueCount + 1];
	POINT m_nNegativeDpiValues[DpiValueCount + 1];

	static BOOL m_bDpiAwarenessInitialized;
	typedef HRESULT(WINAPI* PFNSetProcessDpiAwareness)(XTPDpiAwareness dpiAwareness);
	typedef HRESULT(WINAPI* PFNGetProcessDpiAwareness)(HANDLE hProcess,
													   XTPDpiAwareness* dpiAwareness);
	static PFNSetProcessDpiAwareness m_pfnSetProcessDpiAwareness;
	static PFNGetProcessDpiAwareness m_pfnGetProcessDpiAwareness;

	typedef BOOL(WINAPI* PFNSetProcessDPIAware)();
	static PFNSetProcessDPIAware m_pfnSetProcessDPIAware;
	static BOOL m_bSetProcessDPIAwareResult;

	typedef HANDLE(WINAPI* PFNMonitorFromWindow)(HWND hWnd, DWORD dwFlags);
	static PFNMonitorFromWindow m_pfnMonitorFromWindow;

	enum MonitorDpiType
	{
		xtpEffectiveDpiType = 0,				  // Same as MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI.
		xtpAngularDpiType   = 1,				  // Same as MONITOR_DPI_TYPE::MDT_ANGULAR_DPI.
		xtpRawDpiType		= 2,				  // Same as MONITOR_DPI_TYPE::MDT_RAW_DPI.
		xtpDefaultDpiType   = xtpEffectiveDpiType // Same as MONITOR_DPI_TYPE::MDT_DEFAULT.
	};

	typedef HRESULT(WINAPI* PFNGetDpiForMonitor)(HANDLE hMonitor, MonitorDpiType dpiType,
												 UINT* dpiX, UINT* dpiY);
	static PFNGetDpiForMonitor m_pfnGetDpiForMonitor;
};

AFX_INLINE BOOL CXTPDpi::IsDpiBitmapScalingEnabled() const
{
	return m_bDpiBitmapScaling;
}

AFX_INLINE void CXTPDpi::EnableDpiBitmapScaling(BOOL bEnable /*= TRUE*/)
{
	m_bDpiBitmapScaling = bEnable;
}

//-------------------------------------------------------------------------
// Summary:
//     Provides access to a global DPI helper object instance.
// Returns:
//     A pointer to a global DPI helper object instance.
//-------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPDpi* AFX_CDECL XTPDpiHelper();

//-------------------------------------------------------------------------
// Summary:
//     Helper macros for scaling of x- and y- coordinates, rectangle,
//     size, and point objects according to active DPI settings.
//-------------------------------------------------------------------------
#	define XTP_DPI_X(n) XTPDpiHelper()->ScaleX(n)
#	define XTP_DPI_Y(n) XTPDpiHelper()->ScaleY(n)	// <combine XTP_DPI_X>
#	define XTP_DPI(what) XTPDpiHelper()->Scale(what) // <combine XTP_DPI_X>
#	define XTP_DPI_RECT(rc) XTP_DPI(rc)			  // <combine XTP_DPI_X>
#	define XTP_DPI_POINT(pt) XTP_DPI(pt)			  // <combine XTP_DPI_X>
#	define XTP_DPI_SIZE(sz) XTP_DPI(sz)			  // <combine XTP_DPI_X>
#	define XTP_DC_DPI_X(dc, n) XTPDpiHelper()->ScaleX(XTPGetHandle(dc), n)
#	define XTP_DC_DPI_Y(dc, n) XTPDpiHelper()->ScaleY(XTPGetHandle(dc), n) // <combine XTP_DPI_X>
#	define XTP_DC_DPI(dc, what)                                                                   \
		XTPDpiHelper()->Scale(XTPGetHandle(dc), what)   // <combine XTP_DPI_X>
#	define XTP_DC_DPI_RECT(dc, rc) XTP_DC_DPI(dc, rc)  // <combine XTP_DPI_X>
#	define XTP_DC_DPI_POINT(dc, pt) XTP_DC_DPI(dc, pt) // <combine XTP_DPI_X>
#	define XTP_DC_DPI_SIZE(dc, sz) XTP_DC_DPI(dc, sz)  // <combine XTP_DPI_X>

//-------------------------------------------------------------------------
// Summary:
//     Helper macros for unscaling of x- and y- coordinates, rectangle,
//     size, and point objects according to active DPI settings.
//-------------------------------------------------------------------------
#	define XTP_UNDPI_X(n) XTPDpiHelper()->UnscaleX(n)
#	define XTP_UNDPI_Y(n) XTPDpiHelper()->UnscaleY(n)	// <combine XTP_UNDPI_X>
#	define XTP_UNDPI(what) XTPDpiHelper()->Unscale(what) // <combine XTP_UNDPI_X>
#	define XTP_UNDPI_RECT(rc) XTP_UNDPI(rc)			  // <combine XTP_UNDPI_X>
#	define XTP_UNDPI_POINT(pt) XTP_UNDPI(pt)			  // <combine XTP_UNDPI_X>
#	define XTP_UNDPI_SIZE(sz) XTP_UNDPI(sz)			  // <combine XTP_UNDPI_X>
#	define XTP_DC_UNDPI_X(dc, n) XTPDpiHelper()->UnscaleX(XTPGetHandle(dc), n)
#	define XTP_DC_UNDPI_Y(dc, n)                                                                  \
		XTPDpiHelper()->UnscaleY(XTPGetHandle(dc), n) // <combine XTP_UNDPI_X>
#	define XTP_DC_UNDPI(dc, what)                                                                 \
		XTPDpiHelper()->Unscale(XTPGetHandle(dc), what)		// <combine XTP_UNDPI_X>
#	define XTP_DC_UNDPI_RECT(dc, rc) XTP_DC_UNDPI(dc, rc)  // <combine XTP_UNDPI_X>
#	define XTP_DC_UNDPI_POINT(dc, pt) XTP_DC_UNDPI(dc, pt) // <combine XTP_UNDPI_X>
#	define XTP_DC_UNDPI_SIZE(dc, sz) XTP_DC_UNDPI(dc, sz)  // <combine XTP_UNDPI_X>

//-------------------------------------------------------------------------
// Summary:
//     Identifies supported primitive symbols rendered from internal primitive font.
//-------------------------------------------------------------------------
XTP_DECLARE_ENUM(XTPPrimitiveSymbol){
	xtpPrimitiveSymbolInvalid = -1, xtpPrimitiveSymbolFirst = 0x41,

	// Common glyphs.
	xtpPrimitiveSymbolMinimize = xtpPrimitiveSymbolFirst, xtpPrimitiveSymbolMaximize,
	xtpPrimitiveSymbolRestore, xtpPrimitiveSymbolQuestionMark, xtpPrimitiveSymbolClose,
	xtpPrimitiveSymbolHome, xtpPrimitiveSymbolCheckMark, xtpPrimitiveSymbolCheckMarkRTL,
	xtpPrimitiveSymbolRadioMark,

	// Normal Play glyph.
	xtpPrimitiveSymbolPlayLeft, xtpPrimitiveSymbolPlayRight, xtpPrimitiveSymbolPlayUp,
	xtpPrimitiveSymbolPlayDown,

	// Small Play glyph.
	xtpPrimitiveSymbolPlayLeft2, xtpPrimitiveSymbolPlayRight2, xtpPrimitiveSymbolPlayUp2,
	xtpPrimitiveSymbolPlayDown2,

	// Rewind glyph.
	xtpPrimitiveSymbolRewindLeft, xtpPrimitiveSymbolRewindRight, xtpPrimitiveSymbolRewindUp,
	xtpPrimitiveSymbolRewindDown,

	// Shift glyph.
	xtpPrimitiveSymbolShiftLeft = 0x61, xtpPrimitiveSymbolShiftRight, xtpPrimitiveSymbolShiftDown,
	xtpPrimitiveSymbolShiftUp,

	// Eject glyph.
	xtpPrimitiveSymbolEjectLeft, xtpPrimitiveSymbolEjectRight, xtpPrimitiveSymbolEjectUp,
	xtpPrimitiveSymbolEjectDown,

	// Inject glyph.
	xtpPrimitiveSymbolInjectLeft, xtpPrimitiveSymbolInjectRight, xtpPrimitiveSymbolInjectUp,
	xtpPrimitiveSymbolInjectDown,

	// Pin glyph.
	xtpPrimitiveSymbolPinLeft, xtpPrimitiveSymbolPinRight, xtpPrimitiveSymbolPinDown,

	xtpPrimitiveSymbolPlus, xtpPrimitiveSymbolMinus, xtpPrimitiveSymbolEllipsis,

	xtpPrimitiveSymbolTabClose, xtpPrimitiveSymbolTabPlayLeft, xtpPrimitiveSymbolTabPlayRight,
	xtpPrimitiveSymbolTabPlayLeftDisabled, xtpPrimitiveSymbolTabPlayRightDisabled,

	xtpPrimitiveSymbolLast = xtpPrimitiveSymbolTabPlayRightDisabled
};

//-------------------------------------------------------------------------
// Summary:
//     Provides tools for drawing predefined primitives.
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPPrimitiveDrawer
{
	friend class CXTPSingleton<CXTPPrimitiveDrawer>;

	CXTPPrimitiveDrawer();
	~CXTPPrimitiveDrawer();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Draws a primitive symbol.
	// Parameters:
	//	   pDC - Target device context pointer.
	//	   nSymbol - Primitive symbol identifier.
	//	   rc - Target rectangle.
	//	   crForeColor - Foreground color value, or COLORREF_NULL if left unchanged.
	//	   crBackColor - Background color value, or COLORREF_NULL if left unchanged.
	//	   bOutline - If TRUE, the primitive symbol will be outlined instead of filled.
	//	   hOutlinePen - If bOutline is TRUE, this argument specifies outline pen.
	// Remarks:
	//	   Starting from Windows 10, when the Untrusted Font Blocking feature was added to
	//	   the OS, this method may stop working which results in no primitives being drawn.
	//	   The Untrusted Font Blocking feature prevents applications from loading memory
	//	   fonts which is required for this method to work correctly.
	//
	//	   The issue can be fixed in 3 possible ways:
	//     1. Installing the Codejock.ttf file into %WINDIR%\\Fonts of the host OS
	//        prior to executing an application that draws Codejock primitives.
	//     2. Less recommended. Exclude the application process from Untrusted Font
	//        Blocking by adding a new empty registry key HKEY_LOCAL_MACHINE\\
	//        Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution
	//        Options\\<process_image_name>.
	//     3. Not recommended. Disabling Untrusted Font Blocking in Group Policy
	//        editor (dpedit.msc): Computer Configuration\\Administrative
	//        Templates\\System\\Mitigation Options\\Untrusted Font Blocking.
	//
	//	   Read more about Untrusted Font Blocking at:
	//	   https://docs.microsoft.com/en-us/windows/threat-protection/block-untrusted-fonts-in-enterprise
	//-------------------------------------------------------------------------
	void DrawSymbol(CDC* pDC, XTPPrimitiveSymbol nSymbol, const CRect& rc,
					COLORREF crForeColor = COLORREF_NULL, COLORREF crBackColor = COLORREF_NULL,
					BOOL bOutline = FALSE, HPEN hOutlinePen = NULL);

private:
	void InitFont();
	void InitMemFont();

	CRITICAL_SECTION m_fontAccess;
	CXTPFont m_fntCodejock;
	LOGFONT m_lfCodejock;
	HANDLE m_hCodejockMemFont;
	BOOL m_bCodejockFontLoadingAttempted;

	typedef HANDLE(WINAPI* PFNAddFontMemResourceEx)(PVOID, DWORD, PVOID, DWORD*);
	typedef BOOL(WINAPI* PFNRemoveFontMemResourceEx)(HANDLE);

	PFNAddFontMemResourceEx m_pfnAddFontMemResourceEx;
	PFNRemoveFontMemResourceEx m_pfnRemoveFontMemResourceEx;
};

//-------------------------------------------------------------------------
// Summary:
//     Provides access to a global primitive drawer instance.
// See Also:
//     CXTPPrimitiveDrawer
//-------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPPrimitiveDrawer* AFX_CDECL XTPPrimitiveDrawer();

//-------------------------------------------------------------------------
// Summary:
//     This class is used as a base class to store printing options.
//     It contains base properties and operations for printing tasks.
// See Also:
//     CXTPPrintPageHeaderFooter
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPPrintOptions : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPPrintOptions)

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Default Object constructor.
	//-------------------------------------------------------------------------
	CXTPPrintOptions();

	//-------------------------------------------------------------------------
	// Summary:
	//     Default Object destructor.
	//-------------------------------------------------------------------------
	virtual ~CXTPPrintOptions();

	CRect m_rcMargins; // Margins (mm*100 or inches*1000).

	BOOL m_bBlackWhitePrinting; // Store printing mode: Color or Black&White.
	int m_nBlackWhiteContrast;  // Black&White printing contrast: 0 ... 255. (default value is 0).

	//-------------------------------------------------------------------------
	// Summary:
	//     Retrieves page header properties.
	// Returns:
	//     Page header properties.
	// See Also:
	//     GetPageFooter
	//-------------------------------------------------------------------------
	CXTPPrintPageHeaderFooter* GetPageHeader();

	//-------------------------------------------------------------------------
	// Summary:
	//     Retrieves page footer properties.
	// Returns:
	//     Page footer properties.
	// See Also:
	//     GetPageHeader
	//-------------------------------------------------------------------------
	CXTPPrintPageHeaderFooter* GetPageFooter();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to copy members from a specified source object.
	// Parameters:
	//     pSrc - A pointer to a source object to copy data from.
	//-----------------------------------------------------------------------
	void Set(const CXTPPrintOptions* pSrc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if margines are measured
	//     in inches or millimeters (mm*100 or inches*1000).
	// Returns:
	//     TRUE if margins are measured in inches, otherwise FALSE.
	// See Also:
	//     m_rcMargins, GetMarginsHimetric, GetMarginsLP
	//-----------------------------------------------------------------------
	virtual BOOL IsMarginsMeasureInches();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get margins in Himetric units.
	// Returns:
	//     Margins in Himetric units.
	// See Also:
	//     m_rcMargins, GetMarginsLP
	//-----------------------------------------------------------------------
	virtual CRect GetMarginsHimetric();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get margins in Logical points.
	// Parameters:
	//     pDC - [in] A pointer to a device context.
	// Returns:
	//     Margins in Logical points.
	// See Also:
	//     m_rcMargins, GetMarginsHimetric
	//-----------------------------------------------------------------------
	virtual CRect GetMarginsLP(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function in a derived class to retrieve
	//     the active locale ID.
	// Returns:
	//     Locale ID. Base implementation returns LOCALE_USER_DEFAULT.
	//-----------------------------------------------------------------------
	virtual LCID GetActiveLCID();

protected:
	CXTPPrintPageHeaderFooter* m_pPageHeader; // Page header properties.
	CXTPPrintPageHeaderFooter* m_pPageFooter; // Page footer properties.

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()

	afx_msg BOOL OleGetMarginsMeasureMetrics();

	afx_msg BOOL OleGetLandscape();
	afx_msg void OleSetLandscape(BOOL bLandscape);

	afx_msg LPDISPATCH OleGetHeader();
	afx_msg LPDISPATCH OleGetFooter();

	afx_msg LPDISPATCH OleGetPrinterInfo();
	//}}AFX_CODEJOCK_PRIVATE
#	endif
};

//{{AFX_CODEJOCK_PRIVATE
/////////////////////////////////////////////////////////////////////////////

_XTP_EXT_CLASS BOOL CALLBACK _XTPAbortProc(HDC, int);

// Printing Dialog
class _XTP_EXT_CLASS CXTPPrintingDialog : public CDialog
{
public:
	enum
	{
		IDD = AFX_IDD_PRINTDLG
	};

	CXTPPrintingDialog(CWnd* pParent);
	virtual ~CXTPPrintingDialog()
	{
	}

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
};
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPDrawHelpers::IsFastFillAvailable() const
{
	return m_pfnFastGradientFill != NULL;
}
AFX_INLINE void CXTPDrawHelpers::Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2)
{
	CPoint pts[] = { pt0, pt1, pt2 };
	pDC->Polygon(pts, 3);
}
AFX_INLINE void CXTPSplitterTracker::SetBoundRect(CRect rcBoundRect)
{
	m_rcBoundRect = rcBoundRect;
}

/////////////////////////////////////////////////////////////////////////////

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDRAWHELPERS_H__)
