#include "pch.h"
#include <iostream>
#include <list>
#include <regex>
#include <Windows.h>
#include <Shlwapi.h>
#include "do_curl.h"
#include "vk_api_interface.h"
#include "Map.h"
#include "FlBin.h"
#include "HTML.h"
#include "urgb.h"
#include "json.hpp"
#include "WSTRUtils.h"
#include "UsrInf.h"
#include "WinMessageBox.h"
#include "ColorEdit.h"
#include "VKApiToolsDialogCapcha.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;

#define AVATAR_USER_DEFAULT "https://vk.com/images/camera_200.png?ava=1"
#define AVATAR_CHAT_DEFAULT "https://vk.com/images/icons/im_multichat_200.png"
#define UTF8START "\xef\xbb\xbf"
#define MAX_HTML_MESSAGES_COUNT 1000
#define MAX_EXECUTE_VK_METHODS 25
#define MAX_VIDEO_REQUEST 200
#define minu(a,b)(((a) < (b)) ? (a) : (b))
#define maxu(a,b)(((a) > (b)) ? (a) : (b))
#define ENDL "\r\n"
#define WENDL L"\r\n"

static Map::Map<int64_t, int> CachedLastMessages;
static Map::Map<int64_t, int> CurrentLastMessages;
static bool needCheckLastMessages = false;

