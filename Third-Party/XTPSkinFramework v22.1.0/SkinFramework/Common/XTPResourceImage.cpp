// XTPResourceImage.cpp: implementation of the CXTPResourceImage class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceImage.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/IIDs/XTPResourceImagesIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

#define TILE_VERTICAL 1
#define TILE_HORIZONTAL 2
#define TILE_ALL 3

//---------------------------------------------------------------------------
//    Should be a prime number:
// 37, 53, 79 , 101, 127, 199, 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007, 16001,
// 32003, 48017, 64007
#define XTP_OFFICE2007_IMAGES_HASH_TABLE_SIZE 199

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPResourceImage::CXTPResourceImage(CXTPResourceImages* pImages)
{
	ASSERT_VALID(pImages);

	m_bAlpha	= FALSE;
	m_bInvert   = FALSE;
	m_bMultiply = FALSE;

	m_szExtent = CSize(0, 0);

	m_bImageOrientaionPreserved = FALSE;
	m_pImages					= pImages;

	m_pBits	= NULL;
	m_bFlipped = FALSE;
}

CXTPResourceImage::~CXTPResourceImage()
{
	if (m_pBits != NULL)
	{
		free(m_pBits);
	}
}

BOOL CXTPResourceImage::ConvertToBits(HBITMAP hBitmap)
{
	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	LPVOID pBits = NULL;

	if (m_bAlpha)
	{
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		if (GetDIBits(dcSrc, hBitmap, 0, 0, NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) == 0)
			return FALSE;

		if (bmi.bmiHeader.biBitCount != 32)
			return FALSE;

		size_t nSize = XTPToSizeTChecked(bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth) * 4;
		pBits		 = malloc(nSize);
		if (pBits == NULL)
			return FALSE;

		PBITMAPINFO pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER)
													  + 3 * sizeof(COLORREF));
		if (pBitmapInfo == NULL)
		{
			free(pBits);
			return FALSE;
		}

		MEMCPY_S(pBitmapInfo, &bmi, sizeof(BITMAPINFOHEADER));

		if (GetDIBits(dcSrc, hBitmap, 0, XTPToUIntChecked(bmi.bmiHeader.biHeight), pBits,
					  pBitmapInfo, DIB_RGB_COLORS)
			== NULL)
		{
			free(pBits);
			free(pBitmapInfo);
			return FALSE;
		}

		free(pBitmapInfo);
	}
	else
	{
		LPDWORD lpBits	= NULL;
		HBITMAP hDCBitmap = CXTPImageManager::Create32BPPDIBSection(NULL, m_szExtent.cx,
																	m_szExtent.cy,
																	(LPBYTE*)&lpBits);
		if (!lpBits)
			return FALSE;

		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(dcSrc, hDCBitmap);

		dcSrc.DrawState(CPoint(0, 0), m_szExtent, hBitmap, DSS_NORMAL, 0);

		::SelectObject(dcSrc, hOldBitmap);

		size_t nSize = XTPToSizeTChecked(m_szExtent.cx * m_szExtent.cy) * 4;
		pBits		 = malloc(nSize);
		if (!pBits)
			return FALSE;

		MEMCPY_S(pBits, lpBits, nSize);

		DeleteObject(hDCBitmap);
	}

	ASSERT(pBits);

	m_pBits = pBits;

	return TRUE;
}

void CXTPResourceImage::SetBitmap(HBITMAP hBitmap, BOOL bAlpha /*= FALSE*/)
{
	ASSERT(m_pBits == NULL);

	m_bAlpha	= bAlpha;
	m_bInvert   = FALSE;
	m_bMultiply = FALSE;

	BITMAP bmpInfo;
	if (!GetObject(hBitmap, sizeof(BITMAP), &bmpInfo))
		return;

	m_szExtent = CSize(bmpInfo.bmWidth, bmpInfo.bmHeight);

	VERIFY(ConvertToBits(hBitmap));

	DeleteObject(hBitmap);
}

BOOL CXTPResourceImage::LoadFile(HMODULE hModule, LPCTSTR lpszBitmapFileName)
{
	m_bMultiply = FALSE;
	m_bInvert   = FALSE;
	m_bAlpha	= FALSE;

	if (m_pBits)
	{
		free(m_pBits);
		m_pBits = NULL;
	}

	if (!hModule)
		return FALSE;

	BOOL bAlpha = FALSE;

	HBITMAP hBmp = CXTPImageManagerIcon::LoadBitmapFromResource(hModule, lpszBitmapFileName,
																&bAlpha);

	if (!hBmp)
		return FALSE;

	SetBitmap(hBmp, bAlpha);

	return TRUE;
}

void CXTPResourceImage::Premultiply()
{
	if (!m_bAlpha || !m_pBits || m_bMultiply)
		return;

	RGBQUAD* lprgbBitmap = (RGBQUAD*)m_pBits;
	int nCount			 = m_szExtent.cx * m_szExtent.cy;

	for (int i = 0; i < nCount; i++)
	{
		int nAlpha			  = lprgbBitmap->rgbReserved;
		lprgbBitmap->rgbBlue  = BYTE(lprgbBitmap->rgbBlue * nAlpha / 255);
		lprgbBitmap->rgbGreen = BYTE(lprgbBitmap->rgbGreen * nAlpha / 255);
		lprgbBitmap->rgbRed   = BYTE(lprgbBitmap->rgbRed * nAlpha / 255);
		lprgbBitmap++;
	}

	m_bMultiply = TRUE;
}

void CXTPResourceImage::DrawImageFlipped(BOOL bFlip)
{
	if (m_bFlipped == bFlip)
		return;

	m_bFlipped = bFlip;

	InvertBitmap();
	m_bInvert = !m_bInvert; // restore.
}

