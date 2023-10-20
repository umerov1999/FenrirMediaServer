// XTPHookManager.cpp : implementation of the CXTPHookManager class.
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

#include "stdafx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPHookManager.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FALSE_EXIT 2

#ifndef WH_CALLWNDPROCRET
#	define WH_CALLWNDPROCRET 12
#endif

#ifndef WH_KEYBOARD_LL
#	define WH_KEYBOARD_LL 13
#endif

#ifndef WH_MOUSE_LL
#	define WH_MOUSE_LL 14
#endif

#ifdef WH_MAX
#	undef WH_MAX
#endif

#ifndef WH_HARDWARE
#	define WH_HARDWARE 8
#endif

#define WH_MAX WH_MOUSE_LL

#define WH_INDEX(idHook) ((idHook) + 1)
#define ASSERT_IDHOOK(idHook)                                                                      \
	ASSERT(WH_MIN <= (idHook) && (idHook) <= WH_MAX);                                              \
	ASSERT("Global only hooks are not supported" && WH_JOURNALPLAYBACK != (idHook));               \
	ASSERT("Global only hooks are not supported" && WH_JOURNALRECORD != (idHook));                 \
	ASSERT("Global only hooks are not supported" && WH_KEYBOARD_LL != (idHook));                   \
	ASSERT("Global only hooks are not supported" && WH_MOUSE_LL != (idHook));                      \
	ASSERT("Global only hooks are not supported" && WH_SYSMSGFILTER != (idHook));

CXTPHookManagerHookAble::CXTPHookManagerHookAble()
{
	m_bAutoDestroy = FALSE;
}

CXTPHookManagerHookAble::~CXTPHookManagerHookAble()
{
}

int CXTPHookManagerHookAble::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam,
										   LRESULT& lResult)
{
	// Do nothing by default
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(nMessage);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lResult);
	return FALSE;
}

int CXTPHookManagerHookAble::OnPostHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam,
											   LPARAM& lParam, LRESULT& lResult)
{
	// Do nothing by default
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(nMessage);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lResult);
	return FALSE;
}

void CXTPHookManagerHookAble::OnHookAttached(HWND hWnd)
{
	// Do nothing by default
	UNREFERENCED_PARAMETER(hWnd);
}

void CXTPHookManagerHookAble::OnHookDetached()
{
	// Do nothing by default
}

class CXTPHookManager::CHookSink : public CArray<CXTPHookManagerHookAble*, CXTPHookManagerHookAble*>
{
	friend class CXTPHookManager;

public:
	CHookSink(int idHook, HHOOK hHook, DWORD dwThreadId);
	CHookSink(HWND hWnd);

	~CHookSink();

public:
	void UnsubclassWnd();

	int Find(CXTPHookManagerHookAble* pItem)
	{
		ASSERT(NULL != pItem);

		for (int i = 0; i < GetSize(); i++)
		{
			if (pItem == GetAt(i))
				return i;
		}
		return -1;
	}

	void Add(CXTPHookManagerHookAble* pHook)
	{
		ASSERT(NULL != pHook);

		CArray<CXTPHookManagerHookAble*, CXTPHookManagerHookAble*>::Add(pHook);
		pHook->OnHookAttached(m_hWnd);
	}

	void RemoveHook(CXTPHookManagerHookAble* pHook)
	{
		ASSERT(NULL != pHook);

		int nIndex = Find(pHook);
		if (nIndex != -1)
		{
			RemoveAt(nIndex);
			pHook->OnHookDetached();

			if (pHook->m_bAutoDestroy)
				delete pHook;

			if (GetSize() == 0)
			{
				if (NULL != m_hWnd)
				{
					UnsubclassWnd();
					XTPHookManager()->m_mapHooks.RemoveKey(m_hWnd);
				}
				else
				{
					XTPHookManager()->m_WindowsHooks[WH_INDEX(m_idHook)].RemoveKey(m_dwThreadId);
				}

				delete this;
			}
		}
	}

public:
	BOOL OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	BOOL OnPostHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam,
						   LRESULT& lResult);

protected:
	int m_idHook;
	HHOOK m_hHook;
	DWORD m_dwThreadId;

	WNDPROC m_pOldWndProc;
	HWND m_hWnd;
	BOOL m_bUnicode;
	BOOL m_bLastMessage;
#ifdef _AFXDLL
	AFX_MODULE_STATE* m_pModuleState;
#endif
};

#if (_MSC_VER <= 1200) && !defined(_WIN64)
#	define GetWindowLongPtrW GetWindowLongW
#	define GetWindowLongPtrA GetWindowLongA
#	define SetWindowLongPtrW SetWindowLongW
#	define SetWindowLongPtrA SetWindowLongA
#endif

CXTPHookManager::CHookSink::CHookSink(int idHook, HHOOK hHook, DWORD dwThreadId)
	: m_idHook(idHook)
	, m_hHook(hHook)
	, m_dwThreadId(dwThreadId)
	, m_hWnd(NULL)
#ifdef _AFXDLL
	, m_pModuleState(NULL)
#endif
{
}

