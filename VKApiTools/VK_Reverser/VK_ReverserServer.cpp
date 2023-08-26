#include "pch.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Map.h"
#include <regex>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "do_curl.h"
#include "FlBin.h"
#include "resource.h"
#include "VK_ReverserServer.h"
#include "VK_ReverserDialog.h"
#include <io.h>
#include <fcntl.h>
#include "sha3.hpp"
#include "json.hpp"
#include "libimage.h"
#include "WSTRUtils.h"
#include "win_api_utils.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;

SOCKET HTTPSserver_sock = 0;
USSL_CERT CertificateData;

const char* Capcha = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d\x49\x48\x44\x52"
"\x00\x00\x00\xe6\x00\x00\x00\x4b\x08\x03\x00\x00\x00\x53\x36\x0b"
"\xb9\x00\x00\x00\x01\x73\x52\x47\x42\x00\xae\xce\x1c\xe9\x00\x00"
"\x00\x04\x67\x41\x4d\x41\x00\x00\xb1\x8f\x0b\xfc\x61\x05\x00\x00"
"\x03\x00\x50\x4c\x54\x45\xff\x00\x00\xff\x02\x02\xff\x04\x04\xff"
"\x05\x05\xff\x06\x06\xff\x08\x08\xff\x0b\x0b\xff\x0c\x0c\xff\x0f"
"\x0f\xff\x10\x10\xff\x13\x13\xff\x14\x14\xff\x15\x15\xff\x16\x16"
"\xff\x17\x17\xff\x18\x18\xff\x1a\x1a\xff\x1b\x1b\xff\x1c\x1c\xff"
"\x1f\x1f\xff\x20\x20\xff\x22\x22\xff\x24\x24\xff\x25\x25\xff\x27"
"\x27\xff\x28\x28\xff\x29\x29\xff\x2a\x2a\xff\x2b\x2b\xff\x2c\x2c"
"\xff\x2e\x2e\xff\x2f\x2f\xff\x30\x30\xff\x31\x31\xff\x33\x33\xff"
"\x34\x34\xff\x36\x36\xff\x38\x38\xff\x3a\x3a\xff\x3c\x3c\xff\x3e"
"\x3e\xff\x3f\x3f\xff\x40\x40\xff\x42\x42\xff\x44\x44\xff\x48\x48"
"\xff\x4b\x4b\xff\x4c\x4c\xff\x4d\x4d\xff\x4f\x4f\xff\x50\x50\xff"
"\x51\x51\xff\x54\x54\xff\x55\x55\xff\x56\x56\xff\x57\x57\xff\x58"
"\x58\xff\x5c\x5c\xff\x60\x60\xff\x61\x61\xff\x64\x64\xff\x67\x67"
"\xff\x68\x68\xff\x69\x69\xff\x6b\x6b\xff\x6c\x6c\xff\x6d\x6d\xff"
"\x6f\x6f\xff\x70\x70\xff\x74\x74\xff\x76\x76\xff\x78\x78\xff\x7a"
"\x7a\xff\x7b\x7b\xff\x7c\x7c\xff\x7e\x7e\xff\x7f\x7f\xff\x80\x80"
"\xff\x83\x83\xff\x85\x85\xff\x87\x87\xff\x88\x88\xff\x89\x89\xff"
"\x8b\x8b\xff\x8c\x8c\xff\x8f\x8f\xff\x90\x90\xff\x93\x93\xff\x94"
"\x94\xff\x97\x97\xff\x9a\x9a\xff\x9b\x9b\xff\x9c\x9c\xff\x9f\x9f"
"\xff\xa1\xa1\xff\xa3\xa3\xff\xa4\xa4\xff\xa5\xa5\xff\xa7\xa7\xff"
"\xa8\xa8\xff\xa9\xa9\xff\xaa\xaa\xff\xab\xab\xff\xaf\xaf\xff\xb1"
"\xb1\xff\xb2\xb2\xff\xb3\xb3\xff\xb5\xb5\xff\xb6\xb6\xff\xb7\xb7"
"\xff\xb8\xb8\xff\xbb\xbb\xff\xbc\xbc\xff\xbf\xbf\xff\xc1\xc1\xff"
"\xc3\xc3\xff\xc5\xc5\xff\xcb\xcb\xff\xcc\xcc\xff\xce\xce\xff\xcf"
"\xcf\xff\xd1\xd1\xff\xd3\xd3\xff\xd4\xd4\xff\xd7\xd7\xff\xd8\xd8"
"\xff\xd9\xd9\xff\xda\xda\xff\xdb\xdb\xff\xdc\xdc\xff\xdd\xdd\xff"
"\xde\xde\xff\xdf\xdf\xff\xe3\xe3\xff\xe6\xe6\xff\xe7\xe7\xff\xe8"
"\xe8\xff\xea\xea\xff\xeb\xeb\xff\xee\xee\xff\xef\xef\xff\xf3\xf3"
"\xff\xf6\xf6\xff\xf7\xf7\xff\xf8\xf8\xff\xfa\xfa\xff\xfb\xfb\xff"
"\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\xe3\xce\x0b\x7b\x00\x00\x00\x09\x70\x48"
"\x59\x73\x00\x00\x0e\xc3\x00\x00\x0e\xc3\x01\xc7\x6f\xa8\x64\x00"
"\x00\x04\xce\x49\x44\x41\x54\x78\x5e\xed\x99\x8d\x53\x1b\x45\x18"
"\x87\x7f\x5e\x4d\x63\x4c\x6b\x34\x46\x34\xd6\x6a\x2a\xa5\x51\x24"
"\xad\xc6\xa8\x80\x34\x4a\x35\x7e\x60\x04\x14\x23\x46\x4c\xeb\x37"
"\x96\x8a\x48\x23\x4a\x2b\x6a\xc5\xa6\x48\x6c\x8c\xde\xfe\xc3\xbe"
"\x7b\xf7\x26\xb9\x00\x53\x1d\x79\x6f\xd8\xb9\xde\x33\x53\xee\xb7"
"\x9b\xeb\x0c\x0f\xbb\xb7\xfb\xde\x06\xea\xb6\x20\xd4\x0c\x12\xa1"
"\x66\x90\x08\x35\x83\x44\xa8\x19\x24\x42\xcd\x20\x11\x6a\x8a\xd0"
"\xe0\xeb\x01\xe3\xa7\x66\xbd\x38\x18\x05\x72\x6d\x6e\x1e\x24\xfe"
"\x69\x36\x46\x13\x85\x42\x0c\x88\x05\x5a\xb3\x1a\xcd\xb4\xe8\x32"
"\x1d\x5d\x70\xdb\x07\x8b\x5f\x9a\x15\xa4\xb6\x9c\xa0\x5d\x0f\x1e"
"\x9f\x34\xab\xc0\x05\x8e\x46\xe0\x8f\xe6\x46\x14\x83\x1c\xcd\xc0"
"\x1f\xcd\x21\x60\x85\xa3\x19\xf8\xa2\xb9\x08\xa4\x39\x1a\x82\x2f"
"\x9a\x19\x60\x8e\xa3\x21\xf8\xa1\x49\x83\x19\x69\x72\x36\x04\x3f"
"\x34\xb3\xc0\x28\x47\x53\xf0\x41\xb3\x01\x60\x91\xf3\xbf\x73\xe5"
"\xe2\x2f\x9c\xfc\xc4\x07\xcd\x0a\xcd\xd9\xff\x58\x14\x34\xca\x09"
"\xfa\x9b\x7c\xcc\x2d\x1f\xf1\x41\x93\xe6\x6c\x9e\xe3\xad\x69\x95"
"\x2c\x92\x04\xca\xdc\xee\xf1\xd1\xb1\x43\x9f\x70\x94\x41\x5e\x53"
"\xcf\xd9\x3d\x2a\xa0\x56\x6e\x9c\x53\x87\xe5\x54\x62\x7a\x79\x9e"
"\xee\xfe\x9c\x3b\x3a\x6c\x3e\x45\x9d\x4b\xdc\x90\x41\x56\xb3\x5e"
"\xab\xd5\x26\xe8\x77\x9c\xaf\xd5\xaa\xe5\x72\x79\xdd\xae\xf3\x07"
"\xca\xce\x03\x57\x39\xbb\xcc\x59\xa3\xf4\xee\x72\x9d\xee\xfe\x89"
"\x7b\x98\xd5\x7b\xa8\xef\xce\xbf\xb9\x25\x83\xa8\xe6\x30\xfd\x7e"
"\x3d\xac\xc2\xfa\xd0\x71\xfe\x44\x95\x76\x3c\xb0\x76\x11\x79\x9b"
"\xae\x17\x80\x97\xdc\x1e\xe2\x0b\xfd\xe3\xd2\x21\xe0\x09\x9c\x76"
"\x3a\xc4\x90\xd4\xa4\xe9\x6a\xa5\xe2\x24\x98\x4c\x65\x47\x5f\xaf"
"\x35\xed\x41\xe0\x9a\xfb\xd1\x56\x04\x18\x71\xa3\xcb\x38\xb2\x8e"
"\xf5\x30\xa6\x9c\xb6\xe6\x59\x6c\x2b\xb5\x72\x07\xee\xbf\x3c\x83"
"\xf7\xb9\x4f\x08\xd1\xd1\xdc\x6a\xd0\x0b\x66\x6f\xd8\xe8\xb9\xeb"
"\xac\x45\x93\x24\xef\xdd\x64\x66\x91\x70\x6f\x5a\xfc\xd5\xb9\x68"
"\xbe\x41\x82\xa6\xfd\x61\x9c\xd8\x54\x8f\xa0\xd7\x2d\x82\xf8\x12"
"\x44\x85\x5e\xc7\x6d\x20\x5e\xe2\x62\x48\x0f\xe6\x80\x9e\xa3\xcc"
"\x92\x85\x0a\xc7\x1e\x93\x78\x5a\xad\xdd\x8d\x93\xdb\xea\x3c\x4e"
"\x71\x9f\x14\xd2\x9a\xd7\x68\xd8\x3a\xeb\x6c\x36\xc6\x41\x4d\x51"
"\x6f\x8d\x33\xd1\x8c\x21\xb1\xfb\xe8\xe4\x01\xcc\xfc\x7c\xaf\xb6"
"\x54\xbf\xe3\x5d\xee\x93\x42\x5a\x73\xb6\x37\x67\x17\x31\xe0\x06"
"\xd5\x8e\xf5\x0f\x66\x11\x7b\x0c\xe6\xf7\x40\xfd\x04\x70\x85\xe2"
"\x0f\x47\x77\xee\x31\xfb\x45\x5a\x93\x6a\x83\x1c\xc7\x5c\xb7\xb2"
"\xa5\xba\xc8\xb3\xd2\xa8\x35\x6b\xaf\x73\xb0\xf7\xe8\x7f\x02\x1f"
"\x70\x4b\x18\x61\x4d\x5d\x1b\x74\xce\xb8\xd2\x98\xe7\x44\x7b\xa6"
"\x77\xd3\x24\x9b\x3d\xca\xa4\x02\xdd\x84\x28\xd2\x37\xb8\x2d\x8a"
"\xb0\x26\x8d\x9b\xd5\x79\x07\xb3\x30\xcb\x29\xd9\xe7\xa5\xff\x14"
"\x55\xce\x1e\xe8\x26\xe0\x74\xdf\xb0\xcb\x21\xac\x39\xe2\xd9\x1e"
"\x81\x92\x1b\x6e\xd2\xef\xbf\xee\x46\x4d\x99\x9a\xab\x9c\x7b\x6c"
"\x6a\xcb\xfb\xee\x7a\x08\x8f\x73\x87\x28\xb2\x9a\x4d\xaa\xc5\x3b"
"\x73\xd6\x06\xb8\x8a\x9d\xf3\x0c\xe6\xea\x58\x21\x4d\x3e\xfd\x75"
"\x9f\x86\xde\xc5\xf1\x98\x52\x1f\x02\x7f\x70\x8f\x24\xb2\x9a\x0b"
"\x9e\x39\x4b\xab\x91\x3b\xb0\x57\x23\xb0\x58\xab\x35\x89\xe3\x0d"
"\xfd\x5a\x72\xdd\x6d\x33\xbf\xd1\xbf\x77\xa8\xf7\xbc\xae\x11\xd0"
"\xad\x83\x05\x91\xd5\xa4\x39\xdb\x7b\x08\xcf\x20\xa3\x2f\x75\x7a"
"\xa5\xcc\x3a\x1d\x6a\x29\x49\x25\xde\x06\xf9\xf4\x6b\x2e\x1c\xfe"
"\x5a\xa9\x17\x81\xa3\x94\xe9\xc1\x15\xae\xf3\x1c\x44\x35\xf5\x40"
"\x7d\xc9\x59\x6b\x26\x69\xcb\x9c\xa6\x02\x08\x71\xdd\x6e\x8e\x01"
"\x85\xb6\x5a\xd7\x9a\x9e\xc2\xef\xc7\x67\x1e\xfc\x96\x2e\xa7\x80"
"\x73\xba\x79\x04\x27\xf5\x45\xa9\xaf\x66\x86\xdc\x20\x81\xa8\x26"
"\xad\xb3\xd1\xde\x86\x48\xa5\x4f\x73\x6e\x00\x48\x7f\x0a\x6c\x28"
"\xbb\x16\x87\xa5\x97\xde\x96\xd6\x9c\x70\x6f\xa1\x32\xf0\x65\x9c"
"\xfd\x4b\x87\x47\x81\xef\xf4\xf5\x39\xe0\x79\xa5\xb6\x3f\x7b\x01"
"\xaf\xe8\xa6\x10\xa2\x9a\x54\x1b\x14\x39\x12\x2b\x54\x10\xd1\xb3"
"\x5a\x6a\xab\x04\xb2\xd3\xe4\x9b\x74\x9f\xba\x54\x77\x38\xed\x95"
"\x89\xc8\xf0\x65\xa7\x93\x4a\x3d\xb7\x34\xac\xd3\x87\xc7\x1e\xa6"
"\x1f\x6f\x3a\x4d\x21\x24\x35\x75\x3d\xbb\xc6\x99\xb0\xf5\x3b\xd9"
"\x88\x5e\x7c\xa8\xb8\x03\x62\xe5\x9b\x6e\x7f\x55\xb7\x90\x1e\xcb"
"\x67\xa2\xd6\x68\xf7\xfe\x23\x9d\xea\xe9\x6d\xe7\xe3\x8c\xb9\xa7"
"\x07\xb4\x21\x76\x5f\xa3\x35\x0b\xf1\x82\xbb\x3f\xda\x93\x89\x33"
"\xd3\x2c\x49\x2d\xa7\xde\x21\x92\x53\xee\x97\x66\x0e\x31\x9c\xe5"
"\x74\xf1\xb5\xb1\xb7\x2e\x71\x96\x42\x52\x93\xe6\x65\xa7\xbc\xbb"
"\x35\xcb\xa5\x7c\xbe\x58\xe9\xdf\x3b\x9f\xc4\x1b\x9c\xfc\x40\x50"
"\x93\x9e\xaa\xf8\xff\xff\x66\xfa\x4f\xd9\xa3\xbc\x1d\x08\x6a\xd2"
"\x7e\xb1\xfb\x28\xd2\x10\xe4\x34\x9b\x11\xc4\xba\xcf\x9f\x69\xc8"
"\x69\x52\xe5\x6a\xec\x60\xca\x69\xb6\x13\x7b\x1d\x7c\x98\x82\x98"
"\x26\x55\x40\x9e\xd3\x1e\xd3\x90\xd2\xa4\xc1\x1c\xf2\x9c\xf6\x98"
"\x86\x94\x66\x05\x51\x3e\x79\x36\x12\x21\xcd\x66\xbc\xfb\x3a\x6d"
"\x24\x32\x9a\x76\x0e\xaf\x72\x34\x13\x11\x4d\xaa\x52\x4d\xfb\x16"
"\x7e\x07\x12\x9a\xad\x02\x26\x0c\x5e\x7e\x34\x02\x9a\xab\x29\x6b"
"\xf7\x19\xba\x61\xec\x5b\x73\xab\x60\x8d\x9b\xbc\xc6\xba\xec\x5f"
"\x73\xd1\xfb\xed\xac\xa9\x88\x2c\x41\xe6\x13\x6a\x06\x89\x50\x33"
"\x48\x84\x9a\x41\x22\xd4\x0c\x12\xa1\x66\x90\xb8\x2d\x34\x95\xfa"
"\x07\x2a\xee\x60\x15\x89\x1f\xc8\xcc\x00\x00\x00\x00\x49\x45\x4e"
"\x44\xae\x42\x60\x82";

