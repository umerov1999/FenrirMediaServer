// XTPSystemHelpers
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
#if !defined(__XTPSYSTEMHELPERS_H__)
#	define __XTPSYSTEMHELPERS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#	include <oleacc.h>
#	include <tlhelp32.h>
#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE
class CXTPDummySingletonDependency;
//}}AFX_CODEJOCK_PRIVATE
template<
	class Singleton1 = CXTPDummySingletonDependency,
	class Singleton2 = CXTPDummySingletonDependency, class Singleton3 = CXTPDummySingletonDependency,
	class Singleton4 = CXTPDummySingletonDependency, class Singleton5 = CXTPDummySingletonDependency,
	class Singleton6 = CXTPDummySingletonDependency, class Singleton7 = CXTPDummySingletonDependency,
	class Singleton8 = CXTPDummySingletonDependency, class Singleton9 = CXTPDummySingletonDependency,
	class Singleton10 = CXTPDummySingletonDependency>
class CXTPSingletonDependencies;
template<class T, class Dependencies = CXTPSingletonDependencies<> >
class CXTPSingleton;

class CXTPSystemVersion;
// -----------------------------------------------------------------------
// Summary:
//     The XTPSystemVersion function is used for access to the
//     CXTPSystemVersion class.
// Remarks:
//     Call this function to access CXTPSystemVersion members. Since
//     this class is designed as a single instance object you can only
//     access its members through this method. You cannot directly
//     instantiate an object of type CXTPSystemVersion.
// Returns:
//     A pointer to the one and only CXTPSystemVersion instance.
// Example:
//     The following example demonstrates the use of XTPSystemVersion.
// <code>
// bool bIsWinNT = XTPSystemVersion()-\>IsWinNT4();
// </code>
// See Also:
//     CXTPSystemVersion
// -----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPSystemVersion* AFXAPI XTPSystemVersion();

//===========================================================================
// CXTPModuleHandle class is helper for LoadLibray/GetModuleHandle methods
//===========================================================================
class _XTP_EXT_CLASS CXTPModuleHandle
{
public:
	//-----------------------------------------------------------------------
	// Module state
	//-----------------------------------------------------------------------
	enum XTPModuleState
	{
		xtpModNone,	  // Indicates no module found.
		xtpModMapped, // Indicates the module handle was set using GetModuleHandle
		xtpModLoaded  // Indicates the module handle was set using LoadLibrary.
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPModuleHandle object.
	//-----------------------------------------------------------------------
	CXTPModuleHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPModuleHandle object.
	// Parameters:
	//     lpszModuleName - Pointer to a null-terminated string that names
	//                  the .DLL file. The name specified is the
	//                  filename of the module and is not related to the
	//                  name stored in the library module itself, as
	//                  specified by the LIBRARY keyword in the module-
	//                  definition (.DEF) file.
	//                  If the string specifies a path but the file does
	//                  not exist in the specified directory, the
	//                  function fails.  When specifying a path, be sure
	//                  to use backslashes (\), not forward slashes (/).
	//-----------------------------------------------------------------------
	CXTPModuleHandle(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPModuleHandle object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPModuleHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to LoadLibrary
	//-----------------------------------------------------------------------
	BOOL Init(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to load the library specified
	//     by 'lpszModule'.  Once the library is loaded, you can
	//     retrieve the instance handle by using the HINSTANCE operator.
	// Parameters:
	//     lpszModuleName - Pointer to a null-terminated string that names
	//                  the .DLL file. The name specified is the
	//                  filename of the module and is not related to the
	//                  name stored in the library module itself, as
	//                  specified by the LIBRARY keyword in the module-
	//                  definition (.DEF) file.
	//                  If the string specifies a path but the file does
	//                  not exist in the specified directory, the
	//                  function fails.  When specifying a path, be sure
	//                  to use backslashes (\), not forward slashes (/).
	//-----------------------------------------------------------------------
	BOOL LoadLibrary(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     The GetModuleHandle function retrieves a module handle for the specified module
	//     if the file has been mapped into the address space of the calling process.
	// Parameters:
	//     lpModuleName - Pointer to a null-terminated string that contains the name
	//     of the module (either a .dll or .exe file).
	//-----------------------------------------------------------------------
	BOOL GetModuleHandle(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     The FreeLibrary function decrements the reference count of the loaded
	//     dynamic-link library (DLL).
	//-----------------------------------------------------------------------
	BOOL FreeLibrary();

	//-----------------------------------------------------------------------
	// Summary:
	//     The GetProcAddress function retrieves the address of the specified
	//     exported dynamic-link library (DLL) function.
	//-----------------------------------------------------------------------
	BOOL GetProcAddress(FARPROC* ppFnPtr, LPCSTR lpProcName, DWORD dwMinVer = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Use this member function to return the version number of the
	//     module attached to this CLoadLibrary object. The high-order
	//     word of the return value represents the major version number
	//     and the low-order word of the returned value represents the
	//     minor version number.
	// Returns:
	//     A DWORD value if successful, otherwise 0L.
	//-----------------------------------------------------------------------
	DWORD GetVersion();

	//-----------------------------------------------------------------------
	// Summary:
	//     This overloaded operator returns a handle to the module
	//     indicating success. NULL indicates failure.
	// Returns:
	//     A handle to the module if successful, otherwise returns NULL.
	//-----------------------------------------------------------------------
	operator HMODULE() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method returns a handle to the module
	//     indicating success. NULL indicates failure.
	// Returns:
	//     A handle to the module if successful, otherwise returns NULL.
	//-----------------------------------------------------------------------
	HMODULE GetHandle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Path to loaded library
	//-----------------------------------------------------------------------
	CString GetName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns module state
	//-----------------------------------------------------------------------
	XTPModuleState GetModuleState() const;

private:
	BOOL GetVersionInfo();
	BOOL EnsureModuleState();
	BOOL IsModule(LPCTSTR lpName, int nLen = -1) const;

	struct XTP_DLLVERSIONINFO
	{
		DWORD cbSize;
		DWORD dwMajorVersion; // Major version
		DWORD dwMinorVersion; // Minor version
		DWORD dwBuildNumber;  // Build number
		DWORD dwPlatformID;	  // DLLVER_PLATFORM_*
	};

	CString m_strModuleName;	   // Module path
	HMODULE m_hModule;			   // A handle to the module indicates success.
	XTPModuleState m_eModuleState; // Module state
	XTP_DLLVERSIONINFO m_dvInfo;   // Version information

	typedef HRESULT(WINAPI* PFNDLLVERSIONINFO)(XTP_DLLVERSIONINFO*);
	PFNDLLVERSIONINFO m_pfnDllGetVersion;
};

AFX_INLINE HMODULE CXTPModuleHandle::GetHandle() const
{
	return m_hModule;
}
AFX_INLINE CXTPModuleHandle::operator HMODULE() const
{
	return m_hModule;
}
AFX_INLINE CString CXTPModuleHandle::GetName() const
{
	return m_strModuleName;
}
AFX_INLINE CXTPModuleHandle::XTPModuleState CXTPModuleHandle::GetModuleState() const
{
	return m_eModuleState;
}

// Obsolete name
//#define CXTLoadLibrary CXTPModuleHandle

//===========================================================================
// Summary:
//     CXTPSystemVersion is used to get the current
//     Windows OS version. CXTPSystemVersion is a single instance, or
//     "singleton" object, that is accessed with the Get() method.
//===========================================================================
class _XTP_EXT_CLASS CXTPSystemVersion
{
	friend class CXTPSingleton<CXTPSystemVersion>;

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSystemVersion object.
	//-----------------------------------------------------------------------
	CXTPSystemVersion();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Compares a set of operating system version requirements to the
	//     corresponding values for the currently running version of the system.
	// Input:
	//		dwConditionMask   - A variable to be passed as the condition mask, can be either
	// VER_EQUAL or VER_GREATER_EQUAL. 		dwMajorVersion    - The major version number of the
	// operating system. 		dwMinorVersion    - The minor version number of the operating
	// system.
	//		dwPlatformId      - The operating system platform.
	// Returns:
	//     If the currently running operating system satisfies the specified
	//     requirements, the return value is true, otherwise returns false.
	//-----------------------------------------------------------------------
	bool VerifyWinVer(BYTE dwConditionMask, DWORD dwMajorVersion, DWORD dwMinorVersion,
					  DWORD dwPlatformId) const;

	LRESULT VerifyVersion(BYTE dwConditionMask, DWORD dwMajorVersion, DWORD dwMinorVersion,
						  DWORD dwPlatformId) const;

	LRESULT GetVersion(BYTE dwConditionMask, DWORD dwMajorVersion, DWORD dwMinorVersion,
					   DWORD dwPlatformId) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 3.1.
	// Returns:
	//     true if the OS is Windows 3.1, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin31() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 95.
	// Returns:
	//     true if the OS is Windows 95, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin95() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 98.
	// Returns:
	//     true if the OS is Windows 98, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin98() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows ME.
	// Returns:
	//     true if the OS is Windows ME, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinME() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows NT 4.
	// Returns:
	//     true if the OS is Windows NT 4, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinNT4() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 2000.
	// Returns:
	//     true if the OS is Windows 2000, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin2K() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows XP.
	// Returns:
	//     true if the OS is Windows XP, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinXP() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is greater than or equal to Windows 3.1.
	// Returns:
	//     true if the OS is greater than or equal to Windows 3.1,
	//     otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin31OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to check whether or not the
	//      operating system is of the Windows 9x family, and if it is
	//      Windows 95 or a later version.
	// Returns:
	//      true if the OS is of the Windows 9x family, and is Windows 95
	///     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin95OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows 9x family, and if it is
	//     Windows 98 or a later version.
	// Returns:
	//     true if the OS is of the Windows 9x family, and is Windows 98
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin98OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows 9x family, and if it is
	//     Windows ME or a later version.
	// Returns:
	//     true if the OS is of the Windows 9x family, and is Windows ME
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinMEOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows 9x family
	// Returns:
	//     true if the OS is of the Windows 9x family
	//-----------------------------------------------------------------------
	bool IsWin9x() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows NT 4 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows NT
	//     4 or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinNT4OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows 2000 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows
	//     2000 or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin2KOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows XP or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows XP
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinXPOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows Vista or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows Vista
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinVistaOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows 7 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows 7
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin7OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows 8 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows 8
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin8OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows 8.1 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows 8.1
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin81OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows 10 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows 10
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin10OrGreater() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Use this member function to return the version number of the
	//      comctl32.dll. The high-order word of the return
	//     value represents the major version number and the low-order
	//     word of the returned value represents the minor version number.
	// Returns:
	//     A DWORD value if successful, otherwise 0L.
	//-----------------------------------------------------------------------
	DWORD GetComCtlVersion() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if right-to-left (RTL) formatting is supported by current OS
	// Returns:
	//     TRUE if right-to-left (RTL) formatting is supported.
	//-----------------------------------------------------------------------
	BOOL IsLayoutRTLSupported() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if ClearType text is supported by current OS
	//-----------------------------------------------------------------------
	BOOL IsClearTypeTextQualitySupported() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the maximum length, in bytes, of a character in an OEM code page.
	// Returns:
	//     The number of bytes per character.
	//-----------------------------------------------------------------------
	int GetMaxCharSize() const;

private:
	CXTPModuleHandle m_modKernel32;
	CXTPModuleHandle m_modNtdll;

	BOOL(WINAPI* m_pfnVerifyVersionInfo)(LPOSVERSIONINFOEX, DWORD, DWORDLONG);
	ULONGLONG(WINAPI* m_pfnVerSetConditionMask)(ULONGLONG, DWORD, BYTE);

	LONG(WINAPI* m_pfnRtlGetVersion)(OSVERSIONINFOEXW*);

	friend _XTP_EXT_CLASS CXTPSystemVersion* AFXAPI XTPSystemVersion();
};

// deprecated
#	define XTOSVersionInfo XTPSystemVersion

//-----------------------------------------------------------------------
// Summary:
//     MultiMonitor API wrapper class
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPMultiMonitor
{
	friend class CXTPSingleton<CXTPMultiMonitor>;

private:
	DECLARE_HANDLE(XTP_HMONITOR);

private:
	struct XTP_MONITORINFO
	{
		DWORD cbSize;
		RECT rcMonitor;
		RECT rcWork;
		DWORD dwFlags;
	};

	struct XTP_MONITORINFOEX : XTP_MONITORINFO
	{
		TCHAR szDevice[CCHDEVICENAME];
	};

private:
	CXTPMultiMonitor();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This function retrieves work area for the display monitor that
	//     the mouse cursor is currently positioned over.
	// Parameters:
	//     rect  - Reference to a CRect class that specifies the
	//             coordinates of the rectangle of interest in virtual
	//             screen coordinates.
	//     pWnd  - Pointer to the window of interest.
	//     hWnd  - Handle to the window of interest.
	//     point - Point of interest.
	// Returns:
	//     If the mouse cursor is positioned over a display monitor, the
	//     return value is a CRect object that specifies the work area
	//     rectangle of the display monitor, expressed in virtual-screen
	//     coordinates.  Otherwise, returns the size of the work area on
	//     the primary display monitor.
	//-----------------------------------------------------------------------
	CRect GetWorkArea();
	CRect GetWorkArea(const POINT& point);	 //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetWorkArea(LPCRECT rect);		 //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetWorkArea(const CWnd* pWnd);	 //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetWorkArea(HWND hWnd);			 //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(const CWnd* pWnd);	 //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(HWND hWnd);			 //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(const POINT& point); //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(LPCRECT rect);		 //<COMBINE CXTPMultiMonitor::GetWorkArea>

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains display settings for a monitor associated with the window
	//      handle provided.
	// Parameters:
	//      pWnd - Window object pointer
	//      hWnd - Window handle
	//      lpDevMode - Points to DEVMODE object instance with dmSize field
	//                  initialized to a valid structure size value. Upon
	//                  successful completion this object will contain valid
	//                  display settings.
	// Returns:
	//      TRUE if display settings obtained successfully.
	//-----------------------------------------------------------------------
	BOOL GetDisplaySettings(const CWnd* pWnd, DEVMODE* lpDevMode);
	BOOL GetDisplaySettings(HWND hWnd,
							DEVMODE* lpDevMode); // <COMBINE
												 // CXTPMultiMonitor::GetDisplaySettings@const
												 // CWnd*@DEVMODE*>

private:
	BOOL GetMonitorInfo(XTP_HMONITOR hMonitor, XTP_MONITORINFO* lpMonitorInfo);
	CRect GetWorkArea(XTP_HMONITOR hMonitor);
	CRect GetScreenArea(XTP_HMONITOR hMonitor);

	int(WINAPI* m_pfnGetSystemMetrics)(int);
	XTP_HMONITOR(WINAPI* m_pfnMonitorFromWindow)(HWND, DWORD);
	XTP_HMONITOR(WINAPI* m_pfnMonitorFromRect)(LPCRECT, DWORD);
	XTP_HMONITOR(WINAPI* m_pfnMonitorFromPoint)(POINT, DWORD);
	BOOL(WINAPI* m_pfnGetMonitorInfo)(XTP_HMONITOR, XTP_MONITORINFO*);
	BOOL(WINAPI* m_pfnEnumDisplaySettingsEx)(LPCTSTR, DWORD, DEVMODE*, DWORD);
	CXTPModuleHandle m_modUser32;

	friend _XTP_EXT_CLASS CXTPMultiMonitor* AFX_CDECL XTPMultiMonitor();
};

//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPMultiMonitor members.
//     Since this class is designed as a single instance object you can
//     only access its members through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPMultiMonitor.
// Example:
//     <code>CRect rcWork = XTPMultiMonitor()->GetWorkArea();</code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPMultiMonitor* AFX_CDECL XTPMultiMonitor();

//===========================================================================
// Summary: Critical Section Wrapper
//===========================================================================
class _XTP_EXT_CLASS CXTPCriticalSection
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCriticalSection object
	//-----------------------------------------------------------------------
	CXTPCriticalSection();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCriticalSection object, handles cleanup and deallocation
	//-------------------------------------------------------------------------
	~CXTPCriticalSection();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     The EnterCriticalSection function waits for ownership of the
	//     specified critical section object
	//-------------------------------------------------------------------------
	void EnterCritical();

	//-------------------------------------------------------------------------
	// Summary:
	//     The LeaveCriticalSection function releases ownership of the
	//     specified critical section object
	//-------------------------------------------------------------------------
	void LeaveCritical();

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns CRITICAL_SECTION reference
	//-------------------------------------------------------------------------
	operator CRITICAL_SECTION&()
	{
		return m_csMutex;
	}

private:
	CRITICAL_SECTION m_csMutex;
};

//===========================================================================
// Summary: Critical Sections Lock/Unlock helper
//===========================================================================
class _XTP_EXT_CLASS CXTPLockGuard
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPLockGuard object
	// Parameters:
	//     key - CRITICAL_SECTION used to lock thread
	//-----------------------------------------------------------------------
	CXTPLockGuard(CRITICAL_SECTION& key);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCriticalSection object, handles cleanup and deallocation
	//-------------------------------------------------------------------------
	~CXTPLockGuard();

public:
	//-------------------------------------------------------------------------
	// Summary: This method call EnterCritical of key object
	//-------------------------------------------------------------------------
	void LockThread();

	//-------------------------------------------------------------------------
	// Summary: This method call LeaveCritical of key object
	//-------------------------------------------------------------------------
	void UnLockThread();

private:
	DISABLE_COPY_OPERATOR(CXTPLockGuard);

private:
	CRITICAL_SECTION& m_key;
};

//{{AFX_CODEJOCK_PRIVATE

#	ifndef CHILDID_SELF
#		define CHILDID_SELF 0
#	endif

#	ifndef WM_GETOBJECT
#		define WM_GETOBJECT 0x003D
#	endif

#	ifndef EVENT_OBJECT_FOCUS
#		define EVENT_OBJECT_FOCUS 0x8005
#	endif

#	ifndef OBJID_MENU
#		define OBJID_MENU 0xFFFFFFFD
#	endif

#	ifndef OBJID_CLIENT
#		define OBJID_CLIENT 0xFFFFFFFC
#	endif

#	ifndef OBJID_WINDOW
#		define OBJID_WINDOW 0x00000000
#	endif

class _XTP_EXT_CLASS CXTPAccessible
{
public:
	CXTPAccessible();
	virtual ~CXTPAccessible();

public:
	virtual HRESULT GetAccessibleParent(IDispatch** ppdispParent);
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleValue(VARIANT varChild, BSTR* pszValue);
	virtual HRESULT GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual HRESULT GetAccessibleHelp(VARIANT varChild, BSTR* pszHelp);
	virtual HRESULT GetAccessibleHelpTopic(BSTR* pszHelpFile, VARIANT varChild, long* pidTopic);
	virtual HRESULT GetAccessibleKeyboardShortcut(VARIANT varChild, BSTR* pszKeyboardShortcut);
	virtual HRESULT GetAccessibleFocus(VARIANT* pvarChild);
	virtual HRESULT GetAccessibleSelection(VARIANT* pvarChildren);
	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight,
									   VARIANT varChild);
	virtual HRESULT AccessibleNavigate(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	// Obsolete
	virtual HRESULT PutAccessibleName(VARIANT varChild, BSTR szName);
	// Obsolete
	virtual HRESULT PutAccessibleValue(VARIANT varChild, BSTR szValue);

	virtual CCmdTarget* GetAccessible();

	void AccessibleNotifyWinEvent(DWORD event, HWND hwnd, LONG idObject, LONG idChild);

protected:
	HRESULT AccessibleObjectFromWindow(HWND hwnd, DWORD dwId, REFIID riid, void** ppvObject);
	LRESULT LresultFromObject(REFIID riid, WPARAM wParam, LPUNKNOWN punk);

	long GetChildIndex(VARIANT* varChild);

public:
#	pragma warning(push)
// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4619: #pragma warning : there is no warning number 'number'
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#	pragma warning(disable : 4616 4619 5204)
	class _XTP_EXT_CLASS XAccessible;
	BEGIN_INTERFACE_PART(Accessible, IAccessible)
	STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo);
	STDMETHOD(GetIDsOfNames)
	(REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames, LCID lcid, DISPID FAR* rgdispid);
	STDMETHOD(Invoke)
	(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pdispparams,
	 VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr);

	STDMETHOD(get_accParent)(IDispatch* FAR* ppdispParent);
	STDMETHOD(get_accChildCount)(long FAR* pChildCount);
	STDMETHOD(get_accChild)(VARIANT varChildIndex, IDispatch* FAR* ppdispChild);

	STDMETHOD(get_accName)(VARIANT varChild, BSTR* pszName);
	STDMETHOD(get_accValue)(VARIANT varChild, BSTR* pszValue);
	STDMETHOD(get_accDescription)(VARIANT varChild, BSTR FAR* pszDescription);
	STDMETHOD(get_accRole)(VARIANT varChild, VARIANT* pvarRole);
	STDMETHOD(get_accState)(VARIANT varChild, VARIANT* pvarState);
	STDMETHOD(get_accHelp)(VARIANT varChild, BSTR* pszHelp);
	STDMETHOD(get_accHelpTopic)(BSTR* pszHelpFile, VARIANT varChild, long* pidTopic);
	STDMETHOD(get_accKeyboardShortcut)(VARIANT varChild, BSTR* pszKeyboardShortcut);
	STDMETHOD(get_accFocus)(VARIANT FAR* pvarFocusChild);
	STDMETHOD(get_accSelection)(VARIANT FAR* pvarSelectedChildren);
	STDMETHOD(get_accDefaultAction)(VARIANT varChild, BSTR* pszDefaultAction);

	STDMETHOD(accSelect)(long flagsSelect, VARIANT varChild);
	STDMETHOD(accLocation)
	(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild);
	STDMETHOD(accNavigate)(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt);
	STDMETHOD(accHitTest)(long xLeft, long yTop, VARIANT* pvarChildAtPoint);
	STDMETHOD(accDoDefaultAction)(VARIANT varChild);

	STDMETHOD(put_accName)(VARIANT varChild, BSTR szName);
	STDMETHOD(put_accValue)(VARIANT varChild, BSTR pszValue);
	END_INTERFACE_PART(ExternalAccessible)
#	pragma warning(pop)

private:
	typedef VOID(WINAPI* LPFNNOTIFYWINEVENT)(DWORD event, HWND hwnd, LONG idObject, LONG idChild);

	LPFNNOTIFYWINEVENT m_pNotifyWinEvent;
	LPFNLRESULTFROMOBJECT m_pLresultFromObject;
	LPFNACCESSIBLEOBJECTFROMWINDOW m_pAccessibleObjectFromWindow;

	HMODULE m_hUser32;
	HMODULE m_hOleAcc;
};

//}}AFX_CODEJOCK_PRIVATE

//---------------------------------------------------------------------------
// Summary:
//     Zipped memory file
//---------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPZipMemFile : public CMemFile
{
	struct ZIPBUFFERHEADER
	{
		DWORD dwType;
		DWORD dwSize;
	};

	struct ZIPBUFFERINFO
	{
		ZIPBUFFERHEADER bh;
		BYTE bBuffer[1];
	};

public:
	CXTPZipMemFile();
	CXTPZipMemFile(BYTE* lpBuffer, UINT nBufferSize);

	~CXTPZipMemFile();

public:
	void AttachCompressedBuffer(BYTE* lpBuffer, UINT nBufferSize, BOOL bAutoDelete = FALSE);
	void DetachCompressedBuffer(BYTE*& lpBuffer, DWORD& dwCount);
	BOOL OpenCompressedFile(LPCTSTR lpszFileName);

public:
};

_XTP_EXT_CLASS CWinThread* AFX_CDECL XTPGetThread();

//-----------------------------------------------------------------------
// Summary: URI type identifier.
//-----------------------------------------------------------------------
enum XTPUriType
{
	xtpUriFile, // Local file URI.
	xtpUriRes	// Resource in a local file URI.
};

//-----------------------------------------------------------------------
// Summary: Parse URI information.
//-----------------------------------------------------------------------
struct XTP_URI
{
	XTPUriType type; // URI type.
	union
	{
		TCHAR file[MAX_PATH + 1]; // Valid only for xtpUriFile. Local file full path.

		struct
		{
			TCHAR szFilePath[MAX_PATH + 1]; // Local file full path.
			LPCTSTR lpType;					// Resource type.
			LPCTSTR lpName;					// Resource name.
			struct
			{
				TCHAR szType[50];
				TCHAR szName[50];
			} reserved; // Reserved for internal needs.
		} res;			// Valid only for xtpUriRes.
	};
};

//-----------------------------------------------------------------------
// Summary: Default URI values.
//-----------------------------------------------------------------------
struct XTP_URI_DEFAULTS
{
	struct
	{
		LPCTSTR lpType; // Default resource type.
	} res;
};

//-----------------------------------------------------------------------
// Summary: Parses URI.
// Parameters:
//     lpszUri - Pointer to a NULL-terminated URI.
//
//               Sample valid file URIs:
//                   file://C:\Windows\data.txt - absolute file URI
//                   C:\Windows\data.txt - absolute file URI
//                   file://data.txt - relative to executable module file URI.
//                   data.txt - relative to executable module file URI.
//                   file://..\..\bin\data.txt - relative to executable module file URI.
//                   data.txt - relative to executable module file URI.
//
//               Resource URI format:
//                   res://[absolute or relative path|.][/(type name|type ID)/](name|#ID)
//               If optional resource type is not specified and no default values passed,
//               the default value return by XTPGetDefaultResourceTypeFromPath will be used.
//               A special '.' symbol must be used as file name for referring to
//               current executable resource directory.
//               Sample valid resource URIs:
//                   res://C:\Windows\System32\mydll.dll/24/1
//                   res://C:\Windows\System32\mydll.dll/test.html
//                   res://..\..\bin\mydll.dll/2/101
//                   res://./2/101
//                   res://./1000
//                   res://./1000.html
//                   res://#1000
//                   res://1000.html
//     pUri - Pointer to XTP_URI structure that will be filled with valid
//            values on successful return.
//     pDefaults - Optional pointer to default URI values.
// Returns: S_OK if success, otherwise a standard error code that indicates
//          the problem occured.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPParseUri(LPCTSTR lpszUri, XTP_URI* pUri,
											 const XTP_URI_DEFAULTS* pDefaults = NULL);

//-----------------------------------------------------------------------
// Summary:
//     Creates a read-only stream on object identified by the specified URI.
// Parameters:
//     pUri - Pointer to an object URI structure.
//     grfMode - STGM flags.
//     ppStream - Output pointer that receives stream pointer.
// Returns:
//     S_OK if successful, or a standard COM error code.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateReadOnlyStreamOnUri(const XTP_URI* pUri,
															  IStream** ppStream);

//-----------------------------------------------------------------------
// Summary:
//     Creates a read-only stream on object identified by the specified URI.
// Parameters:
//     lpszUri - Object URI string.
//     grfMode - STGM flags.
//     ppStream - Output pointer that receives stream pointer.
//     pDefaults - Optional pointer to default URI values.
// Returns:
//     S_OK if successful, or a standard COM error code.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateReadOnlyStreamOnUri(
	LPCTSTR lpszUri, IStream** ppStream, const XTP_URI_DEFAULTS* pDefaults = NULL);

//-----------------------------------------------------------------------
// Summary:
//     Loads an arbitrary text as Unicode. If source text is not
//     in Unicode format, converts to Unicode depending on detected
//     text encoding. Supported encodings: ANSI, UTF-8, Unicode.
// Parameters:
//     pRawData - Pointer to raw text data to load.
//     cbRawData - Size of raw text data in bytes.
//     lpUnicodeText - Pointer to a buffer large enough to hold text
//                     after being converted to Unicode.
//     pchUnicodeText - On input, contains number of WCHARs available in
//                      lpUnicodeText buffer including terminating 0.
//                      On output, contains number of WCHARs placed to the
//                      buffer excluding terminating 0.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPLoadUnicodeText(const void* pRawData, UINT cbRawData,
												 LPWSTR lpUnicodeText, UINT* pchUnicodeText);

//-----------------------------------------------------------------------
// Summary:
//     Creates a read-only stream on a block of memory.
// Parameters:
//     pData - Pointer to the beginning of a memory block.
//     cbData - Memory block size in bytes.
//     bClone - If TRUE, the block will be cloned for exclusive stream use
//              and released when the returned stream is destructed.
//     ppStream - Output pointer that receives stream pointer.
// Returns:
//     S_OK if successful, or a standard COM error code.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateReadOnlyMemoryStream(const void* pData, UINT cbData,
															   BOOL bClone, IStream** ppStream);

