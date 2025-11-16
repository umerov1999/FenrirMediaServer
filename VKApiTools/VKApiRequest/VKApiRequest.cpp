#include "pch.h"
#include <iostream>
#include <list>
#include <regex>
#include <Windows.h>
#include <Shlwapi.h>
#include "do_curl.h"
#include "vk_api_interface.h"
#include "Map.h"
#include "urgb.h"
#include "json.hpp"
#include "WSTRUtils.h"
#include "UsrInf.h"
#include "WinMessageBox.h"
#include "VKApiToolsDialogCapcha.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;

#define AVATAR_USER_DEFAULT "https://vk.ru/images/camera_200.png?ava=1"
#define AVATAR_CHAT_DEFAULT "https://vk.ru/images/icons/im_multichat_200.png"
#define UTF8START "\xef\xbb\xbf"

////VKAPITOOLSDIALOGINTERFACE
extern void PrintMessage(const string &Msg, URGB Color);
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

class Out
{
public:
	Out()
	{
		Color = URGB();
		IsFlush = true;
	}
	Out(URGB PColor)
	{
		Color = PColor;
		IsFlush = false;
	}
	Out& operator<<(const wstring &msg) { UTF8Message += wchar_to_UTF8(msg); return *this; }
	Out& operator<<(const wchar_t *msg) { UTF8Message += wchar_to_UTF8(msg); return *this; }
	Out& operator<<(const string &msg) { UTF8Message += msg; return *this; }
	Out& operator<<(const char* msg) { UTF8Message += msg; return *this; }
	Out& operator<<(int msg) { UTF8Message += to_string(msg); return *this; }
	Out& operator<<(long long msg) { UTF8Message += to_string(msg); return *this; }
	Out& operator<<(size_t msg) { UTF8Message += to_string(msg); return *this; }
	Out& operator<<(char msg) { UTF8Message.push_back(msg); return *this; }
	Out& operator<<(bool msg) { UTF8Message += (msg == true ? "true" : "false"); return *this; }
	Out& operator<<(const json &msg) { UTF8Message += msg.dump(4); return *this; }
	Out& operator<<(const Out& msg) { if (msg.IsFlush) { flush(); } return *this; }
	Out& flush()
	{
		if (UTF8Message.size() <= 0)
			UTF8Message = " ";
		PrintMessage(UTF8Message, Color);
		UTF8Message.clear();
		return *this;
	}

private:
	string UTF8Message;
	URGB Color;
	bool IsFlush;
};

enum class TypeColor
{
	TYPE_NORMAL,
	TYPE_WARRNING,
	TYPE_ERROR,
	TYPE_INFO,
	TYPE_SUCC
};
Out FLUSH;

Out PRINT(TypeColor Type)
{
	switch (Type)
	{
	case TypeColor::TYPE_NORMAL:
		return Out(URGB(255, 255, 255));
	case TypeColor::TYPE_WARRNING:
		return Out(URGB(255, 220, 0));
	case TypeColor::TYPE_ERROR:
		return Out(URGB(255, 0, 0));
	case TypeColor::TYPE_INFO:
		return Out(URGB(0, 180, 255));
	case TypeColor::TYPE_SUCC:
		return Out(URGB(0, 255, 0));
	}
	return Out(URGB(255, 255, 255));
}
//\\VKAPITOOLSDIALOGINTERFACE

struct Links
{
	list<string>photos;
	list<string>walls;
};

struct Analyzer
{
	vector<int>search_ids;
	vector<int>pattern_user_id;
	vector<string> patterns;
};

static Links ResultLinksT;
static list<int>UsersScannedT;

