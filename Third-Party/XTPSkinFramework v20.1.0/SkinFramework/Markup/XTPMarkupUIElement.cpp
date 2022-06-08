// XTPMarkupUIElement.cpp: implementation of the CXTPMarkupUIElement class.
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

#include "stdafx.h"

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
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Transform/XTPMarkupRenderTransform.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupTransformationMatrix.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CXTPMarkupUIElement

CXTPMarkupDependencyProperty* CXTPMarkupUIElement::m_pClipToBoundsProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUIElement::m_pVisibilityProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUIElement::m_pFocusableProperty		  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUIElement::m_pIsEnabledProperty		  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUIElement::m_pRenderTransformProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUIElement::m_pOpacityProperty		  = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupUIElement, CXTPMarkupVisual);

void CXTPMarkupUIElement::RegisterMarkupClass()
{
	m_pClipToBoundsProperty = CXTPMarkupDependencyProperty::Register(
		L"ClipToBounds", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupUIElement));

	m_pVisibilityProperty = CXTPMarkupDependencyProperty::Register(
		L"Visibility", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupUIElement),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertVisibility,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pFocusableProperty = CXTPMarkupDependencyProperty::Register(
		L"Focusable", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupUIElement),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateFalseValue()));

	m_pIsEnabledProperty = CXTPMarkupDependencyProperty::Register(
		L"IsEnabled", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupUIElement),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateTrueValue()));

	m_pRenderTransformProperty = CXTPMarkupDependencyProperty::Register(
		L"RenderTransform", MARKUP_TYPE(CXTPMarkupRenderTransform),
		MARKUP_TYPE(CXTPMarkupUIElement));

	m_pOpacityProperty = CXTPMarkupDependencyProperty::Register(
		L"Opacity", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupUIElement),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsRender
												 | CXTPMarkupPropertyMetadata::flagInherited));
}

CXTPMarkupUIElement::CXTPMarkupUIElement()
{
	m_szDesiredSize = 0;
	m_rcFinalRect.SetRectEmpty();

	m_bNeverMeasured = TRUE;
	m_bMeasureDirty	 = FALSE;

	m_bNeverArranged = TRUE;
	m_bArrangeDirty	 = FALSE;

	m_szPreviousAvailableSize = 0;
	m_bArrangeInProgress	  = FALSE;
	m_bMeasureInProgress	  = FALSE;
}

CXTPMarkupUIElement::~CXTPMarkupUIElement()
{
}

CXTPMarkupVisual* CXTPMarkupVisual::GetVisualParent() const
{
	CXTPMarkupObject* pUIElement = m_pLogicalParent;

	while (pUIElement && !pUIElement->IsKindOf(MARKUP_TYPE(CXTPMarkupVisual)))
	{
		pUIElement = pUIElement->GetLogicalParent();
	}

	return (CXTPMarkupVisual*)pUIElement;
}

CXTPMarkupRenderTransform* CXTPMarkupUIElement::GetRenderTransform() const
{
	return MARKUP_STATICCAST(CXTPMarkupRenderTransform, GetValue(m_pRenderTransformProperty));
}

void CXTPMarkupUIElement::InvalidateArrange()
{
	if (!m_bArrangeInProgress && !m_bNeverArranged)
	{
		m_bArrangeDirty = TRUE;

		Arrange(m_rcFinalRect);
		InvalidateVisual();
	}
}

void CXTPMarkupUIElement::InvalidateMeasure()
{
	CXTPMarkupDrawingContext dc(GetMarkupContext());
	InvalidateMeasureOverride(&dc);
}

void CXTPMarkupUIElement::InvalidateMeasureOverride(CXTPMarkupDrawingContext* pDC)
{
	if (!m_bMeasureInProgress && !m_bNeverMeasured)
	{
		m_bMeasureDirty = TRUE;
		m_bArrangeDirty = TRUE;

		CSize sz = GetDesiredSize();

		Measure(pDC, m_szPreviousAvailableSize);

		if (sz != GetDesiredSize())
		{
			m_bMeasureDirty = TRUE;
			m_bArrangeDirty = TRUE;

			CXTPMarkupUIElement* pParent = MARKUP_DYNAMICCAST(CXTPMarkupUIElement,
															  GetVisualParent());
			if (pParent)
			{
				pParent->InvalidateMeasureOverride(pDC);
			}
			else if (m_pMarkupContext)
			{
				m_pMarkupContext->OnInvalidateArrange(this);
			}
		}
		else
		{
			Arrange(m_rcFinalRect);
			InvalidateVisual();
		}
	}
}

