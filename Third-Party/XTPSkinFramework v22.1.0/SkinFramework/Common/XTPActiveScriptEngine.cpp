// XTPActiveScriptEngine.cpp: contains ActiveScript convenience classes definitions.
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

#include "StdAfx.h"

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <atlbase.h>
#include <DispEx.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPActiveScriptEngine.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
// CXTPActiveScriptObjectContext implementation

CXTPActiveScriptObjectContext::CXTPActiveScriptObjectContext(CXTPActiveScriptEngine* pEngine,
															 IDispatch* pObject)
	: m_nRefs(1)
	, m_pEngine(pEngine)
	, m_pObject(pObject)
{
	ASSERT(NULL != m_pEngine);
	ASSERT(NULL != m_pObject);

	m_pEngine->GetControllingUnknown()->AddRef();
	m_pObject->AddRef();
}

CXTPActiveScriptObjectContext::~CXTPActiveScriptObjectContext()
{
	ASSERT(0 == m_nRefs);

	m_pObject->Release();
	m_pEngine->GetControllingUnknown()->Release();
}

ULONG CXTPActiveScriptObjectContext::AddRef()
{
	return ++m_nRefs;
}

ULONG CXTPActiveScriptObjectContext::Release()
{
	ASSERT(0 < m_nRefs);

	ULONG nRefs = --m_nRefs;
	if (0 == --nRefs)
	{
		delete this;
	}

	return nRefs;
}

HRESULT CXTPActiveScriptObjectContext::GetValue(LPCWSTR lpszName, CComVariant* pvtResult)
{
	ASSERT(NULL != lpszName);
	ASSERT(NULL != pvtResult);
	return ProcessNamedItem(lpszName, DISPATCH_PROPERTYGET, NULL, 0, pvtResult);
}

HRESULT CXTPActiveScriptObjectContext::SetValue(LPCWSTR lpszName, VARIANT vtValue)
{
	ASSERT(NULL != lpszName);
	return ProcessNamedItem(lpszName, DISPATCH_PROPERTYPUT, &vtValue, 1);
}

HRESULT CXTPActiveScriptObjectContext::InvokeWithArgs(VARIANT* lpArguments /*= NULL*/,
													  UINT nCount /*= 0*/,
													  VARIANT* pvtResult /*= NULL*/)
{
	return ProcessNamedItem(NULL, DISPATCH_METHOD, lpArguments, nCount, pvtResult);
}

CXTPActiveScriptObjectContext* CXTPActiveScriptObjectContext::GetContext(LPCWSTR lpszName)
{
	CXTPActiveScriptObjectContext* pContext = NULL;
	CComVariant value;
	if (SUCCEEDED(GetValue(lpszName, &value)))
	{
		ASSERT(VT_DISPATCH == value.vt);
		pContext = new CXTPActiveScriptObjectContext(m_pEngine, value.pdispVal);
	}

	return pContext;
}

