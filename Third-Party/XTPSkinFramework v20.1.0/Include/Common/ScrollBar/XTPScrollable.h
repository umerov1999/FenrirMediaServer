// XTPScrollable.h interface for the CXTPScrollable class.
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
#if !defined(__XTPSCROLLABLE_H__)
#	define __XTPSCROLLABLE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE

struct IXTPScrollable;

class _XTP_EXT_CLASS CXTPScrollHost
{
public:
	explicit CXTPScrollHost();
	~CXTPScrollHost();

protected:
	BOOL InitializeScrollHost(IXTPScrollable* pScrollable, DWORD dwInitialStyle,
							  DWORD dwInitialExStyle);
	void UninitializeScrollHost();
	void SetScrollBarTheme(XTPScrollBarTheme nTheme);
	void EnableDefaultBorderDrawing(BOOL bEnable);
	BOOL IsScrollHostInitialized() const;
	BOOL OnHostWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	BOOL OnHostDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	LRESULT SendControlMessage(UINT message, WPARAM wParam, LPARAM lParam);
	CWnd* GetEmbeddedControl();

private:
	CScrollBar* CreateBar(DWORD style, CWnd* pParent);
	void Layout();
	void SynchronizeScrollBar(int nBar, PBOOL pbRequiresLayout = NULL, BOOL bInInit = FALSE);
	static LRESULT CALLBACK CtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnStyleChanging(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	BOOL OnScroll(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void FilterStyles();
#	ifdef _XTP_INCLUDE_CONTROLS
	BOOL OnSetTheme(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
#	endif
	BOOL OnCtrlMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	BOOL OnCtrlMouseHWheel(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
	IXTPScrollable* m_pScrollable;
	CScrollBar* m_bars[2];
	SCROLLINFO m_scrollInfo[2];
	BOOL m_bLeftBar;
	BOOL m_bInitialized;
	CWnd* m_pControl;
	WNDPROC m_pfnCtrlProc;
	CWnd m_oldControl;
	CWnd* m_pGapCtrl;
	BOOL m_bEnableDefaultBorder;
	int m_nAccumulatedWheelDelta;
	int m_nAccumulatedHWheelDelta;
	BOOL m_bInLayout;
	DWORD m_dwCurrentCtlStyle;
	DWORD m_dwCurrentCtlExStyle;

	static CFont m_Font;
	static CXTPGlobalAtom m_propAtom;
};

AFX_INLINE BOOL CXTPScrollHost::IsScrollHostInitialized() const
{
	return m_bInitialized;
}

AFX_INLINE CWnd* CXTPScrollHost::GetEmbeddedControl()
{
	return m_pControl;
}

//}}AFX_CODEJOCK_PRIVATE

//=======================================================================
// Summary:
//	This interface is to be implemented only by special scrollable controls
//	that require overriding of scroll bars using CXTPScrollable adaptor and for which
//	the default implementation of this interface by CXTPScrollable does not
//	work.
// See also:
//	CXTPScrollable
//=======================================================================
struct IXTPScrollable
{
	//-----------------------------------------------------------------------
	// Handles object destruction
	//-----------------------------------------------------------------------
	virtual ~IXTPScrollable()
	{
	}

	//-----------------------------------------------------------------------
	// Summary:
	//	Obtains scrollable control window reference.
	// Returns:
	//	Scrollable control window reference.
	//-----------------------------------------------------------------------
	virtual CWnd& GetWnd() = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Returns TRUE if the control has vertical scroll bar for the combination of styles specified.
	// Parameters:
	//	dwStyle - Window style bits.
	//	dwExStyle - Window extended style bits.
	// Returns:
	//	TRUE if the control has vertical scroll bar for the combination of styles specified,
	// otherwise the return value is FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL HasVScroll(DWORD dwStyle, DWORD dwExStyle) const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Returns TRUE if the control has horizontal scroll bar for the combination of styles
	// specified.
	// Parameters:
	//	dwStyle - Window style bits.
	//	dwExStyle - Window extended style bits.
	// Returns:
	//	TRUE if the control has horizontal scroll bar for the combination of styles specified,
	// otherwise
	// the return value is FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL HasHScroll(DWORD dwStyle, DWORD dwExStyle) const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Returns TRUE if the control has vertical scroll bar on the left side for the combination of
	// styles specified.
	// Parameters:
	//	dwStyle - Window style bits.
	//	dwExStyle - Window extended style bits.
	// Returns:
	//	TRUE if the control has vertical scroll bar on the left side for the combination of styles
	// specified, FALSE indicates scroll bar on the right side.
	//-----------------------------------------------------------------------
	virtual BOOL HasLeftScrollbar(DWORD dwStyle, DWORD dwExStyle) const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Disables/removes standard scroll bars so that custom scroll bars created via this interface
	// could be used.
	// Parameters:
	//  wnd - Window for which scrollbars must be disabled.
	//-----------------------------------------------------------------------
	virtual void DisableScrollbars()		  = 0;
	virtual void DisableScrollbars(CWnd& wnd) = 0; // <combine IXTPScrollable::DisableScrollbars>

	//-----------------------------------------------------------------------
	// Summary:
	//	Creates a new scroll bar object.
	// Returns:
	//	A new instance of a scroll bar object that is derived from CScrollBar. It's caller's
	// responsibility to handle object lifetime.
	//-----------------------------------------------------------------------
	virtual CScrollBar* CreateBar() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Creates a close instance of the control itself.
	// Returns:
	//	A new instance of the control object. It's caller's
	// responsibility to handle object lifetime.
	//-----------------------------------------------------------------------
	virtual CWnd* CreateControl() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Filters supplied window style if needed. The method is called every
	//  time a change in window style is detected.
	// Parameters:
	//  dwStyle - Current style bits.
	// Returns:
	//	Modified style bits or the original value if no change in style
	//  is required.
	//-----------------------------------------------------------------------
	virtual DWORD FilterStyle(DWORD dwStyle) const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Filters supplied extended window style if needed. The method is
	//  called every time a change in extended window style is detected.
	// Parameters:
	//  dwExStyle - Current extended style bits.
	// Returns:
	//	Modified extended style bits or the original value if no change in
	//  extended style is required.
	//-----------------------------------------------------------------------
	virtual DWORD FilterExStyle(DWORD dwExStyle) const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//	Determines if mouse wheel handling logic must be overridden by scroll
	//  host when a hosted control is not able to handle mouse wheel interaction
	//  when its scroll bars are disabled.
	// Returns:
	//	TRUE if scroll host is to provide its own default implementation of
	//  mouse wheel rotation handling. For default dehavior determined by the
	//  hosted control the return value must be FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL RequiresMouseWheelOverriding() const = 0;
};

//=======================================================================
// Summary:
//	The adaptor class for a scrollable control for which standard scroll bars
//	have to be overriden.
// Parameters:
//	Base - Class name of the base control class.
// See also:
//	IXTPScrollable
//=======================================================================
template<class Base>
class CXTPScrollable
	: public Base
	, public CXTPScrollHost
	, public IXTPScrollable
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//	Initializes a scrollable object instance.
	//-----------------------------------------------------------------------
	CXTPScrollable();

#	if (1310 < _MSC_VER)
	using Base::Create;
	using Base::CreateEx;
#	endif

	//-----------------------------------------------------------------------
	// Summary:
	//	Creates a scrollable control window.
	// Parameters:
	//	lpszClassName - Pointer to a null-terminated string that contains the name of a registered
	// system window class; or the name of a predefined system window class.
	//
	//	lpszWindowName - Pointer to a null-terminated string that contains the window display name;
	// otherwise NULL for no window display name.
	//
	//	dwStyle - Bitwise combination (OR) of window styles. The WS_POPUP option is not a valid
	// style.
	//
	//	rect - The size and location of the window relative to the top-left corner of the parent
	// window.
	//
	//	pParentWnd - Pointer to the parent window.
	//
	//	nID - ID of the window.
	//
	//	pContext - Pointer to a CCreateContext structure that is used to customize the document-view
	// architecture for the application.
	// Returns:
	//	TRUE if the window is successfully created, otherwise the return value is FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID,
						CCreateContext* pContext = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//	Creates a scrollable control window.
	// Parameters:
	//	dwExStyle - Bitwise combination(OR) of extended window styles; otherwise NULL for the
	// default extended window style.
	//
	//	lpszClassName - Pointer to a null-terminated string that contains the name of a registered
	// system window class; or the name of a predefined system window class.
	//
	//	lpszWindowName - Pointer to a null-terminated string that contains the window display name;
	// otherwise NULL for no window display name.
	//
	//	dwStyle - Bitwise combination (OR) of window styles. The WS_POPUP option is not a valid
	// style.
	//
	//	x - The initial horizontal distance of the window from the left side of the screen or the
	// parent window.
	//
	//	y - The initial vertical distance of the window from the top of the screen or the parent
	// window.
	//
	//	nWidth - The width, in pixels, of the window.
	//
	//	nHeight - The height, in pixels, of the window.
	//
	//	rect - The size and location of the window relative to the top-left corner of the parent
	// window.
	//
	//	pParentWnd - Pointer to the parent window.
	//
	//	nID - ID of the window.
	//
	//	pContext - Pointer to a CCreateContext structure that is used to customize the document-view
	// architecture for the application.
	// Returns:
	//	TRUE if the window is successfully created, otherwise the return value is FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
						  DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,
						  HMENU nIDorHMenu, LPVOID lpParam = NULL);

	// <combine
	// CXTPScrollable<Base>::CreateEx@DWORD@LPCTSTR@LPCTSTR@DWORD@int@int@int@int@HWND@HMENU@LPVOID>
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
						  DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
						  LPVOID lpParam = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//	Performs one time custom scroll bars intitialization. In most common use cases there is no
	// need to call
	// it manually as intitialization is performed automatically on window creation or DDX binding.
	// Returns:
	//	TRUE if scroll bars are initialized successfully.
	//-----------------------------------------------------------------------
	BOOL InitScrollBars();

	//-----------------------------------------------------------------------
	// Summary:
	//	Sets custom scroll bars theme.
	// Parameters:
	//	nTheme - scroll bar theme identifier.
	//-----------------------------------------------------------------------
	void SetScrollBarTheme(XTPScrollBarTheme nTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//	Enables or disables default border drawing for a host control.
	//	Enabled by default.
	// Parameters:
	//	bEnable - TRUE to enable default border drawing, FALSE to disable it.
	//-----------------------------------------------------------------------
	void EnableDefaultBorderDrawing(BOOL bEnable = TRUE);

private:
	virtual CWnd& GetWnd();
	virtual BOOL HasVScroll(DWORD dwStyle, DWORD dwExStyle) const;
	virtual BOOL HasHScroll(DWORD dwStyle, DWORD dwExStyle) const;
	virtual BOOL HasLeftScrollbar(DWORD dwStyle, DWORD dwExStyle) const;
	virtual void DisableScrollbars();
	virtual void DisableScrollbars(CWnd& wnd);
	virtual CScrollBar* CreateBar() const;
	virtual CWnd* CreateControl() const;
	virtual DWORD FilterStyle(DWORD dwStyle) const;
	virtual DWORD FilterExStyle(DWORD dwStyle) const;
	virtual BOOL RequiresMouseWheelOverriding() const;

	//{{AFX_CODEJOCK_PRIVATE
protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_CODEJOCK_PRIVATE

private:
	DWORD m_dwInitialStyle;
	DWORD m_dwInitialExStyle;
};

template<class Base>
AFX_INLINE CXTPScrollable<Base>::CXTPScrollable()
	: m_dwInitialStyle(0)
	, m_dwInitialExStyle(0)
{
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
											 DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
											 UINT nID, CCreateContext* pContext /*= NULL*/)
{
	if (0 == m_dwInitialStyle)
		m_dwInitialStyle = dwStyle | WS_CHILD;

	// The window being created will be destroyed right away so it should be OK to bypass
	// Base::Create call and call CWnd::Create directly.
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle & ~WS_VISIBLE, rect, pParentWnd, nID,
					  pContext))
		return FALSE;

	if (!IsScrollHostInitialized())
	{
		VERIFY(CXTPScrollHost::InitializeScrollHost(this, m_dwInitialStyle, 0));
		m_dwInitialStyle = 0;
	}

	return TRUE;
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
											   LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y,
											   int nWidth, int nHeight, HWND hWndParent,
											   HMENU nIDorHMenu, LPVOID lpParam /*= NULL*/)
{
	if (0 == m_dwInitialStyle)
		m_dwInitialStyle = dwStyle | WS_CHILD;
	if (0 == m_dwInitialExStyle)
		m_dwInitialExStyle = dwExStyle;

	// The window being created will be destroyed right away so it should be OK to bypass
	// Base::CreateEx call and call CWnd::CreateEx directly.
	if (!CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle & ~WS_VISIBLE, x, y,
						nWidth, nHeight, hWndParent, nIDorHMenu, lpParam))
		return FALSE;

