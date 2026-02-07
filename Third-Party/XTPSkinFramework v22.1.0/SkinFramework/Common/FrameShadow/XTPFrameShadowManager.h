// XTPFrameShadowManager.h: interface for the CXTPFrameShadowManager class.
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
#if !defined(__XTPFRAMESHADOWMANAGER_H__)
#	define __XTPFRAMESHADOWMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPFrameShadow;
class CXTPSimpleCriticalSection;
class CXTPFrameShadowManager;

//===========================================================================
// Summary: Global Frame Shadow Manager flags.
//===========================================================================
enum XTPFrameShadowManagerFlags
{
	// Frame shadows disabled. When this flags is set the existing shadow made
	// invisible and not resizeable (xtpFrameShadowsInvisible | xtpFrameShadowsNotResizeable
	// get enabled automatically) but no new shadows will be created for new windows.
	xtpFrameShadowsDisabled = 1,

	// Frame shadows are not visible but remain resizeable.
	xtpFrameShadowsInvisible = 2,

	// Frame shadows cannot be used for resizing the owner windows.
	xtpFrameShadowsNotResizeable = 4,

	// Frame shadows are automatically disabled for Terminal Services sessions,
	// e.g. Remote Desktop Connection. When it's set and a Terminal Services
	// session is detected all existing and new shadows will be made invisible
	// and not resizeable (xtpFrameShadowsInvisible | xtpFrameShadowsNotResizeable).
	// Once terminal session abandoning is detected those flags
	// will get restored to their original values.
	xtpFrameShadowsDisabledInTSS = 8,

	// Frame shadows are automatically invisible for Terminal Services sessions,
	// e.g. Remote Desktop Connection, but remain resizealbe.
	xtpFrameShadowsInvisibleInTSS = 0x10
};

//===========================================================================
// Summary: An interface for Frame Shadow Manager event listener.
//===========================================================================
struct IXTPFrameShadowManagerEvents
{
	//-----------------------------------------------------------------------
	// Handles object destruction
	//-----------------------------------------------------------------------
	virtual ~IXTPFrameShadowManagerEvents()
	{
	}

	//===========================================================================
	// Summary: Invoked right after a shadow is enabled for a window.
	// Parameters:
	//		pManager - Frame Shadow Manager pointer
	//		hwndOwner - Shadow owner window handle
	//		pShadow - Shadow pointer
	//===========================================================================
	virtual void OnFrameShadowEnabled(CXTPFrameShadowManager* pManager, HWND hwndOwner,
									  CXTPFrameShadow* pShadow)
	{
		UNREFERENCED_PARAMETER(pManager);
		UNREFERENCED_PARAMETER(hwndOwner);
		UNREFERENCED_PARAMETER(pShadow);
	}

	//===========================================================================
	// Summary: Invoked right before a shadow is disabled for a window.
	// Parameters:
	//		pManager - Frame Shadow Manager pointer
	//		hwndOwner - Shadow owner window handle
	//		pShadow - Shadow pointer
	//===========================================================================
	virtual void OnFrameShadowDisabled(CXTPFrameShadowManager* pManager, HWND hwndOwner,
									   CXTPFrameShadow* pShadow)
	{
		UNREFERENCED_PARAMETER(pManager);
		UNREFERENCED_PARAMETER(hwndOwner);
		UNREFERENCED_PARAMETER(pShadow);
	}

	//===========================================================================
	// Summary: Invoked when flags are changed.
	// Parameters:
	//		pManager - Frame Shadow Manager pointer
	//		nOldEffectiveFlags - A set of old effective flags.
	//		nNewEffectiveFlags - A set of new effective flags.
	// See also: XTPFrameShadowManagerFlags
	//===========================================================================
	virtual void OnFlagsChanged(CXTPFrameShadowManager* pManager, int nOldEffectiveFlags,
								int nNewEffectiveFlags)
	{
		UNREFERENCED_PARAMETER(pManager);
		UNREFERENCED_PARAMETER(nOldEffectiveFlags);
		UNREFERENCED_PARAMETER(nNewEffectiveFlags);
	}
};

