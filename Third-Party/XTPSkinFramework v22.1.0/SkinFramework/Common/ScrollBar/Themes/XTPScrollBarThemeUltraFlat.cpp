// XTPScrollBarThemeUltraFlat.cpp
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

#include "StdAfx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPColorManager.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsClassic.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsUx.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeFlat.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeUltraFlat.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CXTPScrollBarThemeUltraFlat::RefreshMetrics()
{
	CXTPScrollBarThemeFlat::RefreshMetrics();
	m_crBorderHilite = m_crBorder;

	COLORREF clrWindow = GetXtremeColor(COLOR_WINDOW);
	COLORREF clrFace   = GetXtremeColor(COLOR_3DFACE);
	COLORREF clrPushed = GetXtremeColor(COLOR_3DSHADOW);

	m_crBackPushed = XTPColorManager()->LightColor(clrPushed, clrFace, 0x32);
	m_crBackHilite = XTPColorManager()->LightColor(clrFace, clrWindow, 0x1E);
}
