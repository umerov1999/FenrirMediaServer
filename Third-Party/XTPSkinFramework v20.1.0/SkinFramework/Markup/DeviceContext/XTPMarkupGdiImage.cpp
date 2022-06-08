// XTPMarkupGdiImage.cpp: implementation of the CXTPMarkupGdiImage class.
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

#include "stdafx.h"

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPImageManager.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceDependentImage.h"
#include "Markup/DeviceContext/XTPMarkupGdiImage.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CXTPMarkupGdiImage, CXTPMarkupDeviceDependentImage);

CXTPMarkupGdiImage::CXTPMarkupGdiImage(IXTPMarkupDeviceDependentImageSite* pSite)
	: CXTPMarkupDeviceDependentImage(pSite)
	, m_pImageManager(NULL)
	, m_pImage(NULL)
{
}

CXTPMarkupGdiImage::~CXTPMarkupGdiImage()
{
	CMDTARGET_RELEASE(m_pImage);
	CMDTARGET_RELEASE(m_pImageManager);
}

BOOL CXTPMarkupGdiImage::Load(LPCWSTR lpszSource, int nWidth)
{
	ASSERT(NULL != lpszSource);

	CMDTARGET_RELEASE(m_pImage);
	CMDTARGET_RELEASE(m_pImageManager);
	m_imageSize.cx = 0;
	m_imageSize.cy = 0;
	m_bIsIcon	   = FALSE;

	if (wcsncmp(lpszSource, L"file://", 7) == 0)
	{
		CString strFileName(lpszSource + 7);

		m_pImageManager = new CXTPImageManager();
		if (NULL != GetSite()->GetMarkupContext())
		{
			m_pImageManager->SetMaskColor(GetSite()->GetMarkupContext()->GetImageColorKey());
		}

		if (strFileName.Right(3).CompareNoCase(_T("ico")) == 0)
		{
			m_pImageManager->SetIconFromIcoFile(strFileName, 0, 0, xtpImageNormal);
			m_bIsIcon = TRUE;
		}
		else
		{
			BOOL bAlphaBitmap = FALSE;
			HBITMAP hBitmap = CXTPImageManagerIcon::LoadBitmapFromFile(strFileName, &bAlphaBitmap);

			if (hBitmap)
			{
				CBitmap bmp;
				bmp.Attach(hBitmap);
				m_pImageManager->SetIcons(bmp, 0, 1, 0, xtpImageNormal, bAlphaBitmap);
			}
		}
		m_pImage = m_pImageManager->GetImage(0, nWidth);
		CMDTARGET_ADDREF(m_pImage);
	}
	else if (wcsncmp(lpszSource, L"res://", 6) == 0)
	{
		CString strResourceName(lpszSource + 6);

		LPCTSTR lpszResource = (LPCTSTR)strResourceName;
		if (lpszResource[0] == _T('#'))
		{
			lpszResource = MAKEINTRESOURCE(_ttoi(lpszResource + 1));
		}

		m_pImageManager = new CXTPImageManager();
		if (NULL != GetSite()->GetMarkupContext())
		{
			m_pImageManager->SetMaskColor(GetSite()->GetMarkupContext()->GetImageColorKey());
		}

		BOOL bAlphaBitmap = FALSE;
		HBITMAP hBitmap	  = CXTPImageManagerIcon::LoadBitmapFromResource(AfxGetResourceHandle(),
																		 lpszResource, &bAlphaBitmap);

		if (!hBitmap)
		{
			hBitmap = CXTPImageManagerIcon::LoadBitmapFromResource(lpszResource, &bAlphaBitmap);
		}

		if (hBitmap)
		{
			CBitmap bmp;
			bmp.Attach(hBitmap),
				m_pImageManager->SetIcons(bmp, 0, 1, 0, xtpImageNormal, bAlphaBitmap);
		}
		else
		{
			m_pImageManager->SetIcon(strResourceName, 0);
		}

		m_pImage = m_pImageManager->GetImage(0, nWidth);
		CMDTARGET_ADDREF(m_pImage);
	}
	else
	{
		int nIndex = _wtoi(lpszSource);
		// if (nIndex > 0)
		{
			CXTPMarkupContext* pContext = m_pSite->GetMarkupContext();
			ASSERT(NULL != pContext);

			m_pImageManager = pContext->GetImageManager();
			ASSERT(NULL != m_pImageManager);
			CMDTARGET_ADDREF(m_pImageManager);

			m_pImage = m_pImageManager->GetImage(XTPToUInt(nIndex), nWidth);
			CMDTARGET_ADDREF(m_pImage);
			if (NULL != m_pImage)
			{
				m_bIsIcon = TRUE;
			}
		}
	}

	if (NULL != m_pImage)
	{
		m_imageSize.cx = m_pImage->GetWidth();
		m_imageSize.cy = m_pImage->GetHeight();
	}

	return (NULL != m_pImage);
}
