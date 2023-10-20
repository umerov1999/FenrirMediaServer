// XTPMarkupStaticExtension.cpp: implementation of the CXTPMarkupStaticExtension class.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/Extensions/XTPMarkupStaticExtension.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

template<>
inline BOOL AFXAPI CompareElements(const LPCWSTR* pElement1, const LPCWSTR* pElement2)
{
	return wcscmp(*pElement1, *pElement2) == 0;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtension

CXTPMarkupStaticExtension::CXTPMarkupStaticExtension(LPCWSTR lpzName, BOOL bProperty)
	: m_bProperty(bProperty)
{
	ASSERT(ValidateExtensionName(lpzName, lpzName + wcslen(lpzName)));
	m_lpzName = _wcsdup(lpzName);
}

CXTPMarkupStaticExtension::CXTPMarkupStaticExtension(LPCWSTR lpzName)
	: m_bProperty(FALSE)
{
	ASSERT(ValidateExtensionName(lpzName, lpzName + wcslen(lpzName)));
	m_lpzName = _wcsdup(lpzName);
}

CXTPMarkupStaticExtension::~CXTPMarkupStaticExtension()
{
	if (!m_bProperty)
	{
		POSITION pos = m_mapExtensions.GetStartPosition();
		while (NULL != pos)
		{
			LPCWSTR lpzName					= NULL;
			CXTPMarkupStaticExtension* pExt = NULL;
			m_mapExtensions.GetNextAssoc(pos, lpzName, pExt);
			delete pExt;
		}
	}

	free(m_lpzName);
}

BOOL AFX_CDECL CXTPMarkupStaticExtension::Parse(LPCWSTR lpzValue, LPCWSTR& lpzSubValue)
{
	ASSERT(NULL != lpzValue);

	BOOL bParsed = FALSE;

	LPCWSTR lpDot = wcschr(lpzValue, L'.');
	if (NULL != lpDot)
	{
		if (ValidateExtensionName(lpzValue, lpDot))
		{
			lpzSubValue = lpDot + 1;
			bParsed		= TRUE;
		}
	}

	return bParsed;
}

void CXTPMarkupStaticExtension::Extend(CXTPMarkupStaticExtension* pExtension,
									   BOOL bReplace /*= FALSE*/)
{
	if (!m_bProperty)
	{
		ASSERT(NULL != pExtension);

		CXTPMarkupStaticExtension*& pExt = m_mapExtensions[pExtension->GetName()];
		if (NULL == pExt)
		{
			pExt = pExtension;
		}
		else if (bReplace)
		{
			m_mapExtensions.RemoveKey(pExtension->GetName());
			delete pExt;

			m_mapExtensions[pExtension->GetName()] = pExtension;
		}
		else
		{
			ASSERT(!"An extension is already added");
		}
	}
	else
	{
		ASSERT(!"Static extension leaves cannot be extended");
	}
}

CXTPMarkupStaticExtension* CXTPMarkupStaticExtension::FindExtension(LPCWSTR lpzName) const
{
	ASSERT(!m_bProperty);

	CXTPMarkupStaticExtension* pExt = NULL;

	LPCWSTR lpzSubName;
	if (Parse(lpzName, lpzSubName))
	{
		CArray<WCHAR, WCHAR> buffExtName;
		buffExtName.SetSize(lpzSubName - lpzName);
		lstrcpynW(buffExtName.GetData(), lpzName, (int)buffExtName.GetSize());
		if (m_mapExtensions.Lookup(buffExtName.GetData(), pExt))
		{
			pExt = pExt->FindExtension(lpzSubName);
		}
	}
	else if (ValidateExtensionName(lpzName, lpzName + wcslen(lpzName)))
	{
		m_mapExtensions.Lookup(lpzName, pExt);
	}

	return pExt;
}

CXTPMarkupObject* CXTPMarkupStaticExtension::ProvideValue(LPCWSTR lpzValue)
{
	ASSERT(!m_bProperty);

	CXTPMarkupObject* pValue = NULL;

	LPCWSTR lpzSubValue;
	if (Parse(lpzValue, lpzSubValue))
	{
		CArray<WCHAR, WCHAR> buffExtName;
		buffExtName.SetSize(lpzSubValue - lpzValue);
		lstrcpynW(buffExtName.GetData(), lpzValue, (int)buffExtName.GetSize());
		CXTPMarkupStaticExtension* pExt = NULL;
		if (m_mapExtensions.Lookup(buffExtName.GetData(), pExt))
		{
			pValue = pExt->ProvideValue(lpzSubValue);
		}
	}
	else if (ValidateExtensionName(lpzValue, lpzValue + wcslen(lpzValue)))
	{
		CXTPMarkupStaticExtension* pExt = NULL;
		if (m_mapExtensions.Lookup(lpzValue, pExt))
		{
			pValue = pExt->ProvideValue();
		}
	}

	return pValue;
}

CXTPMarkupObject* CXTPMarkupStaticExtension::ProvideValue()
{
	return NULL;
}

BOOL AFX_CDECL CXTPMarkupStaticExtension::ValidateExtensionName(LPCWSTR lpzBegin, LPCWSTR lpzEnd)
{
	ASSERT(lpzBegin <= lpzEnd);

	BOOL bValid = FALSE;

	if (NULL != lpzBegin)
	{
		bValid		 = TRUE;
		LPCWSTR lpCh = lpzBegin;
		while (lpCh < lpzEnd)
		{
			if (!((L'a' <= *lpCh && *lpCh <= L'z') || (L'A' <= *lpCh && *lpCh <= L'Z')
				  || L'_' == *lpCh))
			{
				bValid = FALSE;
				break;
			}

			++lpCh;
		}
	}

	return bValid;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionProperty

CXTPMarkupStaticExtensionProperty::CXTPMarkupStaticExtensionProperty(
	LPCWSTR lpzName, CXTPMarkupObject* pObject /*= NULL*/)
	: CXTPMarkupStaticExtension(lpzName, TRUE)
	, m_pObject(NULL)
{
	SetValue(pObject);
}

CXTPMarkupStaticExtensionProperty::~CXTPMarkupStaticExtensionProperty()
{
	SetValue(NULL);
}

void CXTPMarkupStaticExtensionProperty::SetValue(CXTPMarkupObject* pObject)
{
	MARKUP_RELEASE(m_pObject);
	m_pObject = pObject;
}

CXTPMarkupObject* CXTPMarkupStaticExtensionProperty::ProvideValue()
{
	MARKUP_ADDREF(m_pObject);
	return m_pObject;
}

CXTPMarkupObject* CXTPMarkupStaticExtensionProperty::ProvideValue(LPCWSTR lpzValue)
{
	return CXTPMarkupStaticExtension::ProvideValue(lpzValue);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionStringProperty

CXTPMarkupStaticExtensionStringProperty::CXTPMarkupStaticExtensionStringProperty(
	LPCWSTR lpzName, LPCWSTR lpzValue /*= NULL*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(lpzValue);
}

CXTPMarkupStaticExtensionStringProperty::CXTPMarkupStaticExtensionStringProperty(
	LPCWSTR lpzName, LPCSTR lpzValue /*= NULL*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(lpzValue);
}

void CXTPMarkupStaticExtensionStringProperty::SetValue(LPCWSTR lpzValue)
{
	CXTPMarkupObject* pObject = NULL;
	if (NULL != lpzValue)
	{
		pObject = CXTPMarkupString::CreateValue(lpzValue);
	}

	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

void CXTPMarkupStaticExtensionStringProperty::SetValue(LPCSTR lpzValue)
{
	CXTPMarkupObject* pObject = NULL;
	if (NULL != lpzValue)
	{
		pObject = CXTPMarkupString::CreateValue(lpzValue);
	}

	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

LPCWSTR CXTPMarkupStaticExtensionStringProperty::GetRawValue() const
{
	LPCWSTR lpzValue = NULL;

	CXTPMarkupString* pStringObject = MARKUP_DYNAMICCAST(CXTPMarkupString, m_pObject);
	if (NULL != pStringObject)
	{
		lpzValue = pStringObject->operator LPCWSTR();
	}

	return lpzValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionIntegerProperty

CXTPMarkupStaticExtensionIntegerProperty::CXTPMarkupStaticExtensionIntegerProperty(
	LPCWSTR lpzName, int nValue /*= 0*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(nValue);
}

void CXTPMarkupStaticExtensionIntegerProperty::SetValue(int nValue)
{
	CXTPMarkupObject* pObject = new CXTPMarkupInt(nValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

int CXTPMarkupStaticExtensionIntegerProperty::GetRawValue() const
{
	int nRawValue = 0;

	CXTPMarkupInt* pIntObject = MARKUP_DYNAMICCAST(CXTPMarkupInt, m_pObject);
	if (NULL != pIntObject)
	{
		nRawValue = pIntObject->GetValue();
	}

	return nRawValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionBooleanProperty

CXTPMarkupStaticExtensionBooleanProperty::CXTPMarkupStaticExtensionBooleanProperty(
	LPCWSTR lpzName, BOOL bValue /*= FALSE*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(bValue);
}

void CXTPMarkupStaticExtensionBooleanProperty::SetValue(BOOL bValue)
{
	CXTPMarkupObject* pObject = new CXTPMarkupBool(bValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

BOOL CXTPMarkupStaticExtensionBooleanProperty::GetRawValue() const
{
	BOOL bRawValue = 0;

	CXTPMarkupBool* pBoolObject = MARKUP_DYNAMICCAST(CXTPMarkupBool, m_pObject);
	if (NULL != pBoolObject)
	{
		bRawValue = pBoolObject->operator BOOL();
	}

	return bRawValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionDoubleProperty

CXTPMarkupStaticExtensionDoubleProperty::CXTPMarkupStaticExtensionDoubleProperty(
	LPCWSTR lpzName, double dblValue /*= 0*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(dblValue);
}

void CXTPMarkupStaticExtensionDoubleProperty::SetValue(double dblValue)
{
	CXTPMarkupObject* pObject = new CXTPMarkupDouble(dblValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

double CXTPMarkupStaticExtensionDoubleProperty::GetRawValue() const
{
	double dblRawValue = 0;

	CXTPMarkupDouble* pDoubleObject = MARKUP_DYNAMICCAST(CXTPMarkupDouble, m_pObject);
	if (NULL != pDoubleObject)
	{
		dblRawValue = pDoubleObject->operator double();
	}

	return dblRawValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionColorProperty

CXTPMarkupStaticExtensionColorProperty::CXTPMarkupStaticExtensionColorProperty(
	LPCWSTR lpzName, COLORREF crValue /*= RGB(0, 0, 0)*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(crValue);
}

CXTPMarkupStaticExtensionColorProperty::CXTPMarkupStaticExtensionColorProperty(LPCWSTR lpzName,
																			   COLORREF crValue,
																			   BYTE bAlpha)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(crValue, bAlpha);
}

void CXTPMarkupStaticExtensionColorProperty::SetValue(COLORREF crValue)
{
	CXTPMarkupObject* pObject = new CXTPMarkupColor(crValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

void CXTPMarkupStaticExtensionColorProperty::SetValue(COLORREF crValue, BYTE bAlpha)
{
	CXTPMarkupObject* pObject = new CXTPMarkupColor(bAlpha, crValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

COLORREF CXTPMarkupStaticExtensionColorProperty::GetRawValue() const
{
	COLORREF crRawValue = 0;

	CXTPMarkupColor* pColorObject = MARKUP_DYNAMICCAST(CXTPMarkupColor, m_pObject);
	if (NULL != pColorObject)
	{
		crRawValue = pColorObject->operator COLORREF();
	}

	return crRawValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStaticExtensionBrushProperty

CXTPMarkupStaticExtensionBrushProperty::CXTPMarkupStaticExtensionBrushProperty(
	LPCWSTR lpzName, COLORREF crValue /*= RGB(0, 0, 0)*/)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(crValue);
}

CXTPMarkupStaticExtensionBrushProperty::CXTPMarkupStaticExtensionBrushProperty(LPCWSTR lpzName,
																			   COLORREF crValue,
																			   BYTE bAlpha)
	: CXTPMarkupStaticExtensionProperty(lpzName)
{
	SetValue(crValue, bAlpha);
}

void CXTPMarkupStaticExtensionBrushProperty::SetValue(COLORREF crValue)
{
	CXTPMarkupObject* pObject = new CXTPMarkupSolidColorBrush(crValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

void CXTPMarkupStaticExtensionBrushProperty::SetValue(COLORREF crValue, BYTE bAlpha)
{
	CXTPMarkupObject* pObject = new CXTPMarkupSolidColorBrush(bAlpha, crValue);
	CXTPMarkupStaticExtensionProperty::SetValue(pObject);
}

COLORREF CXTPMarkupStaticExtensionBrushProperty::GetRawValue() const
{
	COLORREF crRawValue = 0;

	CXTPMarkupSolidColorBrush* pColorBrush = MARKUP_DYNAMICCAST(CXTPMarkupSolidColorBrush,
																m_pObject);
	if (pColorBrush)
	{
		CXTPMarkupColor* pColorObject = pColorBrush->GetColor();
		if (NULL != pColorObject)
		{
			crRawValue = pColorObject->operator COLORREF();
		}
	}

	return crRawValue;
}
