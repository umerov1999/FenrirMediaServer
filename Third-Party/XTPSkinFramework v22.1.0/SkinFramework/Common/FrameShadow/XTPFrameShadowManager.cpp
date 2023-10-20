// XTPFrameShadowManager.cpp : implementation file
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
#include "Common/XTPHookManager.h"

#include "Common/FrameShadow/XTPFrameShadowManager.h"
#include "Common/FrameShadow/XTPFrameShadow.h"
#include "Common/IIDs/XTPFrameShadowManagerIIDs.h"

#include "Common/XTPSynchro.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SM_REMOTESESSION
#	define SM_REMOTESESSION 0x1000
#endif

#ifndef WM_WTSSESSION_CHANGE
#	define WM_WTSSESSION_CHANGE 0x02B1
#endif

#ifndef NOTIFY_FOR_THIS_SESSION
#	define NOTIFY_FOR_THIS_SESSION 0
#endif

#ifndef WTS_CONSOLE_CONNECT
#	define WTS_CONSOLE_CONNECT 0x1
#endif

#ifndef WTS_CONSOLE_DISCONNECT
#	define WTS_CONSOLE_DISCONNECT 0x2
#endif

#ifndef WTS_REMOTE_CONNECT
#	define WTS_REMOTE_CONNECT 0x3
#endif

#ifndef WTS_REMOTE_DISCONNECT
#	define WTS_REMOTE_DISCONNECT 0x4
#endif

CXTPFrameShadowManager* XTPFrameShadowManager()
{
	return &CXTPSingleton<CXTPFrameShadowManager>::Instance();
}

///////////////////////////////////////////////////////////////////////////////
// CXTPFrameShadowManager::CFrameShadowHook

class CXTPFrameShadowManager::CFrameShadowHook : public CXTPHookManagerHookAble
{
public:
	explicit CFrameShadowHook(CXTPFrameShadowManager* pManager)
		: m_pManager(pManager)
		, m_nRefs(1)
	{
		ASSERT(NULL != m_pManager);
		m_bAutoDestroy = TRUE;
	}

	int AddRef()
	{
		return ++m_nRefs;
	}

	int Release()
	{
		--m_nRefs;
		ASSERT(0 <= m_nRefs);
		return m_nRefs;
	}

private:
	virtual int OnHookMessage(HWND /*hWnd*/, UINT /*nCode*/, WPARAM& /*wParam*/, LPARAM& lParam,
							  LRESULT& /*lResult*/)
	{
		m_pManager->OnHookMessage(reinterpret_cast<PCWPRETSTRUCT>(lParam));
		return FALSE;
	}

	CXTPFrameShadowManager* m_pManager;
	int m_nRefs;
};

///////////////////////////////////////////////////////////////////////////////
// CXTPFrameShadowManager::CWtsEventListener

class CXTPFrameShadowManager::CWtsEventListener : public CWnd
{
	typedef BOOL(WINAPI* PFNWTSRegisterSessionNotification)(HWND hWnd, DWORD dwFlags);
	typedef BOOL(WINAPI* PFNWTSUnRegisterSessionNotification)(HWND hWnd);

	HMODULE m_hWtsapi32;
	BOOL m_bRegistered;
	PFNWTSRegisterSessionNotification m_pfnWTSRegisterSessionNotification;
	PFNWTSUnRegisterSessionNotification m_pfnWTSUnRegisterSessionNotification;

public:
	CWtsEventListener()
		: m_bRegistered(FALSE)
		, m_pfnWTSRegisterSessionNotification(NULL)
		, m_pfnWTSUnRegisterSessionNotification(NULL)
	{
		if (XTPSystemVersion()->IsWinXPOrGreater())
		{
			m_hWtsapi32 = ::LoadLibrary(_T("wtsapi32.dll"));
			if (NULL != m_hWtsapi32)
			{
				m_pfnWTSRegisterSessionNotification =
					XTPToFunctionPtr<PFNWTSRegisterSessionNotification>(
						::GetProcAddress(m_hWtsapi32, "WTSRegisterSessionNotification"));
				m_pfnWTSUnRegisterSessionNotification =
					XTPToFunctionPtr<PFNWTSUnRegisterSessionNotification>(
						::GetProcAddress(m_hWtsapi32, "WTSUnRegisterSessionNotification"));
			}
		}
	}

