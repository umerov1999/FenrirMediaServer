// XTPActiveScriptEngine.h: contains ActiveScript convenience classes declarations.
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
#if !defined(__XTPACTIVESCRIPTENGINE_H__)
#	define __XTPACTIVESCRIPTENGINE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPActiveScriptEngine;

//===========================================================================
// Summary: Wrapper around scriptable object.
//===========================================================================
class _XTP_EXT_CLASS CXTPActiveScriptObjectContext
{
	friend class CXTPActiveScriptEngine;

private:
	//-----------------------------------------------------------------------
	// Summary: Constructs scriptable object context.
	// Parameters:
	//  pEngine - Scripting engine to which an object belongs.
	//  pObject - Object's IDispatch pointer.
	//-----------------------------------------------------------------------
	CXTPActiveScriptObjectContext(CXTPActiveScriptEngine* pEngine, IDispatch* pObject);

	//-----------------------------------------------------------------------
	// Summary: Handles scriptable object context deallocation.
	//-----------------------------------------------------------------------
	~CXTPActiveScriptObjectContext();

public:
	//-----------------------------------------------------------------------
	// Summary: Increments context reference count.
	// Returns: Current number of references.
	//-----------------------------------------------------------------------
	ULONG AddRef();

	//-----------------------------------------------------------------------
	// Summary: Decrements context reference count.
	// Returns: Current number of references.
	//-----------------------------------------------------------------------
	ULONG Release();

	//-----------------------------------------------------------------------
	// Summary: Obtains parent scripting engine instance pointer.
	//-----------------------------------------------------------------------
	CXTPActiveScriptEngine* GetEngine();

	//-----------------------------------------------------------------------
	// Summary: Obtains parent scripting engine instance pointer.
	//-----------------------------------------------------------------------
	const CXTPActiveScriptEngine* GetEngine() const;

	//-----------------------------------------------------------------------
	// Summary: Obtains named property value.
	// Parameters:
	//  lpszName - Object's property name.
	//  pvtResult - A pointer to a value that receives property value.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	HRESULT GetValue(LPCWSTR lpszName, CComVariant* pvtResult);