//-----------------------------------------------------------------------
// Summary:
//     Creates a stream on the file specified.
// Parameters:
//     lpPath - File path.
//     grfMode - A combination of STGM flags from Windows SDK.
//     ppStream - Output pointer that receives stream pointer.
// Returns:
//     S_OK if successful, or a standard COM error code.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateFileStream(LPCTSTR lpPath, DWORD grfMode,
													 IStream** ppStream);

//-----------------------------------------------------------------------
// Summary:
//     Creates a read-only stream on the file specified.
// Parameters:
//     lpPath - File path.
//     ppStream - Output pointer that receives stream pointer.
// Returns:
//     S_OK if successful, or a standard COM error code.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateReadOnlyFileStream(LPCTSTR lpPath, IStream** ppStream);

//-----------------------------------------------------------------------
// Summary:
//     Creates a read-only stream on a resource.
// Parameters:
//     hModule - Module handle where to look for the resource specified.
//     lpResource - Resource name pointer or identifier.
//     lpResourceType - Resource type name pointer or identifier.
//     bClone - If TRUE, the block will be cloned for exclusive stream use
//              and released when the returned stream is destructed.
//     ppStream - Output pointer that receives stream pointer.
// Returns:
//     S_OK if successful, or a standard COM error code.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateReadOnlyResourceStream(HMODULE hModule,
																 LPCTSTR lpResource,
																 LPCTSTR lpResourceType,
																 BOOL bClone, IStream** ppStream);

