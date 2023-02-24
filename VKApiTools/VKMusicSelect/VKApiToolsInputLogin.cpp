#include "pch.h"
#include <iostream>
#include "VKApiToolsInputLogin.h"
#include "VKApiToolsDialog.h"
#include "WSTRUtils.h"
#include "WinMessageBox.h"
#include "UsrInf.h"
#include "json.hpp"
#include "Map.h"
#include "curl/curl.h"
#include "do_curl.h"
#include "pugixml/pugixml.hpp"
#include "libimage.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;
using namespace LIB_IMAGE;

string DEFAULT_USER_AGENT = ANDROID_USERAGENT;

extern UserInfo CallToGetUserNameById(const string &Token, const string &UserAgent, int64_t UserId);
extern VKApiToolsDialog dlgS;

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

class HParam
{
public:
	HParam(const string &TName, const string& TParam)
	{
		Name = url_encode(TName);
		Param = url_encode(TParam);
	}
	HParam(const string& TName, int TParam)
	{
		Name = url_encode(TName);
		Param = url_encode(to_string(TParam));
	}
	HParam(const string& TName, int64_t TParam)
	{
		Name = url_encode(TName);
		Param = url_encode(to_string(TParam));
	}
	string Name;
	string Param;
};

class HTTPParams
{
public:
	template<typename ...Args>
	static constexpr std::size_t va_count(const Args...) { return sizeof...(Args); }
	static HTTPParams make_param(size_t count, HTTPParams old, ...)
	{
		HTTPParams Ret;
		if (old.Params.length() <= 0)
			Ret.Params += old.Params;
		else
			Ret.Params += ("&" + old.Params);
		if (count <= 0)
			return Ret;
		va_list args;
		va_start(args, old);
		for (size_t i = 0; i < count; i++)
		{
			HParam pp = va_arg(args, HParam);
			if (Ret.Params.length() > 0)
				Ret.Params += "&";
			Ret.Params += pp.Name;
			Ret.Params += "=";
			Ret.Params += pp.Param;
		}
		va_end(args);
		return Ret;
	}
#define HTTP_PARAMS(ptr,...) HTTPParams::make_param(HTTPParams::va_count(__VA_ARGS__), ptr, __VA_ARGS__)
	string Params;
};

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
}

BEGIN_MESSAGE_MAP(VKApiToolsInputLogin, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnLogin)
	ON_BN_CLICKED(IDC_BUTTON3, OnToken)
END_MESSAGE_MAP()

static string generateRandomString(size_t length = 10) {
	string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";
	size_t charactersLength = characters.length();
	string randomString;
	for (size_t i = 0; i < length; i++) {
		randomString += characters[rand() % charactersLength];
	}
	return randomString;
}

static string generateRandomIntString(size_t length = 10) {
	string characters = "0123456789abcdef";
	size_t charactersLength = characters.length();
	string randomString;
	for (size_t i = 0; i < length; i++) {
		randomString += characters[rand() % charactersLength];
	}
	return randomString;
}

class CURL_WR
{
public:
	CURL_WR()
	{
		Data = NULL;
		WritePos = 0;
		Size = -1;
		curl_handle = NULL;
		ContentHasSize = false;
	}
	void reset()
	{
		Data->clear();
		ContentHasSize = false;
		Size = -1;
		WritePos = 0;
	}
	std::string* Data;
	int WritePos;
	int Size;
	bool ContentHasSize;
	void* curl_handle;
};

static size_t CurlWriter(char* Buf, size_t ElementSize, size_t ElementCount, CURL_WR* ReciveData)
{
	if (ReciveData == NULL)
		return 0;
	if (!ReciveData->ContentHasSize)
	{
		curl_off_t sized = -1;
		curl_easy_getinfo((CURL*)ReciveData->curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &sized);
		if (sized > 0)
		{
			ReciveData->Data->resize((int)sized);
			ReciveData->Size = (int)sized;
		}
		ReciveData->ContentHasSize = true;
	}
	if (ReciveData->Size < (ReciveData->WritePos + (int)(ElementSize * ElementCount)))
		ReciveData->Data->resize(ReciveData->WritePos + (int)(ElementSize * ElementCount));
	memcpy(((char*)ReciveData->Data->data()) + ReciveData->WritePos, Buf, (int)ElementSize * ElementCount);
	ReciveData->WritePos += (int)(ElementSize * ElementCount);
	return ElementSize * ElementCount;
}