	~CWtsEventListener()
	{
		Disable();

		if (NULL != m_hWtsapi32)
		{
			FreeLibrary(m_hWtsapi32);
		}
	}

public:
	void Enable()
	{
		if (!m_bRegistered && NULL != m_pfnWTSRegisterSessionNotification
			&& NULL != m_pfnWTSUnRegisterSessionNotification)
		{
			if (!::IsWindow(*this))
			{
				HWND hWnd = ::CreateWindowEx(0, AfxRegisterWndClass(0),
											 _T("CXTPFrameShadowManager::CWtsEventListener"),
											 WS_OVERLAPPED, -100, -100, -50, -50, NULL, 0, NULL,
											 NULL);
				if (NULL != hWnd)
					Attach(hWnd);
			}

			if (!m_bRegistered && ::IsWindow(*this))
			{
				m_bRegistered = m_pfnWTSRegisterSessionNotification(*this, NOTIFY_FOR_THIS_SESSION);

				if (!m_bRegistered)
					::DestroyWindow(Detach());
			}
		}
	}

	void Disable()
	{
		if (m_bRegistered)
		{
			if (::IsWindow(*this))
			{
				m_pfnWTSUnRegisterSessionNotification(*this);
				::DestroyWindow(Detach());
			}

			m_bRegistered = FALSE;
		}
	}

private:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM /*lParam*/, LRESULT* /*pResult*/)
	{
		BOOL bHandled = FALSE;

		if (WM_WTSSESSION_CHANGE == message)
		{
			switch (wParam)
			{
				case WTS_CONSOLE_CONNECT:
				case WTS_REMOTE_CONNECT:
					XTPFrameShadowManager()->SetInTerminalServiceSession(TRUE, TRUE);
					break;
				case WTS_CONSOLE_DISCONNECT:
				case WTS_REMOTE_DISCONNECT:
					XTPFrameShadowManager()->SetInTerminalServiceSession(FALSE, TRUE);
					break;
			}

			bHandled = TRUE;
		}

		return bHandled;
	}
};

///////////////////////////////////////////////////////////////////////////////
// CXTPFrameShadowManager

CXTPFrameShadowManager::CXTPFrameShadowManager()
	: m_nFlags(xtpFrameShadowsDisabledInTSS)
	, m_nEffectiveFlags(0)
	, m_nDefaultOuterSize(0)
	, m_nDefaultInnerSize(0)
	, m_ptDefaultOffset(0, 0)
	, m_crDefaultColor(0)
	, m_bDefaultTransparency(0)
	, m_bDefaultClipOwner(TRUE)
	, m_pGuard(new CXTPSimpleCriticalSection)
	, m_bInUpdateAllShadows(FALSE)
{
	m_bInTerminalServiceSession = ::GetSystemMetrics(SM_REMOTESESSION);
	UpdateTerminalSessionFlags();

	m_pWtsEventListener = new CWtsEventListener();
#ifdef _XTP_ACTIVEX
	EnableAutomation();
	EnableTypeLib();
#endif
}

CXTPFrameShadowManager::~CXTPFrameShadowManager()
{
	delete m_pWtsEventListener;
	delete m_pGuard;
}

BOOL CXTPFrameShadowManager::IsShadowSupported() const
{
	return CXTPFrameShadow::IsShadowSupported();
}

void CXTPFrameShadowManager::SetFlags(int nFlags)
{
	m_nFlags = nFlags;

	int nOldFlags = GetFlags();

	if (0 != (m_nFlags & xtpFrameShadowsDisabled))
	{
		m_nEffectiveFlags = xtpFrameShadowsDisabled | xtpFrameShadowsInvisible
							| xtpFrameShadowsNotResizeable;
	}
	else
	{
		m_nEffectiveFlags = (m_nFlags & (xtpFrameShadowsInvisible | xtpFrameShadowsNotResizeable));
		UpdateTerminalSessionFlags();
	}

	if (GetFlags() != nOldFlags)
	{
		Notify(&IXTPFrameShadowManagerEvents::OnFlagsChanged, nOldFlags, GetFlags());
	}
}

CXTPFrameShadow* CXTPFrameShadowManager::EnableShadow(
	HWND hWnd, COLORREF crColor /*= (COLORREF)-1*/, UINT nOuterSize /*= 0*/,
	UINT nInnerSize /*= 0*/, CPoint ptOffset /*= CPoint(0, 0)*/, BYTE bTransparency /*= 0*/)
{
	ASSERT(::IsWindow(hWnd));
	DWORD dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
	ASSERT(0 == (dwStyle & WS_CHILD));
	UNUSED(dwStyle);

	CXTPFrameShadow* pShadow = NULL;

	if (IsShadowSupported() && 0 == (GetFlags() & xtpFrameShadowsDisabled))
	{
		CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

		pShadow = GetShadow(hWnd);
		if (NULL == pShadow)
		{
			pShadow = new CXTPFrameShadow(hWnd);
			if (pShadow->Create((COLORREF)-1 != crColor ? crColor : m_crDefaultColor,
								0 < nOuterSize ? nOuterSize : m_nDefaultOuterSize,
								0 < nInnerSize ? nInnerSize : m_nDefaultInnerSize,
								0 < ptOffset.x || 0 < ptOffset.y ? ptOffset : m_ptDefaultOffset,
								0 < bTransparency ? bTransparency : m_bDefaultTransparency))
			{
				if (0 < m_DefaultBlendingOptions.Factors.GetSize())
				{
					pShadow->SetBlendingOptions((UINT)m_DefaultBlendingOptions.Factors.GetSize(),
												m_DefaultBlendingOptions.Factors.GetData(),
												m_DefaultBlendingOptions.Positions.GetData());
				}

				m_mapWnd[hWnd] = pShadow;

				SetShadowThreadMessageHook(pShadow);

				if (1 == m_mapWnd.GetCount())
				{
					m_pWtsEventListener->Enable();
				}

				Notify(&IXTPFrameShadowManagerEvents::OnFrameShadowEnabled, hWnd, pShadow);
			}
			else
			{
				TRACE(_T("Unable to create a frame shadow instance"));
				pShadow->InternalRelease();
				pShadow = NULL;
			}
		}
	}

	return pShadow;
}

