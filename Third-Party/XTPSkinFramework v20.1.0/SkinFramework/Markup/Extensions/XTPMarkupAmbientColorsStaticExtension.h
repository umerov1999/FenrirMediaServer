// XTPMarkupAmbientColorsStaticExtension.h: interface for the CXTPMarkupAmbientColorsStaticExtension
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
#if !defined(__XTPMARKUPAMBIENTCOLORSSTATICEXTENSION_H__)
#	define __XTPMARKUPAMBIENTCOLORSSTATICEXTENSION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE

class CXTPPaintManager;
enum XTPPaintManagerAccentColor;

class _XTP_EXT_CLASS CXTPMarkupAmbientColor
	: public CXTPMarkupColor
	, private IXTPApplicationEvents
{
public:
	explicit CXTPMarkupAmbientColor(XTPApplicationAmbientProperty nProperty);
	~CXTPMarkupAmbientColor();

private:
	virtual void OnAmbientPropertyChanged(CXTPApplication* pApplication,
										  XTPApplicationAmbientProperty nProperty);

	XTPApplicationAmbientProperty m_nProperty;
};

class _XTP_EXT_CLASS CXTPMarkupAmbientColorBrush
	: public CXTPMarkupSolidColorBrush
	, private IXTPApplicationEvents
{
public:
	explicit CXTPMarkupAmbientColorBrush(XTPApplicationAmbientProperty nProperty);
	~CXTPMarkupAmbientColorBrush();

private:
	virtual void OnAmbientPropertyChanged(CXTPApplication* pApplication,
										  XTPApplicationAmbientProperty nProperty);
	XTPApplicationAmbientProperty m_nProperty;
};

class _XTP_EXT_CLASS CXTPMarkupAmbientColorsStaticExtensionProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	CXTPMarkupAmbientColorsStaticExtensionProperty(LPCWSTR lpzName,
												   XTPApplicationAmbientProperty nProperty);
	static CXTPMarkupColor* AFX_CDECL GetAmbientColor(XTPApplicationAmbientProperty nProperty);
};

class _XTP_EXT_CLASS CXTPMarkupAmbientColorsBrushStaticExtensionProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	CXTPMarkupAmbientColorsBrushStaticExtensionProperty(LPCWSTR lpzName,
														XTPApplicationAmbientProperty nProperty);

	static CXTPMarkupBrush* AFX_CDECL GetAmbientColorsBrush(XTPApplicationAmbientProperty nProperty);
};

class _XTP_EXT_CLASS CXTPMarkupAmbientColorsStaticExtension : public CXTPMarkupStaticExtension
{
public:
	CXTPMarkupAmbientColorsStaticExtension();
};

class _XTP_EXT_CLASS CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty
	: public CXTPMarkupStaticExtensionProperty
	, private IXTPApplicationEvents
{
public:
	CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty(LPCWSTR lpzName,
																XTPPaintManagerAccentColor nColor);
	~CXTPMarkupPaintManagerBaseThemeBrushStaticExtensionProperty();

private:
	XTPPaintManagerAccentColor m_nColor;

	virtual void OnAmbientPropertyChanged(CXTPApplication* pApplication,
										  XTPApplicationAmbientProperty nProperty);
};

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPAMBIENTCOLORSSTATICEXTENSION_H__)
