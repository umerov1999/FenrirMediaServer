#pragma once
#include <afxwin.h>
#include <iostream>
#include <vector>
#include "win_api_utils.h"

class CImage : public CStatic {
	DECLARE_DYNAMIC(CImage)

public:
	CImage();
	virtual ~CImage();

	afx_msg void SwitchBitmap(HBITMAP pPicture = NULL);
	afx_msg CRect getRect();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pMsg);
private:
	CBitmap mBitmap;
	THREAD_ACCESS_GUARD Async;
};