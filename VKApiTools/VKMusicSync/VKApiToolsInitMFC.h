#pragma once

#include "resource.h"

class VKApiTools : public CXTPWinApp
{
public:
	virtual BOOL InitInstance();
	void CreateDialogMFC();
	void LoadStyle();
	void LoadFont();

	DECLARE_MESSAGE_MAP()
};