// XTPMarkupThickness.cpp: implementation of the CXTPMarkupThickness class.
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
#include "Common/Base/cxminmax.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPVc80Helpers.h"
#include "Common/XTPDrawHelpers.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupThickness
IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupThickness, CXTPMarkupObject);

void CXTPMarkupThickness::RegisterMarkupClass()
{
}

CXTPMarkupThickness* CXTPMarkupThickness::CreateValue()
{
	return new CXTPMarkupThickness();
}

CXTPMarkupThickness::CXTPMarkupThickness()
	: m_left(0)
	, m_top(0)
	, m_right(0)
	, m_bottom(0)
{
	// Nothing
}

CXTPMarkupThickness::CXTPMarkupThickness(const CXTPMarkupThickness& other)
	: m_left(other.m_left)
	, m_top(other.m_top)
	, m_right(other.m_right)
	, m_bottom(other.m_bottom)
{
	// Nothing
}

CXTPMarkupThickness::CXTPMarkupThickness(long nThickness)
	: m_left(nThickness)
	, m_top(nThickness)
	, m_right(nThickness)
	, m_bottom(nThickness)
{
	// Nothing
}

CXTPMarkupThickness::CXTPMarkupThickness(long nXThickness, long nYThickness)
	: m_left(nXThickness)
	, m_top(nYThickness)
	, m_right(nXThickness)
	, m_bottom(nYThickness)
{
	// Nothing
}

CXTPMarkupThickness::CXTPMarkupThickness(long nLeft, long nTop, long nRight, long nBottom)
	: m_left(nLeft)
	, m_top(nTop)
	, m_right(nRight)
	, m_bottom(nBottom)
{
	// Nothing
}

CRect CXTPMarkupThickness::HelperDeflateRect(CRect rt, CXTPMarkupThickness* thick)
{
	if (!thick)
		return rt;

	return CRect(CPoint(rt.left + thick->m_left, rt.top + thick->m_top),
				 CSize(CXTP_max(0, (rt.Width() - thick->m_left) - thick->m_right),
					 CXTP_max(0, (rt.Height() - thick->m_top) - thick->m_bottom)));
}

CXTPMarkupObject* CXTPMarkupThickness::ConvertFrom(CXTPMarkupBuilder* pBuilder,
												   CXTPMarkupObject* pObject) const
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pObject);

	if (IsStringObject(pObject))
	{
		LPCWSTR lpszValue = *((CXTPMarkupString*)pObject);
		int nLength		  = ((CXTPMarkupString*)pObject)->GetLength();

		if (wcschr(lpszValue, L',') == NULL)
		{
			int nThickness = _wtoi(lpszValue);

			if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"pt") == 0)
			{
				nThickness = MulDiv(nThickness, CXTPDpi::DefaultDpi, 72);
			}
			else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"in") == 0)
			{
				nThickness *= CXTPDpi::DefaultDpi;
			}
			else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"cm") == 0)
			{
				nThickness = int((double)nThickness * 37.79528);
			}
			else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"mm") == 0)
			{
				nThickness = int((double)nThickness * 3.779528);
			}

			int nXThickness = nThickness;
			int nYThickness = nThickness;

			if (NULL != m_pAssociatedProperty)
			{
				if (0
					!= (m_pAssociatedProperty->GetFlags()
						& CXTPMarkupPropertyMetadata::flagHorzDpiSensible))
				{
					nXThickness = pBuilder->GetMarkupContext()->ScaleX(nXThickness);
				}

				if (0
					!= (m_pAssociatedProperty->GetFlags()
						& CXTPMarkupPropertyMetadata::flagVertDpiSensible))
				{
					nYThickness = pBuilder->GetMarkupContext()->ScaleY(nYThickness);
				}
			}

			return new CXTPMarkupThickness(nXThickness, nYThickness);
		}
		else
		{
			int left = 0, top = 0, right = 0, bottom = 0;
			if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"pt") == 0)
			{
				if (WSCANF_S(lpszValue, L"%ipt, %ipt, %ipt, %ipt", &left, &top, &right, &bottom)
					!= 4)
					return NULL;

				left   = MulDiv(left, CXTPDpi::DefaultDpi, 72);
				top	= MulDiv(top, CXTPDpi::DefaultDpi, 72);
				right  = MulDiv(right, CXTPDpi::DefaultDpi, 72);
				bottom = MulDiv(bottom, CXTPDpi::DefaultDpi, 72);
			}
			else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"in") == 0)
			{
				if (WSCANF_S(lpszValue, L"%iin, %iin, %iin, %iin", &left, &top, &right, &bottom)
					!= 4)
					return NULL;

				left *= CXTPDpi::DefaultDpi;
				top *= CXTPDpi::DefaultDpi;
				right *= CXTPDpi::DefaultDpi;
				bottom *= CXTPDpi::DefaultDpi;
			}
			else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"cm") == 0)
			{
				if (WSCANF_S(lpszValue, L"%icm, %icm, %icm, %icm", &left, &top, &right, &bottom)
					!= 4)
					return NULL;

				left   = int((double)left * 37.79528);
				top	= int((double)top * 37.79528);
				right  = int((double)right * 37.79528);
				bottom = int((double)bottom * 37.79528);
			}
			else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"mm") == 0)
			{
				if (WSCANF_S(lpszValue, L"%imm, %imm, %imm, %imm", &left, &top, &right, &bottom)
					!= 4)
					return NULL;

				left   = int((double)left * 3.779528);
				top	= int((double)top * 3.779528);
				right  = int((double)right * 3.779528);
				bottom = int((double)bottom * 3.779528);
			}
			else
			{
				if (WSCANF_S(lpszValue, L"%i, %i, %i, %i", &left, &top, &right, &bottom) != 4)
					return NULL;
			}

			if (NULL != m_pAssociatedProperty)
			{
				if (0
					!= (m_pAssociatedProperty->GetFlags()
						& CXTPMarkupPropertyMetadata::flagHorzDpiSensible))
				{
					left  = pBuilder->GetMarkupContext()->ScaleX(left);
					right = pBuilder->GetMarkupContext()->ScaleX(right);
				}

				if (0
					!= (m_pAssociatedProperty->GetFlags()
						& CXTPMarkupPropertyMetadata::flagVertDpiSensible))
				{
					top	= pBuilder->GetMarkupContext()->ScaleY(top);
					bottom = pBuilder->GetMarkupContext()->ScaleY(bottom);
				}
			}

			return new CXTPMarkupThickness(left, top, right, bottom);
		}
	}

	return NULL;
}

BOOL CXTPMarkupThickness::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupThickness))
		return FALSE;

	const CXTPMarkupThickness* pOther = static_cast<const CXTPMarkupThickness*>(pObject);

	return m_left == pOther->m_left && m_top == pOther->m_top && m_right == pOther->m_right
		   && m_bottom == pOther->m_bottom;
}

BEGIN_DISPATCH_MAP(CXTPMarkupThickness, CXTPMarkupObject)
	DISP_PROPERTY_ID(CXTPMarkupThickness, "Left", 100, m_left, VT_I4)
	DISP_PROPERTY_ID(CXTPMarkupThickness, "Top", 101, m_top, VT_I4)
	DISP_PROPERTY_ID(CXTPMarkupThickness, "Right", 102, m_right, VT_I4)
	DISP_PROPERTY_ID(CXTPMarkupThickness, "Bottom", 103, m_bottom, VT_I4)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupThickness, CXTPMarkupObject)
	INTERFACE_PART(CXTPMarkupThickness, XTPDIID_MarkupThickness, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupThickness, XTPDIID_MarkupThickness)
#endif
