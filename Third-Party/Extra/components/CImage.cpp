#include "CImage.h"
#include <iostream>
#include <vector>
#include <string>
#include "afxdialogex.h"
#include "win_api_utils.h"
using namespace std;
IMPLEMENT_DYNAMIC(CImage, CStatic)

void CImage::SwitchBitmap(HBITMAP pPicture) {
	THREAD_ACCESS_LOCK(Async, &mBitmap);
	mBitmap.DeleteObject();
	if (pPicture != NULL) {
		mBitmap.Attach(pPicture);
	}
	THREAD_ACCESS_UNLOCK(Async, &mBitmap);

	InvalidateRect(getRect());
	UpdateWindow();
}

CRect CImage::getRect() {
	CRect rect;
	GetClientRect(&rect);
	return rect;
}

CImage::CImage() {
	THREAD_ACCESS_REGISTER_POINTERS(Async, &mBitmap);
}

CImage::~CImage() {
}


BEGIN_MESSAGE_MAP(CImage, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CImage::OnEraseBkgnd(CDC* pMsg) {
	return TRUE;
}

void CImage::OnPaint() {
	CRect rect = getRect();
	CPaintDC pDC(this);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&pDC, rect.Width(), rect.Height());
	dcMem.SetMapMode(GetMapMode(pDC));

	dcMem.SelectObject(&bitmap);
	CBrush brush(GetSysColor(COLOR_3DFACE));
	dcMem.FillRect(&rect, &brush);

	pDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);

	THREAD_ACCESS_LOCK(Async, &mBitmap);
	if (mBitmap.m_hObject != NULL) {
		BITMAP bmpInfo;
		if (GetObjectW(mBitmap, sizeof(BITMAP), &bmpInfo)) {
			dcMem.SelectObject(&mBitmap);
			pDC.BitBlt(rect.left + (rect.Width() - bmpInfo.bmWidth) / 2, rect.top + (rect.Height() - bmpInfo.bmHeight) / 2, bmpInfo.bmWidth, rect.Height(), &dcMem, 0, 0, SRCCOPY);
		}
	}
	THREAD_ACCESS_UNLOCK(Async, &mBitmap);
}