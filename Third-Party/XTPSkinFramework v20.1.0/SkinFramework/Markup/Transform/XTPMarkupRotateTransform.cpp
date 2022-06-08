// XTPMarkupRotateTransform.cpp: implementation of the CXTPMarkupRotateTransform class.
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
#include "Markup/Transform/XTPMarkupRotateTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRotateTransform

CXTPMarkupDependencyProperty* CXTPMarkupRotateTransform::m_pPropertyAngle	= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRotateTransform::m_pPropertyCenterX = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRotateTransform::m_pPropertyCenterY = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupRotateTransform, CXTPMarkupTransform);

void CXTPMarkupRotateTransform::RegisterMarkupClass()
{
	m_pPropertyAngle = CXTPMarkupDependencyProperty::Register(
		L"Angle", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupRotateTransform));

	m_pPropertyCenterX = CXTPMarkupDependencyProperty::Register(
		L"CenterX", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupRotateTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pPropertyCenterY = CXTPMarkupDependencyProperty::Register(
		L"CenterY", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupRotateTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

double CXTPMarkupRotateTransform::GetAngle() const
{
	double nAngle			 = 0;
	CXTPMarkupDouble* pAngle = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyAngle));
	if (NULL != pAngle)
	{
		nAngle = *pAngle;
	}

	return nAngle;
}

void CXTPMarkupRotateTransform::SetAngle(double angle)
{
	SetValue(m_pPropertyAngle, new CXTPMarkupDouble(angle));
}

double CXTPMarkupRotateTransform::GetCenterX() const
{
	double nCenterX			   = 0;
	CXTPMarkupDouble* pCenterX = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyCenterX));
	if (NULL != pCenterX)
	{
		nCenterX = *pCenterX;
	}

	return nCenterX;
}

void CXTPMarkupRotateTransform::SetCenterX(double centerX)
{
	SetValue(m_pPropertyCenterX, new CXTPMarkupDouble(centerX));
}

double CXTPMarkupRotateTransform::GetCenterY() const
{
	double nCenterY			   = 0;
	CXTPMarkupDouble* pCenterY = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyCenterY));
	if (NULL != pCenterY)
	{
		nCenterY = *pCenterY;
	}

	return nCenterY;
}

void CXTPMarkupRotateTransform::SetCenterY(double centerY)
{
	SetValue(m_pPropertyCenterY, new CXTPMarkupDouble(centerY));
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupRotateTransform::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	UNREFERENCED_PARAMETER(pMatrix);
	return NULL;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupRotateTransform, CXTPMarkupTransform)
	DISP_PROPERTY_EX(CXTPMarkupRotateTransform, "Angle", OleGetAngle, OleSetAngle, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupRotateTransform, "CenterX", OleGetCenterX, OleSetCenterX, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupRotateTransform, "CenterY", OleGetCenterY, OleSetCenterY, VT_R8)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

double CXTPMarkupRotateTransform::OleGetAngle()
{
	return GetAngle();
}

void CXTPMarkupRotateTransform::OleSetAngle(double angle)
{
	SetAngle(angle);
}

double CXTPMarkupRotateTransform::OleGetCenterX()
{
	return GetCenterX();
}

void CXTPMarkupRotateTransform::OleSetCenterX(double centerX)
{
	SetCenterX(centerX);
}

double CXTPMarkupRotateTransform::OleGetCenterY()
{
	return GetCenterY();
}

void CXTPMarkupRotateTransform::OleSetCenterY(double centerY)
{
	SetCenterY(centerY);
}