CXTPFrameShadow* CXTPFrameShadowManager::EnableShadow(
	CWnd* pWnd, COLORREF crColor /*= (COLORREF)-1*/, UINT nOuterSize /*= 0*/,
	UINT nInnerSize /*= 0*/, CPoint ptOffset /*= CPoint(0, 0)*/, BYTE bTransparency /*= 0*/)
{
	ASSERT_VALID(pWnd);
	ASSERT(0 == (pWnd->GetStyle() & WS_CHILD));

	CXTPFrameShadow* pShadow = NULL;

	if (IsShadowSupported() && 0 == (GetFlags() & xtpFrameShadowsDisabled))
	{
		CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

		pShadow = GetShadow(pWnd);
		if (NULL == pShadow)
		{
			pShadow = new CXTPFrameShadow(pWnd->m_hWnd);
			if (pShadow->Create((COLORREF)-1 != crColor ? crColor : m_crDefaultColor,
								0 < nOuterSize ? nOuterSize : m_nDefaultOuterSize,
								0 < nInnerSize ? nInnerSize : m_nDefaultInnerSize,
								0 < ptOffset.x || 0 < ptOffset.y ? ptOffset : m_ptDefaultOffset,
								0 < bTransparency ? bTransparency : m_bDefaultTransparency))
			{
				if (0 < m_DefaultBlendingOptions.Factors.GetSize())
				{
					pShadow->SetBlendingOptions((UINT)m_DefaultBlendingOptions.Factors.GetSize(),
												m_DefaultBlendingOptions.Factors.GetData(),
												m_DefaultBlendingOptions.Positions.GetData());
				}

				m_mapWnd[pWnd->m_hWnd] = pShadow;

				SetShadowThreadMessageHook(pShadow);

				if (1 == m_mapWnd.GetCount())
				{
					m_pWtsEventListener->Enable();
				}

				Notify(&IXTPFrameShadowManagerEvents::OnFrameShadowEnabled, pWnd->m_hWnd, pShadow);
			}
			else
			{
				TRACE(_T("Unable to create a frame shadow instance"));
				pShadow->InternalRelease();
				pShadow = NULL;
			}
		}
	}

	return pShadow;
}

void CXTPFrameShadowManager::DisableShadow(HWND hWnd)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	CXTPFrameShadow* pShadow = GetShadow(hWnd);
	if (NULL != pShadow)
	{
		Notify(&IXTPFrameShadowManagerEvents::OnFrameShadowDisabled, hWnd, pShadow);
		SetShadowThreadMessageHook(pShadow, FALSE);
		m_mapWnd.RemoveKey(hWnd);

		if (::IsWindow(*pShadow))
		{
			pShadow->DestroyWindow();
		}

		pShadow->InternalRelease();

		if (0 == m_mapWnd.GetCount())
		{
			m_pWtsEventListener->Disable();
		}
	}
}

void CXTPFrameShadowManager::DisableShadow(CWnd* pWnd)
{
	ASSERT(NULL != pWnd);
	DisableShadow(pWnd->m_hWnd);
}

CXTPFrameShadow* CXTPFrameShadowManager::GetShadow(HWND hWnd) const
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	CXTPFrameShadow* pShadow = NULL;
	m_mapWnd.Lookup(hWnd, pShadow);
	return pShadow;
}

CXTPFrameShadow* CXTPFrameShadowManager::GetShadow(CWnd* pWnd) const
{
	ASSERT(NULL != pWnd);
	return GetShadow(pWnd->m_hWnd);
}

POSITION CXTPFrameShadowManager::GetFirstShadowPosition() const
{
	return m_mapWnd.GetStartPosition();
}

