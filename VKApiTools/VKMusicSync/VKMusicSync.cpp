#include "pch.h"
#include <iostream>
#include <list>
#include <Windows.h>
#include <Shlwapi.h>
#include <regex>
#include "do_curl.h"
#include "vk_api_interface.h"
#include "CTar.h"
#include "Map.h"
#include "urgb.h"
#include "md5.hpp"
#include "sha3.hpp"
#include "json.hpp"
#include "pugixml/pugixml.hpp"
#include "WSTRUtils.h"
#include "UsrInf.h"
#include "WinMessageBox.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;
using namespace CTar;

extern string DEFAULT_USER_AGENT;

#define AVATAR_USER_DEFAULT "https://vk.com/images/camera_200.png?ava=1"
#define AVATAR_CHAT_DEFAULT "https://vk.com/images/icons/im_multichat_200.png"

#define UTF8START "\xef\xbb\xbf"
#define to ,

extern string GetCapchaCode(const string &Link, const string& UserAgent);

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
	Out& operator<<(const string &msg) { UTF8Message += msg; return *this; }
	Out& operator<<(const wchar_t* msg) { UTF8Message += wchar_to_UTF8(msg); return *this; }
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
	TYPE_SUCC,
	TYPE_COVER
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
	case TypeColor::TYPE_COVER:
		return Out(URGB(180, 80, 255));

	}
	return Out(URGB(255, 255, 255));
}
//\\VKAPITOOLSDIALOGINTERFACE

void VK_APIMETHOD_doLimit(const std::string& Method) {
	PrintMessage("    #VK_APIMETHOD[error] " + wchar_to_UTF8(WSTRUtils::GetTimeLocal()) + " " + Method + (const char*)u8" Ошибка 29: лимит на использование", URGB(255, 0, 0));
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

struct AudioInfo
{
	AudioInfo(const string& tartist, const string& ttitle, const string& turl, const string& tdir, int64_t tdate, bool tDuplicated, int64_t towner_id, const string& tsha3_512, const string& talbum, const string& tcover_url)
	{
		artist = FixFileNameAudio(tartist);
		title = FixFileNameAudio(ttitle);
		owner_id = towner_id;
		url = turl;
		dir = tdir;
		date = tdate;
		IsError = false;
		Duplicated = tDuplicated;
		sha3_512 = tsha3_512;
		album = FixFileNameAudio(talbum);
		cover_url = tcover_url;
	}
	AudioInfo()
	{
		date = 0;
		owner_id = 0;
		IsError = false;
		Duplicated = false;
	}
	string artist;
	string title;
	string album;
	string cover_url;
	string url;
	string dir;
	string sha3_512;
	int64_t owner_id;
	int64_t date;
	bool IsError;
	bool Duplicated;
};

void PrepareWallAudio(Map::Map<int, AudioInfo>& URL, VK_APIMETHOD_SECURE& Akkount)
{
	int MaxMusics = 0;
	int index = 0;
	int iterrr = 0;
	do
	{
		Akkount["wall.get"] << VKI("count", 100) << VKI("offset", index) << VK("filter", "owner");
		VKAPI_ANSWER DWN = Akkount();
		if (DWN.IsError == true)
			break;
		json info = DWN.Object.at("response").get<json>();
		if (MaxMusics == 0)
			MaxMusics = info.at("count").get<int>();
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
					else
						PRINT(TypeColor::TYPE_ERROR) << L"Аудио без ссылки " << att.at("artist").get<string>() << " - " << att.at("title").get<string>() << FLUSH;
				}
			}
		}
		index += (int)items.size();
	} while (index < MaxMusics);
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

