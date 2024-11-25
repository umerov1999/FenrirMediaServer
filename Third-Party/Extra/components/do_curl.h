﻿#pragma once
#include <iostream>
#include <cstdio>
#include <cstring>

int DoCurlCapcha(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, bool IsJSON);
int DoCurlPost(const std::string& Link, const std::string& PostParams, const std::string& UserAgent, std::string& ReciveData, bool IsJSON);
int DoCurlGet(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, bool IsJSON);
int DoCurlGet(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, bool IsJSON, int wait);
int DoCurlGetWithContentType(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, std::string& Content_Type, bool IsJSON, int wait);
int DoCurlGetWithContentType(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, std::string& Content_Type, bool IsJSON);
int DoCurlDownload(const std::string &Link, const std::string &UserAgent, std::string& ReciveData, bool IsJSON);
int DoCurlPostJsonAuth(const std::string& Link, const std::string& PostJson, const std::string& UserAgent, const std::string& login, const std::string& password, std::string& ReciveData);
int DoCurlMultipart(const std::string& Link, const std::wstring& filePath, const std::string& partName, const std::string& fileName, const std::string& UserAgent, std::string& ReciveData, bool IsJSON);
int DoCurlGetAndReturnUrl(const std::string& Link, const std::string& UserAgent, std::string& ReciveData, int wait);
int DoCurlPostJsonAuthCustomRequest(const std::string& Link, const std::string& PostJson, const std::string& UserAgent, const std::string& login, const std::string& password, std::string& ReciveData, const std::string& request = "PATCH");
