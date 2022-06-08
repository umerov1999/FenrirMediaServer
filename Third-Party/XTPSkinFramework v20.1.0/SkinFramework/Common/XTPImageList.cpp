// XTPImageList.cpp interface
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

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageList.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPImageList, CObject);

CXTPImageList::CXTPImageList()
	: m_imageList(NULL)
	, m_imageCx(0)
{
	m_imageList = new CImageList();
	ASSERT_VALID(m_imageList);
}

CXTPImageList::~CXTPImageList()
{
	delete m_imageList;
}

BOOL CXTPImageList::Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow)
{
	m_imageCx = cx;
	return m_imageList->Create(XTP_DPI_X(cx), XTP_DPI_Y(cy), nFlags, nInitial, nGrow);
}

BOOL CXTPImageList::Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
{
	CBitmap bmp;
	if (!bmp.LoadBitmap(nBitmapID))
	{
		return FALSE;
	}

	return Create(&bmp, cx, nGrow, crMask);
}

BOOL CXTPImageList::Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask)
{
	CBitmap bmp;
	if (!bmp.LoadBitmap(lpszBitmapID))
	{
		return FALSE;
	}

	return Create(&bmp, cx, nGrow, crMask);
}

BOOL CXTPImageList::Create(CXTPImageList& imagelist1, int nImage1, CXTPImageList& imagelist2,
						   int nImage2, int dx, int dy)
{
	return m_imageList->Create(imagelist1, nImage1, imagelist2, nImage2, dx, dy);
}

BOOL CXTPImageList::Create(CXTPImageList* pImageList)
{
	return m_imageList->Create(pImageList->GetNativeGdiObject());
}

CXTPImageList::operator HIMAGELIST() const
{
	return (HIMAGELIST)m_imageList;
}

HIMAGELIST CXTPImageList::GetSafeHandle() const
{
	return (HIMAGELIST)m_imageList->GetSafeHandle();
}

int CXTPImageList::GetImageCount() const
{
	return m_imageList->GetImageCount();
}

COLORREF CXTPImageList::SetBkColor(COLORREF cr)
{
	return m_imageList->SetBkColor(cr);
}

COLORREF CXTPImageList::GetBkColor() const
{
	return m_imageList->GetBkColor();
}

BOOL CXTPImageList::GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const
{
	return m_imageList->GetImageInfo(nImage, pImageInfo);
}

BOOL CXTPImageList::DeleteImageList()
{
	return m_imageList->DeleteImageList();
}

BOOL CXTPImageList::SetImageCount(UINT uNewCount)
{
	return m_imageList->SetImageCount(uNewCount);
}

int CXTPImageList::Add(CBitmap* pbmImage, CBitmap* pbmMask)
{
	CBitmap resizedImage;
	if (!Resize(pbmImage, &resizedImage))
		return -1;

	return m_imageList->Add(&resizedImage, pbmMask);
}

int CXTPImageList::Add(CBitmap* pbmImage, COLORREF crMask)
{
	CBitmap resizedImage;
	if (!Resize(pbmImage, &resizedImage))
		return -1;

	return m_imageList->Add(&resizedImage, crMask);
}

BOOL CXTPImageList::Remove(int nImage)
{
	return m_imageList->Remove(nImage);
}

BOOL CXTPImageList::Replace(int nImage, CBitmap* pbmImage, CBitmap* pbmMask)
{
	return m_imageList->Replace(nImage, pbmImage, pbmMask);
}

int CXTPImageList::Add(HICON hIcon)
{
	return m_imageList->Add(hIcon);
}

int CXTPImageList::Replace(int nImage, HICON hIcon)
{
	return m_imageList->Replace(nImage, hIcon);
}

HICON CXTPImageList::ExtractIcon(int nImage)
{
	return m_imageList->ExtractIcon(nImage);
}

BOOL CXTPImageList::Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle)
{
	return m_imageList->Draw(pDC, nImage, pt, nStyle);
}

BOOL CXTPImageList::SetOverlayImage(int nImage, int nOverlay)
{
	return m_imageList->SetOverlayImage(nImage, nOverlay);
}

BOOL CXTPImageList::Copy(int iDst, int iSrc, UINT uFlags)
{
	return m_imageList->Copy(iDst, iSrc, uFlags);
}

BOOL CXTPImageList::Copy(int iDst, CXTPImageList* pSrc, int iSrc, UINT uFlags)
{
	return m_imageList->Copy(iDst, pSrc->GetNativeGdiObject(), iSrc, uFlags);
}

BOOL CXTPImageList::DrawIndirect(IMAGELISTDRAWPARAMS* pimldp)
{
	return m_imageList->DrawIndirect(pimldp);
}

BOOL CXTPImageList::Read(CArchive* pArchive)
{
	return m_imageList->Read(pArchive);
}

BOOL CXTPImageList::Write(CArchive* pArchive)
{
	return m_imageList->Write(pArchive);
}

BOOL CXTPImageList::BeginDrag(int nImage, CPoint ptHotSpot)
{
	return m_imageList->BeginDrag(nImage, ptHotSpot);
}

BOOL CXTPImageList::SetDragCursorImage(int nDrag, CPoint ptHotSpot)
{
	return m_imageList->SetDragCursorImage(nDrag, ptHotSpot);
}

BOOL CXTPImageList::Attach(HIMAGELIST hImageList)
{
	return m_imageList->Attach(hImageList);
}

HIMAGELIST CXTPImageList::Detach()
{
	return m_imageList->Detach();
}

#if defined(_DEBUG) || defined(_AFXDLL)

void CXTPImageList::Dump(CDumpContext& dc) const
{
	ASSERT(NULL != this);
	m_imageList->Dump(dc);
}

void CXTPImageList::AssertValid() const
{
	ASSERT(NULL != this);
	m_imageList->AssertValid();
}

#endif

BOOL CXTPImageList::Create(CBitmap* bitmap, int cx, int nGrow, COLORREF crMask)
{
	BOOL bRet = FALSE;
	m_imageCx = cx;

	if (bitmap)
	{
		CBitmap newBitmap;
		if (Resize(bitmap, &newBitmap))
		{
			BITMAP info;
			if (newBitmap.GetBitmap(&info))
			{
				HIMAGELIST imageList = ImageList_Create(XTP_DPI_X(cx), info.bmHeight,
														ILC_MASK | ILC_COLOR8, 1, nGrow);
				if (imageList)
				{
					if (ImageList_AddMasked(imageList, newBitmap, crMask) != -1)
					{
						bRet = m_imageList->Attach(imageList);
					}
				}
			}
		}
	}

	return bRet;
}

BOOL CXTPImageList::Resize(CBitmap* src, CBitmap* dst)
{
	ASSERT_VALID(src);
	ASSERT_VALID(dst);
	ASSERT(0 != m_imageCx);

	BOOL bRet = FALSE;

	if (src && dst)
	{
		CDC srcDC, destDC;
		if (srcDC.CreateCompatibleDC(NULL) && destDC.CreateCompatibleDC(NULL))
		{
			CBitmap* pSrcBitmap = srcDC.SelectObject(src);
			BITMAP bmpInfo;
			if (src->GetBitmap(&bmpInfo))
			{
				int imageCount = bmpInfo.bmWidth / m_imageCx;
				int deltaWidth = bmpInfo.bmWidth - imageCount * m_imageCx;
				int width	   = XTP_DPI_X(m_imageCx) * imageCount;
				int height	   = XTP_DPI_Y(static_cast<int>(bmpInfo.bmHeight));

				if (dst->CreateCompatibleBitmap(&srcDC, width, height))
				{
					CBitmap* pDstBitmap = destDC.SelectObject(dst);
					if (destDC.StretchBlt(0, 0, width, height, &srcDC, 0, 0,
										  bmpInfo.bmWidth - deltaWidth, bmpInfo.bmHeight, SRCCOPY))
					{
						bRet = TRUE;
					}

					destDC.SelectObject(pDstBitmap);
				}
			}

			srcDC.SelectObject(pSrcBitmap);
		}
	}

	return bRet;
}
