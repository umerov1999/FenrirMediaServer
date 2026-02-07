// XTPTypeId.cpp
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

#include "Common/XTPTypeId.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPTypeIdProviderBase::DerivedList::DerivedList(CXTPTypeId derivedTypeId,
												 CXTPTypeIdProviderBase* pTypeProvider,
												 void* pInstance, DerivedList* pFLink)
	: derivedTypeId(derivedTypeId)
	, pTypeProvider(pTypeProvider)
	, pInstance(pInstance)
	, pFLink(pFLink)
{
}

CXTPTypeIdProviderBase::CXTPTypeIdProviderBase(CXTPTypeId typeId)
	: m_typeId(typeId)
	, m_pDerivedList(NULL)
{
}

CXTPTypeIdProviderBase::~CXTPTypeIdProviderBase()
{
	DerivedList* pNode = m_pDerivedList;
	while (NULL != pNode)
	{
		DerivedList* pNextNode = pNode->pFLink;
		delete pNode;
		pNode = pNextNode;
	}
}

CXTPTypeId CXTPTypeIdProviderBase::GetTypeId() const
{
	return m_typeId;
}

void CXTPTypeIdProviderBase::AddDerived(CXTPTypeId typeId, CXTPTypeIdProviderBase* pTypeProvider,
										void* pInstance)
{
	if (typeId != m_typeId)
	{
		m_pDerivedList = new DerivedList(typeId, pTypeProvider, pInstance, m_pDerivedList);
	}
}

void* CXTPTypeIdProviderBase::TryCastTo(CXTPTypeId typeId) const
{
	void* pCasted = NULL;

	DerivedList* pNode = m_pDerivedList;
	while (NULL != pNode)
	{
		if (pNode->derivedTypeId == typeId)
		{
			pCasted = pNode->pInstance;
			break;
		}
		else
		{
			pCasted = pNode->pTypeProvider->TryCastTo(typeId);
			if (NULL != pCasted)
			{
				break;
			}
		}

		pNode = pNode->pFLink;
	}

	return pCasted;
}