////VKAPITOOLSDIALOGINTERFACE
extern void PrintMessage(const string &Msg, URGB Color);
extern void PrintDownload(const string& Msg, URGB Color);
extern void RemoveLastLine();
extern ColoredMessage getLastLine();
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
	Out& flush(bool isDownload = false)
	{
		if (UTF8Message.size() <= 0)
			UTF8Message = " ";
		if (!isDownload) {
			PrintMessage(UTF8Message, Color);
		}
		else {
			PrintDownload(UTF8Message, Color);
		}
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

UserInfo GetUserNameById(VK_APIMETHOD& Method, int64_t UserId)
{
	VK_APIMETHOD& point = (UserId >= 0 ? Method["users.get"] : Method["groups.getById"]);
	if (UserId != 0)
	{
		if (UserId > 0)
		{
			point << VKI("user_ids", UserId);
			point << VK("fields", "photo_200_orig,contacts,connections,site");
		}
		else
		{
			point << VKI("group_ids", abs(UserId));
			point << VK("fields", "photo_200_orig");
		}
	}
	else
		point << VK("fields", "photo_200_orig,contacts,connections,site");
	VKAPI_ANSWER Answer = point();
	if (Answer.IsError == true || Answer.Object.find("response") == Answer.Object.end())
		return UserInfo(UserId, wstring(L"id") + to_wstring(UserId), AVATAR_USER_DEFAULT, "", "", "", false);
	try {
		json info = Answer.Object.at("response").get<json>();
		info = info.begin().value();
		string AvatarLink = AVATAR_USER_DEFAULT;
		if (info.find("photo_200_orig") != info.end())
		{
			if (!info.at("photo_200_orig").is_string())
				AvatarLink = AVATAR_USER_DEFAULT;
			else
				AvatarLink = info.at("photo_200_orig").get<string>();
		}
		if (UserId >= 0)
		{
			string phone_number;
			string instagram;
			string site;
			int64_t user_id = 0;
			if (info.find("mobile_phone") != info.end() && info.at("mobile_phone").is_string())
				phone_number = FixFileName(info.at("mobile_phone").get<string>());
			if (info.find("instagram") != info.end() && info.at("instagram").is_string())
				instagram = FixFileName(info.at("instagram").get<string>());
			if (info.find("site") != info.end() && info.at("site").is_string())
				site = FixFileName(info.at("site").get<string>());
			if (info.find("id") != info.end())
				user_id = info.at("id").get<int64_t>();

			return UserInfo(user_id, FixFileName(UTF8_to_wchar(info.at("last_name").get<string>()) + L" " + UTF8_to_wchar(info.at("first_name").get<string>())), AvatarLink, phone_number, instagram, site, true);
		}
		else
			return UserInfo(UserId, FixFileName(UTF8_to_wchar(info.at("name").get<string>())), AvatarLink, "", "", "", true);
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
		Request = "https://api.vk.com/method/" + IRequest;
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

struct DocDownloader
{
	int64_t CreatedTime;
	int64_t DialogTime;
	int64_t DialogID;
	int64_t ConversationID;
	std::string Type;
	std::string SingleLink;
	Map::Map<std::string, std::string>MultiLink;
	std::string Preview;
	std::string FileName;
	std::string Video_Title;
	std::string Dir;
	std::string UID;
	std::string TempAccessKey;
};

struct VKDialog
{
	VKDialog(bool IsChat, const std::wstring& Title, int last_conversation_message_id)
	{
		this->IsChat = IsChat;
		this->Title = Title;
		this->last_conversation_message_id = last_conversation_message_id;
	}
	VKDialog()
	{
		IsChat = false;
		last_conversation_message_id = 0;
	}
	bool IsChat;
	std::wstring Title;
	int last_conversation_message_id;
};

UserInfo GetChatNameById(VK_APIMETHOD& Method, int64_t ChatId)
{
	VK_APIMETHOD& point = Method["messages.getChat"];
	point << VKI("chat_id", ChatId - 2000000000);
	VKAPI_ANSWER Answer = point();
	if (Answer.IsError == true || Answer.Object.find("response") == Answer.Object.end())
		return UserInfo(ChatId, wstring(L"id") + to_wstring(ChatId), AVATAR_CHAT_DEFAULT, "", "", "", false);
	try {
		json info = Answer.Object.at("response").get<json>();
		string AvatarLink = AVATAR_CHAT_DEFAULT;
		vector<string> vpreviews;
		for (json::iterator videoi = info.begin(); videoi != info.end(); ++videoi)
		{
			if (videoi.key().length() >= 5 && strncmp(videoi.key().c_str(), "photo", 5) == 0)
				vpreviews.push_back(videoi.key());
		}
		if (vpreviews.size() > 0)
		{
			sort(vpreviews.begin(), vpreviews.end(), greater<string>());
			AvatarLink = info.at(vpreviews[vpreviews.size() - 1]).get<string>();
		}
		return UserInfo(ChatId, FixFileName(UTF8_to_wchar(info.at("title").get<string>())), AvatarLink, "", "", "", true);
	}
	catch (json::exception e) {
		e.what();
	}
	return UserInfo(ChatId, wstring(L"id") + to_wstring(ChatId), AVATAR_CHAT_DEFAULT, "", "", "", false);
}

static string FixFileExt(const string& filename, const string& ext)
{
	size_t off = filename.find_last_of('.');
	string exttemp;
	if (off != string::npos)
		exttemp = filename.substr(off + 1);
	if (!compare(exttemp, ext))
		return filename + "." + ext;
	return filename;
}

Map::Map<string, string> GetVideoLinks(VK_APIMETHOD& Method, int id, int64_t ownerid, const string& access_key)
{
	Map::Map<string, string> Ret;
	Method["video.get"] << VK("videos", to_string(ownerid) + "_" + to_string(id) + "_" + access_key);
	VKAPI_ANSWER Answer = Method();
	if (Answer.IsError == true)
		return Ret;

	json info = Answer.Object.at("response").get<json>();
	if (info.at("count").get<int>() == 0)
		return Ret;
	info = info.at("items").get<json>();
	info = info.begin().value();
	if (info.find("files") == info.end())
	{
		if (info.find("player") != info.end())
			Ret["player"] = info.at("player").get<string>();
		return Ret;
	}
	for (json::iterator videoi = info.at("files").begin(); videoi != info.at("files").end(); ++videoi)
		Ret[videoi.key()] = videoi.value().get<string>();
	return Ret;
}

static Map::Map<string, string>GetPhotoURL(const json& ite)
{
	Map::Map<string, string>Photos;
	if (ite.find("sizes") != ite.end() && ite.at("sizes").is_null() == false)
	{
		json hataph = ite.at("sizes").get<json>();
		vector<string> sizes = { "w", "z", "y", "x", "r", "q", "p", "o", "m", "s" };
		for (auto& szs : sizes)
		{
			for (auto& phsz : hataph) {
				if (phsz.at("type").get<string>() == szs) {
					Photos[szs] = phsz.at("url").get<string>();
				}
			}
		}
	}
	return Photos;
}

class UTF8Stream
{
public:
	UTF8Stream& operator<<(const std::wstring& String);
	UTF8Stream& operator<<(const std::string& String);
	UTF8Stream& operator<<(size_t value);
	UTF8Stream& operator<<(int value);
	UTF8Stream& operator<<(int64_t value);
	const std::string& GetString() const;
	int GetSize();
	void Clear();
private:
	std::string Data;
};

UTF8Stream& UTF8Stream::operator<<(const wstring& String)
{
	if (String.length() > 0)
	{
		string OT = wchar_to_UTF8(String);
		Data += OT;
	}
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(const string& String)
{
	if (String.length() > 0)
		Data += String;
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(size_t value)
{
	Data += to_string(value);
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(int value)
{
	Data += to_string(value);
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(int64_t value)
{
	Data += to_string(value);
	return *this;
}
const string& UTF8Stream::GetString() const
{
	return Data;
}
int UTF8Stream::GetSize()
{
	return (int)Data.length();
}
void UTF8Stream::Clear()
{
	Data.clear();
}

static void ReadExtendedInfoProfiles(Map::Map<int64_t, UserInfo>& UsersDataContent, const json& info)
{
	if (info.find("profiles") == info.end())
		return;
	json items = info.at("profiles").get<json>();
	if (items.is_array() == false || items.is_null() == true || items.size() == 0)
		return;
	for (json::iterator it = items.begin(); it != items.end(); ++it)
	{
		json ite = it.value();
		UserInfo temp;
		temp.pUserName = UTF8_to_wchar(ite.at("last_name").get<string>() + " " + ite.at("first_name").get<string>());
		string avatar;
		if (ite.find("photo_200_orig") != ite.end())
		{
			if (!ite.at("photo_200_orig").is_string())
				avatar = AVATAR_USER_DEFAULT;
			else
				avatar = ite.at("photo_200_orig").get<string>();
		}
		else
			avatar = AVATAR_USER_DEFAULT;
		temp.pAvatarLink = avatar;
		temp.pUserName = FixFileName(temp.pUserName);
		if (ite.find("mobile_phone") != ite.end() && ite.at("mobile_phone").is_string())
			temp.pPhoneNumber = FixFileName(ite.at("mobile_phone").get<string>());
		if (ite.find("instagram") != ite.end() && ite.at("instagram").is_string())
			temp.pInstagram = FixFileName(ite.at("instagram").get<string>());
		if (ite.find("site") != ite.end() && ite.at("site").is_string())
			temp.pSite = FixFileName(ite.at("site").get<string>());
		temp.user_id = ite.at("id").get<int64_t>();
		UsersDataContent[ite.at("id").get<int64_t>()] = temp;
	}
	if (info.find("groups") == info.end())
		return;
	items = info.at("groups").get<json>();
	if (items.is_array() == false || items.is_null() == true || items.size() == 0)
		return;
	for (json::iterator it = items.begin(); it != items.end(); ++it)
	{
		json ite = it.value();
		UserInfo temp;
		temp.pUserName = UTF8_to_wchar(ite.at("name").get<string>());
		string avatar;
		if (ite.find("photo_200_orig") != ite.end())
		{
			if (!ite.at("photo_200_orig").is_string())
				avatar = AVATAR_USER_DEFAULT;
			else
				avatar = ite.at("photo_200_orig").get<string>();
		}
		else
			avatar = AVATAR_USER_DEFAULT;
		temp.pAvatarLink = avatar;
		int64_t idd = ite.at("id").get<int64_t>();
		if (idd >= 0)
			idd *= -1;
		temp.user_id = idd;
		temp.pUserName = FixFileName(temp.pUserName);
		UsersDataContent[idd] = temp;
	}
}

void PrebuildParseAttachment(VK_APIMETHOD& VkReq, const json& message, list<DocDownloader>& DocDownloads, int64_t DialogId, const VKDialog& Dialog, int Message_Id)
{
	bool HasMessagePhoto = false;
	bool HasMessageVideo = false;
	bool HasWall = false;
	if (message.find("attachments") != message.end() && message.at("attachments").is_null() == false)
	{
		int message_id = Message_Id;

		json hata = message.at("attachments").get<json>();
		for (json::iterator st = hata.begin(); st != hata.end(); ++st)
		{
			bool NeedDoc = true;
			json atta = st.value();
			string DocType = atta.at("type").get<string>();
			if (DocType == "video" || DocType == "wall")
			{
				if (DocType == "wall")
				{
					atta = atta.at(DocType).get<json>();
					HasWall = true;
					extern void PrebuildAttachments_MESSAGE(VK_APIMETHOD & VkReq, const json & message, list<DocDownloader> &DocDownloads, int64_t DialogId, const VKDialog & Dialog, int Message_Id);
					if (atta.find("copy_history") != atta.end() && atta.at("copy_history").is_array() && atta.at("copy_history").size() > 0)
					{
						json copy_histor = atta.at("copy_history").get<json>();
						for (json::iterator cph = copy_histor.begin(); cph != copy_histor.end(); ++cph)
							PrebuildAttachments_MESSAGE(VkReq, cph.value(), DocDownloads, DialogId, Dialog, Message_Id);
					}
					PrebuildAttachments_MESSAGE(VkReq, atta, DocDownloads, DialogId, Dialog, Message_Id);
					continue;
				}
				if (DocType == "video")
				{
					if (HasMessageVideo == false)
						HasMessageVideo = true;
					Map::Map<int, string> vpreviews;
					int vid = 0;
					int64_t vownerid = 0;
					string access_key = "";
					string vpreview = "";
					json vprevj = atta.at(DocType).get<json>().at("image").get<json>();
					for (json::iterator videoi = vprevj.begin(); videoi != vprevj.end(); ++videoi)
						vpreviews[videoi.value().at("height").get<int>() * videoi.value().at("width").get<int>()] = videoi.value().at("url").get<string>();
					atta = atta.at(DocType).get<json>();
					if (vpreviews.size() > 0)
					{
						int lastsizevp = 0;
						for (auto& videoi : vpreviews)
						{
							if (lastsizevp < videoi.get_key())
							{
								vpreview = videoi.get_value();
								lastsizevp = videoi.get_key();
							}
						}
					}
					if (atta.find("access_key") != atta.end())
						access_key = atta.at("access_key").get<string>();
					if (atta.find("id") != atta.end())
						vid = atta.at("id").get<int>();
					if (atta.find("owner_id") != atta.end())
						vownerid = atta.at("owner_id").get<int64_t>();

					DocDownloader dwnldr;
					dwnldr.Type = "video";
					dwnldr.Preview = vpreview;
					dwnldr.CreatedTime = atta.at("date").get<int64_t>();
					dwnldr.DialogTime = message.at("date").get<int64_t>();
					dwnldr.Video_Title = FixFileName(atta.at("title").get<string>());
					dwnldr.UID = to_string(vownerid) + "_" + to_string(vid);
					dwnldr.Dir = wchar_to_UTF8(FixFileName(Dialog.Title + (Dialog.IsChat == false ? L" (Диалог)" : L" (Беседа)")));
					dwnldr.DialogID = DialogId;
					dwnldr.ConversationID = message_id;
					dwnldr.TempAccessKey = access_key;

					if (atta.find("files") != atta.end()) {
						for (json::iterator videoi = atta.at("files").begin(); videoi != atta.at("files").end(); ++videoi) {
							dwnldr.MultiLink[videoi.key()] = videoi.value().get<string>();
						}
						if (dwnldr.MultiLink.size() == 1 && dwnldr.MultiLink.exist("hls"))
							dwnldr.MultiLink.clear();
					}

					DocDownloads.push_back(dwnldr);
					continue;
				}
			}
		}
	}
}

void PrebuildAttachments_MESSAGE(VK_APIMETHOD& VkReq, const json& message, list<DocDownloader>& DocDownloads, int64_t DialogId, const VKDialog& Dialog, int Message_Id)
{
	int message_id = Message_Id;
	if (message.find("conversation_message_id") != message.end())
		message_id = message.at("conversation_message_id").get<int>();
	else if (message_id == -1)
		message_id = message.at("id").get<int>();

	PrebuildParseAttachment(VkReq, message, DocDownloads, DialogId, Dialog, message_id);

	if (message.find("fwd_messages") != message.end() && message.at("fwd_messages").size() > 0)
	{
		json array_forward = message.at("fwd_messages").get<json>();
		for (json::iterator array_it = array_forward.begin(); array_it != array_forward.end(); ++array_it)
		{
			json fwdmessage = array_it.value();
			PrebuildAttachments_MESSAGE(VkReq, fwdmessage, DocDownloads, DialogId, Dialog, message_id);
		}
	}
	else if (message.find("reply_message") != message.end())
	{
		json forward = message.at("reply_message").get<json>();
		PrebuildAttachments_MESSAGE(VkReq, forward, DocDownloads, DialogId, Dialog, message_id);
	}
}

void ParseAttachment(VK_APIMETHOD& VkReq, const string& UserAgent, Map::Map<int64_t, UserInfo>& UsersDataContent, HTML_MESSAGE_WITH_SUB& html_MESSAGE, const json& message, HTML_PAGE* page, bool is_sub, bool _isOut, HTML_MESSAGE* Work_Message, list<DocDownloader>& DocDownloads, int64_t DialogId, const VKDialog& Dialog, UserInfo& user_info_at_message, int Message_Id, bool DownloadAvatar)
{
	if (message.find("attachments") != message.end() && message.at("attachments").is_null() == false)
	{
		int message_id = Message_Id;

		json hata = message.at("attachments").get<json>();
		for (json::iterator st = hata.begin(); st != hata.end(); ++st)
		{
			bool NeedDoc = true;
			json atta = st.value();
			string DocType = atta.at("type").get<string>();
			if (DocType == "photo" || DocType == "doc" || DocType == "video" || DocType == "wall" || DocType == "link" || DocType == "album")
			{
				if (DocType == "wall")
				{
					Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_WALL);
					atta = atta.at(DocType).get<json>();
					extern void BuildHTML_MESSAGE(VK_APIMETHOD & VkReq, const string & UserAgent, Map::Map<int64_t, UserInfo> &UsersDataContent, HTML_MESSAGE_WITH_SUB & html_MESSAGE, const json & message, HTML_PAGE * page, bool is_sub, bool _isOut, list<DocDownloader> &DocDownloads, int64_t DialogId, const VKDialog & Dialog, int Message_Id, bool DownloadAvatar);
					BuildHTML_MESSAGE(VkReq, UserAgent, UsersDataContent, html_MESSAGE, atta, page, true, _isOut, DocDownloads, DialogId, Dialog, Message_Id, DownloadAvatar);
					if (atta.find("copy_history") != atta.end() && atta.at("copy_history").is_array() && atta.at("copy_history").size() > 0)
					{
						json copy_histor = atta.at("copy_history").get<json>();
						for (json::iterator cph = copy_histor.begin(); cph != copy_histor.end(); ++cph)
							BuildHTML_MESSAGE(VkReq, UserAgent, UsersDataContent, html_MESSAGE, cph.value(), page, true, _isOut, DocDownloads, DialogId, Dialog, Message_Id, DownloadAvatar);
					}
					continue;
				}
				if (DocType == "link") {
					Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_LINK);
					string original_photo;
					atta = atta.at(DocType).get<json>();
					if (atta.find("url") != atta.end()) {
						original_photo = atta.at("url").get<string>();
					}
					Work_Message->add_attacment_image(original_photo, "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAMAAABrrFhUAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAMAUExURQAAAEtLSwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAe2/O8AAAEAdFJOU////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////wBT9wclAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAKQklEQVR4Xu3aS6LbuBEF0GT/i3ZE6rwnisUPAOJDOj4Tt1Eo4up2Z5b//Pk/17+A/x5yqZ+OBfiJSax00KUAvyqb9aaaF+C3FPOZZtoW4Edc5GNtNCxA/Cp8soFWBQhekQ/X1qQAkavz+arqFyBsIx6pp3YBcjbkoVqqFiBic56romIB0nXhyQqqFSBZN569rFIBUnXl6YuqFCBRd56/pEIB0gwhwgXXCxBlECHKXS1AjoEEKXWtABkGE6bMpQIEGE6cIhcK8PotiFSguAAv34ZY2UoL8OyNCJarsACP3opomYoK8OLtiJelpADP3ZCAOQoK8NgtiZghvwBP3ZSQ6XIL8M6NCZoqswCP3JqoifIK8MTNCZsmqwAP3J64SXIK8PkHEDhFRgE+/ggiJ0gvwKcfQuhzqQX47oMIfiaxAB99FNFPpBXgkw8j/LGkAnzwccQ/9K8Afx7xuQfyA44kFOBjj+QnHDgvwKceyo/Yd1qADz2Wn7HrrACfeTA/ZM9JAT7yaH7KjuMCfOLh/JhthwX4wOP5OZv+FeDPLdb/An7QloMCLP8V/KQN+wVYHU6cH05z2Y7uXYAoa6Y5bEa7BVgcSZJNrqSzF+wVYG0gQXa5lsza2l0LEOOQq4ksre0UYGkYMc64ncbOynYBVkaRIoGFNHa+3bEAIZJYSWLl22YBFgYRIpGlJFa+bBXg+iBCpLOXwsbS7QqQIYfNBBaWNgpweQwZ8thNYGHhZgWIkMlyAgsLsQBXhxAhm/UEFj5CAS4OIUIBH0hg4dedCpCgiE+cc//XugDXRpCgjG8ksPDjPgUIsC3xRgr3f6wKcGkAAba4MXMUmSewwF0K8P4GF345DozPuc93Aa705/3I/IvRimECC2/3KMDzkfmK4YrhOfffvgpwoTvPR+aR+RejBBZmdyjA65H5FjeWTBJYmC0LMO7N65H5Jle+GCWwMBlfgMcj8x0uLZkksDAZXoC3I/Ndri0YJLAwWRRg2Je3I/N97i0YpLDxMrgAT0fmR9xcMEhg4eVTgFFXno7MD7m6YJDCxuACvByZH3N3wSCFjbEFeDgyP+P2h/MUNhYFGHTk4cj8lOsfzpNYGViAdyPzc+5/OE9iZVwBno3ME1j4cJ7Eym8BjrvxbGSewsaH8zR2BhXg1cg8iZUP52nsjCnAo5F5GjsfztPYUYDDTt5vbjBPY2fBINF7Z0gBX94BZk4SWVowSPTeGV/AyztDbghbCwaJ3jvvAhyN1P33+8JtCsg2B/9ikGxeemwBc+5vJsnmpbkAB48yp/9mkm5aemoBU+wVkwzT1kMLmFKvGWWYtgYVML164WHr38wyTFvdC5jeWzFJZ++bWY5pbSrAXzuYXtxknsbOimGW11rPAuacu1w65/6KYZ7XXr8C5piHXDzh8pppntderwLmkGfcPeTqmmmm12KnAuaQCVzf515gnOm1+CrAXxp6Z0xiY49bgXG2PgUImcbONnci82xdCpAxla0tbkTm+XoUIGMGi4FxZF6gQwEy5rC5ZhqZl2hfgIx57H4zi8yLTAX4xzZkzGV7ySQyL9S4ABnz2f9wHpmXaluAjCV84YfTyLxY0wJkLOITOIzMy7UsQMZCPjJzFJlf0LAAGTe58uIgMp84icyvaFeAjBtc+OF0xXDiJDK/pFkBMkbmSyZLJhMnkfk1rQqQMTL/ZvbhfOIkMr+oUQEyRuaBMQ4nTiLzq9oUIGNkvsGFmaOJk8j8siYFyBiZb3Fj4mTiJDK/rkUBMkbm29zp/PtbFCBjZL4n3nISmddQvwAZI/Nd4db7YIN5FX/+4x9qkTEyP7C6NW9tMa+k8n8BMkbmk9Vfd8xbW8wrqfw/ARkj80k42PS+tcG8lroFyBiZT+LJFrci82qqFiBjZD5xcvJL3InM66lZgIyR+cTJxMkWNyLziioWIGNkPnEyc7TBhci8pnoFyBiZT5zgMDCOzKuqVoCMkfnEyYfzb2aReV21CpAxMp84WTJZMonMK6tUgIyR+cTJiuEPpxtcqK1OATJG5hMnkfmLg02uVFelABkj85mjQj5SX40CZIzMcVjEJxqoUICMkfkPpyV8oYXrBcgYmX84z2e/icsFyBiZL5nkst3Gq4BLD8gYmX8zy2O3jav/DxEZI/M10xw2G7lYgIyReWCcwWIr1wqQMTLf4kYqW81cKkDGyHybO2nstNOkAOM9bqWw0dBcQOE774yR8T73Trne0uuV6gWYHnL1mLtNvZ4pLmAOGZmecPmAi429HqpcgOE593e41NzrqdIC5pyBYRo7kXkHr8emAkpenNYCs3T2lky6mN4rLGDaCsyyXVwvN71asQCjB5lS1yvA5Emm2GUFTEsrJo8y5Z4LyE4/L30zeZI5d60CDB5lDl6pAOfJbtHYnPxdQGac986SQaqipdreIYYUYGniZIR3gBEF2InMO3m/qYCLv8B5GjuReSce7V+Alci8F692L8BGZN6NZ38KuPYv0XkCC5F5N57tXYD7kXk/3u1cgOuReUce/hSQk8HGh/MzbkfmHXm4VgFpy+5G5j15uWsBrkbmXXl6WUBGDgsLBkfcjMy78vRLUQElDbgXmffl7ZdFARlRLCwY7HItMu/L25NaBZwsuxSZd+bxSbUCDrddicw78/hsWcC1Bky2uBGZ9+b1Wb0C9rfNI/PuPD/7KiA9kfsrhiuGkXl3nn8rLCCjAaPIvD/vv30XkB7K/cD4l+MNLvTnfUoLSPsX62iLGwMIwKqA9Fzub0u8MYQAP4oLOP59Z3xjBAl+rAuo9J/ACZ8YQYJf5QVcaMAHhhDhVyigQwPWhxDhIxbQvAHLQ4iw0L8Au2PIsLBRQNsGbI4hw9K1AvIbsDeIEEtbBTRswNYgQnzZLCArp5UkVgYR4tv1AjIasDCKFN+2C2jSgNvDiLGyU0BmWkuHXB1GjLU6BZxX4NpAgqztFZCf2N4mV0aSJNgtoCS0zTXToUSJqhYwsf3D6XDiRPsF3CZ7DX7ShoMC/qIG/KAtRwX8NQ34OZv+FeDPbT7wcH7MtuMC/ooG/JQdJwX8BQ34IXvOCnh8A37GrtMCHt6AH7HvvIBHN+AnHEgo4MEN+AFHUgp4bAPiH/pXgD+P+eDDCH8srYBHNiD6icQCHtiA4GdSC3haBUKfSy/gUQ2InCCjgAc1IHCKnAIe04C4SbIKeEgDwqbJK+ARDYiaKLOABzQgaKrcAu5egZDp8gu4dQMiZigo4MYNCJijpIDbNiBelqIC7lmBaJkKC7hhA4LlKi3gdg2Ila24gHtVIFKBCwXcpwJxilwq4CYNCFPmWgF3qECQUlcLGF2BEOWuFzC0AREuqFDAuAo8f0mVAsZU4OmLKhXQvwLPXlatgL4VeLKCigX0q8BzVVQt4EXEhjxUS+0CWlfgkXrqF/AibHU+X1WTAl5ErsiHa2tVQOUKfLOBdgVMxL/Ix9poW8CLH1HMZ5ppXsDEb8lmvakuBbz5VUmsdNCxAPzEHS7107+AW/nz538Nvezh4AZTvQAAAABJRU5ErkJggg==");
					continue;
				}
				if (DocType == "album") {
					Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_ALBUM);
					string original_photo = "https://vk.com/album";
					atta = atta.at(DocType).get<json>();
					string vid = "";
					int64_t vownerid = 0;
					if (atta.find("id") != atta.end()) {
						if (atta.at("id").is_string()) {
							vid = atta.at("id").get<string>();
						}
						else {
							vid = to_string(atta.at("id").get<int>());
						}
					}
					if (atta.find("owner_id") != atta.end())
						vownerid = atta.at("owner_id").get<int64_t>();

					Work_Message->add_attacment_image(original_photo + to_string(vownerid) + "_" + vid, "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAMAAACahl6sAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAMAUExURQAAAAEBAQICAgMDAwQEBAUFBQYGBgcHBwgICAkJCQoKCgsLCwwMDA0NDQ4ODg8PDxAQEBERERISEhMTExQUFBUVFRYWFhcXFxgYGBkZGRoaGhsbGxwcHB0dHR4eHh8fHyAgICEhISIiIiMjIyQkJCUlJSYmJicnJygoKCkpKSoqKisrKywsLC0tLS4uLi8vLzAwMDExMTIyMjMzMzQ0NDU1NTY2Njc3Nzg4ODo6Ojw8PD09PT4+Pj8/P0BAQEFBQUJCQkNDQ0REREVFRUZGRkdHR0hISElJSUpKSktLS0xMTE1NTU5OTk9PT1BQUFFRUVJSUlNTU1VVVVZWVldXV1hYWFlZWVpaWgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALcULVgAAAEAdFJOU////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////wBT9wclAAAACXBIWXMAAAsTAAALEwEAmpwYAAAHaElEQVR4Xu2dCXfUNhRGH0sIO4SwBChMIKEQthYSMpSELbRAgVCgLf3/v2T6bH+eGVuLn+RN5uiew8nzaCy/6/EiyZqBJj8IUSQ0okhoRJHQiCKhEUVCI4qERhQJjSgSGlEkNKJIaESR0IgioRFFQiOKhEYUCQ25yGHqg3vYeiVSkbOouAceIQU7QpFjXOHK+6+d8uX91plM5VdkYUMmcpfoOMKu2UlMDmDBgkxEVFVbfE9UXmHBiEjkH6LvCHvhjuDwEolskPBUag02eYLQgCjDq72LJCbfEOoZikhigkjPYET2iE4h1DIYkeSWjEjLcET44DqESMeARG5YsxiQCH8kK4g0DEnkkC2NIYk8/1FE+Nj6jEhlYCJ3EakMTOQsIpWBiZjvJAMTMecRRfogiuREkYaJIjlRpGGiSM75QYukg64FjqKkNzgHRCrmksUs+3lQ0hu2FCzJbazlHCa6ura2bh/q6wBPkRnxqtUwUSQnijRMFMmJIg1TW+TKsEUO8HoFfkZBb3AOiFTMJYey7OdBSW/YUrAklyU/xzUU9AbngEhFtJdX+v8wUmqLxMtvw0SRnMZEbnMqRMf3sehKKCJznefHeMkNXhGRSnciW6nB+Y2b6eTIw3jVCV4PkUpnIrc4iyuI/0pUELsQgshuMQluvXnMyfMUGW/mLBDd3tzcwutelHMY+TQU/ESSiaUlUOLBMtHfCAHvHERybCmYS55kyc+xgBIPNLNUidYQieEcEKmIdsu5Oh9Gwr6mAu4lIBJTW6T2yX5JU8HmEEW0GUQRLVHEiQuaCraHKPJBU8HBIYpwCsqkXaIRIjEBiCwpNXCbHpGc2iInqyamV1POYX3WFpbjJ7LCq5VAiQ9Pi6tzm9GjNttK5pLLSepFUOLFKq+/ijgdxETogm0tUX0NHFqTyYNkV9C1rfvHk78H8aoTvB4iFZFI/ZM9ZW4Q1jxb1AaviEilS5HJ5FpqsfAGi66EI1KTKJITRRomiuR4ipzE38aoLeI3+GDbrB9+Io94tSK272qpJEO8bmtUwjUiUjGXpC2JIigRsZyusYSlZrClYEnu2TjnCLcpxuOneF0Et6yOJW00v7aIAU+RGe4n+5dsm9zK9X2oo6MHkXyTtk27070IfxLv0+BzxeOD0sh2BZ2LnCK6jfA+0QmEGi66Vdy1yE2iMwjTe9C0Y1jmHWfmMsTfschesf/H3amXCMtwYvPOlXQsUt6ccfNHiXb5Ci0f3upWhLdW+qULw/YfEl1OPzDxM73aIi6TahbVh+jbREcQzoMLGmcnvdd4ivBqRXj/VXKF6CLCGZd1X+CePrHimrOgEts7zSVeE8+e6i9D/DGVjx+W+yWLvopqTrC90VKFYiL4ZSV+F6Iiyuv7SVsMjKXDXKbaEywiM8Qnu3lL5ZLCMt94LHfNGZ2JcBdkF2GZ18XOCfcR3iFMWLL+CsKUrkT4Jn4doQrv9dl3uDfLlwTuKNxHaKEjEW5W2X67im99Gwg1GfErewjNdCOSTPlBqIevtx+ySPfUrWrthG5EqjPJ3/GTtuMoMOlEZLa/jeAz+6a/0b+qnvnUhQifAXcQmsnOIlM6G0SLCA10IFK4Jpnh69pVVn6NxRKXiJYR6vEUWRvl8D1+ZTRa/YQCFb5LyO7NaWvhHBYU+O6yjlCLnwhf2sugRMVWVsRaTXI5+w2hDmsK+KvyItlmgWnjqAyX/YewCr4mjBHq4Jos4xFcikhFtCcrvnTMbdtNhFWsEp1HqMeWa30R+8nODfJLCCupHGz4YE22VRFD/0+LdX9nPLZMbm5XRJBczhmiHYRmrhOdRlimVREHjx3Z2M9p45TgNkX4xvAcYSVSZ+7W6H8at0URvlWLh6U4P0RVcMJ/IizQnsg9h5/PXSe6gbASQ8atiXy21VzG5UHcv/qKWxNx85C/dzJ5pm28tSXCzY2PCCvhk8np0d0tXYuoJZETDk8Iuddk/UlVFW4WKUOW7fx02w2XZ7ZuB1YKt+CmgxXAtkVvEd7H8pOXuwRuD9kS+MgtPVixDXZ6i7jsY+7E+nzPV9kE0RdEKr4iTseK5xTG8ka4iYNIg6cIHyvyr1x5nCAZvxfX5OYQIg1+IiuysdqMC77fn0znT8x1Eaxznr1Enjh0QSZvHZoxCtxpm16oRtYP1kvE8QRxeLPCyVkLzf51Uh8Rp9SO1pyOwidGNlrBvbL0rwEPEa7a9OBchY9yyaNHC7zbvvKfFxW9MneRJctUBpUG/nuJ7ACoOgycRe4KH5Nl8N0ZkT/JlKnX/M/+/1SItsPJI5p8ctrFfJl+gLAG22xR2XgWiXyfNZW4RkQC2LqRH+Bb4K1WDQHK0pqmz4eKeShbwcnaSPIcnv7AghHZlvjak+5aly6I8ujWj3GiIWiqCXcZG9DyC85sYW9fyEe+8C++xYIPH988xEzXbWRhQ/rZZ7Nfe0GiIRdJe2zdc1A6zO8gEjhRJDSiSGhEkdCIIqERRUIjioRGFAmNKBIaUSQ0okhoRJHQiCKhEUVCI4qERhQJi8nkfwK9uMixRO8lAAAAAElFTkSuQmCC");
					continue;
				}
				if (DocType == "video")
				{
					Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_VIDEO);
					atta = atta.at(DocType).get<json>();
					int vid = 0;
					int64_t vownerid = 0;
					if (atta.find("id") != atta.end())
						vid = atta.at("id").get<int>();
					if (atta.find("owner_id") != atta.end())
						vownerid = atta.at("owner_id").get<int64_t>();
					for (auto& attcm : DocDownloads)
					{
						if (attcm.UID == (to_string(vownerid) + "_" + to_string(vid)))
						{
							Work_Message->add_attacment_video(attcm.Preview, FixFileName(atta.at("title").get<string>()), attcm.MultiLink);
							break;
						}
					}
					continue;
				}
				atta = atta.at(DocType).get<json>();
				string DocumentUrl;
				Map::Map<string, string>PhotosUrls;
				wstring ResDt;
				ResDt = GetTimeAT(atta.at("date").get<int64_t>());
				if (DocType == "photo")
				{
					ResDt += L".jpg";
					PhotosUrls = GetPhotoURL(atta);
					if (PhotosUrls.size() <= 0)
						NeedDoc = false;
					else
					{
						vector<string> vsz = { "q", "r", "x", "y", "z", "w" };
						string little_preview;
						string original_photo;
						for (auto& imgsz : vsz)
						{
							if (PhotosUrls.exist(imgsz))
							{
								little_preview = PhotosUrls.at(imgsz);
								break;
							}
						}
						vector<string> vszm = { "w", "z", "y", "x", "r", "q", "p", "o", "m", "s" };
						for (auto& imgsz : vszm)
						{
							if (PhotosUrls.exist(imgsz))
							{
								original_photo = PhotosUrls.at(imgsz);
								break;
							}
						}
						string res_url = (const char*)u8"./Материалы/ФОТО/" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName)) + "/" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName + L" " + L"[" + to_wstring(message_id) + L"] " + ResDt));
						Work_Message->add_attacment_image(res_url, res_url);
					}
				}
				else if (DocType == "doc")
				{
					if (atta.at("type").get<int>() == 4)
					{
						DocumentUrl = atta.at("url").get<string>();
						if (DocumentUrl.length() <= 0)
							NeedDoc = false;
						else
						{
							ResDt = UTF8_to_wchar(FixFileExt(atta.at("title").get<string>(), atta.at("ext").get<string>()));
							string res_url = (const char*)u8"./Материалы/ДОКУМЕНТЫ/" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName)) + "/" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName + L" " + L"[" + to_wstring(message_id) + L"] " + ResDt));
							Work_Message->add_attacment_image(res_url, res_url);
						}
					}
					else if (atta.at("type").get<int>() == 6)
					{
						Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_DOC);
						Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_VIDEO);
						int vid = 0;
						int64_t vownerid = 0;
						if (atta.find("id") != atta.end())
							vid = atta.at("id").get<int>();
						if (atta.find("owner_id") != atta.end())
							vownerid = atta.at("owner_id").get<int64_t>();
						Map::Map<string, string> VideoLinks;
						VideoLinks["url"] = atta.at("url").get<string>();
						Work_Message->add_attacment_video("", FixFileName(atta.at("title").get<string>()), VideoLinks);

						DocDownloader dwnldr;
						dwnldr.Type = "video";
						dwnldr.MultiLink = VideoLinks;
						dwnldr.Preview = "";
						dwnldr.CreatedTime = atta.at("date").get<int64_t>();
						dwnldr.DialogTime = message.at("date").get<int64_t>();
						dwnldr.Video_Title = FixFileName(atta.at("title").get<string>());
						dwnldr.UID = to_string(vownerid) + "_" + to_string(vid);
						dwnldr.Dir = wchar_to_UTF8(FixFileName(Dialog.Title + (Dialog.IsChat == false ? L" (Диалог)" : L" (Беседа)")));
						dwnldr.DialogID = DialogId;
						dwnldr.ConversationID = message_id;
						DocDownloads.push_back(dwnldr);
						continue;
					}
					else
						NeedDoc = false;
				}
				if (NeedDoc == true)
				{
					DocDownloader dwnldr;
					dwnldr.Type = DocType;
					if (DocType == "doc")
					{
						dwnldr.FileName = (const char*)u8"ДОКУМЕНТЫ\\" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName)) + "\\" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName + L" " + L"[" + to_wstring(message_id) + L"] " + ResDt));
						Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_DOC);
						dwnldr.SingleLink = DocumentUrl;
					}
					else
					{
						dwnldr.FileName = (const char*)u8"ФОТО\\" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName)) + "\\" + wchar_to_UTF8(FixFileName(user_info_at_message.pUserName + L" " + L"[" + to_wstring(message_id) + L"] " + ResDt));
						Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_PHOTO);
						dwnldr.MultiLink = PhotosUrls;
					}
					dwnldr.UID = to_string(atta.at("owner_id").get<int64_t>()) + "_" + to_string(atta.at("id").get<int>());
					dwnldr.Dir = "html\\" + wchar_to_UTF8(FixFileName(Dialog.Title + (Dialog.IsChat == false ? L" (Диалог)" : L" (Беседа)")) + L"\\id" + to_wstring(DialogId) + L"\\Материалы");
					dwnldr.CreatedTime = atta.at("date").get<int64_t>();
					dwnldr.DialogTime = message.at("date").get<int64_t>();
					dwnldr.DialogID = DialogId;
					dwnldr.ConversationID = message_id;

					DocDownloads.push_back(dwnldr);
				}
				else
					Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_UNKNOW);
			}
			else
				Work_Message->SetHasAttachment(HTML_ATTACHMENT_FLAG_UNKNOW);
		}
	}
}

