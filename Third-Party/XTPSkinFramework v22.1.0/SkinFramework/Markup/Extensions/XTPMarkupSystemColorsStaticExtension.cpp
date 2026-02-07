// XTPMarkupSystemColorsStaticExtension.cpp: implementation of the
// CXTPMarkupSystemColorsStaticExtension class.
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
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/Extensions/XTPMarkupStaticExtension.h"
#include "Markup/Extensions/XTPMarkupSystemColorsStaticExtension.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSystemColorStaticExtension

CXTPMarkupSystemColorStaticExtension::CXTPMarkupSystemColorStaticExtension(LPCWSTR lpzName,
																		   int nColorIndex)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(GetSystemColor(nColorIndex));
}

CXTPMarkupColor* AFX_CDECL CXTPMarkupSystemColorStaticExtension::GetSystemColor(int nColorIndex)
{
	return new CXTPMarkupColor(GetSysColor(nColorIndex) | 0xFF000000);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSystemColorKeyStaticExtension

CXTPMarkupSystemColorKeyStaticExtension::CXTPMarkupSystemColorKeyStaticExtension(LPCWSTR lpzName,
																				 int nColorIndex)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(new CXTPMarkupColorKey(nColorIndex));
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSystemBrushStaticExtension

CXTPMarkupSystemBrushStaticExtension::CXTPMarkupSystemBrushStaticExtension(LPCWSTR lpzName,
																		   int nColorIndex)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(GetSystemBrush(nColorIndex));
}

CXTPMarkupBrush* AFX_CDECL CXTPMarkupSystemBrushStaticExtension::GetSystemBrush(int nColorIndex)
{
	return new CXTPMarkupSolidColorBrush(GetSysColor(nColorIndex) | 0xFF000000);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSystemBrushKeyStaticExtension

CXTPMarkupSystemBrushKeyStaticExtension::CXTPMarkupSystemBrushKeyStaticExtension(LPCWSTR lpzName,
																				 int nColorIndex)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(new CXTPMarkupBrushKey(nColorIndex));
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSystemColorsStaticExtension

CXTPMarkupSystemColorsStaticExtension::CXTPMarkupSystemColorsStaticExtension()
	: CXTPMarkupStaticExtension(L"SystemColors")
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
		LPCWSTR lpzColorKeyName;
		LPCWSTR lpzBrushName;
		LPCWSTR lpzBrushKeyName;
		int nColorIndex;
	} systemColors[] = {
#define XTP_DEFINE_SYSTEM_ENTITY_NAMES(Name)                                                       \
	Name L"Color", Name L"ColorKey", Name L"Brush", Name L"BrushKey"
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ActiveBorder"), COLOR_ACTIVEBORDER },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ActiveCaption"), COLOR_ACTIVECAPTION },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ActiveCaptionText"), COLOR_CAPTIONTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"AppWorkspace"), COLOR_APPWORKSPACE },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Control"), COLOR_BTNFACE },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ControlDark"), COLOR_BTNSHADOW },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ControlDarkDark"), COLOR_3DDKSHADOW },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ControlLight"), COLOR_3DLIGHT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ControlLightLight"), COLOR_BTNHIGHLIGHT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ControlText"), COLOR_BTNTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Desktop"), COLOR_BACKGROUND },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"GradientActiveCaption"), COLOR_GRADIENTACTIVECAPTION },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"GradientInactiveCaption"),
		  COLOR_GRADIENTINACTIVECAPTION },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"GrayText"), COLOR_GRAYTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Highlight"), COLOR_HIGHLIGHT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"HighlightText"), COLOR_HIGHLIGHTTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"HotTrack"), COLOR_HOTLIGHT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"InactiveBorder"), COLOR_INACTIVEBORDER },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"InactiveCaption"), COLOR_INACTIVECAPTION },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"InactiveCaptionText"), COLOR_INACTIVECAPTIONTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Info"), COLOR_INFOBK },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"InfoText"), COLOR_INFOTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Menu"), COLOR_MENU },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"MenuBar"), COLOR_MENUBAR },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"MenuHighlight"), COLOR_MENUHILIGHT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"MenuText"), COLOR_MENUTEXT },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"ScrollBar"), COLOR_SCROLLBAR },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"Window"), COLOR_WINDOW },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"WindowFrame"), COLOR_WINDOWFRAME },
		{ XTP_DEFINE_SYSTEM_ENTITY_NAMES(L"WindowText"), COLOR_WINDOWTEXT },
	};

	for (int i = 0; i < _countof(systemColors); ++i)
	{
		Extend(new CXTPMarkupSystemColorStaticExtension(systemColors[i].lpzColorName,
														systemColors[i].nColorIndex));
		Extend(new CXTPMarkupSystemColorKeyStaticExtension(systemColors[i].lpzColorKeyName,
														   systemColors[i].nColorIndex));
		Extend(new CXTPMarkupSystemBrushStaticExtension(systemColors[i].lpzBrushName,
														systemColors[i].nColorIndex));
		Extend(new CXTPMarkupSystemBrushKeyStaticExtension(systemColors[i].lpzBrushKeyName,
														   systemColors[i].nColorIndex));
	}
}
