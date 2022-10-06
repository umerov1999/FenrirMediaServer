#include "pch.h"
#include <iostream>
#include "VKApiToolsInputLogin.h"
#include "VKApiToolsDialog.h"
#include "WSTRUtils.h"
#include "WinMessageBox.h"
#include "UsrInf.h"
#include "json.hpp"
#include "Map.h"
#include "do_curl.h"
#include "pugixml/pugixml.hpp"
#include "libimage.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;
using namespace LIB_IMAGE;

extern UserInfo CallToGetUserNameById(const string &Token, const string &UserAgent, int UserId);
extern VKApiToolsDialog dlgS;
static Map::Map<string, string>KateUsers;

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

VKApiToolsInputLogin::VKApiToolsInputLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(VKApiToolsInputLogin::IDD, pParent)
{
}

VKApiToolsInputLogin::~VKApiToolsInputLogin()
{
}

void VKApiToolsInputLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VK_ICON, VK_Icon);
	DDX_Control(pDX, IDC_EDIT1, EdLogin);
	DDX_Control(pDX, IDC_EDIT2, EdToken);
	DDX_Control(pDX, IDC_EDIT3, UserName);
	DDX_Control(pDX, IDC_EDIT4, EdPassword);
	DDX_Control(pDX, IDC_EDIT5, SMS);
	DDX_Control(pDX, IDC_COMBO1, KateMobilePref);
}

BEGIN_MESSAGE_MAP(VKApiToolsInputLogin, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnLogin)
	ON_BN_CLICKED(IDC_BUTTON2, CheckAccess)
	ON_BN_CLICKED(IDC_BUTTON3, OnToken)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelectKateUser)
END_MESSAGE_MAP()

BOOL VKApiToolsInputLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CRect rt;
	VK_Icon.GetClientRect(rt);
	std::string Data = GetDataFromResourceUtil(L"PNG", IDB_PNG1);
	VK_IconBitmap = PrepareImageFromBufferAutoType(VK_Icon.m_hWnd, Data.data(), (int)Data.size()).resize(rt.Width(), rt.Height());
	VK_Icon.SetBitmap(VK_IconBitmap.get_hBitmap());

	KateMobilePref.ResetContent();
	for (auto& i : KateUsers)
		KateMobilePref.AddString(UTF8_to_wchar(i.get_key()).c_str());

	SMS.EnableWindow(FALSE);
	return TRUE;
}

void VKApiToolsInputLogin::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void VKApiToolsInputLogin::OnPaint()
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

HCURSOR VKApiToolsInputLogin::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void VKApiToolsInputLogin::CheckAccess_TOKEN(std::string Token)
{
	try {
		UserInfo inf = CallToGetUserNameById(Token, ANDROID_USERAGENT, 0);
		CRect rt;
		VK_Icon.GetClientRect(rt);
		string Data;
		DoCurlGet(inf.pAvatarLink, ANDROID_USERAGENT, Data, false);
		VK_IconBitmap = PrepareImageFromBufferAutoType(VK_Icon.m_hWnd, Data.data(), (int)Data.size()).resize(rt.Width(), rt.Height());
		VK_Icon.SetBitmap(VK_IconBitmap.get_hBitmap());
		UserName.SetWindowTextW((inf.pUserName + L" (id" + to_wstring(inf.user_id) + L")").c_str());
		return;
	}
	catch (json::exception e) {
		(win_message().message_type(MSG_TYPE::TYPE_ERROR) << L"Статус: Ошибка парсинга ответа : " << e.what()).show();
		return;
	}
}

void VKApiToolsInputLogin::CheckAccess()
{
	if (EdToken.GetWindowTextLengthW() <= 0)
		return;
	CString EnteredAccessToken;
	EdToken.GetWindowTextW(EnteredAccessToken);
	CheckAccess_TOKEN(wchar_to_UTF8(EnteredAccessToken.GetString()));
}

