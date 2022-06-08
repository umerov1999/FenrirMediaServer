// XTPMarkupTranslateTransform.cpp: implementation of the CXTPMarkupTranslateTransform class.
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
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupTranslateTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupTranslateTransform

CXTPMarkupDependencyProperty* CXTPMarkupTranslateTransform::m_pPropertyX = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTranslateTransform::m_pPropertyY = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupTranslateTransform, CXTPMarkupTransform);

void CXTPMarkupTranslateTransform::RegisterMarkupClass()
{
	m_pPropertyX = CXTPMarkupDependencyProperty::Register(
		L"X", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupTranslateTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pPropertyY = CXTPMarkupDependencyProperty::Register(
		L"Y", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupTranslateTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

double CXTPMarkupTranslateTransform::GetX() const
{
	double nX			 = 0;
	CXTPMarkupDouble* pX = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyX));
	if (NULL != pX)
	{
		nX = *pX;
	}

	return nX;
}

void CXTPMarkupTranslateTransform::SetX(double x)
{
	SetValue(m_pPropertyX, new CXTPMarkupDouble(x));
}

double CXTPMarkupTranslateTransform::GetY() const
{
	double nY			 = 0;
	CXTPMarkupDouble* pY = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyY));
	if (NULL != pY)
	{
		nY = *pY;
	}

	return nY;
}

void CXTPMarkupTranslateTransform::SetY(double y)
{
	SetValue(m_pPropertyY, new CXTPMarkupDouble(y));
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupTranslateTransform::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	UNREFERENCED_PARAMETER(pMatrix);
	return NULL;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupTranslateTransform, CXTPMarkupTransform)
	DISP_PROPERTY_EX(CXTPMarkupTranslateTransform, "X", OleGetX, OleSetX, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupTranslateTransform, "Y", OleGetY, OleSetY, VT_R8)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

double CXTPMarkupTranslateTransform::OleGetX()
{
	return GetX();
}

void CXTPMarkupTranslateTransform::OleSetX(double x)
{
	SetX(x);
}

double CXTPMarkupTranslateTransform::OleGetY()
{
	return GetY();
}

void CXTPMarkupTranslateTransform::OleSetY(double y)
{
	SetY(y);
}
