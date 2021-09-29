// XTPMarkupSystemColorsStaticExtension.h: interface for the CXTPMarkupSystemColorsStaticExtension
// class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPSYSTEMCOLORSSTATICEXTENSION_H__)
#	define __XTPMARKUPSYSTEMCOLORSSTATICEXTENSION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupColor;
class CXTPMarkupColorKey;
class CXTPMarkupBrush;
class CXTPMarkupBrushKey;

//===========================================================================
// Summary:
//     A property static extension object that represents a system color.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupSystemColorStaticExtension : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a system color extension object.
	// Parameters:
	//     lpzName     - System color name. Has to satisfy extension name rules.
	//     nColorIndex - System color index. Refer to the GetSysColor Win32
	//                   API function documentation for possible values.
	// See Also:
	//     CXTPMarkupStaticExtension::Parse
	//-----------------------------------------------------------------------
	CXTPMarkupSystemColorStaticExtension(LPCWSTR lpzName, int nColorIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a system color markup object.
	// Parameters:
	//     nColorIndex - System color index. Refer to the GetSysColor Win32
	//                   API function documentation for possible values.
	// Returns:
	//     A pointer to a CXTPMarkupColor object. A caller is responsible
	//     for releasing an object.
	//-----------------------------------------------------------------------
	static CXTPMarkupColor* AFX_CDECL GetSystemColor(int nColorIndex);
};

//===========================================================================
// Summary:
//     A property static extension object that represents a system color key.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupSystemColorKeyStaticExtension
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a system color key extension object.
	// Parameters:
	//     lpzName     - System color name. Has to satisfy extension name rules.
	//     nColorIndex - System color index. Refer to the GetSysColor Win32
	//                   API function documentation for possible values.
	// See Also:
	//     CXTPMarkupStaticExtension::Parse
	//-----------------------------------------------------------------------
	CXTPMarkupSystemColorKeyStaticExtension(LPCWSTR lpzName, int nColorIndex);
};

//===========================================================================
// Summary:
//     A property static extension object that represents a system brush.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupSystemBrushStaticExtension : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a system brush extension object.
	// Parameters:
	//     lpzName     - System brush name. Has to satisfy extension name rules.
	//     nColorIndex - System color index. Refer to the GetSysColor Win32
	//                   API function documentation for possible values.
	// See Also:
	//     CXTPMarkupStaticExtension::Parse
	//-----------------------------------------------------------------------
	CXTPMarkupSystemBrushStaticExtension(LPCWSTR lpzName, int nColorIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a system brush markup object.
	// Parameters:
	//     nColorIndex - System color index. Refer to the GetSysColor Win32
	//                   API function documentation for possible values.
	// Returns:
	//     A pointer to a CXTPMarkupBrush object. A caller is responsible for
	//     releasing an object.
	//-----------------------------------------------------------------------
	static CXTPMarkupBrush* AFX_CDECL GetSystemBrush(int nColorIndex);
};

//===========================================================================
// Summary:
//     A property static extension object that represents a system brush key.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupSystemBrushKeyStaticExtension
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a system brush key extension object.
	// Parameters:
	//     lpzName     - System brush name. Has to satisfy extension name rules.
	//     nColorIndex - System color index. Refer to the GetSysColor Win32
	//                   API function documentation for possible values.
	// See Also:
	//     CXTPMarkupStaticExtension::Parse
	//-----------------------------------------------------------------------
	CXTPMarkupSystemBrushKeyStaticExtension(LPCWSTR lpzName, int nColorIndex);
};

//===========================================================================
// Summary:
//     A composite static extension object that represents system colors.
//==========================================================================
class _XTP_EXT_CLASS CXTPMarkupSystemColorsStaticExtension : public CXTPMarkupStaticExtension
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupSystemColorsStaticExtension object.
	//-----------------------------------------------------------------------
	CXTPMarkupSystemColorsStaticExtension();
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSYSTEMCOLORSSTATICEXTENSION_H__)
