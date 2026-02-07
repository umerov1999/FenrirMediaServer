// XTPFontManager.cpp: implementation of the CXTPFontManager class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPFontManager.h"
#include "Common/XTPVC80Helpers.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPFontManager::CXTPFontManager()
{
	CXTPNonClientMetrics ncm;
	ncm.lfMenuFont.lfCharSet = XTPResourceManager()->GetFontCharset();

	if (XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		ncm.lfMenuFont.lfQuality = CLEARTYPE_QUALITY;
	}

	CreateFonts(ncm.lfMenuFont);
}

AFX_STATIC int CALLBACK XTPEnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme,
											 DWORD FontType, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lpntme);
	UNREFERENCED_PARAMETER(FontType);

	const ENUMLOGFONTEX* pelf = reinterpret_cast<const ENUMLOGFONTEX*>(lpelfe);
	ASSERT(NULL != pelf);

	LPCTSTR strFontName = reinterpret_cast<LPCTSTR>(lParam);
	ASSERT(NULL != strFontName);

	CString strFaceName = pelf->elfLogFont.lfFaceName;

	return (strFaceName.CompareNoCase(strFontName) == 0 ? 0 : 1);
}

BOOL CXTPFontManager::FontExists(LPCTSTR strFaceName)
{
	// Enumerate all styles and charsets of all fonts:
	LOGFONT lfEnum;
	::ZeroMemory(&lfEnum, sizeof(LOGFONT));

	lfEnum.lfFaceName[0] = 0;
	lfEnum.lfCharSet	 = DEFAULT_CHARSET;

	CWindowDC dc(NULL);

	return ::EnumFontFamiliesEx(dc.m_hDC, &lfEnum, (FONTENUMPROC)XTPEnumFontFamExProc,
								(LPARAM)strFaceName, 0)
		   == 0;
}

#define DELETE_FONT(font)                                                                          \
	if ((font).GetSafeHandle())                                                                    \
	{                                                                                              \
		(font).DeleteObject();                                                                     \
	}

void CXTPFontManager::CreateFonts(LOGFONT lf)
{
	// normal font.
	DELETE_FONT(m_xtpFont);
	m_xtpFont.CreateFontIndirect(&lf);

	// bold font.
	DELETE_FONT(m_xtpFontBold);
	lf.lfWeight = FW_BOLD;
	m_xtpFontBold.CreateFontIndirect(&lf);

	// italic font.
	DELETE_FONT(m_xtpFontItalic);
	lf.lfItalic = TRUE;
	lf.lfWeight = FW_NORMAL;
	m_xtpFontItalic.CreateFontIndirect(&lf);

	// italic bold font.
	DELETE_FONT(m_xtpFontItalicBold);
	lf.lfWeight = FW_BOLD;
	m_xtpFontItalicBold.CreateFontIndirect(&lf);

	// older OS needs Arial to display vertical fonts.
	if (!XTPSystemVersion()->IsWin2KOrGreater())
	{
		if (FontExists(_T("Arial")))
			STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Arial"));
	}

	// vertical font.
	DELETE_FONT(m_xtpFontVertical);
	lf.lfEscapement = 900; // 90 degreees rotated text
	lf.lfItalic		= FALSE;
	m_xtpFontVertical.CreateFontIndirect(&lf);

	// vertical bold font.
	DELETE_FONT(m_xtpFontVerticalBold);
	lf.lfWeight = FW_BOLD;
	m_xtpFontVerticalBold.CreateFontIndirect(&lf);
}

//////////////////////////////////////////////////////////////////////
// Function: XTPFontManager()

CXTPFontManager* AFX_CDECL XTPFontManager()
{
	return &CXTPSingleton<CXTPFontManager>::Instance();
}
