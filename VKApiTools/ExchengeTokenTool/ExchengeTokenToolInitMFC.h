#pragma once

#include "resource.h"

class ExchengeTokenTool : public CXTPWinApp
{
public:
	virtual BOOL InitInstance();
	void CreateDialogMFC();
	void LoadStyle();

	DECLARE_MESSAGE_MAP()
};