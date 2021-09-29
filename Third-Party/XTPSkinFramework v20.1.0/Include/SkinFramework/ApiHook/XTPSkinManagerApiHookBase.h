// XTPSkinManagerApiHookBase.h: interface for the CXTPSkinManagerApiHookBase class.
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
#if !defined(__XTPSKINMANAGERAPIHOOKBASE_H__)
#	define __XTPSKINMANAGERAPIHOOKBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPWinThemeWrapper;

//{{AFX_CODEJOCK_PRIVATE

#	define XTP_LDR_IS_DATAFILE(handle) (((ULONG_PTR)(handle)) & (ULONG_PTR)1)
#	define XTP_LDR_IS_IMAGEMAPPING(handle) (((ULONG_PTR)(handle)) & (ULONG_PTR)2)
#	define XTP_LDR_IS_RESOURCE(handle)                                                            \
		(XTP_LDR_IS_IMAGEMAPPING(handle) || XTP_LDR_IS_DATAFILE(handle))

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerApiHookBase

class _XTP_EXT_CLASS CXTPSkinManagerApiHookBase
	: public CObject
	, public CXTPSynchronized
{
	DECLARE_DYNAMIC(CXTPSkinManagerApiHookBase);

public:
	virtual ~CXTPSkinManagerApiHookBase();

	virtual void InitializeHookManagement()							= 0;
	virtual void FinalizeHookManagement()							= 0;
	virtual void ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x)		= 0;
	virtual LPVOID GetOriginalFunctionAddress(int nFunction) const	= 0;
	virtual BOOL AdjustWindowRectExOrig(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
										DWORD dwExStyle)			= 0;
	virtual LRESULT CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam,
									   LPARAM lParam)				= 0;
	virtual LPVOID GetOpenThemeDataOriginalPtr() const				= 0;
	virtual LPVOID GetCloseThemeDataOriginalPtr() const				= 0;
	virtual void ConnectWrapper(CXTPWinThemeWrapper* pThemeWrapper) = 0;

	static BOOL AFX_CDECL IsSystemWindowModule(WNDPROC lpWndProc);

	BOOL IsLegacyImplementation() const;

	static PVOID GetAfxWndProc();

protected:
	explicit CXTPSkinManagerApiHookBase(BOOL bLegacy);

	static BOOL AFX_CDECL IsHookedByAppHelpDll(LPVOID lpProc);

	BOOL CallHookDefWindowProc(HWND hWnd, LPVOID pfnOrig, XTPSkinDefaultProc defProc, LPVOID lpPrev,
							   UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

private:
	BOOL m_bLegacy;

	static PVOID sm_pfnAfxWndProc;
};

AFX_INLINE BOOL CXTPSkinManagerApiHookBase::IsLegacyImplementation() const
{
	return m_bLegacy;
}

AFX_INLINE PVOID CXTPSkinManagerApiHookBase::GetAfxWndProc()
{
	return sm_pfnAfxWndProc;
}

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGERAPIHOOKBASE_H__)