CXTPFrameShadow* CXTPFrameShadowManager::GetNextShadow(POSITION& pos) const
{
	ASSERT(NULL != pos);

	HWND hWnd				 = NULL;
	CXTPFrameShadow* pShadow = NULL;
	m_mapWnd.GetNextAssoc(pos, hWnd, pShadow);
	return pShadow;
}

void CXTPFrameShadowManager::FreezeUpdates(BOOL bFreeze /*= TRUE*/)
{
	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->FreezeUpdates(bFreeze);
	}
}

void CXTPFrameShadowManager::SetGlobalOuterSize(UINT nSize)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultOuterSize(nSize);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetOuterSize(nSize);
	}
}

void CXTPFrameShadowManager::SetGlobalInnerSize(UINT nSize)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultInnerSize(nSize);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetInnerSize(nSize);
	}
}

void CXTPFrameShadowManager::SetGlobalOffset(CPoint ptOffset)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultOffset(ptOffset);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetOffset(ptOffset);
	}
}

void CXTPFrameShadowManager::SetGlobalColor(COLORREF crColor)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultColor(crColor);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetColor(crColor);
	}
}

void CXTPFrameShadowManager::SetGlobalTransparency(BYTE bTransparency)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultTransparency(bTransparency);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetTransparency(bTransparency);
	}
}

UINT CXTPFrameShadowManager::GetDefaultBlendingOptions(OUT OPTIONAL float* pFactors,
													   OUT OPTIONAL float* pPositions) const
{
	UINT nCount = (UINT)m_DefaultBlendingOptions.Factors.GetSize();
	if (0 < nCount)
	{
		if (NULL != pFactors)
		{
			memcpy(pFactors, m_DefaultBlendingOptions.Factors.GetData(), nCount * sizeof(float));
		}

		if (NULL != pPositions)
		{
			memcpy(pPositions, m_DefaultBlendingOptions.Positions.GetData(),
				   nCount * sizeof(float));
		}
	}

	return nCount;
}

void CXTPFrameShadowManager::SetDefaultBlendingOptions(UINT nCount, const float* pFactors,
													   const float* pPositions)
{
	if (0 < nCount)
	{
		ASSERT(NULL != pFactors);
		ASSERT(NULL != pPositions);

		m_DefaultBlendingOptions.Factors.SetSize(XTPToIntPtrChecked(nCount));
		m_DefaultBlendingOptions.Positions.SetSize(XTPToIntPtrChecked(nCount));

		for (INT_PTR i = 0; i < XTPToIntPtrChecked(nCount); ++i)
		{
			m_DefaultBlendingOptions.Factors.SetAt(i, pFactors[i]);
			m_DefaultBlendingOptions.Positions.SetAt(i, pPositions[i]);
		}
	}
	else
	{
		m_DefaultBlendingOptions.Factors.RemoveAll();
		m_DefaultBlendingOptions.Positions.RemoveAll();
	}
}

void CXTPFrameShadowManager::SetGlobalBlendingOptions(UINT nCount, const float* pFactors,
													  const float* pPositions)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultBlendingOptions(nCount, pFactors, pPositions);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetBlendingOptions(nCount, pFactors, pPositions);
	}
}

void CXTPFrameShadowManager::SetGlobalClipOwner(BOOL bClipOwner)
{
	CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

	SetDefaultClipOwner(bClipOwner);

	POSITION pos = GetFirstShadowPosition();
	while (NULL != pos)
	{
		CXTPFrameShadow* pShadow = GetNextShadow(pos);
		pShadow->SetClipOwner(bClipOwner);
	}
}

void CXTPFrameShadowManager::SetShadowThreadMessageHook(CXTPFrameShadow* pShadow,
														BOOL bSet /*= FALSE*/)
{
	ASSERT(NULL != pShadow);

	DWORD dwThreadId		= GetWindowThreadProcessId(pShadow->m_hWnd, NULL);
	CFrameShadowHook* pHook = NULL;
	if (m_mapHooks.Lookup(dwThreadId, pHook))
	{
		if (bSet)
		{
			pHook->AddRef();
		}
		else if (0 == pHook->Release())
		{
			XTPHookManager()->RemoveAll(pHook);
			m_mapHooks.RemoveKey(dwThreadId);
		}
	}
	else if (bSet)
	{
		pHook = new CFrameShadowHook(this);
		if (XTPHookManager()->SetHook(WH_CALLWNDPROCRET, pHook, dwThreadId))
		{
			m_mapHooks[dwThreadId] = pHook;
		}
		else
		{
			delete pHook;
		}
	}
}

