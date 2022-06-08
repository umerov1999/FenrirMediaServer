// XTPMarkupHyperlink.cpp: implementation of the CXTPMarkupHyperlink class.
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupStyle.h"
#include "Markup/XTPMarkupSetter.h"
#include "Markup/XTPMarkupSetterCollection.h"
#include "Markup/XTPMarkupTrigger.h"
#include "Markup/XTPMarkupTriggerCollection.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/XTPMarkupDrawingContext.h"

#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupButtonBase.h"

#include "Markup/XTPMarkupRoutedEvent.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"
#include "Markup/XTPMarkupMouseButtonEventArgs.h"

#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupSpan.h"
#include "Markup/Text/XTPMarkupHyperlink.h"

#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupHyperlink

CXTPMarkupRoutedEvent* CXTPMarkupHyperlink::m_pClickEvent = NULL;

IMPLEMENT_MARKUPCLASS(L"Hyperlink", CXTPMarkupHyperlink, CXTPMarkupSpan);

void CXTPMarkupHyperlink::RegisterMarkupClass()
{
	CXTPMarkupStyle::RegisterType();
	CXTPMarkupButtonBase::RegisterType();
	CXTPMarkupSolidColorBrush::RegisterType();
	CXTPMarkupHyperlink::RegisterType();

	CXTPMarkupType* pType = MARKUP_TYPE(CXTPMarkupHyperlink);

	CXTPMarkupStyle* pStyle = new CXTPMarkupStyle();
	pStyle->GetSetters()->Add(new CXTPMarkupSetter(CXTPMarkupHyperlink::m_pTextDecorationsProperty,
												   CXTPMarkupEnum::CreateValue(1)));
	pStyle->GetSetters()->Add(new CXTPMarkupSetter(CXTPMarkupHyperlink::m_pForegroundProperty,
												   new CXTPMarkupSolidColorBrush(RGB(0, 0, 255))));
	pStyle->GetSetters()->Add(
		new CXTPMarkupSetter(CXTPMarkupHyperlink::m_pCursorProperty, new CXTPMarkupInt(32649)));

	CXTPMarkupTriggerCollection* pTriggers = new CXTPMarkupTriggerCollection();
	CXTPMarkupTrigger* pTrigger = new CXTPMarkupTrigger(CXTPMarkupHyperlink::m_pIsMouseOverProperty,
														CXTPMarkupBool::CreateTrueValue());
	CXTPMarkupSetterCollection* pSetters = new CXTPMarkupSetterCollection();
	pSetters->Add(new CXTPMarkupSetter(CXTPMarkupHyperlink::m_pForegroundProperty,
									   new CXTPMarkupSolidColorBrush(RGB(255, 0, 0))));
	pTrigger->SetSetters(pSetters);
	pTriggers->Add(pTrigger);
	pStyle->SetTriggers(pTriggers);

	pType->SetTypeStyle(pStyle);

	m_pClickEvent = (CXTPMarkupRoutedEvent*)CXTPMarkupButtonBase::m_pClickEvent->AddOwner(
		MARKUP_TYPE(CXTPMarkupHyperlink));
}

CXTPMarkupHyperlink::CXTPMarkupHyperlink()
{
	m_bPressed = FALSE;
}

void CXTPMarkupHyperlink::OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e)
{
	if (m_bPressed)
	{
		m_bPressed = FALSE;
		OnClick();
		e->SetHandled();
	}
}

void CXTPMarkupHyperlink::OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e)
{
	m_bPressed = TRUE;
	e->SetHandled();
}

void CXTPMarkupHyperlink::OnClick()
{
	CXTPMarkupRoutedEventArgs* e = new CXTPMarkupRoutedEventArgs(m_pClickEvent, this);
	RaiseEvent(e);

	e->Release();
}

/////////////////////////////////////////////////////////////////////////
// CXTPMarkupHyperlink

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupHyperlink, CXTPMarkupSpan)
	DISP_PROPERTY_EX_ID(CXTPMarkupHyperlink, "ClickEvent", 800, OleGetClickEvent, SetNotSupported,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupHyperlink, CXTPMarkupSpan)
	INTERFACE_PART(CXTPMarkupHyperlink, XTPDIID_MarkupHyperlink, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupHyperlink, XTPDIID_MarkupHyperlink)
#endif

LPDISPATCH CXTPMarkupHyperlink::OleGetClickEvent()
{
	return XTPGetDispatch(m_pClickEvent);
}
