// XTPMarkupGdiPlusTransformObject.cpp: implementation of the CXTPMarkupGdiPlusTransformObjectImpl
// class.
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

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/Transform/XTPMarkupTransformationMatrix.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformationMatrix.h"
#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformObject.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

CXTPMarkupTransformationMatrix* CXTPMarkupGdiPlusTransformObjectImpl ::ApplyTransform(
	const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	ASSERT_KINDOF(CXTPMarkupGdiPlusTransformationMatrix, pMatrix);

	const CXTPMarkupGdiPlusTransformationMatrix* pSourceMatrix =
		static_cast<const CXTPMarkupGdiPlusTransformationMatrix*>(pMatrix);

	CXTPMarkupGdiPlusTransformationMatrix* pResultMatrix = NULL;
	GpMatrix* pGpMatrix									 = NULL;
	if (Gdiplus::Ok
		== GdipCloneMatrix(const_cast<GpMatrix*>(pSourceMatrix->GetMatrix()), &pGpMatrix))
	{
		if (TransformMatrix(NULL, pGpMatrix))
		{
			pResultMatrix = new CXTPMarkupGdiPlusTransformationMatrix(NULL, pGpMatrix);
		}
		else
		{
			GdipDeleteMatrix(pGpMatrix);
		}
	}

	return pResultMatrix;
}