void CXTPFrameShadowManager::OnHookMessage(PCWPRETSTRUCT pc)
{
	ASSERT(NULL != pc);

	CXTPFrameShadow* pShadow = NULL;

	switch (pc->message)
	{
		case WM_NCPAINT:
			// It is assumed that a window receives WM_NCPAINT when being restored
			// as no other suitable message are received by the hook due to
			// WH_CALLWNDPROCRET specifics.
			pShadow = GetShadow(pc->hwnd);
			if (NULL != pShadow && pShadow->IsIconic())
			{
				pShadow->OnRestore();
			}
			break;
		case WM_STYLECHANGED:
			if (GWL_STYLE == pc->wParam)
			{
				pShadow = GetShadow(pc->hwnd);
				if (NULL != pShadow)
				{
					LPSTYLESTRUCT pStyle = reinterpret_cast<LPSTYLESTRUCT>(pc->lParam);
					pShadow->OnOwnersStyleChanged(pStyle->styleOld, pStyle->styleNew);
				}
			}
			break;
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
			pShadow = GetShadow(pc->hwnd);
			if (NULL != pShadow)
			{
				LPWINDOWPOS lpPos = reinterpret_cast<LPWINDOWPOS>(pc->lParam);
				if (0 != (lpPos->flags & SWP_SHOWWINDOW))
				{
					pShadow->Show();
				}
				else if (0 != (lpPos->flags & SWP_HIDEWINDOW))
				{
					pShadow->Hide();
				}

				pShadow->UpdatePosition();
			}
			break;
		case WM_SHOWWINDOW:
			pShadow = GetShadow(pc->hwnd);
			if (NULL != pShadow)
			{
				if (pc->wParam)
				{
					pShadow->Show();
				}
				else
				{
					pShadow->Hide();
				}
			}
			break;
		case WM_DESTROY: DisableShadow(pc->hwnd); break;
	}
}

void CXTPFrameShadowManager::UpdateAllShadows(BOOL bRedraw /*= FALSE*/)
{
	if (!m_bInUpdateAllShadows)
	{
		CXTPSimpleCriticalSection::CLock lock(*m_pGuard);

		m_bInUpdateAllShadows = TRUE;

		POSITION pos			 = m_mapWnd.GetStartPosition();
		HWND hWnd				 = NULL;
		CXTPFrameShadow* pShadow = NULL;
		while (NULL != pos)
		{
			m_mapWnd.GetNextAssoc(pos, hWnd, pShadow);
			pShadow->Update(TRUE, bRedraw);
		}

		m_bInUpdateAllShadows = FALSE;
	}
}

void CXTPFrameShadowManager::SetInTerminalServiceSession(BOOL bInTSS, BOOL bUpdate /*= FALSE*/)
{
	if (bInTSS != m_bInTerminalServiceSession)
	{
		m_bInTerminalServiceSession = bInTSS;
		SetFlags(m_nFlags);

		if (bUpdate)
		{
			UpdateAllShadows();
		}
	}
}

void CXTPFrameShadowManager::UpdateTerminalSessionFlags()
{
	if (m_bInTerminalServiceSession)
	{
		if (0 != (m_nFlags & xtpFrameShadowsDisabledInTSS))
		{
			m_nEffectiveFlags |= xtpFrameShadowsInvisible | xtpFrameShadowsNotResizeable;
		}
		else if (0 != (m_nFlags & xtpFrameShadowsInvisibleInTSS))
		{
			m_nEffectiveFlags |= xtpFrameShadowsInvisible;
		}
	}
}

void CXTPFrameShadowManager::OnFinalRelease()
{
	CXTPCmdTarget::OnFinalRelease();
}

