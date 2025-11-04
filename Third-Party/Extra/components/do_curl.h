#pragma once
#include <iostream>
#include <list>
#include <cstdio>
#include <cstring>

int DoCurlCapcha(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, bool IsJSON, int wait = 2);
int DoCurlPost(const std::string& Link, const std::string& PostParams, const std::string& UserAgent, std::string& ReciveData, bool IsJSON, int wait = 20, const std::list<std::string>& HeaderParams = std::list<std::string>());
int DoCurlGet(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, bool IsJSON, int wait = 20, const std::list<std::string>& HeaderParams = std::list<std::string>());
int DoCurlGetWithContentType(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, std::string& Content_Type, bool IsJSON, int wait = 20);
int DoCurlDownload(const std::string &Link, const std::string &UserAgent, std::string& ReciveData, bool IsJSON, int wait = 20);
int DoCurlPostJsonAuth(const std::string& Link, const std::string& PostJson, const std::string& UserAgent, const std::string& login, const std::string& password, std::string& ReciveData, int wait = 20);
int DoCurlMultipartAuth(const std::string& Link, const std::wstring& filePath, const std::string& partName, const std::string& fileName, const std::string& UserAgent, const std::string& login, const std::string& password, std::string& ReciveData, bool IsJSON, int wait = 90);
int DoCurlMultipart(const std::string& Link, const std::wstring& filePath, const std::string& partName, const std::string& fileName, const std::string& UserAgent, std::string& ReciveData, bool IsJSON, int wait = 90);
int DoCurlGetAndReturnUrl(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, int wait = 20);
int DoCurlPostJsonAuthCustomRequest(const std::string& Link, const std::string& PostJson, const std::string& UserAgent, const std::string& login, const std::string& password, std::string& ReciveData, const std::string& request = "PATCH", int wait = 20);
