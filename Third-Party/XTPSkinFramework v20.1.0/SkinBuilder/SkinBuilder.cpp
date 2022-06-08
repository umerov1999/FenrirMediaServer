#include "pch.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <Windows.h>
#include "Lz4Pack.h"

using namespace std;
using namespace Lz4Pack;

void SkinAddDirectory(Compressor& Pack, wstring FileDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	wstring OO = L"*";
	if (FileDirectory.length() > 0)
		OO = FileDirectory + L"\\*";
	hf = FindFirstFileW(OO.c_str(), &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do {
			if (wstring(FindFileData.cFileName) == L"." || wstring(FindFileData.cFileName) == L"..")
				continue;

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (FileDirectory.length() > 0)
					SkinAddDirectory(Pack, FileDirectory + L"\\" + FindFileData.cFileName);
				else
					SkinAddDirectory(Pack, FindFileData.cFileName);
			}
			else
			{
				if (FileDirectory.length() > 0)
				{
					FileWraper* wrp = NULL;
					CreateFileWraperFromFile(wrp, FileDirectory + L"\\" + FindFileData.cFileName, L"rb");
					size_t PSz = (size_t)wrp->fsize();
					if (PSz <= 0) {
						wcout << L"Empty File " << FileDirectory + L"\\" + FindFileData.cFileName << endl;
						CloseFileWraper(wrp);
						continue;
					}
					wcout << FileDirectory + L"\\" + FindFileData.cFileName << endl;
					vector<char> Data(PSz);
					wrp->fread(Data.data(), 1, PSz);
					CloseFileWraper(wrp);
					if (!Pack.add(FileDirectory + L"\\" + FindFileData.cFileName, Data)) {
						cout << "Fatal Error!!!" << endl;
						system("pause");
						_exit(-1);
					}
				}
				else
				{
					wcout << FindFileData.cFileName << endl;
					FileWraper* wrp = NULL;
					CreateFileWraperFromFile(wrp, FindFileData.cFileName, L"rb");
					size_t PSz = (size_t)wrp->fsize();
					if (PSz <= 0) {
						wcout << L"Empty File " << FindFileData.cFileName << endl;
						CloseFileWraper(wrp);
						continue;
					}
					vector<char> Data(PSz);
					wrp->fread(Data.data(), 1, PSz);
					CloseFileWraper(wrp);
					if (!Pack.add(FindFileData.cFileName, Data)) {
						cout << "Fatal Error!!!" << endl;
						system("pause");
						_exit(-1);
					}
				}
			}
		} while (FindNextFileW(hf, &FindFileData) != 0);
		FindClose(hf);
	}
}

std::wstring RemovePath(std::wstring fname)
{
	const wchar_t* FnS = L"\\/";
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(pos + 1);
}

std::wstring RemoveFileInPath(std::wstring fname)
{
	const wchar_t* FnS = L"\\/";
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(0, pos);
}

std::wstring RemoveExtInPath(std::wstring fname)
{
	const wchar_t* FnS = L".";
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(0, pos);
}

void GenerateStyle(wstring Directory)
{
	wstring CurDir = RemoveFileInPath(Directory);
	SetCurrentDirectoryW(CurDir.c_str());

	wstring TT = RemovePath(Directory) + L".cjstyles";
	Compressor c;
	SetCurrentDirectoryW(Directory.c_str());
	SkinAddDirectory(c, L"");
	SetCurrentDirectoryW(CurDir.c_str());
	if (!c.build(TT)) {
		cout << "Fatal Error!!!" << endl;
		system("pause");
		_exit(-1);
	}
}

void UngenerateStyle(wstring File)
{
	wstring CurDir = RemoveFileInPath(File);
	SetCurrentDirectoryW(CurDir.c_str());
	FileWraper*wrp = NULL;
	CreateFileWraperFromFile(wrp, File, L"rb");
	size_t PSz = (size_t)wrp->fsize();
	vector<char> Data(PSz);
	wrp->fread(Data.data(), 1, PSz);
	CloseFileWraper(wrp);

	Decompressor d(Data.data(), Data.size(), false);
	if (!d.read_headers()) {
		cout << "Fatal Error!!!" << endl;
		system("pause");
		_exit(-1);
	}

	wstring MainDir = RemovePath(RemoveExtInPath(File));
	CreateDirectoryW(MainDir.c_str(), NULL);
	for (auto &i : d.Dirs)
		CreateDirectoryW((MainDir + L"\\" + i).c_str(), NULL);
	for (auto &i : d.Files)
	{
		string Data = d[i.get_value()];
		if (Data.empty()) {
			cout << "Fatal Error!!!" << endl;
			system("pause");
			_exit(-1);
		}
		CreateFileWraperFromFile(wrp, (MainDir + L"\\" + i.get_key()), L"wb");
		wrp->fwrite(Data.data(), 1, Data.size());
		CloseFileWraper(wrp);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	system("chcp 1251");
	setlocale(0, "Rus");
	if (argc < 2)
		return -1;
	for (int i = 1; i < argc; i++)
	{
		if (GetFileAttributesW(argv[i]) & FILE_ATTRIBUTE_DIRECTORY)
			GenerateStyle(argv[i]);
		else
			UngenerateStyle(argv[i]);
	}
	cout << "End." << endl;
	system("pause");
	return 0;
}