//===========================================================================
// Summary:
//     Implements thread local variable adapter.
// Parameter:
//     T - type of thread local variable. Must be DefaultConstructible.
//===========================================================================
template<class T>
class CXTPThreadLocal
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Construct adapter only, no thread local variable is constructed.
	//-----------------------------------------------------------------------
	CXTPThreadLocal();

	//-----------------------------------------------------------------------
	// Summary:
	//     Deallocates all thread local variables.
	//-----------------------------------------------------------------------
	~CXTPThreadLocal();

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if an adapter is valid and can create thread local variables.
	// Returns:
	//     TRUE if the adapter is valid.
	//-----------------------------------------------------------------------
	BOOL IsValid() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gives access to a variable accessible only to the caller's thread.
	//     If accesses occur for the first time during a thread's
	//     execution, then a new variable is allocated.
	// Returns:
	//     A reference to a thread local variable.
	//-----------------------------------------------------------------------
	T& operator*();

	//-----------------------------------------------------------------------
	// Summary:
	//     Gives access to a variable accessible only to the caller's thread.
	//     If accesses occur for the first time during a thread's
	//     execution, then a new variable is allocated.
	// Returns:
	//     A reference to a thread local variable.
	//-----------------------------------------------------------------------
	const T& operator*() const;

private:
	T* GetData();

	CRITICAL_SECTION m_csDataListAccess;
	DWORD m_dwSlot;
	CList<T*, T*> m_dataList;
};

template<class T>
AFX_INLINE CXTPThreadLocal<T>::CXTPThreadLocal()
{
	InitializeCriticalSection(&m_csDataListAccess);
	m_dwSlot = TlsAlloc();
}

template<class T>
AFX_INLINE CXTPThreadLocal<T>::~CXTPThreadLocal()
{
	if (IsValid())
	{
		TlsFree(m_dwSlot);
	}

	while (!m_dataList.IsEmpty())
	{
		delete m_dataList.RemoveTail();
	}

	DeleteCriticalSection(&m_csDataListAccess);
}

template<class T>
AFX_INLINE BOOL CXTPThreadLocal<T>::IsValid() const
{
	return (TLS_OUT_OF_INDEXES != m_dwSlot);
}

template<class T>
AFX_INLINE T* CXTPThreadLocal<T>::GetData()
{
	T* pData = NULL;

	if (IsValid())
	{
		pData = reinterpret_cast<T*>(TlsGetValue(m_dwSlot));
		if (NULL == pData)
		{
			pData = new T();
			if (TlsSetValue(m_dwSlot, pData))
			{
				EnterCriticalSection(&m_csDataListAccess);
				m_dataList.AddTail(pData);
				LeaveCriticalSection(&m_csDataListAccess);
			}
			else
			{
				delete pData;
				pData = NULL;
			}
		}
	}

	return pData;
}

template<class T>
AFX_INLINE T& CXTPThreadLocal<T>::operator*()
{
	ASSERT(IsValid());
	return *GetData();
}

template<class T>
AFX_INLINE const T& CXTPThreadLocal<T>::operator*() const
{
	return const_cast<CXTPThreadLocal*>(this)->operator*();
}

// -----------------------------------------------------------------------
// Summary:
//     The CXTPExcludedMemberPadding type is used for declaring padding
//     members for the members that were excluded by conditional compilation
//     in order to preserve the layout of the compiled classes.
// -----------------------------------------------------------------------
template<class Type>
class CXTPExcludedMemberPadding
{
	BYTE _[sizeof(Type)];
};

// -----------------------------------------------------------------------
// Summary:
//     Defines known global window classes IDs. One ID represents a whole
//     family of the control including all its verions. Should not be confused
//     with a particular window class.
// -----------------------------------------------------------------------
enum XTPWindowClass
{
	xtpWindowClassRichEdit // RichEdit window class family.
};

// -----------------------------------------------------------------------
// Summary:
//     Defines rules of how window class version has to be selected.
// -----------------------------------------------------------------------
enum XTPWindowClassVersionSelectionVerb
{
	xtpWindowClassVersionExact,	  // The window class version is matched exactly.
	xtpWindowClassVersionAtLeast, // At least the specified window class version should be
								  // available. The largest available is selected in result.
	xtpWindowClassVersionHighest  // The largest available is selected.
};

// -----------------------------------------------------------------------
// Summary:
//     Defines supported RichEdit versions.
// -----------------------------------------------------------------------
enum XTPRichEditWindowClassVersion
{
	xtpRichEditWindowClassVersion10,
	xtpRichEditWindowClassVersion20,
	xtpRichEditWindowClassVersion30,
	xtpRichEditWindowClassVersion31,
	xtpRichEditWindowClassVersion40,
	xtpRichEditWindowClassVersion41,
	xtpRichEditWindowClassVersion50,
	xtpRichEditWindowClassVersion60,
	xtpRichEditWindowClassVersion70,

	xtpRichEditWindowClassDefault = xtpRichEditWindowClassVersion20
};

//-----------------------------------------------------------------------
// Summary:
//     Loads and verifies a window class module of a specified version
//     according to version selection rule.
// Parameters:
//     nClass - One of the predefined window class identifiers.
//     nVerb - Determines how windows class version is selected.
//     pnVersion - On input, must contain a valid window class specific
//                 version identified. In case of success on return,
//                 it contains the actual value of the window class
//                 version selected. Ignored if nVerb == xtpWindowClassVersionHighest.
//     nVersion - Same as pnVersion, but not changed on return.
//     pWndClass - Optional. If not NULL and succeeded, then it gets filled
//                 with window class specific information.
// Returns:
//     TRUE if the class specified has been loaded and verified.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPLoadWindowClass(XTPWindowClass nClass,
												 XTPWindowClassVersionSelectionVerb nVerb,
												 int* pnVersion, LPWNDCLASS pWndClass = NULL);

AFX_INLINE BOOL AFX_CDECL XTPLoadWindowClass(
	XTPWindowClass nClass, XTPWindowClassVersionSelectionVerb nVerb, int nVersion,
	LPWNDCLASS pWndClass =
		NULL) // <combine
			  // XTPLoadWindowClass@XTPWindowClass@XTPWindowClassVersionSelectionVerb@int*@LPWNDCLASS>
{
	return XTPLoadWindowClass(nClass, nVerb, &nVersion, pWndClass);
}