CXTPHookManager::CHookSink::CHookSink(HWND hWnd)
{
	ASSERT(NULL != hWnd);

	m_idHook	 = 0;
	m_hHook		 = NULL;
	m_dwThreadId = 0;

#ifdef _AFXDLL
	m_pModuleState = NULL;
#endif
	m_bUnicode = IsWindowUnicode(hWnd);

	m_hWnd = hWnd;

	m_bLastMessage = FALSE;

	if (m_bUnicode)
	{
		m_pOldWndProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
		SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)CXTPHookManager::HookWndProc);
	}
	else
	{
		m_pOldWndProc = (WNDPROC)GetWindowLongPtrA(hWnd, GWLP_WNDPROC);
		SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)CXTPHookManager::HookWndProc);
	}
}
UINT CXTPHookManager::m_nMsgSinkRemoved = RegisterWindowMessage(_T("WM_HOOKMANAGER_SINKREMOVED"));

void CXTPHookManager::CHookSink::UnsubclassWnd()
{
	ASSERT(NULL != m_hWnd);

	for (int i = (int)GetSize() - 1; i >= 0; i--)
	{
		GetAt(i)->OnHookDetached();
	}

	if (!m_bLastMessage)
	{
		if (::SendMessage(m_hWnd, CXTPHookManager::m_nMsgSinkRemoved,
						  (WPARAM)(LONG_PTR)CXTPHookManager::HookWndProc,
						  (LPARAM)(LONG_PTR)m_pOldWndProc))
			return;
	}

	if (m_bUnicode)
		SetWindowLongPtrW(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pOldWndProc);
	else
		SetWindowLongPtrA(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pOldWndProc);
}

CXTPHookManager::CHookSink::~CHookSink()
{
	for (int i = (int)GetSize() - 1; i >= 0; i--)
	{
		CXTPHookManagerHookAble* pHookAble = GetAt(i);
		if (pHookAble->m_bAutoDestroy)
		{
			delete pHookAble;
		}
	}

	if (NULL != m_hHook)
	{
		::UnhookWindowsHookEx(m_hHook);
	}
}

BOOL CXTPHookManager::CHookSink::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam,
											   LPARAM& lParam, LRESULT& lResult)
{
	BOOL bResult  = FALSE;
	BOOL bProceed = TRUE;

	if (NULL != m_hWnd)
	{
		if (nMessage == CXTPHookManager::m_nMsgSinkRemoved
			&& (LONG_PTR)wParam == (LONG_PTR)m_pOldWndProc)
		{
			m_pOldWndProc = (WNDPROC)lParam;
			lResult		  = TRUE;
			bProceed	  = FALSE;
			bResult		  = TRUE;
		}
	}
	else
	{
		bProceed = (GetCurrentThreadId() == m_dwThreadId);
	}

	if (bProceed)
	{
		int nCount = (int)GetSize();

		for (int i = nCount - 1; i >= 0; i--)
		{
			int nResult = GetAt(i)->OnHookMessage(hWnd, nMessage, wParam, lParam, lResult);

			if (nResult == TRUE)
			{
				bResult = TRUE;
				break;
			}

			if (nResult == FALSE_EXIT)
			{
				break;
			}
		}
	}

	return bResult;
}

BOOL CXTPHookManager::CHookSink::OnPostHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam,
												   LPARAM& lParam, LRESULT& lResult)
{
	ASSERT(NULL == m_hHook);

	if (nMessage == CXTPHookManager::m_nMsgSinkRemoved
		&& (LONG_PTR)wParam == (LONG_PTR)m_pOldWndProc)
	{
		m_pOldWndProc = (WNDPROC)lParam;
		lResult		  = TRUE;
		return TRUE;
	}

	int nCount = (int)GetSize();

	for (int i = nCount - 1; i >= 0; i--)
	{
		int nResult = GetAt(i)->OnPostHookMessage(hWnd, nMessage, wParam, lParam, lResult);

		if (nResult == TRUE)
			return TRUE;
		if (nResult == FALSE_EXIT)
			return FALSE;
	}

	return FALSE;
}

CXTPHookManager* AFX_CDECL XTPHookManager()
{
	return &CXTPSingleton<CXTPHookManager>::Instance();
}

CXTPHookManager::CXTPHookManager()
	: m_pGuard(new CXTPSimpleCriticalSection)
{
	m_WindowsHooks.SetSize(WH_MAX + 1);
}

CXTPHookManager::~CXTPHookManager()
{
	RemoveAll();
	delete m_pGuard;
}

LRESULT CXTPHookManager::Default(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHookSink* pSink = Lookup(hWnd);
	ASSERT(pSink);
	if (!pSink)
		return 0;

	WNDPROC wndProc = pSink->m_pOldWndProc;

	LRESULT lResult = ::CallWindowProc(wndProc, hWnd, message, wParam, lParam);

	return lResult;
}

LRESULT CXTPHookManager::Default(WPARAM wParam, LPARAM lParam)
{
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;

	return Default(curMsg.hwnd, curMsg.message, wParam, lParam);
}

LRESULT CXTPHookManager::Default()
{
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	return Default(curMsg.hwnd, curMsg.message, curMsg.wParam, curMsg.lParam);
	;
}

