#include "pch.h"
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <Windows.h>
#include <Shlwapi.h>
#include <regex>
#include "do_curl.h"
#include "vk_api_interface.h"
#include "VKApiToolsDialogCapcha.h"
#include "Map.h"
#include "urgb.h"
#include "FlBin.h"
#include "sha3.hpp"
#include "json.hpp"
#include "WSTRUtils.h"
#include "UsrInf.h"
#include "WinMessageBox.h"
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
	Out& operator<<(const char* msg) { UTF8Message += msg; return *this; }
	Out& operator<<(const string &msg) { UTF8Message += msg; return *this; }
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

UserInfo GetOwnerByShortName(VK_APIMETHOD& Method, const string& Link) {
	string Linkt = Link;
	bool isNumeric = true;
	for (auto i = Linkt.begin(); i != Linkt.end(); i++) {
		if (*i == '-') {
			if (i != Linkt.begin() || Linkt.size() <= 1) {
				isNumeric = false;
				break;
			}
		}
		else if (*i < '0' || *i > '9') {
			isNumeric = false;
			break;
		}
	}
	if (isNumeric) {
		return GetUserNameById(Method, stoll(Link));
	}
	size_t Offset = Linkt.find_last_of(u8'/');
	if (Offset != string::npos)
		Linkt = Linkt.substr(Offset + 1);
	VK_APIMETHOD& point = Method["utils.resolveScreenName"];
	point << VK("screen_name", Linkt);
	VKAPI_ANSWER Answer = point();
	if (Answer.IsError == true || Answer.Object.find("response") == Answer.Object.end()) {
		return UserInfo(0, UTF8_to_wchar(Linkt), AVATAR_USER_DEFAULT, "", "", "", false);
	}
	try {
		json info = Answer.Object.at("response").get<json>();
		if (info.find("type") != info.end() && info.find("object_id") != info.end())
		{
			string type = info.at("type").get<string>();
			if (type == "user") {
				return GetUserNameById(Method, info.at("object_id").get<int64_t>());
			}
			else if (type == "group" || type == "event" || type == "page") {
				return GetUserNameById(Method, info.at("object_id").get<int64_t>() * -1);
			}
		}
	}
	catch (json::exception e) {
		e.what();
	}
	return UserInfo(0, UTF8_to_wchar(Linkt), AVATAR_USER_DEFAULT, "", "", "", false);
}

UserInfo CallToGetUserNameById(const string& Token, const string& UserAgent, int64_t UserId)
{
	auto v = VK_APIMETHOD(Token, UserAgent);
	return GetUserNameById(v, UserId);
}

void VKAPI_TOOLS_CloneMusic(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD AkkountOld(OldToken, KATE_USERAGENT);
	VK_APIMETHOD AkkountNew(Token, KATE_USERAGENT);

	VKAPI_ANSWER Answer = AkkountOld["audio.get"]();
	if (Answer.IsError == true)
	{
		PRINT(TypeColor::TYPE_ERROR) << Answer.Object << FLUSH;
		return;
	}

	json info = Answer.Object.at("response").get<json>();
	if (info.at("count").get<int>() == 0)
		return;
	info = info.at("items").get<json>();
	struct AudioObj
	{
		AudioObj(int taudio_id, int64_t towner_id, string taccess_key)
		{
			audio_id = taudio_id;
			owner_id = towner_id;
			access_key = taccess_key;
		}
		int audio_id;
		int64_t owner_id;
		string access_key;
	};
	vector<AudioObj> Audio;
	for (json::iterator audioi = info.begin(); audioi != info.end(); ++audioi)
		Audio.push_back(AudioObj(audioi.value().at("id").get<int>(), audioi.value().at("owner_id").get<int64_t>(), (audioi.value().find("access_key") != audioi.value().end() ? audioi.value().at("access_key").get<string>() : "")));
	vector<AudioObj> AudioResult;
	for (auto i = Audio.end() - 1; i >= Audio.begin(); i--)
		AudioResult.push_back(*i);

	for (auto&i : AudioResult)
	{
		auto tt = AkkountNew["audio.add"];
		tt << VKI("audio_id", i.audio_id) << VKI("owner_id", i.owner_id);
		if (i.access_key.length() > 0)
			tt << VK("access_key", i.access_key);
		tt();
	}
}