UserInfo GetUserNameById(VK_APIMETHOD& Method, int64_t UserId) {
	bool isGroup = UserId < 0;
	VK_APIMETHOD& point = (!isGroup ? Method["users.get"] : Method["groups.getById"]);
	if (UserId != 0) {
		if (!isGroup) {
			point << VKI("user_ids", UserId);
			point << VK("fields", "photo_200_orig,contacts,connections,site");
		}
		else {
			point << VKI("group_ids", abs(UserId));
			point << VK("fields", "photo_200_orig");
		}
	}
	else {
		point << VK("fields", "photo_200_orig,contacts,connections,site");
	}
	VKAPI_ANSWER Answer = point();
	if (Answer.IsError == true || Answer.Object.find("response") == Answer.Object.end()) {
		return UserInfo(UserId, wstring(L"id") + to_wstring(UserId), AVATAR_USER_DEFAULT, "", "", "", false);
	}
	try {
		json info = Answer.Object.at("response").get<json>();
		if (isGroup && info.find("groups") == info.end()) {
			return UserInfo(UserId, wstring(L"id") + to_wstring(UserId), AVATAR_USER_DEFAULT, "", "", "", false);
		}
		info = (!isGroup ? info.begin().value() : info.at("groups").begin().value());
		string AvatarLink = AVATAR_USER_DEFAULT;
		if (info.find("photo_200_orig") != info.end()) {
			if (!info.at("photo_200_orig").is_string()) {
				AvatarLink = AVATAR_USER_DEFAULT;
			}
			else {
				AvatarLink = info.at("photo_200_orig").get<string>();
			}
		}
		if (!isGroup) {
			string phone_number;
			string instagram;
			string site;
			int64_t user_id = 0;
			if (info.find("mobile_phone") != info.end() && info.at("mobile_phone").is_string()) {
				phone_number = FixFileName(info.at("mobile_phone").get<string>());
			}
			if (info.find("instagram") != info.end() && info.at("instagram").is_string()) {
				instagram = FixFileName(info.at("instagram").get<string>());
			}
			if (info.find("site") != info.end() && info.at("site").is_string()) {
				site = FixFileName(info.at("site").get<string>());
			}
			if (info.find("id") != info.end()) {
				user_id = info.at("id").get<int64_t>();
			}
			return UserInfo(user_id, FixFileName(UTF8_to_wchar(info.at("last_name").get<string>()) + L" " + UTF8_to_wchar(info.at("first_name").get<string>())), AvatarLink, phone_number, instagram, site, true);
		}
		else {
			return UserInfo(UserId, FixFileName(UTF8_to_wchar(info.at("name").get<string>())), AvatarLink, "", "", "", true);
		}
	}
	catch (json::exception e) {
		e.what();
	}
	return UserInfo(UserId, wstring(L"id") + to_wstring(UserId), AVATAR_USER_DEFAULT, "", "", "", false);
}

UserInfo CallToGetUserNameById(const string &Token, const string &UserAgent, int64_t UserId)
{
	auto v = VK_APIMETHOD(Token, UserAgent);
	return GetUserNameById(v, UserId);
}

bool VK_APIMETHOD::VK_APIMETHOD_doCapcha(const std::string& captcha_img, const std::string& user_agent, std::string& code) {
	VKApiToolsDialogCapcha capcha(UTF8_to_wchar(captcha_img).c_str(), UTF8_to_wchar(user_agent).c_str());
	if (capcha.DoModal() == TRUE) {
		code = wchar_to_UTF8(capcha.CapchaCode.GetString());
		return true;
	}
	return false;
}

void VK_APIMETHOD::VK_APIMETHOD_doLimit(const std::string& Method) {
	PrintMessage("    #VK_APIMETHOD[error] " + wchar_to_UTF8(WSTRUtils::GetTimeLocal()) + " " + Method + (const char*)u8" Ошибка 29: лимит на использование", URGB(255, 0, 0));
}

bool VK_APIMETHOD_doCapcha(const std::string& captcha_img, const std::string& user_agent, std::string& code) {
	VKApiToolsDialogCapcha capcha(UTF8_to_wchar(captcha_img).c_str(), UTF8_to_wchar(user_agent).c_str());
	if (capcha.DoModal() == TRUE) {
		code = wchar_to_UTF8(capcha.CapchaCode.GetString());
		return true;
	}
	return false;
}

void VK_APIMETHOD_doLimit(const std::string& Method) {
	PrintMessage("    #VK_APIMETHOD[error] " + wchar_to_UTF8(WSTRUtils::GetTimeLocal()) + " " + Method + (const char*)u8" Ошибка 29: лимит на использование", URGB(255, 0, 0));
}

class VK_APIMETHODCUSTOM
{
public:
	static int VKHasError(const nlohmann::json& jres)
	{
		if (jres.size() == 0 || jres.is_null() == true)
			return -1;
		if (jres.find("execute_errors") != jres.end())
			return -1;
		if (jres.find("error") == jres.end())
			return 0;
		nlohmann::json uot = jres.at("error").get<nlohmann::json>();
		if (uot.find("error_code") == uot.end())
			return -1;
		return uot.at("error_code").get<int>();
	}
	VK_APIMETHODCUSTOM(const string &Access_Token, const string &IUserAgent, const string &IRequest)
	{
		Token = Access_Token;
		UserAgent = IUserAgent;
		if (!IRequest.empty() && IRequest[0] == '#') {
			Request = "https://oauth.vk.ru/" + IRequest.substr(1);
		}
		else {
			Request = "https://api.vk.ru/method/" + IRequest;
		}
	}

