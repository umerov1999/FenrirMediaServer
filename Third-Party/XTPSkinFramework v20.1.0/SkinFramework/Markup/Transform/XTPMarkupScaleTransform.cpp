// XTPMarkupScaleTransform.cpp: implementation of the CXTPMarkupScaleTransform class.
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
#include "Markup/Transform/XTPMarkupScaleTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupScaleTransform

CXTPMarkupDependencyProperty* CXTPMarkupScaleTransform::m_pPropertyScaleX  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupScaleTransform::m_pPropertyScaleY  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupScaleTransform::m_pPropertyCenterX = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupScaleTransform::m_pPropertyCenterY = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupScaleTransform, CXTPMarkupTransform);

void CXTPMarkupScaleTransform::RegisterMarkupClass()
{
	m_pPropertyScaleX = CXTPMarkupDependencyProperty::Register(
		L"ScaleX", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupScaleTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pPropertyScaleY = CXTPMarkupDependencyProperty::Register(
		L"ScaleY", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupScaleTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagVertDpiSensible));

	m_pPropertyCenterX = CXTPMarkupDependencyProperty::Register(
		L"CenterX", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupScaleTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pPropertyCenterY = CXTPMarkupDependencyProperty::Register(
		L"CenterY", MARKUP_TYPE(CXTPMarkupDouble), MARKUP_TYPE(CXTPMarkupScaleTransform),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

double CXTPMarkupScaleTransform::GetScaleX() const
{
	double nScaleX			  = 0;
	CXTPMarkupDouble* pScaleX = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyScaleX));
	if (NULL != pScaleX)
	{
		nScaleX = *pScaleX;
	}

	return nScaleX;
}

void CXTPMarkupScaleTransform::SetScaleX(double scaleX)
{
	SetValue(m_pPropertyScaleX, new CXTPMarkupDouble(scaleX));
}

double CXTPMarkupScaleTransform::GetScaleY() const
{
	double nScaleY			  = 0;
	CXTPMarkupDouble* pScaleY = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyScaleY));
	if (NULL != pScaleY)
	{
		nScaleY = *pScaleY;
	}

	return nScaleY;
}

void CXTPMarkupScaleTransform::SetScaleY(double scaleY)
{
	SetValue(m_pPropertyScaleY, new CXTPMarkupDouble(scaleY));
}

double CXTPMarkupScaleTransform::GetCenterX() const
{
	double nCenterX			   = 0;
	CXTPMarkupDouble* pCenterX = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyCenterX));
	if (NULL != pCenterX)
	{
		nCenterX = *pCenterX;
	}

	return nCenterX;
}

void CXTPMarkupScaleTransform::SetCenterX(double CenterX)
{
	SetValue(m_pPropertyCenterX, new CXTPMarkupDouble(CenterX));
}

double CXTPMarkupScaleTransform::GetCenterY() const
{
	double nCenterY			   = 0;
	CXTPMarkupDouble* pCenterY = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pPropertyCenterY));
	if (NULL != pCenterY)
	{
		nCenterY = *pCenterY;
	}

	return nCenterY;
}

void CXTPMarkupScaleTransform::SetCenterY(double CenterY)
{
	SetValue(m_pPropertyCenterY, new CXTPMarkupDouble(CenterY));
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupScaleTransform::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	UNREFERENCED_PARAMETER(pMatrix);
	return NULL;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupScaleTransform, CXTPMarkupTransform)
	DISP_PROPERTY_EX(CXTPMarkupScaleTransform, "ScaleX", OleGetScaleX, OleSetScaleX, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupScaleTransform, "ScaleY", OleGetScaleY, OleSetScaleY, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupScaleTransform, "CenterX", OleGetCenterX, OleSetCenterX, VT_R8)
	DISP_PROPERTY_EX(CXTPMarkupScaleTransform, "CenterY", OleGetCenterY, OleSetCenterY, VT_R8)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

double CXTPMarkupScaleTransform::OleGetScaleX()
{
	return GetScaleX();
}

void CXTPMarkupScaleTransform::OleSetScaleX(double scaleX)
{
	SetScaleX(scaleX);
}

double CXTPMarkupScaleTransform::OleGetScaleY()
{
	return GetScaleY();
}

void CXTPMarkupScaleTransform::OleSetScaleY(double scaleY)
{
	SetScaleY(scaleY);
}

double CXTPMarkupScaleTransform::OleGetCenterX()
{
	return GetCenterX();
}

void CXTPMarkupScaleTransform::OleSetCenterX(double centerX)
{
	SetCenterX(centerX);
}

double CXTPMarkupScaleTransform::OleGetCenterY()
{
	return GetCenterY();
}

void CXTPMarkupScaleTransform::OleSetCenterY(double centerY)
{
	SetCenterY(centerY);
}