#ifdef _XTP_ACTIVEX
BEGIN_DISPATCH_MAP(CXTPFrameShadowManager, CXTPCmdTarget)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "Flags", 1, OleGetFlags, OleSetFlags, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultOuterSize", 2, OleGetDefaultOuterSize,
						OleSetDefaultOuterSize, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultInnerSize", 3, OleGetDefaultInnerSize,
						OleSetDefaultInnerSize, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultOffsetX", 4, OleGetDefaultOffsetX,
						OleSetDefaultOffsetX, VT_XSIZE_PIXELS)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultOffsetY", 5, OleGetDefaultOffsetY,
						OleSetDefaultOffsetY, VT_YSIZE_PIXELS)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultColor", 6, OleGetDefaultColor,
						OleSetDefaultColor, VT_COLOR)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultTransparency", 7, OleGetDefaultTransparency,
						OleSetDefaultTransparency, VT_I2)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultClipOwner", 24, OleGetDefaultClipOwner,
						OleSetDefaultClipOwner, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultBlendingFactors", 8,
						OleGetDefaultBlendingFactors, SetNotSupported, VT_VARIANT)
	DISP_PROPERTY_EX_ID(CXTPFrameShadowManager, "DefaultBlendingPositions", 9,
						OleGetDefaultBlendingPositions, SetNotSupported, VT_VARIANT)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "IsShadowSupported", 10, IsShadowSupported, VT_BOOL,
					 VTS_NONE)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "EnableShadow", 11, OleEnableShadow, VT_DISPATCH,
					 VTS_HANDLE VTS_COLOR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I2)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "DisableShadow", 12, OleDisableShadow, VT_EMPTY,
					 VTS_HANDLE)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "IsShadowEnabled", 13, OleIsShadowEnabled, VT_BOOL,
					 VTS_NONE)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "GetShadow", 14, OleGetShadow, VT_DISPATCH, VTS_HANDLE)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "FreezeUpdates", 15, OleFreezeUpdates, VT_EMPTY,
					 VTS_BOOL)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalOuterSize", 16, OleSetGlobalOuterSize,
					 VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalInnerSize", 17, OleSetGlobalInnerSize,
					 VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalOffset", 18, OleSetGlobalOffset, VT_EMPTY,
					 VTS_XSIZE_PIXELS VTS_YSIZE_PIXELS)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalColor", 19, OleSetGlobalColor, VT_EMPTY,
					 VTS_COLOR)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalTransparency", 20, OleSetGlobalTransparency,
					 VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalClipOwner", 25, OleSetGlobalClipOwner,
					 VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetDefaultBlendingOptions", 21,
					 OleSetDefaultBlendingOptions, VT_EMPTY, VTS_VARIANT VTS_VARIANT)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "SetGlobalBlendingOptions", 22,
					 OleSetGlobalBlendingOptions, VT_EMPTY, VTS_VARIANT VTS_VARIANT)
	DISP_FUNCTION_ID(CXTPFrameShadowManager, "FrameShadows", 23, OleGetFrameShadows, VT_DISPATCH,
					 VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CXTPFrameShadowManager, CXTPCmdTarget)
	INTERFACE_PART(CXTPFrameShadowManager, XTPDIID_IFrameShadowManager, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPFrameShadowManager, XTPDIID_IFrameShadowManager)

LPDISPATCH CXTPFrameShadowManager::OleGetFrameShadows()
{
	CRuntimeClass* pRuntimeClass = RUNTIME_CLASS(CFrameShadows);
	CObject* pObject			 = pRuntimeClass->CreateObject();
	CFrameShadows* pDispShadows  = DYNAMIC_DOWNCAST(CFrameShadows, pObject);
	return XTPGetDispatch(pDispShadows, FALSE);
}

long CXTPFrameShadowManager::OleGetFlags()
{
	return (long)GetFlags();
}

void CXTPFrameShadowManager::OleSetFlags(long flags)
{
	SetFlags((int)flags);
}

long CXTPFrameShadowManager::OleGetDefaultOuterSize()
{
	return (long)GetDefaultOuterSize();
}

void CXTPFrameShadowManager::OleSetDefaultOuterSize(long size)
{
	SetDefaultOuterSize((UINT)size);
}

long CXTPFrameShadowManager::OleGetDefaultInnerSize()
{
	return (long)GetDefaultInnerSize();
}

void CXTPFrameShadowManager::OleSetDefaultInnerSize(long size)
{
	SetDefaultInnerSize((UINT)size);
}

OLE_XSIZE_PIXELS CXTPFrameShadowManager::OleGetDefaultOffsetX()
{
	CPoint p = GetDefaultOffset();
	return p.x;
}

void CXTPFrameShadowManager::OleSetDefaultOffsetX(OLE_XSIZE_PIXELS offset)
{
	CPoint p = GetDefaultOffset();
	p.x		 = offset;
	SetDefaultOffset(p);
}

OLE_YSIZE_PIXELS CXTPFrameShadowManager::OleGetDefaultOffsetY()
{
	CPoint p = GetDefaultOffset();
	return p.y;
}

void CXTPFrameShadowManager::OleSetDefaultOffsetY(OLE_YSIZE_PIXELS offset)
{
	CPoint p = GetDefaultOffset();
	p.y		 = offset;
	SetDefaultOffset(p);
}

OLE_COLOR CXTPFrameShadowManager::OleGetDefaultColor()
{
	return GetDefaultColor();
}

void CXTPFrameShadowManager::OleSetDefaultColor(OLE_COLOR color)
{
	SetDefaultColor(color);
}

short CXTPFrameShadowManager::OleGetDefaultTransparency()
{
	return (short)GetDefaultTransparency();
}

void CXTPFrameShadowManager::OleSetDefaultTransparency(short val)
{
	SetDefaultTransparency((BYTE)val);
}

BOOL CXTPFrameShadowManager::OleGetDefaultClipOwner()
{
	return GetDefaultClipOwner();
}

void CXTPFrameShadowManager::OleSetDefaultClipOwner(BOOL bClip)
{
	SetDefaultClipOwner(bClip);
}