HRESULT CXTPActiveScriptObjectContext::ProcessNamedItem(LPCWSTR lpszName, WORD wFlags,
														VARIANT* lpArguments, UINT nCount,
														VARIANT* pvtResult /*= NULL*/)
{
	ASSERT(NULL != lpArguments ? 0 < nCount : 0 == nCount);

	DISPID dispId = 0;
	HRESULT hr	= S_OK;

	// Obtain named item DISPID.
	if (NULL != lpszName)
	{
		IDispatchEx* pDispEx = NULL;
		hr					 = m_pObject->QueryInterface(&pDispEx);
		if (SUCCEEDED(hr))
		{
			hr = pDispEx->GetDispID(CComBSTR(lpszName), fdexNameCaseSensitive, &dispId);
			if (SUCCEEDED(hr))
			{
				hr = m_pObject->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpszName, 1,
											  LOCALE_SYSTEM_DEFAULT, &dispId);
			}
		}
	}
	else
	{
		dispId = DISPID_VALUE;
	}

	if (SUCCEEDED(hr))
	{
		// If arguments specified, build a reversed list of arguments.
		DISPPARAMS dispParams = { 0 };
		CArray<VARIANT, VARIANT> vtArgs;
		if (NULL != lpArguments)
		{
			vtArgs.SetSize(XTPToIntPtrChecked(nCount));
			for (UINT i = 0; i < nCount; ++i)
			{
				vtArgs.SetAt(XTPToIntPtrChecked(nCount - 1 - i), lpArguments[i]);
			}

			dispParams.rgvarg = &vtArgs[0];
			dispParams.cArgs  = nCount;
		}

		DISPID dispidNamed = DISPID_PROPERTYPUT;
		if (wFlags & DISPATCH_PROPERTYPUT)
		{
			dispParams.cNamedArgs		 = 1;
			dispParams.rgdispidNamedArgs = &dispidNamed;
		}

		// Invoke named item with arguments specified.
		EXCEPINFO exceptionInfo = { 0 };
		hr = m_pObject->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, wFlags, &dispParams,
							   pvtResult, &exceptionInfo, NULL);
		if (FAILED(hr) && DISP_E_EXCEPTION == hr)
		{
			m_pEngine->SetExceptionInfo(&exceptionInfo);
		}
	}

	return hr;
}

template<>
inline BOOL AFXAPI CompareElements<LPCWSTR, LPCWSTR>(const LPCWSTR* pElement1,
													 const LPCWSTR* pElement2)
{
	return wcscmp(*pElement1, *pElement2) == 0;
}

////////////////////////////////////////////////////////////////////////////////
// CXTPActiveScriptEngine implementation

IMPLEMENT_DYNAMIC(CXTPActiveScriptEngine, CXTPCmdTarget);

BEGIN_INTERFACE_MAP(CXTPActiveScriptEngine, CXTPCmdTarget)
	INTERFACE_PART(CXTPActiveScriptEngine, IID_IActiveScriptSite, ActiveScriptSite)
END_INTERFACE_MAP()

CXTPActiveScriptEngine::CXTPActiveScriptEngine()
	: m_pSite(NULL)
	, m_pEngine(NULL)
	, m_pParser(NULL)
	, m_pExcepInfo(NULL)
{
}

CXTPActiveScriptEngine::~CXTPActiveScriptEngine()
{
	Uninitialize();
}

HRESULT CXTPActiveScriptEngine::Initialize(LPCWSTR lpszEngineProgId,
										   IActiveScriptSite* pSite /*= NULL*/)
{
	ASSERT(NULL != lpszEngineProgId);

	HRESULT hr = E_FAIL;

	Uninitialize();

	do
	{
		// Create engine instance.
		CLSID clsidScriptEngine;
		hr = CLSIDFromProgID(lpszEngineProgId, &clsidScriptEngine);
		if (FAILED(hr))
		{
			break;
		}

		hr = CoCreateInstance(clsidScriptEngine, NULL, CLSCTX_ALL, IID_IActiveScript,
							  (LPVOID*)&m_pEngine);
		if (FAILED(hr))
		{
			break;
		}

		// Initialize parser.
		hr = m_pEngine->QueryInterface(&m_pParser);
		if (FAILED(hr))
		{
			break;
		}

		hr = m_pParser->InitNew();
		if (FAILED(hr))
		{
			break;
		}

		hr = m_pEngine->SetScriptSite(&m_xActiveScriptSite);
		if (FAILED(hr))
		{
			break;
		}

		m_pSite = pSite;
		if (NULL != m_pSite)
		{
			m_pSite->AddRef();
		}
	} while (FALSE);

	if (FAILED(hr))
	{
		Uninitialize();
	}

	return hr;
}