//-----------------------------------------------------------------------
// Summary:
//      Provides access to the global frame shdow manager instance.
// Returns:
//      Global frame shadow instance pointer.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPFrameShadowManager* AFX_CDECL XTPFrameShadowManager();

//===========================================================================
// Summary:
//     Provides frame shadow management functionality.
//===========================================================================
class _XTP_EXT_CLASS CXTPFrameShadowManager
	: public CXTPCmdTarget
	, public CXTPObservable<CXTPFrameShadowManager, IXTPFrameShadowManagerEvents>
{
	friend class CXTPSingleton<CXTPFrameShadowManager>;

	class CFrameShadowHook;
	friend class CFrameShadowHook;

	class CWtsEventListener;
	friend class CWtsEventListener;

	CXTPFrameShadowManager();

public:
	//{{AFX_CODEJOCK_PRIVATE
	~CXTPFrameShadowManager();
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame shadows are supported by the platform.
	// Returns:
	//     TRUE if frame shadows are supported, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsShadowSupported() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains global frame shadow flags.
	// Returns:
	//     Global frame shadow flags combination.
	// See also: XTPFrameShadowManagerFlags
	//-----------------------------------------------------------------------
	int GetFlags() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets global frame shadow flags. The previous value is overwritten.
	// Parameters:
	//		nFlags - Global frame shadow flags combination.
	// See also: XTPFrameShadowManagerFlags
	//-----------------------------------------------------------------------
	void SetFlags(int nFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables frame shadow for a window specified.
	// Parameters:
	//     hWnd - A non-child window handle for which frame shadow has to be enabled.
	//     pWnd - A pointer to a non-child window for which frame shadow has to be enabled.
	//     crColor - Optional. Frame shadow color. If not specified, default value is used.
	//     nOuterSize - Optional. Frame shadow outer size. If not specified, default value is used.
	//     nInnerSize - Optional. Frame shadow inner size. If not specified, default value is used.
	//     ptOffset - Optional. Frame shadow offset. If not specified, default value is used.
	//     bTransparency - Optional. Frame shadow transparency (0 - fully transparent, 255 - fully
	//     opaque).
	//                     If not specified, default value is used.
	// Returns:
	//     Shadow object pointer or NULL if failed.
	// See Also:
	//     DisableShadow, IsShadowEnabled, GetShadow
	//-----------------------------------------------------------------------
	CXTPFrameShadow* EnableShadow(CWnd* pWnd, COLORREF crColor = (COLORREF)-1, UINT nOuterSize = 0,
								  UINT nInnerSize = 0, CPoint ptOffset = CPoint(0, 0),
								  BYTE bTransparency = 0);
	CXTPFrameShadow* EnableShadow(
		HWND hWnd, COLORREF crColor = (COLORREF)-1, UINT nOuterSize = 0, UINT nInnerSize = 0,
		CPoint ptOffset = CPoint(0, 0),
		BYTE bTransparency =
			0); // <combine
				// CXTPFrameShadowManager::EnableShadow@CWnd*@COLORREF@UINT@UINT@CPoint@BYTE>

	//-----------------------------------------------------------------------
	// Summary:
	//     Disable a previsously enabled frame shadow for a window specified.
	// Parameters:
	//     pWnd - A pointer to a non-child window for which frame shadow has to be disabled.
	// See Also:
	//     EnableShadow, IsShadowEnabled, GetShadow
	//-----------------------------------------------------------------------
	void DisableShadow(HWND hWnd);
	void DisableShadow(CWnd* pWnd); // <combine CXTPFrameShadowManager::DisableShadow@HWND>

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a shadow is enabled for the window specified.
	// Parameters:
	//     pWnd - A pointer to a window to check shadow for.
	// Returns:
	//     TRUE if shadow is enabled for the window specified.
	// See Also:
	//     EnableShadow, DisableShadow, GetShadow
	//-----------------------------------------------------------------------
	BOOL IsShadowEnabled(CWnd* pWnd) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains shadow object pointer for a window specified.
	// Parameters:
	//     pWnd - A pointer to a window for which to obtain a shadow object pointer.
	// Returns:
	//     Window's frame shadow object pointer or NULL if the window has no shadow enabled.
	// See Also:
	//     EnableShadow, DisableShadow, IsShadowEnabled
	//-----------------------------------------------------------------------
	CXTPFrameShadow* GetShadow(HWND hWnd) const;
	CXTPFrameShadow* GetShadow(CWnd* pWnd) const; // <combine
												  // CXTPFrameShadowManager::GetShadow@HWND@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the position of the first shadow in the list of all enabled shadows.
	// Returns:
	//     A position of the first enabled shadow in the internal list
	//     or NULL if not shadow is enabled.
	// See Also:
	//     GetNextShadow
	//-----------------------------------------------------------------------
	POSITION GetFirstShadowPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains frame shadow pointer in the position specified and moves
	//     the position cursor to the next enabled shadow.
	// Returns:
	//     A pointer to the shadow at the position specified or NULL
	//     if the end of the list is reached.
	// See Also:
	//     GetFirstShadowPosition
	//-----------------------------------------------------------------------
	CXTPFrameShadow* GetNextShadow(POSITION& pos) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Freezes/unfreezes updates on changing shadow settings, as a result
	//     shadow blinking is reduced.
	// Parameters:
	//     bFreeze - If TRUE the update freeze counter is incremented. Otherwise
	//               the update freeze counter is decremented.
	//-----------------------------------------------------------------------
	void FreezeUpdates(BOOL bFreeze = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a value of default outer size used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Returns:
	//     A default value.
	// See Also:
	//     SetDefaultOuterSize, SetGlobalOuterSize
	//-----------------------------------------------------------------------
	UINT GetDefaultOuterSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default outer size used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Parameters:
	//     nSize - A new default value to be used.
	// See Also:
	//     GetDefaultOuterSize, SetGlobalOuterSize
	//-----------------------------------------------------------------------
	void SetDefaultOuterSize(UINT nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default outer size used for all shadows to be created
	//     if a default value is used in EnableShadow and applies the change for
	//     all enabled shadows.
	// Parameters:
	//     nSize - A new value to be applied and used by default.
	// See Also:
	//     GetDefaultOuterSize, SetDefaultOuterSize
	//-----------------------------------------------------------------------
	void SetGlobalOuterSize(UINT nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a value of default inner size used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Returns:
	//     A default value.
	// See Also:
	//     SetDefaultInnerSize, SetGlobalInnerSize
	//-----------------------------------------------------------------------
	UINT GetDefaultInnerSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default inner size used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Parameters:
	//     nSize - A new default value to be used.
	// See Also:
	//     GetDefaultInnerSize, SetGlobalInnerSize
	//-----------------------------------------------------------------------
	void SetDefaultInnerSize(UINT nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default inner size used for all shadows to be created
	//     if a default value is used in EnableShadow and applies the change for
	//     all enabled shadows.
	// Parameters:
	//     nSize - A new value to be applied and used by default.
	// See Also:
	//     GetDefaultInnerSize, SetDefaultInnerSize
	//-----------------------------------------------------------------------
	void SetGlobalInnerSize(UINT nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a value of default shadow offset used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Returns:
	//     A default value.
	// See Also:
	//     SetDefaultOffset, SetGlobalOffset
	//-----------------------------------------------------------------------
	CPoint GetDefaultOffset() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default shadow offset used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Parameters:
	//     ptOffset - A new default value to be used.
	// See Also:
	//     GetDefaultOffset, SetGlobalOffset
	//-----------------------------------------------------------------------
	void SetDefaultOffset(CPoint ptOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default shadow offset used for all shadows to be created
	//     if a default value is used in EnableShadow and applies the change for
	//     all enabled shadows.
	// Parameters:
	//     ptOffset - A new value to be applied and used by default.
	// See Also:
	//     GetDefaultOffset, SetDefaultOffset
	//-----------------------------------------------------------------------
	void SetGlobalOffset(CPoint ptOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a value of default color used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Returns:
	//     A default value.
	// See Also:
	//     SetDefaultColor, SetGlobalColor
	//-----------------------------------------------------------------------
	COLORREF GetDefaultColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default color used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Parameters:
	//     crColor - A new default value to be used.
	// See Also:
	//     GetDefaultColor, SetGlobalColor
	//-----------------------------------------------------------------------
	void SetDefaultColor(COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default color used for all shadows to be created
	//     if a default value is used in EnableShadow and applies the change for
	//     all enabled shadows.
	// Parameters:
	//     crColor - A new value to be applied and used by default.
	// See Also:
	//     GetDefaultColor, SetDefaultColor
	//-----------------------------------------------------------------------
	void SetGlobalColor(COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a value of default transparency used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Returns:
	//     A default value. 0 means a fully transparent shadow, 255 means fully opaque.
	// See Also:
	//     SetDefaultTransparency, SetGlobalTransparency
	//-----------------------------------------------------------------------
	BYTE GetDefaultTransparency() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default transparency used for all shadows to be created
	//     if a default value is used in EnableShadow.
	// Parameters:
	//     bTransparency - A new default value to be used. 0 means a fully transparent shadow, 255
	//     means fully opaque.
	// See Also:
	//     GetDefaultTransparency, SetGlobalTransparency
	//-----------------------------------------------------------------------
	void SetDefaultTransparency(BYTE bTransparency);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value of default trasparency used for all shadows to be created
	//     if a default value is used in EnableShadow and applies the change for
	//     all enabled shadows.
	// Parameters:
	//     bTransparency - A new default value to be used. 0 means a fully transparent shadow, 255
	//     means fully opaque.
	// See Also:
	//     GetDefaultTransparency, SetDefaultTransparency
	//-----------------------------------------------------------------------
	void SetGlobalTransparency(BYTE bTransparency);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains default blending options used for all shadows to be created
	//     if a default value is used in EnableShadow. Refer to CXTPFrameShadow
	//     documentation for further details.
	// Parameters:
	//     pFactors - Optionals. If not NULL, should be a pointer to an array of
	//                the number of elements equal to the returned value.
	//     pPositions - Optionals. If not NULL, should be a pointer to an array of
	//                the number of elements equal to the returned value.
	// Returns:
	//     A number of factors and positions used for blending.
	// See Also:
	//     SetDefaultBlendingOptions, SetGlobalBlendingOptions, CXTPFrameShadow
	//-----------------------------------------------------------------------
	UINT GetDefaultBlendingOptions(OUT OPTIONAL float* pFactors,
								   OUT OPTIONAL float* pPositions) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets default blending options used for all shadows to be created
	//     if a default value is used in EnableShadow. Refer to CXTPFrameShadow
	//     documentation for further details.
	// Parameters:
	//     nCount - The number of blending factors and positions.
	//     pFactors - A pointer to an array of nCount elements. Ignored if nCount = 0.
	//     pPositions - A pointer to an array of nCount elements. Ignored if nCount = 0.
	// See Also:
	//     GetDefaultBlendingOptions, SetGlobalBlendingOptions, CXTPFrameShadow
	//-----------------------------------------------------------------------
	void SetDefaultBlendingOptions(UINT nCount, const float* pFactors, const float* pPositions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets default blending options used for all shadows to be created
	//     if a default value is used in EnableShadow and applies the change for
	//     all enabled shadows. Refer to CXTPFrameShadow
	//     documentation for further details.
	// Parameters:
	//     nCount - The number of blending factors and positions.
	//     pFactors - A pointer to an array of nCount elements. Ignored if nCount = 0.
	//     pPositions - A pointer to an array of nCount elements. Ignored if nCount = 0.
	// See Also:
	//     GetDefaultBlendingOptions, SetDefaultBlendingOptions, CXTPFrameShadow
	//-----------------------------------------------------------------------
	void SetGlobalBlendingOptions(UINT nCount, const float* pFactors, const float* pPositions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains or sets default owner clipping for new shadows.
	// Remarks:
	//     By default owner clipping is enabled in order to reduce flickering
	//     and improve rendering speed in slow environments. Disabling owner
	//     clipping may be necessary for owner windows that non-rectangular shape
	//     as the clipping box is always rectangular.
	// See Also:
	//     SetGlobalClipOwner
	//-----------------------------------------------------------------------
	BOOL GetDefaultClipOwner() const;
	void SetDefaultClipOwner(BOOL bClipOwner); // <combine
											   // CXTPFrameShadowManager::GetDefaultClipOwner@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets owner clipping for all shadows.
	// Remarks:
	//     By default owner clipping is enabled in order to reduce flickering
	//     and improve rendering speed in slow environments. Disabling owner
	//     clipping may be necessary for owner windows that non-rectangular shape
	//     as the clipping box is always rectangular.
	// See Also:
	//     GetDefaultClipOwner, SetDefaultClipOwner
	//-----------------------------------------------------------------------
	void SetGlobalClipOwner(BOOL bClipOwner);

protected:
	virtual void OnFinalRelease();

private:
	void SetShadowThreadMessageHook(CXTPFrameShadow* pShadow, BOOL bSet = TRUE);
	void OnHookMessage(PCWPRETSTRUCT pc);
	void UpdateAllShadows(BOOL bRedraw = FALSE);
	void SetInTerminalServiceSession(BOOL bInTSS, BOOL bUpdate = FALSE);
	void UpdateTerminalSessionFlags();

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPFrameShadowManager);

	afx_msg long OleGetFlags();
	afx_msg void OleSetFlags(long flags);
	afx_msg long OleGetDefaultOuterSize();
	afx_msg void OleSetDefaultOuterSize(long size);
	afx_msg long OleGetDefaultInnerSize();
	afx_msg void OleSetDefaultInnerSize(long size);
	afx_msg OLE_XSIZE_PIXELS OleGetDefaultOffsetX();
	afx_msg void OleSetDefaultOffsetX(OLE_XSIZE_PIXELS offset);
	afx_msg OLE_YSIZE_PIXELS OleGetDefaultOffsetY();
	afx_msg void OleSetDefaultOffsetY(OLE_YSIZE_PIXELS offset);
	afx_msg OLE_COLOR OleGetDefaultColor();
	afx_msg void OleSetDefaultColor(OLE_COLOR color);
	afx_msg short OleGetDefaultTransparency();
	afx_msg void OleSetDefaultTransparency(short val);
	afx_msg BOOL OleGetDefaultClipOwner();
	afx_msg void OleSetDefaultClipOwner(BOOL bClip);
	afx_msg VARIANT OleGetDefaultBlendingFactors();
	afx_msg VARIANT OleGetDefaultBlendingPositions();
	afx_msg void OleDisableShadow(OLE_HANDLE hWnd);
	afx_msg BOOL OleIsShadowEnabled(OLE_HANDLE hWnd);
	afx_msg void OleFreezeUpdates(VARIANT_BOOL bFreeze);
	afx_msg void OleSetGlobalOuterSize(long size);
	afx_msg void OleSetGlobalInnerSize(long size);
	afx_msg void OleSetGlobalOffset(OLE_XSIZE_PIXELS offsetX, OLE_YSIZE_PIXELS offsetY);
	afx_msg void OleSetGlobalColor(OLE_COLOR color);
	afx_msg void OleSetDefaultBlendingOptions(const VARIANT& factors, const VARIANT& positions);
	afx_msg void OleSetGlobalBlendingOptions(const VARIANT& factors, const VARIANT& positions);
	afx_msg void OleSetGlobalTransparency(short val);
	afx_msg void OleSetGlobalClipOwner(BOOL bClip);
	afx_msg LPDISPATCH OleEnableShadow(OLE_HANDLE hWnd, OLE_HANDLE color, long outerSize,
									   long innerSize, OLE_XSIZE_PIXELS offsetX = 0,
									   OLE_YSIZE_PIXELS offsetY = 0, short transparancy = 0);
	afx_msg LPDISPATCH OleGetShadow(OLE_HANDLE hWnd);
	afx_msg LPDISPATCH OleGetFrameShadows();
	//}}AFX_CODEJOCK_PRIVATE
#	endif

private:
	int m_nFlags, m_nEffectiveFlags;
	BOOL m_bInTerminalServiceSession;
	CWtsEventListener* m_pWtsEventListener;

	CMap<HWND, HWND&, CXTPFrameShadow*, CXTPFrameShadow*&> m_mapWnd;
	UINT m_nDefaultOuterSize;
	UINT m_nDefaultInnerSize;
	CPoint m_ptDefaultOffset;
	COLORREF m_crDefaultColor;
	BYTE m_bDefaultTransparency;
	BOOL m_bDefaultClipOwner;

	struct DefaultBlendingOptions
	{
		CArray<float, const float&> Factors;
		CArray<float, const float&> Positions;
	} m_DefaultBlendingOptions;

	CXTPSimpleCriticalSection* m_pGuard;
	CMap<DWORD, DWORD&, CFrameShadowHook*, CFrameShadowHook*&> m_mapHooks;
	BOOL m_bInUpdateAllShadows;
};

#	ifdef _XTP_ACTIVEX
//{{AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////
// CFrameShadows command target

class CFrameShadows : public CXTPCmdTarget
{
	DECLARE_DYNCREATE(CFrameShadows)

	CFrameShadows(); // protected constructor used by dynamic creation

	// Attributes
public:
	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameShadows)
public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CFrameShadows();

	// Generated message map functions
	//{{AFX_MSG(CFrameShadows)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFrameShadows)
	DECLARE_ENUM_VARIANTLIST(CFrameShadows)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CFrameShadows);
};

//}}AFX_CODEJOCK_PRIVATE
#	endif

AFX_INLINE int CXTPFrameShadowManager::GetFlags() const
{
	return m_nEffectiveFlags;
}

AFX_INLINE BOOL CXTPFrameShadowManager::IsShadowEnabled(CWnd* pWnd) const
{
	return NULL != GetShadow(pWnd);
}

AFX_INLINE UINT CXTPFrameShadowManager::GetDefaultOuterSize() const
{
	return m_nDefaultOuterSize;
}

AFX_INLINE void CXTPFrameShadowManager::SetDefaultOuterSize(UINT nSize)
{
	m_nDefaultOuterSize = nSize;
}

AFX_INLINE UINT CXTPFrameShadowManager::GetDefaultInnerSize() const
{
	return m_nDefaultInnerSize;
}

AFX_INLINE void CXTPFrameShadowManager::SetDefaultInnerSize(UINT nSize)
{
	m_nDefaultInnerSize = nSize;
}

AFX_INLINE CPoint CXTPFrameShadowManager::GetDefaultOffset() const
{
	return m_ptDefaultOffset;
}

AFX_INLINE void CXTPFrameShadowManager::SetDefaultOffset(CPoint ptOffset)
{
	m_ptDefaultOffset = ptOffset;
}

AFX_INLINE COLORREF CXTPFrameShadowManager::GetDefaultColor() const
{
	return m_crDefaultColor;
}

AFX_INLINE void CXTPFrameShadowManager::SetDefaultColor(COLORREF crColor)
{
	m_crDefaultColor = crColor;
}

AFX_INLINE BYTE CXTPFrameShadowManager::GetDefaultTransparency() const
{
	return m_bDefaultTransparency;
}

AFX_INLINE void CXTPFrameShadowManager::SetDefaultTransparency(BYTE bTransparency)
{
	m_bDefaultTransparency = bTransparency;
}

AFX_INLINE BOOL CXTPFrameShadowManager::GetDefaultClipOwner() const
{
	return m_bDefaultClipOwner;
}

AFX_INLINE void CXTPFrameShadowManager::SetDefaultClipOwner(BOOL bClipOwner)
{
	m_bDefaultClipOwner = bClipOwner;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPFRAMESHADOWMANAGER_H__)
