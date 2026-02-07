// XTPMarkupTransformationMatrix.h: CXTPMarkupTransformationMatrix class implementation.
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

#include "Markup/Transform/XTPMarkupTransformationMatrix.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CXTPMarkupTransformationMatrix, CObject);

void CXTPMarkupTransformationMatrix::Offset(const POINT& offset)
{
	UNREFERENCED_PARAMETER(offset);

	// nothing to offset.
}

BOOL CXTPMarkupTransformationMatrix::PtInRect(const RECT& rect, const POINT& absoluteOffset,
											  const POINT& point) const
{
	UNREFERENCED_PARAMETER(absoluteOffset);
	return ::PtInRect(&rect, point);
}

CRect CXTPMarkupTransformationMatrix::GetBoundRect(const RECT& rect,
												   const POINT& absoluteOffset) const
{
	UNREFERENCED_PARAMETER(absoluteOffset);
	return rect;
}
