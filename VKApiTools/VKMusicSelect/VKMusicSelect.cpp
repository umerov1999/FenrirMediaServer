#include "pch.h"
#include <iostream>
#include <list>
#include <Windows.h>
#include <Shlwapi.h>
#include <regex>
#include "do_curl.h"
#include "vk_api_interface.h"
#include "VKApiToolsDialog.h"
#include "Map.h"
#include "md5.hpp"
#include "json.hpp"
#include "pugixml/pugixml.hpp"
#include "WSTRUtils.h"
#include "UsrInf.h"
#include "WinMessageBox.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;

extern string DEFAULT_USER_AGENT;
extern Map::Map<int, AudioInfo> URL;
extern wstring UserName;

#define AVATAR_USER_DEFAULT "https://vk.com/images/camera_200.png?ava=1"
#define AVATAR_CHAT_DEFAULT "https://vk.com/images/icons/im_multichat_200.png"

#define UTF8START "\xef\xbb\xbf"
#define to ,

extern string GetCapchaCode(const string &Link, const string& UserAgent);

////VKAPITOOLSDIALOGINTERFACE
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

void VK_APIMETHOD_doLimit(const std::string& Method) {
	(win_message().message_type(MSG_TYPE::TYPE_ERROR) << ("    #VK_APIMETHOD[error] " + wchar_to_UTF8(WSTRUtils::GetTimeLocal()) + " " + Method + (const char*)u8" Ошибка 29: лимит на использование")).timeout(5).show();
}

class VK_APIMETHOD_SECURE
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
	VK_APIMETHOD_SECURE(const string &Access_Token, const string &IUserAgent)
	{
		Token = Access_Token;
		UserAgent = IUserAgent;
		Disabled = false;
	}
	VK_APIMETHOD_SECURE(const string& Access_Token, const string& Secure, const string& IUserAgent)
	{
		Token = Access_Token + "secure" + Secure;
		UserAgent = IUserAgent;
		Disabled = false;
	}
	VK_APIMETHOD_SECURE& operator<<(const HTTPParams& ParamU)
	{
		for (auto& i : ParamU.Params)
		{
			VK_ApiMethodParam pp(i.Name, i.Param, true);
			Params.push_back(pp);
		}
		return *this;
	}
	VK_APIMETHOD_SECURE& operator<<(const VK_ApiMethodParam& ParamU)
	{
		Params.push_back(ParamU);
		return *this;
	}
	VK_APIMETHOD_SECURE& operator[](const string &LMethod)
	{
		Method = LMethod;
		Params.clear();
		return *this;
	}
	VK_APIMETHOD_SECURE& switch_token(const string& token)
	{
		Token = token;
		return *this;
	}
	VK_APIMETHOD_SECURE& switch_token(const string& token, const string& secret)
	{
		Token = token + "secure" + secret;
		return *this;
	}
	string GetExecuteMethod()
	{
		string Ret = "API." + Method + "({";
		list<VK_ApiMethodParam> TempParams = Params;
		TempParams.push_back(VK("v", VKAPI_VERSION_FOR_AUDIO));
		bool FirstParam = true;
		for (auto& i : TempParams)
		{
			if (FirstParam == true)
				FirstParam = false;
			else
				Ret += ",";
			Ret += ("\"" + i.name + "\":");
			if (i.is_string)
				Ret += ("\"" + i.value + "\"");
			else
				Ret += i.value;
		}
		Ret += "})";
		return Ret;
	}
	VKAPI_ANSWER operator()()
	{
		int errorcode = 0;
		VKAPI_ANSWER ret;
		ret.IsError = true;

		if (Disabled == true || Method.length() <= 0)
			return ret;
		list<VK_ApiMethodParam> TempParams = Params;
		string secure;
		string TempToken = Token;
		size_t SecurePos;
		if ((SecurePos = TempToken.find("secure")) != string::npos)
		{
			secure = TempToken.substr(SecurePos + 6);
			TempToken = TempToken.substr(0, SecurePos);
		}
		TempParams.push_back(VK("access_token", TempToken));
		TempParams.push_back(VK("v", VKAPI_VERSION_FOR_AUDIO));

		string ResultLink = string("https://api.vk.com/method") + "/" + Method;
		string PostParam;
		bool FirstParam = true;
		for (auto& i : TempParams)
		{
			if (FirstParam == true)
				FirstParam = false;
			else
				PostParam += "&";
			PostParam += (i.name + "=" + i.value);
		}
		string TmpUserAgent = UserAgent;
		string BackupPostParam = PostParam;
		if (SecurePos != string::npos)
		{
			//TmpUserAgent = VKAndroid_USERAGENT;
			PostParam += ("&sig=" + (MD5::md5("/method/" + Method + "?" + PostParam + secure)));
		}
		ret.Request = ResultLink;
		ret.PostParams = PostParam;
		do
		{
			if (errorcode == 6 || errorcode == 1 || errorcode == 9 || errorcode == 10)
				Sleep(1100);

			string Rsj;
			if (DoCurlPost(ResultLink, PostParam, TmpUserAgent, Rsj, true) <= 0)
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
				if (PostParam.find("captcha_sid=") != string::npos)
					PostParam = BackupPostParam;
				json info = ret.Object.at("error").get<json>();
				string cpcode = GetCapchaCode(info.at("captcha_img").get<string>(), UserAgent);
				if (cpcode.length() > 0)
					PostParam += ("&captcha_sid=" + url_encode(info.at("captcha_sid").get<string>()) + "&captcha_key=" + url_encode(cpcode));
			}
			else if (errorcode == 29) {
				VK_APIMETHOD_doLimit(Method);
			}
		} while (errorcode == 6 || errorcode == 14 || errorcode == 1 || errorcode == 9 || errorcode == 10);
		return ret;
	}
	void Disable()
	{
		Disabled = true;
	}
	bool IsDisabled() const
	{
		return Disabled;
	}