void VKApiToolsInputLogin::OnLogin()
{
	if (EdLogin.GetWindowTextLengthW() <= 0 || EdPassword.GetWindowTextLengthW() <= 0)
		return;

	CString Login;
	CString Password;
	EdLogin.GetWindowTextW(Login);
	EdPassword.GetWindowTextW(Password);
	string AnswerVK;
	stringstream Link;

	Link << "https://oauth.vk.com/token?grant_type=password&client_id=" << "2685278" << "&client_secret=" << "lxhD8OD7dMsqtXIm5IUY" << "&2fa_supported=1&force_sms=1&libverify_support=1&scope=friends,photos,audio,video,stories,pages,messages,wall,offline,docs,groups&v=" << VKAPI_VERSION_AUTH << "&username=" << url_encode(wchar_to_UTF8(Login.GetString())) << "&password=" << url_encode(wchar_to_UTF8(Password.GetString()));
	if (SMS.GetWindowTextLengthW() > 0)
	{
		CString SMSCOde;
		SMS.GetWindowTextW(SMSCOde);
		Link << "&code=" << url_encode(wchar_to_UTF8(SMSCOde.GetString()));
		SMS.SetWindowTextW(L"");
	}
	DoCurlGet(Link.str(), ANDROID_USERAGENT, AnswerVK, true);
	json jres;
	try {
		jres = json::parse(AnswerVK);
		if (jres.find("error") == jres.end() || jres.find("access_token") != jres.end()) {
			string token = jres.at("access_token").get<string>();
			EdToken.SetWindowTextW(UTF8_to_wchar(token).c_str());
			CheckAccess_TOKEN(token);
			return;
		}
		else
		{
			string ErrorType = jres.at("error").get<string>();
			if (ErrorType == "need_validation")
			{
				string PhoneMask;
				if (jres.find("phone_mask") == jres.end())
					PhoneMask = (const char*)u8"Неопознанная двухфакторная авторизация";
				else
					PhoneMask = (const char*)u8"Введите код, отправленный на" + jres.at("phone_mask").get<string>();
				if (jres.find("validation_sid") != jres.end())
					DoCurlGet(string("https://api.vk.com/method/auth.validatePhone?client_id=") + "2685278" + "&api_id=" + "2685278" + "&client_secret=" + "lxhD8OD7dMsqtXIm5IUY" + "&sid=" + jres.at("validation_sid").get<string>() + "&v=" + VKAPI_VERSION_AUTH, ANDROID_USERAGENT, AnswerVK, true);
				SMS.EnableWindow(TRUE);
				(win_message().message_type(MSG_TYPE::TYPE_WARNING) << L"Валидация: " << PhoneMask).show();
			}
			else
				(win_message().message_type(MSG_TYPE::TYPE_ERROR) << L"Статус: Ошибка: " << AnswerVK).show();
			return;
		}
	}
	catch (json::exception e) {
		(win_message().message_type(MSG_TYPE::TYPE_ERROR) << L"Статус: Ошибка парсинга ответа :" << e.what()).show();
		(win_message().message_type(MSG_TYPE::TYPE_INFO) << L"Статус: JSON: " << AnswerVK).show();
		return;
	}
}

void VKApiToolsInputLogin::OnSelectKateUser()
{
	CString Sel;
	KateMobilePref.GetWindowTextW(Sel);
	EdToken.SetWindowTextW(UTF8_to_wchar(KateUsers[wchar_to_UTF8(Sel.GetString())]).c_str());
	CheckAccess();
}

BOOL VKApiToolsInputLogin::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf) / sizeof(wchar_t));
				wstring dFile = szBuf;
				wstring Ext = dFile.substr(dFile.find_last_of(L'.'));
				if (Ext == L".json")
				{
					FILE* fl = NULL;
					if (_wfopen_s(&fl, dFile.c_str(), L"rb") != 0)
						return CDialogEx::PreTranslateMessage(pMsg);
					fseek(fl, 0, SEEK_END);
					int fsize = ftell(fl);
					fseek(fl, 0, SEEK_SET);
					if (fsize > 3) {
						vector<char> hdr(3);
						fread(hdr.data(), 1, 3, fl);
						if (memcmp(hdr.data(), "\xef\xbb\xbf", 3) != 0) {
							fseek(fl, 0, SEEK_SET);
						}
						else {
							fsize -= 3;
						}
					}
					string bu;
					bu.resize(fsize + 1);
					fread((char*)bu.data(), 1, fsize, fl);
					fclose(fl);
					json jres;
					try {
						jres = json::parse(bu).at("fenrir_accounts");
						for (json::iterator it = jres.begin(); it != jres.end(); ++it)
						{
							json acc = it.value();
							KateUsers[acc.at("user_name").get<string>() + " [" + to_string(acc.at("user_id").get<int>()) + "]"] = acc.at("access_token").get<string>();
						}
						KateMobilePref.ResetContent();
						for (auto& i : KateUsers)
							KateMobilePref.AddString(UTF8_to_wchar(i.get_key()).c_str());
						KateMobilePref.SetCurSel(KateMobilePref.GetCount() - 1);
						OnSelectKateUser();
					}
					catch (json::exception e) {
						(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << e.what()).show();
						return CDialogEx::PreTranslateMessage(pMsg);
					}
				}
				else if (Ext == L".xml")
				{
					KateUsers.clear();
					pugi::xml_document doc;
					doc.load_file(dFile.c_str());
					auto lv_Node = doc.child("map");
					string jsh;
					for (auto& lv_Val : lv_Node)
					{
						if (string(lv_Val.attribute("name").as_string()) == "accounts")
						{
							jsh = lv_Val.text().get();
							break;
						}
					}
					json jres;
					try {
						jres = json::parse(jsh);
						for (json::iterator it = jres.begin(); it != jres.end(); ++it)
						{
							json acc = it.value();
							KateUsers[acc.at("user_name").get<string>() + " [" + acc.at("mid").get<string>() + "]"] = acc.at("access_token").get<string>();
						}
					}
					catch (json::exception e) {
						(win_message().timeout(20).message_type(MSG_TYPE::TYPE_ERROR) << e.what()).show();
						(win_message().timeout(20).message_type(MSG_TYPE::TYPE_ERROR) << jsh).show();
						return CDialogEx::PreTranslateMessage(pMsg);
					}
					KateMobilePref.ResetContent();
					for (auto& i : KateUsers)
						KateMobilePref.AddString(UTF8_to_wchar(i.get_key()).c_str());
					KateMobilePref.SetCurSel(KateMobilePref.GetCount() - 1);
					OnSelectKateUser();
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void VKApiToolsInputLogin::OnToken()
{
	CString Token;
	EdToken.GetWindowTextW(Token);
	dlgS.NewToken.SetWindowTextW(Token);
}

void VKApiToolsInputLogin::OnClose()
{
	EndDialog(0);
}