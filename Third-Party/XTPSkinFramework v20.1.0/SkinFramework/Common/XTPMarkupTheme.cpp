// XTPMarkupTheme.cpp
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

#include "GraphicLibrary/unzip/unzip.h"
#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPMarkupTheme.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPMarkupRender.h"

#include "Markup/XTPMarkupObject.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPMarkupThemePart::CXTPMarkupThemePart()
	: m_pMarkupUIElement(NULL)
{
}

CXTPMarkupThemePart::~CXTPMarkupThemePart()
{
	if (NULL != m_pMarkupUIElement)
	{
		XTPMarkupReleaseElement(m_pMarkupUIElement);
	}
}

BOOL CXTPMarkupThemePart::LoadPart(HZIP hZip, LPCTSTR pszFileName,
								   CXTPMarkupContext* pMarkupContext)
{
	ZRESULT zResult;
	ZIPENTRY zipEntry;
	int iIndex;

	ASSERT(NULL == m_pMarkupUIElement);

	zResult = ::FindZipItem(hZip, pszFileName, true, &iIndex, &zipEntry);
	ASSERT(ZR_OK == zResult); // File not found in archive

	if (ZR_OK == zResult)
	{
		char* pszBuffer = new char[XTPToUIntChecked(zipEntry.unc_size + 1)];
		ZeroMemory(pszBuffer, sizeof(char) * (zipEntry.unc_size + 1));

		zResult = ::UnzipItem(hZip, iIndex, pszBuffer, XTPToUIntChecked(zipEntry.unc_size));
		ASSERT(ZR_OK == zResult); // Extracting failed

		CString sXaml	   = pszBuffer;
		m_pMarkupUIElement = XTPMarkupParseText(pMarkupContext, sXaml);
		ASSERT(NULL != m_pMarkupUIElement);

		SAFE_DELETE_AR(pszBuffer);
	}

	return (ZR_OK == zResult);
}

void CXTPMarkupThemePart::RenderMarkup(CDC* pDC, CRect rc)
{
	if (NULL != m_pMarkupUIElement)
	{
		ASSERT(pDC);
		XTPMarkupRenderElement(m_pMarkupUIElement, pDC->GetSafeHdc(), rc);
	}
}

CSize CXTPMarkupThemePart::Measure(int cx, int cy)
{
	if (NULL != m_pMarkupUIElement)
	{
		return XTPMarkupMeasureElement(m_pMarkupUIElement, cx, cy);
	}
	else
	{
		return CSize(0, 0);
	}
}
