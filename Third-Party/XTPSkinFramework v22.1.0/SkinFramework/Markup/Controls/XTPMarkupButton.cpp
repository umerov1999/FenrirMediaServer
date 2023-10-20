// XTPMarkupButton.cpp: implementation of the CXTPMarkupButton class.
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
#include "Common/Base/cxminmax.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"

#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupButtonBase.h"
#include "Markup/Controls/XTPMarkupButton.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupButton

CXTPMarkupButton::CXTPMarkupButton()
{
}

CXTPMarkupButton::~CXTPMarkupButton()
{
}

IMPLEMENT_MARKUPCLASS(L"Button", CXTPMarkupButton, CXTPMarkupButtonBase)

void CXTPMarkupButton::RegisterMarkupClass()
{
}

CSize CXTPMarkupButton::ArrangeOverride(CSize szFinalSize)
{
	CXTPMarkupUIElement* pContent = GetContent();
	if (pContent != NULL)
	{
		CRect rt(0, 0, szFinalSize.cx, szFinalSize.cy);

		if (IsPressed()
			&& (!m_themeButton->IsAppThemed() || GetValue(m_pBackgroundProperty) != NULL))
		{
			rt.OffsetRect(m_pMarkupContext->ScaleX(1), m_pMarkupContext->ScaleY(1));
		}

		CRect finalRect = CXTPMarkupThickness::HelperDeflateRect(rt, GetPadding());
		finalRect.DeflateRect(m_pMarkupContext->ScaleX(4), m_pMarkupContext->ScaleY(4));
		pContent->Arrange(finalRect);
	}
	return szFinalSize;
}

CSize CXTPMarkupButton::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	m_themeButton->OpenThemeData(NULL, L"BUTTON");

	CSize size3 = GetPadding()->GetSize();
	size3.cx += m_pMarkupContext->ScaleX(8);
	size3.cy += m_pMarkupContext->ScaleY(8);

	CXTPMarkupUIElement* pContent = GetContent();
	if (pContent != NULL)
	{
		CSize availableSize(CXTP_max(0, szAvailableSize.cx - size3.cx),
			CXTP_max(0, szAvailableSize.cy - size3.cy));
		pContent->Measure(pDC, availableSize);

		CSize desiredSize = pContent->GetDesiredSize();

		return CSize(desiredSize.cx + size3.cx, desiredSize.cy + size3.cy);
	}

	return size3;
}

