#pragma once
#include <iostream>

class UserInfo
{
public:
	UserInfo()
	{
		user_id = 0;
		pSuccess = false;
	}
	UserInfo(int id, const std::wstring &UserName, const std::string &AvatarLink, const std::string &PhoneNumber, const std::string &Instagram, const std::string &Site, bool Success)
	{
		user_id = id;
		pUserName = UserName;
		pAvatarLink = AvatarLink;
		pSuccess = Success;
		pPhoneNumber = PhoneNumber;
		pInstagram = Instagram;
		pSite = Site;
	}
	int user_id;
	std::wstring pUserName;
	std::string pAvatarLink;
	std::string pPhoneNumber;
	std::string pInstagram;
	std::string pSite;
	std::string pAvatarBase64Data;
	std::string pAvatarBase64Type;
	bool pSuccess;
};