VARIANT CXTPFrameShadowManager::OleGetDefaultBlendingFactors()
{
	COleSafeArray retVal;
	DWORD nElems[]			   = { 0 };
	UINT nBlendingOptionsCount = GetDefaultBlendingOptions(NULL, NULL);
	if (nBlendingOptionsCount)
		nElems[0] = nBlendingOptionsCount;
	retVal.Create(VT_R4, 1, nElems);
	if (!nBlendingOptionsCount)
		return retVal.Detach();
	CArray<float, float&> arrFactors;
	arrFactors.SetSize(nBlendingOptionsCount);
	GetDefaultBlendingOptions(arrFactors.GetData(), NULL);
	long i;
	for (i = 0; i < (long)nBlendingOptionsCount; i++)
	{
		float fac = arrFactors.GetAt((UINT)i);
		retVal.PutElement(&i, &fac);
	}
	return retVal.Detach();
}

VARIANT CXTPFrameShadowManager::OleGetDefaultBlendingPositions()
{
	COleSafeArray retVal;
	DWORD nElems[]			   = { 0 };
	UINT nBlendingOptionsCount = GetDefaultBlendingOptions(NULL, NULL);
	if (nBlendingOptionsCount)
		nElems[0] = nBlendingOptionsCount;
	retVal.Create(VT_R4, 1, nElems);
	if (!nBlendingOptionsCount)
		return retVal.Detach();
	CArray<float, float&> arrPositions;
	arrPositions.SetSize(nBlendingOptionsCount);
	GetDefaultBlendingOptions(NULL, arrPositions.GetData());
	long i;
	for (i = 0; i < (long)nBlendingOptionsCount; i++)
	{
		float fac = arrPositions.GetAt((UINT)i);
		retVal.PutElement(&i, &fac);
	}

	return retVal.Detach();
}

void CXTPFrameShadowManager::OleDisableShadow(OLE_HANDLE hWnd)
{
	DisableShadow(reinterpret_cast<HWND>((LONG_PTR)hWnd));
}

BOOL CXTPFrameShadowManager::OleIsShadowEnabled(OLE_HANDLE hWnd)
{
	return NULL != GetShadow(reinterpret_cast<HWND>((LONG_PTR)hWnd));
}

void CXTPFrameShadowManager::OleFreezeUpdates(VARIANT_BOOL bFreeze)
{
	FreezeUpdates(bFreeze);
}

void CXTPFrameShadowManager::OleSetGlobalOuterSize(long size)
{
	SetGlobalOuterSize((UINT)size);
}

void CXTPFrameShadowManager::OleSetGlobalInnerSize(long size)
{
	SetGlobalInnerSize((UINT)size);
}

void CXTPFrameShadowManager::OleSetGlobalOffset(OLE_XSIZE_PIXELS offsetX, OLE_YSIZE_PIXELS offsetY)
{
	CPoint p(offsetX, offsetY);
	SetGlobalOffset(p);
}

void CXTPFrameShadowManager::OleSetGlobalColor(OLE_COLOR color)
{
	SetGlobalColor(color);
}

