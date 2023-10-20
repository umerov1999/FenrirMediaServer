// XTPMarkupGdiPlusMatrixTransform.cpp: implementation of the CXTPMarkupGdiPlusMatrixTransform
// class.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/XTPMarkupObject.h"
#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupRenderTransform.h"
#include "Markup/Transform/XTPMarkupMatrixTransform.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformObject.h"
#include "Markup/Transform/XTPMarkupGdiPlusMatrixTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_MARKUPCLASS(L"MatrixTransform", CXTPMarkupGdiPlusMatrixTransform,
					  CXTPMarkupMatrixTransform);

CXTPMarkupGdiPlusMatrixTransform::CXTPMarkupGdiPlusMatrixTransform()
{
}

void CXTPMarkupGdiPlusMatrixTransform::RegisterMarkupClass()
{
}

BOOL CXTPMarkupGdiPlusMatrixTransform::TransformMatrix(const CXTPGdiPlus* pReserved,
													   GpMatrix* pMatrix)
{
	UNREFERENCED_PARAMETER(pReserved);

	ASSERT(NULL != pMatrix);

	const int matrixValues				= 6;
	CXTPMarkupDoubleCollection* pValues = GetMatrix();
	if (pValues->GetCount() < matrixValues || matrixValues > pValues->GetCount())
		return FALSE;

	Gdiplus::REAL scaleX  = (Gdiplus::REAL)pValues->GetAt(0); // M11
	Gdiplus::REAL scaleY  = (Gdiplus::REAL)pValues->GetAt(3); // M22
	Gdiplus::REAL shearY  = (Gdiplus::REAL)pValues->GetAt(1); // M12
	Gdiplus::REAL shearX  = (Gdiplus::REAL)pValues->GetAt(2); // M21
	Gdiplus::REAL offsetX = (Gdiplus::REAL)pValues->GetAt(4); // OffsetX
	Gdiplus::REAL offsetY = (Gdiplus::REAL)pValues->GetAt(5); // OffsetY
	Gdiplus::REAL originX = (Gdiplus::REAL)m_pRenderTransform->GetOriginX();
	Gdiplus::REAL originY = (Gdiplus::REAL)m_pRenderTransform->GetOriginY();

	if (Gdiplus::Ok != GdipTranslateMatrix(pMatrix, originX, originY, Gdiplus::MatrixOrderPrepend))
		return FALSE;

	if (Gdiplus::Ok != GdipTranslateMatrix(pMatrix, offsetX, offsetY, Gdiplus::MatrixOrderPrepend))
		return FALSE;

	if (Gdiplus::Ok != GdipScaleMatrix(pMatrix, scaleX, scaleY, Gdiplus::MatrixOrderPrepend))
		return FALSE;

	if (Gdiplus::Ok != GdipShearMatrix(pMatrix, shearX, shearY, Gdiplus::MatrixOrderPrepend))
		return FALSE;

	if (Gdiplus::Ok
		!= GdipTranslateMatrix(pMatrix, -originX, -originY, Gdiplus::MatrixOrderPrepend))
		return FALSE;

	return TRUE;
}