void VKAPI_TOOLS_DownloadMusicAll(const string &Token, const string &OldToken, const UsesOptionInMethodResult &Options)
{
	int64_t owner_id = 0;
	if (OldToken.length() > 0) {
		owner_id = stoll(OldToken);
	}
	Map::Map<int, AudioInfo> DeprecatedURL;
	list<string>Hashs;
	FILE* fl = NULL;
	PRINT(TypeColor::TYPE_WARRNING) << L"Использует audio_deprecated.json" << FLUSH;
	if (_wfopen_s(&fl, L"audio_deprecated.json", L"rb") == 0)
	{
		PRINT(TypeColor::TYPE_INFO) << L"Анализ предыдущего скачивания..." << FLUSH;
		fseek(fl, 0, SEEK_END);
		int fsz = ftell(fl) - (int)strlen(UTF8START);
		fseek(fl, (int)strlen(UTF8START), SEEK_SET);
		string jsonna;
		jsonna.resize(fsz + 1);
		fread((char*)jsonna.data(), 1, fsz, fl);
		fclose(fl);
		json deprecatedaudios;
		try {
			deprecatedaudios = json::parse(jsonna);
			deprecatedaudios = deprecatedaudios.at("deprecated_audio").get<json>();
			for (json::iterator itatt = deprecatedaudios.begin(); itatt != deprecatedaudios.end(); ++itatt)
			{
				json audioj = itatt.value();
				DeprecatedURL[audioj.at("id").get<int>()] = AudioInfo(audioj.at("artist").get<string>(), audioj.at("title").get<string>(), audioj.at("url").get<string>(), audioj.at("dir").get<string>(), audioj.at("date").get<int64_t>(), audioj.at("duplicated").get<int>() == 1, audioj.at("owner_id").get<int64_t>(), audioj.at("sha3_512").get<string>(), audioj.at("album").get<string>(), audioj.at("cover_url").get<string>());
				Hashs.push_back(audioj.at("sha3_512").get<string>());
			}
		}
		catch (...) {

		}
	}
	Map::Map<int, AudioInfo> URL;
	VK_APIMETHOD_SECURE Akkount(Token, DEFAULT_USER_AGENT);
	wstring UserName = GetUserNameById(Akkount, owner_id).pUserName;
	PRINT(TypeColor::TYPE_INFO) << L"[Подготовка Аудио] пользователя " << UserName << FLUSH;
	if (Options.Option1)
		PrepareWallAudio(URL, Akkount);

	int MaxMusics = 0;
	int index = 0;
	int iterrr = 0;

	do
	{
		auto& sd = Akkount["audio.get"];
		sd << VKI("count", 1000) << VKI("offset", index);
		if (owner_id != 0) {
			sd << VKI("owner_id", owner_id);
		}
		const VKAPI_ANSWER& answ = sd();
		if (answ.IsError)
			return;
		int offset = 0;
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		if (MaxMusics == 0)
			MaxMusics = answ.Object.at("response").at("count").get<int>();
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
			else
				PRINT(TypeColor::TYPE_ERROR) << L"Аудио без ссылки " << att.at("artist").get<string>() << " - " << att.at("title").get<string>() << FLUSH;
		}
		index += (int)items.size();
	} while (index < MaxMusics);

	if (DeprecatedURL.size() > 0)
		PRINT(TypeColor::TYPE_INFO) << L"[Синхронизация с передыдущими результатами скачивания] " << URL.size() << "/" << DeprecatedURL.size() << FLUSH;
	for (auto& i : DeprecatedURL)
	{
		if (URL.exist(i.get_key()))
			URL.erase(i.get_key());
	}
	PRINT(TypeColor::TYPE_SUCC) << L"Подготовка к скачиванию " << URL.size() << L" треков..." << FLUSH;
	if (URL.size() <= 0)
		return;
	time_t SyncDataTime = time(0);
	TarCreater ETar(L"Аудио " + UserName + L" " + GetTimeAT(SyncDataTime) + L".tar", SyncDataTime, L"Аудио " + GetUserNameById(Akkount, owner_id).pUserName + L", Синхронизировано " + GetTimeAT(SyncDataTime), false);
	int dwnd = 0;
	for (auto& i : URL)
	{
		string MP3M;
		string CoverM;
		if (DoCurlGet(i.get_value().url, DEFAULT_USER_AGENT, MP3M, false) > 0 && MP3M.length() > 0)
		{
			string flnm = i.get_value().dir + "/" + i.get_value().artist + "/" + i.get_value().artist + " - " + i.get_value().title;
			string HSH = SHA3_512::sha3(MP3M.data(), MP3M.size());
			i.get_value().sha3_512 = HSH;
			bool Findd = false;
			for (auto& ss : Hashs)
			{
				if (ss == HSH)
				{
					Findd = true;
					break;
				}
			}
			if (!Findd)
				Hashs.push_back(HSH);
			else
			{
				++dwnd;
				i.get_value().Duplicated = true;
				PRINT(TypeColor::TYPE_WARRNING) << L"	[Найден дубликат] Файл: " << flnm << FLUSH;
				PRINT(TypeColor::TYPE_WARRNING) << L"	[Найден дубликат] Hash: " << HSH << FLUSH;
				continue;
			}
			PRINT(TypeColor::TYPE_SUCC) << L"Скачен Трек [" << ++dwnd << "/" << URL.size() << "] " << i.get_value().dir + "/" + i.get_value().artist + "/" + i.get_value().artist + " - " + i.get_value().title + ".mp3" << FLUSH;
			TarCreaterWriter AudioData = ETar.AddFileEntry(UTF8_to_wchar(flnm + ".mp3"), i.get_value().date);
			AudioData.Write(MP3M.data(), MP3M.size());
			AudioData.Finalize();

			if (Options.Option2 && i.get_value().cover_url.length() > 0)
			{
				if (DoCurlGet(i.get_value().cover_url, DEFAULT_USER_AGENT, CoverM, false) > 0 && CoverM.length() > 0)
				{
					PRINT(TypeColor::TYPE_COVER) << L"             Скачена Обложка [" << dwnd << "/" << URL.size() << "] " << i.get_value().dir + "/" + i.get_value().artist + "/" + i.get_value().artist + " - " + i.get_value().title + ".jpg" << FLUSH;
					TarCreaterWriter CoverData = ETar.AddFileEntry(UTF8_to_wchar(flnm + ".jpg"), i.get_value().date);
					CoverData.Write(CoverM.data(), CoverM.size());
					CoverData.Finalize();
				}
				else
					PRINT(TypeColor::TYPE_WARRNING) << L"	       Ошибка Обложка [" << dwnd << "/" << URL.size() << "] " << i.get_value().dir + "/" + i.get_value().artist + "/" + i.get_value().artist + " - " + i.get_value().title + ".jpg" << FLUSH;
			}
		}
		else
		{
			i.get_value().IsError = true;
			PRINT(TypeColor::TYPE_ERROR) << L"	Ошибка Трек [" << ++dwnd << "/" << URL.size() << "] " << i.get_value().dir + "/" + i.get_value().artist + "/" + i.get_value().artist + " - " + i.get_value().title + ".mp3" << FLUSH;
		}
	}

	for (auto& i : URL)
		DeprecatedURL[i.get_key()] = i.get_value();

	TarCreaterWriter AudioData = ETar.AddFileEntry(L"audio_deprecated(downloaded).json", time(0));

	AudioData << "{ \"deprecated_audio\":[";
	bool isFirst = true;
	for (auto& i : URL)
	{
		if (i.get_value().IsError)
			continue;
		if (isFirst == true)
			isFirst = false;
		else
			AudioData << ",";
		AudioData << "{ \"id\":" << i.get_key() << ", \"owner_id\":" << i.get_value().owner_id << ", \"duplicated\":" << i.get_value().Duplicated << ", \"date\":" << i.get_value().date << ", \"artist\":\"" << i.get_value().artist << "\", \"title\":\"" << i.get_value().title << "\", \"album\":\"" << i.get_value().album << "\", \"dir\":\"" << i.get_value().dir << "\", \"sha3_512\":\"" << i.get_value().sha3_512 << "\", \"cover_url\":\"" << i.get_value().cover_url << "\", \"url\":\"" << i.get_value().url << "\" }\r\n";
	}
	AudioData << "] }";
	AudioData.Finalize();

	ETar.close();

	if (_wfopen_s(&fl, L"audio_deprecated.json", L"wb") == 0)
	{
		fwrite(UTF8START, 1, strlen(UTF8START), fl);
		stringstream jsonout;
		jsonout << "{ \"deprecated_audio\":[";
		bool isFirst = true;
		for (auto& i : DeprecatedURL)
		{
			if (i.get_value().IsError)
				continue;
			if (isFirst == true)
				isFirst = false;
			else
				jsonout << ",";
			jsonout << "{ \"id\":" << i.get_key() << ", \"owner_id\":" << i.get_value().owner_id << ", \"duplicated\":" << i.get_value().Duplicated << ", \"date\":" << i.get_value().date << ", \"artist\":\"" << i.get_value().artist << "\", \"title\":\"" << i.get_value().title << "\", \"album\":\"" << i.get_value().album << "\", \"dir\":\"" << i.get_value().dir << "\", \"sha3_512\":\"" << i.get_value().sha3_512 << "\", \"cover_url\":\"" << i.get_value().cover_url << "\", \"url\":\"" << i.get_value().url << "\" }\r\n";
		}
		jsonout << "] }";
		fwrite(jsonout.str().c_str(), 1, jsonout.str().length(), fl);
		fclose(fl);
	}
}