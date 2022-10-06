#pragma once
#include <iostream>
#include <vector>
#include <ctime>
#include "Map.h"
#include "WSTRUtils.h"
#define SERVER_NAME "Fenrir"
#define SOCKTIMEOUT 25
#define POST_BODY_MAX 200 * 1024 * 1024
//#define CAPCHA_METHOD_NAME L"messages.send"
//#define SUPPORT_MULTIPART 1
//#define PRINT_SSL_ACCEPT_ERROR 1
//#define PRINT_REDIRECT 1

#define minu(a,b)(((a) < (b)) ? (a) : (b))
#define maxu(a,b)(((a) > (b)) ? (a) : (b))

#define UTF8START "\xef\xbb\xbf"
#define ENDL "\r\n"
#define WENDL L"\r\n"

static std::wstring GetAppName()
{
	const wchar_t* FnS = L"\\/";
	TCHAR szFileName[MAX_PATH];
	GetModuleFileNameW(0, szFileName, MAX_PATH);

	std::wstring fname = szFileName;
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(pos + 1);
}
#define SETTINGS (GetAppName() + L".settings.json")

struct InitServer
{
	int HttpsPort;
};

#define USSL_CERTPART_CERT 0
#define USSL_CERTPART_BUNDLE 1
#define USSL_CERTPART_KEY 2

struct USSL_CERT
{
	USSL_CERT()
	{
		Inited = false;
	}
	bool Inited;
	std::string Name;
	std::string AddTime;
	Map::Map<int, std::string>CertPart;
};

struct ACCEPT_STRUCT
{
	std::string connection;
	void* ssl_ctx;
	SOCKET client_sock;
};

class wstringparam
{
public:
	void add(const std::string& name, const std::wstring& value);
	void add_decode(const std::string& name, const std::string& value);
	void add(const std::string& name, const std::string& value);
	int count() const;
	void parse(const std::string& paramstring, const std::string& separator, bool need_url_decode);
	void parse(const std::wstring& paramstring, const std::string& separator);
	bool exist(const std::string& name);
	void erase(const std::string& name);
	std::wstring operator[](const std::string& name) const;
	std::string get_utf8(const std::string& name) const;
	void set_utf8(const std::string& name, const std::string& value);
public:
	Map::Map<std::string, std::wstring> paramsdata;
};

class multipart
{
public:
	struct multipart_data
	{
		multipart_data() {

		}
		multipart_data(const std::string& filename, const std::string& data) {
			this->filename = filename;
			this->data = data;
		}

		bool isFile() const {
			return filename.length() > 0;
		}

		std::wstring get_Wfilename() {
			return WSTRUtils::UTF8_to_wchar(filename);
		}

		std::wstring get_Wvalue() {
			return WSTRUtils::UTF8_to_wchar(data);
		}

		std::string filename;
		std::string data;
	};

	int count() const {
		return (int)paramsdata.size();
	}
	bool exist(const std::string &name) const {
		return paramsdata.exist(name);
	}
	void erase(const std::string &name) {
		paramsdata.erase(name);
	}
	const multipart_data& get_data(const std::string &name) const {
		return paramsdata[name];
	}

	size_t parse(const std::string& body, size_t offset, const std::string& boundary);
public:
	Map::Map<std::string, multipart_data> paramsdata;
};

enum class HTTP_METHOD
{
	HTTP_METHOD_GET,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_CONNECT,
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_PATCH,
	HTTP_METHOD_UNKNOW
};

enum class TARGET_DEVICES
{
	DEVICE_ANDROID,
	DEVICE_IPHONE,
	DEVICE_IPAD,
	DEVICE_WINDOWS,
	DEVICE_WINDOWS_PHONE,
	DEVICE_UNKNOW,
};
void InitVK_Reverser();

struct RequestParserStruct
{
	TARGET_DEVICES Device;
	HTTP_METHOD method;
	std::string connection;
	wstringparam cookies;
	wstringparam http_post_param;
	wstringparam http_get_param;
	wstringparam http_request;
	multipart multi_part;
};

class UTF8Stream
{
public:
	UTF8Stream & operator<<(const std::wstring &String);
	UTF8Stream &operator<<(const std::string &String);
	UTF8Stream &operator<<(size_t value);
	UTF8Stream &operator<<(int value);
	const std::string &GetString() const;
	int GetSize();
	void Clear();
private:
	std::string Data;
};