static bool getReceipt(const string &device_id, int64_t id, const string &token) {
	CURL_WR wrt;
	string ReciveData;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
		curl_easy_setopt(curl_handle, CURLOPT_URL, "https://android.clients.google.com/c2dm/register3");
		struct curl_slist* list = NULL;
		list = curl_slist_append(list, "Authorization: AidLogin");
		list = curl_slist_append(list, (device_id + ":" + token).c_str());

		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Android-GCM/1.5 (generic_x86 KK)");
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 20);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 20);
		HTTPParams Posts = HTTP_PARAMS(HTTPParams(),
			HParam("X-scope", "GCM"),
			HParam("X-osv", "23"),
			HParam("X-subtype", "54740537194"),
			HParam("X-app_ver", "460"),
			HParam("X-kid", "|ID|1|"),
			HParam("X-appid", generateRandomString(11)),
			HParam("X-gmsv", "200313005"),
			HParam("X-cliv", "iid-12211000"),
			HParam("X-app_ver_name", "56 lite"),
			HParam("X-X-kid", "|ID|1|"),
			HParam("X-subscription", "54740537194"),
			HParam("X-X-subscription", "54740537194"),
			HParam("X-X-subtype", "54740537194"),
			HParam("app", "com.perm.kate_new_6"),
			HParam("sender", "54740537194"),
			HParam("device", device_id),
			HParam("cert", "966882ba564c2619d55d0a9afd4327a38c327456"),
			HParam("app_ver", "460"),
			HParam("info", "U_ojcf1ahbQaUO6eTSP7b7WomakK_hY"),
			HParam("gcm_ver", "200313005"),
			HParam("plat", "0"),
			HParam("target_ver", "28"));

		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, Posts.Params.c_str());
		curl_easy_perform(curl_handle);
		(win_message() << L"[-1] " << ReciveData).show();
		wrt.reset();
		Posts = HTTP_PARAMS(Posts, HParam("X-scope", "id" + to_string(id)),
			HParam("X-kid", "|ID|2|"), HParam("X-X-kid", "|ID|2|"));
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, Posts.Params.c_str());
		CURLcode res = curl_easy_perform(curl_handle);
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(list);
		(win_message() << L"[" << Code << L"] " << ReciveData).show();
		return true;
	}
	return false;
}

void UpgradeToken(int64_t id, const string &token)
{
	string device_id = generateRandomIntString(16);
	getReceipt(device_id, id, token);
}

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

void VKApiToolsInputLogin::CheckAccess_TOKEN(const std::string &Token)
{
	try {
		UserInfo inf = CallToGetUserNameById(Token, KATE_USERAGENT, 0);
		CRect rt;
		VK_Icon.GetClientRect(rt);
		string Data;
		DoCurlGet(inf.pAvatarLink, KATE_USERAGENT, Data, false);
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

	string DeviceID = generateRandomIntString(16);

	//Link << "https://oauth.vk.com/token?grant_type=password&client_id=" << "2274003" << "&client_secret=" << "hHbZxrka2uZ6jB1inYsH" << "&2fa_supported=1&scope=nohttps,all&v=" << VKAPI_VERSION_AUTH << "&username=" << url_encode(wchar_to_UTF8(Login.GetString())) << "&password=" << url_encode(wchar_to_UTF8(Password.GetString())) << "&lang=ru&device_id=" << DeviceID;
	Link << "https://oauth.vk.com/token?grant_type=password&client_id=" << "2274003" << "&client_secret=" << "hHbZxrka2uZ6jB1inYsH" << "&2fa_supported=1&scope=all&v=" << VKAPI_VERSION_AUTH << "&username=" << url_encode(wchar_to_UTF8(Login.GetString())) << "&password=" << url_encode(wchar_to_UTF8(Password.GetString())) << "&lang=ru&device_id=" << DeviceID;
	bool hasSMS = false;
	if (SMS.GetWindowTextLengthW() > 0)
	{
		CString SMSCOde;
		SMS.GetWindowTextW(SMSCOde);
		Link << "&code=" << url_encode(wchar_to_UTF8(SMSCOde.GetString()));
		SMS.SetWindowTextW(L"");
		hasSMS = true;
	}
	DoCurlGet(Link.str(), ANDROID_USERAGENT, AnswerVK, true);
	json jres;
	try {
		jres = json::parse(AnswerVK);
		if (jres.find("error") == jres.end() || jres.find("access_token") != jres.end()) {
			string token = jres.at("access_token").get<string>();
			DEFAULT_USER_AGENT = ANDROID_USERAGENT;
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
					DoCurlGet(string("https://api.vk.com/method/auth.validatePhone?client_id=") + "2274003" + "&api_id=" + "2274003" + "&client_secret=" + "hHbZxrka2uZ6jB1inYsH" + "&sid=" + jres.at("validation_sid").get<string>() + "&v=" + VKAPI_VERSION_AUTH, ANDROID_USERAGENT, AnswerVK, true);
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

void VKApiToolsInputLogin::OnToken()
{
	CString Token;
	EdToken.GetWindowTextW(Token);
	if (Token.GetLength() <= 0) {
		return;
	}
	dlgS.NewToken.SetWindowTextW(Token);
	EndDialog(0);
}

void VKApiToolsInputLogin::OnClose()
{
	EndDialog(0);
}