extern std::wstring ExtractAppPath();
extern VK_ReverserDialog dlgS;
extern void PrintMessage(const wstring &Msg, URGB Color = URGB(255, 255, 255));
extern const char* HTML_DIALOG_FRAME;
extern const char* HTML_DIALOG_MY_MESSAGE;
extern const char* HTML_DIALOG_TARGET_MESSAGE;

extern CString EnteredAccessToken;

EXTERN_THREAD_ACCESS_GUARD_OBJECT

InitServer Startinit;

struct VKAPI_ANSWER
{
	string Request;
	string PostParams;
	json Object;
	bool IsError;
};

bool IsMobile(TARGET_DEVICES Device)
{
	if (Device == TARGET_DEVICES::DEVICE_ANDROID || Device == TARGET_DEVICES::DEVICE_IPHONE || Device == TARGET_DEVICES::DEVICE_IPAD || Device == TARGET_DEVICES::DEVICE_WINDOWS_PHONE)
		return true;
	return false;
}

void wstringparam::add(const string& name, const wstring& value)
{
	paramsdata[name] = value;
}
void wstringparam::add_decode(const string& name, const string& value)
{
	paramsdata[name] = UTF8_to_wchar(url_decode(value));
}
void wstringparam::add(const string& name, const string& value)
{
	paramsdata[name] = UTF8_to_wchar(value);
}
void wstringparam::parse(const string& paramstr, const string& separator, bool need_url_decode)
{
	string paramstring = paramstr;
	while (!paramstring.empty())
	{
		size_t SepPos = paramstring.find(separator);
		string SubKeys = paramstring.substr(0, SepPos);
		string key = SubKeys.substr(0, SubKeys.find('='));
		string value = SubKeys.substr(SubKeys.find('=') + 1);
		if (need_url_decode)
			value = url_decode(value);
		add(key, UTF8_to_wchar(value));
		if (SepPos == string::npos)
			paramstring.clear();
		else
			paramstring = paramstring.substr(SepPos + separator.length());
	}
}
void wstringparam::parse(const wstring& paramstring, const string& separator)
{
	parse(wchar_to_UTF8(paramstring), separator, false);
}
int wstringparam::count() const
{
	return (int)paramsdata.size();
}
wstring wstringparam::operator[](const string& name) const
{
	if (paramsdata.exist(name) == true)
		return paramsdata.at(name);
	return L"";
}
bool wstringparam::exist(const string& name)
{
	if (paramsdata.exist(name) == true)
		return true;
	return false;
}
void wstringparam::erase(const string& name)
{
	if (paramsdata.exist(name) == true)
		paramsdata.erase(name);
}
string wstringparam::get_utf8(const string& name) const
{
	if (paramsdata.exist(name) == true)
		return wchar_to_UTF8(paramsdata.at(name));
	return "";
}
void wstringparam::set_utf8(const string& name, const string& value)
{
	if (paramsdata.exist(name) == true)
		paramsdata[name] = UTF8_to_wchar(value);
}

