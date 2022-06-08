// XTPMarkupGdiPlusImage.cpp: implementation of the CXTPMarkupGdiPlusImage class.
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
#include <comdef.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPImageManager.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceDependentImage.h"
#include "Markup/DeviceContext/XTPMarkupGdiPlusImage.h"
#include "Markup/DeviceContext/XTPGdiPlus.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

const GUID XTPFrameDimensionTime = { 0x6aedbd6d,
									 0x3fb5,
									 0x418a,
									 { 0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72 } };

IMPLEMENT_DYNAMIC(CXTPMarkupGdiPlusImage, CXTPMarkupDeviceDependentImage);

CXTPMarkupGdiPlusImage::CXTPMarkupGdiPlusImage(CXTPGdiPlus* pReserved,
											   IXTPMarkupDeviceDependentImageSite* pSite)
	: CXTPMarkupDeviceDependentImage(pSite)
	, m_pImage(NULL)
	, m_nFrameCount(0)
	, m_nCurrentFrame(0)
	, m_pDelayProperty(NULL)
	, m_nTimerId(0)
	, m_pImageManager(NULL)
	, m_pIcon(NULL)
{
	UNREFERENCED_PARAMETER(pReserved);
}

CXTPMarkupGdiPlusImage::~CXTPMarkupGdiPlusImage()
{
	Cleanup();
}

BOOL CXTPMarkupGdiPlusImage::LoadIcon(UINT nIconId, int nWidth)
{
	CXTPMarkupContext* pContext = m_pSite->GetMarkupContext();
	ASSERT(NULL != pContext);

	CXTPImageManager* pImageManager = pContext->GetImageManager();
	ASSERT(NULL != pImageManager);

	m_pIcon = pImageManager->GetImage(nIconId, nWidth);
	if (NULL != m_pIcon)
	{
		m_pImageManager = pImageManager;
		CMDTARGET_ADDREF(m_pImageManager);
		CMDTARGET_ADDREF(m_pIcon);
		m_bIsIcon = TRUE;

		m_imageSize.cx = m_pIcon->GetWidth();
		m_imageSize.cy = m_pIcon->GetHeight();
	}

	return NULL != m_pIcon;
}

#ifndef IS_INTRESOURCE
#	define IS_INTRESOURCE(x) (((ULONG_PTR)(x) >> 16) == 0)
#endif

BOOL CXTPMarkupGdiPlusImage::LoadIcon(const XTP_URI& uri, int nWidth)
{
	ASSERT(xtpUriRes == uri.type);

	LPCTSTR lpszResource = uri.res.lpName;
	if (!IS_INTRESOURCE(lpszResource) && lpszResource[0] == _T('#'))
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
	if (NULL == hBitmap)
	{
		hBitmap = CXTPImageManagerIcon::LoadBitmapFromResource(lpszResource, &bAlphaBitmap);
	}

	if (NULL != hBitmap)
	{
		CBitmap bmp;
		bmp.Attach(hBitmap);
		m_pImageManager->SetIcons(bmp, 0, 1, 0, xtpImageNormal, bAlphaBitmap);
	}
	else
	{
		m_pImageManager->SetIcon(uri.res.lpName, 0);
	}

	m_pIcon = m_pImageManager->GetImage(0, nWidth);
	if (NULL != m_pIcon)
	{
		CMDTARGET_ADDREF(m_pIcon);
		m_imageSize.cx = m_pIcon->GetWidth();
		m_imageSize.cy = m_pIcon->GetHeight();
		m_bIsIcon	   = TRUE;
	}

	return NULL != m_pIcon;
}

BOOL CXTPMarkupGdiPlusImage::LoadImage(const XTP_URI& uri)
{
	BOOL bSuccess = FALSE;

	IStreamPtr pImageStream;
	if (SUCCEEDED(XTPCreateReadOnlyStreamOnUri(&uri, &pImageStream)))
	{
		Gdiplus::GpImage* pImage = NULL;
		do
		{
			if (Gdiplus::Ok != GdipLoadImageFromStream(pImageStream, &pImage))
			{
				break;
			}

			UINT cx = 0;
			if (Gdiplus::Ok != GdipGetImageWidth(pImage, &cx))
			{
				break;
			}

			UINT cy = 0;
			if (Gdiplus::Ok != GdipGetImageHeight(pImage, &cy))
			{
				break;
			}

			m_imageSize.cx = static_cast<int>(cx);
			m_imageSize.cy = static_cast<int>(cy);
			m_pImage	   = pImage;
			pImage		   = NULL;

			LoadAnimationData();
			bSuccess = TRUE;
		} while (FALSE);

		if (NULL != pImage)
		{
			GdipDisposeImage(pImage);
		}
	}

	return bSuccess;
}

BOOL CXTPMarkupGdiPlusImage::Load(LPCWSTR lpszSource, int nWidth)
{
	BOOL bSuccess = FALSE;

	Cleanup();

	UINT nIconId = 0;
	if (ParseIconSource(lpszSource, nIconId))
	{
		bSuccess = LoadIcon(nIconId, nWidth);
	}
	else
	{
		XTP_URI uri;
		if (SUCCEEDED(XTPParseUri(XTP_CW2CT(lpszSource), &uri)))
		{
			bSuccess = LoadImage(uri);
			if (!bSuccess && xtpUriRes == uri.type)
			{
				bSuccess = LoadIcon(uri, nWidth);
			}
		}
	}

	return bSuccess;
}

CXTPMarkupGdiPlusImage::AnimationTimerData& AFX_CDECL CXTPMarkupGdiPlusImage::GetAnimationTimerData()
{
	static CXTPThreadLocal<AnimationTimerData> data;
	return *data;
}

BOOL AFX_CDECL CXTPMarkupGdiPlusImage::ParseIconSource(LPCWSTR lpszSource, UINT& nIconId)
{
	ASSERT(NULL != lpszSource);

	BOOL bSuccess = FALSE;

	LPCWSTR lpCh = lpszSource;
	while (L'\0' != *lpCh)
	{
		if (isdigit(*lpCh++))
		{
			bSuccess = TRUE;
		}
		else
		{
			bSuccess = FALSE;
			break;
		}
	}

	if (bSuccess)
	{
		nIconId = XTPToUInt(_wtol(lpszSource));
	}

	return bSuccess;
}

void CXTPMarkupGdiPlusImage::Cleanup()
{
	StopAnimation();

	m_imageSize.cx	= 0;
	m_imageSize.cy	= 0;
	m_nFrameCount	= 0;
	m_nCurrentFrame = 0;
	m_bIsIcon		= FALSE;

	if (NULL != m_pDelayProperty)
	{
		free(m_pDelayProperty);
		m_pDelayProperty = NULL;
	}

	if (NULL != m_pImage)
	{
		GdipDisposeImage(m_pImage);
		m_pImage = NULL;
	}

	CMDTARGET_RELEASE(m_pIcon);
	CMDTARGET_RELEASE(m_pImageManager);
}

void CXTPMarkupGdiPlusImage::LoadAnimationData()
{
	ASSERT(NULL != m_pImage);
	ASSERT(NULL == m_pDelayProperty);

	LPGUID lpgDimensions = NULL;

	do
	{
		// Obtain frame count.
		UINT nDimCount = 0;
		if (Gdiplus::Ok != GdipImageGetFrameDimensionsCount(m_pImage, &nDimCount) || 0 == nDimCount)
		{
			break;
		}

		lpgDimensions = new GUID[nDimCount];
		if (Gdiplus::Ok != GdipImageGetFrameDimensionsList(m_pImage, lpgDimensions, nDimCount))
		{
			break;
		}

		if (Gdiplus::Ok != GdipImageGetFrameCount(m_pImage, lpgDimensions, &m_nFrameCount))
		{
			break;
		}

		// Obtain frame delays.
		UINT nPropertyTagFrameDelaySize = 0;
		if (Gdiplus::Ok
				!= GdipGetPropertyItemSize(m_pImage, PropertyTagFrameDelay,
										   &nPropertyTagFrameDelaySize)
			|| 0 == nPropertyTagFrameDelaySize)
		{
			break;
		}

		m_pDelayProperty = reinterpret_cast<Gdiplus::PropertyItem*>(
			malloc(nPropertyTagFrameDelaySize));
		if (Gdiplus::Ok
			!= GdipGetPropertyItem(m_pImage, PropertyTagFrameDelay, nPropertyTagFrameDelaySize,
								   m_pDelayProperty))
		{
			free(m_pDelayProperty);
			m_pDelayProperty = NULL;
			break;
		}

		m_nCurrentFrame = 0;
		RenderAnimation();
	} while (FALSE);

	if (NULL != lpgDimensions)
	{
		delete[] lpgDimensions;
	}
}

void CXTPMarkupGdiPlusImage::StopAnimation()
{
	if (0 != m_nTimerId)
	{
		KillTimer(NULL, m_nTimerId);
		GetAnimationTimerData().RemoveKey(m_nTimerId);
		m_nTimerId = 0;
	}
}

void CXTPMarkupGdiPlusImage::RenderAnimation()
{
	ASSERT(NULL != m_pImage);
	ASSERT(NULL != m_pDelayProperty);

	StopAnimation();

	GdipImageSelectActiveFrame(m_pImage, &XTPFrameDimensionTime, m_nCurrentFrame);

	m_pSite->OnImageUpdateRequired();

	if (1 < m_nFrameCount)
	{
		UINT delay = reinterpret_cast<PUINT>(m_pDelayProperty->value)[m_nCurrentFrame] * 10;
		if (0 == delay)
		{
			delay = 100;
		}

		m_nCurrentFrame = ++m_nCurrentFrame % m_nFrameCount;
		m_nTimerId		= ::SetTimer(NULL, 0, delay, CXTPMarkupGdiPlusImage::OnAnimationTimer);
		GetAnimationTimerData()[m_nTimerId] = this;
	}
}

void CALLBACK CXTPMarkupGdiPlusImage::OnAnimationTimer(HWND hWnd, UINT uMsg, UINT_PTR nTimerId,
													   DWORD dwTime)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(dwTime);

	CXTPMarkupGdiPlusImage* pThis = NULL;
	if (GetAnimationTimerData().Lookup(nTimerId, pThis))
	{
		pThis->RenderAnimation();
	}
}
