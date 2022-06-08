// XTPMarkupPathGeometryFigures.cpp: implementation of the CXTPMarkupPathGeometryFigures class.
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

#include "Common/XTPFramework.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/Shapes/XTPMarkupPathGeometryFigures.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPathGeometry

IMPLEMENT_MARKUPCLASS(L"Figures", CXTPMarkupPathGeometryFigures, CXTPMarkupObject);

CXTPMarkupPathGeometryFigures::CXTPMarkupPathGeometryFigures()
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
}

CXTPMarkupPathGeometryFigures::~CXTPMarkupPathGeometryFigures()
{
}

void CXTPMarkupPathGeometryFigures::RegisterMarkupClass()
{
}

CXTPMarkupObject* CXTPMarkupPathGeometryFigures::ConvertFrom(CXTPMarkupBuilder* pBuilder,
															 CXTPMarkupObject* pObject) const
{
	UNREFERENCED_PARAMETER(pBuilder);
	UNREFERENCED_PARAMETER(pObject);
	return NULL;
}