static wstring GetDublicates(int count, const wstring &str)
{
	wstring out = L"";
	for (int i = 0; i < count; i++)
		out += str;
	return out;
}

UTF8Stream& UTF8Stream::operator<<(const wstring &String)
{
	if (String.length() > 0)
	{
		string OT = wchar_to_UTF8(String);
		Data += OT;
	}
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(const string &String)
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
const string &UTF8Stream::GetString() const
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

SOCKET tcp_listen(int Port)
{
	SOCKET sock;
	struct sockaddr_in sin;
	const int qlen = 1;
	if ((sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
		return SOCKET_ERROR;
	}

	int tr = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&tr, sizeof(tr));

	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	if (::bind(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
		return SOCKET_ERROR;
	}
	if (::listen(sock, qlen) < 0) {
		return SOCKET_ERROR;
	}
	return sock;
}

void SetTimeOutsSocket(SOCKET Socket)
{
	DWORD timeout = SOCKTIMEOUT * 1000;
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

void InitOpenSSL()
{
	/*
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();
	*/
}

static time_t ASN1_GetTimeT(ASN1_TIME* time) {
	struct tm t;
	const char* str = (const char*)time->data;
	size_t i = 0;

	memset(&t, 0, sizeof(t));

	if (time->type == V_ASN1_UTCTIME) {
		t.tm_year = (str[i++] - '0') * 10;
		t.tm_year += (str[i++] - '0');
		if (t.tm_year < 70)
			t.tm_year += 100;
	}
	else if (time->type == V_ASN1_GENERALIZEDTIME) {
		t.tm_year = (str[i++] - '0') * 1000;
		t.tm_year += (str[i++] - '0') * 100;
		t.tm_year += (str[i++] - '0') * 10;
		t.tm_year += (str[i++] - '0');
		t.tm_year -= 1900;
	}
	t.tm_mon = (str[i++] - '0') * 10;
	t.tm_mon += (str[i++] - '0') - 1;
	t.tm_mday = (str[i++] - '0') * 10;
	t.tm_mday += (str[i++] - '0');
	t.tm_hour = (str[i++] - '0') * 10;
	t.tm_hour += (str[i++] - '0');
	t.tm_min = (str[i++] - '0') * 10;
	t.tm_min += (str[i++] - '0');
	t.tm_sec = (str[i++] - '0') * 10;
	t.tm_sec += (str[i++] - '0');

	return mktime(&t);
}

inline void PrintSertValid(X509* Sert, const wstring& Name)
{
	PrintMessage(L"Сертификат " + Name + L" Действителен с " + GetTimeAT(ASN1_GetTimeT(X509_get_notBefore(Sert))) + L" по " + GetTimeAT(ASN1_GetTimeT(X509_get_notAfter(Sert))), URGB(140, 140, 140));
}

SSL_CTX* initialize_ctx()
{
	auto CA_CERT = std::shared_ptr<BIO>(BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_CERT].data(), (int)CertificateData.CertPart[USSL_CERTPART_CERT].size()), BIO_free);
	auto CA_KEY = std::shared_ptr<BIO>(BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_KEY].data(), (int)CertificateData.CertPart[USSL_CERTPART_KEY].size()), BIO_free);
	SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
	BIO* CA_BUNDLE = NULL;
	X509* bundle = NULL;
	X509* cert = NULL;
	EVP_PKEY* rsa = NULL;
	X509* p = NULL;
	bool first = true;
	int bundle_part = 0;

	if (ctx == NULL)
		goto error;


	while ((p = PEM_read_bio_X509(CA_CERT.get(), NULL, 0, NULL)) != NULL) {
		if (first) {
			if (SSL_CTX_use_certificate(ctx, p) <= 0)
				goto error;
			first = false;
			/*if (print_sert)*/ {
				PrintSertValid(p, L"Certificate.crt");
			}
		}
		else {
			if (SSL_CTX_add_extra_chain_cert(ctx, p) <= 0)
				goto error;
			/*if (print_sert)*/ {
				PrintSertValid(p, L"Ca_Bundle[" + to_wstring(++bundle_part) + L"].crt");
			}
		}
	}
	rsa = PEM_read_bio_PrivateKey(CA_KEY.get(), NULL, 0, NULL);

	if (first || rsa == NULL)
		goto error;
	if (CertificateData.CertPart.exist(USSL_CERTPART_BUNDLE) && !CertificateData.CertPart[USSL_CERTPART_BUNDLE].empty()) {
		CA_BUNDLE = BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_BUNDLE].data(), (int)CertificateData.CertPart[USSL_CERTPART_BUNDLE].size());
		if (!CA_BUNDLE) {
			goto error;
		}
		while ((p = PEM_read_bio_X509(CA_BUNDLE, NULL, 0, NULL)) != NULL) {
			if (SSL_CTX_add_extra_chain_cert(ctx, p) <= 0) {
				BIO_free(CA_BUNDLE);
				goto error;
			}

			/*if (print_sert)*/ {
				PrintSertValid(p, L"Ca_Bundle[" + to_wstring(++bundle_part) + L"].crt");
			}
		}
		BIO_free(CA_BUNDLE);
	}
	if (SSL_CTX_use_PrivateKey(ctx, rsa) <= 0)
		goto error;
	/*if (print_sert)*/ {
		PrintMessage(L"    #Домен " + UTF8_to_wchar(CertificateData.Name), URGB(180, 0, 0));
	}
	return ctx;
