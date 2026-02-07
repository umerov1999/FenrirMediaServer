// XTPScrollBarThemeVisualStudio2015.cpp
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
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPResourceImage.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeVisualStudio2012.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeVisualStudio2015.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPScrollBarThemeVisualStudio2015

void CXTPScrollBarThemeVisualStudio2015::DrawScrollBar(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();

	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);
	if (cWidth <= 0)
	{
		return;
	}

	m_State.nPressedHt = pSBTrack ? (pSBTrack->bTrackThumb || pSBTrack->fHitOld ? pSBInfo->ht : -1)
								  : -1;
	m_State.nHotHt   = pSBTrack ? -1 : pSBInfo->ht;
	m_State.bEnabled = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0)
					   && pScroll->IsScrollBarEnabled();
	m_State.nBtnTrackSize = pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	m_State.nBtnTrackPos  = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!m_State.bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		m_State.nBtnTrackPos = m_State.nBtnTrackSize = 0;

	m_State.clrBackground = XTPIniColor(_T("Common.ScrollBar"), _T("NormalBack"),
										RGB(243, 243, 243));

	m_State.clrArrowDisabled = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledScrollArrow"),
										   RGB(171, 171, 171));
	m_State.clrArrowEnabled  = XTPIniColor(_T("Common.ScrollBar"), _T("NormalScrollArrow"),
										   RGB(119, 119, 119));
	m_State.clrArrowHot		 = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedScrollArrow"),
									   RGB(119, 119, 119));
	m_State.clrArrowPressed  = XTPIniColor(_T("Common.ScrollBar"), _T("PressedScrollArrow"),
										   RGB(119, 119, 119));

	m_State.clrBtnTrackEnabled = XTPIniColor(_T("Common.ScrollBar"), _T("NormalThumbBack"),
											 RGB(255, 255, 255));
	m_State.clrBtnTrackHot	 = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedThumbBack"),
										 RGB(240, 240, 240));
	m_State.clrBtnTrackPressed = XTPIniColor(_T("Common.ScrollBar"), _T("PressedThumbBack"),
											 RGB(240, 240, 240));

	CXTPScrollBarPaintManager::DrawScrollBar(pDC, pScroll);
}
