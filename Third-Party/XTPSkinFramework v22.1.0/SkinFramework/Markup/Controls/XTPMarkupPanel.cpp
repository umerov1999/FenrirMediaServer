// XTPMarkupPanel.cpp: implementation of the CXTPMarkupPanel class.
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
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupUIElementCollection.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXTPMarkupDependencyProperty* CXTPMarkupPanel::m_pBackgroundProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"Panel", CXTPMarkupPanel, CXTPMarkupFrameworkElement)

void CXTPMarkupPanel::RegisterMarkupClass()
{
	m_pBackgroundProperty = CXTPMarkupDependencyProperty::Register(
		L"Background", MARKUP_TYPE(CXTPMarkupBrush), MARKUP_TYPE(CXTPMarkupPanel),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsRender));
}

CXTPMarkupPanel::CXTPMarkupPanel()
{
	m_pChildren = new CXTPMarkupUIElementCollection();
	m_pChildren->SetLogicalParent(this);
}

CXTPMarkupPanel::~CXTPMarkupPanel()
{
	if (m_pChildren)
	{
		m_pChildren->SetLogicalParent(NULL);
		m_pChildren->Release();
	}
}

void CXTPMarkupPanel::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	m_pChildren->SetContentObject(pBuilder, pContent);
}

BOOL CXTPMarkupPanel::HasContentObject() const
{
	return m_pChildren->HasContentObject();
}

CXTPMarkupBrush* CXTPMarkupPanel::GetBackground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pBackgroundProperty));
}

void CXTPMarkupPanel::SetBackground(CXTPMarkupBrush* brush)
{
	SetValue(m_pBackgroundProperty, brush);
}

void CXTPMarkupPanel::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CSize szRender = GetRenderSize();
	if (szRender.cx <= 0 || szRender.cy <= 0)
		return;

	CXTPMarkupBrush* pBackground = GetBackground();

	if (pBackground)
	{
		pDC->FillRectangle(CRect(0, 0, szRender.cx, szRender.cy), pBackground);
	}
}

CXTPMarkupInputElement* CXTPMarkupPanel::InputHitTestOverride(CPoint /*point*/) const
{
	return GetBackground() != NULL ? (CXTPMarkupInputElement*)this : NULL;
}

int CXTPMarkupPanel::GetVisualChildrenCount() const
{
	return m_pChildren->GetCount();
}

CXTPMarkupVisual* CXTPMarkupPanel::GetVisualChild(int nIndex) const
{
	return m_pChildren->GetItem(nIndex);
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupUIElementCollection, CXTPMarkupObject)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "Count", 1, OleGetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "Item", DISPID_VALUE, OleGetItem, VT_DISPATCH,
					 VTS_I4)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "_NewEnum", DISPID_NEWENUM, OleNewEnum,
					 VT_UNKNOWN, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "Add", 2, OleAdd, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "Insert", 3, OleInsert, VT_EMPTY,
					 VTS_I4 VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "Clear", 4, RemoveAll, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupUIElementCollection, "RemoveAt", 5, Remove, VT_EMPTY, VTS_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupUIElementCollection, CXTPMarkupObject)
	INTERFACE_PART(CXTPMarkupUIElementCollection, XTPDIID_MarkupUIElementCollection, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupUIElementCollection, XTPDIID_MarkupUIElementCollection)
#endif

IMPLEMENT_ENUM_VARIANT(CXTPMarkupUIElementCollection)

long CXTPMarkupUIElementCollection::OleGetItemCount()
{
	return (long)GetCount();
}

LPDISPATCH CXTPMarkupUIElementCollection::OleGetItem(long nIndex)
{
	return XTPGetDispatch(GetItem(nIndex));
}

void CXTPMarkupUIElementCollection::OleAdd(LPDISPATCH lpElementDisp)
{
	if (!lpElementDisp)
		AfxThrowOleException(E_INVALIDARG);

	Add(MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpElementDisp)));
}

void CXTPMarkupUIElementCollection::OleInsert(long Index, LPDISPATCH lpElementDisp)
{
	if (!lpElementDisp)
		AfxThrowOleException(E_INVALIDARG);

	Insert(Index, MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpElementDisp)));
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupPanel, CXTPMarkupFrameworkElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupPanel, "Background", 500, OleGetBackground, OleSetBackground,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupPanel, "Children", 501, OleGetChildren, SetNotSupported,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupPanel, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupPanel, XTPDIID_MarkupPanel, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupPanel, XTPDIID_MarkupPanel)
#endif

LPDISPATCH CXTPMarkupPanel::OleGetChildren()
{
	return XTPGetDispatch(GetChildren());
}

LPDISPATCH CXTPMarkupPanel::OleGetBackground()
{
	return XTPGetDispatch(GetBackground());
}

void CXTPMarkupPanel::OleSetBackground(LPDISPATCH lpBackground)
{
	SetBackground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpBackground)));
}
