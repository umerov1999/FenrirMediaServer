// XTPMarkupMouseEventArgs.cpp: implementation file
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
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPMarkupMouseEventArgs::CXTPMarkupMouseEventArgs(CXTPMarkupRoutedEvent* pEvent)
	: CXTPMarkupRoutedEventArgs(pEvent, NULL)
{
	m_hWnd	= 0;
	m_point = 0;
}

CPoint CXTPMarkupMouseEventArgs::GetPosition(CXTPMarkupInputElement* relativeTo)
{
	CPoint pt = m_point;

	CXTPMarkupObject* pVisual = relativeTo;

	while (pVisual != 0)
	{
		if (pVisual->IsKindOf(MARKUP_TYPE(CXTPMarkupVisual)))
		{
			pt -= ((CXTPMarkupVisual*)pVisual)->GetVisualOffset();
		}
		pVisual = pVisual->GetLogicalParent();
	}
	return pt;
}
