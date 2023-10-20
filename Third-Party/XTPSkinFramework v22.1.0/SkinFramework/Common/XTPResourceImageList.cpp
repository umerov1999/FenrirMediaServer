// XTPResourceImageList.cpp
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
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPResourceImage.h"
#include "Common/XTPResourceImageList.h"
#include "Common/IIDs/XTPImageManagerIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPResourceImageList

IMPLEMENT_DYNCREATE(CXTPResourceImageList, CXTPCmdTarget)

CXTPResourceImageList::CXTPResourceImageList()
{
#ifdef _XTP_ACTIVEX
	EnableAutomation();
#endif
}

CXTPResourceImageList::~CXTPResourceImageList()
{
	RemoveAll();
}

static HBITMAP CreateDIB(HDC hdc, int aWidth, int aHeight, int aPixelBit)
{
#define BitmapWidth(width, bitsPerPixel) (((width * bitsPerPixel + 31) & ~31) >> 3)
	HBITMAP hBitmap = NULL;

	BITMAPINFO      pBitmapInfo = { 0 };

	pBitmapInfo.bmiHeader.biSize = sizeof(pBitmapInfo.bmiHeader);
	pBitmapInfo.bmiHeader.biBitCount = (WORD)aPixelBit;
	pBitmapInfo.bmiHeader.biCompression = BI_RGB;
	pBitmapInfo.bmiHeader.biWidth = aWidth;
	pBitmapInfo.bmiHeader.biHeight = -aHeight;
	pBitmapInfo.bmiHeader.biPlanes = 1;
	pBitmapInfo.bmiHeader.biSizeImage = BitmapWidth(pBitmapInfo.bmiHeader.biWidth, pBitmapInfo.bmiHeader.biBitCount) * pBitmapInfo.bmiHeader.biHeight;
	void* BBits = NULL;
	hBitmap = CreateDIBSection(hdc, &pBitmapInfo, DIB_RGB_COLORS, &BBits, 0, 0);

	return hBitmap;
}

static HBITMAP CopyBitmap(HBITMAP hSRCBitmap)
{
	if (!hSRCBitmap) {
		return hSRCBitmap;
	}

	BITMAP bmpInfo;
	if (!GetObjectW(hSRCBitmap, sizeof(BITMAP), &bmpInfo)) {
		return hSRCBitmap;
	}

	HDC srcDC, destDC;
	srcDC = CreateCompatibleDC(NULL);
	destDC = CreateCompatibleDC(NULL);
	if (!srcDC || !destDC) {
		if (srcDC) {
			DeleteDC(srcDC);
		}
		if (destDC) {
			DeleteDC(destDC);
		}
		return hSRCBitmap;
	}
	SetStretchBltMode(destDC, COLORONCOLOR);
	SelectObject(srcDC, hSRCBitmap);
	HBITMAP dst = CreateDIB(srcDC, bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel);
	if (!dst) {
		DeleteDC(srcDC);
		DeleteDC(destDC);
		return hSRCBitmap;
	}
	SelectObject(destDC, dst);
	if (!BitBlt(destDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, srcDC, 0, 0, SRCCOPY)) {
		DeleteDC(srcDC);
		DeleteDC(destDC);
		DeleteObject(dst);
		return hSRCBitmap;
	}
	DeleteDC(srcDC);
	DeleteDC(destDC);
	return dst;
}

CXTPResourceImage* CXTPResourceImageList::SetBitmap(HBITMAP hBitmap, UINT nID, BOOL bAlptha,
													BOOL bCopyBitmap)
{
	ASSERT(hBitmap);
	if (!hBitmap)
		return NULL;

	HBITMAP hBmp2 = hBitmap;
	if (bCopyBitmap)
	{
		//hBmp2 = CXTPImageManagerIcon::CopyAlphaBitmap(hBitmap);
		hBmp2 = CopyBitmap(hBitmap);
		//hBmp2 = (HBITMAP)::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(hBmp2);
		if (!hBmp2)
			return NULL;
	}

	BITMAP bmpinfo;
	if (!::GetObject(hBmp2, sizeof(bmpinfo), &bmpinfo))
	{
		ASSERT(FALSE);
		::DeleteObject(hBmp2);
		return NULL;
	}

	CXTPResourceImage* pImage = new CXTPResourceImage(this);
	if (!pImage)
		return NULL;

	pImage->SetBitmap(hBmp2, bAlptha);

	SetBitmap(pImage, nID, FALSE);

	return pImage;
}

BOOL CXTPResourceImageList::SetBitmap(CXTPResourceImage* pImage, UINT nID, BOOL bCallAddRef)
{
	ASSERT(pImage);
	if (!pImage)
		return FALSE;

	if (bCallAddRef)
		CMDTARGET_ADDREF(pImage);

	// free prev. image if such exists
	CXTPResourceImage* pImage_old = GetBitmap(nID);
	CMDTARGET_RELEASE(pImage_old);

	m_mapID2Image.SetAt(nID, pImage);

	return TRUE;
}

CXTPResourceImage* CXTPResourceImageList::GetBitmap(UINT nID)
{
	CXTPResourceImage* pImage = NULL;
	if (!m_mapID2Image.Lookup(nID, pImage))
		return NULL;

	return pImage;
}

BOOL CXTPResourceImageList::Remove(UINT nID)
{
	CString strImageFile;
	CXTPResourceImage* pImage = NULL;

	//-------------------------------------------------
	if (!m_mapID2Image.Lookup(nID, pImage))
		return FALSE;

	m_mapID2Image.RemoveKey(nID);
	CMDTARGET_RELEASE(pImage);

	return TRUE;
}

void CXTPResourceImageList::RemoveAll()
{
	UINT nID;
	CXTPResourceImage* pImage;

	POSITION pos = m_mapID2Image.GetStartPosition();
	while (pos != NULL)
	{
		m_mapID2Image.GetNextAssoc(pos, nID, pImage);
		CMDTARGET_RELEASE(pImage);
	}

	m_mapID2Image.RemoveAll();

	m_mapPropeties.RemoveAll();
}

BOOL CXTPResourceImageList::LoadBitmap(LPCTSTR lpcszPath, UINT nID)
{
	BOOL bAlphaBitmap = FALSE;
	HBITMAP hBmp	  = CXTPImageManagerIcon::LoadBitmapFromFile(lpcszPath, &bAlphaBitmap);

	CXTPResourceImage* pImage = NULL;
	if (hBmp)
	{
		pImage = SetBitmap(hBmp, nID, bAlphaBitmap, FALSE);
	}

	return !!pImage;
}

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPResourceImageList, CXTPCmdTarget)
	INTERFACE_PART(CXTPResourceImageList, XTPDIID_IResourceImageList, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPResourceImageList, XTPDIID_IResourceImageList)

BEGIN_DISPATCH_MAP(CXTPResourceImageList, CXTPCmdTarget)
	DISP_FUNCTION_ID(CXTPResourceImageList, "AddBitmap", 1, OleAddBitmap, VT_BOOL, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CXTPResourceImageList, "LoadBitmap", 2, LoadBitmap, VT_BOOL, VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CXTPResourceImageList, "Remove", 3, Remove, VT_BOOL, VTS_I4)
	DISP_FUNCTION_ID(CXTPResourceImageList, "RemoveAll", 4, RemoveAll, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BOOL CXTPResourceImageList::OleAddBitmap(OLE_HANDLE ohBitmap, UINT nID)
{
	CXTPResourceImage* pImage = SetBitmap(reinterpret_cast<HBITMAP>((LONG_PTR)ohBitmap), nID, TRUE);
	return NULL != pImage;
}

void CXTPResourceImageList::OnFinalRelease()
{
	CXTPCmdTarget::OnFinalRelease();
}

#endif