//-----------------------------------------------------------------------
// Summary:
//     Initializes COM and increments and holds COM initialization reference
//     counter for the thread where constructed.
//-----------------------------------------------------------------------
class CXTPComInitializer
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes COM for the calling thread. If COM has already been
	//     initialized, just increments the COM reference counter.
	//-----------------------------------------------------------------------
	CXTPComInitializer()
	{
		m_hr = CoInitialize(NULL);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Decrements COM reference counter and uninitializes COM for
	//     the calling thread if it holds the last COM reference.
	//-----------------------------------------------------------------------
	~CXTPComInitializer()
	{
		if (S_OK == m_hr || S_FALSE == m_hr)
		{
			CoUninitialize();
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains COM initialization status code.
	// Returns:
	//     COM initialization status code.
	//-----------------------------------------------------------------------
	HRESULT GetInitializationStatus() const
	{
		return m_hr;
	}

private:
	CXTPComInitializer(const CXTPComInitializer&);
	CXTPComInitializer& operator=(const CXTPComInitializer&);

	HRESULT m_hr;
};

template<class Owner, class Observer>
class CXTPObservable;

//=======================================================================
// Summary:
//     Holds a list of observable objects to which an oberser is subscribed.
//     When a list is destructed, it unsubscribes the observer from all
//     subscribed observable objects.
// Parameters:
//     Observable - Type of the observable object.
//     Observer - Type of the observer object.
// See also:
//     CXTPObservable
//=======================================================================
template<class Observable, class Observer>
class CXTPSubscriptionList
{
	friend class CXTPObservable<Observable, Observer>;

	struct SubscriptionInfo
	{
		CXTPObservable<Observable, Observer>* pObservable;
		BOOL bMarkedUnsubscribed;
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs a subscription list for the provided observer.
	// Parameters:
	//      observer - Observer reference.
	//-----------------------------------------------------------------------
	explicit CXTPSubscriptionList(Observer& observer);

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles subscription list destruction.
	//-----------------------------------------------------------------------
	~CXTPSubscriptionList();

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains the number of registered subscriptions for the observer.
	// Returns:
	//      The number of registered subscriptions for the observer.
	//-----------------------------------------------------------------------
	UINT GetSubscriptionCount() const;

private:
	void OnSubscribed(CXTPObservable<Observable, Observer>* pObservable);
	void OnUnsubscribed(CXTPObservable<Observable, Observer>* pObservable);

	Observer& m_Observer;
	CList<SubscriptionInfo, SubscriptionInfo&> m_Subscriptions;
	BOOL m_bBeingDestructed;
};

//-----------------------------------------------------------------------
// Summary:
//     Provides a basic implementation of the Observable design pattern.
// Parameters:
//     Owner - Type of the derived owner class.
//     Observer - Type of an observer object.
//-----------------------------------------------------------------------
template<class Owner, class Observer>
class CXTPObservable
{
	struct ObserverInfo
	{
		Observer* pObserver;
		BOOL bRemove;
		CXTPSubscriptionList<Owner, Observer>* pSubcriptionList;
	};

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs an observable object.
	//-----------------------------------------------------------------------
	CXTPObservable();

	//-----------------------------------------------------------------------
	// Summary:
	//     Handles observable object destruction.
	//-----------------------------------------------------------------------
	~CXTPObservable();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Subscribes a unique observer for events.
	// Parameters:
	//     pObserver - A pointer to an observer object to be subscribed to events.
	//     pSubcriptionList - An optional pointer to the subscriber list. If
	//                        provided, then the observable object pointer is
	//                        added to the subscription list.
	//-----------------------------------------------------------------------
	void Subscribe(Observer* pObserver,
				   CXTPSubscriptionList<Owner, Observer>* pSubcriptionList = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Unsubscribes an observer from events.
	// Parameters:
	//     pObserver - A pointer to the observer object to unsubscribe from events.
	// Remarks:
	//     If an observer has been previously subscribed with the provided subscription
	//     list pointer, then the subscription list is notified about observer
	//     unsubscription for removing the observable object from the list.
	// See also:
	//     CXTPObservable::Subscribe
	//-----------------------------------------------------------------------
	void Unsubscribe(Observer* pObserver);

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called for
	//     each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//-----------------------------------------------------------------------
	void Notify(void (Observer::*method)(Owner*))
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this));
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of
	//     subscription about an event determined by an observer's method
	//     to be called for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//-----------------------------------------------------------------------
	void NotifyReversed(void (Observer::*method)(Owner*))
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this));
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called for
	//     each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O>
	void NotifyDynamic(void (T::*method)(O*))
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)));
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called for
	//     each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O>
	void NotifyDynamicReversed(void (T::*method)(O*))
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)));
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class A1>
	void Notify(void (Observer::*method)(Owner*, A1), A1 arg1)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this), arg1);
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class A1>
	void NotifyReversed(void (Observer::*method)(Owner*, A1), A1 arg1)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this), arg1);
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called for
	//     each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O, class A1>
	void NotifyDynamic(void (T::*method)(O*, A1), A1 arg1)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)), arg1);
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called for
	//     each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O, class A1>
	void NotifyDynamicReversed(void (T::*method)(O*, A1), A1 arg1)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)), arg1);
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class A1, class A2>
	void Notify(void (Observer::*method)(Owner*, A1, A2), A1 arg1, A2 arg2)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this), arg1, arg2);
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class A1, class A2>
	void NotifyReversed(void (Observer::*method)(Owner*, A1, A2), A1 arg1, A2 arg2)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this), arg1, arg2);
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O, class A1, class A2>
	void NotifyDynamic(void (T::*method)(O*, A1, A2), A1 arg1, A2 arg2)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)), arg1,
											   arg2);
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O, class A1, class A2>
	void NotifyDynamicReversed(void (T::*method)(O*, A1, A2), A1 arg1, A2 arg2)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)), arg1,
											   arg2);
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg3 - The third argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class A1, class A2, class A3>
	void Notify(void (Observer::*method)(Owner*, A1, A2, A3), A1 arg1, A2 arg2, A3 arg3)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this), arg1, arg2, arg3);
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg3 - The third argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class A1, class A2, class A3>
	void NotifyReversed(void (Observer::*method)(Owner*, A1, A2, A3), A1 arg1, A2 arg2, A3 arg3)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				(m_Observers[i].pObserver->*method)(static_cast<Owner*>(this), arg1, arg2, arg3);
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg3 - The third argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O, class A1, class A2, class A3>
	void NotifyDynamic(void (T::*method)(O*, A1, A2, A3), A1 arg1, A2 arg2, A3 arg3)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)), arg1,
											   arg2, arg3);
				}
			}
		}

		OnAfterNotify();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies all subscribed observers in the reversed order of subscription
	//     about an event determined by an observer's method to be called
	//     for each subscribed observer.
	// Parameters:
	//     method - A pointer to the observer's method to be called for each
	//              subscribed observer.
	//     arg1 - The first argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg2 - The second argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//     arg3 - The third argument to be passed to each call of the specified
	//            method for each subscribed observer.
	//-----------------------------------------------------------------------
	template<class T, class O, class A1, class A2, class A3>
	void NotifyDynamicReversed(void (T::*method)(O*, A1, A2, A3), A1 arg1, A2 arg2, A3 arg3)
	{
		ASSERT(NULL != method);

		OnBeforeNotify();

		CXTPTypeId specializedTypeId = CXTPTypeIdOf<T>::Id();
		for (INT_PTR i = m_Observers.GetSize() - 1; 0 <= i; --i)
		{
			if (!m_Observers[i].bRemove)
			{
				T* pCastedObserver = reinterpret_cast<T*>(
					static_cast<CXTPTypeIdProvider<Observer>*>(m_Observers[i].pObserver)
						->TryCastTo(specializedTypeId));
				if (NULL != pCastedObserver)
				{
					(pCastedObserver->*method)(static_cast<O*>(static_cast<Owner*>(this)), arg1,
											   arg2, arg3);
				}
			}
		}

		OnAfterNotify();
	}

private:
	CArray<ObserverInfo, ObserverInfo&> m_Observers;
	LONG m_nNotificationDepth;

	INT_PTR FindObserver(Observer* pObserver) const;
	void OnBeforeNotify();
	void OnAfterNotify();
	void RemoveObserver(INT_PTR nIndex);
};

template<class Observable, class Observer>
AFX_INLINE CXTPSubscriptionList<Observable, Observer>::CXTPSubscriptionList(Observer& observer)
	: m_Observer(observer)
	, m_bBeingDestructed(FALSE)
{
}

template<class Observable, class Observer>
AFX_INLINE CXTPSubscriptionList<Observable, Observer>::~CXTPSubscriptionList()
{
	m_bBeingDestructed = TRUE;

	POSITION pos = m_Subscriptions.GetHeadPosition();
	while (NULL != pos)
	{
		SubscriptionInfo& subscription = m_Subscriptions.GetNext(pos);
		if (!subscription.bMarkedUnsubscribed)
		{
			subscription.pObservable->Unsubscribe(&m_Observer);
		}
	}

	ASSERT("Unable to unsubscribe from all subscriptions. The dangling observer pointer can lead "
		   "to unspecified behavior."
		   && 0 == GetSubscriptionCount());
}

