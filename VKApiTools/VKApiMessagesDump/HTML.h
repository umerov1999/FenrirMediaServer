#pragma once
#include <iostream>
#include <vector>
#include <list>
#include "WSTRUtils.h"
#include "UsrInf.h"
#include "Map.h"

class STRReplacer
{
public:
	STRReplacer() {
	}
	STRReplacer(const std::string &Data) {
		BackupContent = Data;
		Content = Data;
	}
	void operator=(const std::string &Data) {
		BackupContent = Data;
		Content = Data;
	}
	std::string& operator()() {
		return Content;
	}
	STRReplacer& Param(const std::string &ParamName, const std::wstring &Data, bool Loop = false) {
		Param(ParamName, WSTRUtils::wchar_to_UTF8(Data), Loop);
		return *this;
	}
	STRReplacer& Param(const std::string &ParamName, const std::string &Data, bool Loop = false) {
		do
		{
			size_t in = Content.find(ParamName);
			if (in == std::string::npos)
				return *this;
			if (Data.length() > 0)
				Content.replace(in, ParamName.length(), Data);
			else
				Content.erase(in, ParamName.length());
		} while (Loop);
		return *this;
	}
	template<typename... T>
	STRReplacer& finalise_params(const T&... args)
	{
		for (auto&& p : std::initializer_list<std::string>{ args... })
			Param(p, "");
		return *this;
	}
	STRReplacer& reset()
	{
		Content = BackupContent;
		return *this;
	}
private:
	std::string Content;
	std::string BackupContent;
};

class HTML_PAGE
{
public:
	HTML_PAGE(const std::string &UserAgent);

	std::string build_html();
	void add_user_avatar(int64_t avatar_id, UserInfo& user_info, bool DownloadAvatar);
	void add_page_info(UserInfo& user_info, bool DownloadAvatar, const std::string &dialog_name, const std::string &page_link, const std::string &phone_number, const std::string &instagram, const std::string &site, int PartNum);
	void add_message(const std::string &message);
	bool exist_avatar(int64_t avatar_id)
	{
		for (auto& i : Avatars)
		{
			if (i == avatar_id)
				return true;
		}
		return false;
	}

private:
	STRReplacer main_html;
	std::string User_Agent;
	STRReplacer user_avatars;
	std::list<std::string> main_messages;
	std::vector<int64_t>Avatars;
};

#define HTML_ATTACHMENT_FLAG_PHOTO 0x01U
#define HTML_ATTACHMENT_FLAG_VIDEO 0x02U
#define HTML_ATTACHMENT_FLAG_DOC 0x04U
#define HTML_ATTACHMENT_FLAG_WALL 0x08U
#define HTML_ATTACHMENT_FLAG_ALBUM 0x10U
#define HTML_ATTACHMENT_FLAG_LINK 0x20U
#define HTML_ATTACHMENT_FLAG_UNKNOW 0x40U

class HTML_MESSAGE
{
public:
	HTML_MESSAGE();
	void set_is_sub_message();
	void add_attachment_header();
	void add_attacment_image(const std::string &original, const std::string &little);
	void add_attacment_video(const std::string &preview, const std::string &title, const Map::Map<std::string, std::string> &video_links);
	void set_message(bool IsYouMessage, const std::string &utf8_message, const std::string &page_link, int64_t avatar_id, const std::string &user_name, const std::string &date_time);
	std::string build_message();
	void SetHasAttachment(int Flag)
	{
		AttachmentsFlag |= Flag;
	}
	STRReplacer main_message;
	STRReplacer sub_content;

	int AttachmentsFlag;
private:
	bool IsSubMessage;
};

class HTML_MESSAGE_WITH_SUB
{
public:
	HTML_MESSAGE& add_sub_message();
	std::string build_message();

	HTML_MESSAGE main_html_message;
private:
	std::list<HTML_MESSAGE> sub_messages;
};