error:
	(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"OpenSSL Key Error : Невозможно создать Server CTX").show();
	return NULL;
}

bool SSL_SendData(SSL* ssl, const string& BinData)
{
	size_t nCurrentPos = 0;
	int err = 0;
	while (nCurrentPos < BinData.size())
	{
		err = SSL_write(ssl, &BinData[nCurrentPos], (int)BinData.size());
		if (err > 0)
		{
			nCurrentPos += err;
			continue;
		}
		const int nCode = SSL_get_error(ssl, err);
		if ((nCode != SSL_ERROR_WANT_READ) && (nCode != SSL_ERROR_WANT_WRITE))
			return false;
	}
	return true;
}

bool SSL_RecvData(SSL* ssl, const void* BinData, int Size)
{
	size_t nCurrentPos = 0;
	int err = 0;
	while (nCurrentPos < (size_t)Size)
	{
		err = SSL_read(ssl, &((char*)BinData)[nCurrentPos], Size);
		if (err > 0)
		{
			nCurrentPos += err;
			continue;
		}
		const int nCode = SSL_get_error(ssl, err);
		if ((nCode != SSL_ERROR_WANT_READ) && (nCode != SSL_ERROR_WANT_WRITE))
			return false;
	}
	return true;
}

bool SSL_RecvDataString(SSL* ssl, string& BinData, int Size)
{
	BinData.resize((size_t)Size);
	return SSL_RecvData(ssl, BinData.data(), Size);
}