void CXTPFrameShadowManager::OleSetDefaultBlendingOptions(const VARIANT& factors,
														  const VARIANT& positions)
{
	if ((factors.vt & VT_ARRAY) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((factors.vt & VT_R4) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((positions.vt & VT_ARRAY) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((positions.vt & VT_R4) == 0)
		AfxThrowOleException(E_INVALIDARG);
	COleSafeArray saFac(factors), saPos(positions);

	long upper, low, count, count2, low2, upper2, i;
	saFac.GetUBound(1, &upper);
	saFac.GetLBound(1, &low);
	count = upper - low + 1;

	saPos.GetUBound(1, &upper2);
	saPos.GetLBound(1, &low2);

	count2 = upper - low + 1;
	if (count != count2)
		AfxThrowOleException(E_INVALIDARG);
	CArray<float, float&> facs, poss;
	facs.SetSize(count);
	poss.SetSize(count);
	int fType = factors.vt & (~VT_ARRAY);
	int pType = positions.vt & (~VT_ARRAY);
	for (i = low; i <= upper; i++)
	{
		float val;
		if (fType == VT_VARIANT)
		{
			VARIANT var;
			saFac.GetElement(&i, &var);
			val = var.fltVal;
		}
		else
			saFac.GetElement(&i, &val);
		facs[i - low] = val;
	}

	for (i = low2; i <= upper2; i++)
	{
		float val;
		if (pType == VT_VARIANT)
		{
			VARIANT var;
			saPos.GetElement(&i, &var);
			val = var.fltVal;
		}
		else
			saPos.GetElement(&i, &val);
		poss[i - low] = val;
	}

	SetDefaultBlendingOptions((UINT)count, facs.GetData(), poss.GetData());
}

void CXTPFrameShadowManager::OleSetGlobalBlendingOptions(const VARIANT& factors,
														 const VARIANT& positions)
{
	if ((factors.vt & VT_ARRAY) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((factors.vt & VT_R4) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((positions.vt & VT_ARRAY) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((positions.vt & VT_R4) == 0)
		AfxThrowOleException(E_INVALIDARG);
	COleSafeArray saFac(factors), saPos(positions);

	long upper, low, count, count2, low2, upper2, i;
	saFac.GetUBound(1, &upper);
	saFac.GetLBound(1, &low);
	count = upper - low + 1;

	saPos.GetUBound(1, &upper2);
	saPos.GetLBound(1, &low2);

	count2 = upper - low + 1;
	if (count != count2)
		AfxThrowOleException(E_INVALIDARG);
	CArray<float, float&> facs, poss;
	facs.SetSize(count);
	poss.SetSize(count);
	int fType = factors.vt & (~VT_ARRAY);
	int pType = positions.vt & (~VT_ARRAY);
	for (i = low; i <= upper; i++)
	{
		float val;
		if (fType == VT_VARIANT)
		{
			VARIANT var;
			saFac.GetElement(&i, &var);
			val = var.fltVal;
		}
		else
			saFac.GetElement(&i, &val);
		facs[i - low] = val;
	}

	for (i = low2; i <= upper2; i++)
	{
		float val;
		if (pType == VT_VARIANT)
		{
			VARIANT var;
			saPos.GetElement(&i, &var);
			val = var.fltVal;
		}
		else
			saPos.GetElement(&i, &val);
		poss[i - low] = val;
	}

	SetGlobalBlendingOptions((UINT)count, facs.GetData(), poss.GetData());
}

void CXTPFrameShadowManager::OleSetGlobalTransparency(short val)
{
	SetGlobalTransparency((BYTE)val);
}

void CXTPFrameShadowManager::OleSetGlobalClipOwner(BOOL bClip)
{
	SetGlobalClipOwner(bClip);
}

LPDISPATCH CXTPFrameShadowManager::OleEnableShadow(OLE_HANDLE hWndOLE, OLE_HANDLE color,
												   long outerSize, long innerSize,
												   OLE_XSIZE_PIXELS offsetX,
												   OLE_YSIZE_PIXELS offsetY, short transparancy)
{
	CPoint p(offsetX, offsetY);
	HWND hWnd = reinterpret_cast<HWND>((LONG_PTR)hWndOLE);

	CXTPFrameShadow* pShadow = NULL;
	BYTE tr					 = (BYTE)transparancy;
	pShadow					 = EnableShadow(hWnd, color, outerSize, innerSize, p, tr);
	if (pShadow != NULL)
		return pShadow->GetIDispatch(TRUE);
	return NULL;
}

LPDISPATCH CXTPFrameShadowManager::OleGetShadow(OLE_HANDLE hWnd)
{
	CXTPFrameShadow* shadow = XTPFrameShadowManager()->GetShadow(
		reinterpret_cast<HWND>((LONG_PTR)hWnd));
	if (shadow == NULL)
		return NULL;
	return shadow->GetIDispatch(TRUE);
}

// CFrameShadows

IMPLEMENT_DYNCREATE(CFrameShadows, CXTPCmdTarget)

CFrameShadows::CFrameShadows()
{
	EnableAutomation();
	EnableTypeLib();
}

CFrameShadows::~CFrameShadows()
{
}

void CFrameShadows::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CXTPCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CFrameShadows, CXTPCmdTarget)
	//{{AFX_MSG_MAP(CFrameShadows)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFrameShadows, CXTPCmdTarget)
	//{{AFX_DISPATCH_MAP(CFrameShadows)
	DISP_FUNCTION_ID(CFrameShadows, "_NewEnum", DISPID_NEWENUM, OleNewEnum, VT_UNKNOWN, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CFrameShadows, CXTPCmdTarget)
	INTERFACE_PART(CFrameShadows, XTPDIID_IFrameShadows, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CFrameShadows, XTPDIID_IFrameShadows)
IMPLEMENT_ENUM_VARIANTLIST(CFrameShadows)

/////////////////////////////////////////////////////////////////////////////
// CFrameShadows message handlers

POSITION CFrameShadows::OleGetFirstItemPosition()
{
	return XTPFrameShadowManager()->GetFirstShadowPosition();
}

BOOL CFrameShadows::OleGetNextItem(POSITION& rPos, VARIANT* pVar)
{
	ASSERT(pVar);
	if (!rPos || !pVar)
	{
		return FALSE;
	}

	CXTPFrameShadow* pShadow = XTPFrameShadowManager()->GetNextShadow(rPos);
	if (pShadow == NULL)
	{
		pVar->vt = VT_NULL;
		return FALSE;
	}

	pVar->vt	   = VT_DISPATCH;
	pVar->pdispVal = XTPGetDispatch(pShadow);

	return TRUE;
}
#endif