	//-----------------------------------------------------------------------
	// Summary: Sets named property value.
	// Parameters:
	//  lpszName - Object's property name.
	//  vtValue - New property value.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	HRESULT SetValue(LPCWSTR lpszName, VARIANT vtValue);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object.
	// Parameters:
	//  lpArguments - Pointer to an array of function arguments.
	//  nCount - Number of arguments pointed by lpArguments.
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	HRESULT InvokeWithArgs(VARIANT* lpArguments, UINT nCount, VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object with no arguments.
	// Parameters:
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	HRESULT Invoke(VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object with 1 argument.
	// Parameters:
	//  arg1 - The only function argument.
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	template<class A1>
	HRESULT Invoke(A1 arg1, VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object with 1 argument.
	// Parameters:
	//  arg1 - 1st function argument.
	//  arg2 - 2nd function argument.
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	template<class A1, class A2>
	HRESULT Invoke(A1 arg1, A2 arg2, VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object with 1 argument.
	// Parameters:
	//  arg1 - 1st function argument.
	//  arg2 - 2nd function argument.
	//  arg3 - 3rd function argument.
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	template<class A1, class A2, class A3>
	HRESULT Invoke(A1 arg1, A2 arg2, A3 arg3, VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object with 1 argument.
	// Parameters:
	//  arg1 - 1st function argument.
	//  arg2 - 2nd function argument.
	//  arg3 - 3rd function argument.
	//  arg4 - 4th function argument.
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	template<class A1, class A2, class A3, class A4>
	HRESULT Invoke(A1 arg1, A2 arg2, A3 arg3, A4 arg4, VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Invokes a function object with 1 argument.
	// Parameters:
	//  arg1 - 1st function argument.
	//  arg2 - 2nd function argument.
	//  arg3 - 3rd function argument.
	//  arg4 - 4th function argument.
	//  arg5 - 5th function argument.
	//  pvtResult - Optional pointer to value that revceives a value returned
	//      from function.
	// Returns: S_OK if successful or standard COM error code.
	//-----------------------------------------------------------------------
	template<class A1, class A2, class A3, class A4, class A5>
	HRESULT Invoke(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, VARIANT* pvtResult = NULL);

	//-----------------------------------------------------------------------
	// Summary: Obtains context object's property or function.
	// Parameters:
	//  lpszName - Name of object's property or function.
	// Returns: A pointer to new context object or NULL if failed.
	//-----------------------------------------------------------------------
	CXTPActiveScriptObjectContext* GetContext(LPCWSTR lpszName);

private:
	HRESULT ProcessNamedItem(LPCWSTR lpszName, WORD wFlags, VARIANT* lpArguments, UINT nCount,
							 VARIANT* pvtResult = NULL);

private:
	ULONG m_nRefs;
	CXTPActiveScriptEngine* m_pEngine;
	IDispatch* m_pObject;
};

AFX_INLINE CXTPActiveScriptEngine* CXTPActiveScriptObjectContext::GetEngine()
{
	return m_pEngine;
}

AFX_INLINE const CXTPActiveScriptEngine* CXTPActiveScriptObjectContext::GetEngine() const
{
	return m_pEngine;
}

AFX_INLINE HRESULT CXTPActiveScriptObjectContext::Invoke(VARIANT* pvtResult /*= NULL*/)
{
	return InvokeWithArgs(NULL, 0, pvtResult);
}

template<class A1>
AFX_INLINE HRESULT CXTPActiveScriptObjectContext::Invoke(A1 arg1, VARIANT* pvtResult /*= NULL*/)
{
	CComVariant va(arg1);
	return InvokeWithArgs(&va, 1, pvtResult);
}

template<class A1, class A2>
AFX_INLINE HRESULT CXTPActiveScriptObjectContext::Invoke(A1 arg1, A2 arg2,
														 VARIANT* pvtResult /*= NULL*/)
{
	CComVariant va[] = { CComVariant(arg1), CComVariant(arg2) };
	return InvokeWithArgs(va, _countof(va), pvtResult);
}

template<class A1, class A2, class A3>
AFX_INLINE HRESULT CXTPActiveScriptObjectContext::Invoke(A1 arg1, A2 arg2, A3 arg3,
														 VARIANT* pvtResult /*= NULL*/)
{
	CComVariant va[] = { CComVariant(arg1), CComVariant(arg2), CComVariant(arg3) };
	return InvokeWithArgs(va, _countof(va), pvtResult);
}

template<class A1, class A2, class A3, class A4>
AFX_INLINE HRESULT CXTPActiveScriptObjectContext::Invoke(A1 arg1, A2 arg2, A3 arg3, A4 arg4,
														 VARIANT* pvtResult /*= NULL*/)
{
	CComVariant va[] = { CComVariant(arg1), CComVariant(arg2), CComVariant(arg3),
						 CComVariant(arg4) };
	return InvokeWithArgs(va, _countof(va), pvtResult);
}

template<class A1, class A2, class A3, class A4, class A5>
AFX_INLINE HRESULT CXTPActiveScriptObjectContext::Invoke(A1 arg1, A2 arg2, A3 arg3, A4 arg4,
														 A5 arg5, VARIANT* pvtResult /*= NULL*/)
{
	CComVariant va[] = { CComVariant(arg1), CComVariant(arg2), CComVariant(arg3), CComVariant(arg4),
						 CComVariant(arg5) };
	return InvokeWithArgs(va, _countof(va), pvtResult);
}

//===========================================================================
// Summary: Implements ActiveScript engine component.
//===========================================================================
class _XTP_EXT_CLASS CXTPActiveScriptEngine : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPActiveScriptEngine);
	friend class CXTPActiveScriptObjectContext;

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs scripting engine object.
	//-----------------------------------------------------------------------
	CXTPActiveScriptEngine();

	//-----------------------------------------------------------------------
	// Summary: Handles scripting engine deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPActiveScriptEngine();

public:
	//-----------------------------------------------------------------------
	// Summary: Initializes scripting engine.
	// Parameters:
	//     lpszEngineProgId - ProgID of scripting language to initialize scripting
	//                        engine for. Standard ProgIDs are JScript and VBScript.
	//                        More language providers can be installed if necessary.
	//     pSite - An optional pointer to an engine site interface that receives
	//             notifications and requests from the scripting engine.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT Initialize(LPCWSTR lpszEngineProgId, IActiveScriptSite* pSite = NULL);

	//-----------------------------------------------------------------------
	// Summary: Uninitializes scripting engine.
	//-----------------------------------------------------------------------
	void Uninitialize();

	//-----------------------------------------------------------------------
	// Summary: Loads a chunk of code into execution context. Number of
	//  chunks of code in one execution context is unlimited.
	// Parameters:
	//  lpszCode - Chunk code.
	// Returns: S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT LoadCode(LPCWSTR lpszCode);

	//-----------------------------------------------------------------------
	// Summary: Runs execution context.
	// Returns: S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT Run();

	//-----------------------------------------------------------------------
	// Summary: Obtains global object's context instance.
	// Returns: Global object's context instance pointer or NULL if failed.
	//-----------------------------------------------------------------------
	CXTPActiveScriptObjectContext* GetGlobalContext() const;

	//-----------------------------------------------------------------------
	// Summary: Information about the last unhandled exception.
	// Returns: The last unhandled exception pointer or NULL.
	//-----------------------------------------------------------------------
	const EXCEPINFO* GetExceptionInfo() const;

	//-----------------------------------------------------------------------
	// Summary: Adds named item name and data into execution context.
	// Parameters:
	//  lpszName - Item name pointer.
	//  lpItemData - Opional item data pointer.
	//  dwFlags - Item flags. For all available values refer to
	//      IActiveScript::AddNamedItem Win32 SDK documentation.
	// Returns: S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT AddNamedItem(LPCWSTR lpszName, LPVOID lpItemData = NULL,
						 DWORD dwFlags = SCRIPTITEM_ISVISIBLE);

	//-----------------------------------------------------------------------
	// Summary: Obtains data pointer associated with named item.
	// Parameters:
	//  lpszName - Item name pointer.
	// Returns: Item data pointer value or NULL if not specified.
	//-----------------------------------------------------------------------
	LPVOID GetNamedItemData(LPCWSTR lpszName) const;

private:
	void SetExceptionInfo(const EXCEPINFO* pExcepInfo);

	DECLARE_INTERFACE_MAP();

#	pragma warning(push)
// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4619: #pragma warning : there is no warning number 'number'
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#	pragma warning(disable : 4616 4619 5204)
	BEGIN_INTERFACE_PART(ActiveScriptSite, IActiveScriptSite)
	STDMETHOD(GetLCID)(LCID* plcid);
	STDMETHOD(GetItemInfo)
	(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown** ppiunkItem, ITypeInfo** ppti);
	STDMETHOD(GetDocVersionString)(BSTR* pbstrVersion);
	STDMETHOD(OnScriptTerminate)(const VARIANT* pvarResult, const EXCEPINFO* pexcepinfo);
	STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState);
	STDMETHOD(OnScriptError)(IActiveScriptError* pScriptError);
	STDMETHOD(OnEnterScript)();
	STDMETHOD(OnLeaveScript)();
	END_INTERFACE_PART(ActiveScriptSite);
#	pragma warning(pop)

private:
	IActiveScriptSite* m_pSite;
	IActiveScript* m_pEngine;
	IActiveScriptParse* m_pParser;
	EXCEPINFO* m_pExcepInfo;
	CMap<LPCWSTR, LPCWSTR, LPVOID, LPVOID> m_NamedItemDataMap;
};

AFX_INLINE const EXCEPINFO* CXTPActiveScriptEngine::GetExceptionInfo() const
{
	return m_pExcepInfo;
}

//===========================================================================
// Summary: Implements standard scripting runtime library.
//===========================================================================
class _XTP_EXT_CLASS CXTPActiveScriptStdRuntime : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPActiveScriptStdRuntime);

	class TimeoutTask;
	class TimeoutTaskRegistry;

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs scripting runtime library.
	//-----------------------------------------------------------------------
	CXTPActiveScriptStdRuntime();

	//-----------------------------------------------------------------------
	// Summary: Handles standard scripting runtime library deallocation.
	//-----------------------------------------------------------------------
	~CXTPActiveScriptStdRuntime();

private:
	DECLARE_DISPATCH_MAP();
	afx_msg UINT OleSetTimeout(LPDISPATCH lpDispCallback, UINT nTimeout);
	afx_msg void OleClearTimeout(UINT nTimeoutId);
	afx_msg void OleDebugLog(LPCTSTR lpszMessage);

	static void CALLBACK TimeoutProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

private:
	CMap<UINT, UINT, TimeoutTask*, TimeoutTask*> m_TimeoutTaskMap;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPACTIVESCRIPTENGINE_H__)
