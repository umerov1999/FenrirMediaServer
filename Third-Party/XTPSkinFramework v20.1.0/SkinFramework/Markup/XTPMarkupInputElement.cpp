// XTPMarkupContentElement.cpp: implementation of the CXTPMarkupInputElement class.
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

#include "stdafx.h"

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

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupStyle.h"
#include "Markup/XTPMarkupSetter.h"
#include "Markup/XTPMarkupSetterCollection.h"
#include "Markup/XTPMarkupTrigger.h"
#include "Markup/XTPMarkupTriggerCollection.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupResourceDictionary.h"
#include "Markup/XTPMarkupKeyboardNavigation.h"
#include "Markup/XTPMarkupDelegate.h"
#include "Markup/XTPMarkupRoutedEvent.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"
#include "Markup/XTPMarkupEventHandlerMap.h"
#include "Markup/XTPMarkupQueryCursorEventArgs.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/Transform/XTPMarkupTransformationMatrix.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

class CXTPMarkupDeviceContextOpacityScope
{
	double m_dOldOpacity;
	CXTPMarkupDrawingContext* m_pDC;

public:
	explicit CXTPMarkupDeviceContextOpacityScope(CXTPMarkupDrawingContext* pDC, double dOpacity)
		: m_dOldOpacity(pDC->GetOpacity())
		, m_pDC(pDC)
	{
		ASSERT(NULL != m_pDC);
		m_pDC->SetOpacity(dOpacity);
	}

	~CXTPMarkupDeviceContextOpacityScope()
	{
		m_pDC->SetOpacity(m_dOldOpacity);
	}
};

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupInputElement

CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pCursorProperty			   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pIsMouseOverProperty	   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pIsKeyboardFocusedProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pResourcesProperty		   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pStyleProperty			   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pIsFocusableProperty	   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupInputElement::m_pToolTipProperty		   = NULL;

CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseLeaveEvent			= NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseEnterEvent			= NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseLeftButtonUpEvent	= NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseLeftButtonDownEvent	= NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseRightButtonUpEvent	= NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseRightButtonDownEvent = NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pMouseMoveEvent			= NULL;
CXTPMarkupRoutedEvent* CXTPMarkupInputElement::m_pLostMouseCaptureEvent		= NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupInputElement, CXTPMarkupObject);

void CXTPMarkupInputElement::RegisterMarkupClass()
{
	CXTPMarkupStyle::RegisterType();

	m_pCursorProperty = CXTPMarkupDependencyProperty::Register(
		L"Cursor", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupInputElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertCursor,
									   &CXTPMarkupInputElement::OnCursorPropertyChanged));

	m_pIsMouseOverProperty = CXTPMarkupDependencyProperty::Register(
		L"IsMouseOver", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupInputElement),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateFalseValue()));

	m_pResourcesProperty = CXTPMarkupStyle::m_pResourcesProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupInputElement));

	m_pStyleProperty = CXTPMarkupDependencyProperty::Register(
		L"Style", MARKUP_TYPE(CXTPMarkupStyle), MARKUP_TYPE(CXTPMarkupInputElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupInputElement::OnStylePropertyChanged));

	m_pIsKeyboardFocusedProperty = CXTPMarkupDependencyProperty::Register(
		L"IsKeyboardFocused", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupInputElement),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateFalseValue(),
									   CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pIsFocusableProperty = CXTPMarkupDependencyProperty::Register(
		L"IsFocusable", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupInputElement),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateFalseValue()));

	m_pToolTipProperty = CXTPMarkupDependencyProperty::Register(
		L"ToolTip", MARKUP_TYPE(CXTPMarkupObject), MARKUP_TYPE(CXTPMarkupInputElement));

	m_pMouseLeaveEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseLeave", CXTPMarkupRoutedEvent::routingDirect, MARKUP_TYPE(CXTPMarkupInputElement));
	m_pMouseEnterEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseEnter", CXTPMarkupRoutedEvent::routingDirect, MARKUP_TYPE(CXTPMarkupInputElement));
	m_pMouseLeftButtonUpEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseLeftButtonUp", CXTPMarkupRoutedEvent::routingDirect,
		MARKUP_TYPE(CXTPMarkupInputElement));
	m_pMouseLeftButtonDownEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseLeftButtonDown", CXTPMarkupRoutedEvent::routingDirect,
		MARKUP_TYPE(CXTPMarkupInputElement));
	m_pMouseRightButtonUpEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseRightButtonUp", CXTPMarkupRoutedEvent::routingDirect,
		MARKUP_TYPE(CXTPMarkupInputElement));
	m_pMouseRightButtonDownEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseRightButtonDown", CXTPMarkupRoutedEvent::routingDirect,
		MARKUP_TYPE(CXTPMarkupInputElement));
	m_pMouseMoveEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"MouseMove", CXTPMarkupRoutedEvent::routingDirect, MARKUP_TYPE(CXTPMarkupInputElement));
	m_pLostMouseCaptureEvent = CXTPMarkupRoutedEvent::RegisterRoutedEvent(
		L"LostMouseCapture", CXTPMarkupRoutedEvent::routingDirect,
		MARKUP_TYPE(CXTPMarkupInputElement));
}

CXTPMarkupInputElement::CXTPMarkupInputElement()
{
	m_pMarkupContext = NULL;
	m_pHandlers		 = NULL;

	m_pActiveTriggers	 = NULL;
	m_pTriggerProperties = NULL;
	m_pStyleCache		 = NULL;
}

CXTPMarkupInputElement::~CXTPMarkupInputElement()
{
	SAFE_DELETE(m_pHandlers);

	MARKUP_RELEASE(m_pActiveTriggers);
	MARKUP_RELEASE(m_pTriggerProperties);
}

void CXTPMarkupInputElement::OnMouseLeave(CXTPMarkupMouseEventArgs* /*e*/)
{
	SetValue(m_pIsMouseOverProperty, NULL);
}

void CXTPMarkupInputElement::OnMouseEnter(CXTPMarkupMouseEventArgs* /*e*/)
{
	SetValue(m_pIsMouseOverProperty, CXTPMarkupBool::CreateTrueValue());
}

void CXTPMarkupInputElement::OnMouseMove(CXTPMarkupMouseEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::OnQueryCursor(CXTPMarkupQueryCursorEventArgs* e)
{
	CXTPMarkupInt* pCursor = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pCursorProperty));
	if (pCursor)
	{
		e->SetHandled();
		e->m_hCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE((int)*pCursor));
	}
}

void CXTPMarkupInputElement::OnMouseWheel(CXTPMarkupMouseWheelEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::OnMouseRightButtonUp(CXTPMarkupMouseButtonEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::OnMouseRightButtonDown(CXTPMarkupMouseButtonEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::OnLostMouseCapture(CXTPMarkupMouseEventArgs* /*e*/)
{
}

void CXTPMarkupInputElement::SetToolTip(LPCTSTR lpszTooltip)
{
	SetValue(m_pToolTipProperty, new CXTPMarkupString(lpszTooltip));
}

void CXTPMarkupInputElement::RaiseEvent(CXTPMarkupRoutedEventArgs* pEventArgs)
{
	if (pEventArgs->GetEvent() == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	if (pEventArgs->GetEvent()->GetFlags() & CXTPMarkupRoutedEvent::routingBubble)
	{
		CXTPMarkupObject* pParent = this;
		while (pParent)
		{
			if (pParent->IsKindOf(MARKUP_TYPE(CXTPMarkupInputElement)))
			{
				CXTPMarkupEventHandlerMap* pHandlers =
					((CXTPMarkupInputElement*)pParent)->m_pHandlers;
				if (pHandlers)
					pHandlers->Raise(pParent, pEventArgs);

				if (pEventArgs->IsHandled())
					return;
			}

			pParent = pParent->GetLogicalParent();
		}

		CXTPMarkupContext* pMarkupContext = GetMarkupContext();
		if (pMarkupContext)
		{
			pMarkupContext->RaiseEvent(this, pEventArgs);
		}
	}
	else
	{
		if (m_pHandlers)
			m_pHandlers->Raise(this, pEventArgs);
	}
}

void CXTPMarkupInputElement::AddHandler(CXTPMarkupRoutedEvent* pEvent,
										CXTPMarkupDelegate* pDelegate)
{
	if (m_pHandlers == NULL)
		m_pHandlers = new CXTPMarkupEventHandlerMap();

	m_pHandlers->Add(pEvent, pDelegate);
}

CXTPMarkupInputElement* CXTPMarkupInputElement::InputHitTest(CPoint /*point*/) const
{
	return NULL;
}

void CXTPMarkupInputElement::SetPropertyObject(CXTPMarkupBuilder* pBuilder,
											   CXTPMarkupDependencyProperty* pProperty,
											   CXTPMarkupObject* pValue)
{
	if (pProperty->IsEvent())
	{
		CXTPMarkupContext* pMarkupContext = GetMarkupContext();
		if (!pMarkupContext)
		{
			pBuilder->ThrowBuilderException(_T("Markup Context is NULL"));
		}

		LPCWSTR lpszDelegate = (LPCWSTR) * ((CXTPMarkupString*)pValue);

		CXTPMarkupDelegate* pDelegate = pMarkupContext->LookupDelegate(lpszDelegate);
		if (!pDelegate)
		{
			pBuilder->ThrowBuilderException(
				CXTPMarkupBuilder::FormatString(_T("'%ls' delegate not found"),
												(LPCTSTR)lpszDelegate));
		}

		AddHandler((CXTPMarkupRoutedEvent*)pProperty, pDelegate);
		MARKUP_ADDREF(pDelegate);
	}
	else
	{
		CXTPMarkupObject::SetPropertyObject(pBuilder, pProperty, pValue);
	}
}

void CXTPMarkupInputElement::OnFinalRelease()
{
	if (NULL != m_pMarkupContext)
	{
		m_pMarkupContext->FinalizeMarkupObject(this);
	}

	CXTPMarkupObject::OnFinalRelease();
}

void CXTPMarkupInputElement::CaptureMouse()
{
	m_pMarkupContext->CaptureMouse(this);
}

void CXTPMarkupInputElement::ReleaseMouseCapture()
{
	m_pMarkupContext->ReleaseMouseCapture(this);
}

void CXTPMarkupInputElement::ApplyTree(CXTPMarkupObject* pChild)
{
	int nCount = pChild->GetLogicalChildrenCount();

	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupObject* p = pChild->GetLogicalChild(i);

		ApplyTree(p);
	}

	if (pChild->IsKindOf(MARKUP_TYPE(CXTPMarkupInputElement)))
	{
		((CXTPMarkupInputElement*)pChild)->OnApply();
	}
}

void CXTPMarkupInputElement::Apply()
{
	ApplyTree(this);
}

CXTPMarkupObject* CXTPMarkupInputElement::FindResource(const CXTPMarkupObject* pKey) const
{
	return CXTPMarkupResourceDictionary::FindResource(this, pKey);
}

void CXTPMarkupInputElement::OnApply()
{
	if (m_pStyleCache == NULL)
	{
		CXTPMarkupType* pType = GetType();
		if (!pType)
			return;

		CXTPMarkupStyle* pStyle = MARKUP_DYNAMICCAST(CXTPMarkupStyle, FindResource(pType));
		if (pStyle)
		{
			pStyle->Seal();
			m_pStyleCache = pStyle;
		}
	}

	if (m_pStyleCache != NULL)
	{
		FireTriggers(NULL, NULL);
	}
}

class CXTPMarkupActiveTriggerCollection : public CXTPMarkupTriggerCollection
{
public:
	CXTPMarkupActiveTriggerCollection()
	{
		m_bLogicalParent = FALSE;
	}
};

BOOL CXTPMarkupInputElement::AddStyleTriggers(CXTPMarkupStyle* pStyle,
											  CXTPMarkupDependencyProperty* pProperty,
											  CXTPMarkupObject* pNewValue)
{
	BOOL bChanged = FALSE;

	CXTPMarkupStyle* pBasedStyle = pStyle->GetBasedStyle();
	if (pBasedStyle)
	{
		bChanged = AddStyleTriggers(pBasedStyle, pProperty, pNewValue);
	}

	CXTPMarkupTriggerCollection* pTriggers = pStyle->GetTriggers();
	if (!pTriggers)
		return bChanged;

	BOOL bAllProperties = (pProperty == NULL);

	for (int i = pTriggers->GetCount() - 1; i >= 0; i--)
	{
		CXTPMarkupTrigger* pTrigger = pTriggers->GetItem(i);

		if (bAllProperties)
		{
			pProperty = pTrigger->GetTriggerProperty();
			pNewValue = GetValue(pProperty);
		}

		if (pTrigger->GetTriggerProperty() == pProperty
			&& pTrigger->GetTriggerValue()->IsEqual(pNewValue))
		{
			if (!m_pActiveTriggers)
			{
				m_pActiveTriggers = new CXTPMarkupActiveTriggerCollection();
			}

			pTrigger->AddRef();
			m_pActiveTriggers->Add(pTrigger);
			bChanged = TRUE;
		}
	}

	return bChanged;
}

void CXTPMarkupInputElement::FireTriggers(CXTPMarkupDependencyProperty* pProperty,
										  CXTPMarkupObject* pNewValue)
{
	BOOL bChanged = FALSE;
	int i;

	CMap<CXTPMarkupDependencyProperty*, CXTPMarkupDependencyProperty*, CXTPMarkupObject*,
		 CXTPMarkupObject*>
		mapOldValues;

	if (m_pActiveTriggers)
	{
		for (i = m_pActiveTriggers->GetCount() - 1; i >= 0; i--)
		{
			CXTPMarkupTrigger* pTrigger = m_pActiveTriggers->GetItem(i);

			if (pTrigger->GetTriggerProperty() == pProperty)
			{
				if (!pTrigger->GetTriggerValue()->IsEqual(pNewValue))
				{
					CXTPMarkupSetterCollection* pSetters = pTrigger->GetSetters();
					if (NULL == pSetters)
					{
						continue;
					}

					for (int j = 0; j < pSetters->GetCount(); j++)
					{
						CXTPMarkupSetter* pSetter					  = pSetters->GetItem(j);
						CXTPMarkupDependencyProperty* pSetterProperty = pSetter->GetSetterProperty();

						CXTPMarkupObject* pOldValue = m_pTriggerProperties->Lookup(pSetterProperty);

						CXTPMarkupObject* pTempValue;
						if (!mapOldValues.Lookup(pSetterProperty, pTempValue))
						{
							MARKUP_ADDREF(pOldValue);
							mapOldValues.SetAt(pSetterProperty, pOldValue);
						}

						m_pTriggerProperties->Set(pSetterProperty, NULL);
					}

					m_pActiveTriggers->Remove(i);
					bChanged = TRUE;
				}
			}
		}
	}

	if (m_pStyleCache)
	{
		bChanged = AddStyleTriggers(m_pStyleCache, pProperty, pNewValue) || bChanged;
	}
	else if (GetType()->GetTypeStyle())
	{
		bChanged = AddStyleTriggers(GetType()->GetTypeStyle(), pProperty, pNewValue) || bChanged;
	}

	if (bChanged)
	{
		if (!m_pTriggerProperties)
			m_pTriggerProperties = new CXTPMarkupProperties(NULL);

		for (i = 0; i < m_pActiveTriggers->GetCount(); i++)
		{
			CXTPMarkupTrigger* pTrigger = m_pActiveTriggers->GetItem(i);

			CXTPMarkupSetterCollection* pSetters = pTrigger->GetSetters();
			if (NULL == pSetters)
			{
				continue;
			}

			for (int j = 0; j < pSetters->GetCount(); j++)
			{
				CXTPMarkupSetter* pSetter = pSetters->GetItem(j);

				CXTPMarkupObject* pValue = pSetter->GetSetterValue();

				CXTPMarkupDependencyProperty* pSetterProperty = pSetter->GetSetterProperty();

				CXTPMarkupObject* pOldValue = m_pTriggerProperties->Lookup(pSetterProperty);

				if (!pValue->IsEqual(pOldValue))
				{
					CXTPMarkupObject* pTempValue;
					if (!mapOldValues.Lookup(pSetterProperty, pTempValue))
					{
						MARKUP_ADDREF(pOldValue);
						mapOldValues.SetAt(pSetterProperty, pOldValue);
					}

					pValue->AddRef();
					m_pTriggerProperties->Set(pSetterProperty, pValue);
				}
			}
		}
	}

	POSITION pos = mapOldValues.GetStartPosition();
	while (pos)
	{
		CXTPMarkupDependencyProperty* pProp;
		CXTPMarkupObject* pOldValue;
		mapOldValues.GetNextAssoc(pos, pProp, pOldValue);

		CXTPMarkupObject* pNewVal = m_pTriggerProperties->Lookup(pProp);

		if (!::IsEqual(pNewVal, pOldValue))
		{
			OnPropertyChanged(pProp, pOldValue, pNewVal);

			if (pProp->GetFlags() & CXTPMarkupPropertyMetadata::flagInherited)
			{
				RecursePropertyChanged(pProp, pOldValue, pNewVal);
			}
		}

		MARKUP_RELEASE(pOldValue);
	}
}

void CXTPMarkupInputElement::OnCursorPropertyChanged(CXTPMarkupObject* d,
													 CXTPMarkupPropertyChangedEventArgs* /*e*/)
{
	CXTPMarkupInputElement* pElement = MARKUP_DYNAMICCAST(CXTPMarkupInputElement, d);
	if (!pElement)
	{
		ASSERT(FALSE);
		return;
	}

	pElement->GetMarkupContext()->HandleSetCursor();
}

void CXTPMarkupInputElement::OnStylePropertyChanged(CXTPMarkupObject* d,
													CXTPMarkupPropertyChangedEventArgs* e)
{
	CXTPMarkupInputElement* pElement = MARKUP_DYNAMICCAST(CXTPMarkupInputElement, d);
	if (!pElement)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPMarkupStyle* pStyle = MARKUP_STATICCAST(CXTPMarkupStyle, e->m_pNewValue);

	if (pStyle)
	{
		pStyle->Seal();
	}

	pElement->m_pStyleCache = pStyle;
}

CXTPMarkupObject* CXTPMarkupInputElement::GetValueCore(CXTPMarkupDependencyProperty* pProperty) const
{
	CXTPMarkupObject* pValue = CXTPMarkupObject::GetValueCore(pProperty);
	if (pValue)
		return pValue;

	if (m_pTriggerProperties)
	{
		pValue = m_pTriggerProperties->Lookup(pProperty);
		if (pValue)
			return pValue;
	}

	if (m_pStyleCache)
	{
		pValue = m_pStyleCache->GetStyleValue(pProperty);
		if (pValue)
			return pValue;
	}

	CXTPMarkupStyle* pTypeStyle = GetType()->GetTypeStyle();
	if (pTypeStyle)
		return pTypeStyle->GetStyleValue(pProperty);

	return NULL;
}

void CXTPMarkupInputElement::Focus()
{
	if (!m_pMarkupContext)
		return;

	m_pMarkupContext->GetKeyboardNavigation()->Focus(this);
}

CXTPMarkupString* CXTPMarkupInputElement::GetName() const
{
	return MARKUP_STATICCAST(CXTPMarkupString, GetValue(m_pNameProperty));
}

void CXTPMarkupInputElement::SetName(LPCTSTR lpszName)
{
	SetValue(m_pNameProperty, new CXTPMarkupString(lpszName));
}

BOOL CXTPMarkupInputElement::IsMouseOver() const
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool, GetValue(m_pIsMouseOverProperty));
	return pValue ? (BOOL)*pValue : FALSE;
}

BOOL CXTPMarkupInputElement::IsKeyboardFocused() const
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool,
											   GetValue(m_pIsKeyboardFocusedProperty));
	return pValue ? (BOOL)*pValue : FALSE;
}

CXTPMarkupObject* CXTPMarkupInputElement::GetToolTip() const
{
	return GetValue(m_pToolTipProperty);
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupInputElement, CXTPMarkupObject)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "Cursor", 100, OleGetCursor, OleSetCursor, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "ToolTip", 101, OleGetToolTip, OleSetToolTip,
						VT_VARIANT)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "IsMouseOver", 102, OleGetIsMouseOver,
						SetNotSupported, VT_BOOL)
#ifdef _XTP_ACTIVEX
	DISP_FUNCTION_ID(CXTPMarkupInputElement, "AddHandler", 103, OleAddHandler, VT_EMPTY,
					 VTS_DISPATCH VTS_WBSTR)
#endif

	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseLeaveEvent", 104, OleGetMouseLeaveEvent,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseEnterEvent", 105, OleGetMouseEnterEvent,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseLeftButtonUpEvent", 106,
						OleGetMouseLeftButtonUpEvent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseLeftButtonDownEvent", 107,
						OleGetMouseLeftButtonDownEvent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseRightButtonUpEvent", 108,
						OleGetMouseRightButtonUpEvent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseRightButtonDownEvent", 109,
						OleGetMouseRightButtonDownEvent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "MouseMoveEvent", 110, OleGetMouseMoveEvent,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "LostMouseCaptureEvent", 111,
						OleGetLostMouseCaptureEvent, SetNotSupported, VT_DISPATCH)

	DISP_FUNCTION_ID(CXTPMarkupInputElement, "FindName", 112, OleFindName, VT_DISPATCH, VTS_WBSTR)
	DISP_PROPERTY_EX_ID(CXTPMarkupInputElement, "Name", 113, OleGetName, OleSetName, VT_BSTR)

	DISP_FUNCTION_ID(CXTPMarkupInputElement, "CaptureMouse", 114, CaptureMouse, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupInputElement, "ReleaseMouseCapture", 115, ReleaseMouseCapture,
					 VT_EMPTY, VTS_NONE)

END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupInputElement, CXTPMarkupObject)
	INTERFACE_PART(CXTPMarkupInputElement, XTPDIID_MarkupInputElement, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupInputElement, XTPDIID_MarkupInputElement)
#endif

LPDISPATCH CXTPMarkupInputElement::OleFindName(LPCOLESTR lpszName)
{
	return XTPGetDispatch(FindName(lpszName));
}

BSTR CXTPMarkupInputElement::OleGetName()
{
	CXTPMarkupString* pName = GetName();
	if (pName)
		return SysAllocString(*pName);

	return NULL;
}

void CXTPMarkupInputElement::OleSetName(LPCTSTR lpszName)
{
	SetName(lpszName);
}

long CXTPMarkupInputElement::OleGetCursor()
{
	CXTPMarkupInt* pCursor = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pCursorProperty));
	return pCursor ? (int)*pCursor : 0;
}

void CXTPMarkupInputElement::OleSetCursor(long nValue)
{
	SetValue(m_pCursorProperty, new CXTPMarkupInt(nValue));
}

VARIANT CXTPMarkupInputElement::OleGetToolTip()
{
	VARIANT va;
	VariantInit(&va);
	va.vt = VT_NULL;

	CXTPMarkupObject* pTag = GetToolTip();
	if (!pTag)
		return va;

	if (IsStringObject(pTag))
	{
		va.vt	   = VT_BSTR;
		va.bstrVal = SysAllocString(*(CXTPMarkupString*)pTag);
	}
	else
	{
		va.vt		= VT_DISPATCH;
		va.pdispVal = XTPGetDispatch(pTag);
	}
	return va;
}

