#include "pch.h"
#include <iostream>
#include "base.hpp"
#include "json.hpp"
#include "do_curl.h"
#include "FlBin.h"
#include "VKApiToolsDialog.h"
#include "VKApiToolsDialogCapcha.h"
#include "VKApiToolsInputLogin.h"
#include "WinMessageBox.h"
#include "config_json_parse.h"
#include "../../Third-Party/USound/src/USound.h"
#include "Map.h"
#include "libimage.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;
using namespace LIB_IMAGE;

extern string DEFAULT_USER_AGENT;
Map::Map<int, AudioInfo> URL;
wstring UserName;

static AudioInfo info;
static USound player;
#define UTF8START "\xef\xbb\xbf"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000 
#define LWA_COLORKEY 0x00000001 
#define LWA_ALPHA 0x00000002 
#endif

class UsesOptionInMethodResult
{
public:
	UsesOptionInMethodResult()
	{
		Option1 = false;
		Option2 = false;
		Option3 = false;
		Option4 = false;
	}
	bool Option1;
	bool Option2;
	bool Option3;
	bool Option4;
};

////Methods
extern void VKAPI_TOOLS_DownloadMusicAll(const string &Token, const string& OldToken, const UsesOptionInMethodResult &Options);
//\\Methods

extern VKApiToolsDialog dlgS;

struct CheckesUses
{
	CheckesUses()
	{
		Uses = false;
		Name = L"Неиспользуется";
		IsSelected = false;
	}
	CheckesUses(int)
	{
		Uses = false;
		Name = L"Неиспользуется";
		IsSelected = false;
	}
	CheckesUses(const wstring &PName, bool PIsSelected = false)
	{
		Uses = true;
		Name = PName;
		IsSelected = PIsSelected;
	}
	bool IsSelected;
	bool Uses;
	wstring Name;
};

class UsesOptionInMethod
{
public:
	UsesOptionInMethod(CheckesUses PCheck1 = NULL, CheckesUses PCheck2 = NULL, CheckesUses PCheck3 = NULL, CheckesUses PCheck4 = NULL)
	{
		Check1 = PCheck1;
		Check2 = PCheck2;
		Check3 = PCheck3;
		Check4 = PCheck4;
	}
	CheckesUses Check1;
	CheckesUses Check2;
	CheckesUses Check3;
	CheckesUses Check4;
};

class Method
{
public:
	Method()
	{
		NeedOldToken = false;
		Func = NULL;
	}
	Method(void* PFunc, bool PNeedOldToken, const UsesOptionInMethod &PUsesOption)
	{
		NeedOldToken = PNeedOldToken;
		Func = PFunc;
		UsesOption = PUsesOption;
	}
	bool NeedOldToken;
	void* Func;
	UsesOptionInMethod UsesOption;
};

class Executing
{
public:
	void REGISTER(const wstring &Name, const void* Func, bool NeedOldToken, const UsesOptionInMethod &UsesOption)
	{
		Methods[Name] = Method((void*)Func, NeedOldToken, UsesOption);
		dlgS.FFunc.AddString(Name.c_str());
		dlgS.FFunc.SetCurSel(dlgS.FFunc.GetCount() - 1);
		dlgS.OnSelect();
	}
	Method GetMethod(const wstring &Name) const
	{
		return Methods[Name];
	}
#define NO_OPTIONS UsesOptionInMethod()
private:
	Map::Map <wstring, Method> Methods;
};
static Executing RegisteredMethods;

std::string tw_tostring(const TextWrite& input)
{
	std::string inp;
	inp.resize(sizeof(TextWrite));
	memcpy((void*)inp.data(), &input, sizeof(TextWrite));
	return inp;
}

string GetCapchaCode(const string &Link, const string &UserAgent)
{
	VKApiToolsDialogCapcha capcha(UTF8_to_wchar(Link).c_str(), UTF8_to_wchar(UserAgent).c_str());
	if (capcha.DoModal() == TRUE)
		return wchar_to_UTF8(capcha.CapchaCode.GetString());
	return "";
}

VKApiToolsDialog::VKApiToolsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(VKApiToolsDialog::IDD, pParent)
{
	m_hIcon = nullptr;
}

VKApiToolsDialog::~VKApiToolsDialog()
{
}

void VKApiToolsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, Edk);
	DDX_Control(pDX, IDC_EDIT2, NewToken);
	DDX_Control(pDX, IDC_COMBO1, FFunc);
	DDX_Control(pDX, IDC_BUTTON1, Exec);
	DDX_Control(pDX, IDC_BUTTON3, Download);
	DDX_Control(pDX, IDC_BUTTON4, Play);
	DDX_Control(pDX, IDC_BUTTON5, Stop);
	DDX_Control(pDX, IDC_CHECK1, Check1);
	DDX_Control(pDX, IDC_CHECK2, Check2);
	DDX_Control(pDX, IDC_CHECK3, Check3);
	DDX_Control(pDX, IDC_CHECK4, Check4);
}