bool SendHTTTPAnswerWithData(SSL* ssl, const string &CodeAnsw, const string &ContentType, const string& Data)
{
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << CodeAnsw << ENDL
		<< "Server: " << SERVER_NAME << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL
		<< "Content-Type: " << ContentType << ENDL;
	if (Data.size() > 0)
		HttpAnswer << "Content-Length: " << Data.size() << ENDL;
	HttpAnswer << "Connection: keep-alive" << ENDL
		<< "Cache-Control: no-cache, no-store, must-revalidate" << ENDL
		<< ENDL;
	if (SSL_SendData(ssl, HttpAnswer.str()) == false || SSL_SendData(ssl, Data) == false)
		return false;
	return true;
}

void Root(RequestParserStruct& Req, SSL* ssl)
{
	if (Req.http_request["Path"] == L"captcha.png") {
		string cpch;
		cpch.resize(2117);
		memcpy((char*)cpch.data(), Capcha, 2117);
		SendHTTTPAnswerWithData(ssl, "200 OK", "image/png", cpch);
		return;
	}

	string ResultLink = string(Req.http_request["Path"] == L"token" ? "https://oauth.vk.com/" : "https://api.vk.com/") + Req.http_request.get_utf8("OPath");
	string PostParam;
	bool FirstParam = true;
	for (auto& i : Req.http_post_param.paramsdata)
	{
		if (FirstParam == true)
			FirstParam = false;
		else
			PostParam += "&";
		PostParam += (i.get_key() + "=" + wchar_to_UTF8(i.get_value()));
	}
	string ansv;
#ifdef CAPCHA_METHOD_NAME
	if (wsearch(Req.http_request["Path"], CAPCHA_METHOD_NAME) != wstring::npos) {
		if (!Req.http_get_param.exist("captcha_sid") && !Req.http_post_param.exist("captcha_sid") && !Req.http_get_param.exist("captcha_key") && !Req.http_post_param.exist("captcha_key")) {
			ansv = "{\"error\": {\"error_code\": 14,\"error_msg\": \"Captcha needed\", \"captcha_sid\": \"1234567890\",\"captcha_img\": \"https://" + CertificateData.Name + "/captcha.png?sid=1234567890&s=1\"}}";
			SendHTTTPAnswerWithData(ssl, "200 OK", "application/json; charset=utf-8", ansv);
			return;
		}
		string ckey = Req.http_get_param.exist("captcha_key") ? Req.http_get_param.get_utf8("captcha_key") : Req.http_post_param.get_utf8("captcha_key");
		string csid = Req.http_get_param.exist("captcha_sid") ? Req.http_get_param.get_utf8("captcha_sid") : Req.http_post_param.get_utf8("captcha_sid");
		if (ckey == "fuck" && csid == "1234567890") {
			PrintMessage(L"Succ Capcha", URGB(0, 255, 0));
		}
		else {
			PrintMessage(L"Error Capcha " + UTF8_to_wchar(csid) + L":" + UTF8_to_wchar(ckey), URGB(255, 0, 0));
		}
	}
#endif
/*
	if (wsearch(Req.http_request["Path"], L"audio.get") != wstring::npos) {
		SendHTTTPAnswerWithData(ssl, "200 OK", "application/json; charset=utf-8", "{\"error\": {\"error_code\": 34, \"error_msg\":\"Deprecated\"}}");
		return;
	}
*/

	if (Req.http_post_param.paramsdata.size() > 0) {
		DoCurlPost(ResultLink, PostParam, Req.http_request.get_utf8("User-Agent"), ansv, true);
	}
	else {
		DoCurlGet(ResultLink, Req.http_request.get_utf8("User-Agent"), ansv, true);
	}
	
	PrintMessage(L"[" + Req.http_request["Path"] + L"] {");
	PrintMessage(L"Get-Запрос: " + Req.http_request["OPath"], URGB(200, 120, 255));
	if (Req.http_post_param.paramsdata.size() > 0) {
		PrintMessage(L"Post-Запрос: [", URGB(200, 255, 0));
		for (auto& i : Req.http_post_param.paramsdata) {
			PrintMessage(UTF8_to_wchar(i.get_key()) + L"=" + i.get_value());
		}
		PrintMessage(L"]");
	}
	PrintMessage(L"};");
	PrintMessage(L" ");


	wstring Path = L"VK_Reverser/" + Req.http_request["Path"] + L".dump.txt";

	bool flexist = false;
	flexist = PathFileExistsW(Path.c_str()) == TRUE;
	FlBin ffy;
	if (ffy.wopen(Path, OPENType::OPENAppend, true)) {
		if (!flexist) {
			ffy.BWrite(UTF8START, 1, strlen(UTF8START));
		}
		else {
			ffy.BWrite(ENDL, 1, strlen(ENDL));
			ffy.BWrite(ENDL, 1, strlen(ENDL));
		}
		ffy << L"[" + Req.http_request["Path"] + L"] {" << ENDL;
		ffy << L"Время: " + GetTimeLocal() << ENDL;
		ffy << L"User-Agent: " + Req.http_request["User-Agent"] << ENDL;
		ffy << L"Get-Запрос: " + Req.http_request["OPath"] << ENDL;
		if (Req.http_post_param.paramsdata.size() > 0) {
			ffy << L"Post-Запрос: [";
			for (auto& i : Req.http_post_param.paramsdata) {
				ffy << (UTF8_to_wchar(i.get_key()) + L"=" + i.get_value()) << L"; ";
			}
			ffy << L"]" << ENDL;
		}
		if (Req.http_request.paramsdata.size() > 0) {
			ffy << L"Headers: [";
			for (auto& i : Req.http_request.paramsdata) {
				ffy << (UTF8_to_wchar(i.get_key()) + L"=" + i.get_value()) << L"; ";
			}
			ffy << L"]" << ENDL;
		}
		ffy << ENDL;

		ffy << L"Ответ: " << ENDL;
		ffy << L"########################################" << ENDL;
		try {
			json dt = json::parse(ansv);
			ffy << dt.dump(4);
		}
		catch (json::exception e) {
			ffy << e.what();
		}

		ffy << ENDL;
		ffy << L"########################################" << ENDL;
		ffy << L"};";
	}

	SendHTTTPAnswerWithData(ssl, "200 OK", "application/json; charset=utf-8", ansv);
}

