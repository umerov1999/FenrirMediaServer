﻿#include "pch.h"
#include <iostream>
#include "base.hpp"
#include "json.hpp"
#include "VKApiToolsDialog.h"
#include "VKApiToolsDialogCapcha.h"
#include "VKApiToolsInputLogin.h"
#include "WinMessageBox.h"
#include "Map.h"
#include "WSTRUtils.h"
#include "libimage.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;
using namespace LIB_IMAGE;

static std::wstring GetAppName()
{
	const wchar_t* FnS = L"\\/";
	TCHAR szFileName[MAX_PATH];
	GetModuleFileNameW(0, szFileName, MAX_PATH);

	std::wstring fname = szFileName;
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(pos + 1);
}
#define SETTINGS_FILE (GetAppName() + L".settings.json")
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

struct UsesOptionInMethodResult
{
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
extern void VKAPI_TOOLS_DumpMessages(const string& Token, const UsesOptionInMethodResult& Options);
//\\Methods

extern VKApiToolsDialog dlgS;
static HANDLE hMessageMutex;
static HANDLE hDownloadMutex;

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
		Func = NULL;
	}
	Method(void* PFunc, const UsesOptionInMethod &PUsesOption)
	{
		Func = PFunc;
		UsesOption = PUsesOption;
	}
	void* Func;
	UsesOptionInMethod UsesOption;
};

class Executing
{
public:
	void REGISTER(const wstring &Name, const void* Func, const UsesOptionInMethod &UsesOption)
	{
		Methods[Name] = Method((void*)Func, UsesOption);
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

void PrintMessage(const string& Msg, URGB Color)
{
	if (Msg.length() <= 0)
		return;
	WaitForSingleObject(hMessageMutex, INFINITE);
	TextWrite LineData(Color, (unsigned short)Msg.length());
	dlgS.Edk.AddLines(base91::encode(tw_tostring(LineData)) + " " + Msg);
	ReleaseMutex(hMessageMutex);
}

void RemoveLastLine()
{
	WaitForSingleObject(hMessageMutex, INFINITE);
	dlgS.Edk.RemoveLastLine();
	ReleaseMutex(hMessageMutex);
}

ColoredMessage getLastLine()
{
	WaitForSingleObject(hMessageMutex, INFINITE);
	ColoredMessage ret = dlgS.Edk.GetLastLine();
	ReleaseMutex(hMessageMutex);
	return ret;
}

void PrintDownload(const string& Msg, URGB Color)
{
	if (Msg.length() <= 0)
		return;
	WaitForSingleObject(hDownloadMutex, INFINITE);
	TextWrite LineData(Color, (unsigned short)Msg.length());
	dlgS.EDownload.AddLines(base91::encode(tw_tostring(LineData)) + " " + Msg);
	ReleaseMutex(hDownloadMutex);
}


VKApiToolsDialog::VKApiToolsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(VKApiToolsDialog::IDD, pParent)
{
}

VKApiToolsDialog::~VKApiToolsDialog()
{
}

void VKApiToolsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, Edk);
	DDX_Control(pDX, IDC_EDIT2, NewToken);
	DDX_Control(pDX, IDC_EDIT4, EDownload);
	DDX_Control(pDX, IDC_EDIT6, Anim);
	DDX_Control(pDX, IDC_COMBO1, FFunc);
	DDX_Control(pDX, IDC_BUTTON1, Exec);
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
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelect)
END_MESSAGE_MAP()

static bool Validate(HWND m_hwnd, const string &Token)
{
	if (Token.length() <= 0)
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
	ThParam(const string &PToken, void* PMethod, const UsesOptionInMethodResult &POptions)
	{
		Token = PToken;
		Method = PMethod;
		Options = POptions;
	}
	string Token;
	UsesOptionInMethodResult Options;
	void* Method;
};

typedef void(*method_t)(const string &Token, const UsesOptionInMethodResult &Options);

extern int LOADING_ANIMATION_SIZE;
extern const char* LOADING_ANIMATION_DATA;

extern int SUCCESS_ANIMATION_SIZE;
extern const char* SUCCESS_ANIMATION_DATA;

extern int ERROR_ANIMATION_SIZE;
extern const char* ERROR_ANIMATION_DATA;

extern int FLAME_ANIMATION_SIZE;
extern const char* FLAME_ANIMATION_DATA;

