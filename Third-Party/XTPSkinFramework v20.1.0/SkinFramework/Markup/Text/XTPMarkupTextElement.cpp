// XTPMarkupTextElement.cpp: implementation of the CXTPMarkupTextElement class.
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
#include "Common/XTPResourceManager.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupTextElement

CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pBackgroundProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pForegroundProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pFontSizeProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pFontWeightProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pFontFamilyProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pFontStyleProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pTextDecorationsProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pFontQualityProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextElement::m_pFontCharsetProperty		= NULL;

IMPLEMENT_MARKUPCLASS(L"TextElement", CXTPMarkupTextElement, CXTPMarkupFrameworkContentElement);

void CXTPMarkupTextElement::RegisterMarkupClass()
{
	m_pBackgroundProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"Background", MARKUP_TYPE(CXTPMarkupBrush), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagInherited
												 | CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pForegroundProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"Foreground", MARKUP_TYPE(CXTPMarkupBrush), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagInherited
												 | CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pFontSizeProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"FontSize", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertLength,
									   CXTPMarkupPropertyMetadata::flagInherited
										   | CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagVertDpiSensible));

	m_pFontWeightProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"FontWeight", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertFontWeight,
									   CXTPMarkupPropertyMetadata::flagInherited
										   | CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pFontFamilyProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"FontFamily", MARKUP_TYPE(CXTPMarkupString), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagInherited
												 | CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pFontStyleProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"FontStyle", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertFontStyle,
									   CXTPMarkupPropertyMetadata::flagInherited
										   | CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pTextDecorationsProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"TextDecorations", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertTextDecorations,
									   CXTPMarkupPropertyMetadata::flagInherited
										   | CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pFontQualityProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"FontQuality", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertFontQuality,
									   CXTPMarkupPropertyMetadata::flagInherited
										   | CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pFontCharsetProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"FontCharset", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupTextElement),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagInherited
												 | CXTPMarkupPropertyMetadata::flagAffectsRender));
}

CXTPMarkupTextElement::CXTPMarkupTextElement()
{
}

CXTPMarkupTextElement::~CXTPMarkupTextElement()
{
}

void CXTPMarkupTextElement::SetBackground(CXTPMarkupBrush* pBrush)
{
	SetValue(m_pBackgroundProperty, pBrush);
}

void CXTPMarkupTextElement::SetBackground(COLORREF clr)
{
	SetValue(m_pBackgroundProperty, new CXTPMarkupSolidColorBrush(clr));
}

void CXTPMarkupTextElement::SetBackground(CXTPMarkupObject* pObject, CXTPMarkupBrush* pBrush)
{
	pObject->SetValue(m_pBackgroundProperty, pBrush);
}

CXTPMarkupBrush* CXTPMarkupTextElement::GetBackground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pBackgroundProperty));
}

void CXTPMarkupTextElement::SetForeground(CXTPMarkupBrush* pBrush)
{
	SetValue(m_pForegroundProperty, pBrush);
}

void CXTPMarkupTextElement::SetForeground(COLORREF clr)
{
	SetValue(m_pForegroundProperty, new CXTPMarkupSolidColorBrush(clr));
}

void CXTPMarkupTextElement::SetForeground(CXTPMarkupObject* pObject, CXTPMarkupBrush* pBrush)
{
	pObject->SetValue(m_pForegroundProperty, pBrush);
}

CXTPMarkupBrush* CXTPMarkupTextElement::GetForeground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pForegroundProperty));
}

void CXTPMarkupTextElement::SetFontSize(int nFontSize)
{
	SetValue(m_pFontSizeProperty, new CXTPMarkupInt(nFontSize));
}

int CXTPMarkupTextElement::GetFontSize() const
{
	CXTPMarkupInt* pFontSize = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pFontSizeProperty));
	if (!pFontSize)
		return -12;

	return *pFontSize;
}

void CXTPMarkupTextElement::SetFontWeight(int nFontWeight)
{
	SetValue(m_pFontWeightProperty, CXTPMarkupEnum::CreateValue(nFontWeight));
}

int CXTPMarkupTextElement::GetFontWeight() const
{
	CXTPMarkupEnum* pFontWeigh = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pFontWeightProperty));
	if (!pFontWeigh)
		return FW_NORMAL;

	return *pFontWeigh;
}

void CXTPMarkupTextElement::SetFontStyle(int nFontStyle)
{
	SetValue(m_pFontStyleProperty, CXTPMarkupEnum::CreateValue(nFontStyle));
}

