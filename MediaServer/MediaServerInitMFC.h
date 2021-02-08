#pragma once

#include "resource.h"

class MediaServer : public CWinApp
{
public:
	virtual BOOL InitInstance();
	void CreateDialogMFC();
	void LoadStyle();
	void LoadFont();

	DECLARE_MESSAGE_MAP()
};