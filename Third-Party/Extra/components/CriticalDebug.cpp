#include <iostream>
#include <string>
#include <Windows.h>
#pragma warning( push )
#pragma warning( disable : 4091)
#include "WinMessageBox.h"
#include <Dbghelp.h>
#include "CriticalDebug.h"
#pragma warning( pop ) 
using namespace std;
using namespace WSTRUtils;

static bool need_restart = false;

static std::wstring GetAppFullPath()
{
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileNameW(0, szFileName, MAX_PATH);
	return szFileName;
}

static std::wstring AppName()
{
	const wchar_t* FnS = L"\\/";

	std::wstring fname = GetAppFullPath();
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(pos + 1);
}

static std::wstring AppPath()
{
	const wchar_t* FnS = L"\\/";

	std::wstring fname = GetAppFullPath();
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(0, pos);
}

typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
static void create_minidump(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
	wstring Dump = AppPath() + L"/" + AppName() + L" " + GetTimeLocal() + L".DMP";
	HMODULE mhLib = ::LoadLibraryW(L"dbghelp.dll");
	if(mhLib == NULL)
		::ExitProcess(-1);
	MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(mhLib, "MiniDumpWriteDump");
	if(pDump == NULL)
		::ExitProcess(-1);
	HANDLE hFile = ::CreateFileW(Dump.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == NULL)
		::ExitProcess(-1);

	_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
	ExInfo.ThreadId = ::GetCurrentThreadId();
	ExInfo.ExceptionPointers = apExceptionInfo;
	ExInfo.ClientPointers = FALSE;

	pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo, NULL, NULL);
	::CloseHandle(hFile);
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH, szDir);
	wstring ResultExc;
	switch (apExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			ResultExc = L"Попытка чтения/записи области памяти, без соответствующих разрешений.";
			break;
		case EXCEPTION_BREAKPOINT:
			ResultExc = L"Достигнута точка останова.";
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			ResultExc = L"Попытка выполнения невалидной инструкции.";
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			ResultExc = L"Попытка обращения к отсутствующей странице, если её невозможно загрузить.";
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			ResultExc = L"Попытка целочисленного деления на ноль.";
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			ResultExc = L"Попытка возобновления выполнения потока, после \"не продолжаемого\" исключения.";
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			ResultExc = L"Попытка выполнения привилегированной инструкции.";
			break;
		case EXCEPTION_SINGLE_STEP:
			ResultExc = L"В процессе трассировки была выполнена одна инструкция.";
			break;
		case EXCEPTION_STACK_OVERFLOW:
			ResultExc = L"Потоком исчерпан стек.";
			break;
	}
	if (need_restart) {
		if ((win_message().timeout(20).message_type(MSG_TYPE::TYPE_ERROR).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Необрабатываемое ИСКЛЮЧЕНИЕ " << ResultExc << L"!!! Дамп сгенерирован в \"" << wstring(szDir) << L"/" << Dump << L"\".\r\n Отменить ПЕРЕЗАПУСК программы?").show() == IDYES)
			::ExitProcess(-1);
		::ShellExecuteW(NULL, L"open", GetAppFullPath().c_str(), L"--autostart", szDir, SW_NORMAL);
	}
	else {
		(win_message().timeout(20).message_type(MSG_TYPE::TYPE_ERROR) << L"Необрабатываемое ИСКЛЮЧЕНИЕ " << ResultExc << L"!!! Дамп сгенерирован в \"" << wstring(szDir) << L"/" << Dump << L"\".").show();
	}
	::ExitProcess(-1);
}

static LONG WINAPI unhandled_handler(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
	create_minidump(apExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

void INIT_CRITICAL_DEBUGGER(bool NeedRestart)
{
	need_restart = NeedRestart;
	SetUnhandledExceptionFilter(unhandled_handler);
}