private:
	list<VK_ApiMethodParam> Params;
	string Method;
	string Token;
	string UserAgent;
	bool Disabled;
};

UserInfo GetUserNameById(VK_APIMETHOD_SECURE& Method, int64_t UserId) {
	bool isGroup = UserId < 0;
	VK_APIMETHOD_SECURE& point = (!isGroup ? Method["users.get"] : Method["groups.getById"]);
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

UserInfo CallToGetUserNameById(const string &Token, const string& UserAgent, int64_t UserId)
{
	auto v = VK_APIMETHOD_SECURE(Token, UserAgent);
	return GetUserNameById(v, UserId);
}

void PrepareWallAudio(Map::Map<int, AudioInfo>& URL, VK_APIMETHOD_SECURE& Akkount)
{
	int MaxDialogs = 0;
	int index = 0;
	int iterrr = 0;
	do
	{
		Akkount["wall.get"] << VKI("count", 100) << VKI("offset", index) << VK("filter", "owner");
		VKAPI_ANSWER DWN = Akkount();
		if (DWN.IsError == true)
		{
			(win_message() << L"Причина: " << DWN.Object.dump(4)).message_type(MSG_TYPE::TYPE_ERROR).timeout(10).show();
			break;
		}
		json info = DWN.Object.at("response").get<json>();
		if (MaxDialogs == 0)
			MaxDialogs = info.at("count").get<int>();
		json items = info.at("items").get<json>();

		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value();
			if (ite.find("copy_history") == ite.end())
				continue;
			ite = ite.at("copy_history").get<json>().begin().value();
			if (ite.find("attachments") == ite.end())
				continue;
			json attach = ite.at("attachments").get<json>();
			for (json::iterator itatt = attach.begin(); itatt != attach.end(); ++itatt)
			{
				json att = itatt.value();
				if (att.at("type").get<string>() == "audio")
				{
					att = att.at("audio").get<json>();
					string album_title;
					string cover_url;
					if (att.find("album") != att.end())
					{
						json temp = att.at("album").get<json>();
						if (temp.find("title") != temp.end())
							album_title = temp.at("title").get<string>();
						if (temp.find("thumb") != temp.end())
						{
							temp = temp.at("thumb").get<json>();
							if (temp.find("photo_600") != temp.end())
								cover_url = temp.at("photo_600").get<string>();
							else if (temp.find("photo_300") != temp.end())
								cover_url = temp.at("photo_300").get<string>();
						}
					}
					if (att.find("url") != att.end() && att.at("url").is_string() && att.at("url").get<string>().length() > 0)
						URL[att.at("id").get<int>()] = AudioInfo(att.at("artist").get<string>(), att.at("title").get<string>(), att.at("url").get<string>(), "Wall", att.at("date").get<int64_t>(), false, att.at("owner_id").get<int64_t>(), (const char*)u8"нету", album_title, cover_url);
				}
			}
		}
		index += (int)items.size();
	} while (index < MaxDialogs);
}
/*
string UpgradeToken(string Token, string secret, string device_id)
{
	try {
		VK_APIMETHOD_SECURE Akkount(Token, secret, KATE_USERAGENT);

		HTTPParams param = HTTP_PARAMS(
			HP("https" to "1"),
			HP("androidVersion" to "27"),
			HP("androidModel" to "Android 9.0 SDK 28 arm64-v8a"),
			HP("info_fields" to "audio_ads,audio_background_limit,country,discover_design_version,discover_preload,discover_preload_not_seen,gif_autoplay,https_required,inline_comments,intro,lang,menu_intro,money_clubs_p2p,money_p2p,money_p2p_params,music_intro,audio_restrictions,profiler_settings,raise_to_record_enabled,stories,masks,subscriptions,support_url,video_autoplay,video_player,vklive_app,community_comments,webview_authorization,story_replies,animated_stickers,community_stories,live_section,playlists_download,calls,security_issue,eu_user,wallet,vkui_community_create,vkui_profile_edit,vkui_community_manage,vk_apps,stories_photo_duration,stories_reposts,live_streaming,live_masks,camera_pingpong,role,video_discover"),
			HP("device_id" to device_id),
			HP("lang" to "ru"),
			HP("func_v" to "11"),
			HP("androidManufacturer" to "SAMSUNG"),
			HP("fields" to "photo_100,photo_50,exports,country,sex,status,bdate,first_name_gen,last_name_gen,verified,trending")
		);
		Akkount["execute.getUserInfo"] << param;
		(win_message() << Akkount().Object.dump(4)).show();
		Akkount["auth.refreshToken"] << HTTP_PARAMS(
			HP("https" to "1"),
			HP("timestamp" to "0"),
			HP("receipt2" to ""),
			HP("device_id" to device_id),
			HP("receipt" to ""),
			HP("lang" to "ru"),
			HP("nonce" to "")
		);
		(win_message() << Akkount().Object.dump(4)).show();
		string token = Akkount().Object.at("response").get<json>().at("token").get<string>();

		(win_message() << Akkount["audio.get"].switch_token(token, secret)().Object).show();
		return token + "secure" + secret;
	}
	catch (json::exception e) {
		return Token;
	}
}
*/