DWORD WINAPI RunMeth(LPVOID param)
{
	ThParam par = *((ThParam*)param);
	free(param);
	dlgS.Exec.EnableWindow(FALSE);
	dlgS.FFunc.EnableWindow(FALSE);

	FILE* fl = NULL;
	if (_wfopen_s(&fl, SETTINGS_FILE.c_str(), L"wb") == 0)
	{
		string jsonS = "{ \"current_token\":\"" + par.Token + "\" }";
		fwrite(UTF8START, 1, strlen(UTF8START), fl);
		fwrite(jsonS.c_str(), 1, jsonS.length(), fl);
		fclose(fl);
	}
	dlgS.Anim.load_animation(URGB(0, 0, 0), LOADING_ANIMATION_DATA, LOADING_ANIMATION_SIZE);

	PrintMessage("=====================", URGB(0, 170, 100));
	PrintMessage((const char*)u8"Запуск функции...", URGB(0, 170, 100));
	PrintMessage("=====================", URGB(0, 170, 100));
	PrintMessage(" ", URGB(0, 170, 100));
	((method_t)par.Method)(par.Token, par.Options);
	PrintMessage(" ", URGB(255, 0, 0));
	PrintMessage("=====================", URGB(255, 0, 0));
	PrintMessage((const char*)u8"Функция завершена", URGB(255, 0, 0));
	PrintMessage("=====================", URGB(255, 0, 0));
	dlgS.Exec.EnableWindow(TRUE);
	dlgS.FFunc.EnableWindow(TRUE);
	dlgS.Anim.load_animation(URGB(0, 0, 0), SUCCESS_ANIMATION_DATA, SUCCESS_ANIMATION_SIZE);
	return 0;
}

BOOL VKApiToolsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 240);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	Anim.load_animation(URGB(0, 0, 0), FLAME_ANIMATION_DATA, FLAME_ANIMATION_SIZE);

	std::string DataPic = GetDataFromResourceUtil(L"SVG", IDB_SVG1);
	auto mk = Edk.getRect();
	Edk.Init(Align::LEFT_ALIGN, PrepareImageFromSVG(Edk.m_hWnd, mk.Width(), mk.Height(), DataPic.data(), (int)DataPic.size()).get_hBitmap(true), false);
	EDownload.RegisterSpecialPatternOnce(L"[w]", URGB(255, 0, 0));
	EDownload.RegisterSpecialPatternOnce(L"[z]", URGB(255, 200, 0));

	Check1.EnableWindow(FALSE);
	Check2.EnableWindow(FALSE);
	Check3.EnableWindow(FALSE);
	Check4.EnableWindow(FALSE);

	RegisteredMethods.REGISTER(L"Дамп переписок", VKAPI_TOOLS_DumpMessages, UsesOptionInMethod(CheckesUses(L"Авы в base64 (HTML)?", true), CheckesUses(L"Новые сообщения?", true), CheckesUses(L"Дамп альбомов и доков?", false)));

	hMessageMutex = CreateMutexW(NULL, FALSE, NULL);
	hDownloadMutex = CreateMutexW(NULL, FALSE, NULL);

	FILE* fl = NULL;
	if (_wfopen_s(&fl, SETTINGS_FILE.c_str(), L"rb") == 0)
	{
		string jsonS;
		fseek(fl, 0, SEEK_END);
		jsonS.resize(ftell(fl) - strlen(UTF8START) + 1);
		fseek(fl, (int)strlen(UTF8START), SEEK_SET);
		fread((char*)jsonS.data(), 1, jsonS.size() - 1, fl);
		fclose(fl);
		try {
			json info = json::parse(jsonS);
			NewToken.SetWindowTextW(UTF8_to_wchar(info.at("current_token").get<string>()).c_str());
		}
		catch (json::exception e) {
			e.what();
		}
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

static inline std::wstring to_lower(std::wstring s) {
	wstring ret = s;
	for (auto& i : ret) {
		i = ToWLower(i);
	}
	return ret;
}

void VKApiToolsDialog::OnExecute()
{
	CString Str;
	FFunc.GetWindowTextW(Str);
	const Method& Mt = RegisteredMethods.GetMethod(Str.GetString());
	CString Tkn;
	NewToken.GetWindowTextW(Tkn);
	if (!Validate(m_hWnd, wchar_to_UTF8(Tkn.GetString()))) {
		Anim.load_animation(URGB(0, 0, 0), ERROR_ANIMATION_DATA, ERROR_ANIMATION_SIZE);
		return;
	}
	Edk.RemoveAllLines();
	EDownload.RemoveAllLines();

	UsesOptionInMethodResult Options;
	Options.Option1 = Check1.GetCheck() == TRUE;
	Options.Option2 = Check2.GetCheck() == TRUE;
	Options.Option3 = Check3.GetCheck() == TRUE;
	Options.Option4 = Check4.GetCheck() == TRUE;
	CreateThread(NULL, NULL, &RunMeth, new ThParam(wchar_to_UTF8(Tkn.GetString()), Mt.Func, Options), NULL, NULL);
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