void CXTPActiveScriptEngine::Uninitialize()
{
	SetExceptionInfo(NULL);

	// Stop script engine.
	if (NULL != m_pEngine)
	{
		m_pEngine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
	}

	// Release data.
	if (NULL != m_pSite)
	{
		m_pSite->Release();
		m_pSite = NULL;
	}

	if (NULL != m_pParser)
	{
		m_pParser->Release();
		m_pParser = NULL;
	}

	if (NULL != m_pEngine)
	{
		m_pEngine->SetScriptState(SCRIPTSTATE_CLOSED);
		m_pEngine->Release();
		m_pEngine = NULL;
	}

	m_NamedItemDataMap.RemoveAll();
}

HRESULT CXTPActiveScriptEngine::LoadCode(LPCWSTR lpszCode)
{
	ASSERT(NULL != lpszCode);
	ASSERT(NULL != m_pParser);

	HRESULT hr = m_pEngine->SetScriptState(SCRIPTSTATE_INITIALIZED);
	if (SUCCEEDED(hr))
	{
		hr = m_pParser->ParseScriptText(lpszCode, NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISVISIBLE,
										NULL, NULL);
	}

	return hr;
}

HRESULT CXTPActiveScriptEngine::Run()
{
	ASSERT(NULL != m_pEngine);
	return m_pEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
}

CXTPActiveScriptObjectContext* CXTPActiveScriptEngine::GetGlobalContext() const
{
	ASSERT(NULL != m_pEngine);

	CXTPActiveScriptObjectContext* pContext = NULL;
	IDispatch* pDispScript					= NULL;
	if (SUCCEEDED(m_pEngine->GetScriptDispatch(NULL, &pDispScript)))
	{
		pContext = new CXTPActiveScriptObjectContext(const_cast<CXTPActiveScriptEngine*>(this),
													 pDispScript);
	}

	return pContext;
}

HRESULT CXTPActiveScriptEngine::AddNamedItem(LPCWSTR lpszName, LPVOID lpItemData /*= NULL*/,
											 DWORD dwFlags /*= SCRIPTITEM_ISVISIBLE*/)
{
	ASSERT(NULL != lpszName);
	ASSERT(NULL != m_pEngine);
	HRESULT hr = m_pEngine->AddNamedItem(lpszName, dwFlags);
	if (SUCCEEDED(hr) && NULL != lpItemData)
	{
		m_NamedItemDataMap[lpszName] = lpItemData;
	}

	return hr;
}

LPVOID CXTPActiveScriptEngine::GetNamedItemData(LPCWSTR lpszName) const
{
	ASSERT(NULL != lpszName);

	LPVOID lpItemData = NULL;
	m_NamedItemDataMap.Lookup(lpszName, lpItemData);
	return lpItemData;
}

void CXTPActiveScriptEngine::SetExceptionInfo(const EXCEPINFO* pExcepInfo)
{
	if (NULL != m_pExcepInfo)
	{
		SysFreeString(m_pExcepInfo->bstrSource);
		SysFreeString(m_pExcepInfo->bstrDescription);
		SysFreeString(m_pExcepInfo->bstrHelpFile);
		delete m_pExcepInfo;
		m_pExcepInfo = NULL;
	}

	if (NULL != pExcepInfo)
	{
		m_pExcepInfo				  = new EXCEPINFO(*pExcepInfo);
		m_pExcepInfo->bstrSource	  = SysAllocString(pExcepInfo->bstrSource);
		m_pExcepInfo->bstrDescription = SysAllocString(pExcepInfo->bstrDescription);
		m_pExcepInfo->bstrHelpFile	= SysAllocString(pExcepInfo->bstrHelpFile);
	}
}

///////////////////////////////////////////////////////////////////////////////
// IActiveScriptSite implementation

