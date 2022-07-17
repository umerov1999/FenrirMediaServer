#include "Ulottie.h"
#include <iostream>
#include <vector>
#include <string>
#include "afxdialogex.h"
#include "rlottie.h"
using namespace std;
using namespace rlottie;

IMPLEMENT_DYNAMIC(Ulottie, CStatic)
#define TIMER_PLAY_ANIM 1015

Ulottie::Ulottie()
{
	curFrame = -1;
	curColor = UINT32_MAX;
	backColor = URGB(0, 0, 0);
	animWidth = 0;
	animHeight = 0;
	bytesPerLine = 0;
	THREAD_ACCESS_REGISTER_POINTERS(Async, &anim);
}

void Ulottie::load_animation(URGB background, const string& json_data, internal::ColorReplace* colorReplace) {
	THREAD_ACCESS_LOCK(Async, &anim);
	KillTimer(TIMER_PLAY_ANIM);
	setAnimationColor(background.r, background.g, background.b);
	setAnimation(json_data, colorReplace);
	curFrame = -1;
	THREAD_ACCESS_UNLOCK(Async, &anim);
	SetTimer(TIMER_PLAY_ANIM, 10, NULL);
}

void Ulottie::update_color_replacement(rlottie::internal::ColorReplace* colorReplace) {
	THREAD_ACCESS_LOCK(Async, &anim);
	if (!isAnimNULL() && anim->colorMap != nullptr) {
		*anim->colorMap = *colorReplace;
		delete colorReplace;
		anim->resetCurrentFrame();
	}
	THREAD_ACCESS_UNLOCK(Async, &anim);
}

internal::ColorReplace* Ulottie::parseReplacement(const vector<int>& colors, bool useMoveColor) {
	if (colors.empty() || colors.size() % 2 != 0) {
		return NULL;
	}
	auto ret = new internal::ColorReplace(useMoveColor);
	for (int32_t a = 0; a < colors.size() / 2; a++) {
		ret->colorMap[colors[a * 2]] = colors[a * 2 + 1];
	}
	return ret;
}


BEGIN_MESSAGE_MAP(Ulottie, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void Ulottie::setAnimation(const string &json_data, internal::ColorReplace* colorReplace)
{
	anim = Animation::loadFromData(json_data.c_str(), colorReplace);
	anim->size(animWidth, animHeight);
	bytesPerLine = animWidth * sizeof(uint32_t);
	buffer.resize(bytesPerLine * animHeight);
}

void Ulottie::OnTimer(UINT_PTR uid)
{
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

void Ulottie::renderRLottieAnimation(uint32_t frameNum)
{
	Surface surface = Surface(buffer.data(), animWidth, animHeight, bytesPerLine);
	anim->renderSync(frameNum, surface, true);
	for (int i = 0; i < animHeight; i++) {
		for (int j = 0; j < animWidth; ++j)
		{
			uint32_t* v = buffer.data() + i * animWidth + j;
			if (*v == 0) *v = curColor;
		}
	}
}

void Ulottie::setAnimationColor(int r, int g, int b)
{
	curColor = ((255 << 16) * r) + ((255 << 8) * g) + 255 * b;
	backColor = URGB(r, g, b);
}

size_t Ulottie::getTotalFrame()
{
	return anim->totalFrame();
}

bool Ulottie::isAnimNULL()
{
	return anim == NULL;
}

void Ulottie::CreateBitmap()
{
	DeleteObject(target_anim.m_hObject);
	target_anim.CreateBitmap((int)animWidth, (int)animHeight, 1, 32, buffer.data());
}

void Ulottie::renderAnimation(UINT frameNum)
{
	if (isAnimNULL()) return;

	curFrame = frameNum % getTotalFrame();

	renderRLottieAnimation(curFrame);
	CreateBitmap();
	InvalidateRect(FALSE);
}

BOOL Ulottie::OnEraseBkgnd(CDC* pMsg)
{
	return TRUE;
}

void Ulottie::Clear()
{
	THREAD_ACCESS_LOCK(Async, &anim);
	KillTimer(TIMER_PLAY_ANIM);
	anim = nullptr;
	InvalidateRect(FALSE);
	THREAD_ACCESS_UNLOCK(Async, &anim);
}

void Ulottie::OnPaint()
{
	CPaintDC pDC(this);

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