CXTPMarkupVisual::TransformationMatrices
	CXTPMarkupUIElement::ApplyRenderTransform(CXTPMarkupDrawingContext* pContext)
{
	CXTPMarkupVisual::TransformationMatrices matrices;

	CXTPMarkupRenderTransform* pRenderTransform = GetRenderTransform();
	if (NULL != pRenderTransform)
	{
		CXTPMarkupTransform* pTransform = pRenderTransform->GetTransformObject();
		if (NULL != pTransform)
		{
			matrices.pOldMatrix = pContext->GetTransformationMatrix();
			if (NULL != matrices.pOldMatrix)
			{
				matrices.pNewMatrix = pTransform->ApplyTransform(matrices.pOldMatrix);
				if (NULL != matrices.pNewMatrix)
				{
					pContext->SetTransformationMatrix(matrices.pNewMatrix);
					SetCurrentTransformationMatrix(matrices.pNewMatrix);
				}
			}
		}
	}

	return matrices;
}

void CXTPMarkupUIElement::RestoreRenderTransform(CXTPMarkupDrawingContext* pContext,
												 TransformationMatrices& matrices)
{
	if (NULL != matrices.pOldMatrix)
	{
		pContext->SetTransformationMatrix(matrices.pOldMatrix);
		delete matrices.pOldMatrix;
		matrices.pOldMatrix = NULL;
	}
}

void CXTPMarkupUIElement::InvalidateVisual()
{
	if (m_pMarkupContext)
		m_pMarkupContext->OnInvalidateVisual(this);
}

void CXTPMarkupUIElement::Arrange(CRect rcFinalRect)
{
	if (GetVisibility() == xtpMarkupVisibilityCollapsed)
	{
		m_rcFinalRect	 = rcFinalRect;
		m_bArrangeDirty	 = FALSE;
		m_bNeverArranged = FALSE;
		return;
	}

	if (m_bNeverMeasured)
	{
		CXTPMarkupDrawingContext dc(GetMarkupContext());
		Measure(&dc, rcFinalRect.Size());
	}

	if ((m_bArrangeDirty || m_bNeverArranged) || (rcFinalRect != m_rcFinalRect))
	{
		m_bNeverArranged	 = FALSE;
		m_bArrangeInProgress = TRUE;
		ArrangeCore(rcFinalRect);
		m_bArrangeInProgress = FALSE;

		UpdateBoundRect();
	}

	m_rcFinalRect	= rcFinalRect;
	m_bArrangeDirty = FALSE;
}

void CXTPMarkupUIElement::ArrangeCore(CRect rcFinalRect)
{
	m_szRenderSize = rcFinalRect.Size();
}

void CXTPMarkupUIElement::Measure(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	if (GetVisibility() == xtpMarkupVisibilityCollapsed)
	{
		m_bMeasureDirty			  = TRUE;
		m_bNeverMeasured		  = FALSE;
		m_szPreviousAvailableSize = szAvailableSize;
		m_szDesiredSize			  = CSize(0, 0);
		return;
	}

	if ((!m_bMeasureDirty && !m_bNeverMeasured) && (szAvailableSize == m_szPreviousAvailableSize)
		&& !pDC->IsPrinting())
	{
		return;
	}

	m_bNeverMeasured = FALSE;

	m_bArrangeDirty = TRUE;

	m_bMeasureInProgress = TRUE;
	CSize size			 = MeasureCore(pDC, szAvailableSize);
	m_bMeasureInProgress = FALSE;

	m_szDesiredSize			  = size;
	m_szPreviousAvailableSize = szAvailableSize;
	m_bMeasureDirty			  = pDC->IsPrinting() ? TRUE : FALSE;
}

CSize CXTPMarkupUIElement::MeasureCore(CXTPMarkupDrawingContext* /*pDC*/, CSize /*szAvailableSize*/)
{
	return CSize(0, 0);
}

BOOL CXTPMarkupUIElement::GetLayoutClip(CRect& rc) const
{
	if (GetVisibility() != xtpMarkupVisibilityVisible)
	{
		rc.SetRectEmpty();
		return TRUE;
	}
	return FALSE;
}