template<class Observable, class Observer>
AFX_INLINE UINT CXTPSubscriptionList<Observable, Observer>::GetSubscriptionCount() const
{
	UINT nCount	 = 0;
	POSITION pos = m_Subscriptions.GetHeadPosition();
	while (NULL != pos)
	{
		const SubscriptionInfo& subscription = m_Subscriptions.GetNext(pos);
		if (!subscription.bMarkedUnsubscribed)
		{
			++nCount;
		}
	}

	return nCount;
}

template<class Observable, class Observer>
AFX_INLINE void CXTPSubscriptionList<Observable, Observer>::OnSubscribed(
	CXTPObservable<Observable, Observer>* pObservable)
{
	ASSERT(NULL != pObservable);

	SubscriptionInfo subscriptionInfo = { pObservable, FALSE };
	m_Subscriptions.AddTail(subscriptionInfo);
}

template<class Observable, class Observer>
AFX_INLINE void CXTPSubscriptionList<Observable, Observer>::OnUnsubscribed(
	CXTPObservable<Observable, Observer>* pObservable)
{
	POSITION pos = m_Subscriptions.GetHeadPosition();
	while (NULL != pos)
	{
		POSITION oldPos				   = pos;
		SubscriptionInfo& subscription = m_Subscriptions.GetNext(pos);
		if (!subscription.bMarkedUnsubscribed && subscription.pObservable == pObservable)
		{
			if (!m_bBeingDestructed)
			{
				m_Subscriptions.RemoveAt(oldPos);
			}
			else
			{
				subscription.bMarkedUnsubscribed = TRUE;
			}
		}
	}
}

template<class Owner, class Observer>
AFX_INLINE CXTPObservable<Owner, Observer>::CXTPObservable()
	: m_nNotificationDepth(0)
{
}

template<class Owner, class Observer>
AFX_INLINE CXTPObservable<Owner, Observer>::~CXTPObservable()
{
	ASSERT("All observers must be unsubscribed before destruction" && 0 == m_Observers.GetSize());
}

template<class Owner, class Observer>
AFX_INLINE void CXTPObservable<Owner, Observer>::Subscribe(
	Observer* pObserver, CXTPSubscriptionList<Owner, Observer>* pSubcriptionList /*= NULL*/)
{
	ASSERT(NULL != pObserver);

	if (FindObserver(pObserver) == -1)
	{
		ObserverInfo observerInfo = { pObserver, FALSE, pSubcriptionList };
		m_Observers.Add(observerInfo);

		if (NULL != pSubcriptionList)
		{
			pSubcriptionList->OnSubscribed(this);
		}
	}
}

template<class Owner, class Observer>
AFX_INLINE void CXTPObservable<Owner, Observer>::Unsubscribe(Observer* pObserver)
{
	INT_PTR nIndex = FindObserver(pObserver);
	if (0 <= nIndex)
	{
		if (0 == m_nNotificationDepth)
		{
			RemoveObserver(nIndex);
		}
		else
		{
			ASSERT(0 < m_nNotificationDepth);
			m_Observers[nIndex].bRemove = TRUE;
		}
	}
}

