#include "pch.h"
#include "ExchengeTokenToolInitMFC.h"
#include "ExchengeTokenToolDialog.h"

#ifdef _WIN64
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _WIN32
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int APP_STYLE_SIZE;
extern const char* APP_STYLE_DATA;

void ExchengeTokenTool::CreateDialogMFC()
{
	ExchengeTokenToolDialog dlgS;

	m_pMainWnd = &dlgS;
	dlgS.DoModal();
	return;
}

void ExchengeTokenTool::LoadStyle()
{
	if (XTPSkinMgr()->SkinLoadMemory(APP_STYLE_DATA, APP_STYLE_SIZE) == FALSE)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Стиль повреждён!").show();
		return;
	}
}

std::wstring ExtractAppPath()
{
	const wchar_t* FnS = L"\\/";
	TCHAR szFileName[MAX_PATH];
	GetModuleFileNameW(0, szFileName, MAX_PATH);

	std::wstring fname = szFileName;
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(0, pos);
}

BEGIN_MESSAGE_MAP(ExchengeTokenTool, CXTPWinApp)

END_MESSAGE_MAP()

ExchengeTokenTool theApp;

BOOL ExchengeTokenTool::InitInstance()
{
	AfxEnableControlContainer();
	srand((unsigned int)time(0));

	SetCurrentDirectoryW(ExtractAppPath().c_str());

	LoadStyle();
	CreateDialogMFC();
	return FALSE;
}