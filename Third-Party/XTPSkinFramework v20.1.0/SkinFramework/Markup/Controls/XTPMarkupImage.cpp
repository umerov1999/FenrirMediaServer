// XTPMarkupImage.cpp: implementation of the CXTPMarkupImage class.
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
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPResourceManager.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceDependentImage.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Controls/XTPMarkupImage.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

CXTPMarkupDependencyProperty* CXTPMarkupImage::m_pSourceProperty  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupImage::m_pStretchProperty = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_MARKUPCLASS(L"Image", CXTPMarkupImage, CXTPMarkupFrameworkElement)

void CXTPMarkupImage::RegisterMarkupClass()
{
	m_pSourceProperty = CXTPMarkupDependencyProperty::Register(
		L"Source", MARKUP_TYPE(CXTPMarkupString), MARKUP_TYPE(CXTPMarkupImage),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagAffectsArrange
										   | CXTPMarkupPropertyMetadata::flagAffectsParentMeasure
										   | CXTPMarkupPropertyMetadata::flagAffectsParentArrange
										   | CXTPMarkupPropertyMetadata::flagAffectsRender));

	m_pStretchProperty = CXTPMarkupDependencyProperty::Register(
		L"Stretch", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupImage),
		new CXTPMarkupPropertyMetadata(CXTPMarkupEnum::CreateValue(xtpMarkupStretchNone),
									   &CXTPMarkupBuilder::ConvertStretch,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupImage::CXTPMarkupImage()
	: m_pDeviceImage(NULL)
{
}

CXTPMarkupImage::~CXTPMarkupImage()
{
	SAFE_DELETE(m_pDeviceImage);
}

CString CXTPMarkupImage::GetSource()
{
	CXTPMarkupString* pSource = MARKUP_STATICCAST(CXTPMarkupString, GetValue(m_pSourceProperty));
	return CString(NULL != pSource ? XTP_CW2CT((LPCWSTR)*pSource) : _T(""));
}

void CXTPMarkupImage::SetSource(LPCTSTR lpszSource)
{
	SetValue(m_pSourceProperty, new CXTPMarkupString(lpszSource));
}

AFX_INLINE BOOL IsPositiveInfinity(int size)
{
	return size > 32000;
}

CSize CXTPMarkupImage::GetOriginalSize() const
{
	CSize szRet(0, 0);

	if (NULL != m_pDeviceImage)
	{
		szRet = m_pDeviceImage->GetSize();
		if (NULL != m_pMarkupContext)
		{
			szRet = m_pMarkupContext->Scale(szRet);
		}
	}

	return szRet;
}

void CXTPMarkupImage::CreateImageInstance(CXTPMarkupDrawingContext* pDC)
{
	ASSERT(NULL != pDC);

	if (NULL != m_pDeviceImage)
	{
		delete m_pDeviceImage;
		m_pDeviceImage = NULL;
	}

	if (NULL == m_pDeviceImage)
	{
		const CString& strSource = GetSource();
		if (!strSource.IsEmpty())
		{
			m_pDeviceImage	   = pDC->GetDeviceContext()->CreateDeviceDependentImage(this);
			LPCTSTR lpszSource = strSource;
			m_pDeviceImage->Load(XTP_CT2CW(lpszSource), m_ConstraintSize.cx);
		}
	}
}

CSize CXTPMarkupImage::MeasureArrangeHelper(CSize availableSize)
{
	CSize size(0, 0);
	if (NULL != m_pDeviceImage)
	{
		size = m_pDeviceImage->GetSize();
		if (NULL != m_pMarkupContext)
		{
			size = m_pMarkupContext->Scale(size);
		}

		if (!(size.cx == 0 || size.cy == 0))
		{
			XTPMarkupStretch stretch = GetStretch();

			BOOL bWidth	 = !IsPositiveInfinity(availableSize.cx);
			BOOL bHeight = !IsPositiveInfinity(availableSize.cy);

			if (!((stretch == xtpMarkupStretchNone) || (!bWidth && !bHeight)))
			{
				double cx = (double)availableSize.cx / size.cx;
				double cy = (double)availableSize.cy / size.cy;

				if (!bWidth)
				{
					cx = cy;
				}
				else if (!bHeight)
				{
					cy = cx;
				}
				else
				{
					switch (stretch)
					{
						case xtpMarkupStretchUniform: cx = cy = min(cx, cy); break;

						case xtpMarkupStretchUniformToFill: cx = cy = max(cx, cy); break;
					}
				}

				size.cx = int(size.cx * cx);
				size.cy = int(size.cy * cy);
			}
		}
	}

	return size;
}

CSize CXTPMarkupImage::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize constraint)
{
	if (m_ConstraintSize != constraint || NULL == m_pDeviceImage)
	{
		m_ConstraintSize = constraint;
		CreateImageInstance(pDC);
	}

	return MeasureArrangeHelper(constraint);
}

CSize CXTPMarkupImage::ArrangeOverride(CSize arrangeSize)
{
	return MeasureArrangeHelper(arrangeSize);
}

void CXTPMarkupImage::OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
										CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue)
{
	if (pProperty == m_pSourceProperty && NULL != m_pDeviceImage)
	{
		delete m_pDeviceImage;
		m_pDeviceImage = NULL;
	}

	CXTPMarkupFrameworkElement::OnPropertyChanged(pProperty, pOldValue, pNewValue);
}

void CXTPMarkupImage::OnRender(CXTPMarkupDrawingContext* pDC)
{
	if (NULL == m_pDeviceImage)
	{
		CreateImageInstance(pDC);
	}

	if (NULL != m_pDeviceImage)
	{
		CSize szRender = GetRenderSize();

		pDC->DrawImage(m_pDeviceImage, CRect(0, 0, szRender.cx, szRender.cy));
	}
}

// IXTPMarkupDeviceDependentImageSite overrides

CXTPMarkupContext* CXTPMarkupImage::GetMarkupContext()
{
	return m_pMarkupContext;
}

void CXTPMarkupImage::OnImageUpdateRequired()
{
	InvalidateVisual();
}

XTPMarkupStretch CXTPMarkupImage::GetStretch() const
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pStretchProperty));
	return pValue != NULL ? (XTPMarkupStretch)(int)*pValue : xtpMarkupStretchNone;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupImage, CXTPMarkupFrameworkElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupImage, "Source", 500, OleGetSource, OleSetSource, VT_BSTR)
	DISP_PROPERTY_EX_ID(CXTPMarkupImage, "Stretch", 501, OleGetStretch, OleSetStretch, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupImage, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupImage, XTPDIID_MarkupImage, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupImage, XTPDIID_MarkupImage)
#endif

void CXTPMarkupImage::OleSetSource(LPCTSTR lpszSource)
{
	SetSource(lpszSource);
}

BSTR CXTPMarkupImage::OleGetSource()
{
	return GetSource().AllocSysString();
}

long CXTPMarkupImage::OleGetStretch()
{
	return GetStretch();
}

void CXTPMarkupImage::OleSetStretch(long stretch)
{
	SetStretch((XTPMarkupStretch)stretch);
}
