// XTPMarkupAmbientColorsStaticExtension.cpp: implementation of the
// CXTPMarkupAmbientColorsStaticExtension class.
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
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPColorManager.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/Extensions/XTPMarkupStaticExtension.h"
#include "Markup/Extensions/XTPMarkupAmbientColorsStaticExtension.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

inline COLORREF GetColorAmbientProperty(XTPApplicationAmbientProperty nProperty)
{
	const COleVariant& vtValue = XTPGetApplication()->GetAmbientProperty(nProperty);
	return (static_cast<COLORREF>(vtValue.lVal) | 0xff000000);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupAmbientColor

CXTPMarkupAmbientColor::CXTPMarkupAmbientColor(XTPApplicationAmbientProperty nProperty)
	: CXTPMarkupColor(GetColorAmbientProperty(nProperty))
	, m_nProperty(nProperty)
{
	XTPGetApplication()->Subscribe(this);
}

CXTPMarkupAmbientColor::~CXTPMarkupAmbientColor()
{
	XTPGetApplication()->Unsubscribe(this);
}

void CXTPMarkupAmbientColor::OnAmbientPropertyChanged(CXTPApplication* pApplication,
													  XTPApplicationAmbientProperty nProperty)
{
	UNREFERENCED_PARAMETER(pApplication);

	ASSERT(NULL != pApplication);

	if (nProperty != m_nProperty)
		return;

	m_nValue = GetColorAmbientProperty(nProperty);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupAmbientColorBrush

CXTPMarkupAmbientColorBrush::CXTPMarkupAmbientColorBrush(XTPApplicationAmbientProperty nProperty)
	: CXTPMarkupSolidColorBrush(GetColorAmbientProperty(nProperty))
	, m_nProperty(nProperty)
{
	XTPGetApplication()->Subscribe(this);
}

CXTPMarkupAmbientColorBrush::~CXTPMarkupAmbientColorBrush()
{
	XTPGetApplication()->Unsubscribe(this);
}

void CXTPMarkupAmbientColorBrush::OnAmbientPropertyChanged(CXTPApplication* pApplication,
														   XTPApplicationAmbientProperty nProperty)
{
	UNREFERENCED_PARAMETER(pApplication);

	ASSERT(NULL != pApplication);

	if (nProperty != m_nProperty)
		return;

	OleSetColor(GetColorAmbientProperty(nProperty));
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupAmbientColorsStaticExtensionProperty

CXTPMarkupAmbientColorsStaticExtensionProperty::CXTPMarkupAmbientColorsStaticExtensionProperty(
	LPCWSTR lpzName, XTPApplicationAmbientProperty nProperty)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(GetAmbientColor(nProperty));
}

CXTPMarkupColor* AFX_CDECL CXTPMarkupAmbientColorsStaticExtensionProperty::GetAmbientColor(
	XTPApplicationAmbientProperty nProperty)
{
	return new CXTPMarkupAmbientColor(nProperty);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupAmbientColorsBrushStaticExtension

CXTPMarkupAmbientColorsBrushStaticExtensionProperty::
	CXTPMarkupAmbientColorsBrushStaticExtensionProperty(LPCWSTR lpzName,
														XTPApplicationAmbientProperty nProperty)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(GetAmbientColorsBrush(nProperty));
}

CXTPMarkupBrush* AFX_CDECL
	CXTPMarkupAmbientColorsBrushStaticExtensionProperty::GetAmbientColorsBrush(
		XTPApplicationAmbientProperty nProperty)
{
	return new CXTPMarkupAmbientColorBrush(nProperty);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupAmbientColorsStaticExtension

CXTPMarkupAmbientColorsStaticExtension::CXTPMarkupAmbientColorsStaticExtension()
	: CXTPMarkupStaticExtension(L"AmbientColors")
{
#ifndef COLOR_3DDKSHADOW
#	define COLOR_3DDKSHADOW 21
#endif

#ifndef COLOR_3DLIGHT
#	define COLOR_3DLIGHT 22
#endif

#ifndef COLOR_INFOTEXT
#	define COLOR_INFOTEXT 23
#endif

#ifndef COLOR_INFOBK
#	define COLOR_INFOBK 24
#endif

#ifndef COLOR_HOTLIGHT
#	define COLOR_HOTLIGHT 26
#endif

#ifndef COLOR_GRADIENTACTIVECAPTION
#	define COLOR_GRADIENTACTIVECAPTION 27
#endif

#ifndef COLOR_GRADIENTINACTIVECAPTION
#	define COLOR_GRADIENTINACTIVECAPTION 28
#endif

#ifndef COLOR_MENUHILIGHT
#	define COLOR_MENUHILIGHT 29
#endif

#ifndef COLOR_MENUBAR
#	define COLOR_MENUBAR 30
#endif

	static const struct
	{
		LPCWSTR lpzColorName;
		LPCWSTR lpzBrushName;
		XTPApplicationAmbientProperty nProperty;
	} colors[] = {
#define XTP_DEFINE_SYSTEM_ENTITY_NAMES(Name) Name L"Color", Name L"Brush"
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Accent"), xtpApplicationAccentColor },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"State"), xtpApplicationStateColor },
	};

	for (int i = 0; i < _countof(colors); ++i)
	{
		Extend(new CXTPMarkupAmbientColorsStaticExtensionProperty(colors[i].lpzColorName,
																  colors[i].nProperty));
		Extend(new CXTPMarkupAmbientColorsBrushStaticExtensionProperty(colors[i].lpzBrushName,
																	   colors[i].nProperty));
	}
}

CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty::
	CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty(LPCWSTR lpzName,
																XTPPaintManagerAccentColor nColor)
	: CXTPMarkupStaticExtensionProperty(lpzName)
	, IXTPApplicationEvents()
{
	m_nColor = nColor;

	UpdateColorValue();

	XTPGetApplication()->Subscribe(this);
}

CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty::
	~CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty()
{
	XTPGetApplication()->Unsubscribe(this);
}

void CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty::OnAmbientPropertyChanged(
	CXTPApplication* pApplication, XTPApplicationAmbientProperty nProperty)
{
	UNREFERENCED_PARAMETER(pApplication);

	ASSERT(NULL != pApplication);

	if (nProperty != xtpApplicationAccentColor)
		return;

	UpdateColorValue();
}

void CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty::UpdateColorValue()
{
	COLORREF crColor		  = XTPGetAccentColor(m_nColor);
	CXTPMarkupObject* pObject = new CXTPMarkupSolidColorBrush(crColor);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}
