// XTPMarkupPath.cpp: implementation of the CXTPMarkupPath class.
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
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Shapes/XTPMarkupPath.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/Shapes/XTPMarkupPathData.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPath

CXTPMarkupDependencyProperty* CXTPMarkupPath::m_pPropertyData = NULL;

IMPLEMENT_MARKUPCLASS(L"Path", CXTPMarkupPath, CXTPMarkupShape)

void CXTPMarkupPath::RegisterMarkupClass()
{
	m_pPropertyData = CXTPMarkupDependencyProperty::Register(
		L"Data", MARKUP_TYPE(CXTPMarkupPathData), MARKUP_TYPE(CXTPMarkupPath),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupPath::CXTPMarkupPath()
{
}

CXTPMarkupPath::~CXTPMarkupPath()
{
}

void CXTPMarkupPath::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	UNREFERENCED_PARAMETER(pBuilder);
	ASSERT(NULL != pContent);

	if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupPathData)))
	{
		SetValue(m_pPropertyData, pContent);
		MARKUP_ADDREF(pContent);
	}
	else
	{
		CXTPMarkupObject::SetContentObject(pBuilder, pContent);
	}
}

CXTPMarkupPathData* CXTPMarkupPath::GetData() const
{
	return MARKUP_STATICCAST(CXTPMarkupPathData, GetValue(m_pPropertyData));
}

CSize CXTPMarkupPath::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	UNREFERENCED_PARAMETER(pDC);

	CXTPMarkupPathData* pData = GetData();

	if (NULL == pData)
	{
		return CSize(0, 0);
	}

	CRect rcBounds = pData->GetBounds();

	CSize szBound(max(rcBounds.Width(), rcBounds.right), max(rcBounds.Height(), rcBounds.bottom));

	if (GetStretch() != xtpMarkupStretchNone)
	{
		if (szAvailableSize.cx < INT_MAX)
			szBound.cx = szAvailableSize.cx;

		if (szAvailableSize.cy < INT_MAX)
			szBound.cy = szAvailableSize.cy;
	}
	return szBound;
}

void CXTPMarkupPath::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CXTPMarkupStrokeStyle strokeStyle;
	GetStrokeStyle(&strokeStyle);

	CXTPMarkupBrush* pFillBrush = GetFill();

	if (!strokeStyle.pStrokeBrush && !pFillBrush)
		return;

	CXTPMarkupPathData* pData = GetData();
	if (!pData)
		return;

	if (GetStretch() != xtpMarkupStretchNone)
	{
		CXTPMarkupPathData* pGeometryStretch = pData->Stretch(GetRenderSize());
		pDC->DrawPath(pGeometryStretch, &strokeStyle, pFillBrush);
		delete pGeometryStretch;
	}
	else
	{
		pDC->DrawPath(pData, &strokeStyle, pFillBrush);
	}
}

/////////////////////////////////////////////////////////////////////////
// Path

BEGIN_DISPATCH_MAP(CXTPMarkupPath, CXTPMarkupShape)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupPath, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupPath, XTPDIID_MarkupPath, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupPath, XTPDIID_MarkupPath)
#endif
