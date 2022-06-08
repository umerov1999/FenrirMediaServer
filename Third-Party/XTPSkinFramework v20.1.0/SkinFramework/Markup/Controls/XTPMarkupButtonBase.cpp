// XTPMarkupButtonBase.cpp : implementation file
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
#include "Common/XTPWinThemeWrapper.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupButtonBase.h"

#include "Markup/XTPMarkupRoutedEvent.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"
#include "Markup/XTPMarkupMouseButtonEventArgs.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupButtonBase

CXTPMarkupRoutedEvent* CXTPMarkupButtonBase::m_pClickEvent				 = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupButtonBase::m_pIsPressedProperty = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupButtonBase, CXTPMarkupContentControl)

void CXTPMarkupButtonBase::RegisterMarkupClass()
{
	m_pClickEvent		 = CXTPMarkupRoutedEvent::RegisterRoutedEvent(L"Click",
																  CXTPMarkupRoutedEvent::routingBubble,
																  MARKUP_TYPE(CXTPMarkupButtonBase));
	m_pIsPressedProperty = CXTPMarkupDependencyProperty::Register(
		L"IsPressed", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupButtonBase),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateFalseValue(),
									   CXTPMarkupPropertyMetadata::flagAffectsArrange));
}

CXTPMarkupButtonBase::CXTPMarkupButtonBase()
{
	m_bPushed	 = FALSE;
	m_bMouseOver = FALSE;

	m_themeButton = new CXTPWinThemeWrapper();

	SetValue(m_pFocusableProperty, CXTPMarkupBool::CreateTrueValue());
}

CXTPMarkupButtonBase::~CXTPMarkupButtonBase()
{
	SAFE_DELETE(m_themeButton);
}

void CXTPMarkupButtonBase::OnMouseLeave(CXTPMarkupMouseEventArgs* e)
{
	m_bMouseOver = FALSE;

	if (m_bPushed)
	{
		SetValue(m_pIsPressedProperty, NULL);
	}
	else
	{
		CXTPMarkupContentControl::OnMouseLeave(e);
		InvalidateVisual();
	}
}

void CXTPMarkupButtonBase::OnMouseEnter(CXTPMarkupMouseEventArgs* e)
{
	m_bMouseOver = TRUE;

	if (m_bPushed)
	{
		SetValue(m_pIsPressedProperty, CXTPMarkupBool::CreateTrueValue());
	}
	else
	{
		CXTPMarkupContentControl::OnMouseEnter(e);
		InvalidateVisual();
	}
}

void CXTPMarkupButtonBase::OnLostMouseCapture(CXTPMarkupMouseEventArgs* /*e*/)
{
	SetPressed(FALSE);
}

void CXTPMarkupButtonBase::SetPressed(BOOL bPressed)
{
	if (!bPressed && m_bPushed)
	{
		m_bPushed = FALSE;
		SetValue(m_pIsPressedProperty, NULL);

		if (!m_bMouseOver && IsMouseOver())
		{
			SetValue(m_pIsMouseOverProperty, NULL);
			InvalidateVisual();
		}
	}
	else if (bPressed && !m_bPushed)
	{
		m_bPushed = TRUE;
		SetValue(m_pIsPressedProperty, CXTPMarkupBool::CreateTrueValue());
	}
}

void CXTPMarkupButtonBase::OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e)
{
	if (m_bPushed)
	{
		ReleaseMouseCapture();

		SetPressed(FALSE);

		if (m_bMouseOver)
		{
			OnClick();
		}

		e->SetHandled();
	}
}

void CXTPMarkupButtonBase::OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e)
{
	e->SetHandled();

	if (!IsEnabled())
		return;

	Focus();

	SetPressed(TRUE);

	CaptureMouse();
}

void CXTPMarkupButtonBase::OnClick()
{
	CXTPMarkupRoutedEventArgs* e = new CXTPMarkupRoutedEventArgs(m_pClickEvent, this);
	RaiseEvent(e);
	e->Release();
}

BOOL CXTPMarkupButtonBase::IsPressed() const
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool, GetValue(m_pIsPressedProperty));
	return pValue ? (BOOL)*pValue : FALSE;
}

void CXTPMarkupButtonBase::UseParentBackground(HDC destHdc, HDC srcHdc, CRect srcRect) const
{
	::BitBlt(destHdc, 0, 0, srcRect.Width(), srcRect.Height(), srcHdc, srcRect.left, srcRect.top,
			 SRCCOPY);
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupButtonBase, CXTPMarkupContentControl)
	DISP_PROPERTY_EX_ID(CXTPMarkupButtonBase, "IsPressed", 700, OleGetIsPressed, OleSetIsPressed,
						VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupButtonBase, "ClickEvent", 701, OleGetClickEvent, SetNotSupported,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupButtonBase, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupButtonBase, XTPDIID_MarkupButtonBase, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupButtonBase, XTPDIID_MarkupButtonBase)
#endif

BOOL CXTPMarkupButtonBase::OleGetIsPressed()
{
	return IsPressed();
}

void CXTPMarkupButtonBase::OleSetIsPressed(BOOL /*bNewValue*/)
{
}

LPDISPATCH CXTPMarkupButtonBase::OleGetClickEvent()
{
	return XTPGetDispatch(m_pClickEvent);
}
