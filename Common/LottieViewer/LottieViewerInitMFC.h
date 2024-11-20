#pragma once

#include <iostream>
#include "resource.h"

class LottieViewer : public CXTPWinApp
{
public:
	virtual BOOL InitInstance();
	void CreateDialogMFC();

	void LoadStyle();
	void LoadFont();

	DECLARE_MESSAGE_MAP()
};