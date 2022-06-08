// XTPMarkupGdiPlusScaleTransform.cpp: implementation of the CXTPMarkupGdiPlusScaleTransform class.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPFramework.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/XTPMarkupObject.h"
#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupScaleTransform.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformObject.h"
#include "Markup/Transform/XTPMarkupGdiPlusScaleTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_MARKUPCLASS(L"ScaleTransform", CXTPMarkupGdiPlusScaleTransform, CXTPMarkupScaleTransform);

CXTPMarkupGdiPlusScaleTransform::CXTPMarkupGdiPlusScaleTransform()
{
}

void CXTPMarkupGdiPlusScaleTransform::RegisterMarkupClass()
{
}

BOOL CXTPMarkupGdiPlusScaleTransform::TransformMatrix(const CXTPGdiPlus* pReserved,
													  GpMatrix* pMatrix)
{
	UNREFERENCED_PARAMETER(pReserved);

	ASSERT(NULL != pMatrix);

	BOOL bSuccess		  = FALSE;
	Gdiplus::REAL scaleX  = (Gdiplus::REAL)GetScaleX();
	Gdiplus::REAL scaleY  = (Gdiplus::REAL)GetScaleY();
	Gdiplus::REAL centerX = (Gdiplus::REAL)GetCenterX();
	Gdiplus::REAL centerY = (Gdiplus::REAL)GetCenterY();

	if (Gdiplus::Ok == GdipTranslateMatrix(pMatrix, centerX, centerY, Gdiplus::MatrixOrderPrepend))
	{
		if (Gdiplus::Ok == GdipScaleMatrix(pMatrix, scaleX, scaleY, Gdiplus::MatrixOrderPrepend))
		{
			bSuccess = TRUE;
		}
	}

	return bSuccess;
}
