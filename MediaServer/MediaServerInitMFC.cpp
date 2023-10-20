#include "pch.h"
#include "MediaServerInitMFC.h"
#include "MediaServerDialog.h"
#include "CriticalDebug.h"
#include <string>
#include <ctime>
using namespace std;

#ifdef _WIN64
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _WIN32
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

extern void InitOpenSSL();

extern int APP_STYLE_SIZE;
extern const char* APP_STYLE_DATA;

extern int APP_FONT_SIZE;
extern const char* APP_FONT_DATA;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MediaServerDialog dlgS;

void MediaServer::LoadFont()
{
	DWORD CountUU = 0;
	if(AddFontMemResourceEx((PVOID)APP_FONT_DATA, APP_FONT_SIZE, 0, &CountUU) == NULL)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Повреждён Шрифт").show();
		return;
	}
	XTPSkinMgr()->SetDefaultThemeFont(L"Montserrat Alternates Light");
}

void MediaServer::CreateDialogMFC()
{
	m_pMainWnd = &dlgS;
	dlgS.DoModal();
}

void MediaServer::LoadStyle()
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

BEGIN_MESSAGE_MAP(MediaServer, CXTPWinApp)

END_MESSAGE_MAP()

MediaServer theApp;

BOOL MediaServer::InitInstance()
{
	AfxEnableControlContainer();
	srand((unsigned int)time(0));

	SetCurrentDirectoryW(ExtractAppPath().c_str());
	LoadFont();
	LoadStyle();
	INIT_CRITICAL_DEBUGGER(false);
	InitOpenSSL();
	CreateDialogMFC();

	return FALSE;
}