void CXTPResourceImage::InvertBitmap()
{
	if (!m_pBits)
		return;

	size_t cbSize	= XTPToSizeTChecked(m_szExtent.cx * m_szExtent.cy * 4);
	DWORD* pDest	 = (DWORD*)malloc(cbSize);
	DWORD* pDestOrig = pDest;
	DWORD* pBits	 = (DWORD*)m_pBits;

	for (int y = 0; y < m_szExtent.cy; y++)
	{
		DWORD* pBitsRow = &pBits[y * m_szExtent.cx + m_szExtent.cx - 1];

		for (int x = 0; x < m_szExtent.cx; x++)
		{
			*pDest++ = *pBitsRow--;
		}
	}

	free(m_pBits);
	m_pBits = pDestOrig;

	m_bInvert = !m_bInvert;
}

BOOL CXTPResourceImage::DrawImagePart(CDC* pDCDest, CRect rcDest, CDC* pDCSrc, CRect rcSrc) const
{
	ASSERT_VALID(pDCDest);
	ASSERT_VALID(pDCSrc);

	if (rcDest.Width() <= 0 || rcDest.Height() <= 0 || rcSrc.Width() <= 0 || rcSrc.Height() <= 0)
		return TRUE;

	BOOL bResult = TRUE;

	if (m_bAlpha)
	{
		bResult = XTPImageManager()->AlphaBlend(pDCDest->GetSafeHdc(), rcDest, pDCSrc->GetSafeHdc(),
												rcSrc);
	}
	else if ((rcSrc.Width() == rcDest.Width()) && (rcSrc.Height() == rcDest.Height()))
	{
		bResult = pDCDest->BitBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pDCSrc,
								  rcSrc.left, rcSrc.top, SRCCOPY);
	}
	else
	{
		bResult = pDCDest->StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
									  pDCSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(),
									  SRCCOPY);
	}

	return bResult;
}

void CXTPResourceImage::DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc,
								  const CRect& rcSizingMargins, COLORREF clrTransparent,
								  DWORD dwFlags)
{
	if (this == NULL)
		return;

	if (m_pBits == NULL)
		return;

	if (rcSizingMargins.top + rcSizingMargins.bottom > rcSrc.Height())
		return;

	if ((dwFlags & XTP_DI_COMPOSITE) == 0)
	{
		DrawImage(pDC, rcDest, rcSrc, rcSizingMargins, clrTransparent);
		return;
	}

	if (m_bAlpha)
	{
		DrawImage(pDC, rcDest, rcSrc, rcSizingMargins);
		return;
	}

	CSize sz(rcDest.Width(), rcDest.Height());
	CBitmap bmp;
	LPDWORD lpBits;
	bmp.Attach(
		CXTPImageManager::Create32BPPDIBSection(pDC->GetSafeHdc(), sz.cx, sz.cy, (LPBYTE*)&lpBits));

	CXTPCompatibleDC dc(pDC, &bmp);

	CRect rcDestOrig(0, 0, sz.cx, sz.cy);

	if (clrTransparent != COLORREF_NULL)
		dc.FillSolidRect(rcDestOrig, clrTransparent);
	DrawImage(&dc, rcDestOrig, rcSrc, rcSizingMargins);

	for (int i = 0; i < sz.cx * sz.cy; i++)
	{
		if (clrTransparent != COLORREF_NULL && lpBits[0] == clrTransparent)
		{
			lpBits[0] = 0;
		}
		else
		{
			lpBits[0] |= 0xFF000000;
		}
		lpBits++;
	}

	XTPImageManager()->AlphaBlend(*pDC, rcDest, dc, rcDestOrig);
}

void CXTPResourceImage::DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc,
								  const CRect& rcSizingMargins, COLORREF clrTransparent)
{
	if (this == NULL)
		return;

	if (m_pBits == NULL)
		return;

	if (rcSizingMargins.top + rcSizingMargins.bottom > rcSrc.Height())
		return;

	if (clrTransparent == COLORREF_NULL || m_bAlpha)
	{
		DrawImage(pDC, rcDest, rcSrc, rcSizingMargins);
		return;
	}

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcDest.Width(), rcDest.Height());

	CXTPCompatibleDC dc(pDC, &bmp);
	// dc.SetLayout(0);

	CRect rcDestOrig(0, 0, rcDest.Width(), rcDest.Height());

	dc.FillSolidRect(rcDestOrig, clrTransparent);
	DrawImage(&dc, rcDestOrig, rcSrc, rcSizingMargins);

	XTPImageManager()->TransparentBlt(*pDC, rcDest, dc, rcDestOrig, clrTransparent);
}

void CXTPResourceImage::DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc)
{
	DrawImage(pDC, rcDest, rcSrc, CRect(0, 0, 0, 0));
}

BOOL CXTPResourceImage::GetBitmapPixel(CPoint pt, COLORREF& clrPixel)
{
	if (pt.x < 0 || pt.x >= m_szExtent.cx || pt.y < 0 || pt.y >= m_szExtent.cy)
		return FALSE;

	// Initialize the surface pointers.
	LPDWORD lprgbBitmap = (LPDWORD)m_pBits + m_szExtent.cx * (m_szExtent.cy - pt.y - 1);
	clrPixel			= lprgbBitmap[pt.x];

	return TRUE;
}