XTP_IMPLEMENT_IUNKNOWN(CXTPActiveScriptEngine, ActiveScriptSite)

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::GetLCID(LCID* plcid)
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = E_NOTIMPL;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->GetLCID(plcid);
	}

	return hr;
}

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::GetItemInfo(LPCOLESTR pstrName,
																		DWORD dwReturnMask,
																		IUnknown** ppiunkItem,
																		ITypeInfo** ppti)
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = TYPE_E_ELEMENTNOTFOUND;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->GetItemInfo(pstrName, dwReturnMask, ppiunkItem, ppti);
	}

	return hr;
}

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::GetDocVersionString(BSTR* pbstrVersion)
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = E_NOTIMPL;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->GetDocVersionString(pbstrVersion);
	}

	return hr;
}

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::OnScriptTerminate(
	const VARIANT* pvarResult, const EXCEPINFO* pexcepinfo)
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = S_OK;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->OnScriptTerminate(pvarResult, pexcepinfo);
	}

	return hr;
}

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::OnStateChange(SCRIPTSTATE ssScriptState)
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = S_OK;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->OnStateChange(ssScriptState);
	}

	return hr;
}

XTP_STDMETHODIMP
CXTPActiveScriptEngine::XActiveScriptSite::OnScriptError(IActiveScriptError* pScriptError)
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);

	EXCEPINFO exceptionInfo;
	if (SUCCEEDED(pScriptError->GetExceptionInfo(&exceptionInfo)))
	{
		pThis->SetExceptionInfo(&exceptionInfo);
	}

	HRESULT hr = E_NOTIMPL;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->OnScriptError(pScriptError);
	}

	return hr;
}

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::OnEnterScript()
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = S_OK;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->OnEnterScript();
	}

	return hr;
}

XTP_STDMETHODIMP CXTPActiveScriptEngine::XActiveScriptSite::OnLeaveScript()
{
	METHOD_PROLOGUE(CXTPActiveScriptEngine, ActiveScriptSite);
	HRESULT hr = S_OK;
	if (NULL != pThis->m_pSite)
	{
		hr = pThis->m_pSite->OnLeaveScript();
	}

	return hr;
}

class CXTPActiveScriptStdRuntime::TimeoutTask
{
public:
	TimeoutTask(CXTPActiveScriptStdRuntime& runtime, LPDISPATCH lpDispCallback, UINT nTimeout)
		: m_lpDispCallback(lpDispCallback)
		, m_nTimeout(nTimeout)
		, m_runtime(runtime)
	{
		ASSERT(NULL != m_lpDispCallback);
		m_lpDispCallback->AddRef();
	}

	~TimeoutTask()
	{
		m_lpDispCallback->Release();
	}

	LPDISPATCH m_lpDispCallback;
	UINT m_nTimeout;
	CXTPActiveScriptStdRuntime& m_runtime;
};

class CXTPActiveScriptStdRuntime::TimeoutTaskRegistry : public CXTPSynchronized
{
	friend class CXTPSingleton<TimeoutTaskRegistry>;

public:
	static TimeoutTaskRegistry* GetInstance()
	{
		return &CXTPSingleton<TimeoutTaskRegistry>::Instance();
	}

	void Add(UINT_PTR nTimeoutId, TimeoutTask* pTask)
	{
		ASSERT(NULL != pTask);

		TimeoutTask* pOldTask = Withdraw(nTimeoutId);
		if (NULL != pOldTask)
		{
			delete pOldTask;
		}

		m_TimeoutTaskMap[nTimeoutId] = pTask;
	}

	TimeoutTask* Withdraw(UINT_PTR nTimeoutId)
	{
		TimeoutTask* pTask = NULL;
		if (m_TimeoutTaskMap.Lookup(nTimeoutId, pTask))
		{
			m_TimeoutTaskMap.RemoveKey(nTimeoutId);
		}

		return pTask;
	}

