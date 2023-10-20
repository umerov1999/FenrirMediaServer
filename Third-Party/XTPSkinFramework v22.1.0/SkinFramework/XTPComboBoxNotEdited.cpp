#include "stdafx.h"

#include "XTPComboBoxNotEdited.h"

IMPLEMENT_DYNCREATE(CXTPComboBoxNotEdited, CComboBox)

CXTPComboBoxNotEdited::CXTPComboBoxNotEdited() {
}

CXTPComboBoxNotEdited::~CXTPComboBoxNotEdited() {
}

void CXTPComboBoxNotEdited::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();

	auto hWnd = ::GetWindow(GetSafeHwnd(), GW_CHILD);
	while (hWnd)
	{
		const int BufSize = 32;
		TCHAR className[BufSize];
		if (::GetClassName(hWnd, className, BufSize)
			&& !_tcsicmp(className, _T("EDIT"))) {
			::SendMessage(hWnd, EM_SETREADONLY, 1, 0);
		}

		hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	}
}


BEGIN_MESSAGE_MAP(CXTPComboBoxNotEdited, CComboBox)
END_MESSAGE_MAP()