int CXTPMarkupTextElement::GetFontStyle() const
{
	CXTPMarkupEnum* pFontStyle = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pFontStyleProperty));
	if (!pFontStyle)
		return 0;

	return *pFontStyle;
}

void CXTPMarkupTextElement::SetTextDecorations(int nTextDecorations)
{
	SetValue(m_pTextDecorationsProperty, CXTPMarkupEnum::CreateValue(nTextDecorations));
}

int CXTPMarkupTextElement::GetTextDecorations() const
{
	CXTPMarkupEnum* pTextDecorations = MARKUP_STATICCAST(CXTPMarkupEnum,
														 GetValue(m_pTextDecorationsProperty));
	if (!pTextDecorations)
		return 0;

	return *pTextDecorations;
}

void CXTPMarkupTextElement::SetFontFamily(LPCWSTR lpszFontFamily)
{
	SetValue(m_pFontFamilyProperty, CXTPMarkupString::CreateValue(lpszFontFamily));
}

LPCWSTR CXTPMarkupTextElement::GetFontFamily() const
{
	CXTPMarkupString* pFontFamily = MARKUP_STATICCAST(CXTPMarkupString,
													  GetValue(m_pFontFamilyProperty));
	if (!pFontFamily)
		return NULL;

	return *pFontFamily;
}

/////////////////////////////////////////////////////////////////////////
// TextElement

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupTextElement, CXTPMarkupFrameworkContentElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "Tag", 400, OleGetTag, OleSetTag, VT_VARIANT)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "TextDecorations", 503, OleGetTextDecorations,
						OleSetTextDecorations, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "FontSize", 504, OleGetFontSize, OleSetFontSize,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "FontWeight", 505, OleGetFontWeight,
						OleSetFontWeight, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "FontFamily", 506, OleGetFontFamily,
						OleSetFontFamily, VT_BSTR)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "FontStyle", 507, OleGetFontStyle, OleSetFontStyle,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "Background", 510, OleGetBackground,
						OleSetBackground, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextElement, "Foreground", 511, OleGetForeground,
						OleSetForeground, VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupTextElement, CXTPMarkupFrameworkContentElement)
	INTERFACE_PART(CXTPMarkupTextElement, XTPDIID_MarkupTextElement, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupTextElement, XTPDIID_MarkupTextElement)
#endif

long CXTPMarkupTextElement::OleGetTextDecorations()
{
	return GetTextDecorations();
}

void CXTPMarkupTextElement::OleSetTextDecorations(long nValue)
{
	SetTextDecorations(nValue);
}

long CXTPMarkupTextElement::OleGetFontSize()
{
	return GetFontSize();
}

void CXTPMarkupTextElement::OleSetFontSize(long nValue)
{
	SetFontSize(nValue);
}

long CXTPMarkupTextElement::OleGetFontWeight()
{
	return GetFontWeight();
}

void CXTPMarkupTextElement::OleSetFontWeight(long nValue)
{
	SetFontWeight(nValue);
}

BSTR CXTPMarkupTextElement::OleGetFontFamily()
{
	return SysAllocString(GetFontFamily());
}

void CXTPMarkupTextElement::OleSetFontFamily(LPCTSTR lpszValue)
{
	SetFontFamily(XTP_CT2CW(lpszValue));
}

long CXTPMarkupTextElement::OleGetFontStyle()
{
	return GetFontStyle();
}

void CXTPMarkupTextElement::OleSetFontStyle(long nValue)
{
	SetFontStyle(nValue);
}

LPDISPATCH CXTPMarkupTextElement::OleGetBackground()
{
	return XTPGetDispatch(GetBackground());
}

void CXTPMarkupTextElement::OleSetBackground(LPDISPATCH lpBackground)
{
	SetBackground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpBackground)));
}

LPDISPATCH CXTPMarkupTextElement::OleGetForeground()
{
	return XTPGetDispatch(GetForeground());
}

void CXTPMarkupTextElement::OleSetForeground(LPDISPATCH lpBackground)
{
	SetForeground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpBackground)));
}

VARIANT CXTPMarkupTextElement::OleGetTag()
{
	VARIANT va;
	VariantInit(&va);
	va.vt = VT_NULL;

	CXTPMarkupObject* pTag = GetTag();
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

void CXTPMarkupTextElement::OleSetTag(const VARIANT& va)
{
	if (va.vt == VT_BSTR)
	{
		SetTag(new CXTPMarkupString(va.bstrVal));
	}
}
