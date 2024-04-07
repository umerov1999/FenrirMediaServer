#pragma once
#include <iostream>
#include <list>
#include <cstring>
#include <cstdint>
#include "WSTRUtils.h"
#include "do_curl.h"
#include "json.hpp"

class VKAPI_ANSWER
{
public:
	VKAPI_ANSWER() {
		IsError = false;
	}
	std::string Request;
	std::string PostParams;
	nlohmann::json Object;
	bool IsError;
};

class VK_ApiMethodParam
{
public:
	VK_ApiMethodParam(const std::string &tname, const std::string &tvalue, bool tis_string)
	{
		name = tname;
		value = tvalue;
		is_string = tis_string;
	}
	std::string name;
	std::string value;
	bool is_string;
};

static VK_ApiMethodParam VK(const std::string &name, const std::string &value, bool urlencode = true)
{
	return VK_ApiMethodParam(urlencode ? WSTRUtils::url_encode(name) : name, urlencode ? WSTRUtils::url_encode(value) : value, true);
}
template <typename T>
static VK_ApiMethodParam VKI(const std::string &name, T value, bool urlencode = true)
{
	return VK_ApiMethodParam(urlencode ? WSTRUtils::url_encode(name) : name, urlencode ? WSTRUtils::url_encode(std::to_string(value)) : std::to_string(value), false);
}

class HP
{
public:
	HP(const std::string &TName, const std::string &TParam)
	{
		Name = WSTRUtils::url_encode(TName);
		Param = WSTRUtils::url_encode(TParam);
	}
	HP(const std::string &TName, int32_t TParam)
	{
		Name = WSTRUtils::url_encode(TName);
		Param = WSTRUtils::url_encode(std::to_string(TParam));
	}
	HP(const std::string& TName, int64_t TParam)
	{
		Name = WSTRUtils::url_encode(TName);
		Param = WSTRUtils::url_encode(std::to_string(TParam));
	}
	std::string Name;
	std::string Param;
};

class HTTPParams
{
public:
	template<typename ...Args>
	static constexpr std::size_t va_count(const Args...) { return sizeof...(Args); }
	static HTTPParams make_param(size_t count, ...)
	{
		HTTPParams Ret;
		if (count <= 0)
			return Ret;
		va_list args;
		va_start(args, count);
		for (size_t i = 0; i < count; i++)
		{
			HP pp = va_arg(args, HP);
			Ret.Params.push_back(pp);
		}
		va_end(args);
		return Ret;
	}
#define HTTP_PARAMS(...) HTTPParams::make_param(HTTPParams::va_count(__VA_ARGS__), __VA_ARGS__)
	std::list<HP> Params;
};

class VK_APIMETHOD
{
public:
	static int VKHasError(const nlohmann::json& jres, bool IgnoreExecuteErrors = false)
	{
		if (jres.size() == 0 || jres.is_null() == true)
			return -1;
		if (!IgnoreExecuteErrors && jres.find("execute_errors") != jres.end())
			return -1;
		if (jres.find("error") == jres.end())
			return 0;
		nlohmann::json uot = jres.at("error").get<nlohmann::json>();
		if (uot.find("error_code") == uot.end())
			return -1;
		return uot.at("error_code").get<int>();
	}
	VK_APIMETHOD(const std::string &Access_Token, const std::string &IUserAgent)
	{
		Token = Access_Token;
		UserAgent = IUserAgent;
		Disabled = false;
		IgnoreExecuteErrors = false;
	}
	VK_APIMETHOD& operator<<(const HTTPParams& ParamU)
	{
		for (auto& i : ParamU.Params)
		{
			VK_ApiMethodParam pp(i.Name, i.Param, true);
			Params.push_back(pp);
		}
		return *this;
	}
	VK_APIMETHOD& operator<<(const VK_ApiMethodParam &ParamU)
	{
		Params.push_back(ParamU);
		return *this;
	}
	VK_APIMETHOD& operator[](const std::string &LMethod)
	{
		Method = LMethod;
		Params.clear();
		return *this;
	}
	std::string GetExecuteMethod()
	{
		std::string Ret = "API." + Method + "({";
		std::list<VK_ApiMethodParam> TempParams = Params;
		TempParams.push_back(VK("v", VKAPI_VERSION));
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
		std::list<VK_ApiMethodParam> TempParams = Params;
		TempParams.push_back(VK("access_token", Token));
		TempParams.push_back(VK("lang", "ru"));
		TempParams.push_back(VK("v", VKAPI_VERSION));

		std::string ResultLink = std::string("https://api.vk.com/method") + "/" + Method;
		std::string PostParam;
		bool FirstParam = true;
		for (auto& i : TempParams)
		{
			if (FirstParam == true)
				FirstParam = false;
			else
				PostParam += "&";
			PostParam += (i.name + "=" + i.value);
		}

		std::string BackupPostParam = PostParam;
		ret.Request = ResultLink;
		ret.PostParams = PostParam;
		do
		{
			if (errorcode == 6 || errorcode == 1 || errorcode == 9 || errorcode == 10)
				Sleep(1100);

			std::string Rsj;
			if (DoCurlPost(ResultLink, PostParam, UserAgent, Rsj, true) <= 0)
				return ret;

			try {
				ret.Object = nlohmann::json::parse(Rsj);
			}
			catch (...) {
				return ret;
			}
			errorcode = VKHasError(ret.Object, IgnoreExecuteErrors);
			if (errorcode == 0)
				ret.IsError = false;
			else if (errorcode == 14)
			{
				if (PostParam.find("captcha_sid=") != std::string::npos)
					PostParam = BackupPostParam;
				nlohmann::json info = ret.Object.at("error").get<nlohmann::json>();

				std::string code_capcha;
				if (VK_APIMETHOD_doCapcha(info.at("captcha_img").get<std::string>(), UserAgent, code_capcha)) {
					PostParam += ("&captcha_sid=" + WSTRUtils::url_encode(info.at("captcha_sid").get<std::string>()) + "&captcha_key=" + WSTRUtils::url_encode(code_capcha));
				}
			}
			else if (errorcode == 29) {
				VK_APIMETHOD_doLimit(Method);
			}
		} while (errorcode == 6 || errorcode == 14 || errorcode == 1 || errorcode == 9 || errorcode == 10);
		return ret;
	}

	bool VK_APIMETHOD_doCapcha(const std::string& captcha_img, const std::string& user_agent, std::string& code);
	void VK_APIMETHOD_doLimit(const std::string& Method);

	void Disable()
	{
		Disabled = true;
	}
	void setIgnoreExecuteErrors()
	{
		IgnoreExecuteErrors = true;
	}
	bool IsDisabled() const
	{
		return Disabled;
	}
private:
	std::list<VK_ApiMethodParam> Params;
	std::string Method;
	std::string Token;
	std::string UserAgent;
	bool Disabled;
	bool IgnoreExecuteErrors;
};