int AcceptSSL(SSL* ssl)
{
	int nErrCode = 0;
	do
	{
		int error = SSL_accept(ssl);
		if (error == 1)
			return 1;
		nErrCode = SSL_get_error(ssl, error);
	} while (nErrCode == SSL_ERROR_WANT_READ || nErrCode == SSL_ERROR_WANT_WRITE);
	return -1;
}

size_t multipart::parse(const string& body, size_t offset, const std::string& boundary)
{
	size_t to = body.find("\r\n\r\n", offset);
	if (to == string::npos)
		return to;

	size_t end = body.find(boundary, to + 4);
	if (end == string::npos)
		return end;

	string hdr = body.substr(offset, to - offset);
	std::regex e1("Content-Disposition: form-data; name=\"([^\\r\\n\\t\\f\\v]*)\"", std::regex_constants::ECMAScript);
	std::regex e2("Content-Disposition: form-data; name=\"([^\\r\\n\\t\\f\\v]*)\"; filename=\"([^\\r\\n\\t\\f\\v]*)\"", std::regex_constants::ECMAScript);
	std::smatch matches1;
	std::smatch matches2;
	int status = 0;
	if (std::regex_search(hdr, matches2, e2))
		status = 2;
	else if (std::regex_search(hdr, matches1, e1))
		status = 1;
	if (status == 0) {
		return string::npos;
	}

	multipart_data dt;
	to += 4;
	dt.data.resize(end - to);
	memcpy((char*)dt.data.data(), &body.data()[to], end - to);

	string name;
	if (status == 1) {
		name = matches1[1];
	}

	if (status == 2) {
		name = SHA3_512::sha3(dt.data);
		dt.filename = matches2[2];
	}
	paramsdata[name] = dt;

	return end;
}