	if (!IsScrollHostInitialized())
	{
		VERIFY(CXTPScrollHost::InitializeScrollHost(this, m_dwInitialStyle, m_dwInitialExStyle));
		m_dwInitialStyle   = 0;
		m_dwInitialExStyle = 0;
	}

	return TRUE;
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
											   LPCTSTR lpszWindowName, DWORD dwStyle,
											   const RECT& rect, CWnd* pParentWnd, UINT nID,
											   LPVOID lpParam /*= NULL*/)
{
	if (0 == m_dwInitialStyle)
		m_dwInitialStyle = dwStyle | WS_CHILD;
	if (0 == m_dwInitialExStyle)
		m_dwInitialExStyle = dwExStyle;

	// The window being created will be destroyed right away so it should be OK to bypass
	// Base::CreateEx call and call CWnd::CreateEx directly.
	if (!CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle & ~WS_VISIBLE, rect,
						pParentWnd, nID, lpParam))
		return FALSE;

	if (!IsScrollHostInitialized())
	{
		VERIFY(CXTPScrollHost::InitializeScrollHost(this, m_dwInitialStyle, m_dwInitialExStyle));
		m_dwInitialStyle   = 0;
		m_dwInitialExStyle = 0;
	}

	return TRUE;
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::InitScrollBars()
{
	if (IsScrollHostInitialized())
		return TRUE;

	ASSERT(::IsWindow(*this));
	return CXTPScrollHost::InitializeScrollHost(this, CObject::GetStyle(), CObject::GetExStyle());
}

