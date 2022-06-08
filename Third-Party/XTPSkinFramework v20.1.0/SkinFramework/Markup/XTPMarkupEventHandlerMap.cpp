// XTPMarkupEventHandlerMap.cpp : implementation file
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

#include "StdAfx.h"

#include "Common/XTPFramework.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupDelegate.h"
#include "Markup/XTPMarkupEventHandlerMap.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupEventHandlerMap

CXTPMarkupEventHandlerMap::CXTPMarkupEventHandlerMap()
{
}

CXTPMarkupEventHandlerMap::~CXTPMarkupEventHandlerMap()
{
	RemoveAll();
}

void CXTPMarkupEventHandlerMap::RemoveAll()
{
	POSITION pos = m_mapHandlers.GetStartPosition();
	while (pos)
	{
		CPtrList* pHandlers;
		CXTPMarkupRoutedEvent* pEvent;
		m_mapHandlers.GetNextAssoc(pos, pEvent, pHandlers);

		while (!pHandlers->IsEmpty())
		{
			CXTPMarkupDelegate* pDelegate = (CXTPMarkupDelegate*)pHandlers->RemoveTail();
			MARKUP_RELEASE(pDelegate);
		}
		delete pHandlers;
	}
	m_mapHandlers.RemoveAll();
}

void CXTPMarkupEventHandlerMap::Add(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate)
{
	CPtrList* pHandlers;
	if (!m_mapHandlers.Lookup(pEvent, pHandlers))
	{
		pHandlers = new CPtrList();
		m_mapHandlers.SetAt(pEvent, pHandlers);
	}

	pHandlers->AddTail(pDelegate);
}

void CXTPMarkupEventHandlerMap::Raise(CXTPMarkupObject* pSender,
									  CXTPMarkupRoutedEventArgs* pEventArgs)
{
	CPtrList* pHandlers;
	if (!m_mapHandlers.Lookup(pEventArgs->GetEvent(), pHandlers))
		return;

	POSITION pos = pHandlers->GetHeadPosition();

	while (pos)
	{
		CXTPMarkupDelegate* pDelegate = (CXTPMarkupDelegate*)pHandlers->GetNext(pos);

		pDelegate->Execute(pSender, pEventArgs);
	}
}