int parse_post_url_encoded_multipart(SSL* ssl, const string& RequestHTTP, int content_length, multipart& payload) {
#ifdef SUPPORT_MULTIPART
	std::regex e("Content-Type: multipart/form-data; boundary=([^\\r\\n\\t\\f\\v]*)", std::regex_constants::ECMAScript);
	std::smatch matches;
	if (std::regex_search(RequestHTTP, matches, e)) {
		string boundary = matches[1];
		boundary = "--" + boundary;

		string subdata;
		subdata.resize(content_length);
		if (SSL_RecvData(ssl, subdata.data(), content_length) == false)
			return 3;

		bool isFirst = true;
		size_t offset = 0;

		while ((offset = subdata.find(boundary, offset)) != string::npos) {
			offset += boundary.length();
			if (isFirst) {
				isFirst = false;
				boundary = "\r\n" + boundary;
			}
			if (subdata[offset] == '-' && subdata[offset + 1] == '-')
				break;
			offset = payload.parse(subdata, offset + 2, boundary);
		}
		if (offset == string::npos || payload.count() <= 0) {
			return 3;
		}
		return 2;
	}
	return 0;
#else
	return 0;
#endif
}

void HTTPParse(const string& HttpHeader, RequestParserStruct& hdr, const string& Connection, const multipart& mpart, string& PostUrlEncoded)
{
	string HttpHdr = HttpHeader;
	hdr.multi_part = mpart;
	hdr.connection = Connection;
	size_t Pos = HttpHdr.find(" ");
	string MtStr = HttpHdr.substr(0, Pos);
	if (MtStr == "GET")
		hdr.method = HTTP_METHOD::HTTP_METHOD_GET;
	else if (MtStr == "POST")
		hdr.method = HTTP_METHOD::HTTP_METHOD_POST;
	else if (MtStr == "PUT")
		hdr.method = HTTP_METHOD::HTTP_METHOD_PUT;
	else if (MtStr == "DELETE")
		hdr.method = HTTP_METHOD::HTTP_METHOD_DELETE;
	else if (MtStr == "HEAD")
		hdr.method = HTTP_METHOD::HTTP_METHOD_HEAD;
	else if (MtStr == "CONNECT")
		hdr.method = HTTP_METHOD::HTTP_METHOD_CONNECT;
	else if (MtStr == "OPTIONS")
		hdr.method = HTTP_METHOD::HTTP_METHOD_OPTIONS;
	else if (MtStr == "TRACE")
		hdr.method = HTTP_METHOD::HTTP_METHOD_TRACE;
	else if (MtStr == "PATCH")
		hdr.method = HTTP_METHOD::HTTP_METHOD_PATCH;
	else
		hdr.method = HTTP_METHOD::HTTP_METHOD_UNKNOW;

	HttpHdr = HttpHdr.substr(Pos + 2);
	Pos = HttpHdr.find(" ");
	string GetParams = HttpHdr.substr(0, Pos);
	hdr.http_request.add_decode("OPath", GetParams);
	hdr.http_request.add_decode("Path", GetParams.substr(0, GetParams.find("?")));
	GetParams = GetParams.substr(GetParams.find("?") + 1);
	hdr.http_get_param.parse(GetParams, "&", true);
	HttpHdr = HttpHdr.substr(Pos + 6);
	Pos = HttpHdr.find("\r\n");
	hdr.http_request.add("Version", HttpHdr.substr(0, Pos));
	HttpHdr = HttpHdr.substr(Pos + 2);
	while ((Pos = HttpHdr.find(": ")) != string::npos)
	{
		hdr.http_request.add(HttpHdr.substr(0, Pos), HttpHdr.substr(Pos + 2, HttpHdr.find("\r\n") - (Pos + 2)));
		HttpHdr = HttpHdr.substr(HttpHdr.find("\r\n") + 2);
	}
	//HttpHdr = HttpHdr.substr(HttpHdr.find("\r\n") + 2);

	if (hdr.http_request.exist("Content-Encoding")) {
		if (hdr.http_request["Content-Encoding"] == L"gzip") {
			PostUrlEncoded = LIB_IMAGE::win_image::decompress_gzip(PostUrlEncoded);
			hdr.http_request.add("Content-Length", to_string(PostUrlEncoded.size()));
		}
	}
	hdr.http_post_param.parse(PostUrlEncoded, "&", true);
	if (hdr.http_request.exist("Cookie"))
	{
		wstring Cookiest = hdr.http_request["Cookie"];
		hdr.http_request.erase("Cookie");
		hdr.cookies.parse(wchar_to_UTF8(Cookiest), "; ", true);
	}
	if (search(hdr.http_request.get_utf8("User-Agent"), "iPad") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_IPAD;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "iPhone") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_IPHONE;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "Android") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_ANDROID;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "Windows Phone") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_WINDOWS_PHONE;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "Windows") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_WINDOWS;
	else
		hdr.Device = TARGET_DEVICES::DEVICE_ANDROID;
}