template<class Base>
AFX_INLINE void CXTPScrollable<Base>::SetScrollBarTheme(XTPScrollBarTheme nTheme)
{
	CXTPScrollHost::SetScrollBarTheme(nTheme);
}

template<class Base>
AFX_INLINE void CXTPScrollable<Base>::EnableDefaultBorderDrawing(BOOL bEnable /*= TRUE*/)
{
	CXTPScrollHost::EnableDefaultBorderDrawing(bEnable);
}

template<class Base>
AFX_INLINE CWnd& CXTPScrollable<Base>::GetWnd()
{
	return *this;
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::HasVScroll(DWORD dwStyle, DWORD dwExStyle) const
{
	UNREFERENCED_PARAMETER(dwExStyle);
	return 0 != (dwStyle & WS_VSCROLL);
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::HasHScroll(DWORD dwStyle, DWORD dwExStyle) const
{
	UNREFERENCED_PARAMETER(dwExStyle);
	return 0 != (dwStyle & WS_HSCROLL);
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::HasLeftScrollbar(DWORD dwStyle, DWORD dwExStyle) const
{
	UNREFERENCED_PARAMETER(dwStyle);
	return 0 != (dwExStyle & WS_EX_LEFTSCROLLBAR);
}

template<class Base>
AFX_INLINE void CXTPScrollable<Base>::DisableScrollbars()
{
	DisableScrollbars(*this);
}

template<class Base>
AFX_INLINE void CXTPScrollable<Base>::DisableScrollbars(CWnd& wnd)
{
	wnd.ModifyStyle(WS_VSCROLL | WS_HSCROLL, 0);
	wnd.ModifyStyleEx(WS_EX_LEFTSCROLLBAR, 0);
}

template<class Base>
AFX_INLINE CScrollBar* CXTPScrollable<Base>::CreateBar() const
{
	return new CXTPScrollBarCtrl();
}

template<class Base>
AFX_INLINE CWnd* CXTPScrollable<Base>::CreateControl() const
{
	return new Base();
}

template<class Base>
AFX_INLINE DWORD CXTPScrollable<Base>::FilterStyle(DWORD dwStyle) const
{
	return dwStyle;
}

template<class Base>
AFX_INLINE DWORD CXTPScrollable<Base>::FilterExStyle(DWORD dwExStyle) const
{
	return dwExStyle;
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::RequiresMouseWheelOverriding() const
{
	return FALSE;
}

template<class Base>
AFX_INLINE void CXTPScrollable<Base>::PreSubclassWindow()
{
	Base::PreSubclassWindow();

	if (!IsScrollHostInitialized())
	{
		// If window has coordinates it is assumed the window is fully
		// created and the calls is not made from HCBT_CREATEWND.
		CRect rc;
		::GetWindowRect(CXTPScrollable<Base>::m_hWnd, &rc);
		if (0 != rc.top || 0 != rc.left || 0 != rc.bottom || 0 != rc.right)
			VERIFY(CXTPScrollHost::InitializeScrollHost(this, CXTPScrollable<Base>::GetStyle(),
														CXTPScrollable<Base>::GetExStyle()));
	}
}

template<class Base>
AFX_INLINE LRESULT CXTPScrollable<Base>::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!IsScrollHostInitialized())
		return Base::DefWindowProc(message, wParam, lParam);

	LRESULT lResult = 0;
	if (CXTPScrollHost::OnHostDefWindowProc(message, wParam, lParam, &lResult))
		return lResult;

	return ::DefWindowProc(CXTPScrollable<Base>::m_hWnd, message, wParam, lParam);
}

template<class Base>
AFX_INLINE BOOL CXTPScrollable<Base>::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam,
											   LRESULT* pResult)
{
	if (!IsScrollHostInitialized())
		return Base::OnWndMsg(message, wParam, lParam, pResult);

	if (CXTPScrollHost::OnHostWndMsg(message, wParam, lParam, pResult))
		return TRUE;

	// Base implementation must not be called as the control is already stolen.
	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSCROLLABLE_H__)
