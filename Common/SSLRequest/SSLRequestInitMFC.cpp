#include "pch.h"
#include "SSLRequestInitMFC.h"
#include "SSLRequestDialog.h"

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

void SSLRequest::CreateDialogMFC()
{
	SSLRequestDialog dlgS;

	m_pMainWnd = &dlgS;
	dlgS.DoModal();
	return;
}

void SSLRequest::LoadStyle()
{
	if (XTPSkinMgr()->SkinLoadMemory(APP_STYLE_DATA, APP_STYLE_SIZE) == FALSE)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Стиль повреждён!").show();
		return;
	}
}

BEGIN_MESSAGE_MAP(SSLRequest, CXTPWinApp)

END_MESSAGE_MAP()

SSLRequest theApp;

BOOL SSLRequest::InitInstance()
{
	AfxEnableControlContainer();
	srand((unsigned int)time(0));

	LoadStyle();
	CreateDialogMFC();
	return FALSE;
}