LRESULT CALLBACK CXTPHookManager::HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG& curMsg	= AfxGetThreadState()->m_lastSentMsg;
	MSG oldMsg	 = curMsg;
	curMsg.hwnd	= hWnd;
	curMsg.message = message;
	curMsg.wParam  = wParam;
	curMsg.lParam  = lParam;

	// Get hook object for this window. Get from hook map
	CHookSink* pSink = XTPHookManager()->Lookup(hWnd);
	ASSERT(pSink);
	if (!pSink)
		return 0;

	LRESULT lResult = 0;
	WNDPROC wndProc = pSink->m_pOldWndProc;

	{
#ifdef _AFXDLL
		SAFE_MANAGE_STATE(pSink->m_pModuleState);
#endif

		// Window is being destroyed: unhook all hooks (for this window)
		// and pass message to orginal window proc
		if (message == WM_NCDESTROY)
		{
			XTPHookManager()->RemoveAll(hWnd, TRUE);
		}
		// pass to message hook
		else
		{
			if (pSink->OnHookMessage(hWnd, message, wParam, lParam, lResult))
			{
				curMsg = oldMsg;
				return lResult;
			}
		}
	}

	lResult = ::CallWindowProc(wndProc, hWnd, message, wParam, lParam);

	if (message != WM_NCDESTROY)
	{
		pSink = XTPHookManager()->Lookup(hWnd);
		if (NULL != pSink)
		{
#ifdef _AFXDLL
			SAFE_MANAGE_STATE(pSink->m_pModuleState);
#endif
			pSink->OnPostHookMessage(hWnd, message, wParam, lParam, lResult);
		}
	}

	curMsg = oldMsg;
	return lResult;
}

