// XTPSingleton.cpp
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
#include "Common/XTPSynchro.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPSingletonPointer::CXTPSingletonPointer(SIZE_T cbObjectSize, Creator pfnCreator,
										   Destroyer pfnDestroyer,
										   Initializer pfnInitializer /*= NULL*/)
	: m_pInstance(NULL)
	, m_cbObjectSize(cbObjectSize)
	, m_pfnCreator(pfnCreator)
	, m_pfnDestroyer(pfnDestroyer)
	, m_bDestroyed(FALSE)
{
	ASSERT(1 <= m_cbObjectSize);
	ASSERT(NULL != m_pfnCreator);
	ASSERT(NULL != m_pfnDestroyer);

	if (NULL != pfnInitializer)
	{
		pfnInitializer();
	}

	::InitializeCriticalSection(&m_access);

	XTPAccessExclusive(XTPGetApplication())->Subscribe(this);
}

CXTPSingletonPointer::~CXTPSingletonPointer()
{
	Destroy();
	::DeleteCriticalSection(&m_access);
}

void* CXTPSingletonPointer::GetInstance()
{
	ASSERT(!m_bDestroyed);
	if (m_bDestroyed)
	{
		AfxThrowMemoryException();
	}

	if (NULL == m_pInstance)
	{
		::EnterCriticalSection(&m_access);

		if (NULL == m_pInstance)
		{
			XTP_GUARD_EXCLUSIVE(CXTPApplication, XTPGetApplication())
			{
				// It is necessary to use low level API at this point
				// as runtime might not be initialized properly for 'new' or 'malloc'.
				m_pInstance = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, m_cbObjectSize);

				if (NULL != m_pInstance)
				{
					if (!m_pfnCreator(m_pInstance))
					{
						::HeapFree(::GetProcessHeap(), 0, m_pInstance);
						m_pInstance = NULL;
					}
				}
			}
		}

		::LeaveCriticalSection(&m_access);

		if (NULL == m_pInstance)
		{
			AfxThrowMemoryException();
		}
	}

	return m_pInstance;
}

void CXTPSingletonPointer::Destroy()
{
	if (!m_bDestroyed)
	{
		XTPAccessExclusive(XTPGetApplication())->Unsubscribe(this);

		if (NULL != m_pInstance)
		{
			m_pfnDestroyer(m_pInstance);

			// It is necessary to use low level API at this point
			// as runtime might not be initialized properly for 'delete' or 'free'.
			::HeapFree(::GetProcessHeap(), 0, m_pInstance);

			m_pInstance = NULL;
		}

		m_bDestroyed = TRUE;
	}
}

void CXTPSingletonPointer::OnApplicationShutdown(CXTPApplication* pApplication)
{
	UNREFERENCED_PARAMETER(pApplication);

	Destroy();
}
