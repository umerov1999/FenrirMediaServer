#pragma once

#include "resource.h"

class MakeSSL : public CWinApp
{
public:
	virtual BOOL InitInstance();
	void CreateDialogMFC();
	void LoadStyle();

	DECLARE_MESSAGE_MAP()
};