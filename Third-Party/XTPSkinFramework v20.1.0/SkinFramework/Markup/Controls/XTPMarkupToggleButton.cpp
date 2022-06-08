// XTPMarkupToggleButton.cpp : implementation file
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

#include "Common/XTPCasting.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupButtonBase.h"
#include "Markup/Controls/XTPMarkupToggleButton.h"
#include "Markup/XTPMarkupRoutedEvent.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupToggleButton

CXTPMarkupDependencyProperty* CXTPMarkupToggleButton::m_pIsThreeStateProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupToggleButton::m_pIsCheckedProperty	  = NULL;
CXTPMarkupRoutedEvent* CXTPMarkupToggleButton::m_pCheckedEvent				  = NULL;
CXTPMarkupRoutedEvent* CXTPMarkupToggleButton::m_pUncheckedEvent			  = NULL;
CXTPMarkupRoutedEvent* CXTPMarkupToggleButton::m_pIndeterminateEvent		  = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupToggleButton, CXTPMarkupButtonBase)

void CXTPMarkupToggleButton::RegisterMarkupClass()
{
	m_pCheckedEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"Checked", CXTPMarkupRoutedEvent::routingBubble, MARKUP_TYPE(CXTPMarkupToggleButton));
	m_pUncheckedEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"Unchecked", CXTPMarkupRoutedEvent::routingBubble, MARKUP_TYPE(CXTPMarkupToggleButton));
	m_pIndeterminateEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"Indeterminate", CXTPMarkupRoutedEvent::routingBubble,
		MARKUP_TYPE(CXTPMarkupToggleButton));

	m_pIsCheckedProperty = CXTPMarkupDependencyProperty::Register(
		L"IsChecked", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupToggleButton),
		new CXTPMarkupPropertyMetadata(CXTPMarkupEnum::CreateValue(0),
									   &CXTPMarkupToggleButton::ConvertIsChecked,
									   &CXTPMarkupToggleButton::OnIsCheckedChanged,
									   CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pIsThreeStateProperty = CXTPMarkupDependencyProperty::Register(
		L"IsThreeState", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupToggleButton),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateFalseValue(),
									   CXTPMarkupPropertyMetadata::flagAffectsRender));
}

CXTPMarkupObject* CXTPMarkupToggleButton::ConvertIsChecked(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		if (_wcsicmp(*((CXTPMarkupString*)pObject), L"True") == 0)
		{
			return CXTPMarkupEnum::CreateValue(BST_CHECKED);
		}

		if (_wcsicmp(*((CXTPMarkupString*)pObject), L"False") == 0)
		{
			return CXTPMarkupEnum::CreateValue(BST_UNCHECKED);
		}

		if (_wcsicmp(*((CXTPMarkupString*)pObject), L"") == 0)
		{
			return CXTPMarkupEnum::CreateValue(BST_INDETERMINATE);
		}
	}

	return NULL;
}

void CXTPMarkupToggleButton::OnClick()
{
	OnToggle();
	CXTPMarkupButtonBase::OnClick();
}

void CXTPMarkupToggleButton::OnToggle()
{
	if (GetChecked() == BST_CHECKED)
	{
		SetChecked(IsThreeState() ? BST_INDETERMINATE : BST_UNCHECKED);
	}
	else
	{
		SetChecked(GetChecked() == BST_INDETERMINATE ? BST_UNCHECKED : BST_CHECKED);
	}
}

void CXTPMarkupToggleButton::OnChecked(CXTPMarkupRoutedEventArgs* e)
{
	RaiseEvent(e);
}

void CXTPMarkupToggleButton::OnUnchecked(CXTPMarkupRoutedEventArgs* e)
{
	RaiseEvent(e);
}

void CXTPMarkupToggleButton::OnIndeterminate(CXTPMarkupRoutedEventArgs* e)
{
	RaiseEvent(e);
}

void CXTPMarkupToggleButton::OnIsCheckedChanged(CXTPMarkupObject* d,
												CXTPMarkupPropertyChangedEventArgs* e)
{
	CXTPMarkupToggleButton* pButton = MARKUP_DYNAMICCAST(CXTPMarkupToggleButton, d);
	if (!pButton)
		return;

	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum, e->m_pNewValue);

	int bValue = pValue != NULL ? (int)*pValue : FALSE;

	if (bValue == BST_CHECKED)
	{
		CXTPMarkupRoutedEventArgs args(CXTPMarkupToggleButton::m_pCheckedEvent, pButton);
		pButton->OnChecked(&args);
	}
	else if (bValue == BST_UNCHECKED)
	{
		CXTPMarkupRoutedEventArgs args(CXTPMarkupToggleButton::m_pUncheckedEvent, pButton);
		pButton->OnUnchecked(&args);
	}
	else
	{
		CXTPMarkupRoutedEventArgs args(CXTPMarkupToggleButton::m_pIndeterminateEvent, pButton);
		pButton->OnIndeterminate(&args);
	}
}

BOOL CXTPMarkupToggleButton::GetChecked() const
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pIsCheckedProperty));
	return pValue != NULL ? (BOOL)(int)*pValue : FALSE;
}

BOOL CXTPMarkupToggleButton::IsThreeState() const
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool, GetValue(m_pIsThreeStateProperty));
	return pValue != NULL ? (BOOL)*pValue : FALSE;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupToggleButton, CXTPMarkupButtonBase)
	DISP_PROPERTY_EX_ID(CXTPMarkupToggleButton, "IsChecked", 800, OleGetIsChecked, OleSetIsChecked,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupToggleButton, "IsThreeState", 801, OleGetIsThreeState,
						OleSetIsThreeState, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupToggleButton, "CheckedEvent", 802, OleGetCheckedEvent,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupToggleButton, "IndeterminateEvent", 803, OleGetIndeterminateEvent,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupToggleButton, "UncheckedEvent", 804, OleGetUncheckedEvent,
						SetNotSupported, VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupToggleButton, CXTPMarkupButtonBase)
	INTERFACE_PART(CXTPMarkupToggleButton, XTPDIID_MarkupToggleButton, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupToggleButton, XTPDIID_MarkupToggleButton)
#endif

long CXTPMarkupToggleButton::OleGetIsChecked()
{
	return GetChecked();
}

void CXTPMarkupToggleButton::OleSetIsChecked(long bNewValue)
{
	SetChecked(bNewValue);
}

BOOL CXTPMarkupToggleButton::OleGetIsThreeState()
{
	return IsThreeState();
}

void CXTPMarkupToggleButton::OleSetIsThreeState(BOOL bNewValue)
{
	SetThreeState(bNewValue);
}

LPDISPATCH CXTPMarkupToggleButton::OleGetCheckedEvent()
{
	return XTPGetDispatch(m_pCheckedEvent);
}

LPDISPATCH CXTPMarkupToggleButton::OleGetIndeterminateEvent()
{
	return XTPGetDispatch(m_pIndeterminateEvent);
}

LPDISPATCH CXTPMarkupToggleButton::OleGetUncheckedEvent()
{
	return XTPGetDispatch(m_pUncheckedEvent);
}