struct Repost
{
	Repost(int64_t towner_id, int tid, int It)
	{
		OwnerId = towner_id;
		Id = tid;
		Object = string("wall") + to_string(towner_id) + "_" + to_string(tid);
		PRINT(TypeColor::TYPE_NORMAL) << It << L") Найден репост с музыкой: https://vk.ru/" << Object << FLUSH;
	}
	Repost()
	{
		OwnerId = 0;
		Id = 0;
	}
	bool operator==(const Repost &v) const
	{
		return Object == v.Object;
	}
	bool operator!=(const Repost& v) const
	{
		return Object != v.Object;
	}
	int64_t OwnerId;
	int Id;
	string Object;
};

void PrepareRepostMusic(list<Repost> &RepostsResult, const string &Access_Token)
{
	PRINT(TypeColor::TYPE_INFO) << L"[Анализ репостов с музыкой] " << Access_Token << FLUSH;
	VK_APIMETHOD Akkount(Access_Token, KATE_USERAGENT);

	int MaxDialogs = 0;
	int index = 0;
	int iterrr = 0;
	vector<Repost> Reposts;
	do
	{
		Akkount["wall.get"] << VKI("count", 100) << VKI("offset", index) << VK("filter", "owner");
		VKAPI_ANSWER DWN = Akkount();
		if (DWN.IsError == true)
		{
			PRINT(TypeColor::TYPE_ERROR) << DWN.Object << FLUSH;
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
			if(ite.find("attachments") == ite.end())
				continue;
			bool HasMusic = false;
			json attach = ite.at("attachments").get<json>();
			for (json::iterator itatt = attach.begin(); itatt != attach.end(); ++itatt)
			{
				json att = itatt.value();
				if (att.at("type").get<string>() == "audio")
				{
					HasMusic = true;
					break;
				}
			}
			if (HasMusic == true)
				Reposts.push_back(Repost(ite.at("owner_id").get<int64_t>(), ite.at("id").get<int>(), ++iterrr));
		}
		index += (int)items.size();
	} while (index < MaxDialogs);
	if (Reposts.size() > 0)
	{
		for (auto i = Reposts.end() - 1; i >= Reposts.begin(); i--)
			RepostsResult.push_back(*i);
	}
}

void CloneRepostMusic(const list<Repost> &RepostsResult, const string &Access_Token)
{
	PRINT(TypeColor::TYPE_INFO) << L"[Подготовка к клонированию " << RepostsResult.size() << L" Репостов...]" << FLUSH;
	VK_APIMETHOD Akkount(Access_Token, KATE_USERAGENT);
	int iterr = 0;
	for (auto&i : RepostsResult)
	{
		auto work = Akkount["wall.repost"];
		work << VK("object", i.Object);
		VKAPI_ANSWER answ = work();
		PRINT(TypeColor::TYPE_NORMAL) << ++iterr << ")" << answ.Request << FLUSH << FLUSH;
		bool isError = true;
		if (answ.Object.find("response") != answ.Object.end())
		{
			json info = answ.Object.at("response").get<json>();
			if (info.find("success") != info.end())
			{
				if (info.at("success").get<int>() == 1)
					isError = false;
			}
		}
		if (!isError)
			PRINT(TypeColor::TYPE_SUCC) << L"[Успешно!]" << FLUSH;
		else
		{
			PRINT(TypeColor::TYPE_ERROR) << answ.Object << FLUSH;
			if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Произошла ошибка, возможно вы превысили количество репостов. Выйти?").show() == IDYES)
				return;
		}
	}
}

list<Repost> MergeRepostMusic(const list<Repost>& FromReposts, const list<Repost>& ToReposts)
{
	int count = 0;
	PRINT(TypeColor::TYPE_INFO) << L"[Подготовка к обработке Репостов...]" << FLUSH;
	list<Repost> ResultReposts = FromReposts;
	for (auto& i : ToReposts)
	{
		for (auto s = ResultReposts.begin(); s != ResultReposts.end(); s++)
		{
			if ((*s) == i)
			{
				ResultReposts.erase(s);
				s--;
				count++;
			}
		}
	}
	PRINT(TypeColor::TYPE_INFO) << L"Удалено " << count << FLUSH;
	return ResultReposts;
}

void VKAPI_TOOLS_Clone_Reposts(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	list<Repost> FromReposts;
	list<Repost> ToReposts;

	PrepareRepostMusic(FromReposts, OldToken);
	PrepareRepostMusic(ToReposts, Token);

	CloneRepostMusic(MergeRepostMusic(FromReposts, ToReposts), Token);
}

