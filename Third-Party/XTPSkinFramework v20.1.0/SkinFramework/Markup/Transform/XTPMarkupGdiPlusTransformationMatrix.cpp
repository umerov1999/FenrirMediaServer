// XTPMarkupGdiPlusTransformationMatrix.cpp: CXTPMarkupGdiPlusTransformationMatrix class
// implementation.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/Transform/XTPMarkupTransformationMatrix.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformationMatrix.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CXTPMarkupGdiPlusTransformationMatrix, CObject);

CXTPMarkupGdiPlusTransformationMatrix::CXTPMarkupGdiPlusTransformationMatrix(CXTPGdiPlus* pReserved,
																			 GpMatrix* pMatrix)
	: m_pMatrix(pMatrix)
{
	UNREFERENCED_PARAMETER(pReserved);

	ASSERT(NULL != m_pMatrix);
}

CXTPMarkupGdiPlusTransformationMatrix::~CXTPMarkupGdiPlusTransformationMatrix()
{
	GdipDeleteMatrix(m_pMatrix);
}

CXTPMarkupTransformationMatrix* CXTPMarkupGdiPlusTransformationMatrix::Clone() const
{
	CXTPMarkupTransformationMatrix* pClonedInstance = NULL;
	GpMatrix* pClonedMatrix							= NULL;
	if (Gdiplus::Ok == GdipCloneMatrix(m_pMatrix, &pClonedMatrix))
	{
		pClonedInstance = new CXTPMarkupGdiPlusTransformationMatrix(NULL, pClonedMatrix);
	}

	return pClonedInstance;
}

void CXTPMarkupGdiPlusTransformationMatrix::Offset(const POINT& offset)
{
	GdipTranslateMatrix(m_pMatrix, static_cast<REAL>(offset.x), static_cast<REAL>(offset.y),
						Gdiplus::MatrixOrderPrepend);
}

inline BOOL IsSameTriangleSide(const PointF& p1, const PointF& p2, const PointF& a, const PointF& b)
{
	PointF bsa(b.X - a.X, b.Y - a.Y);
	PointF p1sa(p1.X - a.X, p1.Y - a.Y);
	PointF p2sa(p2.X - a.X, p2.Y - a.Y);
	REAL crossProduct1 = bsa.X * p1sa.Y - bsa.Y * p1sa.X;
	REAL crossProduct2 = bsa.X * p2sa.Y - bsa.Y * p2sa.X;
	REAL dotProduct	   = crossProduct1 * crossProduct2;
	return 0 <= dotProduct;
}

inline BOOL PointInTriangle(const PointF& point, const PointF& a, const PointF& b, const PointF& c)
{
	return IsSameTriangleSide(point, a, b, c) && IsSameTriangleSide(point, b, a, c)
		   && IsSameTriangleSide(point, c, a, b);
}

BOOL CXTPMarkupGdiPlusTransformationMatrix::TransformRect(const RECT& rect,
														  const POINT& absoluteOffset,
														  PointF (&transformedRectPoints)[4]) const
{
	BOOL bSuccess = FALSE;

	transformedRectPoints[0].X = static_cast<REAL>(rect.left);
	transformedRectPoints[0].Y = static_cast<REAL>(rect.top);
	transformedRectPoints[1].X = static_cast<REAL>(rect.right);
	transformedRectPoints[1].Y = transformedRectPoints[0].Y;
	transformedRectPoints[2].X = transformedRectPoints[1].X;
	transformedRectPoints[2].Y = static_cast<REAL>(rect.bottom);
	transformedRectPoints[3].X = transformedRectPoints[0].X;
	transformedRectPoints[3].Y = transformedRectPoints[2].Y;
	if (Ok
		== GdipTransformMatrixPoints(m_pMatrix, transformedRectPoints,
									 _countof(transformedRectPoints)))
	{
		PointF offset(static_cast<REAL>(absoluteOffset.x), static_cast<REAL>(absoluteOffset.y));
		transformedRectPoints[0].X -= offset.X;
		transformedRectPoints[0].Y -= offset.Y;
		transformedRectPoints[1].X -= offset.X;
		transformedRectPoints[1].Y -= offset.Y;
		transformedRectPoints[2].X -= offset.X;
		transformedRectPoints[2].Y -= offset.Y;
		transformedRectPoints[3].X -= offset.X;
		transformedRectPoints[3].Y -= offset.Y;

		bSuccess = TRUE;
	}

	return bSuccess;
}

BOOL CXTPMarkupGdiPlusTransformationMatrix::PtInRect(const RECT& rect, const POINT& absoluteOffset,
													 const POINT& point) const
{
	BOOL bHit = FALSE;

	PointF transformedRectPoints[4];
	if (TransformRect(rect, absoluteOffset, transformedRectPoints))
	{
		PointF pointf(static_cast<REAL>(point.x), static_cast<REAL>(point.y));
		bHit = PointInTriangle(pointf, transformedRectPoints[0], transformedRectPoints[1],
							   transformedRectPoints[2]);
		if (!bHit)
		{
			bHit = PointInTriangle(pointf, transformedRectPoints[2], transformedRectPoints[3],
								   transformedRectPoints[0]);
		}
	}

	return bHit;
}

CRect CXTPMarkupGdiPlusTransformationMatrix::GetBoundRect(const RECT& rect,
														  const POINT& absoluteOffset) const
{
	CRect rcBound;
	rcBound.SetRectEmpty();

	PointF transformedRectPoints[4];
	if (TransformRect(rect, absoluteOffset, transformedRectPoints))
	{
		rcBound.SetRect(INT_MAX, INT_MAX, INT_MIN, INT_MIN);
		for (int i = 0; i < _countof(transformedRectPoints); ++i)
		{
			int x = static_cast<int>(transformedRectPoints[i].X);
			int y = static_cast<int>(transformedRectPoints[i].Y);
			if (x < rcBound.left)
			{
				rcBound.left = x;
			}

			if (y < rcBound.top)
			{
				rcBound.top = y;
			}

			if (rcBound.right < x)
			{
				rcBound.right = x;
			}

			if (rcBound.bottom < y)
			{
				rcBound.bottom = y;
			}
		}
	}

	return rcBound;
}
