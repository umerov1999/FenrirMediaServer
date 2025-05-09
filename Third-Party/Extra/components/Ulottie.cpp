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
	canvas_pushed = false;
	curFrame = -1;
	backColor = URGB(0, 0, 0);
	animWidth = 0;
	animHeight = 0;
	bytesPerLine = 0;
	THREAD_ACCESS_REGISTER_POINTERS(Async, &anim);
	inited = LIB_IMAGE::thorvg_init();

	canvas = nullptr;
	anim = nullptr;
}

Ulottie::~Ulottie() {
	canvas_pushed = false;
	if (canvas) {
		delete canvas;
		canvas = nullptr;
	}
	if (anim) {
		delete anim;
		anim = nullptr;
	}
}

void Ulottie::load_animation(URGB background, const void* json_data, size_t size, tvg::ColorReplace* colorReplacement) {
	if (!inited) {
		return;
	}
	THREAD_ACCESS_LOCK(Async, &anim);
	KillTimer(TIMER_PLAY_ANIM);
	setAnimationColor(background.r, background.g, background.b);

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

bool Ulottie::setAnimation(const string &json_data, tvg::ColorReplace* colorReplacement) {
	canvas_pushed = false;
	if (canvas) {
		delete canvas;
		canvas = nullptr;
	}
	if (anim) {
		delete anim;
		anim = nullptr;
	}
	anim = tvg::Animation::gen();
	if (anim->picture()->load(json_data.c_str(), (uint32_t)json_data.size(), "lottie", nullptr, true, colorReplacement) != tvg::Result::Success) {
		delete anim;
		anim = nullptr;
		return false;
	}

	CRect rect;
	GetClientRect(&rect);
	animWidth = rect.Width();
	animHeight = rect.Height();

	canvas = tvg::SwCanvas::gen();
	if (!canvas) {
		delete anim;
		anim = nullptr;
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
	buffer.resize(bytesPerLine * animHeight);
	if (canvas->target((uint32_t*)buffer.data(), (uint32_t)animWidth, (uint32_t)animWidth, (uint32_t)animHeight, tvg::ColorSpace::ARGB8888) != tvg::Result::Success) {
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

	if (canvas_pushed) {
		canvas->update();
	}
	else {
		canvas->push(anim->picture());
		canvas_pushed = true;
	}
	if (canvas->draw(true) == tvg::Result::Success) {
		canvas->sync();
	}
}

void Ulottie::setAnimationColor(int r, int g, int b) {
	backColor = URGB(r, g, b);
}

int Ulottie::getTotalFrame() {
	return (int)anim->totalFrame();
}

bool Ulottie::isAnimNULL() {
	return anim == NULL;
}

void Ulottie::CreateBitmap() {
	DeleteObject(target_anim.m_hObject);
	target_anim.CreateBitmap((int)animWidth, (int)animHeight, 1, 32, buffer.data());
}

void Ulottie::renderAnimation(UINT frameNum) {
	if (isAnimNULL()) return;

	curFrame = frameNum % std::max(getTotalFrame(), 1);

	renderRLottieAnimation(curFrame);
	CreateBitmap();
	InvalidateRect(FALSE);
}

BOOL Ulottie::OnEraseBkgnd(CDC* pMsg) {
	return TRUE;
}

void Ulottie::Clear() {
	THREAD_ACCESS_LOCK(Async, &anim);
	KillTimer(TIMER_PLAY_ANIM);
	canvas_pushed = false;
	if (canvas) {
		delete canvas;
		canvas = nullptr;
	}
	if (anim) {
		delete anim;
		anim = nullptr;
	}
	InvalidateRect(FALSE);
	THREAD_ACCESS_UNLOCK(Async, &anim);
}

void Ulottie::OnPaint() {
	CPaintDC pDC(this);
	pDC.SetStretchBltMode(COLORONCOLOR);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&pDC);
	dcMem.SetMapMode(GetMapMode(pDC));
	dcMem.SetBkMode(TRANSPARENT);

	CRect rect;
	GetClientRect(&rect);

	bool isnull = false;
	THREAD_ACCESS_LOCK(Async, &anim);
	isnull = isAnimNULL();
	THREAD_ACCESS_UNLOCK(Async, &anim);

	if (!isnull && target_anim.m_hObject != NULL) {
		target_anim.Attach((HBITMAP)CopyImage(target_anim.m_hObject, IMAGE_BITMAP, rect.Width(), rect.Height(), LR_COPYDELETEORG));
		dcMem.SelectObject(&target_anim);
	}
	else {
		CBitmap bitmap;
		CBrush brush(backColor());
		bitmap.CreateCompatibleBitmap(&pDC, rect.Width(), rect.Height());
		dcMem.SelectObject(&bitmap);
		dcMem.FillRect(&rect, &brush);
	}
	pDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}