void CXTPResourceImage::DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc, CRect rcSizingMargins)
{
	if (this == NULL)
		return;

	if (m_pBits == NULL)
		return;

	if (rcSizingMargins.top + rcSizingMargins.bottom > rcSrc.Height())
		return;

	ASSERT(m_pImages);
	if (!m_pImages)
		return;

	CXTPLockGuard lock(m_pImages->m_cs);

	if (rcDest.Width() == rcSrc.Width())
	{
		rcSizingMargins.left  = 0;
		rcSizingMargins.right = 0;
	}

	if (rcDest.Height() == rcSrc.Height())
	{
		rcSizingMargins.top	= 0;
		rcSizingMargins.bottom = 0;
	}

	pDC->SetStretchBltMode(COLORONCOLOR);

	CRect rcDestSizingMargins = rcSizingMargins;

	if (rcDest.left >= rcDest.right || rcDest.top >= rcDest.bottom)
		return;

	BOOL bRtl = CXTPDrawHelpers::IsContextRTL(pDC);

	if (m_bAlpha && m_bImageOrientaionPreserved)
	{
		if ((bRtl && !m_bInvert) || (!bRtl && m_bInvert))
		{
			InvertBitmap();
		}
	}

	LPDWORD lpLayerBits = NULL;
	SIZE szLayerSize;
	HBITMAP hbmpLayer = m_pImages->AllocateLayerBitmap(XTPToUIntChecked(m_szExtent.cx),
													   XTPToUIntChecked(m_szExtent.cy),
													   &lpLayerBits, &szLayerSize);
	if (NULL == hbmpLayer)
		return;

	CXTPBitmapDC* dcSrc = m_pImages->m_pdc;
	if (NULL == dcSrc->GetSafeHdc())
		return;

	LPDWORD lpBitsSrc  = ((LPDWORD)m_pBits) + (m_szExtent.cy - rcSrc.bottom) * m_szExtent.cx;
	LPDWORD lpBitsDest = lpLayerBits + szLayerSize.cx * (szLayerSize.cy - rcSrc.Height());
	rcSrc.OffsetRect(0, -rcSrc.top);

	for (int y = 0; y < rcSrc.bottom; y++)
	{
		MEMCPY_S(lpBitsDest, lpBitsSrc, XTPToSizeTChecked(m_szExtent.cx) * 4);
		lpBitsSrc += m_szExtent.cx;
		lpBitsDest += szLayerSize.cx;
	}

	dcSrc->SetBitmap(hbmpLayer);

	if ((rcDest.Height() <= rcSizingMargins.top + rcSizingMargins.bottom)
		|| (rcSrc.Height() == rcSizingMargins.top + rcSizingMargins.bottom))
	{
		rcDestSizingMargins.top	= MulDiv(rcDest.Height(), rcSizingMargins.top,
											(rcSizingMargins.top + rcSizingMargins.bottom));
		rcDestSizingMargins.bottom = rcDest.Height() - rcDestSizingMargins.top;

		if (rcDestSizingMargins.bottom > 0
			&& (rcDestSizingMargins.bottom % 2 != rcSizingMargins.bottom % 2))
			rcDestSizingMargins.bottom++;
	}

	if ((rcDest.Width() <= rcSizingMargins.left + rcSizingMargins.right)
		|| (rcSrc.Width() == rcSizingMargins.left + rcSizingMargins.right))
	{
		rcDestSizingMargins.left = rcDest.Width() * rcSizingMargins.left
								   / (rcSizingMargins.left + rcSizingMargins.right);
		rcDestSizingMargins.right = rcDest.Width() - rcDestSizingMargins.left;
	}

	if (rcSizingMargins.IsRectNull())
	{
		DrawImagePart(pDC, rcDest, dcSrc, rcSrc);
	}
	else if (bRtl && m_bAlpha)
	{
		DrawImagePart(pDC,
					  CRect(rcDest.right - rcDestSizingMargins.left, rcDest.top, rcDest.right,
							rcDest.top + rcDestSizingMargins.top),
					  dcSrc,
					  CRect(rcSrc.left, rcSrc.top, rcSrc.left + rcSizingMargins.left,
							rcSrc.top + rcSizingMargins.top));
		DrawImagePart(pDC,
					  CRect(rcDest.left + rcDestSizingMargins.right, rcDest.top,
							rcDest.right - rcDestSizingMargins.left,
							rcDest.top + rcDestSizingMargins.top),
					  dcSrc,
					  CRect(rcSrc.left + rcSizingMargins.left, rcSrc.top,
							rcSrc.right - rcSizingMargins.right, rcSrc.top + rcSizingMargins.top));
		DrawImagePart(pDC,
					  CRect(rcDest.left, rcDest.top, rcDest.left + rcDestSizingMargins.right,
							rcDest.top + rcDestSizingMargins.top),
					  dcSrc,
					  CRect(rcSrc.right - rcSizingMargins.right, rcSrc.top, rcSrc.right,
							rcSrc.top + rcSizingMargins.top));

		DrawImagePart(pDC,
					  CRect(rcDest.right - rcDestSizingMargins.left,
							rcDest.top + rcDestSizingMargins.top, rcDest.right,
							rcDest.bottom - rcDestSizingMargins.bottom),
					  dcSrc,
					  CRect(rcSrc.left, rcSrc.top + rcSizingMargins.top,
							rcSrc.left + rcSizingMargins.left,
							rcSrc.bottom - rcSizingMargins.bottom));

		DrawImagePart(pDC,
					  CRect(rcDest.left + rcDestSizingMargins.right,
							rcDest.top + rcDestSizingMargins.top,
							rcDest.right - rcDestSizingMargins.left,
							rcDest.bottom - rcDestSizingMargins.bottom),
					  dcSrc,
					  CRect(rcSrc.left + rcSizingMargins.left, rcSrc.top + rcSizingMargins.top,
							rcSrc.right - rcSizingMargins.right,
							rcSrc.bottom - rcSizingMargins.bottom));

		DrawImagePart(pDC,
					  CRect(rcDest.left, rcDest.top + rcDestSizingMargins.top,
							rcDest.left + rcDestSizingMargins.right,
							rcDest.bottom - rcDestSizingMargins.bottom),
					  dcSrc,
					  CRect(rcSrc.right - rcSizingMargins.right, rcSrc.top + rcSizingMargins.top,
							rcSrc.right, rcSrc.bottom - rcSizingMargins.bottom));

		DrawImagePart(pDC,
					  CRect(rcDest.right - rcDestSizingMargins.left,
							rcDest.bottom - rcDestSizingMargins.bottom, rcDest.right,
							rcDest.bottom),
					  dcSrc,
					  CRect(rcSrc.left, rcSrc.bottom - rcSizingMargins.bottom,
							rcSrc.left + rcSizingMargins.left, rcSrc.bottom));
		DrawImagePart(pDC,
					  CRect(rcDest.left + rcDestSizingMargins.right,
							rcDest.bottom - rcDestSizingMargins.bottom,
							rcDest.right - rcDestSizingMargins.left, rcDest.bottom),
					  dcSrc,
					  CRect(rcSrc.left + rcSizingMargins.left,
							rcSrc.bottom - rcSizingMargins.bottom,
							rcSrc.right - rcSizingMargins.right, rcSrc.bottom));
		DrawImagePart(pDC,
					  CRect(rcDest.left, rcDest.bottom - rcDestSizingMargins.bottom,
							rcDest.left + rcDestSizingMargins.right, rcDest.bottom),
					  dcSrc,
					  CRect(rcSrc.right - rcSizingMargins.right,
							rcSrc.bottom - rcSizingMargins.bottom, rcSrc.right, rcSrc.bottom));
	}
	else
	{
		DrawImagePart(pDC,
					  CRect(rcDest.left, rcDest.top, rcDest.left + rcDestSizingMargins.left,
							rcDest.top + rcDestSizingMargins.top),
					  dcSrc,
					  CRect(rcSrc.left, rcSrc.top, rcSrc.left + rcSizingMargins.left,
							rcSrc.top + rcSizingMargins.top));
		DrawImagePart(pDC,
					  CRect(rcDest.left + rcDestSizingMargins.left, rcDest.top,
							rcDest.right - rcDestSizingMargins.right,
							rcDest.top + rcDestSizingMargins.top),
					  dcSrc,
					  CRect(rcSrc.left + rcSizingMargins.left, rcSrc.top,
							rcSrc.right - rcSizingMargins.right, rcSrc.top + rcSizingMargins.top));
		DrawImagePart(pDC,
					  CRect(rcDest.right - rcDestSizingMargins.right, rcDest.top, rcDest.right,
							rcDest.top + rcDestSizingMargins.top),
					  dcSrc,
					  CRect(rcSrc.right - rcSizingMargins.right, rcSrc.top, rcSrc.right,
							rcSrc.top + rcSizingMargins.top));

		DrawImagePart(pDC,
					  CRect(rcDest.left, rcDest.top + rcDestSizingMargins.top,
							rcDest.left + rcDestSizingMargins.left,
							rcDest.bottom - rcDestSizingMargins.bottom),
					  dcSrc,
					  CRect(rcSrc.left, rcSrc.top + rcSizingMargins.top,
							rcSrc.left + rcSizingMargins.left,
							rcSrc.bottom - rcSizingMargins.bottom));

		DrawImagePart(pDC,
					  CRect(rcDest.left + rcDestSizingMargins.left,
							rcDest.top + rcDestSizingMargins.top,
							rcDest.right - rcDestSizingMargins.right,
							rcDest.bottom - rcDestSizingMargins.bottom),
					  dcSrc,
					  CRect(rcSrc.left + rcSizingMargins.left, rcSrc.top + rcSizingMargins.top,
							rcSrc.right - rcSizingMargins.right,
							rcSrc.bottom - rcSizingMargins.bottom));

		DrawImagePart(pDC,
					  CRect(rcDest.right - rcDestSizingMargins.right,
							rcDest.top + rcDestSizingMargins.top, rcDest.right,
							rcDest.bottom - rcDestSizingMargins.bottom),
					  dcSrc,
					  CRect(rcSrc.right - rcSizingMargins.right, rcSrc.top + rcSizingMargins.top,
							rcSrc.right, rcSrc.bottom - rcSizingMargins.bottom));

		DrawImagePart(pDC,
					  CRect(rcDest.left, rcDest.bottom - rcDestSizingMargins.bottom,
							rcDest.left + rcDestSizingMargins.left, rcDest.bottom),
					  dcSrc,
					  CRect(rcSrc.left, rcSrc.bottom - rcSizingMargins.bottom,
							rcSrc.left + rcSizingMargins.left, rcSrc.bottom));
		DrawImagePart(pDC,
					  CRect(rcDest.left + rcDestSizingMargins.left,
							rcDest.bottom - rcDestSizingMargins.bottom,
							rcDest.right - rcDestSizingMargins.right, rcDest.bottom),
					  dcSrc,
					  CRect(rcSrc.left + rcSizingMargins.left,
							rcSrc.bottom - rcSizingMargins.bottom,
							rcSrc.right - rcSizingMargins.right, rcSrc.bottom));
		DrawImagePart(pDC,
					  CRect(rcDest.right - rcDestSizingMargins.right,
							rcDest.bottom - rcDestSizingMargins.bottom, rcDest.right,
							rcDest.bottom),
					  dcSrc,
					  CRect(rcSrc.right - rcSizingMargins.right,
							rcSrc.bottom - rcSizingMargins.bottom, rcSrc.right, rcSrc.bottom));
	}

	dcSrc->SetBitmap(NULL);
}