BEGIN_MESSAGE_MAP(VKApiToolsDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnExecute)
	ON_BN_CLICKED(IDC_BUTTON2, OnAccount)
	ON_BN_CLICKED(IDC_BUTTON3, OnDownload)
	ON_BN_CLICKED(IDC_BUTTON4, OnPlay)
	ON_BN_CLICKED(IDC_BUTTON5, OnStop)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelect)
END_MESSAGE_MAP()

static bool Validate(HWND m_hwnd, const string &Token, const string& OldToken, bool NeedOldToken)
{
	if (Token.length() <= 0)
	{
		dlgS.OnAccount();
		return false;
	}
	if (NeedOldToken && OldToken.length() <= 0)
	{
		dlgS.OnAccount();
		return false;
	}
	return true;
}

struct ThParam
{
	ThParam()
	{
		Method = NULL;
	}
	ThParam(const string& PToken, const string& POldToken, void* PMethod, const UsesOptionInMethodResult &POptions)
	{
		Token = PToken;
		OldToken = POldToken;
		Method = PMethod;
		Options = POptions;
	}
	string Token;
	string OldToken;
	UsesOptionInMethodResult Options;
	void* Method;
};

typedef void(*method_t)(const string &Token, const string& OldToken, const UsesOptionInMethodResult &Options);

DWORD WINAPI RunMeth(LPVOID param)
{
	ThParam par = *((ThParam*)param);
	free(param);
	dlgS.Exec.EnableWindow(FALSE);
	dlgS.FFunc.EnableWindow(FALSE);

	FILE* fl = NULL;
	if (_wfopen_s(&fl, PREFS_NAME.c_str(), L"wb") == 0)
	{
		if (fl) {
			string jsonS = "{ \"current_token\":\"" + par.Token + "\", \"old_token\":\"" + par.OldToken + "\", \"user_agent\":\"" + DEFAULT_USER_AGENT + "\" }";
			fwrite(UTF8START, 1, strlen(UTF8START), fl);
			fwrite(jsonS.c_str(), 1, jsonS.length(), fl);
			fclose(fl);
		}
	}
	((method_t)par.Method)(par.Token, par.OldToken, par.Options);

	for (auto& i : URL) {
		dlgS.Edk.AddLine(UTF8_to_wchar(i.get_value().artist) + L" - " + UTF8_to_wchar(i.get_value().title));
	}
	dlgS.Edk.UpdateLines();

	dlgS.Exec.EnableWindow(TRUE);
	dlgS.FFunc.EnableWindow(TRUE);
	return 0;
}


void VKApiToolsDialog::OnSelectAudio(VKApiToolsDialog* pClass, int Idx) {
	if (Idx >= 0) {
		pClass->Download.EnableWindow(TRUE);
		pClass->Play.EnableWindow(TRUE);
		int oo = 0;
		for (auto& i : URL) {
			if (oo == Idx) {
				info = i.get_value();
				break;
			}
			oo++;
		}
	}
	else {
		pClass->Download.EnableWindow(FALSE);
		pClass->Play.EnableWindow(FALSE);
	}
}

void VKApiToolsDialog::OnDownload() {
	string MP3M;
	string flnm = info.artist + "/" + info.artist + " - " + info.title;
	if (DoCurlGet(info.url, DEFAULT_USER_AGENT, MP3M, false) > 0 && MP3M.length() > 0)
	{
		FlBin track;
		track.wopen(FixFileName(UserName) + L"\\" + FixFileName(UTF8_to_wchar(flnm + ".mp3")), OPENType::OPENWrite, true);
		track.BWrite(MP3M.data(), 1, MP3M.size());
		track.Close();
		(win_message(m_hWnd) << L"Успешно " << FixFileName(UserName) << L"\\" << FixFileName(UTF8_to_wchar(flnm + ".mp3"))).message_type(MSG_TYPE::TYPE_INFO).timeout(5).show();
	}
}

void VKApiToolsDialog::OnStop() {
	player.Stop();
}

void VKApiToolsDialog::OnPlay() {
	player.Stop();
	string MP3M;
	if (DoCurlGet(info.url, DEFAULT_USER_AGENT, MP3M, false) > 0 && MP3M.length() > 0)
	{
		player.RegisterResourceMP3Sounds(MP3M);
		player.PlayMemorySound(false, false);
	}
}

