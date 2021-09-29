#include "pch.h"
#include "MakeSSLInitMFC.h"
#include "MakeSSLDialog.h"

/*
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int APP_STYLE_SIZE;
extern const char* APP_STYLE_DATA;

void MakeSSL::CreateDialogMFC()
{
	MakeSSLDialog dlgS;

	m_pMainWnd = &dlgS;
	dlgS.DoModal();
	return;
}

void MakeSSL::LoadStyle()
{
	XTPSkinMgr()->SetApplyOptions(XTPSkinManager()->GetApplyOptions() | xtpSkinApplyMetrics);
	if (XTPSkinMgr()->SkinLoadMemory(APP_STYLE_DATA, APP_STYLE_SIZE) == FALSE)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Стиль повреждён!").show();
		return;
	}
}

BEGIN_MESSAGE_MAP(MakeSSL, CWinApp)

END_MESSAGE_MAP()

MakeSSL theApp;

BOOL MakeSSL::InitInstance()
{
	AfxEnableControlContainer();
	srand((unsigned int)time(0));

	LoadStyle();
	CreateDialogMFC();
	return FALSE;
}