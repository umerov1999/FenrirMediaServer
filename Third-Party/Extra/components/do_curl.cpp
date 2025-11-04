#include <iostream>
#if defined(__linux__)
#include <chrono>
#include <thread>
#endif
#include "curl/curl.h"
#include "MimeTypes.hpp"
#include "do_curl.h"
using namespace std;

class CURL_WR
{
public:
	CURL_WR()
	{
		Data = NULL;
		WritePos = 0;
		Size = -1;
		curl_handle = NULL;
		ContentHasSize = false;
	}
	void reset()
	{
		Data->clear();
		ContentHasSize = false;
		Size = -1;
		WritePos = 0;
	}
	std::string* Data;
	int WritePos;
	int Size;
	bool ContentHasSize;
	void* curl_handle;
};

static size_t CurlWriter(char* Buf, size_t ElementSize, size_t ElementCount, CURL_WR* ReciveData)
{
	if (ReciveData == NULL)
		return 0;
	if (!ReciveData->ContentHasSize)
	{
		curl_off_t sized = -1;
		curl_easy_getinfo((CURL*)ReciveData->curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &sized);
		if (sized > 0)
		{
			ReciveData->Data->resize((int)sized);
			ReciveData->Size = (int)sized;
		}
		ReciveData->ContentHasSize = true;
	}
	if (ReciveData->Size < (ReciveData->WritePos + (int)(ElementSize * ElementCount)))
		ReciveData->Data->resize(ReciveData->WritePos + (int)(ElementSize * ElementCount));
	memcpy(((char*)ReciveData->Data->data()) + ReciveData->WritePos, Buf, (int)ElementSize * ElementCount);
	ReciveData->WritePos += (int)(ElementSize * ElementCount);
	return ElementSize * ElementCount;
}

static size_t CurlFileReader(char* Buf, size_t ElementSize, size_t ElementCount, void* pointer)
{
	if (pointer == NULL)
		return 0;
	auto delat = fread(Buf, ElementSize, ElementCount, (FILE*)pointer);
	return delat;
}

static int seek_callback(void* arg, curl_off_t offset, int origin)
{
	auto result = fseek((FILE*)arg, (long)offset, origin);
	return result ? CURL_SEEKFUNC_OK : CURL_SEEKFUNC_FAIL;
}

int DoCurlDownload(const string &Link, const string &UserAgent, string& ReciveData, bool IsJSON, int wait)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		long long Code = 0;
		const char* PContentType = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &PContentType);
		string ContentType;
		if (PContentType != NULL)
			ContentType = PContentType;
		curl_easy_cleanup(curl_handle);
		if (!IsJSON && ContentType.find("text/html") != string::npos)
			ReciveData.clear();
		else
		{
			if (res == CURLE_OK && (IsJSON == true || Code == 200))
				return wrt.WritePos;
			else
				ReciveData.clear();
		}
	}
	return -1;
}

int DoCurlCapcha(const string& Link, const string& UserAgent, string& ReciveData, bool IsJSON, int wait)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		if (res == CURLE_OK && (IsJSON == true || Code == 200))
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

int DoCurlGet(const string& Link, const string& UserAgent, string& ReciveData, bool IsJSON, int wait, const list<string>& HeaderParams)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		if (!HeaderParams.empty()) {
			struct curl_slist* headers = NULL;
			for (auto& i : HeaderParams) {
				headers = curl_slist_append(headers, i.c_str());
			}
			curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
		}
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		if (res == CURLE_OK && (IsJSON == true || Code == 200))
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

int DoCurlGetWithContentType(const string& Link, const string& UserAgent, string& ReciveData, string& Content_Type, bool IsJSON, int wait)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		long long Code = 0;
		const char* ContentType = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ContentType);
		if (ContentType != NULL)
			Content_Type = ContentType;
		curl_easy_cleanup(curl_handle);
		if (res == CURLE_OK && (IsJSON == true || Code == 200))
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

int DoCurlPost(const string& Link, const string& PostParams, const string& UserAgent, string& ReciveData, bool IsJSON, int wait, const list<string>& HeaderParams)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		if (!HeaderParams.empty()) {
			struct curl_slist* headers = NULL;
			for (auto& i : HeaderParams) {
				headers = curl_slist_append(headers, i.c_str());
			}
			curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
		}
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, PostParams.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		if (res == CURLE_OK && (IsJSON == true || Code == 200))
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

int DoCurlPostJsonAuth(const string& Link, const string& PostJson, const string& UserAgent, const string& login, const string& password, string& ReciveData, int wait)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		struct curl_slist* headers = NULL;
		if (!PostJson.empty()) {
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "Content-Type:application/json");
			headers = curl_slist_append(headers, "charsets: utf-8");
			curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
		}
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

		if (!login.empty()) {
			curl_easy_setopt(curl_handle, CURLOPT_USERPWD, (login + ":" + password).c_str());
		}
		
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		if (!PostJson.empty()) {
			curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, PostJson.c_str());
		}
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		curl_easy_cleanup(curl_handle);
		if (headers != NULL) {
			curl_slist_free_all(headers);
		}
		if (res == CURLE_OK)
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

