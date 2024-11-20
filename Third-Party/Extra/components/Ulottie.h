#pragma once
#include <afxwin.h>
#include <iostream>
#include <vector>
#include <thread>
#include <thorvg.h>
#include "win_api_utils.h"
#include "urgb.h"

class Ulottie : public CStatic
{
	DECLARE_DYNAMIC(Ulottie)

public:
	Ulottie();
	~Ulottie();

	afx_msg void Clear();
	afx_msg void load_animation(URGB background, const void* json_data, size_t size, std::unique_ptr<tvg::ColorReplace> colorReplacement = nullptr);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pMsg);
	afx_msg void CreateBitmap();
	afx_msg void renderAnimation(UINT frameNum);
	afx_msg void OnTimer(UINT_PTR uid);

	afx_msg bool setAnimation(const std::string& json_data, std::unique_ptr<tvg::ColorReplace> colorReplacement);
	afx_msg void renderRLottieAnimation(uint32_t frameNum);
	afx_msg void setAnimationColor(int r, int g, int b);
	afx_msg int getTotalFrame();
	afx_msg bool isAnimNULL();
private:
	bool inited;
	bool canvas_pushed;
	std::unique_ptr<tvg::Animation> anim;
	std::unique_ptr<tvg::SwCanvas> canvas;

	std::vector<uint32_t> buffer;
	size_t animWidth, animHeight;
	size_t bytesPerLine;
	int curFrame;
	CBitmap target_anim;
	URGB backColor;
	THREAD_ACCESS_GUARD Async;
};