template<class Owner, class Observer>
AFX_INLINE INT_PTR CXTPObservable<Owner, Observer>::FindObserver(Observer* pObserver) const
{
	ASSERT(NULL != pObserver);

	INT_PTR nIndex = -1;
	for (INT_PTR i = 0; i < m_Observers.GetSize(); ++i)
	{
		if (m_Observers[i].pObserver == pObserver)
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

template<class Owner, class Observer>
AFX_INLINE void CXTPObservable<Owner, Observer>::OnBeforeNotify()
{
	++m_nNotificationDepth;
}

template<class Owner, class Observer>
AFX_INLINE void CXTPObservable<Owner, Observer>::OnAfterNotify()
{
	if (0 == --m_nNotificationDepth)
	{
		CArray<INT_PTR, INT_PTR> indices;
		INT_PTR i;
		for (i = 0; i < m_Observers.GetSize(); ++i)
		{
			if (m_Observers[i].bRemove)
			{
				indices.Add(i);
			}
		}

		for (i = indices.GetSize(); 0 < i; --i)
		{
			RemoveObserver(indices[i - 1]);
		}
	}

	ASSERT(0 <= m_nNotificationDepth);
}

template<class Owner, class Observer>
AFX_INLINE void CXTPObservable<Owner, Observer>::RemoveObserver(INT_PTR nIndex)
{
	ASSERT(nIndex < m_Observers.GetSize());

	ObserverInfo& observerInfo = m_Observers.ElementAt(nIndex);
	if (NULL != observerInfo.pSubcriptionList)
	{
		observerInfo.pSubcriptionList->OnUnsubscribed(this);
	}

	m_Observers.RemoveAt(nIndex);
}

//-----------------------------------------------------------------------
// Summary:
//     Call this function to determine if the class specified by pClassThis
//     is derived from the class specified in the pBaseClass parameter.
//     Unlike CRuntimeClass::IsDerivedFrom, this function compares actual
//     class information which allows using it for comparison of the
//     classes declared in different physical modules.
// Parameters:
//     pBaseClass - Base class information pointer.
//     pClassThis - Derived class information pointer.
// Returns:
//     TRUE if the derived class has the same base class as its parent.
// See Also:
//     XTP_DYNAMIC_DOWNCAST_REMOTE,XTPDynamicDownCastRemote,XTPIsKindOfRemote
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPIsDerivedFromRemote(const CRuntimeClass* pBaseClass,
													 const CRuntimeClass* pClassThis);

//-----------------------------------------------------------------------
// Summary:
//     Tests this object's relationship to a given class.
//     Unlike CObject::IsKindOf, this function compares actual
//     class information which allows using it for comparison of the
//     classes declared in different physical modules.
// Parameters:
//     pBaseClass - Base class information pointer.
//     pObject - Derived class object pointer.
// Returns:
//     TRUE if the object corresponds to the class, otherwise FALSE.
// See Also:
//     XTP_DYNAMIC_DOWNCAST_REMOTE,XTPDynamicDownCastRemote,XTPIsDerivedFromRemote
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPIsKindOfRemote(const CRuntimeClass* pClass,
												const CObject* pObject);

//-----------------------------------------------------------------------
// Summary:
//     Provides a handy way to cast a pointer to a pointer to a class object
//     while checking to see if the cast is legal. Unlike AfxDynamicDowncast,
//     this function compares actual class information which allows using it
//     for casting of the classes declared in different physical modules.
// Parameters:
//     pClass - Class information pointer.
//     pObject - Derived class object pointer.
// Returns:
//     If the object referenced by the pointer is a "kind of" the identified class,
//     then the function returns the appropriate pointer. If it is not a legal cast,
//     then the function returns NULL.
// See Also:
//     XTP_DYNAMIC_DOWNCAST_REMOTE,XTPIsKindOfRemote,XTPIsDerivedFromRemote
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CObject* AFX_CDECL XTPDynamicDownCastRemote(CRuntimeClass* pClass, CObject* pObject);

//-----------------------------------------------------------------------
// Summary:
//     Provides a handy way to cast a pointer to a pointer to a class object
//     while checking to see if the cast is legal. Unlike DYNAMIC_DOWNCAST,
//     this macro compares actual class information which allows using it
//     for casting of the classes declared in different physical modules.
// Parameters:
//     className - Class name to which an object is to be casted.
//     pObject - Derived class object pointer.
// Returns:
//     If the object referenced by the pointer is a "kind of" the identified class,
//     then the macro returns the appropriate pointer. If it is not a legal cast,
//     then the macro returns NULL.
// See Also:
//     XTPDynamicDownCastRemote,XTPIsKindOfRemote,XTPIsDerivedFromRemote
//-----------------------------------------------------------------------
#	define XTP_DYNAMIC_DOWNCAST_REMOTE(className, pObject)                                        \
		static_cast<className*>(XTPDynamicDownCastRemote(RUNTIME_CLASS(className), pObject))

//{{AFX_CODEJOCK_PRIVATE
#	ifdef _XTP_ACTIVEX
#		define XTP_DYNAMIC_DOWNCAST_REMOTE_ XTP_DYNAMIC_DOWNCAST_REMOTE
#	else
#		define XTP_DYNAMIC_DOWNCAST_REMOTE_ DYNAMIC_DOWNCAST
#	endif
//}}AFX_CODEJOCK_PRIVATE

//=============================================================================
// Summary:
//     CXTPNonClientMetrics is a self-initializing NONCLIENTMETRICS derived
//     class. It contains the scalable metrics associated with the
//     non-client area of a non-minimized window. This class is used by
//     the SPI_GETNONCLIENTMETRICS and SPI_SETNONCLIENTMETRICS actions of
//     SystemParametersInfo.
// Example:
//     The following example demonstrates using CXTPNonClientMetrics.
// <code>
// CXTPLogFont lfMenuFont;
// CXTPNonClientMetrics ncm;
//
// lfMenuFont.lfCharSet = (BYTE)::GetTextCharsetInfo (dc, NULL, 0);
// lfMenuFont.lfHeight  = ncm.lfMenuFont.lfHeight;
// lfMenuFont.lfWeight  = ncm.lfMenuFont.lfWeight;
// lfMenuFont.lfItalic  = ncm.lfMenuFont.lfItalic;
//
// STRCPY_S(lfMenuFont.lfFaceName, LF_FACESIZE, ncm.lfMenuFont.lfFaceName);
// </code>
//=============================================================================
class _XTP_EXT_CLASS CXTPNonClientMetrics : public NONCLIENTMETRICS
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPNonClientMetrics object.
	//-------------------------------------------------------------------------
	CXTPNonClientMetrics();
};

//=======================================================================
// Summary:
//      A wrapper around the Toolhelp32 API. The API is loaded dynamically
//      and IsInitialized can be used for checking if the API is supported.
//      Refer to Platform SDK documentation on Toolhelp32 API details.
//=======================================================================
class _XTP_EXT_CLASS CXTPToolhelpApi
{
	friend class CXTPSingleton<CXTPToolhelpApi>;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains the only global class instance.
	//-----------------------------------------------------------------------
	static CXTPToolhelpApi& AFX_CDECL Instance();

	//-----------------------------------------------------------------------
	// Summary:
	//      The method should be used for checking if the Toolhelp32 API
	//      is supported and can be used.
	// Returns:
	//      TRUE if the Toolhelp32 API is supported and can be used.
	//-----------------------------------------------------------------------
	BOOL IsInitialized() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Takes a snapshot of the specified processes, as well as the heaps,
	//      modules, and threads used by these processes.
	// Parameters:
	//      dwFlags - The portions of the system to be included in the snapshot.
	//      th32ProcessID - The process identifier of the process to be included
	//                      in the snapshot.
	// Returns:
	//      If the function succeeds, it returns an open handle to the specified
	//      snapshot. Otherwise, the result value is INVALID_HANDLE_VALUE.
	//-----------------------------------------------------------------------
	HANDLE CreateSnapshot(DWORD dwFlags, DWORD th32ProcessID);

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves information about the first thread of any process
	//      encountered in a system snapshot.
	// Parameters:
	//      hSnapshot - A handle to the snapshot returned from a previous call
	//                  to the CreateToolhelp32Snapshot function.
	//      lpte - A pointer to a THREADENTRY32 structure.
	// Returns:
	//      TRUE if the first entry of the thread list has been copied
	//      to the buffer, otherwise FALSE.
	// See also:
	//      GetNextThread
	//-----------------------------------------------------------------------
	BOOL GetFirstThread(HANDLE hSnapshot, LPTHREADENTRY32 lpte);

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves information about the next thread of any process encountered
	//      in the system memory snapshot.
	// Parameters:
	//      hSnapshot - A handle to the snapshot returned from a previous call
	//                  to the CreateToolhelp32Snapshot function.
	//      lpte - A pointer to a THREADENTRY32 structure.
	// Returns:
	//      TRUE if the next entry of the thread list has been copied
	//      to the buffer, otherwise FALSE.
	// See also:
	//      GetFirstThread
	//-----------------------------------------------------------------------
	BOOL GetNextThread(HANDLE hSnapshot, LPTHREADENTRY32 lpte);

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves information about the first module associated with a process.
	// Parameters:
	//      hSnapshot - A handle to the snapshot returned from a previous call
	//                  to the CreateToolhelp32Snapshot function.
	//      lpme - A pointer to a MODULEENTRY32 structure.
	// Returns:
	//      TRUE if the first entry of the module list has been copied
	//      to the buffer, otherwise FALSE.
	// See also:
	//      GetNextModule
	//-----------------------------------------------------------------------
	BOOL GetFirstModule(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves information about the next module associated with a process
	//      or thread.
	// Parameters:
	//      hSnapshot - A handle to the snapshot returned from a previous call
	//                  to the CreateToolhelp32Snapshot function.
	//      lpme - A pointer to a MODULEENTRY32 structure.
	// Returns:
	//      TRUE if the next entry of the module list has been copied
	//      to the buffer, otherwise FALSE.
	// See also:
	//      GetFirstModule
	//-----------------------------------------------------------------------
	BOOL GetNextModule(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

private:
	typedef HANDLE(WINAPI* PFNCREATETOOLHELP32SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
	typedef BOOL(WINAPI* PFNTHREAD32FIRSTNEXT)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
	typedef BOOL(WINAPI* PFNMODULE32FIRSTNEXT)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

	CXTPToolhelpApi();
	~CXTPToolhelpApi();

	BOOL m_bInitialized;
	PFNCREATETOOLHELP32SNAPSHOT m_pfnCreateToolhelp32Snapshot;
	PFNTHREAD32FIRSTNEXT m_pfnThread32First;
	PFNTHREAD32FIRSTNEXT m_pfnThread32Next;
	PFNMODULE32FIRSTNEXT m_pfnModule32First;
	PFNMODULE32FIRSTNEXT m_pfnModule32Next;
};

AFX_INLINE BOOL CXTPToolhelpApi::IsInitialized() const
{
	return m_bInitialized;
}

//=======================================================================
// Summary:
//     A helper class for handling a global atom primitive.
//=======================================================================
class _XTP_EXT_CLASS CXTPGlobalAtom
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a global atom object using the provided name. If an atom
	//     with such a name already exists, then its reference counter
	//     will be incremented.
	// Parameters:
	//     lpAtomName - An atom name string.
	//-----------------------------------------------------------------------
	explicit CXTPGlobalAtom(LPCTSTR lpAtomName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Handles global atom destruction. Atom reference counter gets decremented
	//     and will be destroyed if it reaches zero.
	//-----------------------------------------------------------------------
	~CXTPGlobalAtom();

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains atom value.
	// Returns:
	//     Atom value.
	//-----------------------------------------------------------------------
	ATOM GetAtom() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains atom value as a string pointer.
	// Returns:
	//     Atom value as a string pointer. The value must not be used for string
	//     operations except where permitted by SDK documentation.
	//-----------------------------------------------------------------------
	LPCTSTR GetAtomString() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains atom name.
	// Returns:
	//      Atom name string.
	//-----------------------------------------------------------------------
	const CString& GetName() const;

private:
	ATOM m_atom;
	CString m_strName;
};

AFX_INLINE ATOM CXTPGlobalAtom::GetAtom() const
{
	return m_atom;
}

AFX_INLINE LPCTSTR CXTPGlobalAtom::GetAtomString() const
{
	return MAKEINTATOM(m_atom);
}

AFX_INLINE const CString& CXTPGlobalAtom::GetName() const
{
	return m_strName;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSYSTEMHELPERS_H__)