void CXTPMarkupInputElement::OleSetToolTip(const VARIANT& va)
{
	if (va.vt == VT_BSTR)
	{
		SetValue(m_pToolTipProperty, new CXTPMarkupString(va.bstrVal));
	}
}

BOOL CXTPMarkupInputElement::OleGetIsMouseOver()
{
	return IsMouseOver();
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseLeaveEvent()
{
	return XTPGetDispatch(m_pMouseLeaveEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseEnterEvent()
{
	return XTPGetDispatch(m_pMouseEnterEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseLeftButtonUpEvent()
{
	return XTPGetDispatch(m_pMouseLeftButtonUpEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseLeftButtonDownEvent()
{
	return XTPGetDispatch(m_pMouseLeftButtonDownEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseRightButtonUpEvent()
{
	return XTPGetDispatch(m_pMouseRightButtonUpEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseRightButtonDownEvent()
{
	return XTPGetDispatch(m_pMouseRightButtonDownEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetMouseMoveEvent()
{
	return XTPGetDispatch(m_pMouseMoveEvent);
}

LPDISPATCH CXTPMarkupInputElement::OleGetLostMouseCaptureEvent()
{
	return XTPGetDispatch(m_pLostMouseCaptureEvent);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRoutedEvent

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupRoutedEvent, CXTPMarkupDependencyProperty)
	DISP_PROPERTY_EX_ID(CXTPMarkupRoutedEvent, "Name", 100, OleGetName, SetNotSupported, VT_BSTR)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupRoutedEvent, CXTPMarkupDependencyProperty)
	INTERFACE_PART(CXTPMarkupRoutedEvent, XTPDIID_MarkupRoutedEvent, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupRoutedEvent, XTPDIID_MarkupRoutedEvent)
#endif

BSTR CXTPMarkupRoutedEvent::OleGetName()
{
	return SysAllocString(GetName());
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupVisual

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupVisual, CXTPMarkupInputElement);

void CXTPMarkupVisual::RegisterMarkupClass()
{
}

CXTPMarkupVisual::CXTPMarkupVisual()
{
	m_ptVisualOffset			   = CPoint(0, 0);
	m_szRenderSize				   = 0;
	m_pCurrentTransformationMatrix = NULL;

	m_rcBoundRect.SetRectEmpty();
	m_rcUpdateRect.SetRectEmpty();
	m_rcPrevUpdateRect.SetRectEmpty();
}

CXTPMarkupVisual::~CXTPMarkupVisual()
{
	SetCurrentTransformationMatrix(NULL);
}

BOOL CXTPMarkupVisual::GetLayoutClip(CRect& /*rc*/) const
{
	return FALSE;
}

void CXTPMarkupVisual::RenderClipped(CXTPMarkupDrawingContext* drawingContext)
{
	int nVisualCount = GetVisualChildrenCount();
	for (int i = 0; i < nVisualCount; i++)
	{
		GetVisualChild(i)->RenderClipped(drawingContext);
	}
}

double CXTPMarkupVisual::GetOpacity() const
{
	return 1.;
}

void CXTPMarkupVisual::Render(CXTPMarkupDrawingContext* drawingContext)
{
	CXTPMarkupDeviceContextOpacityScope scopeOpacity(drawingContext, GetOpacity());

	m_rcUpdateRect.SetRectEmpty();

	CRect rcClipBox = drawingContext->GetClipBox();
	rcClipBox.OffsetRect(-m_ptVisualOffset);

	RECT rcIntersect;
	if (!::IntersectRect(&rcIntersect, GetBoundRect(), &rcClipBox))
	{
		RenderClipped(drawingContext);
		return;
	}

	CRect rcLayoutClip(0, 0, 0, 0);
	BOOL bClipLayout = FALSE;

	if (GetLayoutClip(rcLayoutClip))
	{
		if (rcLayoutClip.IsRectEmpty())
		{
			RenderClipped(drawingContext);
			return;
		}

		rcClipBox.IntersectRect(rcClipBox, rcLayoutClip);

		if (!::IntersectRect(&rcIntersect, GetBoundRect(), &rcClipBox))
		{
			RenderClipped(drawingContext);
			return;
		}

		bClipLayout = TRUE;
	}

	drawingContext->OffsetViewport(m_ptVisualOffset);

	HRGN hrgnClip = 0;

	if (bClipLayout)
	{
		hrgnClip = drawingContext->SaveClipRegion();

		drawingContext->IntersectClipRect(rcLayoutClip);
	}

	TransformationMatrices matrices = ApplyRenderTransform(drawingContext);

	// Increase element rendering quality if transformation is applied.
	XTPMarkupSmoothingMode prevSmoothindMode = xtpMarkupSmoothingDefault;
	if (NULL != matrices.pNewMatrix)
	{
		prevSmoothindMode = drawingContext->SetSmoothingMode(xtpMarkupSmoothingAntiAlias);
	}

	// Render element and its children.
	OnRender(drawingContext);

	int nVisualCount = GetVisualChildrenCount();
	for (int i = 0; i < nVisualCount; i++)
	{
		GetVisualChild(i)->Render(drawingContext);
	}

	if (IsKeyboardFocused())
	{
		OnRenderFocusVisual(drawingContext);
	}

	// Restore rendering quality.
	if (NULL != matrices.pNewMatrix)
	{
		drawingContext->SetSmoothingMode(prevSmoothindMode);
	}

	RestoreRenderTransform(drawingContext, matrices);

	if (hrgnClip != NULL)
	{
		drawingContext->RestoreClipRegion(hrgnClip);
	}

	drawingContext->OffsetViewport(-m_ptVisualOffset);
}

int CXTPMarkupVisual::GetVisualChildrenCount() const
{
	return 0;
}

CXTPMarkupVisual* CXTPMarkupVisual::GetVisualChild(int /*nIndex*/) const
{
	ASSERT(FALSE);
	return NULL;
}

int CXTPMarkupVisual::GetLogicalChildrenCount() const
{
	return GetVisualChildrenCount();
}

CXTPMarkupObject* CXTPMarkupVisual::GetLogicalChild(int nIndex) const
{
	return GetVisualChild(nIndex);
}

CXTPMarkupInputElement* CXTPMarkupVisual::InputHitTest(CPoint point) const
{
	CPoint ptVisualPoint = point - m_ptVisualOffset;

	if (!PtInRectTransform(m_rcBoundRect, ptVisualPoint))
		return NULL;

	CRect rcClip;
	if (GetLayoutClip(rcClip))
	{
		if (!PtInRectTransform(rcClip, ptVisualPoint))
			return NULL;
	}

	int nVisualCount = GetVisualChildrenCount();
	for (int i = nVisualCount - 1; i >= 0; i--)
	{
		CXTPMarkupInputElement* pObject = GetVisualChild(i)->InputHitTest(ptVisualPoint);
		if (pObject)
			return pObject;
	}

	CRect rcRender(0, 0, m_szRenderSize.cx, m_szRenderSize.cy);
	if (!PtInRectTransform(rcRender, ptVisualPoint))
		return NULL;

	return InputHitTestOverride(ptVisualPoint);
}

void CXTPMarkupVisual::UpdateBoundRect()
{
	m_rcBoundRect.SetRect(0, 0, m_szRenderSize.cx, m_szRenderSize.cy);

	int nVisualCount = GetVisualChildrenCount();
	for (int i = 0; i < nVisualCount; i++)
	{
		CXTPMarkupVisual* pVisual = GetVisualChild(i);
		pVisual->UpdateBoundRect();

		CRect rcBoundRect = pVisual->m_rcBoundRect;
		rcBoundRect.OffsetRect(pVisual->m_ptVisualOffset);

		m_rcBoundRect.UnionRect(m_rcBoundRect, rcBoundRect);
	}
}

CXTPMarkupTransformationMatrix* CXTPMarkupVisual::GetAbsoluteTransformationMatrix() const
{
	CXTPMarkupTransformationMatrix* pMatrix = m_pCurrentTransformationMatrix;
	if (NULL == pMatrix)
	{
		CXTPMarkupObject* pParent = GetLogicalParent();
		while (NULL != pParent)
		{
			if (pParent->IsKindOf(MARKUP_TYPE(CXTPMarkupVisual)))
			{
				pMatrix =
					MARKUP_STATICCAST(CXTPMarkupVisual, pParent)->GetAbsoluteTransformationMatrix();
				if (NULL != pMatrix)
				{
					pMatrix->Offset(m_ptVisualOffset);
				}
				break;
			}
			else
			{
				pParent = pParent->GetLogicalParent();
			}
		}
	}
	else
	{
		pMatrix = pMatrix->Clone();
	}

	return pMatrix;
}

void CXTPMarkupVisual::SetCurrentTransformationMatrix(CXTPMarkupTransformationMatrix* pMatrix)
{
	if (NULL != m_pCurrentTransformationMatrix)
	{
		delete m_pCurrentTransformationMatrix;
	}

	m_pCurrentTransformationMatrix = pMatrix;
}

CRect CXTPMarkupVisual::GetUpdateRect() const
{
	CRect rcUpdate = m_rcUpdateRect;
	if (rcUpdate.IsRectEmpty())
	{
		rcUpdate								= GetBoundRect();
		CXTPMarkupTransformationMatrix* pMatrix = GetAbsoluteTransformationMatrix();
		if (NULL != pMatrix)
		{
			POINT ptAbsoluteVisualOffset = GetAbsoluteVisualOffset();
			rcUpdate					 = pMatrix->GetBoundRect(rcUpdate, ptAbsoluteVisualOffset);
			delete pMatrix;
		}
	}

	if (m_rcPrevUpdateRect != rcUpdate)
	{
		if (!m_rcPrevUpdateRect.IsRectEmpty())
		{
			RECT rcPrev												= m_rcPrevUpdateRect;
			const_cast<CXTPMarkupVisual*>(this)->m_rcPrevUpdateRect = rcUpdate;
			rcUpdate.UnionRect(&rcUpdate, &rcPrev);
		}
		else
		{
			const_cast<CXTPMarkupVisual*>(this)->m_rcPrevUpdateRect = rcUpdate;
		}
	}

	return rcUpdate;
}

BOOL CXTPMarkupVisual::PtInRectTransform(const RECT& rect, const POINT& point) const
{
	BOOL bHit = FALSE;

	CXTPMarkupTransformationMatrix* pMatrix = GetAbsoluteTransformationMatrix();
	if (NULL != pMatrix)
	{
		POINT ptAbsoluteVisualOffset = GetAbsoluteVisualOffset();
		bHit						 = pMatrix->PtInRect(rect, ptAbsoluteVisualOffset, point);
		delete pMatrix;
	}
	else
	{
		bHit = PtInRect(&rect, point);
	}

	return bHit;
}

POINT CXTPMarkupVisual::GetAbsoluteVisualOffset() const
{
	POINT offset = m_ptVisualOffset;

	CXTPMarkupObject* pParent = GetLogicalParent();
	while (NULL != pParent)
	{
		if (pParent->IsKindOf(MARKUP_TYPE(CXTPMarkupVisual)))
		{
			CXTPMarkupVisual* pParentVisual = MARKUP_STATICCAST(CXTPMarkupVisual, pParent);
			offset.x += pParentVisual->m_ptVisualOffset.x;
			offset.y += pParentVisual->m_ptVisualOffset.y;
		}

		pParent = pParent->GetLogicalParent();
	}

	return offset;
}

CXTPMarkupInputElement* CXTPMarkupVisual::InputHitTestOverride(CPoint /*point*/) const
{
	return (CXTPMarkupInputElement*)this;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupVisual, CXTPMarkupInputElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "VisualParent", 200, OleGetVisualParent,
						SetNotSupported, VT_DISPATCH)

	DISP_FUNCTION_ID(CXTPMarkupUIElement, "GetBoundingRect", 201, OleGetBoundingRect, VT_EMPTY,
					 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "ActualWidth", 202, OleGetActualWidth, SetNotSupported,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "ActualHeight", 203, OleGetActualHeight,
						SetNotSupported, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupVisual, CXTPMarkupInputElement)
	INTERFACE_PART(CXTPMarkupVisual, XTPDIID_MarkupVisual, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupVisual, XTPDIID_MarkupVisual)
#endif

LPDISPATCH CXTPMarkupVisual::OleGetVisualParent()
{
	return XTPGetDispatch(GetVisualParent());
}

void CXTPMarkupVisual::OleGetBoundingRect(int* left, int* top, int* right, int* bottom)
{
	CRect rc = GetMarkupContext()->GetClientBoundingRect(this);

	if (left)
		*left = rc.left;
	if (top)
		*top = rc.top;
	if (right)
		*right = rc.right;
	if (bottom)
		*bottom = rc.bottom;
}

int CXTPMarkupVisual::OleGetActualWidth()
{
	return m_szRenderSize.cx;
}

int CXTPMarkupVisual::OleGetActualHeight()
{
	return m_szRenderSize.cy;
}