void BuildHTML_MESSAGE(VK_APIMETHOD& VkReq, const string& UserAgent, Map::Map<int64_t, UserInfo>& UsersDataContent, HTML_MESSAGE_WITH_SUB& html_MESSAGE, const json& message, HTML_PAGE* page, bool is_sub, bool _isOut, list<DocDownloader>& DocDownloads, int64_t DialogId, const VKDialog& Dialog, int Message_Id, bool DownloadAvatar)
{
	HTML_MESSAGE* Work_Message = NULL;
	if (is_sub == true)
	{
		Work_Message = &html_MESSAGE.add_sub_message();
		Work_Message->set_is_sub_message();
	}
	else
		Work_Message = &html_MESSAGE.main_html_message;

	int message_id = Message_Id;
	if (message.find("conversation_message_id") != message.end())
		message_id = message.at("conversation_message_id").get<int>();
	else if (message_id == -1)
		message_id = message.at("id").get<int>();

	int64_t from_id = message.at("from_id").get<int64_t>();
	int64_t message_time = message.at("date").get<int64_t>();
	if (!is_sub)
		_isOut = message.at("out").get<int>() > 0;
	string message_text;
	if (message.find("text") != message.end())
		message_text = message.at("text").get<string>();


	if (message.find("fwd_messages") != message.end() && message.at("fwd_messages").size() > 0)
		message_text += (const char*)u8" (ПЕРЕСЛАННОЕ)";
	if (message.find("reply_message") != message.end())
		message_text += (const char*)u8" (ОТВЕТ)";
	if (message.find("copy_history") != message.end() && message.at("copy_history").is_array() && message.at("copy_history").size() > 0)
		message_text += (const char*)u8" (РЕПОСТ)";

	if (UsersDataContent.exist(from_id) == false)
		UsersDataContent[from_id] = GetUserNameById(VkReq, from_id);
	UserInfo& user_info_at_message = UsersDataContent.at(from_id);

	string idlink;
	if (from_id < 0)
		idlink = ("https://vk.com/club" + to_string(abs(from_id)));
	else
		idlink = "https://vk.com/id" + to_string(from_id);
	if (!page->exist_avatar(from_id))
		page->add_user_avatar(from_id, user_info_at_message, DownloadAvatar);
	Work_Message->set_message(_isOut, message_text, idlink, from_id, wchar_to_UTF8(user_info_at_message.pUserName), wchar_to_UTF8(GetTimeAT(message_time)) + " [" + to_string(message_id) + "]");

	ParseAttachment(VkReq, UserAgent, UsersDataContent, html_MESSAGE, message, page, is_sub, _isOut, Work_Message, DocDownloads, DialogId, Dialog, user_info_at_message, message_id, DownloadAvatar);

	if (message.find("fwd_messages") != message.end() && message.at("fwd_messages").size() > 0)
	{
		json array_forward = message.at("fwd_messages").get<json>();
		for (json::iterator array_it = array_forward.begin(); array_it != array_forward.end(); ++array_it)
		{
			json fwdmessage = array_it.value();
			BuildHTML_MESSAGE(VkReq, UserAgent, UsersDataContent, html_MESSAGE, fwdmessage, page, true, _isOut, DocDownloads, DialogId, Dialog, message_id, DownloadAvatar);
		}
	}
	else if (message.find("reply_message") != message.end())
	{
		json forward = message.at("reply_message").get<json>();
		BuildHTML_MESSAGE(VkReq, UserAgent, UsersDataContent, html_MESSAGE, forward, page, true, _isOut, DocDownloads, DialogId, Dialog, message_id, DownloadAvatar);
	}
}

static void WriteHTML(const wstring& HTMLParhT, time_t Sync, const wstring& RootDir, const string& Html, int Part, time_t time)
{
	wstring HTMLParh = HTMLParhT;
	if (Part > 0 && !needCheckLastMessages) {
		HTMLParh += (L".часть " + to_wstring(Part));
	}
	if (time > 0) {
		HTMLParh += (L" (") + FixFileName(GetTimeAT(time)) + L")";
	}
	HTMLParh += L".html";
	FlBin out;
	out.wopen(RootDir + L"\\" + HTMLParh, OPENType::OPENWrite, true);
	(out << UTF8START).BWrite(Html.data(), 1, Html.length());
	out.Close();
}

inline int patch_video(list<DocDownloader>& DocDownloads, const string& UID, const Map::Map<string, string>& links) {
	int ret = 0;
	for (auto i : DocDownloads) {
		if (i.UID == UID) {
			i.MultiLink = links;
			ret++;
		}
	}
	return ret;
}

void BuildHTMLDialog(const wstring& HTMLParh, time_t Sync, const wstring& RootDir, VK_APIMETHOD& VkReq, const string& JSON, const string& UserAgent, Map::Map<int64_t, UserInfo>& UsersDataContent, const VKDialog& Dialog, int64_t DialogId, list<DocDownloader>& DocDownloads, bool DownloadAvatar)
{
	int PartNum = 0;
	time_t SnapTime = 0;
	HTML_PAGE* page = new HTML_PAGE(UserAgent);
	if (UsersDataContent.exist(DialogId) == false)
	{
		if (Dialog.IsChat == true)
			UsersDataContent[DialogId] = GetChatNameById(VkReq, DialogId);
		else
			UsersDataContent[DialogId] = GetUserNameById(VkReq, DialogId);
	}
	UserInfo& Usnm = UsersDataContent.at(DialogId);
	string idlink = to_string(DialogId);
	if (DialogId < 0)
		idlink = ("club" + to_string(abs(DialogId)));
	else
		idlink = ("id" + to_string(DialogId));

	json dialog_content;
	try {
		dialog_content = json::parse(JSON);
		dialog_content = dialog_content.at(dialog_content.at("type").get<string>()).get<json>();
	}
	catch (json::exception e) {
		page->add_page_info(Usnm, DownloadAvatar, wchar_to_UTF8(Usnm.pUserName + (Dialog.IsChat == false ? L" (Диалог)" : L" (Беседа)")), "https://vk.com/" + idlink, Usnm.pPhoneNumber, Usnm.pInstagram, Usnm.pSite, PartNum);
		page->add_message(e.what());
		WriteHTML(HTMLParh, Sync, RootDir, page->build_html(), PartNum, SnapTime);
		delete page;
		return;
	}
	if (dialog_content.size() > MAX_HTML_MESSAGES_COUNT)
		PartNum++;
	page->add_page_info(Usnm, DownloadAvatar, wchar_to_UTF8(Usnm.pUserName + (Dialog.IsChat == false ? L" (Диалог)" : L" (Беседа)")), "https://vk.com/" + idlink, Usnm.pPhoneNumber, Usnm.pInstagram, Usnm.pSite, PartNum);

	list<DocDownloader> VideoDownloads;
	for (json::iterator messages_it = dialog_content.begin(); messages_it != dialog_content.end(); ++messages_it)
	{
		try {
			json message = messages_it.value();
			HTML_MESSAGE_WITH_SUB html_MESSAGE;
			PrebuildAttachments_MESSAGE(VkReq, message, VideoDownloads, DialogId, Dialog, -1);
		}
		catch (json::exception e) {
			page->add_message(e.what());
		}
	}
	list<DocDownloader> VideoDownloadsHasLinks;
	for (auto videoit = VideoDownloads.begin(); videoit != VideoDownloads.end();) {
		if ((*videoit).MultiLink.size() > 0) {
			VideoDownloadsHasLinks.push_back((*videoit));
			videoit = VideoDownloads.erase(videoit);
		}
		else {
			videoit++;
		}
	}

	try {
		auto videoit = VideoDownloads.begin();
		int videoitoff = 0;
		int patched = 0;

		bool NeedRequestVideo = false;
		bool Video_Get_OnlyDialog = false;
		if (NeedRequestVideo && Video_Get_OnlyDialog && Dialog.IsChat) {
			NeedRequestVideo = false;
		}

		if (VideoDownloads.size() > 0 && NeedRequestVideo) {
			PrintMessage((const char*)u8"Обработка видео: " + to_string(VideoDownloads.size()) + (const char*)u8" штук", URGB(255, 255, 255));
		}

		do
		{
			if (VideoDownloads.size() <= 0 || !NeedRequestVideo)
				break;
			int count = ((int)VideoDownloads.size() - videoitoff) >= MAX_VIDEO_REQUEST ? MAX_VIDEO_REQUEST : ((int)VideoDownloads.size() - videoitoff);
			if (count <= 0) {
				break;
			}

			string RequestCode;
			bool first = true;
			for (int s = 0; s < count; s++)
			{
				if (first) {
					first = false;
				}
				else {
					RequestCode += ",";
				}
				RequestCode += (*videoit).UID + "_" + (*videoit).TempAccessKey;
				videoitoff++;
				videoit++;
			}

			VkReq["video.get"] << VK("videos", RequestCode) << VKI("count", MAX_VIDEO_REQUEST);
			VKAPI_ANSWER DWN = VkReq();
			if (DWN.IsError == false)
			{
				json infot = DWN.Object.at("response").get<json>();
				if (infot.at("count").get<int>() == 0) {
					continue;
				}
				infot = infot.at("items").get<json>();
				for (json::iterator array_it = infot.begin(); array_it != infot.end(); ++array_it)
				{
					Map::Map<string, string> Ret;
					json info = array_it.value();

					int vid = 0;
					int64_t vownerid = 0;
					if (info.find("id") != info.end())
						vid = info.at("id").get<int>();
					if (info.find("owner_id") != info.end())
						vownerid = info.at("owner_id").get<int64_t>();
					if (info.find("files") == info.end())
					{
						if (info.find("player") != info.end())
							Ret["player"] = info.at("player").get<string>();
						patched += patch_video(VideoDownloads, to_string(vownerid) + "_" + to_string(vid), Ret);
						continue;
					}
					for (json::iterator videoi = info.at("files").begin(); videoi != info.at("files").end(); ++videoi)
						Ret[videoi.key()] = videoi.value().get<string>();
					patched += patch_video(VideoDownloads, to_string(vownerid) + "_" + to_string(vid), Ret);
				}
			}
		} while (videoit != VideoDownloads.end());

		if (VideoDownloads.size() > 0 && NeedRequestVideo) {
			PrintMessage((const char*)u8"Получено видео: " + to_string(patched) + (const char*)u8" штук", URGB(255, 255, 0));
		}
	}
	catch (json::exception e) {
		PrintMessage((const char*)u8"Ошибка Получение видео: " + (string)e.what(), URGB(255, 0, 0));
	}

	for (auto& i : VideoDownloadsHasLinks)
		DocDownloads.push_back(i);

	for (auto& i : VideoDownloads)
		DocDownloads.push_back(i);

	int CurrentMsgs = 0;
	for (json::iterator messages_it = dialog_content.begin(); messages_it != dialog_content.end(); ++messages_it, CurrentMsgs++)
	{
		if (CurrentMsgs >= MAX_HTML_MESSAGES_COUNT)
		{
			CurrentMsgs = 0;
			WriteHTML(HTMLParh, Sync, RootDir, page->build_html(), PartNum, SnapTime);
			SnapTime = 0;
			delete page;
			PartNum++;
			page = new HTML_PAGE(UserAgent);
			page->add_page_info(Usnm, DownloadAvatar, wchar_to_UTF8(Usnm.pUserName + (Dialog.IsChat == false ? L" (Диалог)" : L" (Беседа)")), "https://vk.com/" + idlink, Usnm.pPhoneNumber, Usnm.pInstagram, Usnm.pSite, PartNum);
		}
		try {
			json message = messages_it.value();
			if (SnapTime == 0) {
				SnapTime = message.at("date").get<int64_t>();
			}
			HTML_MESSAGE_WITH_SUB html_MESSAGE;
			BuildHTML_MESSAGE(VkReq, UserAgent, UsersDataContent, html_MESSAGE, message, page, false, false, DocDownloads, DialogId, Dialog, -1, DownloadAvatar);
			page->add_message(html_MESSAGE.build_message());
		}
		catch (json::exception e) {
			page->add_message(e.what());
		}
	}
	WriteHTML(HTMLParh, Sync, RootDir, page->build_html(), PartNum, SnapTime);
	delete page;
}