string getMp3FromM3u8(const string &url) {
	if (url.find("index.m3u8") == string::npos)
		return url;
	if (url.find("/audios/") != string::npos) {
		std::regex e("^(.+?)/[^/]+?/audios/([^/]+)/.+$", std::regex_constants::ECMAScript);
		return regex_replace(url, e, "$1/audios/$2.mp3");
	}
	else {
		std::regex e("^(.+?)/(p[0-9]+)/[^/]+?/([^/]+)/.+$", std::regex_constants::ECMAScript);
		return regex_replace(url, e, "$1/$2/$3.mp3");
	}
}

void VKAPI_TOOLS_DownloadMusicAll(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD_SECURE Akkount(Token, DEFAULT_USER_AGENT);
	UserName = GetUserNameById(Akkount, 0).pUserName;
	if (Options.Option1)
		PrepareWallAudio(URL, Akkount);

	int MaxDialogs = 0;
	int index = 0;
	int iterrr = 0;

	do
	{
		Akkount["audio.get"] << VKI("count", 1000) << VKI("offset", index);
		const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError) {
			(win_message() << L"Причина: " << answ.Object.dump(4)).message_type(MSG_TYPE::TYPE_ERROR).timeout(10).show();
			return;
		}
		int offset = 0;
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json att = it.value();
			offset++;
			if (att.find("url") != att.end() && att.at("url").is_string() && att.at("url").get<string>().length() > 0)
			{
				string album_title;
				string cover_url;
				if (att.find("album") != att.end())
				{
					json temp = att.at("album").get<json>();
					if (temp.find("title") != temp.end())
						album_title = temp.at("title").get<string>();
					if (temp.find("thumb") != temp.end())
					{
						temp = temp.at("thumb").get<json>();
						if (temp.find("photo_1200") != temp.end())
							cover_url = temp.at("photo_1200").get<string>();
						else if (temp.find("photo_600") != temp.end())
							cover_url = temp.at("photo_600").get<string>();
						else if (temp.find("photo_300") != temp.end())
							cover_url = temp.at("photo_300").get<string>();
					}
				}

				URL[att.at("id").get<int>()] = AudioInfo(att.at("artist").get<string>(), att.at("title").get<string>(), getMp3FromM3u8(att.at("url").get<string>()), "Audio", att.at("date").get<int64_t>(), false, att.at("owner_id").get<int64_t>(), (const char*)u8"нету", album_title, cover_url);
			}
		}
		index += (int)items.size();
	} while (index < MaxDialogs);
}