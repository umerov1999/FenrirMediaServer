#include "Ulottie.h"
#include <iostream>
#include <vector>
#include <string>
#include "afxdialogex.h"
#include "libimage.h"
#include "lz4.h"
using namespace std;

IMPLEMENT_DYNAMIC(Ulottie, CStatic)
#define TIMER_PLAY_ANIM 1015

Ulottie::Ulottie() {
	curFrame = -1;
	animWidth = 0;
	animHeight = 0;
	bytesPerLine = 0;
	THREAD_ACCESS_REGISTER_POINTERS(Async, &anim);
	inited = LIB_IMAGE::thorvg_init();

	canvas = nullptr;
	anim = nullptr;
}

Ulottie::~Ulottie() {
	KillTimer(TIMER_PLAY_ANIM);
	if (canvas) {
		delete canvas;
		canvas = nullptr;
	}
	if (anim) {
		delete anim;
		anim = nullptr;
	}
}

void Ulottie::load_animation(const void* json_data, size_t size, tvg::ColorReplace* colorReplacement) {
	if (!inited) {
		return;
	}
	THREAD_ACCESS_LOCK(Async, &anim);
	KillTimer(TIMER_PLAY_ANIM);

	string data;
	if (size >= 4 && memcmp(json_data, "\x02\x4C\x5A\x34", 4) == 0) {
#pragma pack(push, 1)
		struct HDR {
			char hdr[4];
			int size;
		};
#pragma pack(pop)
		HDR hdr = {};
		memcpy(&hdr, json_data, sizeof(HDR));
		data.resize(hdr.size);
		LZ4_decompress_safe(((const char*)json_data + sizeof(HDR)), (char*)data.data(),
			(int)size - (int)sizeof(HDR), hdr.size);
	}
	else {
		data.resize(size);
		memcpy(data.data(), json_data, size);
	}

	if (!setAnimation(data, colorReplacement)) {
		THREAD_ACCESS_UNLOCK(Async, &anim);
		return;
	}
	curFrame = -1;
	THREAD_ACCESS_UNLOCK(Async, &anim);
	SetTimer(TIMER_PLAY_ANIM, 10, NULL);
}

