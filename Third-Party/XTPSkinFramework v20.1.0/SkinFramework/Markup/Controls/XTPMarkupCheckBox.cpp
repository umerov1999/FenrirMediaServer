// XTPMarkupCheckBox.cpp : implementation file
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupButtonBase.h"
#include "Markup/Controls/XTPMarkupToggleButton.h"
#include "Markup/Controls/XTPMarkupCheckBox.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupCheckBox

IMPLEMENT_MARKUPCLASS(L"CheckBox", CXTPMarkupCheckBox, CXTPMarkupToggleButton)

void CXTPMarkupCheckBox::RegisterMarkupClass()
{
}

CXTPMarkupCheckBox::CXTPMarkupCheckBox()
{
}

CXTPMarkupCheckBox::~CXTPMarkupCheckBox()
{
}

CSize CXTPMarkupCheckBox::ArrangeOverride(CSize szFinalSize)
{
	CXTPMarkupUIElement* pContent = GetContent();
	if (pContent != NULL)
	{
		CRect rt(0, 0, szFinalSize.cx, szFinalSize.cy);

		CRect finalRect = CXTPMarkupThickness::HelperDeflateRect(rt, GetPadding());
		finalRect.DeflateRect(m_pMarkupContext->ScaleX(13 + 3), 0, 0, 0);
		pContent->Arrange(finalRect);
	}
	return szFinalSize;
}

CSize CXTPMarkupCheckBox::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	m_themeButton->OpenThemeData(NULL, L"BUTTON");

	CSize size3 = GetPadding()->GetSize();
	size3.cx += m_pMarkupContext->ScaleX(13 + 3);

	CXTPMarkupUIElement* pContent = GetContent();
	if (pContent != NULL)
	{
		CSize availableSize(max(0, szAvailableSize.cx - size3.cx),
							max(0, szAvailableSize.cy - size3.cy));
		pContent->Measure(pDC, availableSize);

		CSize desiredSize = pContent->GetDesiredSize();

		return CSize(desiredSize.cx + size3.cx,
					 max(m_pMarkupContext->ScaleY(13), desiredSize.cy + size3.cy));
	}

	return size3;
}

void CXTPMarkupCheckBox::OnRender(CXTPMarkupDrawingContext* pDC)
{
	BOOL bPressed = IsPressed();
	BOOL bHot	  = IsMouseOver();
	BOOL bEnabled = IsEnabled();
	BOOL bChecked = GetChecked();

	CRect rc(0, GetRenderSize().cy / 2 - m_pMarkupContext->ScaleY(6), m_pMarkupContext->ScaleX(13),
			 GetRenderSize().cy / 2 + m_pMarkupContext->ScaleY(7));

	CXTPMarkupDedicatedDC* pDDC = pDC->GetDeviceContext()->GetDedicatedDC(rc);
	rc.OffsetRect(pDDC->GetCorrectionOffset());

	if (m_themeButton->IsAppThemeActive())
	{
		int nState = !bEnabled	? CBS_UNCHECKEDDISABLED
					 : bPressed ? CBS_UNCHECKEDPRESSED
					 : bHot		? CBS_UNCHECKEDHOT
								: CBS_UNCHECKEDNORMAL;
		if (bChecked == BST_CHECKED)
			nState += 4;
		if (bChecked == BST_INDETERMINATE)
			nState += 8;

		m_themeButton->DrawThemeBackground(pDDC->GetDC(), BP_CHECKBOX, nState, rc, NULL);
	}
	else
	{
		DrawFrameControl(pDDC->GetDC(), rc, DFC_BUTTON,
						 UINT(DFCS_BUTTONCHECK | (bPressed ? DFCS_PUSHED : 0)
							  | (bChecked == BST_CHECKED ? DFCS_CHECKED : 0)
							  | (bChecked == BST_INDETERMINATE ? DFCS_BUTTON3STATE | DFCS_CHECKED
															   : 0)));
	}

	pDDC->Commit();
	delete pDDC;
}

void CXTPMarkupCheckBox::OnRenderFocusVisual(CXTPMarkupDrawingContext* drawingContext)
{
	CXTPMarkupVisual* pContent = GetContent();
	if (!pContent)
		return;

	CRect rc(0, 0, GetRenderSize().cx, GetRenderSize().cy);
	CXTPMarkupDedicatedDC* pDDC = drawingContext->GetDeviceContext()->GetDedicatedDC(rc);

	rc.left += m_pMarkupContext->ScaleX(13 + 2);
	DrawFocusRect(pDDC->GetDC(), rc);

	pDDC->Commit();
	delete pDDC;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupCheckBox

BEGIN_DISPATCH_MAP(CXTPMarkupCheckBox, CXTPMarkupToggleButton)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupCheckBox, CXTPMarkupToggleButton)
	INTERFACE_PART(CXTPMarkupCheckBox, XTPDIID_MarkupCheckBox, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupCheckBox, XTPDIID_MarkupCheckBox)

#endif