void VKAPI_TOOLS_RemoveAllWall(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	list<Repost> Reposts;
	int offset = 0;
	while (true)
	{
		Akkount["wall.get"] << VK("filter", "all") << VKI("count", 100) << VKI("offset", offset);
		const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError)
			return;
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		if (items.size() <= 0)
			break;
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value();
			Reposts.push_back(Repost(ite.at("owner_id").get<int64_t>(), ite.at("id").get<int>(), ++offset));
		}
	}
	int del = 0;
	for (auto& i : Reposts)
	{
		PRINT(TypeColor::TYPE_NORMAL) << L"Удалён " << ++del << "/" << Reposts.size() << FLUSH;
		Akkount["wall.delete"] << VKI("owner_id", i.OwnerId) << VKI("post_id", i.Id);
		PRINT(TypeColor::TYPE_SUCC) << Akkount().Object << FLUSH;
	}
}

void VKAPI_TOOLS_GetGroupMembers(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	list<int64_t> Ids;
	int offset = 0;
	string ol = OldToken;
	UserInfo v = GetOwnerByShortName(Akkount, ol);
	if (v.user_id == 0) {
		PRINT(TypeColor::TYPE_ERROR) << L"Ошибка ссылки" << FLUSH;
		return;
	}
	while (true)
	{
		Akkount["groups.getMembers"] << VKI("group_id", abs(v.user_id)) << VKI("count", 1000) << VKI("offset", offset);
		offset += 1000;
		const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError) {
			PRINT(TypeColor::TYPE_ERROR) << answ.Object << FLUSH;
			return;
		}
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		if (items.size() <= 0)
			break;
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value();
			Ids.push_back(ite.get<int64_t>());
		}
	}
	FlBin w;
	w.wopen(v.pUserName + L".members.txt", OPENType::OPENWrite);
	for (auto& i : Ids){
		w << to_string(i) << "\r\n";
	}
	w.Close();
}

void VKAPI_TOOLS_GetConversations(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	list<int64_t> Ids;
	int offset = 0;
	string ol = OldToken;
	UserInfo v = GetOwnerByShortName(Akkount, ol);
	while (true)
	{
		Akkount["messages.getConversations"] << VKI("count", 200) << VKI("offset", offset);
		if (v.user_id != 0)
			Akkount << VKI("group_id", abs(v.user_id));
		offset += 200;
		const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError) {
			PRINT(TypeColor::TYPE_ERROR) << answ.Object << FLUSH;
			return;
		}
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		if (items.size() <= 0)
			break;
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value().at("conversation").get<json>();
			json item = ite.at("peer").get<json>();
			Ids.push_back(item.at("id").get<int64_t>());
		}
	}
	FlBin w;
	w.wopen(v.pUserName + L".conversations.txt", OPENType::OPENWrite);
	for (auto& i : Ids) {
		w << to_string(i) << "\r\n";
	}
	w.Close();
}

void VKAPI_TOOLS_RemoveAllMusic(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	list<Repost> Reposts;
	
	Akkount["audio.get"];
	const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError)
			return;
		int offset = 0;
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value();
			Reposts.push_back(Repost(ite.at("owner_id").get<int64_t>(), ite.at("id").get<int>(), ++offset));
		}
	
	int del = 0;
	for (auto& i : Reposts)
	{
		PRINT(TypeColor::TYPE_NORMAL) << L"Удалён " << ++del << "/" << Reposts.size() << FLUSH;
		Akkount["audio.delete"] << VKI("owner_id", i.OwnerId) << VKI("audio_id", i.Id);
		PRINT(TypeColor::TYPE_SUCC) << Akkount().Object << FLUSH;
	}
}

void VKAPI_TOOLS_RemoveAllConv(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	list<int64_t> Covs;
	int offset = 21;
	while (true)
	{
		Akkount["messages.getConversations"] << VKI("count", 200) << VKI("offset", offset);
		const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError)
			return;
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		if (items.size() <= 0)
			break;
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value();
			int64_t id = ite.at("conversation").get<json>().at("peer").get<json>().at("id").get<int64_t>();
			Covs.push_back(id);
			PRINT(TypeColor::TYPE_NORMAL) << "Push " << id << FLUSH;
			offset++;
		}
	}
	int del = 0;
	for (auto& i : Covs)
	{
		PRINT(TypeColor::TYPE_NORMAL) << L"Удалён " << ++del << "/" << Covs.size() << FLUSH;
		Akkount["messages.deleteConversation"] << VKI("peer_id", i);
		PRINT(TypeColor::TYPE_SUCC) << Akkount().Object << FLUSH;
	}
}