BEGIN_MESSAGE_MAP(Ulottie, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CRect Ulottie::getRect() {
	CRect backgr;
	GetClientRect(&backgr);
	return backgr;
}

#define BitmapWidth(width, bitsPerPixel) (((width * bitsPerPixel + 31) & ~31) >> 3)
static HBITMAP CreateDIB(HDC hdc, int aWidth, int aHeight, int aPixelBit) {
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

bool Ulottie::setAnimation(const string &json_data, tvg::ColorReplace* colorReplacement) {
	if (canvas) {
		delete canvas;
		canvas = nullptr;
	}
	if (anim) {
		delete anim;
		anim = nullptr;
	}

	auto backgroundShape = tvg::Shape::gen();
	CRect rect = getRect();
	backgroundShape->appendRect(0, 0, (float)rect.Width(), (float)rect.Height());
	COLORREF cl = GetSysColor(COLOR_3DFACE);
	backgroundShape->fill(GetRValue(cl), GetGValue(cl), GetBValue(cl));

	anim = tvg::Animation::gen();
	if (anim->picture()->load(json_data.c_str(), (uint32_t)json_data.size(), "lottie", nullptr, true, colorReplacement) != tvg::Result::Success) {
		delete anim;
		anim = nullptr;
		tvg::Paint::rel(backgroundShape);
		backgroundShape = nullptr;
		return false;
	}

	animWidth = rect.Width();
	animHeight = rect.Height();

	canvas = tvg::SwCanvas::gen();
	if (!canvas) {
		delete anim;
		anim = nullptr;
		tvg::Paint::rel(backgroundShape);
		backgroundShape = nullptr;
		return false;
	}
	
	float scale;
	float shiftX = 0.0f, shiftY = 0.0f;
	float w2, h2;
	anim->picture()->size(&w2, &h2);

	if (animHeight >= animWidth) {
		if (w2 >= h2) {
			scale = (float)animWidth / w2;
			shiftY = ((float)animHeight - h2 * scale) * 0.5f;
		}
		else {
			scale = (float)animHeight / h2;
			shiftX = ((float)animWidth - w2 * scale) * 0.5f;
		}
	}
	else {
		if (w2 < h2) {
			scale = (float)animWidth / w2;
			shiftY = ((float)animHeight - h2 * scale) * 0.5f;
		}
		else {
			scale = (float)animHeight / h2;
			shiftX = ((float)animWidth - w2 * scale) * 0.5f;
		}
	}

	anim->picture()->scale(scale);
	anim->picture()->translate(shiftX, shiftY);

	bytesPerLine = animWidth * sizeof(uint32_t);

	DeleteObject(target_anim.m_hObject);
	target_anim.Attach(CreateDIB(*GetDC(), (int)animWidth, (int)animHeight, 32));
	BITMAP btmOutputImage;
	::GetObject(target_anim, sizeof(BITMAP), &btmOutputImage);
	uint32_t* pImageBuffer = (uint32_t*)btmOutputImage.bmBits;
	if (!pImageBuffer) {
		delete canvas;
		canvas = nullptr;
		delete anim;
		anim = nullptr;
		tvg::Paint::rel(backgroundShape);
		backgroundShape = nullptr;
		return false;
	}

	if (canvas->target(pImageBuffer, (uint32_t)animWidth, (uint32_t)animWidth, (uint32_t)animHeight, tvg::ColorSpace::ARGB8888S) != tvg::Result::Success) {
		delete canvas;
		canvas = nullptr;
		delete anim;
		anim = nullptr;
		tvg::Paint::rel(backgroundShape);
		backgroundShape = nullptr;
		return false;
	}
	if (canvas->push(backgroundShape) != tvg::Result::Success) {
		delete canvas;
		canvas = nullptr;
		delete anim;
		anim = nullptr;
		tvg::Paint::rel(backgroundShape);
		backgroundShape = nullptr;
		return false;
	}
	if (canvas->push(anim->picture()) != tvg::Result::Success) {
		delete canvas;
		canvas = nullptr;
		delete anim;
		anim = nullptr;
		return false;
	}
	return true;
}

void Ulottie::OnTimer(UINT_PTR uid) {
	if (uid == TIMER_PLAY_ANIM) {
		THREAD_ACCESS_LOCK(Async, &anim);
		if (isAnimNULL()) {
			KillTimer(TIMER_PLAY_ANIM);
			return;
		}
		renderAnimation(curFrame + 1);
		THREAD_ACCESS_UNLOCK(Async, &anim);
	}
}

void Ulottie::renderRLottieAnimation(uint32_t frameNum) {
	anim->frame((float)frameNum);

	canvas->update();
	if (canvas->draw(true) == tvg::Result::Success) {
		canvas->sync();
	}
}

int Ulottie::getTotalFrame() {
	return (int)anim->totalFrame();
}

bool Ulottie::isAnimNULL() {
	return anim == NULL;
}

void Ulottie::renderAnimation(UINT frameNum) {
	if (isAnimNULL()) return;

	curFrame = frameNum % std::max(getTotalFrame(), 1);

	renderRLottieAnimation(curFrame);
	InvalidateRect(getRect(), TRUE);
	UpdateWindow();
}

BOOL Ulottie::OnEraseBkgnd(CDC* pMsg) {
	return TRUE;
}

void Ulottie::Clear() {
	THREAD_ACCESS_LOCK(Async, &anim);
	KillTimer(TIMER_PLAY_ANIM);
	if (canvas) {
		delete canvas;
		canvas = nullptr;
	}
	if (anim) {
		delete anim;
		anim = nullptr;
	}
	InvalidateRect(getRect(), TRUE);
	THREAD_ACCESS_UNLOCK(Async, &anim);
	UpdateWindow();
}

void Ulottie::OnPaint() {
	CRect rect = getRect();
	CPaintDC pDC(this);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&pDC);
	dcMem.SetMapMode(GetMapMode(pDC));

	CBrush brush(GetSysColor(COLOR_3DFACE));
	dcMem.FillRect(&rect, &brush);

	bool isnull = false;
	THREAD_ACCESS_LOCK(Async, &anim);
	isnull = isAnimNULL();
	THREAD_ACCESS_UNLOCK(Async, &anim);

	if (!isnull && target_anim.m_hObject != NULL) {
		dcMem.SelectObject(&target_anim);
	}
	else {
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&pDC, rect.Width(), rect.Height());
		dcMem.SelectObject(&bitmap);
		pDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
		return;
	}
	pDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}