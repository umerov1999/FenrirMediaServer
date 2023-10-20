#pragma once

#include <iostream>
#include "resource.h"

class UStyleViewer : public CXTPWinApp
{
public:
	UStyleViewer();

public:
	virtual BOOL InitInstance();
	void CreateDialogMFC();

	DECLARE_MESSAGE_MAP()
};