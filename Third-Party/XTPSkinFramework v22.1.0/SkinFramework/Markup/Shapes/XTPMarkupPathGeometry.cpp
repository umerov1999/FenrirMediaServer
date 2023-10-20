// XTPMarkupPathGeometry.cpp: implementation of the CXTPMarkupPathGeometry class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/Shapes/XTPMarkupPathGeometry.h"
#include "Markup/Shapes/XTPMarkupPathGeometryFigures.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPathGeometry

IMPLEMENT_MARKUPCLASS(L"PathGeometry", CXTPMarkupPathGeometry, CXTPMarkupObject);

CXTPMarkupDependencyProperty* CXTPMarkupPathGeometry::m_pPropertyGeometryFigures = NULL;

CXTPMarkupPathGeometry::CXTPMarkupPathGeometry()
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
}

CXTPMarkupPathGeometry::~CXTPMarkupPathGeometry()
{
}

void CXTPMarkupPathGeometry::RegisterMarkupClass()
{
	m_pPropertyGeometryFigures = CXTPMarkupDependencyProperty::Register(
		L"Figures", MARKUP_TYPE(CXTPMarkupPathGeometryFigures),
		MARKUP_TYPE(CXTPMarkupPathGeometry));
}

void CXTPMarkupPathGeometry::SetContentObject(CXTPMarkupBuilder* pBuilder,
											  CXTPMarkupObject* pContent)
{
	UNREFERENCED_PARAMETER(pBuilder);
	ASSERT(NULL != pContent);

	if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupPathGeometryFigures)))
	{
		SetValue(m_pPropertyGeometryFigures, pContent);
		MARKUP_ADDREF(pContent);
	}
	else
	{
		CXTPMarkupObject::SetContentObject(pBuilder, pContent);
	}
}

CXTPMarkupObject* CXTPMarkupPathGeometry::ConvertFrom(CXTPMarkupBuilder* pBuilder,
													  CXTPMarkupObject* pObject) const
{
	UNREFERENCED_PARAMETER(pBuilder);
	UNREFERENCED_PARAMETER(pObject);
	return NULL;
}