static void ParseHistorySingle(VK_APIMETHOD& VkReq, int& ParsingOffset, int& ParsingCount, bool& HasError, size_t& DialogsIterators, Map::Map<int64_t, UserInfo>& UsersDataContent, json& DialogData, Map::Pair<int64_t, VKDialog>& i, time_t& dialog_time)
{
	dialog_time = time(0);
	do
	{
		VkReq["messages.getHistory"] << VKI("count", 200) << VKI("offset", ParsingOffset) << VKI("peer_id", i.get_key()) << VKI("extended", 1) << VK("fields", "photo_200_orig,contacts,connections,site");
		VKAPI_ANSWER DWN = VkReq();
		if (DWN.IsError == true)
		{
			HasError = true;
			PrintMessage((i.get_value().IsChat == false ? string((const char*)u8"Ошибка: диалог с ") : string((const char*)u8"Ошибка: беседа ")) + to_string(DialogsIterators) + " \"" + wchar_to_UTF8(i.get_value().Title) + "!!!", URGB(255, 0, 0));
			break;
		}
		json info = DWN.Object.at("response").get<json>();

		if (ParsingCount == 0)
		{
			ParsingCount = info.at("count").get<int>();
			PrintMessage((i.get_value().IsChat == false ? string((const char*)u8"Скачивается диалог с ") : string((const char*)u8"Скачивается беседа ")) + to_string(DialogsIterators) + " \"" + wchar_to_UTF8(i.get_value().Title) + (const char*)u8"\" сообщений " + to_string(ParsingCount) + "...", URGB(255, 255, 255));
		}
		else
		{
			RemoveLastLine();
			PrintMessage((i.get_value().IsChat == false ? string((const char*)u8"Скачивается диалог с ") : string((const char*)u8"Скачивается беседа ")) + to_string(DialogsIterators) + " \"" + wchar_to_UTF8(i.get_value().Title) + (const char*)u8"\" сообщений [" + to_string(ParsingOffset) + ":" + to_string(ParsingCount) + "]", URGB(255, 255, 255));
		}
		ReadExtendedInfoProfiles(UsersDataContent, info);
		json items = info.at("items").get<json>();
		if (items.size() <= 0)
			break;
		if (ParsingOffset == 0) {
			dialog_time = items.begin().value().at("date").get<int64_t>();
			if (needCheckLastMessages) {
				CurrentLastMessages[i.get_key()] = items.begin().value().at("id").get<int>();
			}
		}

		if (needCheckLastMessages && CachedLastMessages.exist(i.get_key())) {
			int last_message = CachedLastMessages.at(i.get_key());
			bool found_last = false;
			json::iterator itatt;
			for (itatt = items.begin(); itatt != items.end(); ++itatt) {
				if (last_message >= itatt.value().at("id").get<int>()) {
					found_last = true;
					break;
				}
			}
			if (found_last) {
				items.erase(itatt, items.end());
				if (!items.is_null() && items.is_array() && items.size() > 0)
					DialogData.insert(DialogData.end(), items.begin(), items.end());
				break;
			}
		}

		if (!items.is_null() && items.is_array() && items.size() > 0)
			DialogData.insert(DialogData.end(), items.begin(), items.end());

		ParsingOffset += (int)items.size();
	} while (ParsingOffset < ParsingCount);
}