LRESULT CALLBACK CXTPHookManager::OnWH_MSGFILTER(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_MSGFILTER, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_KEYBOARD(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_KEYBOARD, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_GETMESSAGE(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_GETMESSAGE, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_CALLWNDPROC(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_CALLWNDPROC, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_CBT(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_CBT, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_MOUSE(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_MOUSE, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_HARDWARE(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_HARDWARE, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_DEBUG(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_DEBUG, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_SHELL(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_SHELL, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_FOREGROUNDIDLE(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_FOREGROUNDIDLE, nCode, wParam, lParam);
}

LRESULT CALLBACK CXTPHookManager::OnWH_CALLWNDPROCRET(int nCode, WPARAM wParam, LPARAM lParam)
{
	return XTPHookManager()->OnWindowsHookProc(WH_CALLWNDPROCRET, nCode, wParam, lParam);
}

LRESULT CXTPHookManager::OnWindowsHookProc(int idHook, int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	ASSERT_IDHOOK(idHook);

	LRESULT lResult = 0;
	if (WH_MIN <= idHook && idHook <= WH_MAX)
	{
		CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

		CHookSink* pSink = NULL;
		if (m_WindowsHooks[WH_INDEX(idHook)].Lookup(GetCurrentThreadId(), pSink))
		{
#ifdef _AFXDLL
			SAFE_MANAGE_STATE(pSink->m_pModuleState);
#endif
			pSink->OnHookMessage(NULL, XTPToUInt(nCode), wParam, lParam, lResult);
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void CXTPHookManager::SetHook(HWND hWnd, CXTPHookManagerHookAble* pHook)
{
	ASSERT(NULL != pHook);

	CHookSink* pSink = Lookup(hWnd);

	if (pSink)
	{
		if (pSink->Find(pHook) == -1)
		{
			pSink->Add(pHook);
		}
	}
	else
	{
		pSink = new CHookSink(hWnd);
		pSink->Add(pHook);

		CXTPSimpleCriticalSection::CLock lock(*m_pGuard);
		m_mapHooks[hWnd] = pSink;
	}

#ifdef _AFXDLL
	pSink->m_pModuleState = AfxGetModuleState();
#endif
}

BOOL CXTPHookManager::SetHook(int idHook, CXTPHookManagerHookAble* pHook, DWORD dwThreadId /*= 0*/)
{
	ASSERT(NULL != pHook);
	ASSERT_IDHOOK(idHook);

	if (0 == dwThreadId)
	{
		dwThreadId = GetCurrentThreadId();
	}

	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);
	WindowsHookMap& whm = m_WindowsHooks[WH_INDEX(idHook)];

	CHookSink* pSink = NULL;
	if (!whm.Lookup(dwThreadId, pSink))
	{
		HOOKPROC pfnHookProc = NULL;
		switch (idHook)
		{
			case WH_MSGFILTER: pfnHookProc = CXTPHookManager::OnWH_MSGFILTER; break;
			case WH_KEYBOARD: pfnHookProc = CXTPHookManager::OnWH_KEYBOARD; break;
			case WH_GETMESSAGE: pfnHookProc = CXTPHookManager::OnWH_GETMESSAGE; break;
			case WH_CALLWNDPROC: pfnHookProc = CXTPHookManager::OnWH_CALLWNDPROC; break;
			case WH_CBT: pfnHookProc = CXTPHookManager::OnWH_CBT; break;
			case WH_MOUSE: pfnHookProc = CXTPHookManager::OnWH_MOUSE; break;
			case WH_HARDWARE: pfnHookProc = CXTPHookManager::OnWH_HARDWARE; break;
			case WH_DEBUG: pfnHookProc = CXTPHookManager::OnWH_DEBUG; break;
			case WH_SHELL: pfnHookProc = CXTPHookManager::OnWH_SHELL; break;
			case WH_FOREGROUNDIDLE: pfnHookProc = CXTPHookManager::OnWH_FOREGROUNDIDLE; break;
			case WH_CALLWNDPROCRET: pfnHookProc = CXTPHookManager::OnWH_CALLWNDPROCRET; break;
			default: ASSERT(!"Unsupported hook type"); break;
		}

		if (NULL != pfnHookProc)
		{
			HHOOK hHook = SetWindowsHookEx(idHook, pfnHookProc, NULL, dwThreadId);
			if (NULL != hHook)
			{
				pSink = new CHookSink(idHook, hHook, dwThreadId);
#ifdef _AFXDLL
				pSink->m_pModuleState = AfxGetModuleState();
#endif
				whm[dwThreadId] = pSink;
			}
		}
	}

	if (NULL != pSink)
	{
		pSink->Add(pHook);
	}

	return NULL != pSink;
}

void CXTPHookManager::RemoveAll()
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	HWND hWnd;
	POSITION pos = m_mapHooks.GetStartPosition();
	CHookSink* pSink;

	while (pos)
	{
		m_mapHooks.GetNextAssoc(pos, hWnd, (void*&)pSink);

		pSink->UnsubclassWnd();
		delete pSink;
	}
	m_mapHooks.RemoveAll();

	RemoveAll(WH_MSGFILTER);
	RemoveAll(WH_KEYBOARD);
	RemoveAll(WH_GETMESSAGE);
	RemoveAll(WH_CALLWNDPROC);
	RemoveAll(WH_CBT);
	RemoveAll(WH_MOUSE);
	RemoveAll(WH_HARDWARE);
	RemoveAll(WH_DEBUG);
	RemoveAll(WH_SHELL);
	RemoveAll(WH_FOREGROUNDIDLE);
	RemoveAll(WH_CALLWNDPROCRET);
}

void CXTPHookManager::RemoveAll(HWND hWnd)
{
	RemoveAll(hWnd, FALSE);
}

void CXTPHookManager::RemoveAll(HWND hWnd, BOOL bLastMessage)
{
	CHookSink* pSink = Lookup(hWnd);
	if (pSink)
	{
		pSink->m_bLastMessage = bLastMessage;

		pSink->UnsubclassWnd();

		CXTPSimpleCriticalSection::CLock lock(*m_pGuard);
		m_mapHooks.RemoveKey(hWnd);

		delete pSink;
	}
}

void CXTPHookManager::RemoveAll(CXTPHookManagerHookAble* pHook)
{
	ASSERT(pHook);
	HWND hWnd;
	CHookSink* pSink = NULL;

	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);
	POSITION pos = m_mapHooks.GetStartPosition();
	while (pos)
	{
		m_mapHooks.GetNextAssoc(pos, hWnd, (void*&)pSink);
		pSink->RemoveHook(pHook);
	}

	for (int i = 0; i < m_WindowsHooks.GetSize(); ++i)
	{
		WindowsHookMap& whm = m_WindowsHooks[i];
		pos					= whm.GetStartPosition();
		while (NULL != pos)
		{
			DWORD dwThreadId = 0;
			whm.GetNextAssoc(pos, dwThreadId, pSink);
			pSink->RemoveHook(pHook);
		}
	}
}

void CXTPHookManager::RemoveAll(int idHook)
{
	ASSERT_IDHOOK(idHook);

	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);
	WindowsHookMap& whm = m_WindowsHooks[WH_INDEX(idHook)];
	POSITION pos		= whm.GetStartPosition();
	while (NULL != pos)
	{
		DWORD dwThreadId = 0;
		CHookSink* pSink = NULL;
		whm.GetNextAssoc(pos, dwThreadId, pSink);

		INT_PTR nSize = 0;
		do
		{
			nSize = pSink->GetSize();
			if (0 < nSize)
			{
				pSink->RemoveHook(pSink->GetAt(--nSize));
			}
		} while (0 < nSize);
	}
}

CXTPHookManager::CHookSink* CXTPHookManager::Lookup(HWND hWnd)
{
	CHookSink* pSink;

	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);
	if (m_mapHooks.Lookup(hWnd, (void*&)pSink))
		return pSink;
	return NULL;
}

void CXTPHookManager::RemoveHook(HWND hWnd, CXTPHookManagerHookAble* pHook)
{
	ASSERT(hWnd);
	CHookSink* pSink = Lookup(hWnd);

	if (pSink)
	{
		pSink->RemoveHook(pHook);
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPShadowManager

class CXTPShadowManager::CShadowWnd
	: public CWnd
	, public CXTPHookManagerHookAble
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CShadowWnd object.
	//-------------------------------------------------------------------------
	CShadowWnd(CXTPShadowManager* pShadowManager);
	~CShadowWnd();

public:
	using CWnd::Create;

	BOOL Create(BOOL bHoriz, CRect rcWindow);
	void LongShadow(CShadowList* pList);
	BOOL ExcludeRect(CRect rcExclude);

private:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);

private:
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam,
							  LRESULT& lResult);
	UINT Factor(int& nRed, int& nGreen, int& nBlue, double dFactor);

private:
	BOOL m_bHoriz;
	BOOL m_bExcluded;
	CWnd* m_pShadowsOwner;
	int m_nLevel;
	BOOL m_bControlPopup;
	CXTPShadowManager* m_pShadowsManager;

	friend class CXTPShadowManager;
};

//===========================================================================
// Summary:
//     Shadow list
//===========================================================================
class CXTPShadowManager::CShadowList : public CList<CShadowWnd*, CShadowWnd*>
{
public:
	void AddShadow(CShadowWnd* pShadow)
	{
		pShadow->LongShadow(this);
		AddTail(pShadow);
	}

	void RemoveShadow(CShadowWnd* pShadow)
	{
		POSITION pos = Find(pShadow);
		ASSERT(pos);
		RemoveAt(pos);
	}
};

CXTPShadowManager::CXTPShadowManager()
{
	m_pfnUpdateLayeredWindow = NULL;

	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (hLib)
	{
		m_pfnUpdateLayeredWindow = XTPToFunctionPtr<LPFNUPDATELAYEREDWINDOW>(
			::GetProcAddress(hLib, "UpdateLayeredWindow"));
	}

	m_bAlphaShadow			  = IsAlphaShadow();
	m_nShadowOptions		  = 0;
	m_clrShadowFactor		  = 0;
	m_bUseSystemSaveBitsStyle = TRUE;

	m_pShadows = new CShadowList();
}

CXTPShadowManager::~CXTPShadowManager()
{
	ASSERT(m_pShadows->IsEmpty());
	SAFE_DELETE(m_pShadows);
}

BOOL CXTPShadowManager::IsAlphaShadow()
{
#ifdef NOALPHASHADOW
	return FALSE;
#endif

	if (XTPColorManager()->IsLowResolution())
		return FALSE;

	return (m_pfnUpdateLayeredWindow != NULL);
}

#ifndef SPI_GETDROPSHADOW
#	define SPI_GETDROPSHADOW 0x1024
#endif

void CXTPShadowManager::SetShadow(CWnd* pShadowOwner, const CRect& rcExclude, int nLevel)
{
	if (!(GetShadowOptions() & xtpShadowOfficeAlpha))
	{
		if (!m_bAlphaShadow)
			return;

		BOOL bDropShadow = FALSE;
		SystemParametersInfo(SPI_GETDROPSHADOW, 0, &bDropShadow, 0);

		if (!bDropShadow)
			return;
	}

	CXTPWindowRect rc(pShadowOwner);
	CreateShadow(TRUE, rc, rcExclude, pShadowOwner, FALSE, nLevel);
	CreateShadow(FALSE, rc, rcExclude, pShadowOwner, FALSE, nLevel);

	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	if (pShadowOwner->GetWindowRgn(hRgn) == COMPLEXREGION)
	{
		CreateShadow(2, rc, rcExclude, pShadowOwner, FALSE, nLevel);
	}
	DeleteObject(hRgn);
}

void CXTPShadowManager::SetShadow(CRect rcWindow, CWnd* pShadowOwner)
{
	if ((GetShadowOptions() & (xtpShadowOfficeAlpha | xtpShadowShowPopupControl))
		== (xtpShadowOfficeAlpha | xtpShadowShowPopupControl))
	{
		CreateShadow(TRUE, rcWindow, CXTPEmptyRect(), pShadowOwner, TRUE, 0);
		CreateShadow(FALSE, rcWindow, CXTPEmptyRect(), pShadowOwner, TRUE, 0);
	}
}

POSITION CXTPShadowManager::GetHeadPosition(CWnd* pShadowOwner) const
{
	POSITION pos = m_pShadows->GetHeadPosition();
	while (pos)
	{
		CShadowWnd* pShadow = m_pShadows->GetAt(pos);
		if (pShadow->m_pShadowsOwner == pShadowOwner)
			return pos;
		m_pShadows->GetNext(pos);
	}

	return NULL;
}

CWnd* CXTPShadowManager::GetNext(POSITION& pos) const
{
	CShadowWnd* pShadowResult = m_pShadows->GetNext(pos);

	while (pos)
	{
		CShadowWnd* pShadow = m_pShadows->GetAt(pos);
		if (pShadow->m_pShadowsOwner == pShadowResult->m_pShadowsOwner)
			break;
		m_pShadows->GetNext(pos);
	}

	return pShadowResult;
}

void CXTPShadowManager::RemoveShadow(CWnd* pShadowOwner)
{
	POSITION pos = m_pShadows->GetHeadPosition();
	while (pos)
	{
		CShadowWnd* pShadow = m_pShadows->GetNext(pos);

		if (pShadowOwner == pShadow->m_pShadowsOwner)
		{
			XTPHookManager()->RemoveHook(pShadow->m_pShadowsOwner->GetSafeHwnd(), pShadow);
			DestroyShadow(pShadow);
		}
	}
}

void CXTPShadowManager::OffsetShadow(CWnd* pShadowOwner, CSize szOffset)
{
	BOOL bLayoutRTL = (pShadowOwner->GetExStyle() & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL;

	POSITION pos = m_pShadows->GetHeadPosition();
	while (pos)
	{
		CShadowWnd* pShadow = m_pShadows->GetNext(pos);

		if (pShadowOwner == pShadow->m_pShadowsOwner && pShadow->GetSafeHwnd()
			&& !pShadow->m_bControlPopup)
		{
			if (pShadow->m_bExcluded)
			{
				pShadow->SetWindowRgn(NULL, FALSE);
				pShadow->m_bExcluded = FALSE;
			}

			CXTPWindowRect rc(pShadow);

			if (bLayoutRTL)
			{
				if (pShadow->m_bHoriz == 1)
				{
					pShadow->SetWindowPos(0, rc.left - szOffset.cx, rc.top + szOffset.cy,
										  rc.Width() + szOffset.cx, rc.Height(),
										  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				}
				else if (pShadow->m_bHoriz == 2)
				{
					pShadow->SetWindowPos(0, rc.left, rc.top + szOffset.cy, rc.Width(), rc.Height(),
										  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				}
				else
				{
					pShadow->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height() + szOffset.cy,
										  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				}
			}
			else
			{
				if (pShadow->m_bHoriz == 1)
				{
					pShadow->SetWindowPos(0, rc.left, rc.top + szOffset.cy,
										  rc.Width() + szOffset.cx, rc.Height(),
										  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				}
				else if (pShadow->m_bHoriz == 2)
				{
					pShadow->SetWindowPos(0, rc.left + szOffset.cx, rc.top + szOffset.cy,
										  rc.Width(), rc.Height(),
										  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				}
				else
				{
					pShadow->SetWindowPos(0, rc.left + szOffset.cx, rc.top, rc.Width(),
										  rc.Height() + szOffset.cy,
										  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				}
			}
		}
	}
}

CXTPShadowManager::CShadowWnd* CXTPShadowManager::CreateShadow(BOOL bHoriz, CRect rc,
															   CRect rcExclude, CWnd* pShadowOwner,
															   BOOL bControlPopup, int nLevel)
{
	CShadowWnd* pShadow = new CShadowWnd(this);

	pShadow->m_pShadowsOwner = pShadowOwner;
	pShadow->m_bControlPopup = bControlPopup;
	pShadow->m_nLevel		 = nLevel;
	pShadow->Create(bHoriz, rc);

	m_pShadows->AddShadow(pShadow);

	if (m_nShadowOptions & xtpShadowShowPopupControl)
	{
		pShadow->ExcludeRect(rcExclude);
	}

	POSITION pos = m_pShadows->GetHeadPosition();
	while (pos)
	{
		CShadowWnd* pShadowPrev = m_pShadows->GetNext(pos);
		if (pShadowPrev->m_pShadowsOwner
			&& (bHoriz != 2 || pShadowPrev->m_pShadowsOwner != pShadowOwner)
			&& pShadowPrev->m_nLevel == nLevel)
		{
			pShadow->ExcludeRect(CXTPWindowRect(pShadowPrev->m_pShadowsOwner));
		}
	}

	pShadow->SetWindowPos(
		pShadowOwner->GetExStyle() & WS_EX_TOPMOST ? &CWnd::wndTopMost : &CWnd::wndTop, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	XTPHookManager()->SetHook(pShadowOwner->m_hWnd, pShadow);

	return pShadow;
}

void CXTPShadowManager::DestroyShadow(CShadowWnd* pWnd)
{
	if (pWnd->GetSafeHwnd())
		pWnd->ShowWindow(SW_HIDE);
	pWnd->m_pShadowsOwner = 0;
	m_pShadows->RemoveShadow(pWnd);
	pWnd->DestroyWindow();
	delete pWnd;
}

CXTPShadowManager::CShadowWnd::CShadowWnd(CXTPShadowManager* pShadowManager)
{
	m_pShadowsOwner = 0;
	m_bHoriz		= FALSE;
	m_bExcluded		= FALSE;
	m_nLevel		= 0;

	m_pShadowsManager = pShadowManager;
	CMDTARGET_ADDREF(m_pShadowsManager);
}

CXTPShadowManager::CShadowWnd::~CShadowWnd()
{
	CMDTARGET_RELEASE(m_pShadowsManager);
}

int CXTPShadowManager::CShadowWnd::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam,
												 LPARAM& /*lParam*/, LRESULT& /*lResult*/)
{
	if (nMessage == WM_SHOWWINDOW && wParam == FALSE)
	{
		ShowWindow(SW_HIDE);
	}
	if (nMessage == WM_SHOWWINDOW && wParam == TRUE)
	{
		ShowWindow(SW_SHOWNA);
	}
	if (nMessage == WM_DESTROY)
	{
		XTPHookManager()->RemoveHook(hWnd, this);
		m_pShadowsManager->DestroyShadow(this);
	}
	return 0;
}

BOOL CXTPShadowManager::CShadowWnd::Create(BOOL bHoriz, CRect rcWindow)
{
	CWnd* pSite = m_pShadowsOwner->GetParent();
	ASSERT(pSite);

	if (m_hWnd == 0)
	{
		UINT nFlags = WS_EX_TOOLWINDOW;
		if (m_pShadowsManager->m_bAlphaShadow)
			nFlags |= 0x80000;

		UINT nClassStyle = m_pShadowsManager->m_bUseSystemSaveBitsStyle
							   ? UINT(CS_SAVEBITS | CS_OWNDC)
							   : 0;

		CString strClass = ::AfxRegisterWndClass(nClassStyle,
												 AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		if (!CreateEx(nFlags, strClass, 0, WS_POPUP, CXTPEmptyRect(), pSite, 0))
			return FALSE;
	}
	SetWindowRgn(NULL, FALSE);

	int nWidth = m_pShadowsManager->m_bAlphaShadow ? 4 : 2;

	m_bHoriz = bHoriz;

	CRect rcShadow;

	if (bHoriz == 2)
		rcShadow.SetRect(rcWindow.right - 1, rcWindow.bottom - 1, rcWindow.right, rcWindow.bottom);
	else if (bHoriz == 1)
		rcShadow.SetRect(rcWindow.left + nWidth, rcWindow.bottom, rcWindow.right + nWidth,
						 rcWindow.bottom + nWidth);
	else
		rcShadow.SetRect(rcWindow.right, rcWindow.top + nWidth, rcWindow.right + nWidth,
						 rcWindow.bottom);

	MoveWindow(rcShadow, FALSE);
	return TRUE;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPShadowManager::CShadowWnd, CWnd)
	// Custom macro expansion is necessary in order to work around VC6 compiler bug that causes:
	//   error C2327: 'CShadowWnd::OnEraseBkgnd' : member from enclosing class is not a type name,
	//   static, or enumerator error C2065: 'OnEraseBkgnd' : undeclared identifier error C2440:
	//   'type cast' : cannot convert from 'int *' to 'int (__thiscall CWnd::*)(class CDC *)'
	{ WM_ERASEBKGND,
	  0,
	  0,
	  0,
	  AfxSig_bD,
	  (AFX_PMSG)(AFX_PMSGW)(static_cast<BOOL (AFX_MSG_CALL CWnd::*)(CDC*)>(
		  CXTPShadowManager::CShadowWnd::OnEraseBkgnd)) },
		{ WM_PAINT,
		  0,
		  0,
		  0,
		  AfxSig_vv,
		  (AFX_PMSG)(AFX_PMSGW)(static_cast<void (AFX_MSG_CALL CWnd::*)(void)>(
			  CXTPShadowManager::CShadowWnd::OnPaint)) },
		{ WM_SIZE,
		  0,
		  0,
		  0,
		  AfxSig_vwii,
		  (AFX_PMSG)(AFX_PMSGW)(static_cast<void (AFX_MSG_CALL CWnd::*)(UINT, int, int)>(
			  CXTPShadowManager::CShadowWnd::OnSize)) },
		{ WM_NCHITTEST,
		  0,
		  0,
		  0,
		  AfxSig_wp,
		  (AFX_PMSG)(AFX_PMSGW)(static_cast<LRESULT (AFX_MSG_CALL CWnd::*)(CPoint)>(
			  CXTPShadowManager::CShadowWnd::OnNcHitTest)) },
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

LRESULT CXTPShadowManager::CShadowWnd::OnNcHitTest(CPoint /*point*/)
{
	return (LRESULT)HTTRANSPARENT;
}

void CXTPShadowManager::CShadowWnd::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rcClient(this);

	dc.FillSolidRect(rcClient,
					 m_pShadowsManager->m_bAlphaShadow ? 0 : GetXtremeColor(COLOR_3DSHADOW));
}

BOOL CXTPShadowManager::CShadowWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

BOOL CXTPShadowManager::CShadowWnd::ExcludeRect(CRect rcExclude)
{
	CXTPWindowRect rcWindow(this);
	CRect rcIntersect;
	if (rcIntersect.IntersectRect(rcWindow, rcExclude))
	{
		CXTPClientRect rcClient(this);
		HRGN hrgnClip = ::CreateRectRgnIndirect(&rcClient);

		rcIntersect.OffsetRect(-rcWindow.TopLeft());
		HRGN hrgnIntersect = ::CreateRectRgnIndirect(&rcIntersect);
		CombineRgn(hrgnClip, hrgnClip, hrgnIntersect, RGN_DIFF);
		DeleteObject(hrgnIntersect);

		if (!SetWindowRgn(hrgnClip, FALSE))
			DeleteObject(hrgnClip);
		m_bExcluded = TRUE;
	}
	return TRUE;
}

void CXTPShadowManager::CShadowWnd::LongShadow(CShadowList* pList)
{
	CXTPWindowRect rcWindow(this);

	POSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CShadowWnd* pWnd = pList->GetNext(pos);
		if (m_bHoriz == !pWnd->m_bHoriz)
		{
			CXTPWindowRect rc(pWnd);
			if (m_bHoriz == FALSE)
			{
				if (rcWindow.top == rc.bottom - 1 && rcWindow.right < rc.right
					&& rcWindow.right > rc.left)
				{
					rcWindow.top -= 2 * rc.Height();
					MoveWindow(rcWindow, FALSE);
				}
			}
			else
			{
				if (rcWindow.left == rc.right - 1 && rcWindow.top > rc.top
					&& rcWindow.bottom < rc.bottom)
				{
					rcWindow.left -= 2 * rc.Width() + 1;
					MoveWindow(rcWindow, FALSE);
				}
			}
		}
	}
}

UINT CXTPShadowManager::CShadowWnd::Factor(int& nRed, int& nGreen, int& nBlue, double dFactor)
{
	return (XTPToUIntChecked(nRed * dFactor) << 16) + (XTPToUIntChecked(nGreen * dFactor) << 8)
		   + (XTPToUIntChecked(nBlue * dFactor));
}

void CXTPShadowManager::CShadowWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!cx || !cy || !m_pShadowsManager->m_bAlphaShadow
		|| !m_pShadowsManager->m_pfnUpdateLayeredWindow || !m_pShadowsOwner)
		return;

	BLENDFUNCTION bf;
	bf.BlendOp			   = AC_SRC_OVER;
	bf.BlendFlags		   = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat		   = 0x01;

	POINT pt = { 0, 0 };
	int x, y;
	HBITMAP hbitmap;
	BITMAPINFO bmi;

	CClientDC cDC(this);
	CDC dc;
	dc.CreateCompatibleDC(&cDC);

	// zero the memory for the bitmap info
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= cx;
	bmi.bmiHeader.biHeight		= cy;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32; // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage   = XTPToUIntChecked(cx * cy * 4);

	UINT* pvBits = NULL;

	// create our DIB section and select the bitmap into the dc
	hbitmap = CreateDIBSection(cDC, &bmi, DIB_RGB_COLORS, (void**)&pvBits, NULL, 0x0);

	if (pvBits == NULL || hbitmap == NULL)
		return;

	COLORREF clrFactor = m_pShadowsManager->m_clrShadowFactor;

	int nRed = GetRValue(clrFactor), nGreen = GetGValue(clrFactor), nBlue = GetBValue(clrFactor);

	int nFactor = m_pShadowsManager->m_nShadowOptions & xtpShadowOfficeAlpha ? 1 : 2;

	if (m_bHoriz == 2)
	{
		pvBits[0] = (UINT)4 * 0x0F000000 * nFactor + Factor(nRed, nGreen, nBlue, 1);
	}
	else if (m_bHoriz == 0)
	{
		ASSERT(cx == 4);

		for (x = 0; x < 4; x++)
			for (y = 0; y < 4; y++)
			{
				pvBits[3 - x + (cy - 1 - y) * cx] = (UINT)3 * (x + 1) * (y + 1) * 0x1000000
														* nFactor
													+ Factor(nRed, nGreen, nBlue,
															 double((x + 1) * (y + 1)) / 20);
			}

		for (x = 3; x >= 0; x--)
		{
			UINT nColor = 0x0F000000 * nFactor * (4 - x)
						  + Factor(nRed, nGreen, nBlue, double(4 - x) / 4);

			for (y = 0; y < cy - 4; y++)
				pvBits[x + y * cx] = nColor;
		}
	}
	else
	{
		ASSERT(cy == 4);

		for (x = 0; x < 4; x++)
			for (y = 0; y < 4; y++)
			{
				pvBits[x + y * cx] = (UINT)3 * (x + 1) * (y + 1) * 0x1000000 * nFactor
									 + Factor(nRed, nGreen, nBlue, double((x + 1) * (y + 1)) / 20);
			}

		for (y = 0; y < 4; y++)
		{
			UINT nColor = 0x0F000000 * nFactor * (y + 1)
						  + Factor(nRed, nGreen, nBlue, double(y + 1) / 4);

			for (x = 4; x < cx - 4; x++)
				pvBits[x + y * cx] = nColor;
		}

		for (x = 0; x < 4; x++)
			for (y = 0; y < 4; y++)
			{
				pvBits[cx - 1 - x + y * cx] = (UINT)3 * (x + 1) * (y + 1) * 0x1000000 * nFactor
											  + Factor(nRed, nGreen, nBlue,
													   double((x + 1) * (y + 1)) / 20);
			}
	}

	HBITMAP hOld = (HBITMAP)SelectObject(dc, hbitmap);

	SIZE sz = { cx, cy };

	m_pShadowsManager->m_pfnUpdateLayeredWindow((HWND)GetSafeHwnd(), (HDC)0, 0, &sz,
												dc.GetSafeHdc(), &pt, 0, &bf, 0x02);

	SelectObject(dc, hOld);
	DeleteObject(hbitmap);
	dc.DeleteDC();
}
