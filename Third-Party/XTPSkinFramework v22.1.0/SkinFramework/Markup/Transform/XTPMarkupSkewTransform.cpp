// XTPMarkupSkewTransform.cpp: implementation of the CXTPMarkupSkewTransform class.
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
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupSkewTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSkewTransform

CXTPMarkupDependencyProperty* CXTPMarkupSkewTransform::m_pPropertyAngleX  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupSkewTransform::m_pPropertyAngleY  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupSkewTransform::m_pPropertyCenterX = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupSkewTransform::m_pPropertyCenterY = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupSkewTransform, CXTPMarkupTransform);

void CXTPMarkupSkewTransform::RegisterMarkupClass()
{
	m_pPropertyAngleX = CXTPMarkupDependencyProperty::Register(
		L"AngleX", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupSkewTransform));

	m_pPropertyAngleY = CXTPMarkupDependencyProperty::Register(
		L"AngleY", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupSkewTransform));

	m_pPropertyCenterX = CXTPMarkupDependencyProperty::Register(
		L"CenterX", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupSkewTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pPropertyCenterY = CXTPMarkupDependencyProperty::Register(
		L"CenterY", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupSkewTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

double CXTPMarkupSkewTransform::GetAngleX() const
{
	double nAngleX			  = 0;
	CXTPMarkupDouble* pAngleX = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyAngleX));
	if (NULL != pAngleX)
	{
		nAngleX = *pAngleX;
	}

	return nAngleX;
}

void CXTPMarkupSkewTransform::SetAngleX(double angleX)
{
	SetValue(m_pPropertyAngleX, new CXTPMarkupDouble(angleX));
}

double CXTPMarkupSkewTransform::GetAngleY() const
{
	double nAngleY			  = 0;
	CXTPMarkupDouble* pAngleY = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyAngleY));
	if (NULL != pAngleY)
	{
		nAngleY = *pAngleY;
	}

	return nAngleY;
}

void CXTPMarkupSkewTransform::SetAngleY(double angleY)
{
	SetValue(m_pPropertyAngleY, new CXTPMarkupDouble(angleY));
}

double CXTPMarkupSkewTransform::GetCenterX() const
{
	double nCenterX			   = 0;
	CXTPMarkupDouble* pCenterX = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyCenterX));
	if (NULL != pCenterX)
	{
		nCenterX = *pCenterX;
	}

	return nCenterX;
}

void CXTPMarkupSkewTransform::SetCenterX(double centerX)
{
	SetValue(m_pPropertyCenterX, new CXTPMarkupDouble(centerX));
}

double CXTPMarkupSkewTransform::GetCenterY() const
{
	double nCenterY			   = 0;
	CXTPMarkupDouble* pCenterY = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyCenterY));
	if (NULL != pCenterY)
	{
		nCenterY = *pCenterY;
	}

	return nCenterY;
}

void CXTPMarkupSkewTransform::SetCenterY(double centerY)
{
	SetValue(m_pPropertyCenterY, new CXTPMarkupDouble(centerY));
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupSkewTransform::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	UNREFERENCED_PARAMETER(pMatrix);
	return NULL;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupSkewTransform, CXTPMarkupTransform)
	DISP_PROPERTY_EX(CXTPMarkupSkewTransform, "AngleX", OleGetAngleX, OleSetAngleX, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupSkewTransform, "AngleY", OleGetAngleY, OleSetAngleY, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupSkewTransform, "CenterX", OleGetCenterX, OleSetCenterX, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupSkewTransform, "CenterY", OleGetCenterY, OleSetCenterY, VT_R8)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

double CXTPMarkupSkewTransform::OleGetAngleX()
{
	return GetAngleX();
}

void CXTPMarkupSkewTransform::OleSetAngleX(double angleX)
{
	SetAngleX(angleX);
}

double CXTPMarkupSkewTransform::OleGetAngleY()
{
	return GetAngleY();
}

void CXTPMarkupSkewTransform::OleSetAngleY(double angleY)
{
	SetAngleY(angleY);
}

double CXTPMarkupSkewTransform::OleGetCenterX()
{
	return GetCenterX();
}

void CXTPMarkupSkewTransform::OleSetCenterX(double centerX)
{
	SetCenterX(centerX);
}

double CXTPMarkupSkewTransform::OleGetCenterY()
{
	return GetCenterY();
}

void CXTPMarkupSkewTransform::OleSetCenterY(double centerY)
{
	SetCenterY(centerY);
}
