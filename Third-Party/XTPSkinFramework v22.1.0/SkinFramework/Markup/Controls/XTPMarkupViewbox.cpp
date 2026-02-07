// XTPMarkupViewbox.cpp: implementation of the CXTPMarkupViewbox class.
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
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Controls/XTPMarkupDecorator.h"
#include "Markup/Controls/XTPMarkupViewbox.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupTransformationMatrix.h"
#include "Markup/Transform/XTPMarkupRenderTransform.h"
#include "Markup/Transform/XTPMarkupScaleTransform.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/Transform/XTPMarkupGdiPlusTransformationMatrix.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformObject.h"
#include "Markup/Transform/XTPMarkupGdiPlusScaleTransform.h"

#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXTPMarkupDependencyProperty* CXTPMarkupViewbox::m_pStretchProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"Viewbox", CXTPMarkupViewbox, CXTPMarkupFrameworkElement);

void CXTPMarkupViewbox::RegisterMarkupClass()
{
	m_pStretchProperty = CXTPMarkupDependencyProperty::Register(
		L"Stretch", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupViewbox),
		new CXTPMarkupPropertyMetadata(CXTPMarkupEnum::CreateValue(xtpMarkupStretchNone),
									   &CXTPMarkupBuilder::ConvertStretch,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupViewbox::CXTPMarkupViewbox()
	: m_scaleX(0)
	, m_scaleY(0)
	, m_centerX(0)
	, m_centerY(0)
{
}

CXTPMarkupViewbox::~CXTPMarkupViewbox()
{
}

CSize CXTPMarkupViewbox::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	ASSERT(pDC);

	if (m_pChild != NULL)
	{
		m_pChild->Measure(pDC, szAvailableSize);
		return m_pChild->GetDesiredSize();
	}

	return CSize();
}

BOOL CXTPMarkupViewbox::HasElementSize()
{
	CXTPMarkupInt* pWidthProperty  = NULL;
	CXTPMarkupInt* pHeightProperty = NULL;

	pWidthProperty  = MARKUP_STATICCAST(CXTPMarkupInt,
										GetValue(CXTPMarkupFrameworkElement::m_pWidthProperty));
	pHeightProperty = MARKUP_STATICCAST(CXTPMarkupInt,
										GetValue(CXTPMarkupFrameworkElement::m_pHeightProperty));

	return pWidthProperty && pHeightProperty;
}

BOOL CXTPMarkupViewbox::ChildHasElementSize()
{
	CXTPMarkupInt* pWidthProperty  = NULL;
	CXTPMarkupInt* pHeightProperty = NULL;

	if (m_pChild != NULL)
	{
		pWidthProperty  = MARKUP_STATICCAST(CXTPMarkupInt,
											m_pChild->GetValue(
												CXTPMarkupFrameworkElement::m_pWidthProperty));
		pHeightProperty = MARKUP_STATICCAST(CXTPMarkupInt,
											m_pChild->GetValue(
												CXTPMarkupFrameworkElement::m_pHeightProperty));
	}

	return pWidthProperty && pHeightProperty;
}

CSize CXTPMarkupViewbox::GetElementSize()
{
	CXTPMarkupInt* pWidthProperty  = NULL;
	CXTPMarkupInt* pHeightProperty = NULL;

	pWidthProperty  = MARKUP_STATICCAST(CXTPMarkupInt,
										GetValue(CXTPMarkupFrameworkElement::m_pWidthProperty));
	pHeightProperty = MARKUP_STATICCAST(CXTPMarkupInt,
										GetValue(CXTPMarkupFrameworkElement::m_pHeightProperty));

	return CSize(XTP_SAFE_GET1(pWidthProperty, GetValue(), 0),
				 XTP_SAFE_GET1(pHeightProperty, GetValue(), 0));
}

CSize CXTPMarkupViewbox::ChildGetElementSize()
{
	CXTPMarkupInt* pWidthProperty  = NULL;
	CXTPMarkupInt* pHeightProperty = NULL;

	if (m_pChild != NULL)
	{
		pWidthProperty  = MARKUP_STATICCAST(CXTPMarkupInt,
											m_pChild->GetValue(
												CXTPMarkupFrameworkElement::m_pWidthProperty));
		pHeightProperty = MARKUP_STATICCAST(CXTPMarkupInt,
											m_pChild->GetValue(
												CXTPMarkupFrameworkElement::m_pHeightProperty));
	}

	return CSize(XTP_SAFE_GET1(pWidthProperty, GetValue(), 0),
				 XTP_SAFE_GET1(pHeightProperty, GetValue(), 0));
}

CSize CXTPMarkupViewbox::ArrangeOverride(CSize szFinalSize)
{
	if (m_pChild != NULL)
	{
		if (szFinalSize.cx == 0 || szFinalSize.cy == 0)
			return szFinalSize;

		const CSize szProperty = ChildHasElementSize() ? ChildGetElementSize()
													   : m_pChild->GetDesiredSize();
		if (szProperty.cx == 0 || szProperty.cy == 0)
			return szFinalSize;

		const CRect rcParent   = CRect(0, 0, szFinalSize.cx, szFinalSize.cy);
		const CRect rcProperty = CRect(0, 0, szProperty.cx, szProperty.cy);
		CRect rcChild;
		BOOL bClip = FALSE;

		XTPMarkupStretch stretch = GetStretch();
		switch (stretch)
		{
			case xtpMarkupStretchNone:
			{
				rcChild = rcProperty;

				if (HasElementSize()
					&& (GetElementSize().cx < rcChild.Width()
						|| GetElementSize().cy < rcChild.Height()))
				{
					bClip = TRUE;
				}

				break;
			}

			case xtpMarkupStretchFill:
			{
				rcChild = rcParent;
				break;
			}

			case xtpMarkupStretchUniform:
			{
				// get Parent square with minimal side
				rcChild = CRect(0, 0, CXTP_min(rcParent.Width(), rcParent.Height()),
								CXTP_min(rcParent.Width(), rcParent.Height()));

				// square has same width as Parent
				if (rcChild.Width() == rcParent.Width())
				{
					// calculate Child height with aspect ratio of rcProperty
					rcChild.bottom = rcProperty.Height() * rcChild.Width() / rcProperty.Width();

					// if calculated Child height more than Parent height
					if (rcChild.Height() > rcParent.Height())
					{
						// correct control size
						rcChild.bottom = rcParent.Height();
						rcChild.right = rcProperty.Width() * rcChild.Height() / rcProperty.Height();
					}
				}
				if (rcChild.Height() == rcParent.Height())
				{
					rcChild.right = rcProperty.Width() * rcChild.Height() / rcProperty.Height();
					if (rcChild.Width() > rcParent.Width())
					{
						rcChild.right  = rcParent.Width();
						rcChild.bottom = rcProperty.Height() * rcChild.Width() / rcProperty.Width();
					}
				}

				// There we have child size, which can be inscribed in parent size
				break;
			}

			case xtpMarkupStretchUniformToFill:
			{
				rcChild = CRect(0, 0, CXTP_max(rcParent.Width(), rcParent.Height()),
								CXTP_max(rcParent.Width(), rcParent.Height()));

				if (rcChild.Width() == rcParent.Width())
				{
					rcChild.bottom = rcProperty.Height() * rcChild.Width() / rcProperty.Width();
					if (rcChild.Height() < rcParent.Height())
					{
						rcChild.bottom = rcParent.Height();
						rcChild.right = rcProperty.Width() * rcChild.Height() / rcProperty.Height();
					}
				}
				if (rcChild.Height() == rcParent.Height())
				{
					rcChild.right = rcProperty.Width() * rcChild.Height() / rcProperty.Height();
					if (rcChild.Width() < rcParent.Width())
					{
						rcChild.right  = rcParent.Width();
						rcChild.bottom = rcProperty.Height() * rcChild.Width() / rcProperty.Width();
					}
				}

				bClip = TRUE;

				break;
			}

			default: ASSERT(FALSE); break;
		}

		m_scaleX  = static_cast<float>(rcChild.Width()) / static_cast<float>(rcProperty.Width());
		m_scaleY  = static_cast<float>(rcChild.Height()) / static_cast<float>(rcProperty.Height());
		m_centerX = static_cast<float>(rcParent.Width()) / 2
					- static_cast<float>(rcChild.Width()) / 2;
		m_centerY = static_cast<float>(rcParent.Height()) / 2
					- static_cast<float>(rcChild.Height()) / 2;

		SetClipToBounds(bClip);

		rcChild = rcProperty;

		m_pChild->Arrange(rcChild);
	}

	return szFinalSize;
}

CXTPMarkupVisual::TransformationMatrices
	CXTPMarkupViewbox::ApplyRenderTransform(CXTPMarkupDrawingContext* pContext)
{
	CXTPMarkupVisual::TransformationMatrices matrices;

	matrices.pOldMatrix = pContext->GetTransformationMatrix();
	if (NULL != matrices.pOldMatrix)
	{
		matrices.pNewMatrix = ApplyTransform(matrices.pOldMatrix);
		if (NULL != matrices.pNewMatrix)
		{
			pContext->SetTransformationMatrix(matrices.pNewMatrix);
			SetCurrentTransformationMatrix(matrices.pNewMatrix);
		}
	}

	return matrices;
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupViewbox::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	ASSERT_KINDOF(CXTPMarkupGdiPlusTransformationMatrix, pMatrix);

	const CXTPMarkupGdiPlusTransformationMatrix* pSourceMatrix =
		static_cast<const CXTPMarkupGdiPlusTransformationMatrix*>(pMatrix);

	CXTPMarkupGdiPlusTransformationMatrix* pResultMatrix = NULL;
	GpMatrix* pGpMatrix									 = NULL;
	if (GdipCloneMatrix(const_cast<GpMatrix*>(pSourceMatrix->GetMatrix()), &pGpMatrix)
		== Gdiplus::Ok)
	{
		if (GdipTranslateMatrix(pGpMatrix, m_centerX, m_centerY, Gdiplus::MatrixOrderPrepend)
			== Gdiplus::Ok)
		{
			if (GdipScaleMatrix(pGpMatrix, m_scaleX, m_scaleY, Gdiplus::MatrixOrderPrepend)
				== Gdiplus::Ok)
			{
				pResultMatrix = new CXTPMarkupGdiPlusTransformationMatrix(NULL, pGpMatrix);
			}
			else
			{
				GdipDeleteMatrix(pGpMatrix);
			}
		}
	}

	return pResultMatrix;
}

void CXTPMarkupViewbox::SetStretch(XTPMarkupStretch stretch)
{
	SetValue(m_pStretchProperty, CXTPMarkupEnum::CreateValue(stretch));
}

XTPMarkupStretch CXTPMarkupViewbox::GetStretch() const
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pStretchProperty));
	return pValue != NULL ? (XTPMarkupStretch)(int)*pValue : xtpMarkupStretchNone;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupViewbox, CXTPMarkupDecorator)
	DISP_PROPERTY_EX_ID(CXTPMarkupViewbox, "Stretch", 501, OleGetStretch, OleSetStretch, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupViewbox, "Child", 502, OleGetChild, OleSetChild, VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupViewbox, CXTPMarkupDecorator)
	INTERFACE_PART(CXTPMarkupViewbox, XTPDIID_MarkupBorder, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupViewbox, XTPDIID_MarkupBorder)
#endif

long CXTPMarkupViewbox::OleGetStretch()
{
	return GetStretch();
}

void CXTPMarkupViewbox::OleSetStretch(long stretch)
{
	SetStretch((XTPMarkupStretch)stretch);
}

LPDISPATCH CXTPMarkupViewbox::OleGetChild()
{
	return XTPGetDispatch(GetChild());
}

void CXTPMarkupViewbox::OleSetChild(LPDISPATCH lpDisp)
{
	SetChild(MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpDisp)));
}