void VKAPI_TOOLS_RemoveAllGroup(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	list<int64_t> Groups;
	int offset = 0;
	while (true)
	{
		Akkount["groups.get"] << VKI("count", 1000) << VKI("offset", offset);
		const VKAPI_ANSWER& answ = Akkount();
		if (answ.IsError)
			return;
		json items = answ.Object.at("response").get<json>().at("items").get<json>();
		if (items.size() <= 0)
			break;
		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json ite = it.value();
			int64_t id = ite.get<int64_t>();
			Groups.push_back(id);
			offset++;
		}
	}
	int del = 0;
	for (auto& i : Groups)
	{
		PRINT(TypeColor::TYPE_NORMAL) << L"Отписан [" << i << "] " << ++del << "/" << Groups.size() << FLUSH;
		Akkount["groups.leave"] << VKI("group_id", i);
		PRINT(TypeColor::TYPE_SUCC) << Akkount().Object << FLUSH;
	}
}

wstring transform_owner(int64_t owner_id)
{
	if (owner_id < 0)
		return L"club" + to_wstring(abs(owner_id));
	else
		return L"id" + to_wstring(owner_id);
}

void VKAPI_TOOLS_FIX_PHOTOS(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	PRINT(TypeColor::TYPE_WARRNING) << L"Использует Phoenix" << FLUSH;
	if (PathFileExistsW(L"Phoenix") == FALSE)
	{
		PRINT(TypeColor::TYPE_ERROR) << L"Ненайден Phoenix" << FLUSH;
		return;
	}
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFileW(L"Phoenix\\*.jpg", &wfd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		int rgx = 0;
		do
		{
			std::wregex e(L"(-?[0-9]+)_([0-9]+)", std::regex_constants::ECMAScript);
			std::wcmatch mch;
			if (std::regex_search(wfd.cFileName, mch, e))
			{
				int64_t owner_id = stoll(mch[1]);
				wstring Path = GetUserNameById(Akkount, owner_id).pUserName;
				wstring result = L"Phoenix\\" + Path + L"\\" + Path + L"_" + transform_owner(owner_id) + L"_" + mch[2].str() + L".jpg";
				if (PathFileExistsW((L"Phoenix\\" + Path).c_str()) == FALSE)
					CreateDirectoryW((L"Phoenix\\" + Path).c_str(), NULL);
				MoveFileW((L"Phoenix\\" + wstring(wfd.cFileName)).c_str(), result.c_str());
				PRINT(TypeColor::TYPE_SUCC) << ++rgx << ") " << result << FLUSH;
			}
		} while (NULL != FindNextFileW(hFind, &wfd));

		FindClose(hFind);
	}
}

void VKAPI_TOOLS_FIX_AUDIOS(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	PRINT(TypeColor::TYPE_WARRNING) << L"Использует Music" << FLUSH;
	PRINT(TypeColor::TYPE_WARRNING) << L"Использует audio_deprecated.json" << FLUSH;
	if (PathFileExistsW(L"Music") == FALSE)
	{
		PRINT(TypeColor::TYPE_ERROR) << L"Ненайден Music" << FLUSH;
		return;
	}
	if (PathFileExistsW(L"audio_deprecated.json") == FALSE)
	{
		PRINT(TypeColor::TYPE_ERROR) << L"Ненайден audio_deprecated.json" << FLUSH;
		return;
	}
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFileW(L"Music\\*.mp3", &wfd);
	Map::Map<string, wstring> MainDir;
	Map::Map<string, wstring> Deprs;

	FILE* fl = NULL;
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
				Deprs[audioj.at("sha3_512").get<string>()] = FixFileNameAudio(UTF8_to_wchar(audioj.at("artist").get<string>()) + L" - " + UTF8_to_wchar(audioj.at("title").get<string>())) + L".mp3";
			}
		}
		catch (...) {
		}
	}
	if (INVALID_HANDLE_VALUE != hFind)
	{
		int rgx = 0;
		do
		{
			string hsh = SHA3_512::sha3_file(wstring(L"Music\\") + wfd.cFileName);
			if (MainDir.exist(hsh))
			{
				DeleteFileW((L"Music\\" + MainDir.at(hsh)).c_str());
				PRINT(TypeColor::TYPE_ERROR) << ++rgx << ") DUPLICATE " << wfd.cFileName << " | " << hsh << FLUSH;
				continue;
			}
			MainDir[hsh] = wfd.cFileName;
			PRINT(TypeColor::TYPE_SUCC) << ++rgx << ") LOADING HASH " << wfd.cFileName << " | " << hsh << FLUSH;
		} while (NULL != FindNextFileW(hFind, &wfd));
		FindClose(hFind);

		for (auto& i : MainDir)
		{
			if (Deprs.exist(i.get_key()))
				MoveFileW((L"Music\\" + i.get_value()).c_str(), (L"Music\\" + Deprs.at(i.get_key())).c_str());
		}
	}
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