	VKAPI_ANSWER operator()()
	{
		int errorcode = 0;
		VKAPI_ANSWER ret;
		ret.IsError = true;

		if (Request.length() <= 0)
			return ret;
		if(Request.find("?") == string::npos)
			Request += "?access_token=" + Token;
		else
			Request += "&access_token=" + Token;

		if (Request.find("v=") == string::npos)
			Request += string("&v=") + VKAPI_VERSION;

		if (Request.find("lang=") == string::npos)
			Request += string("&lang=ru");
		std::string RequestProp = Request;
		do
		{
			if (errorcode == 6 || errorcode == 1 || errorcode == 9 || errorcode == 10)
				Sleep(1100);

			string Rsj;
			if (DoCurlGet(RequestProp, UserAgent, Rsj, true) <= 0)
				return ret;

			try {
				ret.Object = json::parse(Rsj);
			}
			catch (...) {
				return ret;
			}
			errorcode = VKHasError(ret.Object);
			if (errorcode == 0)
				ret.IsError = false;
			else if (errorcode == 14)
			{
				if (RequestProp.find("captcha_sid=") != std::string::npos)
					RequestProp = Request;
				nlohmann::json info = ret.Object.at("error").get<nlohmann::json>();

				std::string code_capcha;
				if (VK_APIMETHOD_doCapcha(info.at("captcha_img").get<std::string>(), UserAgent, code_capcha)) {
					RequestProp += ("&captcha_sid=" + WSTRUtils::url_encode(info.at("captcha_sid").get<std::string>()) + "&captcha_key=" + WSTRUtils::url_encode(code_capcha));
				}
			}
			else if (errorcode == 29) {
				VK_APIMETHOD_doLimit(Request);
			}
		} while (errorcode == 6 || errorcode == 14 || errorcode == 1 || errorcode == 9 || errorcode == 10);
		return ret;
	}
private:
	string Request;
	string Token;
	string UserAgent;
};

static std::vector<std::string> split(const string& input, const string& regex) {
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

inline std::string trim(const std::string& s)
{
	auto  wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
	return std::string(wsfront, std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {return std::isspace(c); }).base());
}

void VKAPI_TOOLS_Request(const string &Token, const string &APIMethod, const string &Params, const UsesOptionInMethodResult &Options)
{
	VK_APIMETHOD ich(Token, ANDROID_USERAGENT);
	auto myuser_id = GetUserNameById(ich, 0).user_id;

	string Tparams;
	try {
		string prm = Params;
		vector<string> params = split(trim(prm), "\\r?\\n");
		bool isFirst = true;
		for (auto& i : params) {
			if (i.size() <= 0) {
				PRINT(TypeColor::TYPE_WARRNING) << (const char*)u8"Нет параметров" << FLUSH;
				Tparams = "";
				break;
			}
			vector<string> res = split(i, "=>");
			if (res.size() < 2) {
				PRINT(TypeColor::TYPE_ERROR) << (const char*)u8"Требуется вводить ключ=значение с новой строки" << FLUSH;
				Tparams = "";
				return;
			}

			string name = res[0];
			string value = res[1];

			if (name.size() <= 0 || value.size() <= 0) {
				PRINT(TypeColor::TYPE_ERROR) << (const char*)u8"Требуется вводить ключ=значение с новой строки" << FLUSH;
				Tparams = "";
				return;
			}

			name = trim(name);
			value = trim(value);
			if (value == string((char*)u8"Я") || value == string((char*)u8"я")) {
				value = to_string(myuser_id);
			}

			if (isFirst) {
				isFirst = false;
				Tparams = "?";
			}
			else {
				Tparams += "&";
			}
			Tparams += url_encode(name);
			Tparams += "=";
			Tparams += url_encode(value);
		}
	}
	catch (std::exception e) {
		PRINT(TypeColor::TYPE_ERROR) << (const char*)u8"Исключение при парсинге параметров: " << e.what() << FLUSH;
	}


	VK_APIMETHODCUSTOM t(Token, ANDROID_USERAGENT, APIMethod + Tparams);
	PRINT(TypeColor::TYPE_NORMAL) << t().Object.dump(4) << FLUSH;
}