CSize CXTPResourceImage::GetExtent() const
{
	return m_szExtent;
}

CRect CXTPResourceImage::GetSource(int nState, int nCount) const
{
	if (this == NULL)
		return CRect(0, 0, 0, 0);

	CRect rcImage(0, 0, m_szExtent.cx, m_szExtent.cy / nCount);
	rcImage.OffsetRect(0, nState * rcImage.Height());

	return rcImage;
}

int CXTPResourceImage::GetHeight() const
{
	return m_szExtent.cy;
}

int CXTPResourceImage::GetWidth() const
{
	return m_szExtent.cx;
}

BOOL CXTPResourceImage::IsAlphaImage() const
{
	return m_bAlpha;
}

void CXTPResourceImage::PreserveOrientation()
{
	m_bImageOrientaionPreserved = TRUE;
}

CXTPResourceImages* XTPResourceImages()
{
	return &CXTPSingleton<CXTPResourceImages>::Instance();
}

CXTPResourceImage* XTPLoadImage(LPCTSTR lpszImageFile)
{
	if (!XTPResourceImages()->m_strIniFileName.IsEmpty())
	{
		return XTPResourceImages()->LoadFile(lpszImageFile);
	}

	return NULL;
}

COLORREF XTPIniColor(LPCTSTR lpszSection, LPCTSTR lpszKey, COLORREF clrDef)
{
	if (!XTPResourceImages()->m_strIniFileName.IsEmpty())
	{
		return XTPResourceImages()->GetImageColor(lpszSection, lpszKey, clrDef);
	}

	return clrDef;
}

int XTPIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nDef)
{
	if (!XTPResourceImages()->m_strIniFileName.IsEmpty())
	{
		return XTPResourceImages()->GetImageInt(lpszSection, lpszKey, nDef);
	}

	return nDef;
}

_XTP_EXT_CLASS CRect AFX_CDECL XTPIniRect(LPCTSTR lpszSection, LPCTSTR lpszKey, CRect rcDef)
{
	if (!XTPResourceImages()->m_strIniFileName.IsEmpty())
	{
		return XTPResourceImages()->GetImageRect(lpszSection, lpszKey, rcDef);
	}

	return rcDef;
}

_XTP_EXT_CLASS CSize AFX_CDECL XTPIniSize(LPCTSTR lpszSection, LPCTSTR lpszKey, CSize szDef)
{
	if (!XTPResourceImages()->m_strIniFileName.IsEmpty())
	{
		return XTPResourceImages()->GetImageSize(lpszSection, lpszKey, szDef);
	}

	return szDef;
}

_XTP_EXT_CLASS CString AFX_CDECL XTPIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszDef)
{
	if (!XTPResourceImages()->m_strIniFileName.IsEmpty())
	{
		return XTPResourceImages()->GetImageString(lpszSection, lpszKey, lpszDef);
	}

	return lpszDef;
}

//////////////////////////////////////////////////////////////////////////
// CXTPResourceImages

CXTPResourceImages::CXTPResourceImages()
{
	m_mapImages.InitHashTable(XTP_OFFICE2007_IMAGES_HASH_TABLE_SIZE, FALSE);
	m_mapPropeties.InitHashTable(101, FALSE);

	m_bFreeOnRelease	 = FALSE;
	m_hResource			 = 0;
	m_pConnection		 = NULL;
	m_bPremultiplyImages = FALSE;
	m_bLoadedNothing	 = FALSE;
	m_hbmLayer			 = NULL;
	m_pLayerBits		 = NULL;

	SetHandle(XTPGetInstanceHandle());

	HDC hdcScreen = ::GetDC(HWND_DESKTOP);

	m_pdc = new CXTPBitmapDC();
	m_pdc->Attach(::CreateCompatibleDC(hdcScreen));

	ReleaseDC(HWND_DESKTOP, hdcScreen);

	::InitializeCriticalSection(&m_cs);

#ifdef _XTP_ACTIVEX
	EnableAutomation();
	EnableTypeLib();
#endif
}

CXTPResourceImages::~CXTPResourceImages()
{
	SAFE_DELETE(m_pdc);
	CMDTARGET_RELEASE(m_pConnection);

	RemoveAll();

	if (m_hResource && m_bFreeOnRelease)
	{
		FreeLibrary(m_hResource);
	}

	if (m_hbmLayer)
	{
		DeleteObject(m_hbmLayer);
	}

	::DeleteCriticalSection(&m_cs);
}

CXTPNotifyConnection* CXTPResourceImages::GetConnection()
{
	if (!m_pConnection)
		m_pConnection = new CXTPNotifyConnection();

	return m_pConnection;
}

void CXTPResourceImages::RemoveAll()
{
	CString strImageFile;
	CXTPResourceImage* pImage;

	POSITION pos = m_mapImages.GetStartPosition();
	while (pos != NULL)
	{
		m_mapImages.GetNextAssoc(pos, strImageFile, (void*&)pImage);
		CMDTARGET_RELEASE(pImage);
	}
	m_mapImages.RemoveAll();
}

CString CXTPResourceImages::_ImageNameToResourceName(LPCTSTR lpszImageFile)
{
	CString strImage = GetImageString(_T("Images"), lpszImageFile);

	if (strImage.IsEmpty())
	{
		strImage = lpszImageFile;
	}

	strImage.Replace(_T('.'), _T('_'));
	strImage.Replace(_T('\\'), _T('_'));

	return strImage;
}

CXTPResourceImage* CXTPResourceImages::LoadImage(LPCTSTR lpszImageFile)
{
	ASSERT(NULL != lpszImageFile);
	CString strImageFile = _ImageNameToResourceName(lpszImageFile);

	CXTPResourceImage* pImage = new CXTPResourceImage(this);

	if (!pImage->LoadFile(m_hResource, strImageFile))
	{
		delete pImage;
		return NULL;
	}

	return pImage;
}

CXTPResourceImage* CXTPResourceImages::LoadFile(LPCTSTR lpszImageFile)
{
	ASSERT(NULL != lpszImageFile);
	if (lpszImageFile == NULL)
		return NULL;

	CXTPLockGuard lock(m_cs);

	CXTPResourceImage* pImage = 0;
	if (m_mapImages.Lookup(lpszImageFile, (void*&)pImage))
	{
		if (NULL != pImage)
		{
			return pImage;
		}
	}

	pImage = LoadImage(lpszImageFile);
	if (!pImage)
	{
		m_mapImages.SetAt(lpszImageFile, NULL);
		return NULL;
	}

	if (m_bPremultiplyImages)
	{
		pImage->Premultiply();
	}

	m_mapImages.SetAt(lpszImageFile, pImage);

	return pImage;
}

BOOL CXTPResourceImages::LoadResources(HRSRC hRsrc, BOOL bAllowOverwrite)
{
	BOOL bResult = FALSE;

	if (hRsrc)
	{
		HGLOBAL hResData = ::LoadResource(m_hResource, hRsrc);
		if (hResData)
		{
			bResult = TRUE;

			LPSTR lpTextFile	= (LPSTR)::LockResource(hResData);
			LPSTR lpTextFileEnd = lpTextFile + ::SizeofResource(m_hResource, hRsrc);

			LoadProperties(lpTextFile, lpTextFileEnd, bAllowOverwrite);
		}
	}

	return bResult;
}

BOOL CXTPResourceImages::LoadResources(LPCTSTR lpszIniFileName)
{
	RemoveAll();
	ClearProperties();

	HRSRC hRsrc = NULL;

	if (lpszIniFileName != NULL)
	{
		m_strIniFileName = lpszIniFileName;
		m_strIniFileName.Replace(_T('.'), _T('_'));

		hRsrc = ::FindResource(m_hResource, m_strIniFileName, _T("TEXTFILE"));
	}
	else
	{
		hRsrc = ::FindResource(m_hResource, xtpIniOffice2007Blue, _T("TEXTFILE"));

		if (!hRsrc)
		{
			hRsrc = FindResourceIniFile(m_hResource);
		}
	}

	BOOL bResult = LoadResources(hRsrc, FALSE);

	if (m_pConnection)
		m_pConnection->SendEvent(XTP_NC_COMMON_RESOURCEIMAGES_CHANGED, 0, 0);

	m_bPremultiplyImages = GetImageInt(_T("Global"), _T("PremultiplyImages"), 0);

	return bResult;
}

HMODULE CXTPResourceImages::GetHandle() const
{
	return m_hResource;
}

BOOL CXTPResourceImages::SetHandle(LPCTSTR lpszDllFileName, LPCTSTR lpszIniFileName)
{
	CString strDllFileName(lpszDllFileName);
	CString strIniFileName(lpszIniFileName);

	if (!lpszIniFileName)
	{
		int nIndex = strDllFileName.Find(_T(','));
		if (nIndex != -1)
		{
			strIniFileName = strDllFileName.Mid(nIndex + 1);
			strDllFileName = strDllFileName.Left(nIndex);
		}
	}

	// if the theme DLL exists try loading it and setting the resource
	// to the specified INI file.
	if (FILEEXISTS_S(strDllFileName) && !strIniFileName.IsEmpty())
	{
		HMODULE hModule = ::LoadLibrary(strDllFileName);

		if (hModule)
		{
			m_strDllFileName = strDllFileName;
			return SetHandle(hModule, strIniFileName, TRUE);
		}
	}

	// If an INI file is specified try loading it from the current instance handle.
	if (!strIniFileName.IsEmpty())
	{
		return SetHandle(XTPGetInstanceHandle(), strIniFileName);
	}

#ifdef _XTP_ACTIVEX
	m_bLoadedNothing = TRUE;
#endif

	// Use defaults.
	return SetHandle(XTPGetInstanceHandle());
}

BOOL CXTPResourceImages::IsValid() const
{
	return !m_mapPropeties.IsEmpty();
}

BOOL CALLBACK CXTPResourceImages::EnumResNameProc(HMODULE /*hModule*/, LPCTSTR /*lpszType*/,
												  LPTSTR lpszName, LONG_PTR lParam)
{
	CString* pIniFileName = (CString*)lParam;
	*pIniFileName		  = lpszName;

	return FALSE;
}

HRSRC CXTPResourceImages::FindResourceIniFile(HMODULE hResource)
{
	CString strIniFileName;
	::EnumResourceNames(hResource, _T("TEXTFILE"), &CXTPResourceImages::EnumResNameProc,
						(LONG_PTR)&strIniFileName);

	if (strIniFileName.IsEmpty())
		return 0;

	return ::FindResource(m_hResource, strIniFileName, _T("TEXTFILE"));
}

BOOL CXTPResourceImages::SetHandle(HMODULE hResource, LPCTSTR lpszThemeFileName,
								   LPCTSTR lpszAppFileName, BOOL bFreeOnRelease)
{
	if (m_hResource && m_bFreeOnRelease)
	{
		::FreeLibrary(m_hResource);
	}

	m_hResource		 = hResource;
	m_bFreeOnRelease = bFreeOnRelease;

	BOOL bTheme = LoadResources(lpszThemeFileName);
	BOOL bApp   = TRUE;

	if (NULL != lpszAppFileName)
	{
		CString strAppFileName = lpszAppFileName;
		strAppFileName.Replace(_T('.'), _T('_'));

		HRSRC hRsrc = ::FindResource(m_hResource, strAppFileName, _T("TEXTFILE"));

		if (NULL != hRsrc)
		{
			bApp = LoadResources(hRsrc, TRUE);
		}
	}

	if (NULL != lpszThemeFileName)
	{
		XT_ASSERT_MSG(bTheme && bApp,
					  "WARNING: Invalid resource handle, the resources for the specified theme "
					  "could not be loaded.  Verify that your theme's resource DLL is in the "
					  "correct location.\n\nDefault settings will be used however your application "
					  "may not display correctly.");
	}

	return bTheme && bApp;
}

BOOL CXTPResourceImages::SetHandle(HMODULE hResource, LPCTSTR lpszIniFileName, BOOL bFreeOnRelease)
{
	return SetHandle(hResource, lpszIniFileName, NULL, bFreeOnRelease);
}

void CXTPResourceImages::SetIniData(LPCSTR lpIniData)
{
	ASSERT(NULL != lpIniData);

	RemoveAll();
	ClearProperties();

	LPCSTR lpIniDataEnd = lpIniData + strlen(lpIniData);
	LoadProperties(const_cast<LPSTR>(lpIniData), const_cast<LPSTR>(lpIniDataEnd), TRUE);
}

BOOL CXTPResourceImages::InitResourceHandle(LPCTSTR lpszTestImageFile, LPCTSTR lpResType)
{
	CString strResourceName = _ImageNameToResourceName(lpszTestImageFile);

	HMODULE hResource = XTPResourceManager()->GetResourceHandle();
	if (hResource == 0)
		hResource = XTPGetInstanceHandle();

	if (!::FindResource(hResource, strResourceName, lpResType))
	{
		hResource = AfxFindResourceHandle(strResourceName, lpResType);
		if (!hResource)
			return FALSE;
	}

	SetHandle(hResource);
	return TRUE;
}

void CXTPResourceImages::AssertValid() const
{
#if defined(_DEBUG) || defined(_AFXDLL)
	CXTPCmdTarget::AssertValid();
#else
	ASSERT(NULL != this);
#endif
}

BOOL CXTPResourceImages::LoadBitmap(CBitmap* pBitmap, LPCTSTR lpszResourceName)
{
	if (::FindResource(m_hResource, lpszResourceName, RT_BITMAP))
	{
		if (pBitmap->Attach(::LoadBitmap(m_hResource, lpszResourceName)))
			return TRUE;
	}

	if (pBitmap->LoadBitmap(lpszResourceName))
		return TRUE;

	return FALSE;
}

COLORREF CXTPResourceImages::GetImageColor(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
										   COLORREF clrDefault) const
{
	CString strValue = GetImageValue(lpszSectionName, lpszKeyName);
	if (strValue.IsEmpty())
		return clrDefault;

	if (strValue == _T("-1"))
		return COLORREF_NULL;

	int r = 0, g = 0, b = 0;
	int n = 0;
	if (3 != SCANF_S(strValue, _T("%i %i %i%n"), &r, &g, &b, &n) || n != strValue.GetLength())
	{
		ASSERT(!"Invalid RGB string provided");
		r = 0, g = 0, b = 0;
	}

	COLORREF clrVal = RGB(r, g, b);
	return clrVal;
}

CRect CXTPResourceImages::GetImageRect(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
									   CRect rcDefault) const
{
	CString strValue = GetImageValue(lpszSectionName, lpszKeyName);
	if (strValue.IsEmpty())
		return rcDefault;

	int l = 0, r = 0, t = 0, b = 0;
	int n = 0;
	if (4 != SCANF_S(strValue, _T("%i %i %i %i%n"), &l, &t, &r, &b, &n)
		|| n != strValue.GetLength())
	{
		ASSERT(!"Invalid RECT string provided");
		l = 0, r = 0, t = 0, b = 0;
	}

	return CRect(l, t, r, b);
}

CSize CXTPResourceImages::GetImageSize(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
									   CSize szDefault) const
{
	CString strValue = GetImageValue(lpszSectionName, lpszKeyName);
	if (strValue.IsEmpty())
		return szDefault;

	int cx = 0, cy = 0;
	int n = 0;
	if (2 != SCANF_S(strValue, _T("%i %i%n"), &cx, &cy, &n) || n != strValue.GetLength())
	{
		ASSERT(!"Invalid SIZE string provided");
		cx = 0, cy = 0;
	}

	return CSize(cx, cy);
}

int CXTPResourceImages::GetImageInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
									int nDefault) const
{
	CString strValue = GetImageValue(lpszSectionName, lpszKeyName);
	if (strValue.IsEmpty())
		return nDefault;

	int nVal = 0;
	int n	= 0;
	if (1 != SCANF_S(strValue, _T("%i%n"), &nVal, &n) || n != strValue.GetLength())
	{
		ASSERT(!"Invalid integer string provided");
		nVal = 0;
	}

	return nVal;
}

CString CXTPResourceImages::GetImageString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
										   LPCTSTR lpszDef /*=NULL*/) const
{
	CString strValue = GetImageValue(lpszSectionName, lpszKeyName);
	if (strValue.IsEmpty())
		return lpszDef;

	return strValue;
}

UINT CXTPResourceImages::GetKeyHash(LPCTSTR lpszSection, LPCTSTR lpszKeyName) const
{
	if (!lpszSection || !lpszKeyName)
		return 0;

	UINT nHash = 0;

	int i;

	for (i = 0; lpszSection[i] != 0; i++)
	{
		UINT c = XTPToUInt(_totupper(static_cast<wint_t>(lpszSection[i])));
		nHash  = (nHash << 5) + nHash + c;
	}

	for (i = 0; lpszKeyName[i] != 0; i++)
	{
		UINT c = XTPToUInt(_totupper(static_cast<wint_t>(lpszKeyName[i])));
		nHash  = (nHash << 5) + nHash + c;
	}

	return nHash;
}

HBITMAP CXTPResourceImages::AllocateLayerBitmap(UINT cx, UINT cy, LPDWORD* lpBits /*= NULL*/,
												SIZE* pSize /*= NULL*/)
{
	// Check if the existing bitmap has to be resized.
	if (NULL != m_hbmLayer)
	{
		BITMAP bmp;
		if (0 < GetObject(m_hbmLayer, sizeof(bmp), &bmp))
		{
			if ((UINT)bmp.bmWidth < cx || (UINT)bmp.bmHeight < cy)
			{
				DeleteObject(m_hbmLayer);
				m_hbmLayer = NULL;
			}
			else if (NULL != pSize)
			{
				pSize->cx = bmp.bmWidth;
				pSize->cy = bmp.bmHeight;
			}
		}
		else
		{
			DeleteObject(m_hbmLayer);
			m_hbmLayer = NULL;
		}
	}

	// Allocate new bitmap.
	if (NULL == m_hbmLayer)
	{
		m_pLayerBits = NULL;

		// Optimize bitmap size (round up to the next power of 2).
		UINT size = __max(cx, cy);
		--size;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		++size;

		m_hbmLayer = CXTPImageManager::Create32BPPDIBSection(NULL, XTPToIntChecked(size),
															 XTPToIntChecked(size),
															 (LPBYTE*)&m_pLayerBits);

		if (NULL != pSize)
		{
			pSize->cx = pSize->cy = (NULL != m_hbmLayer ? XTPToIntChecked(size) : 0);
		}
	}

	if (NULL != lpBits)
	{
		*lpBits = m_pLayerBits;
	}

	return m_hbmLayer;
}

CString CXTPResourceImages::GetImageValue(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName) const
{
	CString strReturn = _T("");

	if (_tcslen(lpszSectionName) == 0 || _tcslen(lpszKeyName) == 0)
	{
		return strReturn;
	}

	if (!m_mapPropeties.IsEmpty())
	{
		UINT nHash = GetKeyHash(lpszSectionName, lpszKeyName);

		CString strValue;
		if (m_mapPropeties.Lookup(nHash, strValue))
		{
			strReturn = strValue;
		}
	}
	/*
		if (strReturn.IsEmpty() && !m_strIniFileName.IsEmpty())
		{
			TRACE3("WARNING! File: '%s', Section: [%s], Value: '%s' was not defined.\n",
				m_strIniFileName, lpszSectionName, lpszKeyName);
		}
	*/
	return strReturn;
}

void CXTPResourceImages::ClearProperties()
{
	m_mapPropeties.RemoveAll();
}

BOOL CXTPResourceImages::ReadString(CString& str, LPSTR& lpTextFile, LPSTR lpTextFileEnd)
{
	ASSERT(lpTextFile);
	if (!lpTextFile)
		return FALSE;

	LPSTR pNextLine = strchr(lpTextFile, '\r');

	if (pNextLine == NULL || pNextLine > lpTextFileEnd)
		return FALSE;

	str		   = CString(lpTextFile, (int)(pNextLine - lpTextFile));
	lpTextFile = pNextLine + 1;
	if (lpTextFile[0] == '\n')
		lpTextFile = lpTextFile + 1;

	return TRUE;
}

void CXTPResourceImages::LoadProperties(LPSTR lpTextFile, LPSTR lpTextFileEnd, BOOL bAllowOverwrite)
{
	UNREFERENCED_PARAMETER(bAllowOverwrite);

	CString str;

	CString strSection;

	while (ReadString(str, lpTextFile, lpTextFileEnd))
	{
		str.TrimLeft();

		int nCommentIndex = str.Find(_T(';'));

		if (nCommentIndex == 0)
			continue;

		if (nCommentIndex > 0)
		{
			str = str.Left(nCommentIndex);
		}

		if (str.IsEmpty())
			continue;

		if (str[0] == _T('['))
		{
			str.TrimRight();

			strSection = str.Mid(1, str.GetLength() - 2);
		}
		else if (!strSection.IsEmpty())
		{
			int nPos = str.Find(_T('='));
			if (nPos <= 0)
				continue;

			CString strProperty = str.Left(nPos);
			strProperty.TrimRight();

			CString strValue = str.Mid(nPos + 1);
			strValue.TrimLeft();
			strValue.TrimRight();

			UINT nHash = GetKeyHash(strSection, strProperty);

#ifdef _DEBUG
			if (!bAllowOverwrite)
			{
				CString strOldValue;
				ASSERT(!m_mapPropeties.Lookup(nHash, strOldValue));
			}
#endif
			m_mapPropeties[nHash] = strValue;
		}
	}
}

void CXTPResourceImages::Reset()
{
	RemoveAll();
	ClearProperties();
}

#ifdef _XTP_ACTIVEX

BEGIN_DISPATCH_MAP(CXTPResourceImages, CXTPCmdTarget)
	DISP_FUNCTION_ID(CXTPResourceImages, "LoadFromFile", 1, OleLoadFromFile, VT_BOOL,
					 VTS_BSTR VTS_BSTR)
	DISP_PROPERTY_EX_ID(CXTPResourceImages, "DllFileName", 2, OleGetDllFileName, SetNotSupported,
						VT_BSTR)
	DISP_PROPERTY_EX_ID(CXTPResourceImages, "IniFileName", 3, OleGetIniFileName, SetNotSupported,
						VT_BSTR)
	DISP_FUNCTION_ID(CXTPResourceImages, "GetColor", 4, OleGetGetColor, VT_COLOR, VTS_BSTR VTS_BSTR)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CXTPResourceImages, CXTPCmdTarget)
	INTERFACE_PART(CXTPResourceImages, XTPDIID_ResourceImages, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPResourceImages, XTPDIID_ResourceImages)

BOOL CXTPResourceImages::OleLoadFromFile(LPCTSTR lpszPath, LPCTSTR lpszIniFileName)
{
	return SetHandle(lpszPath, lpszIniFileName);
}

void CXTPResourceImages::OnFinalRelease()
{
	//
}

BSTR CXTPResourceImages::OleGetDllFileName()
{
	return m_strDllFileName.AllocSysString();
}

BSTR CXTPResourceImages::OleGetIniFileName()
{
	return m_strIniFileName.AllocSysString();
}

OLE_COLOR CXTPResourceImages::OleGetGetColor(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	return GetImageColor(lpszSection, lpszEntry);
}

#endif