	TimeoutTask* WithdrawFirst(CXTPActiveScriptStdRuntime& runtime, UINT_PTR& nTimeoutId)
	{
		TimeoutTask* pFirstTask = NULL;
		POSITION pos			= m_TimeoutTaskMap.GetStartPosition();
		while (NULL != pos)
		{
			TimeoutTask* pTask = NULL;
			m_TimeoutTaskMap.GetNextAssoc(pos, nTimeoutId, pTask);
			if (&runtime == &pTask->m_runtime)
			{
				m_TimeoutTaskMap.RemoveKey(nTimeoutId);
				pFirstTask = pTask;
				break;
			}
		}

		return pFirstTask;
	}

private:
	TimeoutTaskRegistry()
	{
	}

public:
	~TimeoutTaskRegistry()
	{
		ASSERT(m_TimeoutTaskMap.IsEmpty());
	}

private:
	CMap<UINT_PTR, UINT_PTR, TimeoutTask*, TimeoutTask*> m_TimeoutTaskMap;
};

IMPLEMENT_DYNAMIC(CXTPActiveScriptStdRuntime, CXTPCmdTarget);

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPActiveScriptStdRuntime, CXTPCmdTarget)
	DISP_FUNCTION(CXTPActiveScriptStdRuntime, "setTimeout", OleSetTimeout, VT_I4,
				  VTS_DISPATCH VTS_I4)
	DISP_FUNCTION(CXTPActiveScriptStdRuntime, "clearTimeout", OleClearTimeout, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CXTPActiveScriptStdRuntime, "debugLog", OleDebugLog, VT_EMPTY, VTS_BSTR)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

CXTPActiveScriptStdRuntime::CXTPActiveScriptStdRuntime()
{
	EnableAutomation();
}

CXTPActiveScriptStdRuntime::~CXTPActiveScriptStdRuntime()
{
	UINT_PTR nTimeoutId = 0;
	TimeoutTask* pTask  = NULL;
	XTP_GUARD_EXCLUSIVE_(TimeoutTaskRegistry, TimeoutTaskRegistry::GetInstance(), pRegistry)
	{
		while (NULL != (pTask = pRegistry->WithdrawFirst(*this, nTimeoutId)))
		{
			KillTimer(NULL, nTimeoutId);
			delete pTask;
		}
	}
}

UINT CXTPActiveScriptStdRuntime::OleSetTimeout(LPDISPATCH lpDispCallback, UINT nTimeout)
{
	UINT_PTR nTimeoutId = 0;
	if (NULL != lpDispCallback)
	{
		nTimeoutId = SetTimer(NULL, 0, nTimeout, CXTPActiveScriptStdRuntime::TimeoutProc);
		if (0 != nTimeoutId)
		{
			XTPAccessExclusive(TimeoutTaskRegistry::GetInstance())
				->Add(nTimeoutId, new TimeoutTask(*this, lpDispCallback, nTimeout));
		}
	}

	return XTPToUInt(nTimeoutId);
}

void CXTPActiveScriptStdRuntime::OleClearTimeout(UINT nTimeoutId)
{
	TimeoutTask* pTask =
		XTPAccessExclusive(TimeoutTaskRegistry::GetInstance())->Withdraw(nTimeoutId);
	if (NULL != pTask)
	{
		KillTimer(NULL, nTimeoutId);
		delete pTask;
	}
}

void CALLBACK CXTPActiveScriptStdRuntime::TimeoutProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent,
													  DWORD dwTime)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(dwTime);
	UNREFERENCED_PARAMETER(uMsg);

	TimeoutTask* pTask = XTPAccessExclusive(TimeoutTaskRegistry::GetInstance())->Withdraw(idEvent);
	if (NULL != pTask)
	{
		KillTimer(NULL, idEvent);

		DISPPARAMS dispParams = { 0 };
		HRESULT hr = pTask->m_lpDispCallback->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT,
													 DISPATCH_METHOD, &dispParams, NULL, NULL,
													 NULL);
		if (FAILED(hr))
		{
			TRACE1("ActiveScriptStdRuntime::setTimeout failed: 0x%08X\n", hr);
		}

		delete pTask;
	}
}

void CXTPActiveScriptStdRuntime::OleDebugLog(LPCTSTR lpszMessage)
{
	if (NULL != lpszMessage)
	{
		OutputDebugString(lpszMessage);
	}
}
