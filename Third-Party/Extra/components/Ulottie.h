#pragma once
#include <afxwin.h>
#include <iostream>
#include <vector>
#include "rlottie.h"
#include "ThreadAccessGuard.h"
#include "urgb.h"

class Ulottie : public CStatic
{
	DECLARE_DYNAMIC(Ulottie)

public:
	Ulottie();

	afx_msg void Clear();
	afx_msg void load_animation(URGB background, const std::string& json_data, rlottie::internal::ColorReplace* colorReplace = NULL);
	afx_msg void update_color_replacement(rlottie::internal::ColorReplace* colorReplace);
	static rlottie::internal::ColorReplace* parseReplacement(const std::vector<int>& colors, bool useMoveColor = false);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pMsg);
	afx_msg void CreateBitmap();
	afx_msg void renderAnimation(UINT frameNum);
	afx_msg void OnTimer(UINT_PTR uid);

	afx_msg void setAnimation(const std::string& json_data, rlottie::internal::ColorReplace* colorReplace = NULL);
	afx_msg void renderRLottieAnimation(uint32_t frameNum);
	afx_msg void setAnimationColor(int r, int g, int b);
	afx_msg size_t getTotalFrame();
	afx_msg bool isAnimNULL();
private:
	std::unique_ptr<rlottie::Animation> anim;
	std::vector<uint32_t> buffer;
	size_t animWidth, animHeight;
	size_t bytesPerLine;
	uint32_t curColor;
	int curFrame;
	CBitmap target_anim;
	URGB backColor;
	THREAD_ACCESS_GUARD Async;
};