BOOL VKApiToolsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 230);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	std::string DataPic = GetDataFromResourceUtil(L"SVG", IDB_SVG1);
	auto mk = Edk.getRect();
	Edk.Init(PrepareImageFromSVG(Edk.m_hWnd, mk.Width(), mk.Height(), DataPic.data(), (int)DataPic.size(), GetSysColor(COLOR_3DFACE)).get_hBitmap(true), CallBackSelectTouch(VKApiToolsDialog::OnSelectAudio, this), NULL);

	Check1.EnableWindow(FALSE);
	Check2.EnableWindow(FALSE);
	Check3.EnableWindow(FALSE);
	Check4.EnableWindow(FALSE);

	Download.SetBitmap(PrepareImageFromSVGResourceToIcon(Download.m_hWnd, IDB_SVG5));
	Download.EnableWindow(FALSE);

	Play.SetBitmap(PrepareImageFromSVGResourceToIcon(Play.m_hWnd, IDB_SVG4));
	Play.EnableWindow(FALSE);
	Stop.SetBitmap(PrepareImageFromSVGResourceToIcon(Stop.m_hWnd, IDB_SVG3));

	RegisteredMethods.REGISTER(L"Получить музыку из ВК", VKAPI_TOOLS_DownloadMusicAll, false, UsesOptionInMethod(CheckesUses(L"Получить со стены?", false)));

	try {
		json settings;
		if (parseJsonConfig(PREFS_NAME, settings)) {
			NewToken.SetWindowTextW(UTF8_to_wchar(settings.at("current_token").get<string>()).c_str());
			DEFAULT_USER_AGENT = settings.at("user_agent").get<string>();
		}
	}
	catch (json::exception e) {
		(win_message().message_type(MSG_TYPE::TYPE_ERROR).timeout(10) << e.what()).show();
	}

	return TRUE;
}

void VKApiToolsDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void VKApiToolsDialog::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

HCURSOR VKApiToolsDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void VKApiToolsDialog::OnExecute()
{
	CString Str;
	FFunc.GetWindowTextW(Str);
	const Method& Mt = RegisteredMethods.GetMethod(Str.GetString());
	CString Tkn;
	CString OldTkn;
	NewToken.GetWindowTextW(Tkn);
	if (!Validate(m_hWnd, wchar_to_UTF8(Tkn.GetString()), wchar_to_UTF8(OldTkn.GetString()), Mt.NeedOldToken))
		return;
	Edk.Clear();
	Download.EnableWindow(FALSE);
	Play.EnableWindow(FALSE);
	UsesOptionInMethodResult Options;
	Options.Option1 = Check1.GetCheck() == TRUE;
	Options.Option2 = Check2.GetCheck() == TRUE;
	Options.Option3 = Check3.GetCheck() == TRUE;
	Options.Option4 = Check4.GetCheck() == TRUE;
	CreateThreadDetachedSimple(&RunMeth, new ThParam(wchar_to_UTF8(Tkn.GetString()), wchar_to_UTF8(OldTkn.GetString()), Mt.Func, Options));
}

void VKApiToolsDialog::OnSelect()
{
	CString Str;
	FFunc.GetWindowTextW(Str);
	const Method& Mt = RegisteredMethods.GetMethod(Str.GetString());
	Check1.EnableWindow(Mt.UsesOption.Check1.Uses == true ? TRUE : FALSE);
	Check1.SetCheck(Mt.UsesOption.Check1.IsSelected == true ? TRUE : FALSE);
	Check1.SetWindowTextW(Mt.UsesOption.Check1.Name.c_str());
	Check2.EnableWindow(Mt.UsesOption.Check2.Uses == true ? TRUE : FALSE);
	Check2.SetCheck(Mt.UsesOption.Check2.IsSelected == true ? TRUE : FALSE);
	Check2.SetWindowTextW(Mt.UsesOption.Check2.Name.c_str());
	Check3.EnableWindow(Mt.UsesOption.Check3.Uses == true ? TRUE : FALSE);
	Check3.SetCheck(Mt.UsesOption.Check3.IsSelected == true ? TRUE : FALSE);
	Check3.SetWindowTextW(Mt.UsesOption.Check3.Name.c_str());
	Check4.EnableWindow(Mt.UsesOption.Check4.Uses == true ? TRUE : FALSE);
	Check4.SetCheck(Mt.UsesOption.Check4.IsSelected == true ? TRUE : FALSE);
	Check4.SetWindowTextW(Mt.UsesOption.Check4.Name.c_str());
}

void VKApiToolsDialog::OnAccount()
{
	VKApiToolsInputLogin Acc;
	Acc.DoModal();
}

void VKApiToolsDialog::OnClose()
{
	XTPSkinMgr()->ExitProgramm();
}