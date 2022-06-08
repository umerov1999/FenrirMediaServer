// XTPApplication.cpp: implementation of the CXTPApplication class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPFramework.h"
#include "Common/XTPColorManager.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is to enforce CXTPApplication instance to be constructed
// prior to any consumer object.
static const CXTPApplication* gpApplicationInstance = XTPGetApplication();

///////////////////////////////////////////////////////////////////////////////
// CXTPApplication

IMPLEMENT_DYNAMIC(CXTPApplication, CObject);

CXTPApplication::CXTPApplication()
	: m_pModuleState(AfxGetModuleState())
	, m_bShutdown(FALSE)
	, m_hGdiPlusModule(LoadLibrary(_T("GdiPlus.dll")))
	, m_nGdiplusToken(0)
	, m_pGdiplusStartupOutput(new Gdiplus::GdiplusStartupOutput)
{
	if (NULL != m_hGdiPlusModule)
	{
		using namespace Gdiplus;

		GdiplusStartupInput gdiplusStartupInput;
		gdiplusStartupInput.SuppressBackgroundThread = TRUE;
		GdiplusStartup(&m_nGdiplusToken, &gdiplusStartupInput, m_pGdiplusStartupOutput);
		m_pGdiplusStartupOutput->NotificationHook(&m_nGdiplusToken);
	}

	InitAmbientProperties();
}

CXTPApplication::~CXTPApplication()
{
	// It's important to call XTPShutdown, not just Shutdown.
	XTPShutdown();
}

AFX_MODULE_STATE* CXTPApplication::GetModuleState()
{
	return m_pModuleState;
}

void CXTPApplication::SetAmbientProperty(XTPApplicationAmbientProperty nProperty,
										 COleVariant vtValue)
{
	ASSERT(0 <= nProperty && nProperty < xtpApplicationPropertyCount);
	m_arAmbientProperties[nProperty] = vtValue;
	Notify(&IXTPApplicationEvents::OnAmbientPropertyChanged, nProperty);
}

const COleVariant& CXTPApplication::GetAmbientProperty(XTPApplicationAmbientProperty nProperty) const
{
	ASSERT(0 <= nProperty && nProperty < xtpApplicationPropertyCount);
	return m_arAmbientProperties[nProperty];
}

CXTPApplication& AFX_CDECL CXTPApplication::GetInstance()
{
	static CXTPApplication app;

	ASSERT("XTP application has been already shutdown" && !app.m_bShutdown);
	if (app.m_bShutdown)
		AfxThrowOleException(E_FAIL);

	return app;
}

void CXTPApplication::Shutdown()
{
	if (!m_bShutdown)
	{
		TRACE0("Codejock XTP application shutting down ...\n");
		NotifyReversed(&IXTPApplicationEvents::OnBeforeApplicationShutdown);
		NotifyReversed(&IXTPApplicationEvents::OnApplicationShutdown);
		NotifyReversed(&IXTPApplicationEvents::OnAfterApplicationShutdown);

		if (NULL != m_hGdiPlusModule)
		{
			using namespace Gdiplus;

			if (NULL != m_pGdiplusStartupOutput)
				m_pGdiplusStartupOutput->NotificationUnhook(m_nGdiplusToken);

			GdiplusShutdown(m_nGdiplusToken);
			FreeLibrary(m_hGdiPlusModule);
			m_hGdiPlusModule = NULL;
		}

		SAFE_DELETE(m_pGdiplusStartupOutput);

		TRACE0("Codejock XTP application shutting down complete.\n");

		m_bShutdown = TRUE;
	}
}

void CXTPApplication::InitAmbientProperties()
{
	m_arAmbientProperties[xtpApplicationAccentColor] = COleVariant(LONG(COLORREF_NULL));
	m_arAmbientProperties[xtpApplicationStateColor]	 = COleVariant(LONG(COLORREF_NULL));
}

BOOL AFX_CDECL XTPInitialize()
{
	XTPGetApplication();
	return TRUE;
}

CXTPApplication* AFX_CDECL XTPGetApplication()
{
	return &CXTPApplication::GetInstance();
}

void AFX_CDECL XTPShutdown()
{
	// Perform application shutting down just once.
	static CXTPApplication* pApp = XTPGetApplication();
	if (NULL != pApp)
	{
		XTPGetApplication()->Shutdown();
		pApp				  = NULL;
		gpApplicationInstance = NULL;
	}
}

COLORREF XTPGetAccentColor(XTPPaintManagerAccentColor nColor)
{
	COLORREF clrAccent = static_cast<COLORREF>(
		XTPGetApplication()->GetAmbientProperty(xtpApplicationAccentColor).lVal);

	BYTE red		= GetRValue(clrAccent);
	BYTE green		= GetGValue(clrAccent);
	BYTE blue		= GetBValue(clrAccent);
	const BYTE val	= 0x10;
	const BYTE val2 = val * 2;

	COLORREF clrText;
	double luminance = (0.299 * red + 0.587 * green + 0.114 * blue) / 255;
	if (luminance > 0.5)
		clrText = RGB(0, 0, 0);
	else
		clrText = RGB(255, 255, 255);

	COLORREF color;
	switch (nColor)
	{
		case xtpPaintManagerAccentColor: color = clrAccent; break;
		case xtpPaintManagerNormalBackAccentColor:
			color = RGB(val < red ? red - val : 0, val < green ? green - val : 0,
						val < blue ? blue - val : 0);
			break;
		case xtpPaintManagerHighlightBackAccentColor:
			color = RGB(val2 < red ? red - val2 : 0, val2 < green ? green - val2 : 0,
						val2 < blue ? blue - val2 : 0);
			break;
		case xtpPaintManagerPressedBackAccentColor:
		{
			BYTE r = static_cast<BYTE>(red + val);
			BYTE g = static_cast<BYTE>(green + val);
			BYTE b = static_cast<BYTE>(blue + val);
			color  = RGB(red < r ? r : 0xff, green < g ? g : 0xff, blue < b ? b : 0xff);
			break;
		}
		case xtpPaintManagerNormalTextAccentColor: color = clrText; break;
		case xtpPaintManagerHighlightTextAccentColor: color = clrText; break;
		case xtpPaintManagerPressedTextAccentColor: color = clrText; break;
		default: color = clrAccent;
	}

	return color;
}