int DoCurlMultipartAuth(const std::string& Link, const std::wstring& filePath, const std::string& partName, const std::string& fileName, const std::string& UserAgent, const string& login, const string& password, std::string& ReciveData, bool IsJSON, int wait) {
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	FILE* fl = NULL;
	if (curl_handle)
	{
		if (_wfopen_s(&fl, filePath.c_str(), L"rb") != 0) {
			curl_easy_cleanup(curl_handle);
			return -1;
		}
		if (!fl) {
			curl_easy_cleanup(curl_handle);
			return -1;
		}
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

		if (!login.empty()) {
			curl_easy_setopt(curl_handle, CURLOPT_USERPWD, (login + ":" + password).c_str());
		}

		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

		_fseeki64(fl, 0, SEEK_END);
		long long sz = _ftelli64(fl);
		_fseeki64(fl, 0, SEEK_SET);

		curl_mime* multipart;
		curl_mimepart* part;

		multipart = curl_mime_init(curl_handle);
		part = curl_mime_addpart(multipart);

		curl_mime_name(part, partName.c_str());
		curl_mime_filename(part, fileName.c_str());
		try {
			curl_mime_type(part, MimeType::MimeTypes::getType(fileName).c_str());
		}
		catch (...) {
			curl_mime_type(part, "*/*");
		}
		curl_mime_data_cb(part, sz, CurlFileReader, seek_callback, NULL, fl);
		curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, multipart);

		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

		CURLcode res = curl_easy_perform(curl_handle);
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		curl_mime_free(multipart);
		curl_slist_free_all(headers);
		fclose(fl);
		if (res == CURLE_OK && (IsJSON == true || Code == 200))
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

int DoCurlMultipart(const std::string& Link, const std::wstring& filePath, const std::string& partName, const std::string& fileName, const std::string& UserAgent, std::string& ReciveData, bool IsJSON, int wait) {
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	FILE* fl = NULL;
	if (curl_handle)
	{
		if (_wfopen_s(&fl, filePath.c_str(), L"rb") != 0) {
			curl_easy_cleanup(curl_handle);
			return -1;
		}
		if (!fl) {
			curl_easy_cleanup(curl_handle);
			return -1;
		}
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

		_fseeki64(fl, 0, SEEK_END);
		long long sz = _ftelli64(fl);
		_fseeki64(fl, 0, SEEK_SET);

		curl_mime* multipart;
		curl_mimepart* part;

		multipart = curl_mime_init(curl_handle);
		part = curl_mime_addpart(multipart);

		curl_mime_name(part, partName.c_str());
		curl_mime_filename(part, fileName.c_str());
		curl_mime_type(part, "*/*");
		curl_mime_data_cb(part, sz, CurlFileReader, seek_callback, NULL, fl);
		curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, multipart);

		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "X-VK-Android-Client: new");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

		CURLcode res = curl_easy_perform(curl_handle);
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		curl_mime_free(multipart);
		curl_slist_free_all(headers);
		fclose(fl);
		if (res == CURLE_OK && (IsJSON == true || Code == 200))
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}

//0 ok, 1 false, 2 error
int DoCurlGetAndReturnUrl(const string& Link, const string& UserAgent, string& ReciveData, int wait)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		if (res == CURLE_OK) {
			if (Code == 200) {
				char* url_string{ nullptr };
				curl_easy_getinfo(curl_handle, CURLINFO_EFFECTIVE_URL, &url_string);
				ReciveData = url_string;
				curl_easy_cleanup(curl_handle);
				return 0;
			}
			else {
				curl_easy_cleanup(curl_handle);
				return 1;
			}
		}
		else {
			curl_easy_cleanup(curl_handle);
			ReciveData.clear();
		}
	}
	return 2;
}

int DoCurlPostJsonAuthCustomRequest(const string& Link, const string& PostJson, const string& UserAgent, const string& login, const string& password, string& ReciveData, const string &request, int wait)
{
	ReciveData.clear();
	CURL_WR wrt;
	wrt.Data = &ReciveData;
	CURL* curl_handle = curl_easy_init();
	wrt.curl_handle = curl_handle;
	if (curl_handle)
	{
		struct curl_slist* headers = NULL;
		if (!PostJson.empty()) {
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "Content-Type:application/json");
			headers = curl_slist_append(headers, "charsets: utf-8");
			curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
		}
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_setopt(curl_handle, CURLOPT_USERPWD, (login + ":" + password).c_str());

		curl_easy_setopt(curl_handle, CURLOPT_URL, Link.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &wrt);
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
		if (!PostJson.empty()) {
			curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, request.c_str());
			curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, PostJson.c_str());
		}
		curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, wait);
		curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 120000L);
		CURLcode res = curl_easy_perform(curl_handle);
		int retry = 0;
		while (res != CURLE_OK && retry < 3) {
			wrt.reset();
#if defined(__linux__)
			this_thread::sleep_for(chrono::milliseconds(2000));
#else
			Sleep(2000);
#endif
			res = curl_easy_perform(curl_handle);
			retry++;
		}
		long long Code = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &Code);
		curl_easy_cleanup(curl_handle);
		if (headers != NULL) {
			curl_slist_free_all(headers);
		}
		if (res == CURLE_OK)
			return wrt.WritePos;
		else
			ReciveData.clear();
	}
	return -1;
}