void CXTPMarkupButton::OnRender(CXTPMarkupDrawingContext* pDC)
{
	BOOL bPressed = IsPressed();
	BOOL bHot	 = IsMouseOver();
	BOOL bDefault = FALSE;
	BOOL bEnabled = IsEnabled();
	BOOL bChecked = FALSE;
	CRect rc(0, 0, GetRenderSize().cx, GetRenderSize().cy);

	CXTPMarkupBrush* pBackground = MARKUP_STATICCAST(CXTPMarkupBrush,
													 GetValue(m_pBackgroundProperty));

	if (pBackground == NULL && m_themeButton->IsAppThemeActive())
	{
		CXTPMarkupDedicatedDC* pDDC = pDC->GetDeviceContext()->GetDedicatedDC(rc);
		if (pDDC)
		{
			HDC srcHDC = pDC->GetDC();
			if (srcHDC)
			{
				UseParentBackground(pDDC->GetDC(), srcHDC, rc);
				pDC->ReleaseDC(srcHDC);
			}
		}

		rc.OffsetRect(pDDC->GetCorrectionOffset());

		int nState = !bEnabled ? PBS_DISABLED
							   : bPressed ? PBS_PRESSED
										  : bHot ? PBS_HOT
												 : bChecked ? PBS_PRESSED
															: bDefault ? PBS_DEFAULTED : PBS_NORMAL;
		m_themeButton->DrawThemeBackground(pDDC->GetDC(), BP_PUSHBUTTON, nState, rc, NULL);

		pDDC->Commit();
		delete pDDC;
	}
	else
	{
		COLORREF clrLight, clrDark, clrDarkDark, clrBackground;

		if (pBackground)
		{
			clrBackground		  = pBackground->GetHintColor();
			DWORD dwHSLBackground = CXTPDrawHelpers::RGBtoHSL(clrBackground);

			DWORD dwL = GetBValue(dwHSLBackground);

			DWORD dwLight = (dwL + 240) / 2;

			clrLight = CXTPDrawHelpers::HSLtoRGB(
				RGB(GetRValue(dwHSLBackground), GetGValue(dwHSLBackground), dwLight));

			DWORD dwDark = XTPToDWORD(MulDiv(XTPToInt(dwL), 2, 3));

			clrDark = CXTPDrawHelpers::HSLtoRGB(
				RGB(GetRValue(dwHSLBackground), GetGValue(dwHSLBackground), dwDark));
			clrDarkDark = RGB(GetRValue(clrDark) / 2, GetGValue(clrDark) / 2,
							  GetBValue(clrDark) / 2);

			pDC->FillRectangle(rc, pBackground);
		}
		else
		{
			clrBackground = GetSysColor(COLOR_3DFACE);
			clrLight	  = GetSysColor(COLOR_WINDOW);
			clrDark		  = GetSysColor(COLOR_BTNSHADOW);
			clrDarkDark   = GetSysColor(COLOR_3DDKSHADOW);

			CXTPMarkupSolidColorBrush brushBackground(clrBackground);
			pDC->FillRectangle(rc, &brushBackground);
		}

		CXTPMarkupSolidColorBrush brushDark(clrDark);

		if (bPressed)
		{
			CXTPMarkupSolidColorBrush brushFrame(GetSysColor(COLOR_WINDOWFRAME));

			pDC->DrawFrame(rc, &brushFrame);
			rc.DeflateRect(m_pMarkupContext->ScaleX(1), m_pMarkupContext->ScaleY(1));
			pDC->DrawFrame(rc, &brushDark);
		}
		else
		{
			CXTPMarkupSolidColorBrush brushLight(clrLight);
			CXTPMarkupSolidColorBrush brushDarkDark(clrDarkDark);

			pDC->FillRectangle(CRect(rc.left, rc.top, rc.right, m_pMarkupContext->ScaleY(1)),
							   &brushLight);
			pDC->FillRectangle(CRect(rc.left, rc.top, rc.left + m_pMarkupContext->ScaleX(1),
									 rc.bottom),
							   &brushLight);

			pDC->FillRectangle(CRect(rc.left + m_pMarkupContext->ScaleX(1),
									 rc.bottom - m_pMarkupContext->ScaleY(2),
									 rc.right - m_pMarkupContext->ScaleX(1),
									 rc.bottom - m_pMarkupContext->ScaleY(1)),
							   &brushDark);
			pDC->FillRectangle(CRect(rc.right - m_pMarkupContext->ScaleX(2),
									 rc.top + m_pMarkupContext->ScaleY(1),
									 rc.right - m_pMarkupContext->ScaleX(1),
									 rc.bottom - m_pMarkupContext->ScaleY(1)),
							   &brushDark);

			pDC->FillRectangle(CRect(rc.left, rc.bottom - m_pMarkupContext->ScaleY(1),
									 rc.right - m_pMarkupContext->ScaleX(1), rc.bottom),
							   &brushDarkDark);
			pDC->FillRectangle(CRect(rc.right - m_pMarkupContext->ScaleX(1), rc.top, rc.right,
									 rc.bottom),
							   &brushDarkDark);
		}
	}
}

void CXTPMarkupButton::OnRenderFocusVisual(CXTPMarkupDrawingContext* drawingContext)
{
	CRect rc(0, 0, GetRenderSize().cx, GetRenderSize().cy);
	CXTPMarkupDedicatedDC* pDDC = drawingContext->GetDeviceContext()->GetDedicatedDC(rc);

	rc.DeflateRect(m_pMarkupContext->ScaleX(3), m_pMarkupContext->ScaleY(3));
	DrawFocusRect(pDDC->GetDC(), rc);

	pDDC->Commit();
	delete pDDC;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupButton

BEGIN_DISPATCH_MAP(CXTPMarkupButton, CXTPMarkupButtonBase)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupButton, CXTPMarkupButtonBase)
	INTERFACE_PART(CXTPMarkupButton, XTPDIID_MarkupButton, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupButton, XTPDIID_MarkupButton)

#endif