static bool MessageDataSort(const Map::Pair<int64_t, VKDialog>& lhs, const Map::Pair<int64_t, VKDialog>& rhs)
{
	return lhs.get_value().last_conversation_message_id > rhs.get_value().last_conversation_message_id;
}

void DownloadDialog(const string& access_token, bool avatarsBase64, bool needAlbumsAndDocs)
{
	Map::Map<int64_t, UserInfo> UsersDataContent;
	VK_APIMETHOD VkReq(access_token, ANDROID_USERAGENT);
	UserInfo UsnmMain = GetUserNameById(VkReq, 0);
	wstring UserName = UsnmMain.pUserName;
	int64_t user_id = UsnmMain.user_id;

	wstring Path = FixFileName(UserName);
	wstring last_messages = Path + L"\\" + L"last_messages.json";
	if (needCheckLastMessages) {
		PRINT(TypeColor::TYPE_WARRNING) << L"Использует last_messages.json" << FLUSH;
		FILE* fl = NULL;
		if (_wfopen_s(&fl, last_messages.c_str(), L"rb") == 0)
		{
			fseek(fl, 0, SEEK_END);
			int fsz = ftell(fl) - (int)strlen(UTF8START);
			fseek(fl, (int)strlen(UTF8START), SEEK_SET);
			string jsonna;
			jsonna.resize(fsz + 1);
			fread((char*)jsonna.data(), 1, fsz, fl);
			fclose(fl);
			json analiz;
			try {
				analiz = json::parse(jsonna);
				for (json::iterator itatt = analiz.begin(); itatt != analiz.end(); ++itatt) {
					json dt = itatt.value();
					CachedLastMessages[dt.at("peer_id").get<int64_t>()] = dt.at("last_message_id").get<int>();
				}
			}
			catch (...) {
				PRINT(TypeColor::TYPE_ERROR) << analiz << FLUSH;
				return;
			}
			PRINT(TypeColor::TYPE_INFO) << L"[Разобран last_messages.json]" << FLUSH << FLUSH;
		}
		Path += (L"\\Синхронизация " + FixFileName(GetTimeLocal()));
	}
	if (UsersDataContent.exist(UsnmMain.user_id) == false)
		UsersDataContent[UsnmMain.user_id] = UsnmMain;
	time_t SyncDataTime = time(0);

	UTF8Stream EntryInfo;

	Map::Map<int64_t, VKDialog> DlgsChats;
	Map::Map<int64_t, VKDialog> Dlgs;
	int ParsingCount = 0;
	int ParsingOffset = 0;
	list<DocDownloader> DocDownloads;
	do
	{
		VkReq["messages.getConversations"] << VKI("count", 200) << VKI("offset", ParsingOffset);
		VKAPI_ANSWER DWN = VkReq();
		if (DWN.IsError == true)
		{
			break;
		}
		json info = DWN.Object.at("response").get<json>();
		if (ParsingCount == 0)
			ParsingCount = info.at("count").get<int>();
		json items = info.at("items").get<json>();

		for (json::iterator it = items.begin(); it != items.end(); ++it)
		{
			json itemtuo = it.value().at("conversation").get<json>();
			json item = itemtuo.at("peer").get<json>();
			VKDialog gdlg;
			gdlg.last_conversation_message_id = it.value().at("last_message").get<json>().at("conversation_message_id").get<int>();
			int64_t DialogID = item.at("id").get<int64_t>();
			gdlg.Title = wstring(L"id") + to_wstring(DialogID);
			if (item.at("type").get<string>() == "chat")
			{
				if (itemtuo.find("chat_settings") != itemtuo.end())
				{
					json itemcs = itemtuo.at("chat_settings").get<json>();
					gdlg.Title = UTF8_to_wchar(itemcs.at("title").get<string>());
				}
				gdlg.IsChat = true;
			}
			else
				gdlg.IsChat = false;

			if (gdlg.IsChat == true)
				DlgsChats[DialogID] = gdlg;
			else
				Dlgs[DialogID] = gdlg;
		}
		ParsingOffset += (int)items.size();
	} while (ParsingOffset < ParsingCount);
	Dlgs.sort(MessageDataSort);
	DlgsChats.sort(MessageDataSort);

	for (auto& i : DlgsChats)
		Dlgs[i.get_key()] = i.get_value();

	list<DocDownloader> ImagesPage;
	if (needAlbumsAndDocs) {
		PrintMessage(string((const char*)u8"Обработка Альбомов ") + (const char*)u8"Пользователя " + wchar_to_UTF8(UserName) + ": id" + to_string(user_id) + "...", URGB(255, 255, 255));
		PrintMessage(" ", URGB(255, 255, 255));

		ParsingCount = 0;
		ParsingOffset = 0;

		VkReq["photos.getAlbums"] << VKI("need_system", 1);
		VKAPI_ANSWER Alboms = VkReq();
		do
		{
			if (Alboms.IsError == true)
				break;
			json albinf = Alboms.Object.at("response").get<json>();
			if (albinf.find("items") == albinf.end())
				break;
			albinf = albinf.at("items").get<json>();
			for (json::iterator italb = albinf.begin(); italb != albinf.end(); ++italb)
			{
				json albval = italb.value();
				ParsingCount = 0;
				ParsingOffset = 0;
				do
				{
					VkReq["photos.get"] << VKI("count", 1000) << VKI("offset", ParsingOffset) << VKI("rev", 1) << VKI("album_id", albval.at("id").get<int>());
					VKAPI_ANSWER DWN = VkReq();
					if (DWN.IsError == true)
						break;
					json info = DWN.Object.at("response").get<json>();
					if (ParsingCount == 0)
						ParsingCount = info.at("count").get<int>();
					json items = info.at("items").get<json>();
					for (json::iterator it = items.begin(); it != items.end(); ++it)
					{
						json ite = it.value();
						wstring ResDt;
						ResDt = GetTimeAT(ite.at("date").get<int64_t>());
						DocDownloader dwnldr;
						dwnldr.Type = "photo";
						dwnldr.MultiLink = GetPhotoURL(ite);
						if (dwnldr.MultiLink.size() <= 0)
							continue;
						dwnldr.FileName = (const char*)u8"Альбомы/" + wchar_to_UTF8(FixFileName(UTF8_to_wchar(albval.at("title").get<string>()))) + "/" + wchar_to_UTF8(FixFileName(ResDt + L".jpg"));
						dwnldr.CreatedTime = ite.at("date").get<int64_t>();
						dwnldr.DialogTime = -1;
						dwnldr.DialogID = -1;
						dwnldr.ConversationID = -1;
						dwnldr.UID = to_string(ite.at("owner_id").get<int64_t>()) + "_" + to_string(ite.at("id").get<int>());
						ImagesPage.push_back(dwnldr);
					}
					ParsingOffset += (int)items.size();
				} while (ParsingOffset < ParsingCount);
			}
		} while (0);
		do
		{
			VkReq["photos.getUserPhotos"] << VKI("count", 1000) << VKI("offset", ParsingOffset) << VKI("sort", 0);
			VKAPI_ANSWER DWN = VkReq();
			if (DWN.IsError == true)
				break;
			json info = DWN.Object.at("response").get<json>();
			if (ParsingCount == 0)
				ParsingCount = info.at("count").get<int>();
			json items = info.at("items").get<json>();
			for (json::iterator it = items.begin(); it != items.end(); ++it)
			{
				json ite = it.value();
				wstring ResDt;
				ResDt = GetTimeAT(ite.at("date").get<int64_t>());
				DocDownloader dwnldr;
				dwnldr.Type = "photo";
				dwnldr.MultiLink = GetPhotoURL(ite);
				if (dwnldr.MultiLink.size() <= 0)
					continue;
				dwnldr.FileName = (const char*)u8"Фото с пользователем/" + wchar_to_UTF8(FixFileName(ResDt + L".jpg"));
				dwnldr.CreatedTime = ite.at("date").get<int64_t>();
				dwnldr.DialogTime = -1;
				dwnldr.DialogID = -1;
				dwnldr.ConversationID = -1;
				dwnldr.UID = to_string(ite.at("owner_id").get<int64_t>()) + "_" + to_string(ite.at("id").get<int>());
				ImagesPage.push_back(dwnldr);
			}
			ParsingOffset += (int)items.size();
		} while (ParsingOffset < ParsingCount);

		do
		{
			VkReq["docs.get"] << VKI("type", 4) << VKI("count", 2000);
			VKAPI_ANSWER DWN = VkReq();
			if (DWN.IsError == true)
				break;
			json info = DWN.Object.at("response").get<json>();
			json items = info.at("items").get<json>();
			for (json::iterator it = items.begin(); it != items.end(); ++it)
			{
				json ite = it.value();

				string DocumentUrl = ite.at("url").get<string>();
				if (DocumentUrl.length() <= 0)
					continue;

				DocDownloader dwnldr;
				dwnldr.Type = "doc";
				dwnldr.SingleLink = DocumentUrl;
				dwnldr.FileName = (const char*)u8"Документы/" + wchar_to_UTF8(FixFileName(UTF8_to_wchar(FixFileExt(ite.at("title").get<string>(), ite.at("ext").get<string>()))));
				dwnldr.CreatedTime = ite.at("date").get<int64_t>();
				dwnldr.DialogTime = -1;
				dwnldr.DialogID = -1;
				dwnldr.ConversationID = -1;
				dwnldr.UID = to_string(ite.at("owner_id").get<int64_t>()) + "_" + to_string(ite.at("id").get<int>());
				ImagesPage.push_back(dwnldr);
			}
		} while (0);
	}

	PrintMessage(string((const char*)u8"Обработка Диалогов : ") + to_string(Dlgs.size()) + (const char*)u8" Пользователя " + wchar_to_UTF8(UserName) + ": id" + to_string(user_id) + "...", URGB(255, 255, 255));
	PrintMessage(" ", URGB(255, 255, 255));

	string JsonResultDialog = "";
	size_t DialogsIterators = 0;
	for (Map::Pair<int64_t, VKDialog>& i : Dlgs)
	{
		if (i.get_key() == 100)
		{
			PrintMessage((const char*)u8"       Пропуск диалога с Администрацией ВК...", URGB(0, 128, 255));
			continue;
		}
		if (VkReq.IsDisabled() == true)
			break;
		DialogsIterators++;
		if (i.get_value().IsChat == false)
		{
			if (UsersDataContent.exist(i.get_key()) == false)
				UsersDataContent[i.get_key()] = GetUserNameById(VkReq, i.get_key());
			UserInfo Usnm = UsersDataContent.at(i.get_key());
			i.get_value().Title = Usnm.pUserName;
		}
		json JsonDialogData = json(json::value_t::array);
		UTF8Stream DialogData;
		DialogData << "{\"type\":\"" << (i.get_value().IsChat == false ? "dialog" : "chat") << "\", \"" << (i.get_value().IsChat == false ? "dialog" : "chat") << "\": ";
		bool HasError = false;

		ParsingCount = 0;
		ParsingOffset = 0;
		time_t dialog_time;
		ParseHistorySingle(VkReq, ParsingOffset, ParsingCount, HasError, DialogsIterators, UsersDataContent, JsonDialogData, i, dialog_time);
		if (JsonDialogData.size() <= 0) {
			PrintMessage((const char*)u8"       Пропуск диалога...", URGB(0, 128, 255));
			continue;
		}

		if (ParsingOffset >= ParsingCount && ParsingCount != 0)
		{
			RemoveLastLine();
			PrintMessage((i.get_value().IsChat == false ? string((const char*)u8"Скачивается диалог с ") : string((const char*)u8"Скачивается беседа ")) + to_string(DialogsIterators) + " \"" + wchar_to_UTF8(i.get_value().Title) + (const char*)u8"\" сообщений " + to_string(ParsingCount) + "...", URGB(255, 255, 255));
		}

		UserInfo UsnmJSon;
		if (i.get_value().IsChat == true)
			UsnmJSon = GetChatNameById(VkReq, i.get_key());
		else
		{
			if (UsersDataContent.exist(i.get_key()) == false)
				UsersDataContent[i.get_key()] = GetUserNameById(VkReq, i.get_key());
			UsnmJSon = UsersDataContent.at(i.get_key());
		}
		DialogData << JsonDialogData.dump(4);
		DialogData << ", \"vk_api_version\": {\"float\":" << VKAPI_VERSION << ", \"string\":\"" << VKAPI_VERSION << "\" }, \"page_id\":" << i.get_key() << ", \"page_title\":\"" << UsnmJSon.pUserName << "\", \"page_avatar\":\"" << UsnmJSon.pAvatarLink << "\", \"page_phone_number\":\"" << UsnmJSon.pPhoneNumber << "\", \"page_instagram\":\"" << UsnmJSon.pInstagram << "\", \"page_site\":\"" << UsnmJSon.pSite << "\" }";

		FlBin JSonDialogData;
		JSonDialogData.wopen(Path + L"\\" + L"json\\" + FixFileName(i.get_value().Title + (i.get_value().IsChat == false ? L" (Диалог)" : L" (Беседа)") + L".json"), OPENType::OPENWrite, true);

		(JSonDialogData << UTF8START).BWrite(DialogData.GetString().data(), 1, DialogData.GetString().length());
		JSonDialogData.Close();

		if (HasError == false)
		{
			ColoredMessage msg = getLastLine();
			RemoveLastLine();
			PrintMessage((i.get_value().IsChat == false ? string((const char*)u8"Скачивается диалог с ") : string((const char*)u8"Скачивается беседа ")) + to_string(DialogsIterators) + " \"" + wchar_to_UTF8(i.get_value().Title) + (const char*)u8"\" [Сборка HTML...]", URGB(0, 150, 255));

			BuildHTMLDialog(L"html/" + FixFileName(i.get_value().Title + (i.get_value().IsChat == false ? L" (Диалог)" : L" (Беседа)")) + L"/id" + to_wstring(i.get_key()) + L"/" + FixFileName(i.get_value().Title + (i.get_value().IsChat == false ? L" (Диалог)" : L" (Беседа)")), dialog_time, Path, VkReq, DialogData.GetString(), ANDROID_USERAGENT, UsersDataContent, i.get_value(), i.get_key(), DocDownloads, avatarsBase64);
			RemoveLastLine();
			PrintMessage(wchar_to_UTF8(msg.str), msg.Color);
		}
	}
	if (DocDownloads.size() > 0)
	{
		string Header = ("{\"user_name\":\"" + wchar_to_UTF8(UserName) + "\", \"time_to_sync\":" + to_string(SyncDataTime) + ", \"attachments\":[");
		FlBin Dknt;
		Dknt.wopen(Path + L"\\" + L"Вложения из диалогов.json.attachments", OPENType::OPENWrite, true);
		Dknt << UTF8START;
		Dknt << Header << ENDL;
		bool FirstContent = true;
		for (auto& ati : DocDownloads)
		{
			if (ati.Type != "photo" && ati.Type != "doc" && ati.Type != "video")
				continue;
			if (FirstContent == true)
				FirstContent = false;
			else
				Dknt << ", ";
			Dknt << "{";
			if (ati.Type == "photo" || ati.Type == "doc")
			{
				Dknt << "\"type\":\"" << ati.Type << "\", ";
				Dknt << "\"uid\":\"" << ati.UID << "\", ";
				if (ati.Type == "doc")
					Dknt << "\"url\":\"" << ati.SingleLink << "\", ";
				else
				{
					Dknt << "\"urls\":{";
					bool FGData = true;
					for (auto& ost : ati.MultiLink)
					{
						if (FGData == true)
							FGData = false;
						else
							Dknt << ", ";
						Dknt << "\"" + ost.get_key() + "\":\"" << ost.get_value() << "\"";
					}
					Dknt << "}, ";
				}
				Dknt << "\"dir\":\"" << FixJSonSlashs(ati.Dir) << "\", ";
				Dknt << "\"video_title\":\"" << FixFileName(ati.Video_Title) << "\", ";
				Dknt << "\"file_name\":\"" << FixJSonSlashs(ati.FileName) << "\", ";
				Dknt << "\"created_time\":" << to_string(ati.CreatedTime) << ", ";
				Dknt << "\"dialog_time\":" << to_string(ati.DialogTime) << ", ";
				Dknt << "\"conversation_id\":" << to_string(ati.ConversationID) << ", ";
				Dknt << "\"dialog_id\":" << to_string(ati.DialogID);
			}
			else if (ati.Type == "video")
			{
				Dknt << "\"type\":\"" << ati.Type << "\", ";
				Dknt << "\"uid\":\"" << ati.UID << "\", ";
				Dknt << "\"preview\":\"" << ati.Preview << "\", ";
				Dknt << "\"urls\":{";
				bool FGData = true;
				for (auto& ost : ati.MultiLink)
				{
					if (FGData == true)
						FGData = false;
					else
						Dknt << ", ";
					Dknt << "\"" + ost.get_key() + "\":\"" << ost.get_value() << "\"";
				}
				Dknt << "}, ";
				Dknt << "\"dir\":\"" << FixJSonSlashs(ati.Dir) << "\", ";
				Dknt << "\"video_title\":\"" << FixFileName(ati.Video_Title) << "\", ";
				Dknt << "\"created_time\":" << to_string(ati.CreatedTime) << ", ";
				Dknt << "\"dialog_time\":" << to_string(ati.DialogTime) << ", ";
				Dknt << "\"conversation_id\":" << to_string(ati.ConversationID) << ", ";
				Dknt << "\"dialog_id\":" << to_string(ati.DialogID);
			}
			Dknt << "}" << ENDL;
		}
		Dknt << "]}";
		Dknt.Close();
	}

	if (needCheckLastMessages) {
		FILE* fl = NULL;
		if (_wfopen_s(&fl, last_messages.c_str(), L"wb") == 0)
		{
			fwrite(UTF8START, 1, strlen(UTF8START), fl);
			stringstream jsonout;
			jsonout << "[";
			bool isFirst = true;
			for (auto& i : CurrentLastMessages)
			{
				if (isFirst == true)
					isFirst = false;
				else
					jsonout << ",";
				jsonout << "{ \"peer_id\": " << i.get_key() << ", \"last_message_id\": " << i.get_value() << " }\r\n";
			}
			jsonout << "]";
			fwrite(jsonout.str().c_str(), 1, jsonout.str().length(), fl);
			fclose(fl);
		}
	}

	if (needAlbumsAndDocs && ImagesPage.size() > 0 && !VkReq.IsDisabled())
	{
		PrintDownload(string((const char*)u8"Скачивание фото из альбомов: количество ") + to_string(ImagesPage.size()), URGB(255, 255, 255));
		PrintDownload(" ", URGB(255, 255, 255));
		size_t PhotosAlbomsIterators = 0;
		for (auto& i : ImagesPage)
		{
			if (VkReq.IsDisabled())
				break;
			if (i.Type != "photo" && i.Type != "doc")
				continue;
			PhotosAlbomsIterators++;
			if (i.Type == "photo")
			{
				for (auto& szs : i.MultiLink)
				{
					string Data;
					int Size = DoCurlGet(szs.get_value(), ANDROID_USERAGENT, Data, false);
					if (Size > 0 && szs.get_value().length() > 0)
					{
						FlBin Alb;
						Alb.wopen(Path + L"\\" + UTF8_to_wchar(i.FileName), OPENType::OPENWrite, true);
						Alb.BWrite(Data.data(), 1, Size);
						Alb.Close();
						PrintDownload(string((const char*)u8"Успешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"" + ":[" + szs.get_key() + "]", URGB(255, 255, 255));
						break;
					}
					else
					{
						PrintDownload(string((const char*)u8"Неуспешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"" + ":[" + szs.get_key() + "]", URGB(255, 0, 0));
						PrintDownload(string("       ") + szs.get_value(), URGB(255, 0, 0));
					}
				}
			}
			else
			{
				string Data;
				int Size = DoCurlGet(i.SingleLink, ANDROID_USERAGENT, Data, false);
				if (Size > 0 && i.SingleLink.length() > 0)
				{
					FlBin Alb;
					Alb.wopen(Path + L"\\" + UTF8_to_wchar(i.FileName), OPENType::OPENWrite, true);
					Alb.BWrite(Data.data(), 1, Size);
					Alb.Close();
					PrintDownload(string((const char*)u8"Успешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"", URGB(255, 255, 255));
				}
				else
				{
					PrintDownload(string((const char*)u8"Неуспешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"", URGB(255, 0, 0));
					PrintDownload(string("       ") + i.SingleLink, URGB(255, 0, 0));
				}
			}
		}
	}
	if (DocDownloads.size() > 0 && !VkReq.IsDisabled())
	{
		PrintDownload(string((const char*)u8"Скачивание фото из переписок: количество ") + to_string(DocDownloads.size()), URGB(255, 255, 255));
		PrintDownload(" ", URGB(255, 255, 255));
		size_t PhotosAlbomsIterators = 0;
		for (auto& i : DocDownloads)
		{
			if (VkReq.IsDisabled())
				break;
			if (i.Type != "photo" && i.Type != "doc")
				continue;
			PhotosAlbomsIterators++;
			if (i.Type == "photo")
			{
				for (auto& szs : i.MultiLink)
				{
					string Data;
					int Size = DoCurlGet(szs.get_value(), ANDROID_USERAGENT, Data, false);
					if (Size > 0 && szs.get_value().length() > 0)
					{
						FlBin Alb;
						Alb.wopen(Path + L"\\" + UTF8_to_wchar(i.Dir) + L"\\" + UTF8_to_wchar(i.FileName), OPENType::OPENWrite, true);
						Alb.BWrite(Data.data(), 1, Size);
						Alb.Close();
						PrintDownload(string((const char*)u8"Успешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"" + ":[" + szs.get_key() + "]", URGB(255, 255, 255));
						break;
					}
					else
					{
						PrintDownload(string((const char*)u8"Неуспешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"" + ":[" + szs.get_key() + "]", URGB(255, 0, 0));
						PrintDownload(string("       ") + szs.get_value(), URGB(255, 0, 0));
					}
				}
			}
			else
			{
				string Data;
				int Size = DoCurlGet(i.SingleLink, ANDROID_USERAGENT, Data, false);
				if (Size > 0 && i.SingleLink.length() > 0)
				{
					FlBin Alb;
					Alb.wopen(Path + L"\\" + UTF8_to_wchar(i.Dir) + L"\\" + UTF8_to_wchar(i.FileName), OPENType::OPENWrite, true);
					Alb.BWrite(Data.data(), 1, Size);
					Alb.Close();
					PrintDownload(string((const char*)u8"Успешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"", URGB(255, 255, 255));
				}
				else
				{
					PrintDownload(string((const char*)u8"Неуспешно : ") + to_string(PhotosAlbomsIterators) + " \"" + i.FileName + "\"", URGB(255, 0, 0));
					PrintDownload(string("       ") + i.SingleLink, URGB(255, 0, 0));
				}
			}
		}
	}
	if (VkReq.IsDisabled())
		PrintMessage("Отменено пользователем!", URGB(255, 0, 0));
	else
		PrintMessage("Завершено.", URGB(0, 255, 0));
}

void VKAPI_TOOLS_DumpMessages(const string &Token, const UsesOptionInMethodResult &Options)
{
	needCheckLastMessages = Options.Option2;
	DownloadDialog(Token, Options.Option1, Options.Option3);
}