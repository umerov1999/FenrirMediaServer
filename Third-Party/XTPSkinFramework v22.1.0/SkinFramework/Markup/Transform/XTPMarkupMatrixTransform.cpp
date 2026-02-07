// XTPMarkupMatrixTransform.cpp: implementation of the CXTPMarkupMatrixTransform class.
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

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupMatrixTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupMatrixTransform

CXTPMarkupDependencyProperty* CXTPMarkupMatrixTransform::m_pPropertyMatrix = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupMatrixTransform, CXTPMarkupTransform);

void CXTPMarkupMatrixTransform::RegisterMarkupClass()
{
	m_pPropertyMatrix = CXTPMarkupDependencyProperty::Register(
		L"Matrix", MARKUP_TYPE(CXTPMarkupDoubleCollection), MARKUP_TYPE(CXTPMarkupMatrixTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupDoubleCollection* CXTPMarkupMatrixTransform::GetMatrix() const
{
	return MARKUP_STATICCAST(CXTPMarkupDoubleCollection, GetValue(m_pPropertyMatrix));
}

void CXTPMarkupMatrixTransform::SetMatrix(CXTPMarkupDoubleCollection* pMatrix)
{
	SetValue(m_pPropertyMatrix, pMatrix);
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupMatrixTransform::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	UNREFERENCED_PARAMETER(pMatrix);
	return NULL;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupMatrixTransform, CXTPMarkupTransform)
	DISP_PROPERTY_EX(CXTPMarkupMatrixTransform, "Matrix", OleGetMatrix, OleSetMatrix, VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

LPDISPATCH CXTPMarkupMatrixTransform::OleGetMatrix()
{
	return XTPGetDispatch(GetMatrix());
}

void CXTPMarkupMatrixTransform::OleSetMatrix(LPDISPATCH lpValue)
{
	SetMatrix(MARKUP_STATICCAST(CXTPMarkupDoubleCollection, FromDispatch(lpValue)));
}