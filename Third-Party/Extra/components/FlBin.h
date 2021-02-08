#pragma once
#include <iostream>
#include <Windows.h>
#include <Shlwapi.h>
#include "WSTRUtils.h"

enum class OPENType
{
	OPENRead,
	OPENWrite,
	OPENAppend
};

class FlBin
{
public:
	FlBin()
	{
		Fl = NULL;
		Mode = OPENType::OPENRead;
		Error = false;
	}
	bool wopen(const std::wstring &File, OPENType OMode, bool CreateDirs = false)
	{
		Close();
		Mode = OMode;
		Error = false;
		if (CreateDirs == true && (OMode == OPENType::OPENWrite || OMode == OPENType::OPENAppend))
			CreateSubDirsW(File);
		const wchar_t* KMode = L"";
		if (OMode == OPENType::OPENWrite)
			KMode = L"wb";
		else if (OMode == OPENType::OPENRead)
			KMode = L"rb";
		else if (OMode == OPENType::OPENAppend)
			KMode = L"ab";
		if (_wfopen_s(&Fl, File.c_str(), KMode) != 0)
			return false;
		return true;
	}
	bool open(const std::string &File, OPENType OMode, bool CreateDirs = false)
	{
		Close();
		Mode = OMode;
		Error = false;
		if (CreateDirs == true && (OMode == OPENType::OPENWrite || OMode == OPENType::OPENAppend))
			CreateSubDirsA(File);
		const char* KMode = "";
		if (OMode == OPENType::OPENWrite)
			KMode = "wb";
		else if (OMode == OPENType::OPENRead)
			KMode = "rb";
		else if (OMode == OPENType::OPENAppend)
			KMode = "ab";
		if (fopen_s(&Fl, File.c_str(), KMode) != 0)
			return false;
		return true;
	}
	int GetPos()
	{
		if (Fl == NULL || Error == true)
			return -1;
		return ftell(Fl);
	}
	bool SetPos(int Pos, int Seek)
	{
		if (Fl == NULL || Error == true || (Mode == OPENType::OPENWrite || Mode == OPENType::OPENAppend))
			return false;
		return fseek(Fl, Pos, Seek) == 0 ? true : false;
	}
	template<typename T>
	FlBin& operator<<(T value)
	{
		if (Fl == NULL || Error == true || Mode == OPENType::OPENRead)
		{
			TError();
			return *this;
		}
		if (fwrite(&value, sizeof(T), 1, Fl) <= 0)
			TError();
		return *this;
	}
	FlBin& operator<<(const std::string &value)
	{
		if (value.size() <= 0) {
			return *this;
		}
		if (Fl == NULL || Error == true || Mode == OPENType::OPENRead)
		{
			TError();
			return *this;
		}
		if (fwrite(value.c_str(), 1, value.length(), Fl) <= 0)
			TError();
		return *this;
	}

	FlBin& operator<<(const char* value)
	{
		std::string vl = value;
		if (vl.size() <= 0) {
			return *this;
		}
		if (Fl == NULL || Error == true || Mode == OPENType::OPENRead)
		{
			TError();
			return *this;
		}
		if (fwrite(vl.c_str(), 1, vl.length(), Fl) <= 0)
			TError();
		return *this;
	}

	FlBin& operator<<(const std::wstring &value)
	{
		return operator<<(WSTRUtils::wchar_to_UTF8(value));
	}

	FlBin& operator<<(const wchar_t* value)
	{
		return operator<<(WSTRUtils::wchar_to_UTF8(value));
	}
	
	template<typename T>
	FlBin& operator>>(T& value)
	{
		if (Fl == NULL || Error == true || (Mode == OPENType::OPENWrite || Mode == OPENType::OPENAppend) || (size_t)(GetSize() - (int)ftell(Fl)) < sizeof(T))
		{
			TError();
			return *this;
		}
		if (fread(&value, sizeof(T), 1, Fl) <= 0)
			TError();
		return *this;
	}
	void BRead(const void* Buf, size_t ElemSize, size_t Count)
	{
		if (Fl == NULL || Error == true || (Mode == OPENType::OPENWrite || Mode == OPENType::OPENAppend) || (size_t)(GetSize() - (int)ftell(Fl)) < ElemSize * Count)
		{
			TError();
			return;
		}
		if (fread((void*)Buf, ElemSize, Count, Fl) < Count)
			TError();
	}
	void BWrite(const void* Buf, size_t ElemSize, size_t Count)
	{
		if (Fl == NULL || Error == true || Mode == OPENType::OPENRead)
		{
			TError();
			return;
		}
		if (fwrite(Buf, ElemSize, Count, Fl) < Count)
			TError();
	}
	int GetSize()
	{
		if (Fl == NULL || Error == true)
			return -1;
		int Sz = 0;
		if (Mode == OPENType::OPENRead)
		{
			int cur = ftell(Fl);
			fseek(Fl, 0, SEEK_END);
			Sz = ftell(Fl);
			fseek(Fl, cur, SEEK_SET);
		}
		else
			Sz = ftell(Fl);
		return Sz;
	}
	void Close()
	{
		if (Fl)
		{
			fclose(Fl);
			Fl = NULL;
		}
	}
	~FlBin()
	{
		Close();
	}
	bool IsError()
	{
		return Error;
	}
private:
	void TError()
	{
		if (Error == false)
			Error = true;
		Close();
	}
	void CreateSubDirsW(const std::wstring &pth)
	{
		std::wstring String = pth;
		size_t pos = 0;
		std::wstring U = L"";
		while (String.length() > 0 && (pos = String.find_first_of(L"\\/")) != std::wstring::npos)
		{
			U += std::wstring((U.length() > 0 ? L"\\" : L"")) + String.substr(0, pos);
			if (pos == std::wstring::npos)
				String.clear();
			else
				String = String.substr(pos + 1);
			if (PathFileExistsW(U.c_str()) == FALSE)
				CreateDirectoryW(U.c_str(), NULL);
		}
	}
	void CreateSubDirsA(const std::string &pth)
	{
		std::string String = pth;
		size_t pos = 0;
		std::string U = "";
		while (String.length() > 0 && (pos = String.find_first_of("\\/")) != std::string::npos)
		{
			U += std::string((U.length() > 0 ? "\\" : "")) + String.substr(0, pos);
			if (pos == std::string::npos)
				String.clear();
			else
				String = String.substr(pos + 1);
			if (PathFileExistsA(U.c_str()) == FALSE)
				CreateDirectoryA(U.c_str(), NULL);
		}
	}
	FILE* Fl;
	bool Error;
	OPENType Mode;
};