bool ReciveHTTPRequest(SSL* ssl, string& RequestHTTP, multipart& mpart, string& PostUrlEncoded)
{
	size_t nCurrentPos = 0;
	while (RequestHTTP.find("\r\n\r\n") == string::npos)
	{
		char Tmp = 0;
		if (SSL_RecvData(ssl, &Tmp, 1) == false)
			return false;
		RequestHTTP.push_back(Tmp);
	}
	bool IsGet = RequestHTTP.find("GET") != string::npos;
	bool IsPost = RequestHTTP.find("POST") != string::npos;
	if (IsGet == false && IsPost == false)
		return false;
	if (IsPost == true)
	{
		int PaySize = 0;
		size_t CntOf = RequestHTTP.find("Content-Length: ");
		if (CntOf != string::npos) {
			try {
				PaySize = stoi(RequestHTTP.substr(CntOf + 16, RequestHTTP.substr(CntOf + 16).find("\r\n")));
			}
			catch (std::exception e) {
				return false;
			}
		}
		if (PaySize > 0)
		{
			if (PaySize >= POST_BODY_MAX) {
				return false;
			}
			int status = parse_post_url_encoded_multipart(ssl, RequestHTTP, PaySize, mpart);
			if (status == 0)
			{
				PostUrlEncoded.resize(PaySize);
				if (SSL_RecvData(ssl, PostUrlEncoded.data(), PaySize) == false)
					return false;
			}
			else if (status == 3) {
				return false;
			}
		}
	}
	return true;
}

DWORD WINAPI HTTPS_ServerThread(LPVOID arg)
{
	ACCEPT_STRUCT* satr = (ACCEPT_STRUCT*)arg;
	SOCKET client_sock = satr->client_sock;
	SSL_CTX* ctx = (SSL_CTX*)satr->ssl_ctx;
	string connection = satr->connection;
	delete satr;
	SSL* ssl = SSL_new(ctx);
	if (ssl == NULL)
	{
		closesocket(client_sock);
		return 0;
	}
	BIO* soc = BIO_new_socket((int)client_sock, BIO_NOCLOSE);
	SSL_set_bio(ssl, soc, soc);
	RequestParserStruct Req;
	string HttpRequest;
	string http_post_url_encoded;
	multipart mpart;

	bool NeedRedirect = false;
	int sslacceptcode = AcceptSSL(ssl);
	if (sslacceptcode != 1)
	{
#ifdef PRINT_SSL_ACCEPT_ERROR
		PrintMessage(L"       |SSL_accept " + UTF8_to_wchar(connection) + L" вернул " + to_wstring(sslacceptcode) + L"; " + UTF8_to_wchar(ERR_error_string(ERR_get_error(), NULL)) + L"|", URGB(255, 0, 0));
#endif
		goto out;
}

	if (ReciveHTTPRequest(ssl, HttpRequest, mpart, http_post_url_encoded) == false)
		goto out;

	HTTPParse(HttpRequest, Req, connection, mpart, http_post_url_encoded);

	Root(Req, ssl);
	goto out;
out:
	SSL_shutdown(ssl);
	SSL_free(ssl);
	shutdown(client_sock, SD_BOTH);
	closesocket(client_sock);
	Sleep(1000);
	return 0;
}

bool ReciveNetBinary(SOCKET Socket, const void* Buffer, int Len, int Timeout)
{
	memset((void*)Buffer, 0, Len);
	DWORD timeout = Timeout * 1000;
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	int rsz = recv(Socket, (char*)Buffer, Len, 0);
	if (rsz != Len)
		return false;
	return true;
}

bool SendNetBinary(SOCKET Socket, const void* Buffer, int Len, int Timeout)
{
	DWORD timeout = Timeout * 1000;
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	int rsz = send(Socket, (char*)Buffer, Len, 0);
	if (rsz != (int)Len)
		return false;
	return true;
}

static string GetConnectionIP(SOCKADDR_IN& FromAddr)
{
	string buf;
	buf.resize(260);
	inet_ntop(AF_INET, &FromAddr.sin_addr, (char*)buf.data(), 260);
	buf = buf.c_str();
	return buf;
}

void InitVK_Reverser()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Ошибка инициализации сокетов").show();
		return;
	}

	SSL_CTX* ctx = initialize_ctx();
	if (ctx == NULL)
		return;
	HTTPSserver_sock = 0;
	SOCKET client_sock = 0;
	HTTPSserver_sock = tcp_listen(Startinit.HttpsPort);
	if (HTTPSserver_sock == SOCKET_ERROR)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой в инициализации сервера(socket bind)").show();
		return;
	}
	PrintMessage(L"[HTTPS инициализирован, порт " + to_wstring(Startinit.HttpsPort) + L"]", URGB(0, 140, 255));
	SOCKADDR_IN FromAddr;
	int len = sizeof(SOCKADDR_IN);
	while (true)
	{
		client_sock = accept(HTTPSserver_sock, (SOCKADDR*)&FromAddr, &len);
		if (client_sock <= 0)
			continue;
		SetTimeOutsSocket(client_sock);
		ACCEPT_STRUCT* satr = new ACCEPT_STRUCT;
		satr->connection = GetConnectionIP(FromAddr) + string(":") + to_string(ntohs(FromAddr.sin_port));
		satr->client_sock = client_sock;
		satr->ssl_ctx = ctx;
		CreateThreadDetachedSimple(&HTTPS_ServerThread, satr);
	}
	shutdown(HTTPSserver_sock, SD_BOTH);
	closesocket(HTTPSserver_sock);
	SSL_CTX_free(ctx);
}