void VKAPI_TOOLS_UploadAudio(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options) {
	VK_APIMETHOD Akkount(Token, ANDROID_USERAGENT);
	VKAPI_ANSWER Answer = Akkount["audio.getUploadServer"]();
	if (Answer.IsError == true || Answer.Object.find("response") == Answer.Object.end() || Answer.Object.at("response").find("upload_url") == Answer.Object.at("response").end())
	{
		PRINT(TypeColor::TYPE_ERROR) << Answer.Object << FLUSH;
		return;
	}
	string url = Answer.Object.at("response").at("upload_url").get<string>();
	string rt;
	if (DoCurlMultipart(url, UTF8_to_wchar(OldToken), "file", "audio.mp3", ANDROID_USERAGENT, rt, true) <= 0) {
		PRINT(TypeColor::TYPE_ERROR) << rt << FLUSH;
		return;
	}
	try {
		auto v = json::parse(rt);
		VKAPI_ANSWER P = (Akkount["audio.save"] << VK("audio", v.at("audio").get<string>()) << VK("hash", v.at("hash").get<string>()) << VKI("server", v.at("server").get<int>()))();
		if (P.IsError) {
			PRINT(TypeColor::TYPE_ERROR) << Answer.Object << FLUSH;
			return;
		}
		PRINT(TypeColor::TYPE_SUCC) << Answer.Object << FLUSH;
	}
	catch (json::exception e) {
		PRINT(TypeColor::TYPE_ERROR) << e.what() << FLUSH;
		return;
	}
}

void VKAPI_TOOLS_DUMP_AUDIOS(const string& Token, const string& OldToken, const UsesOptionInMethodResult& Options)
{
	VK_APIMETHOD Akkount(Token, KATE_USERAGENT);
	PRINT(TypeColor::TYPE_WARRNING) << L"Использует Music" << FLUSH;
	PRINT(TypeColor::TYPE_WARRNING) << L"Использует audio_downloads.json" << FLUSH;
	if (PathFileExistsW(L"Music") == FALSE)
	{
		PRINT(TypeColor::TYPE_ERROR) << L"Ненайден Music" << FLUSH;
		return;
	}
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFileW(L"Music\\*.mp3", &wfd);
	set<string> MainDir;

	FILE* fl = NULL;
	if (_wfopen_s(&fl, L"audio_downloads.json", L"rb") == 0)
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
			for (json::iterator itatt = deprecatedaudios.begin(); itatt != deprecatedaudios.end(); ++itatt)
			{
				json audioj = itatt.value();
				MainDir.insert(audioj.get<string>());
			}
		}
		catch (...) {
		}
	}
	if (INVALID_HANDLE_VALUE != hFind)
	{
		int rgx = 0;
		do
		{
			MainDir.insert(wchar_to_UTF8(wfd.cFileName));
		} while (NULL != FindNextFileW(hFind, &wfd));
		FindClose(hFind);

		if (_wfopen_s(&fl, L"audio_downloads.json", L"wb") == 0)
		{
			if (fl) {
				fwrite(UTF8START, 1, strlen(UTF8START), fl);
				json JsonAudio = json(json::value_t::array);
				for (auto& i : MainDir)
					JsonAudio.push_back(i);
				string jsonout = JsonAudio.dump(4);
				fwrite(jsonout.c_str(), 1, jsonout.length(), fl);
				fclose(fl);
			}
		}
	}
}