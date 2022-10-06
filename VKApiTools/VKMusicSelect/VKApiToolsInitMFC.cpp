#include "pch.h"
#include "VKApiToolsInitMFC.h"
#include "VKApiToolsDialog.h"
#include "WinMessageBox.h"
#include "CriticalDebug.h"

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

extern int APP_FONT_SIZE;
extern const char* APP_FONT_DATA;

VKApiToolsDialog dlgS;

void VKApiTools::LoadFont()
{
	DWORD CountUU = 0;
	if (AddFontMemResourceEx((PVOID)APP_FONT_DATA, APP_FONT_SIZE, 0, &CountUU) == NULL)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Повреждён Шрифт").show();
		return;
	}
	XTPSkinMgr()->SetDefaultThemeFont(L"Montserrat Alternates Light");
}

void VKApiTools::CreateDialogMFC()
{
	m_pMainWnd = &dlgS;
	dlgS.DoModal();
	return;
}

void VKApiTools::LoadStyle()
{
	XTPSkinMgr()->SetApplyOptions(XTPSkinManager()->GetApplyOptions() | xtpSkinApplyMetrics);
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

BEGIN_MESSAGE_MAP(VKApiTools, CWinApp)

END_MESSAGE_MAP()

VKApiTools theApp;

BOOL VKApiTools::InitInstance()
{
	AfxEnableControlContainer();
	srand((unsigned int)time(0));

	SetCurrentDirectoryW(ExtractAppPath().c_str());
	LoadFont();
	LoadStyle();
	INIT_CRITICAL_DEBUGGER(false);

	CreateDialogMFC();
	return FALSE;
}