double CXTPMarkupUIElement::NormalizeOpacityValue(double dValue)
{
	return (dValue < 0 ? 0. : (1 < dValue ? 1. : dValue));
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupUIElement, CXTPMarkupVisual)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "ClipToBounds", 300, OleGetClipToBounds,
						OleSetClipToBounds, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "Visibility", 301, OleGetVisibility, OleSetVisibility,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "Focusable", 302, OleGetFocusable, OleSetFocusable,
						VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "IsEnabled", 303, OleGetIsEnabled, OleSetIsEnabled,
						VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "RenderTransform", 304, OleGetRenderTransform,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupUIElement, "Opacity", 305, OleGetOpacity, OleSetOpacity, VT_R8)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupUIElement, CXTPMarkupVisual)
	INTERFACE_PART(CXTPMarkupUIElement, XTPDIID_MarkupUIElement, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupUIElement, XTPDIID_MarkupUIElement)
#endif

BOOL CXTPMarkupUIElement::OleGetClipToBounds()
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool, GetValue(m_pClipToBoundsProperty));
	return pValue ? (BOOL)*pValue : FALSE;
}

void CXTPMarkupUIElement::OleSetClipToBounds(BOOL bValue)
{
	SetValue(m_pClipToBoundsProperty, CXTPMarkupBool::CreateValue(bValue));
}

long CXTPMarkupUIElement::OleGetVisibility()
{
	return GetVisibility();
}

void CXTPMarkupUIElement::OleSetVisibility(long nValue)
{
	SetVisibility((XTPMarkupVisibility)nValue);
}

BOOL CXTPMarkupUIElement::OleGetFocusable()
{
	return IsFocusable();
}

void CXTPMarkupUIElement::OleSetFocusable(BOOL bValue)
{
	SetValue(m_pFocusableProperty, CXTPMarkupBool::CreateValue(bValue));
}

BOOL CXTPMarkupUIElement::OleGetIsEnabled()
{
	return IsEnabled();
}

void CXTPMarkupUIElement::OleSetIsEnabled(BOOL bValue)
{
	SetValue(m_pIsEnabledProperty, CXTPMarkupBool::CreateValue(bValue));
}

LPDISPATCH CXTPMarkupUIElement::OleGetRenderTransform()
{
	LPDISPATCH lpDispTransform					= NULL;
	CXTPMarkupRenderTransform* pRenderTransform = GetRenderTransform();
	if (NULL != pRenderTransform)
	{
		CXTPMarkupTransform* pTransformObject = pRenderTransform->GetTransformObject();
		if (NULL != pTransformObject)
		{
			lpDispTransform = XTPGetDispatch(pTransformObject);
		}
	}

	return lpDispTransform;
}

XTPMarkupVisibility CXTPMarkupUIElement::GetVisibility() const
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pVisibilityProperty));
	return pValue ? (XTPMarkupVisibility)(int)*pValue : xtpMarkupVisibilityVisible;
}

void CXTPMarkupUIElement::SetVisibility(XTPMarkupVisibility visibility)
{
	SetValue(m_pVisibilityProperty, CXTPMarkupEnum::CreateValue(visibility));
}

BOOL CXTPMarkupUIElement::IsFocusable() const
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool, GetValue(m_pFocusableProperty));
	return pValue ? (BOOL)*pValue : FALSE;
}

BOOL CXTPMarkupUIElement::IsVisible() const
{
	return GetVisibility() == xtpMarkupVisibilityVisible;
}

BOOL CXTPMarkupUIElement::IsEnabled() const
{
	CXTPMarkupBool* pValue = MARKUP_STATICCAST(CXTPMarkupBool, GetValue(m_pIsEnabledProperty));
	return pValue ? (BOOL)*pValue : TRUE;
}

double CXTPMarkupUIElement::GetOpacity() const
{
	CXTPMarkupDouble* pValue = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pOpacityProperty));
	return NULL != pValue ? NormalizeOpacityValue((double)*pValue) : 1.;
}

double CXTPMarkupUIElement::OleGetOpacity()
{
	return GetOpacity();
}

void CXTPMarkupUIElement::OleSetOpacity(double dValue)
{
	SetValue(m_pOpacityProperty, new CXTPMarkupDouble(NormalizeOpacityValue(dValue)));
}
